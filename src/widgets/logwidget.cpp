#include <QDateTime>
#include <QSettings>
#include <QScrollBar>
#include <QDebug>
#include "logwidget.h"

#ifndef INTEGRATION_TESTS
#include "ui_logwidget.h"
#endif

LogWidget::LogWidget(QWidget *parent) :
        QFrame(parent)
#ifndef INTEGRATION_TESTS
        ,ui(new Ui::LogWidget)
#endif
{
#ifndef INTEGRATION_TESTS
    ui->setupUi(this);

    // init the log text edit search frame
    ui->logTextEdit->initSearchFrame(ui->logTextEditSearchFrame);

    // load the dialog settings
    QSettings settings;
    ui->debugCheckBox->setChecked(
            settings.value("LogWidget/debugLog", false).toBool());
    ui->infoCheckBox->setChecked(
            settings.value("LogWidget/infoLog", false).toBool());
    ui->warningCheckBox->setChecked(
            settings.value("LogWidget/warningLog", true).toBool());
    ui->criticalCheckBox->setChecked(
            settings.value("LogWidget/criticalLog", true).toBool());
    ui->fatalCheckBox->setChecked(
            settings.value("LogWidget/fatalLog", true).toBool());
    ui->statusCheckBox->setChecked(
            settings.value("LogWidget/statusLog", true).toBool());
    ui->scriptingCheckBox->setChecked(
            settings.value("LogWidget/scriptingLog", true).toBool());

    // store the settings if the dialog is closed or the checkboxes are clicked
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

LogWidget::~LogWidget()
{
#ifndef INTEGRATION_TESTS
    delete ui;
#endif
}

/**
 * Stores the settings of the dialog
 */
void LogWidget::storeSettings() const {
#ifndef INTEGRATION_TESTS
    QSettings settings;
    settings.setValue("LogWidget/debugLog",
                      ui->debugCheckBox->isChecked());
    settings.setValue("LogWidget/infoLog",
                      ui->infoCheckBox->isChecked());
    settings.setValue("LogWidget/warningLog",
                      ui->warningCheckBox->isChecked());
    settings.setValue("LogWidget/criticalLog",
                      ui->criticalCheckBox->isChecked());
    settings.setValue("LogWidget/fatalLog",
                      ui->fatalCheckBox->isChecked());
    settings.setValue("LogWidget/statusLog",
                      ui->statusCheckBox->isChecked());
    settings.setValue("LogWidget/scriptingLog",
                      ui->scriptingCheckBox->isChecked());
#endif
}

/**
 * Adds a log entry
 */
void LogWidget::log(LogType logType, QString text) {
#ifndef INTEGRATION_TESTS
    // return if logging wasn't enabled
    if (!qApp->property("loggingEnabled").toBool()) {
        return;
    }

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
LogWidget * LogWidget::instance() {
    LogWidget *logWidget = nullptr;
#ifndef INTEGRATION_TESTS
    logWidget = qApp->property("logWidget").value<LogWidget *>();
#endif

    if (logWidget == Q_NULLPTR) {
        logWidget = createInstance(NULL);
    }

    return logWidget;
}

/**
 * Creates a global instance of the class
 */
LogWidget * LogWidget::createInstance(QWidget *parent) {
    LogWidget *logWidget = new LogWidget(parent);

#ifndef INTEGRATION_TESTS
    qApp->setProperty(
            "logWidget",
            QVariant::fromValue<LogWidget *>(logWidget));
#endif

    return logWidget;
}

/**
 * This is our custom log output
 */
void LogWidget::logMessageOutput(
        QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();

#ifndef INTEGRATION_TESTS
    bool loggingEnabled = qApp->property("loggingEnabled").toBool();
#else
    bool loggingEnabled = false;
#endif

    switch (type) {
        case QtDebugMsg:
//            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(),
//                    context.file, context.line, context.function);
            fprintf(stderr, "Debug: %s\n", localMsg.constData());

            if (loggingEnabled) {
                LogWidget::instance()->log(LogType::DebugLogType, msg);
            }
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
        case QtInfoMsg:
            fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);

            if (loggingEnabled) {
                LogWidget::instance()->log(LogType::InfoLogType, msg);
            }
            break;
#endif
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);

            if (loggingEnabled) {
                LogWidget::instance()->log(LogType::WarningLogType, msg);
            }
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);

            if (loggingEnabled) {
                LogWidget::instance()->log(LogType::CriticalLogType, msg);
            }
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);

            if (loggingEnabled) {
                LogWidget::instance()->log(LogType::FatalLogType, msg);
            }
            abort();
    }
}

/**
 * Clears all log entries
 */
void LogWidget::on_clearButton_clicked()
{
#ifndef INTEGRATION_TESTS
    ui->logTextEdit->clear();
#endif
}
