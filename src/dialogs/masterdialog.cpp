#include "masterdialog.h"

#include <QApplication>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QScreen>

#include "services/metricsservice.h"
#include "services/settingsservice.h"
#include "utils/gui.h"
#include "widgets/loadingoverlaywidget.h"

MasterDialog::MasterDialog(QWidget *parent) : QDialog(parent) { installEventFilter(this); }

void MasterDialog::closeEvent(QCloseEvent *event) {
    //    storeGeometrySettings();
    QDialog::closeEvent(event);
}

void MasterDialog::afterSetupUI() { Utils::Gui::fixDarkModeIcons(this); }

void MasterDialog::resizeEvent(QResizeEvent *event) {
    // save the geometry of the dialog
    storeGeometrySettings();

    QDialog::resizeEvent(event);
}

/**
 * Stores the geometry of the dialog
 */
void MasterDialog::storeGeometrySettings() const {
    SettingsService settings;
    settings.setValue(getGeometrySettingKey(), saveGeometry());
}

bool MasterDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Move) {
        // save the geometry of the dialog
        storeGeometrySettings();
    }

    return QDialog::eventFilter(obj, event);
}

/**
 * Returns the geometry settings key
 *
 * @return settings key
 */
const QString MasterDialog::getGeometrySettingKey() const { return objectName() + "/geometry"; }

void MasterDialog::show() {
    handleOpenDialog();
    return QDialog::show();
}

void MasterDialog::open() {
    handleOpenDialog();
    return QDialog::open();
}

int MasterDialog::exec() {
    handleOpenDialog();
    return QDialog::exec();
}

/**
 * Handles dialog opening
 */
void MasterDialog::handleOpenDialog() {
    // restore the geometry of the dialog
    SettingsService settings;
    QByteArray geometryData = settings.value(getGeometrySettingKey()).toByteArray();

    // restore the geometry if there is some data
    if (geometryData.length() > 0) {
        restoreGeometry(geometryData);
    } else {
        const QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();

        // maximize the dialog window if it looks like that it doesn't fit on
        // the current screen
        if (((window()->width() + 150) > screenGeometry.width()) ||
            ((window()->height() + 150) > screenGeometry.height())) {
            setWindowState(windowState() ^ Qt::WindowMaximized);
        }
    }

    // send metrics for all dialogs but the MainWindow
    if (objectName() != QStringLiteral("MainWindow")) {
        MetricsService::instance()->sendVisitIfEnabled("dialog/" + objectName());
    }
}

void MasterDialog::setIgnoreReturnKey(bool ignore) { _ignoreReturnKey = ignore; }

/**
 * Covers the dialog with a loading overlay
 *
 * Canceling calls the cancel callback if there is one, otherwise the dialog is rejected
 */
void MasterDialog::showLoadingOverlay(const QString &title, const QString &text,
                                      const std::function<void()> &cancelCallback) {
    if (_loadingOverlay == nullptr) {
        _loadingOverlay = new LoadingOverlayWidget(this);
        connect(_loadingOverlay, &LoadingOverlayWidget::cancelRequested, this, [this]() {
            if (_loadingCancelCallback) {
                _loadingCancelCallback();
            } else {
                reject();
            }
        });
    }

    _loadingCancelCallback = cancelCallback;

    if (!_loading) {
        _minimumSizeBeforeLoading = minimumSize();
        _sizeBeforeLoading = size();
    }

    _loading = true;
    _loadingOverlay->start(title, text);

    // Make sure small dialogs have enough room for the loading card
    setMinimumSize(_minimumSizeBeforeLoading.expandedTo(_loadingOverlay->minimumSizeHint()));
}

void MasterDialog::hideLoadingOverlay() {
    if (!_loading) {
        return;
    }

    _loading = false;
    _loadingCancelCallback = nullptr;
    _loadingOverlay->stop();

    // Shrink dialogs back that were only enlarged for the loading card
    const QSize loadingMinimumSize = minimumSize();
    const QSize currentSize = size();
    setMinimumSize(_minimumSizeBeforeLoading);
    resize(currentSize.width() == loadingMinimumSize.width() ? _sizeBeforeLoading.width()
                                                             : currentSize.width(),
           currentSize.height() == loadingMinimumSize.height() ? _sizeBeforeLoading.height()
                                                               : currentSize.height());
}

void MasterDialog::setLoadingProgress(qint64 bytesReceived, qint64 bytesTotal) {
    if (_loadingOverlay != nullptr) {
        _loadingOverlay->setProgress(bytesReceived, bytesTotal);
    }
}

void MasterDialog::setLoadingCountProgress(int value, int maximum, const QString &statusText) {
    if (_loadingOverlay != nullptr) {
        _loadingOverlay->setCountProgress(value, maximum, statusText);
    }
}

bool MasterDialog::isLoading() const { return _loading; }

void MasterDialog::reject() {
    // Also stop the loading if the dialog is closed while loading
    if (_loading && _loadingCancelCallback) {
        _loadingCancelCallback();
    }

    QDialog::reject();
}

void MasterDialog::keyPressEvent(QKeyEvent *keyEvent) {
    if (_ignoreReturnKey &&
        (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)) {
        return;
    }

    QDialog::keyPressEvent(keyEvent);
}
