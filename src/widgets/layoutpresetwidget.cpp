#include "layoutpresetwidget.h"

#include <utils/gui.h>
#include <utils/misc.h>

#include <QDebug>
#include <QtWidgets/QMessageBox>

#include "services/settingsservice.h"
#include "ui_layoutpresetwidget.h"

LayoutPresetWidget::LayoutPresetWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::LayoutPresetWidget) {
    ui->setupUi(this);
    _manualSettingsStoring = true;
    loadLayoutPresets();
}

LayoutPresetWidget::~LayoutPresetWidget() { delete ui; }

void LayoutPresetWidget::loadLayoutPresets() {
    _layoutPresetSettings =
        new QSettings(QStringLiteral(":/configurations/layouts.ini"), QSettings::IniFormat);
    const auto layoutPresetIdentifiers =
        _layoutPresetSettings->value(QStringLiteral("LayoutPresetIdentifiers")).toStringList();

    {
        const QSignalBlocker blocker(ui->layoutPresetComboBox);
        Q_UNUSED(blocker)
        ui->layoutPresetComboBox->clear();

        for (const QString &layoutPresetIdentifier : layoutPresetIdentifiers) {
            ui->layoutPresetComboBox->addItem(getLayoutPresetName(layoutPresetIdentifier),
                                              layoutPresetIdentifier);
        }
    }

    ui->layoutPresetComboBox->setCurrentIndex(0);
    on_layoutPresetComboBox_currentIndexChanged(0);
}

void LayoutPresetWidget::on_layoutPresetComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index);
    updateCurrentLayoutPreset();
}

void LayoutPresetWidget::updateCurrentLayoutPreset() {
    const QString layoutPresetIdentifier = ui->layoutPresetComboBox->currentData().toString();
    const QString layoutPresetSettingsPrefix =
        QStringLiteral("LayoutPreset-") + layoutPresetIdentifier + QStringLiteral("/");
    const QString screenshot =
        _layoutPresetSettings->value(layoutPresetSettingsPrefix + QStringLiteral("screenshot"))
            .toString();
    QString layoutPresetDescription = getLayoutPresetDescription(layoutPresetIdentifier);

    ui->layoutPresetDescriptionLabel->setText(layoutPresetDescription);

    auto scene = new QGraphicsScene();

    // Adapt the preview background to the current palette.
    QColor bg = ui->layoutPresetGraphicsView->palette().window().color();
    ui->layoutPresetGraphicsView->setStyleSheet(QStringLiteral("background-color:") +
                                                bg.name(QColor::HexArgb));

    QString filePath(":/images/layouts/" + screenshot);

    scene->addPixmap(QPixmap(filePath));
    ui->layoutPresetGraphicsView->setScene(scene);
    ui->layoutPresetGraphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // if (!_manualSettingsStoring) {
    //    storeLayoutPreset();
    //}
}

void LayoutPresetWidget::storeLayoutPreset() {
    if (_manualSettingsStoring) {
        const QString title = tr("Use new layout preset");
        const QString text = tr("Do you want to use the selected layout preset?");

        if (Utils::Gui::question(this, title, text, "layoutpresetwidget-use-layout-preset",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No) == QMessageBox::No) {
            return;
        }
    }

    const QString layoutPresetIdentifier = ui->layoutPresetComboBox->currentData().toString();
    const QString layoutPresetSettingsPrefix =
        QStringLiteral("LayoutPreset-") + layoutPresetIdentifier + QStringLiteral("/");
    SettingsService settings;
    QStringList layouts = settings.value(QStringLiteral("layouts")).toStringList();
    const QString layoutUuid =
        _manualSettingsStoring ? Utils::Misc::generateRandomString(12) : QStringLiteral("initial");

    if (!layouts.contains(layoutUuid)) {
        layouts << layoutUuid;
        settings.setValue(QStringLiteral("layouts"), layouts);
    }

    settings.setValue(QStringLiteral("initialLayoutPresetIdentifier"), layoutPresetIdentifier);
    QString centralWidget =
        _layoutPresetSettings->value(layoutPresetSettingsPrefix + QStringLiteral("centralWidget"))
            .toString();
    if (centralWidget.isEmpty()) {
        centralWidget =
            _layoutPresetSettings
                    ->value(layoutPresetSettingsPrefix + QStringLiteral("noteEditIsCentralWidget"))
                    .toBool()
                ? QStringLiteral("note-edit")
                : QStringLiteral("none");
    }

    // Only set the global central widget during initial setup (welcome dialog).
    // When switching layouts manually, restoreCurrentLayout() will set it
    // for the correct layout.
    if (!_manualSettingsStoring) {
        settings.setValue(QStringLiteral("centralWidget"), centralWidget);
        settings.setValue(QStringLiteral("noteEditIsCentralWidget"),
                          centralWidget == QLatin1String("note-edit"));
    }

    settings.setValue(QStringLiteral("layout-") + layoutUuid + QStringLiteral("/centralWidget"),
                      centralWidget);
    settings.setValue(
        QStringLiteral("layout-") + layoutUuid + QStringLiteral("/noteEditIsCentralWidget"),
        centralWidget == QLatin1String("note-edit"));
    settings.setValue(
        QStringLiteral("layout-") + layoutUuid + QStringLiteral("/windowState"),
        _layoutPresetSettings->value(layoutPresetSettingsPrefix + QStringLiteral("windowState")));
    settings.setValue(QStringLiteral("layout-") + layoutUuid + QStringLiteral("/name"),
                      getLayoutPresetName(layoutPresetIdentifier));
    settings.setValue(
        QStringLiteral("layout-") + layoutUuid + QStringLiteral("/noteSubFolderDockWidgetVisible"),
        _layoutPresetSettings->value(layoutPresetSettingsPrefix +
                                     QStringLiteral("noteSubFolderDockWidgetVisible")));

    // Since a new layout is installed we later want to center and resize the
    // window
    settings.setValue(QStringLiteral("initialLayout"), true);

    emit layoutStored(layoutUuid);
}

void LayoutPresetWidget::setManualSettingsStoring(bool enabled) {
    _manualSettingsStoring = enabled;
    ui->useLayoutPresetPushButton->setVisible(enabled);

    if (!enabled) {
        updateCurrentLayoutPreset();
    }
}

QString LayoutPresetWidget::getLayoutPresetName(const QString &layoutPresetIdentifier) {
    if (layoutPresetIdentifier == QLatin1String("minimal")) {
        return tr("Minimal", "Layout preset name");
    } else if (layoutPresetIdentifier == QLatin1String("full")) {
        return tr("Full", "Layout preset name");
    } else if (layoutPresetIdentifier == QLatin1String("preview-only")) {
        return tr("Preview only", "Layout preset name");
    } else if (layoutPresetIdentifier == QLatin1String("full-vertical")) {
        return tr("Full vertical", "Layout preset name");
    } else if (layoutPresetIdentifier == QLatin1String("1col")) {
        return tr("Single column", "Layout preset name");
    }

    return QString();
}

QString LayoutPresetWidget::getLayoutPresetDescription(const QString &layoutPresetIdentifier) {
    const QString &centralWidgetAddText =
        " " + tr("The note edit panel is the central widget that will be "
                 "resized automatically.",
                 "Layout preset description");

    const QString &noCentralWidgetAddText =
        " " + tr("Because of this there is no central widget that will be "
                 "resized automatically.",
                 "Layout preset description");

    const QString &previewCentralWidgetAddText =
        " " + tr("The note preview panel is the central widget that will be "
                 "resized automatically.",
                 "Layout preset description");

    if (layoutPresetIdentifier == QLatin1String("minimal")) {
        return tr("Just the note list on the left and the note edit panel "
                  "on the right are enabled by default.",
                  "Layout preset description") +
               centralWidgetAddText;
    } else if (layoutPresetIdentifier == QLatin1String("full")) {
        return tr("Most of the panels, like the note list on the left, the "
                  "tagging panels, the note edit panel in the center and the "
                  "preview panel on the right are enabled by default.",
                  "Layout preset description") +
               centralWidgetAddText;
    } else if (layoutPresetIdentifier == QLatin1String("preview-only")) {
        return tr("Most of the panels, like the note list on the left, the "
                  "tagging panels, and only the preview panel on the right "
                  "are enabled by default. You will need another layout to "
                  "actually edit notes!",
                  "Layout preset description") +
               previewCentralWidgetAddText;
    } else if (layoutPresetIdentifier == QLatin1String("full-vertical")) {
        return tr("Most of the panels, like the note list on the left, the "
                  "tagging panels, the note edit panel on the right and the "
                  "preview panel on top of the note edit panel are enabled by "
                  "default.",
                  "Layout preset description") +
               noCentralWidgetAddText;
    } else if (layoutPresetIdentifier == QLatin1String("1col")) {
        return tr("Tiny one column layout with note search, note list and note "
                  "edit on top of each other.",
                  "Layout preset description") +
               centralWidgetAddText;
    }

    return QString();
}

void LayoutPresetWidget::resizeEvent(QResizeEvent *event) {
    resizeLayoutPresetImage();
    QWidget::resizeEvent(event);
}

void LayoutPresetWidget::resizeLayoutPresetImage() const {
    if (ui->layoutPresetGraphicsView->scene() != nullptr) {
        ui->layoutPresetGraphicsView->fitInView(ui->layoutPresetGraphicsView->scene()->sceneRect(),
                                                Qt::KeepAspectRatio);
    }
}

void LayoutPresetWidget::on_useLayoutPresetPushButton_clicked() { storeLayoutPreset(); }
