#include <QDateTime>
#include <QSettings>
#include <QScrollBar>
#include <QMenu>
#include <QFile>
#include <QKeyEvent>
#include <QDebug>
#include <utils/misc.h>
#include "logwidget.h"

#ifndef INTEGRATION_TESTS
#include "ui_logwidget.h"
#endif

LogWidget::LogWidget(QWidget *parent) :
        QFrame(parent)
#ifndef INTEGRATION_TESTS
        , ui(new Ui::LogWidget)
#endif
{
#ifndef INTEGRATION_TESTS
    connect(this, SIGNAL(destroyed(QObject*)),
            this, SLOT(onDestroyed(QObject*)));

    ui->setupUi(this);

    connect(ui->logTextEdit, SIGNAL(destroyed(QObject*)),
            this, SLOT(onDestroyed(QObject*)));

    ui->buttonFrame->hide();
    QSettings settings;

    // init the log text edit search frame
    bool darkMode = settings.value("darkMode").toBool();
    ui->logTextEdit->initSearchFrame(ui->logTextEditSearchFrame, darkMode);

    // turn off markdown highlighting
    ui->logTextEdit->setHighlightingEnabled(false);

    // load the dialog settings
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

    ui->logTextEdit->installEventFilter(this);
    ui->buttonFrame->installEventFilter(this);
#endif
}

LogWidget::~LogWidget()
{
#ifndef INTEGRATION_TESTS
    delete ui;
#endif
}

/**
 * Returns the text of the log widget
 */
QString LogWidget::getLogText() {
#ifndef INTEGRATION_TESTS
    return ui->logTextEdit->toPlainText();
#else
    return QString();
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
void LogWidget::log(LogWidget::LogType logType, const QString &text) {
    // ignore "libpng sRGB profile", "QXcbConnection: XCB error: 8" and
    // "QFileSystemWatcher::removePaths" warnings
    if (logType == WarningLogType &&
        (text.contains("libpng warning: iCCP: known incorrect sRGB profile") ||
         text.contains("QXcbConnection: XCB error:") ||
         text.contains("QFileSystemWatcher::removePaths: list is empty"))) {
        return;
    }

#ifndef INTEGRATION_TESTS
    // log to the log file
    logToFileIfAllowed(logType, text);

    // return if logging wasn't enabled or if widget is not visible
    if (!qApp->property("loggingEnabled").toBool() || !isVisible()) {
        return;
    }

    QString type = logTypeText(logType);
    QColor color = QColor(Qt::black);

    switch (logType) {
        case DebugLogType:
            if (!ui->debugCheckBox->isChecked()) {
                return;
            }

            // print debug messages to stderr if in release-mode but debug
            // logging is enabled in the log panel
#ifndef QT_DEBUG
            fprintf(stderr, "Debug: %s\n", text.toLocal8Bit().constData());
#endif

            // gray
            color = QColor(98, 98, 98);
            break;
        case InfoLogType:
            if (!ui->infoCheckBox->isChecked()) {
                return;
            }
            color = QColor(Qt::darkBlue);
            break;
        case WarningLogType:
            if (!ui->warningCheckBox->isChecked()) {
                return;
            }

            // this is a "fix" for crashes that occur when a network goes away
            // and this message should be printed, I haven't managed to get
            // around this crash with other methods
            if (text.contains(
                    "/org/freedesktop/NetworkManager/ActiveConnection")) {
                return;
            }

            // orange
            color = QColor(255, 128, 0);
            break;
        case CriticalLogType:
            if (!ui->criticalCheckBox->isChecked()) {
                return;
            }
            // light red
            color = QColor(192, 0, 0);
            break;
        case FatalLogType:
            if (!ui->fatalCheckBox->isChecked()) {
                return;
            }
            // lighter red
            color = QColor(210, 0, 0);
            break;
        case StatusLogType:
            if (!ui->statusCheckBox->isChecked()) {
                return;
            }
            // green
            color = QColor(0, 128, 0);
            break;
        case ScriptingLogType:
            if (!ui->scriptingCheckBox->isChecked()) {
                return;
            }
            // blue
            color = QColor(0, 102, 255);
            break;
    }

    QDateTime dateTime = QDateTime::currentDateTime();
//    text.prepend("[" + dateTime.toString("hh:mm:ss") + "] [" + type + "] ");
//    text.append("\n");

    QString html = QString("<div style=\"color: %1\">[%2] [%3] %4</div>")
            .arg(color.name(), dateTime.toString("hh:mm:ss"), type,
                 text.toHtmlEscaped());

    QScrollBar *scrollBar = ui->logTextEdit->verticalScrollBar();

    // we want to scroll down later if the scrollbar is near the bottom
    bool scrollDown = scrollBar->value() >=
            (scrollBar->maximum() - scrollBar->singleStep());

    // return if logging isn't enabled any more
    if (!qApp->property("loggingEnabled").toBool()) {
        return;
    }

    const QSignalBlocker blocker(ui->logTextEdit);
    Q_UNUSED(blocker)

    // insert the text at the end
    ui->logTextEdit->appendHtml(html.trimmed());

    if (scrollDown) {
        // move the text cursor to the end
        ui->logTextEdit->moveCursor(QTextCursor::End);
//        scrollBar->setValue(scrollBar->maximum());
    }
#else
    Q_UNUSED(logType)
    Q_UNUSED(text)
#endif
}

/**
 * Returns the text for a LogType
 *
 * @param logType
 * @return
 */
QString LogWidget::logTypeText(LogType logType) {
    QString type = QString();

    switch (logType) {
        case DebugLogType:
            type = "debug";
            break;
        case InfoLogType:
            type = "info";
            break;
        case WarningLogType:
            type = "warning";
            break;
        case CriticalLogType:
            type = "critical";
            break;
        case FatalLogType:
            type = "fatal";
            break;
        case StatusLogType:
            type = "status";
            break;
        case ScriptingLogType:
            type = "scripting";
            break;
        default:
            type = "unknown";
            break;
    }

    return type;
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
        logWidget = createInstance(nullptr);
    }

    return logWidget;
}

/**
 * Creates a global instance of the class
 */
LogWidget * LogWidget::createInstance(QWidget *parent) {
    auto *logWidget = new LogWidget(parent);

#ifndef INTEGRATION_TESTS
    qApp->setProperty(
            "logWidget",
            QVariant::fromValue<LogWidget *>(logWidget));
#endif

    return logWidget;
}

/**
 * Custom log output
 */
void LogWidget::logMessageOutput(
        QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    LogType logType = LogType::DebugLogType;

    switch (type) {
        case QtDebugMsg:
//            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(),
//                    context.file, context.line, context.function);
            // only print debug messages in debug-mode
#ifdef QT_DEBUG
            fprintf(stderr, "Debug: %s\n", localMsg.constData());
#endif
            logType = LogType::DebugLogType;
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
        case QtInfoMsg:
            fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            logType = LogType::InfoLogType;
            break;
#endif
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            logType = LogType::WarningLogType;
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            logType = LogType::CriticalLogType;
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            logType = LogType::FatalLogType;
    }

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != Q_NULLPTR) {
        // handle logging as signal/slot to even more prevent crashes when
        // writing to the log-widget while the app is shutting down
        emit(mainWindow->log(logType, msg));
    }
#else
    Q_UNUSED(logType)
#endif

    // it's harder to debug a problem if we abort here
//    if (type == QtFatalMsg) {
//        abort();
//    }
}

/**
 * Logs to the log file if allowed
 *
 * @param logType
 * @param msg
 */
void LogWidget::logToFileIfAllowed(LogType logType, const QString& msg) {
    QSettings settings;
    if (settings.value("Debug/fileLogging").toBool()) {
        QFile logFile(Utils::Misc::logFilePath());
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Text |
                         QIODevice::Append)) {
            QTextStream out(&logFile);
            QDateTime dateTime = QDateTime::currentDateTime();
            QString typeStr = logTypeText(logType);
            QString text = QString("[%1] [%2]: %3\n").arg(
                    dateTime.toString("MMM dd hh:mm:ss").remove("."),
                    typeStr, msg);
            out << text;
            logFile.close();
        }
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

/**
 * Shows a context menu with more options
 *
 * @param pos
 */
void LogWidget::on_logTextEdit_customContextMenuRequested(const QPoint &pos) {
#ifndef INTEGRATION_TESTS
    QPoint globalPos = ui->logTextEdit->mapToGlobal(pos);
    QMenu *menu = ui->logTextEdit->createStandardContextMenu();

    menu->addSeparator();

    QString actionText = ui->buttonFrame->isHidden() ?
                         tr("Show options") : tr("Hide options");
    QAction *toggleOptionsAction = menu->addAction(actionText);
    QAction *clearLogAction = menu->addAction(tr("Clear log"));

    QAction *selectedItem = menu->exec(globalPos);
    if (selectedItem) {
        if (selectedItem == toggleOptionsAction) {
            ui->buttonFrame->setVisible(ui->buttonFrame->isHidden());
        } else if (selectedItem == clearLogAction) {
            on_clearButton_clicked();
        }
    }
#else
    Q_UNUSED(pos)
#endif
}

/**
 * In case that method gets ever called it should disable logging if an
 * object is destroyed
 *
 * @param obj
 */
void LogWidget::onDestroyed(QObject *obj) {
    Q_UNUSED(obj)
#ifndef INTEGRATION_TESTS
    qApp->setProperty("loggingEnabled", false);
#endif
}

/**
 * Event filters
 *
 * @param obj
 * @param event
 * @return
 */
bool LogWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        // hide the option frame on Escape key
        if (keyEvent->key() == Qt::Key_Escape) {
#ifndef INTEGRATION_TESTS
            ui->buttonFrame->hide();
#endif
            return false;
        }
    }

    return QFrame::eventFilter(obj, event);
}
