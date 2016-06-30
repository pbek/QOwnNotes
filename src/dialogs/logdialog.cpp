#include <QDateTime>
#include <QSettings>
#include <QScrollBar>
#include <QDebug>
#include "logdialog.h"

#ifndef INTEGRATION_TESTS
#include "ui_logdialog.h"
#endif

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent)
#ifndef INTEGRATION_TESTS
        ,ui(new Ui::LogDialog)
#endif
{
#ifndef INTEGRATION_TESTS
    ui->setupUi(this);

    // init the log text edit search frame
    ui->logTextEdit->initSearchFrame(ui->logTextEditSearchFrame);

    // load the dialog settings
    QSettings settings;
    restoreGeometry(settings.value("LogDialog/geometry").toByteArray());
    ui->debugCheckBox->setChecked(
            settings.value("LogDialog/debugLog", false).toBool());
    ui->infoCheckBox->setChecked(
            settings.value("LogDialog/infoLog", false).toBool());
    ui->warningCheckBox->setChecked(
            settings.value("LogDialog/warningLog", false).toBool());
    ui->criticalCheckBox->setChecked(
            settings.value("LogDialog/criticalLog", true).toBool());
    ui->fatalCheckBox->setChecked(
            settings.value("LogDialog/fatalLog", true).toBool());
    ui->statusCheckBox->setChecked(
            settings.value("LogDialog/statusLog", true).toBool());
    ui->scriptingCheckBox->setChecked(
            settings.value("LogDialog/scriptingLog", true).toBool());

    // store the settings if the dialog is closed or the checkboxes are clicked
    connect(this, SIGNAL(rejected()), this, SLOT(storeSettings()));
    connect(ui->debugCheckBox, SIGNAL(clicked()), this, SLOT(storeSettings()));
    connect(ui->infoCheckBox, SIGNAL(clicked()), this, SLOT(storeSettings()));
    connect(ui->warningCheckBox, SIGNAL(clicked()),
            this, SLOT(storeSettings()));
    connect(ui->criticalCheckBox, SIGNAL(clicked()),
            this, SLOT(storeSettings()));
    connect(ui->fatalCheckBox, SIGNAL(clicked()), this, SLOT(storeSettings()));
    connect(ui->statusCheckBox, SIGNAL(clicked()), this, SLOT(storeSettings()));
    connect(ui->scriptingCheckBox, SIGNAL(clicked()),
            this, SLOT(storeSettings()));
#endif
}

LogDialog::~LogDialog()
{
#ifndef INTEGRATION_TESTS
    delete ui;
#endif
}

/**
 * Stores the settings of the dialog
 */
void LogDialog::storeSettings() const {
#ifndef INTEGRATION_TESTS
    QSettings settings;
    settings.setValue("LogDialog/geometry", saveGeometry());
    settings.setValue("LogDialog/debugLog",
                      ui->debugCheckBox->isChecked());
    settings.setValue("LogDialog/infoLog",
                      ui->infoCheckBox->isChecked());
    settings.setValue("LogDialog/warningLog",
                      ui->warningCheckBox->isChecked());
    settings.setValue("LogDialog/criticalLog",
                      ui->criticalCheckBox->isChecked());
    settings.setValue("LogDialog/fatalLog",
                      ui->fatalCheckBox->isChecked());
    settings.setValue("LogDialog/statusLog",
                      ui->statusCheckBox->isChecked());
    settings.setValue("LogDialog/scriptingLog",
                      ui->scriptingCheckBox->isChecked());
#endif
}

/**
 * Adds a log entry
 */
void LogDialog::log(LogType logType, QString text) {
#ifndef INTEGRATION_TESTS
    QString type = "";
    QColor color = QColor(Qt::black);

    switch (logType) {
        case DebugLogType:
            if (!ui->debugCheckBox->isChecked()) {
                return;
            }
            type = "debug";
            // gray
            color = QColor(98, 98, 98);
            break;
        case InfoLogType:
            if (!ui->infoCheckBox->isChecked()) {
                return;
            }
            type = "info";
            color = QColor(Qt::darkBlue);
            break;
        case WarningLogType:
            if (!ui->warningCheckBox->isChecked()) {
                return;
            }
            type = "warning";
            // orange
            color = QColor(255, 128, 0);
            break;
        case CriticalLogType:
            if (!ui->criticalCheckBox->isChecked()) {
                return;
            }
            type = "critical";
            // light red
            color = QColor(192, 0, 0);
            break;
        case FatalLogType:
            if (!ui->fatalCheckBox->isChecked()) {
                return;
            }
            type = "fatal";
            // lighter red
            color = QColor(210, 0, 0);
            break;
        case StatusLogType:
            if (!ui->statusCheckBox->isChecked()) {
                return;
            }
            type = "status";
            // green
            color = QColor(0, 128, 0);
            break;
        case ScriptingLogType:
            if (!ui->scriptingCheckBox->isChecked()) {
                return;
            }
            type = "scripting";
            // blue
            color = QColor(0, 102, 255);
            break;
        default:
            type = "unknown";
            break;
    }

    QDateTime dateTime = QDateTime::currentDateTime();
//    text.prepend("[" + dateTime.toString("hh:mm:ss") + "] [" + type + "] ");
//    text.append("\n");

    QString html = QString("<div style=\"color: %1\">[%2] [%3] %4</div><br />")
            .arg(color.name(), dateTime.toString("hh:mm:ss"), type,
                 text.toHtmlEscaped());

    QScrollBar *scrollBar = ui->logTextEdit->verticalScrollBar();

    // we want to scroll down later if the scrollbar is near the end bottom
    bool scrollDown = scrollBar->value() >=
            (scrollBar->maximum() - scrollBar->singleStep());

    QTextCursor c = ui->logTextEdit->textCursor();

    // insert the text at the end
    c.movePosition(QTextCursor::End);
    c.insertHtml(html);

    if (scrollDown) {
        // move the text cursor to the end
        ui->logTextEdit->moveCursor(QTextCursor::End);
    }
#else
    Q_UNUSED(logType);
    Q_UNUSED(text);
#endif
}

/**
 * Fetches the global instance of the class
 * The instance will be created if it doesn't exist.
 */
LogDialog * LogDialog::instance() {
    LogDialog *logDialog = nullptr;
#ifndef INTEGRATION_TESTS
    logDialog = qApp->property("logDialog").value<LogDialog *>();
#endif

    if (logDialog == Q_NULLPTR) {
        logDialog = createInstance(NULL);
    }

    return logDialog;
}

/**
 * Creates a global instance of the class
 */
LogDialog * LogDialog::createInstance(QWidget *parent) {
    LogDialog *logDialog = new LogDialog(parent);

#ifndef INTEGRATION_TESTS
    qApp->setProperty(
            "logDialog",
            QVariant::fromValue<LogDialog *>(logDialog));
#endif

    return logDialog;
}

/**
 * This is our custom log output
 */
void LogDialog::logMessageOutput(
        QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
//            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(),
//                    context.file, context.line, context.function);
            fprintf(stderr, "Debug: %s\n", localMsg.constData());
            LogDialog::instance()->log(LogType::DebugLogType, msg);
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
        case QtInfoMsg:
            fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            LogDialog::instance()->log(LogType::InfoLogType, msg);
            break;
#endif
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            LogDialog::instance()->log(LogType::WarningLogType, msg);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            LogDialog::instance()->log(LogType::CriticalLogType, msg);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            LogDialog::instance()->log(LogType::FatalLogType, msg);
            abort();
    }
}

/**
 * Clears all log entries
 */
void LogDialog::on_clearButton_clicked()
{
#ifndef INTEGRATION_TESTS
    ui->logTextEdit->clear();
#endif
}
