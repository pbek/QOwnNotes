#include "layoutwidget.h"

#include <utils/gui.h>
#include <utils/misc.h>

#include <QDebug>
#include <QSettings>
#include <QtWidgets/QMessageBox>

#include "ui_layoutwidget.h"

LayoutWidget::LayoutWidget(QWidget *parent) : QWidget(parent), ui(new Ui::LayoutWidget) {
    ui->setupUi(this);
    _manualSettingsStoring = true;
    loadLayouts();
}

LayoutWidget::~LayoutWidget() { delete ui; }

void LayoutWidget::loadLayouts() {
    _layoutSettings =
        new QSettings(QStringLiteral(":/configurations/layouts.ini"), QSettings::IniFormat);
    auto layoutIdentifiers =
        _layoutSettings->value(QStringLiteral("LayoutIdentifiers")).toStringList();

    {
        const QSignalBlocker blocker(ui->layoutComboBox);
        Q_UNUSED(blocker)
        ui->layoutComboBox->clear();

        for (int i = 0; i < layoutIdentifiers.count(); i++) {
            const QString &layoutIdentifier = layoutIdentifiers.at(i);
            const QString &layoutName = getLayoutName(layoutIdentifier);
            ui->layoutComboBox->addItem(layoutName, layoutIdentifier);
        }
    }

    ui->layoutComboBox->setCurrentIndex(0);
    on_layoutComboBox_currentIndexChanged(0);
}

void LayoutWidget::on_layoutComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index);
    updateCurrentLayout();
}

void LayoutWidget::updateCurrentLayout() {
    QString layoutIdentifier = ui->layoutComboBox->currentData().toString();
    QString layoutSettingsPrefix = "Layout-" + layoutIdentifier + "/";
    QString screenshot = _layoutSettings->value(layoutSettingsPrefix + "screenshot").toString();
    QString layoutDescription = getLayoutDescription(layoutIdentifier);

    if (_manualSettingsStoring) {
        layoutDescription +=
            "\n\n" + tr("Keep in mind that workspaces that "
                        "demand that there is no central widget will not work properly if "
                        "the central widget is enabled.");
    }

    ui->layoutDescriptionLabel->setText(layoutDescription);

    auto scene = new QGraphicsScene();

    // adapt layoutGraphicsView background color
    QColor bg = ui->layoutGraphicsView->palette().window().color();
    ui->layoutGraphicsView->setStyleSheet(QStringLiteral("background-color:") +
                                          bg.name(QColor::HexArgb));

    QString filePath(":/images/layouts/" + screenshot);

    scene->addPixmap(QPixmap(filePath));
    ui->layoutGraphicsView->setScene(scene);
    ui->layoutGraphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // if (!_manualSettingsStoring) {
    //    storeSettings();
    //}
}

void LayoutWidget::storeSettings() {
    if (_manualSettingsStoring) {
        QString title = tr("Use new layout");
        QString text = tr("Do you want to use the selected layout?") + "\n\n" +
                       tr("The application will be restarted afterwards.") +
                       Utils::Misc::appendSingleAppInstanceTextIfNeeded();

        if (Utils::Gui::question(this, title, text, "layoutwidget-use-layout",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No) == QMessageBox::No) {
            return;
        }
    }

    QString layoutIdentifier = ui->layoutComboBox->currentData().toString();
    QString layoutSettingsPrefix = "Layout-" + layoutIdentifier + "/";
    QSettings settings;
    QStringList workspaces = settings.value(QStringLiteral("workspaces")).toStringList();
    QString workspaceIdentifier =
        _manualSettingsStoring ? Utils::Misc::generateRandomString(12) : "initial";

    if (!workspaces.contains(workspaceIdentifier)) {
        workspaces << workspaceIdentifier;
        settings.setValue(QStringLiteral("workspaces"), workspaces);
    }

    settings.setValue(QStringLiteral("initialLayoutIdentifier"), layoutIdentifier);
    settings.setValue(QStringLiteral("currentWorkspace"), workspaceIdentifier);
    settings.setValue(QStringLiteral("noteEditIsCentralWidget"),
                      _layoutSettings->value(layoutSettingsPrefix + "noteEditIsCentralWidget"));
    settings.setValue("workspace-" + workspaceIdentifier + "/windowState",
                      _layoutSettings->value(layoutSettingsPrefix + "windowState"));
    settings.setValue("workspace-" + workspaceIdentifier + "/name",
                      getLayoutName(layoutIdentifier));
    settings.setValue(
        "workspace-" + workspaceIdentifier + "/noteSubFolderDockWidgetVisible",
        _layoutSettings->value(layoutSettingsPrefix + "noteSubFolderDockWidgetVisible"));

    // since a new layout is installed we later want to center and resize the
    // window
    settings.setValue(QStringLiteral("initialWorkspace"), true);

    emit settingsStored();

    if (_manualSettingsStoring) {
        // make sure no settings get written after quitting
        qApp->setProperty("clearAppDataAndExit", true);

        Utils::Misc::restartApplication();
    }
}

void LayoutWidget::setManualSettingsStoring(bool enabled) {
    _manualSettingsStoring = enabled;
    ui->useLayoutPushButton->setVisible(enabled);

    if (!enabled) {
        updateCurrentLayout();
    }
}

QString LayoutWidget::getLayoutName(const QString &layoutIdentifier) {
    if (layoutIdentifier == QLatin1String("minimal")) {
        return tr("Minimal", "Layout name");
    } else if (layoutIdentifier == QLatin1String("full")) {
        return tr("Full", "Layout name");
    } else if (layoutIdentifier == QLatin1String("preview-only")) {
        return tr("Preview only", "Layout name");
    } else if (layoutIdentifier == QLatin1String("full-vertical")) {
        return tr("Full vertical", "Layout name");
    } else if (layoutIdentifier == QLatin1String("1col")) {
        return tr("Single column", "Layout name");
    }

    return QString();
}

QString LayoutWidget::getLayoutDescription(const QString &layoutIdentifier) {
    const QString &centralWidgetAddText =
        " " + tr("The note edit panel is the central widget that will be "
                 "resized automatically.",
                 "Layout description");

    const QString &noCentralWidgetAddText =
        " " + tr("Because of this there is no central widget that will be "
                 "resized automatically.",
                 "Layout description");

    if (layoutIdentifier == QLatin1String("minimal")) {
        return tr("Just the note list on the left and the note edit panel "
                  "on the right are enabled by default.",
                  "Layout description") +
               centralWidgetAddText;
    } else if (layoutIdentifier == QLatin1String("full")) {
        return tr("Most of the panels, like the note list on the left, the "
                  "tagging panels, the note edit panel in the center and the "
                  "preview panel on the right are enabled by default.",
                  "Layout description") +
               centralWidgetAddText;
    } else if (layoutIdentifier == QLatin1String("preview-only")) {
        return tr("Most of the panels, like the note list on the left, the "
                  "tagging panels, and only the preview panel on the right "
                  "are enabled by default. You will need another workspace to "
                  "actually edit notes!",
                  "Layout description") +
               noCentralWidgetAddText;
    } else if (layoutIdentifier == QLatin1String("full-vertical")) {
        return tr("Most of the panels, like the note list on the left, the "
                  "tagging panels, the note edit panel on the right and the "
                  "preview panel on top of the note edit panel are enabled by "
                  "default.",
                  "Layout description") +
               noCentralWidgetAddText;
    } else if (layoutIdentifier == QLatin1String("1col")) {
        return tr("Tiny one column layout with note search, note list and note "
                  "edit on top of each other.",
                  "Layout description") +
               centralWidgetAddText;
    }

    return QString();
}

void LayoutWidget::resizeEvent(QResizeEvent *event) {
    resizeLayoutImage();
    QWidget::resizeEvent(event);
}

void LayoutWidget::resizeLayoutImage() const {
    if (ui->layoutGraphicsView->scene() != nullptr) {
        ui->layoutGraphicsView->fitInView(ui->layoutGraphicsView->scene()->sceneRect(),
                                          Qt::KeepAspectRatio);
    }
}

void LayoutWidget::on_useLayoutPushButton_clicked() { storeSettings(); }
