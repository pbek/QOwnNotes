/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

#include "layoutssettingswidget.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTimer>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "managers/layoutmanager.h"
#include "services/settingsservice.h"
#include "utils/gui.h"
#include "widgets/layoutpresetwidget.h"

LayoutsSettingsWidget::LayoutsSettingsWidget(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto *managementGroup = new QGroupBox(tr("Layouts"), this);
    auto *managementLayout = new QVBoxLayout(managementGroup);
    auto *descriptionLabel =
        new QLabel(tr("Create, rename, remove, and order layouts. Changes on this page are "
                      "applied immediately."),
                   managementGroup);
    descriptionLabel->setWordWrap(true);
    managementLayout->addWidget(descriptionLabel);

    auto *listLayout = new QHBoxLayout();
    _layoutListWidget = new QListWidget(managementGroup);
    _layoutListWidget->setAlternatingRowColors(true);
    _layoutListWidget->setMinimumHeight(180);
    listLayout->addWidget(_layoutListWidget, 1);

    auto *buttonLayout = new QVBoxLayout();
    auto *addButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-add")),
                                      tr("Add from current"), managementGroup);
    addButton->setToolTip(
        tr("Creates a new layout by copying the current panel and toolbar arrangement, panel "
           "visibility, and central-widget setting. The new layout becomes active immediately."));
    auto *addFromPresetButton =
        new QPushButton(QIcon::fromTheme(QStringLiteral("window-duplicate")),
                        tr("Add from preset..."), managementGroup);
    addFromPresetButton->setToolTip(tr("Opens the layout preset selector in a new dialog"));
    _switchButton = new QPushButton(QIcon::fromTheme(QStringLiteral("dialog-ok-apply")),
                                    tr("Switch to selected"), managementGroup);
    _switchButton->setToolTip(tr("Switches to the selected layout"));
    _removeButton = new QPushButton(QIcon::fromTheme(QStringLiteral("list-remove")), tr("Remove"),
                                    managementGroup);
    _renameButton = new QPushButton(QIcon::fromTheme(QStringLiteral("document-edit")), tr("Rename"),
                                    managementGroup);
    _moveUpButton =
        new QPushButton(QIcon::fromTheme(QStringLiteral("go-up")), tr("Move up"), managementGroup);
    _moveDownButton = new QPushButton(QIcon::fromTheme(QStringLiteral("go-down")), tr("Move down"),
                                      managementGroup);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(addFromPresetButton);
    buttonLayout->addWidget(_switchButton);
    buttonLayout->addWidget(_removeButton);
    buttonLayout->addWidget(_renameButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(_moveUpButton);
    buttonLayout->addWidget(_moveDownButton);
    listLayout->addLayout(buttonLayout);
    managementLayout->addLayout(listLayout);

    auto *selectedLayoutGroup = new QGroupBox(tr("Selected layout"), managementGroup);
    auto *selectedLayout = new QHBoxLayout(selectedLayoutGroup);
    selectedLayout->addWidget(new QLabel(tr("Central widget:"), selectedLayoutGroup));
    _centralWidgetComboBox = new QComboBox(selectedLayoutGroup);
    _centralWidgetComboBox->addItem(tr("Note edit"), QStringLiteral("note-edit"));
    _centralWidgetComboBox->addItem(tr("Note preview"), QStringLiteral("note-preview"));
    _centralWidgetComboBox->addItem(tr("No central widget"), QStringLiteral("none"));
    _centralWidgetComboBox->setToolTip(
        tr("The central widget automatically uses the space left by the surrounding panels"));
    selectedLayout->addWidget(_centralWidgetComboBox, 1);
    managementLayout->addWidget(selectedLayoutGroup);
    mainLayout->addWidget(managementGroup);

    mainLayout->addStretch();

    connect(_layoutListWidget, &QListWidget::currentRowChanged, this,
            [this]() { updateSelectedLayout(); });
    connect(_layoutListWidget, &QListWidget::itemChanged, this,
            [this](QListWidgetItem *item) { updateCurrentLayout(item); });
    connect(_layoutListWidget, &QListWidget::itemDoubleClicked, this, [this]() { renameLayout(); });
    connect(addButton, &QPushButton::clicked, this, [this]() { addLayout(); });
    connect(addFromPresetButton, &QPushButton::clicked, this, [this]() { addLayoutFromPreset(); });
    connect(_switchButton, &QPushButton::clicked, this, [this]() { switchToLayout(); });
    connect(_removeButton, &QPushButton::clicked, this, [this]() { removeLayout(); });
    connect(_renameButton, &QPushButton::clicked, this, [this]() { renameLayout(); });
    connect(_moveUpButton, &QPushButton::clicked, this, [this]() { moveLayout(-1); });
    connect(_moveDownButton, &QPushButton::clicked, this, [this]() { moveLayout(1); });
    connect(_centralWidgetComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int index) {
                if (_loadingSelection || (index < 0)) {
                    return;
                }

                MainWindow *mainWindow = MainWindow::instance();
                if (mainWindow != nullptr) {
                    mainWindow->layoutManager()->setLayoutCentralWidget(
                        selectedLayoutUuid(), _centralWidgetComboBox->itemData(index).toString());
                }
            });
    refreshLayouts();
}

QString LayoutsSettingsWidget::selectedLayoutUuid() const {
    QListWidgetItem *item = _layoutListWidget->currentItem();
    return item == nullptr ? QString() : item->data(Qt::UserRole).toString();
}

void LayoutsSettingsWidget::refreshLayouts(const QString &selectedUuid) {
    SettingsService settings;
    const QStringList uuids = settings.value(QStringLiteral("layouts")).toStringList();
    const QString currentUuid = settings.value(QStringLiteral("currentLayout")).toString();
    const QString uuidToSelect = selectedUuid.isEmpty() ? currentUuid : selectedUuid;

    const QSignalBlocker blocker(_layoutListWidget);
    _layoutListWidget->clear();

    for (const QString &uuid : uuids) {
        const QString name =
            settings.value(QStringLiteral("layout-") + uuid + QStringLiteral("/name")).toString();

        auto *item = new QListWidgetItem(name, _layoutListWidget);
        item->setData(Qt::UserRole, uuid);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(uuid == currentUuid ? Qt::Checked : Qt::Unchecked);
        if (uuid == uuidToSelect) {
            _layoutListWidget->setCurrentItem(item);
        }
    }

    if ((_layoutListWidget->currentItem() == nullptr) && (_layoutListWidget->count() > 0)) {
        _layoutListWidget->setCurrentRow(0);
    }

    updateSelectedLayout();
}

void LayoutsSettingsWidget::updateSelectedLayout() {
    const QString uuid = selectedLayoutUuid();
    const int row = _layoutListWidget->currentRow();
    const bool hasSelection = !uuid.isEmpty();
    SettingsService settings;

    _switchButton->setEnabled(hasSelection &&
                              (uuid != settings.value(QStringLiteral("currentLayout")).toString()));
    _removeButton->setEnabled(hasSelection && (_layoutListWidget->count() > 1));
    _renameButton->setEnabled(hasSelection);
    _moveUpButton->setEnabled(hasSelection && (row > 0));
    _moveDownButton->setEnabled(hasSelection && (row < _layoutListWidget->count() - 1));
    _centralWidgetComboBox->setEnabled(hasSelection);

    _loadingSelection = true;
    if (hasSelection) {
        MainWindow *mainWindow = MainWindow::instance();
        const QString centralWidget = (mainWindow != nullptr)
                                          ? mainWindow->layoutManager()->layoutCentralWidget(uuid)
                                          : QStringLiteral("note-edit");
        _centralWidgetComboBox->setCurrentIndex(_centralWidgetComboBox->findData(centralWidget));
    } else {
        _centralWidgetComboBox->setCurrentIndex(-1);
    }
    _loadingSelection = false;
}

void LayoutsSettingsWidget::updateCurrentLayout(QListWidgetItem *item) {
    if (item == nullptr) {
        return;
    }

    const QString uuid = item->data(Qt::UserRole).toString();
    MainWindow *mainWindow = MainWindow::instance();
    if ((mainWindow == nullptr) || uuid.isEmpty()) {
        return;
    }

    const bool isCurrent = uuid == mainWindow->layoutManager()->currentLayoutUuid();
    if (item->checkState() != Qt::Checked) {
        if (isCurrent) {
            refreshLayouts(uuid);
        }
        return;
    }

    if (!isCurrent) {
        switchToLayout(uuid);
    }
}

void LayoutsSettingsWidget::addLayout() {
    const QString name =
        QInputDialog::getText(this, tr("Add layout"), tr("Layout name:")).trimmed();
    MainWindow *mainWindow = MainWindow::instance();
    if ((mainWindow == nullptr) || name.isEmpty()) {
        return;
    }

    mainWindow->layoutManager()->storeCurrentLayout();
    if (!mainWindow->layoutManager()->createNewLayout(name)) {
        return;
    }

    refreshLayouts(mainWindow->layoutManager()->currentLayoutUuid());
}

void LayoutsSettingsWidget::addLayoutFromPreset() {
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Create layout from preset"));
    dialog.resize(720, 600);

    auto *dialogLayout = new QVBoxLayout(&dialog);
    auto *descriptionLabel = new QLabel(
        tr("Create a new layout from a built-in preset. Panel positions can be changed after "
           "unlocking panels."),
        &dialog);
    descriptionLabel->setWordWrap(true);
    dialogLayout->addWidget(descriptionLabel);

    auto *layoutPresetWidget = new LayoutPresetWidget(&dialog);
    dialogLayout->addWidget(layoutPresetWidget, 1);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    dialogLayout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(layoutPresetWidget, &LayoutPresetWidget::layoutStored, &dialog,
            [this, &dialog](const QString &uuid) {
                refreshLayouts(uuid);
                Q_EMIT layoutStored(uuid);
                dialog.accept();
            });

    QTimer::singleShot(0, layoutPresetWidget, &LayoutPresetWidget::resizeLayoutPresetImage);
    dialog.exec();
}

void LayoutsSettingsWidget::switchToLayout(const QString &uuid) {
    const QString targetUuid = uuid.isEmpty() ? selectedLayoutUuid() : uuid;
    MainWindow *mainWindow = MainWindow::instance();
    if ((mainWindow == nullptr) || targetUuid.isEmpty() ||
        (targetUuid == mainWindow->layoutManager()->currentLayoutUuid())) {
        return;
    }

    mainWindow->layoutManager()->setCurrentLayout(targetUuid);
    refreshLayouts(targetUuid);
}

void LayoutsSettingsWidget::removeLayout() {
    const QString uuid = selectedLayoutUuid();
    if (uuid.isEmpty() || (_layoutListWidget->count() < 2) ||
        (Utils::Gui::question(this, tr("Remove layout"), tr("Remove the selected layout?"),
                              QStringLiteral("remove-layout")) != QMessageBox::Yes)) {
        return;
    }

    MainWindow *mainWindow = MainWindow::instance();
    if ((mainWindow != nullptr) && mainWindow->layoutManager()->removeLayout(uuid)) {
        refreshLayouts();
    }
}

void LayoutsSettingsWidget::renameLayout() {
    const QString uuid = selectedLayoutUuid();
    if (uuid.isEmpty()) {
        return;
    }

    SettingsService settings;
    const QString oldName =
        settings.value(QStringLiteral("layout-") + uuid + QStringLiteral("/name")).toString();
    const QString name = QInputDialog::getText(this, tr("Rename layout"), tr("Layout name:"),
                                               QLineEdit::Normal, oldName)
                             .trimmed();
    MainWindow *mainWindow = MainWindow::instance();
    if ((mainWindow != nullptr) && mainWindow->layoutManager()->renameLayout(uuid, name)) {
        refreshLayouts(uuid);
    }
}

void LayoutsSettingsWidget::moveLayout(int offset) {
    const int row = _layoutListWidget->currentRow();
    const int targetRow = row + offset;
    if ((row < 0) || (targetRow < 0) || (targetRow >= _layoutListWidget->count())) {
        return;
    }

    QStringList uuids;
    for (int i = 0; i < _layoutListWidget->count(); ++i) {
        uuids.append(_layoutListWidget->item(i)->data(Qt::UserRole).toString());
    }
    uuids.move(row, targetRow);

    MainWindow *mainWindow = MainWindow::instance();
    if ((mainWindow != nullptr) && mainWindow->layoutManager()->setLayoutOrder(uuids)) {
        refreshLayouts(uuids.at(targetRow));
    }
}
