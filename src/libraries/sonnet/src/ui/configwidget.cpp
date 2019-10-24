/**
 * configwidget.cpp
 *
 * Copyright (C)  2004  Zack Rusin <zack@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#include "configwidget.h"
#include "ui_configui.h"

#include "loader_p.h"
#include "settings_p.h"

#include <QCheckBox>
#include <QListWidget>
#include <QLineEdit>
#include <QListWidgetItem>
#include "ui_debug.h"

using namespace Sonnet;

class ConfigWidgetPrivate
{
public:
    Ui_SonnetConfigUI ui;
    Loader *loader = nullptr;
    QWidget *wdg = nullptr;
};

ConfigWidget::ConfigWidget(QWidget *parent)
    : QWidget(parent)
    , d(new ConfigWidgetPrivate)
{
    d->loader = Loader::openLoader();
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setObjectName(QStringLiteral("SonnetConfigUILayout"));
    d->wdg = new QWidget(this);
    d->ui.setupUi(d->wdg);

    d->ui.m_langCombo->setCurrentByDictionary(d->loader->settings()->defaultLanguage());

    QStringList preferredLanguages = d->loader->settings()->preferredLanguages();
    for (int i = 0; i < d->ui.m_langCombo->count(); i++) {
        const QString tag = d->ui.m_langCombo->itemData(i).toString();
        if (tag.isEmpty()) { // skip separator
            continue;
        }

        QListWidgetItem *item = new QListWidgetItem(d->ui.m_langCombo->itemText(i), d->ui.languageList);
        item->setData(Qt::UserRole, tag);
        if (preferredLanguages.contains(tag)) {
            item->setCheckState(Qt::Checked);
        } else {
            item->setCheckState(Qt::Unchecked);
        }
    }

    d->ui.m_skipUpperCB->setChecked(!d->loader->settings()->checkUppercase());
    d->ui.m_skipRunTogetherCB->setChecked(d->loader->settings()->skipRunTogether());
    d->ui.m_checkerEnabledByDefaultCB->setChecked(d->loader->settings()->checkerEnabledByDefault());
    d->ui.m_autodetectCB->setChecked(d->loader->settings()->autodetectLanguage());
    QStringList ignoreList = d->loader->settings()->currentIgnoreList();
    ignoreList.sort();
    d->ui.ignoreListWidget->addItems(ignoreList);
    d->ui.m_bgSpellCB->setChecked(d->loader->settings()->backgroundCheckerEnabled());
    d->ui.m_bgSpellCB->hide();//hidden by default
    connect(d->ui.addButton, &QAbstractButton::clicked, this, &ConfigWidget::slotIgnoreWordAdded);
    connect(d->ui.removeButton, &QAbstractButton::clicked, this,
            &ConfigWidget::slotIgnoreWordRemoved);

    layout->addWidget(d->wdg);
    connect(d->ui.m_langCombo, &DictionaryComboBox::dictionaryChanged, this,
            &ConfigWidget::configChanged);
    connect(d->ui.languageList, &QListWidget::itemChanged, this, &ConfigWidget::configChanged);

    connect(d->ui.m_bgSpellCB, &QAbstractButton::clicked, this, &ConfigWidget::configChanged);
    connect(d->ui.m_skipUpperCB, &QAbstractButton::clicked, this, &ConfigWidget::configChanged);
    connect(d->ui.m_skipRunTogetherCB, &QAbstractButton::clicked, this,
            &ConfigWidget::configChanged);
    connect(d->ui.m_checkerEnabledByDefaultCB, &QAbstractButton::clicked, this,
            &ConfigWidget::configChanged);
    connect(d->ui.m_autodetectCB, &QAbstractButton::clicked, this, &ConfigWidget::configChanged);
    connect(d->ui.newIgnoreEdit, &QLineEdit::textChanged, this, &ConfigWidget::slotUpdateButton);
    connect(d->ui.ignoreListWidget, &QListWidget::itemSelectionChanged, this, &ConfigWidget::slotSelectionChanged);
    d->ui.nobackendfound->setVisible(d->loader->clients().isEmpty());
    d->ui.addButton->setEnabled(false);
    d->ui.removeButton->setEnabled(false);
}

ConfigWidget::~ConfigWidget()
{
    delete d;
}

void ConfigWidget::slotUpdateButton(const QString &text)
{
    d->ui.addButton->setEnabled(!text.isEmpty());
}

void ConfigWidget::slotSelectionChanged()
{
    d->ui.removeButton->setEnabled(!d->ui.ignoreListWidget->selectedItems().isEmpty());
}

void ConfigWidget::save()
{
    setFromGui();
}

void ConfigWidget::setFromGui()
{
    Settings *settings = d->loader->settings();

    if (d->ui.m_langCombo->count()) {
        settings->setDefaultLanguage(d->ui.m_langCombo->currentDictionary());
    }

    QStringList preferredLanguages;
    for (int i = 0; i < d->ui.languageList->count(); i++) {
        if (d->ui.languageList->item(i)->checkState() == Qt::Unchecked) {
            continue;
        }
        preferredLanguages << d->ui.languageList->item(i)->data(Qt::UserRole).toString();
    }
    settings->setPreferredLanguages(preferredLanguages);

    settings->setCheckUppercase(!d->ui.m_skipUpperCB->isChecked());
    settings->setSkipRunTogether(d->ui.m_skipRunTogetherCB->isChecked());
    settings->setBackgroundCheckerEnabled(d->ui.m_bgSpellCB->isChecked());
    settings->setCheckerEnabledByDefault(d->ui.m_checkerEnabledByDefaultCB->isChecked());
    settings->setAutodetectLanguage(d->ui.m_autodetectCB->isChecked());

    if (settings->modified()) {
        settings->save();
    }
}

void ConfigWidget::slotIgnoreWordAdded()
{
    QStringList ignoreList = d->loader->settings()->currentIgnoreList();
    QString newWord = d->ui.newIgnoreEdit->text();
    d->ui.newIgnoreEdit->clear();
    if (newWord.isEmpty() || ignoreList.contains(newWord)) {
        return;
    }
    ignoreList.append(newWord);
    d->loader->settings()->setCurrentIgnoreList(ignoreList);

    d->ui.ignoreListWidget->clear();
    d->ui.ignoreListWidget->addItems(ignoreList);

    emit configChanged();
}

void ConfigWidget::slotIgnoreWordRemoved()
{
    QStringList ignoreList = d->loader->settings()->currentIgnoreList();
    const QList<QListWidgetItem *> selectedItems = d->ui.ignoreListWidget->selectedItems();
    for (const QListWidgetItem *item : selectedItems) {
        ignoreList.removeAll(item->text());
    }
    d->loader->settings()->setCurrentIgnoreList(ignoreList);

    d->ui.ignoreListWidget->clear();
    d->ui.ignoreListWidget->addItems(ignoreList);

    emit configChanged();
}

void ConfigWidget::setBackgroundCheckingButtonShown(bool b)
{
    d->ui.m_bgSpellCB->setVisible(b);
}

bool ConfigWidget::backgroundCheckingButtonShown() const
{
    return !d->ui.m_bgSpellCB->isHidden();
}

void ConfigWidget::slotDefault()
{
    d->ui.m_autodetectCB->setChecked(true);
    d->ui.m_skipUpperCB->setChecked(false);
    d->ui.m_skipRunTogetherCB->setChecked(false);
    d->ui.m_checkerEnabledByDefaultCB->setChecked(false);
    d->ui.m_bgSpellCB->setChecked(true);
    d->ui.ignoreListWidget->clear();
    d->ui.m_langCombo->setCurrentByDictionary(d->loader->settings()->defaultLanguage());
}

void ConfigWidget::setLanguage(const QString &language)
{
    d->ui.m_langCombo->setCurrentByDictionary(language);
}

QString ConfigWidget::language() const
{
    if (d->ui.m_langCombo->count()) {
        return d->ui.m_langCombo->currentDictionary();
    } else {
        return QString();
    }
}
