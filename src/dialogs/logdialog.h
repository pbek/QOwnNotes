#pragma once

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
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

    explicit LogDialog(QWidget *parent = 0);
    ~LogDialog();
    static LogDialog *instance();
    static LogDialog *createInstance(QWidget *parent);
    void log(LogType logType, QString text);
    static void logMessageOutput(
            QtMsgType type,
            const QMessageLogContext &context,
            const QString &msg);

private slots:
    void storeSettings() const;

    void on_clearButton_clicked();

private:
    Ui::LogDialog *ui;
};
