/*
 * Copyright (c) 2003 Ingo Kloecker <kloecker@kde.org>
 * Copyright (c) 2008 Tom Albers <tomalbers@kde.nl>
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

#include "dictionarycombobox.h"

#include <speller.h>
#include "ui_debug.h"

namespace Sonnet {
//@cond PRIVATE
class DictionaryComboBoxPrivate
{
public:
    explicit DictionaryComboBoxPrivate(DictionaryComboBox *combo) : q(combo)
    {
    }

    DictionaryComboBox *q;
    void slotDictionaryChanged(int idx);
};

void DictionaryComboBoxPrivate::slotDictionaryChanged(int idx)
{
    emit q->dictionaryChanged(q->itemData(idx).toString());
    emit q->dictionaryNameChanged(q->itemText(idx));
}

//@endcon

DictionaryComboBox::DictionaryComboBox(QWidget *parent)
    : QComboBox(parent)
    , d(new DictionaryComboBoxPrivate(this))
{
    reloadCombo();
    connect(this, SIGNAL(activated(int)),
            SLOT(slotDictionaryChanged(int)));
}

DictionaryComboBox::~DictionaryComboBox()
{
    delete d;
}

QString DictionaryComboBox::currentDictionaryName() const
{
    return currentText();
}

QString DictionaryComboBox::currentDictionary() const
{
    return itemData(currentIndex()).toString();
}

bool DictionaryComboBox::assignDictionnaryName(const QString &name)
{
    if (name.isEmpty() || name == currentText()) {
        return false;
    }

    int idx = findText(name);
    if (idx == -1) {
        qCDebug(SONNET_LOG_UI) << "name not found" << name;
        return false;
    }

    setCurrentIndex(idx);
    d->slotDictionaryChanged(idx);
    return true;
}

void DictionaryComboBox::setCurrentByDictionaryName(const QString &name)
{
    assignDictionnaryName(name);
}

bool DictionaryComboBox::assignByDictionnary(const QString &dictionary)
{
    if (dictionary.isEmpty()) {
        return false;
    }
    if (dictionary == itemData(currentIndex()).toString()) {
        return true;
    }

    int idx = findData(dictionary);
    if (idx == -1) {
        qCDebug(SONNET_LOG_UI) << "dictionary not found" << dictionary;
        return false;
    }

    setCurrentIndex(idx);
    d->slotDictionaryChanged(idx);
    return true;
}

void DictionaryComboBox::setCurrentByDictionary(const QString &dictionary)
{
    assignByDictionnary(dictionary);
}

void DictionaryComboBox::reloadCombo()
{
    clear();
    Sonnet::Speller speller;
    QMap<QString, QString> preferredDictionaries = speller.preferredDictionaries();
    QMapIterator<QString, QString> i(preferredDictionaries);
    while (i.hasNext()) {
        i.next();
        addItem(i.key(), i.value());
    }
    if (count()) {
        insertSeparator(count());
    }

    QMap<QString, QString> dictionaries = speller.availableDictionaries();
    i = dictionaries;
    while (i.hasNext()) {
        i.next();
        if (preferredDictionaries.contains(i.key())) {
            continue;
        }
        addItem(i.key(), i.value());
    }
}

} // namespace Sonnet

#include "moc_dictionarycombobox.cpp"
