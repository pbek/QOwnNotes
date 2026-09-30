/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "interfacesettingswidget.h"

#include <utils/gui.h>
#include <utils/misc.h>

#include <QRegularExpression>
#include <QStyleFactory>
#include <QToolBar>
#include <QTreeWidget>

#include "services/settingsservice.h"
#include "ui_interfacesettingswidget.h"

InterfaceSettingsWidget::InterfaceSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::InterfaceSettingsWidget) {
    ui->setupUi(this);

#ifdef Q_OS_WIN
    // Populate the font engine combo box in the constructor, because readSettings()
    // and storeSettings() are called before the lazy initialize() of this page
    ui->fontEngineComboBox->addItem(tr("Automatic (follow Windows font smoothing)"),
                                    QStringLiteral("auto"));
    ui->fontEngineComboBox->addItem(tr("DirectWrite"), QStringLiteral("directwrite"));
    ui->fontEngineComboBox->addItem(tr("GDI (legacy)"), QStringLiteral("gdi"));
#endif
}

InterfaceSettingsWidget::~InterfaceSettingsWidget() { delete ui; }

/**
 * Initializes the interface settings widget
 */
void InterfaceSettingsWidget::initialize() {
    // Set the translation help label URL
    ui->helpTranslateLabel->setText(ui->helpTranslateLabel->text().arg(
        QStringLiteral("https://www.qownnotes.org/contributing/translation.html")));

#ifdef Q_OS_MAC
    // There is no system tray on macOS, rename to menu bar
    ui->systemTrayGroupBox->setTitle(tr("Menu bar"));
    ui->showSystemTrayCheckBox->setText(tr("Show menu bar item"));
#endif
#ifndef Q_OS_WIN
    ui->fontRenderingGroupBox->hide();
#else
    connect(ui->fontEngineComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(needRestart()));
#endif

    // Connect needRestart signals for widgets that require a restart
    connect(ui->languageListWidget, SIGNAL(itemSelectionChanged()), this, SIGNAL(needRestart()));
    connect(ui->hideIconsInMenusCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(needRestart()));
    connect(ui->showSystemTrayCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(needRestart()));
    connect(ui->startHiddenCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(needRestart()));
}

/**
 * Reads the interface settings from the settings service
 */
void InterfaceSettingsWidget::readSettings() {
    SettingsService settings;

#ifdef Q_OS_WIN
    const QSignalBlocker fontEngineBlocker(ui->fontEngineComboBox);
    Q_UNUSED(fontEngineBlocker)
    const int fontEngineIndex = ui->fontEngineComboBox->findData(
        settings.value(QStringLiteral("interfaceFontEngine"), QStringLiteral("auto")).toString());
    ui->fontEngineComboBox->setCurrentIndex(fontEngineIndex >= 0 ? fontEngineIndex : 0);
#endif

    {
        const QSignalBlocker b1(ui->overrideInterfaceFontSizeGroupBox);
        const QSignalBlocker b2(ui->interfaceFontSizeSpinBox);
        Q_UNUSED(b1)
        Q_UNUSED(b2)
        ui->overrideInterfaceFontSizeGroupBox->setChecked(
            settings.value(QStringLiteral("overrideInterfaceFontSize"), false).toBool());
        ui->interfaceFontSizeSpinBox->setValue(
            settings.value(QStringLiteral("interfaceFontSize"), 11).toInt());
    }

    {
        const QSignalBlocker b3(ui->overrideInterfaceScalingFactorGroupBox);
        const QSignalBlocker b4(ui->interfaceScalingFactorSpinBox);
        Q_UNUSED(b3)
        Q_UNUSED(b4)
        ui->overrideInterfaceScalingFactorGroupBox->setChecked(
            settings.value(QStringLiteral("overrideInterfaceScalingFactor"), false).toBool());
        ui->interfaceScalingFactorSpinBox->setValue(
            settings.value(QStringLiteral("interfaceScalingFactor"), 100).toInt());
    }

    ui->toolbarIconSizeSpinBox->setValue(
        settings.value(QStringLiteral("MainWindow/mainToolBar.iconSize")).toInt());

    // Determine default item height from a temporary tree widget
    QTreeWidget treeWidget(this);
    auto *treeWidgetItem = new QTreeWidgetItem();
    treeWidget.addTopLevelItem(treeWidgetItem);
    int height = treeWidget.visualItemRect(treeWidgetItem).height();
    ui->itemHeightSpinBox->setValue(settings.value(QStringLiteral("itemHeight"), height).toInt());

    // Select the saved interface language in the list widget
    const QString savedLanguage = settings.value(QStringLiteral("interfaceLanguage")).toString();
    for (int i = 0; i < ui->languageListWidget->count(); ++i) {
        QListWidgetItem *item = ui->languageListWidget->item(i);
        if (item->whatsThis() == savedLanguage) {
            ui->languageListWidget->setCurrentItem(item);
            break;
        }
    }

    ui->hideIconsInMenusCheckBox->setChecked(Utils::Misc::areMenuIconsHidden());

    ui->showStatusBarNotePathCheckBox->setChecked(
        settings.value(QStringLiteral("showStatusBarNotePath"), true).toBool());
    ui->showStatusBarRelativeNotePathCheckBox->setChecked(
        settings.value(QStringLiteral("showStatusBarRelativeNotePath")).toBool());
    ui->showStatusBarRelativeNotePathCheckBox->setEnabled(
        ui->showStatusBarNotePathCheckBox->isChecked());

    ui->hideStatusBarInDistractionFreeModeCheckBox->setChecked(
        settings.value(QStringLiteral("DistractionFreeMode/hideStatusBar")).toBool());
    ui->openDistractionFreeModeInFullScreenCheckBox->setChecked(
        settings.value(QStringLiteral("DistractionFreeMode/openInFullScreen"), true).toBool());

    {
        const QSignalBlocker b5(ui->showSystemTrayCheckBox);
        Q_UNUSED(b5)
        bool showSystemTray = settings.value(QStringLiteral("ShowSystemTray")).toBool();
        ui->showSystemTrayCheckBox->setChecked(showSystemTray);
        ui->startHiddenCheckBox->setEnabled(showSystemTray);
        ui->startHiddenCheckBox->setChecked(settings.value(QStringLiteral("StartHidden")).toBool());
        if (!showSystemTray) {
            ui->startHiddenCheckBox->setChecked(false);
        }
    }

    loadInterfaceStyleComboBox();
}

/**
 * Stores the interface settings to the settings service
 */
void InterfaceSettingsWidget::storeSettings() {
    SettingsService settings;

#ifdef Q_OS_WIN
    settings.setValue(QStringLiteral("interfaceFontEngine"), ui->fontEngineComboBox->currentData());
#endif

    settings.setValue(QStringLiteral("overrideInterfaceFontSize"),
                      ui->overrideInterfaceFontSizeGroupBox->isChecked());
    settings.setValue(QStringLiteral("interfaceFontSize"), ui->interfaceFontSizeSpinBox->value());
    settings.setValue(QStringLiteral("overrideInterfaceScalingFactor"),
                      ui->overrideInterfaceScalingFactorGroupBox->isChecked());
    settings.setValue(QStringLiteral("interfaceScalingFactor"),
                      ui->interfaceScalingFactorSpinBox->value());
    settings.setValue(QStringLiteral("itemHeight"), ui->itemHeightSpinBox->value());
    settings.setValue(QStringLiteral("MainWindow/mainToolBar.iconSize"),
                      ui->toolbarIconSizeSpinBox->value());

    // Store selected interface language (stored in whatsThis role)
    QString interfaceLanguage;
    QListWidgetItem *currentItem = ui->languageListWidget->currentItem();
    if (currentItem != nullptr) {
        interfaceLanguage = currentItem->whatsThis();
    }
    settings.setValue(QStringLiteral("interfaceLanguage"), interfaceLanguage);

    settings.setValue(QStringLiteral("hideIconsInMenus"),
                      ui->hideIconsInMenusCheckBox->isChecked());
    settings.setValue(QStringLiteral("showStatusBarNotePath"),
                      ui->showStatusBarNotePathCheckBox->isChecked());
    settings.setValue(QStringLiteral("showStatusBarRelativeNotePath"),
                      ui->showStatusBarRelativeNotePathCheckBox->isChecked());
    settings.setValue(QStringLiteral("DistractionFreeMode/hideStatusBar"),
                      ui->hideStatusBarInDistractionFreeModeCheckBox->isChecked());
    settings.setValue(QStringLiteral("DistractionFreeMode/openInFullScreen"),
                      ui->openDistractionFreeModeInFullScreenCheckBox->isChecked());

    // Store the interface style settings
    if (ui->interfaceStyleComboBox->currentIndex() > 0) {
        settings.setValue(QStringLiteral("interfaceStyle"),
                          ui->interfaceStyleComboBox->currentText());
    } else {
        settings.remove(QStringLiteral("interfaceStyle"));
    }

    settings.setValue(QStringLiteral("ShowSystemTray"), ui->showSystemTrayCheckBox->isChecked());
    settings.setValue(QStringLiteral("StartHidden"), ui->startHiddenCheckBox->isChecked());
}

/**
 * Updates the search icon in the language search line edit to match dark/light mode
 */
void InterfaceSettingsWidget::updateSearchIcons() {
    SettingsService settings;
    bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();

    const QString searchIconFileName =
        darkMode ? QStringLiteral("search-notes-dark.svg") : QStringLiteral("search-notes.svg");
    static const QRegularExpression searchIconRegex(
        QStringLiteral("background-image: url\\(:.+\\);"));
    const QString searchIconStyle =
        QStringLiteral("background-image: url(:/images/%1);").arg(searchIconFileName);

    QString styleSheet = ui->languageSearchLineEdit->styleSheet();
    styleSheet.replace(searchIconRegex, searchIconStyle);
    ui->languageSearchLineEdit->setStyleSheet(styleSheet);
}

/**
 * Loads the settings for the interface style combo box
 */
void InterfaceSettingsWidget::loadInterfaceStyleComboBox() {
    const QSignalBlocker blocker(ui->interfaceStyleComboBox);
    Q_UNUSED(blocker)

    ui->interfaceStyleComboBox->clear();
    ui->interfaceStyleComboBox->addItem(tr("Automatic (needs restart)"));

    Q_FOREACH (const QString &style, QStyleFactory::keys()) {
        ui->interfaceStyleComboBox->addItem(style);
    }

    SettingsService settings;
    const QString interfaceStyle = settings.value(QStringLiteral("interfaceStyle")).toString();

    if (!interfaceStyle.isEmpty()) {
        ui->interfaceStyleComboBox->setCurrentText(interfaceStyle);
    } else {
        ui->interfaceStyleComboBox->setCurrentIndex(0);
    }

    Utils::Gui::applyInterfaceStyle();
}

void InterfaceSettingsWidget::on_interfaceStyleComboBox_currentTextChanged(const QString &arg1) {
    Utils::Gui::applyInterfaceStyle(arg1);

    // If the interface style was set to automatic we need a restart
    if (ui->interfaceStyleComboBox->currentIndex() == 0) {
        emit needRestart();
    }
}

/**
 * Also enable the single instance feature if the system tray icon is turned on
 */
void InterfaceSettingsWidget::on_showSystemTrayCheckBox_toggled(bool checked) {
    // We don't need to do that on macOS
#ifndef Q_OS_MAC
    if (checked) {
        emit systemTrayToggled(true);
    }
#endif

    ui->startHiddenCheckBox->setEnabled(checked);

    if (!checked) {
        ui->startHiddenCheckBox->setChecked(false);
    }
}

void InterfaceSettingsWidget::on_interfaceFontSizeSpinBox_valueChanged(int arg1) {
    SettingsService settings;
    settings.setValue(QStringLiteral("interfaceFontSize"), arg1);
    Utils::Gui::updateInterfaceFontSize(arg1);
}

void InterfaceSettingsWidget::on_overrideInterfaceFontSizeGroupBox_toggled(bool arg1) {
    SettingsService settings;
    settings.setValue(QStringLiteral("overrideInterfaceFontSize"), arg1);
    Utils::Gui::updateInterfaceFontSize();
}

void InterfaceSettingsWidget::on_languageSearchLineEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInListWidget(ui->languageListWidget, arg1, true);
}

void InterfaceSettingsWidget::on_showStatusBarNotePathCheckBox_toggled(bool checked) {
    ui->showStatusBarRelativeNotePathCheckBox->setEnabled(checked);
}

void InterfaceSettingsWidget::on_overrideInterfaceScalingFactorGroupBox_toggled(bool arg1) {
    if (!arg1) {
        Utils::Gui::information(
            this, tr("Override interface scaling factor"),
            tr("If you had this setting enabled, you now need to restart the application manually "
               "so the previous environment variable that overrides the scale factor is not in "
               "your environment again."),
            QStringLiteral("settings-override-interface-scale-factor-off"));
    } else {
        emit needRestart();
    }
}

void InterfaceSettingsWidget::on_interfaceScalingFactorSpinBox_valueChanged(int arg1) {
    Q_UNUSED(arg1);
    emit needRestart();
}

/**
 * Resets the item height spinbox to the default tree widget item height
 */
void InterfaceSettingsWidget::on_itemHeightResetButton_clicked() {
    QTreeWidget treeWidget(this);
    auto *treeWidgetItem = new QTreeWidgetItem();
    treeWidget.addTopLevelItem(treeWidgetItem);
    int height = treeWidget.visualItemRect(treeWidgetItem).height();
    ui->itemHeightSpinBox->setValue(height);
}

/**
 * Resets the toolbar icon size spinbox to the default toolbar icon size
 */
void InterfaceSettingsWidget::on_toolbarIconSizeResetButton_clicked() {
    QToolBar toolbar(this);
    ui->toolbarIconSizeSpinBox->setValue(toolbar.iconSize().height());
}
