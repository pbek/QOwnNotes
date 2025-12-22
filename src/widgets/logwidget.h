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
#include <QFrame>

namespace Ui {
class LogWidget;
}

class LogWidget : public QFrame {
    Q_OBJECT

   public:
    enum LogType {
        DebugLogType,
        InfoLogType,
        WarningLogType,
        CriticalLogType,
        FatalLogType,
        StatusLogType,
        ScriptingLogType
    };
    Q_ENUM(LogType);

    explicit LogWidget(QWidget *parent = 0);
    ~LogWidget();
    static void logMessageOutput(QtMsgType type, const QMessageLogContext &context,
                                 const QString &msg);
    bool eventFilter(QObject *obj, QEvent *event) override;
    QString getLogText() const;

   public slots:
    void log(LogWidget::LogType logType, const QString &text);

   private slots:
    void storeSettings() const;

    void on_clearButton_clicked();

    void on_logTextEdit_customContextMenuRequested(QPoint pos);

   private:
    Ui::LogWidget *ui;

    static QString logTypeText(LogType logType);

    static void logToFileIfAllowed(LogType logType, const QString &msg);
};
