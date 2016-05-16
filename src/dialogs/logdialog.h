#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    enum LogType {
        StatusLogType,
        ScriptingLogType,
    };

    explicit LogDialog(QWidget *parent = 0);
    ~LogDialog();
    static LogDialog *instance();
    static LogDialog *createInstance(QWidget *parent);
    void addLogEntry(LogType logType, QString text);

private slots:
    void storeSettings() const;

private:
    Ui::LogDialog *ui;
};

#endif // LOGDIALOG_H
