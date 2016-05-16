#include <QDateTime>
#include <QSettings>
#include <QDebug>
#include "logdialog.h"
#include "ui_logdialog.h"

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);

    // load the dialog settings
    QSettings settings;
    restoreGeometry(settings.value("LogDialog/geometry").toByteArray());
    ui->tabWidget->setCurrentIndex(
            settings.value("LogDialog/currentTab", StatusLogType).toInt());

    // store the settings if the dialog is closed
    connect(this, SIGNAL(rejected()), this, SLOT(storeSettings()));
}

LogDialog::~LogDialog()
{
    delete ui;
}

/**
 * Stores the settings of the dialog
 */
void LogDialog::storeSettings() const {
    QSettings settings;
    settings.setValue("LogDialog/geometry", saveGeometry());
    settings.setValue("LogDialog/currentTab", ui->tabWidget->currentIndex());
}

/**
 * Adds a log entry
 */
void LogDialog::addLogEntry(LogType logType, QString text) {
    QTextEdit *textEdit;

    switch (logType) {
        case ScriptingLogType:
            textEdit = ui->scriptingLogTextEdit;
            break;
        default:
            textEdit = ui->statusLogTextEdit;
            break;
    }

    QDateTime dateTime = QDateTime::currentDateTime();
    text.prepend("\n[" + dateTime.toString("hh:mm:ss") + "] ");
    text.prepend(textEdit->toPlainText());
    text = text.trimmed();
    textEdit->setText(text);

    // move the text cursor to the end
    QTextCursor c = textEdit->textCursor();
    c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    textEdit->setTextCursor(c);
}

/**
 * Fetches the global instance of the class
 * The instance will be created if it doesn't exist.
 */
LogDialog * LogDialog::instance() {
    LogDialog *logDialog =
            qApp->property("logDialog").value<LogDialog *>();

    if (logDialog == NULL) {
        logDialog = createInstance(NULL);
    }

    return logDialog;
}

/**
 * Creates a global instance of the class
 */
LogDialog * LogDialog::createInstance(QWidget *parent) {
    LogDialog *logDialog = new LogDialog(parent);

    qApp->setProperty(
            "logDialog",
            QVariant::fromValue<LogDialog *>(logDialog));

    return logDialog;
}
