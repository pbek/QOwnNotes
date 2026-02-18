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

#pragma once

#include <QEvent>
#include <QWidget>

class QLiteHtmlWidget;

namespace Ui {
class QLiteHtmlSearchWidget;
}

class QLiteHtmlSearchWidget : public QWidget
{
    Q_OBJECT

public:
    enum SearchMode { PlainTextMode, WholeWordsMode, RegularExpressionMode };

    explicit QLiteHtmlSearchWidget(QLiteHtmlWidget *parent = nullptr);
    bool doSearch(bool searchDown = true, bool allowRestartAtTop = true);
    void setDarkMode(bool enabled);
    ~QLiteHtmlSearchWidget();

private:
    Ui::QLiteHtmlSearchWidget *ui;

protected:
    QLiteHtmlWidget *_liteHtmlWidget;
    bool _darkMode;
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void activate();
    void deactivate();
    void doSearchDown();
    void doSearchUp();

protected slots:
    void searchLineEditTextChanged(const QString &arg1);
};
