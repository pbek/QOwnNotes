/*
 * configdialog.h
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
#ifndef SONNET_CONFIGDIALOG_H
#define SONNET_CONFIGDIALOG_H

#include <QDialog>
class ConfigDialogPrivate;
#include "sonnetui_export.h"

namespace Sonnet {
/// The sonnet ConfigDialog
class SONNETUI_EXPORT ConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConfigDialog(QWidget *parent);
    ~ConfigDialog();

    /**
     * Sets the language/dictionary that will be selected by default
     * in this config dialog.
     * This overrides the setting in the config file.
     *
     * @param language the language which will be selected by default.
     * @since 4.1
     */
    void setLanguage(const QString &language);
    /**
     * return selected language
     * @since 4.8.1
     */
    QString language() const;

protected Q_SLOTS:
    virtual void slotOk();
    virtual void slotApply();

Q_SIGNALS:

    /**
     * This is emitted all the time when we change config and not just language
     *
     * @param language the language which the user has selected
     * @since 4.1
     */

    void languageChanged(const QString &language);

    /**
     * This is emitted when configChanged
     * @since 4.8.1
     */
    void configChanged();

private:
    ConfigDialogPrivate *const d;
    Q_DISABLE_COPY(ConfigDialog)
    Q_PRIVATE_SLOT(d, void slotConfigChanged())
};
}

#endif
