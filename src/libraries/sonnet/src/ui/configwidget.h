/*
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
#ifndef SONNET_CONFIGWIDGET_H
#define SONNET_CONFIGWIDGET_H

#include <QWidget>
class ConfigWidgetPrivate;
#include "sonnetui_export.h"

namespace Sonnet {
/// The sonnet ConfigWidget
class SONNETUI_EXPORT ConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigWidget(QWidget *parent);
    ~ConfigWidget();

    bool backgroundCheckingButtonShown() const;

    /**
     * Sets the language/dictionary that will be selected by default
     * in this config widget.
     * This overrides the setting in the config file.
     *
     * @param language the language which will be selected by default.
     * @since 4.1
     */
    void setLanguage(const QString &language);

    /**
     * Get the currently selected language for spell checking.  Returns an empty string if
     * kdelibs was built without any spellchecking plugins.
     * @return the language currently selected in the language combobox
     * @since 4.1
     */
    QString language() const;

public Q_SLOTS:
    void save();
    void setBackgroundCheckingButtonShown(bool);
    void slotDefault();
protected Q_SLOTS:
    void slotIgnoreWordRemoved();
    void slotIgnoreWordAdded();
private Q_SLOTS:
    void slotUpdateButton(const QString &text);
    void slotSelectionChanged();

Q_SIGNALS:
    /**
     * Signal sends when config was changed
     * @since 4.1
     */
    void configChanged();

private:
    void setFromGui();

private:
    ConfigWidgetPrivate *const d;
};
}

#endif
