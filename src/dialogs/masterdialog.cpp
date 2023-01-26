#include "masterdialog.h"

#include <QApplication>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QScreen>
#include <QSettings>

#include "services/metricsservice.h"

MasterDialog::MasterDialog(QWidget *parent) : QDialog(parent) { installEventFilter(this); }

void MasterDialog::closeEvent(QCloseEvent *event) {
    //    storeGeometrySettings();
    QDialog::closeEvent(event);
}

void MasterDialog::resizeEvent(QResizeEvent *event) {
    // save the geometry of the dialog
    storeGeometrySettings();

    QDialog::resizeEvent(event);
}

/**
 * Stores the geometry of the dialog
 */
void MasterDialog::storeGeometrySettings() const {
    QSettings settings;
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
    QSettings settings;
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

void MasterDialog::keyPressEvent(QKeyEvent *keyEvent) {
    if (_ignoreReturnKey &&
        (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)) {
        return;
    }

    QDialog::keyPressEvent(keyEvent);
}
