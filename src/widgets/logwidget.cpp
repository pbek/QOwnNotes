#include "logwidget.h"

#include <utils/misc.h>

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QMenu>
#include <QPointer>
#include <QScrollBar>

#include "mainwindow.h"
#include "services/settingsservice.h"

#ifndef INTEGRATION_TESTS
#include "ui_logwidget.h"
#endif

static QPointer<LogWidget> s_logWidget = nullptr;

LogWidget::LogWidget(QWidget *parent)
    : QFrame(parent)
#ifndef INTEGRATION_TESTS
      ,
      ui(new Ui::LogWidget)
#endif
{
#ifndef INTEGRATION_TESTS

    // static reference to us for use in the message handler
    s_logWidget = this;

    ui->setupUi(this);

    // drop some stuff that doesn't apply here
    disconnect(ui->logTextEdit, &QMarkdownTextEdit::zoomIn, nullptr, nullptr);
    disconnect(ui->logTextEdit, &QMarkdownTextEdit::zoomOut, nullptr, nullptr);
    disconnect(ui->logTextEdit, &QOwnNotesMarkdownTextEdit::urlClicked, nullptr, nullptr);

    // The order of zoomIn is opposite (in calls out and vice versa), to be consistent with other
    // edits
    connect(ui->logTextEdit, &QMarkdownTextEdit::zoomIn, this,
            [this] { static_cast<QPlainTextEdit *>(ui->logTextEdit)->zoomOut(-1); });
    connect(ui->logTextEdit, &QMarkdownTextEdit::zoomOut, this,
            [this] { static_cast<QPlainTextEdit *>(ui->logTextEdit)->zoomIn(-1); });

    // we use a different context menu here, not default one
    disconnect(ui->logTextEdit, &QPlainTextEdit::customContextMenuRequested, nullptr, nullptr);
    connect(ui->logTextEdit, &QPlainTextEdit::customContextMenuRequested, this,
            &LogWidget::on_logTextEdit_customContextMenuRequested);

    ui->buttonFrame->hide();
    const SettingsService settings;

    // init the log text edit search frame
    const bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();
    ui->logTextEdit->initSearchFrame(ui->logTextEditSearchFrame, darkMode);

    // turn off Markdown highlighting
    ui->logTextEdit->setHighlightingEnabled(false);
    ui->logTextEdit->disableSpellChecking();

    // load the dialog settings
    ui->debugCheckBox->setChecked(
        settings.value(QStringLiteral("LogWidget/debugLog"), false).toBool());
    ui->infoCheckBox->setChecked(
        settings.value(QStringLiteral("LogWidget/infoLog"), false).toBool());
    ui->warningCheckBox->setChecked(
        settings.value(QStringLiteral("LogWidget/warningLog"), true).toBool());
    ui->criticalCheckBox->setChecked(
        settings.value(QStringLiteral("LogWidget/criticalLog"), true).toBool());
    ui->fatalCheckBox->setChecked(
        settings.value(QStringLiteral("LogWidget/fatalLog"), true).toBool());
    ui->statusCheckBox->setChecked(
        settings.value(QStringLiteral("LogWidget/statusLog"), true).toBool());
    ui->scriptingCheckBox->setChecked(
        settings.value(QStringLiteral("LogWidget/scriptingLog"), true).toBool());

    // store the settings if the dialog is closed or the checkboxes are clicked
    connect(ui->debugCheckBox, &QCheckBox::clicked, this, &LogWidget::storeSettings);
    connect(ui->infoCheckBox, &QCheckBox::clicked, this, &LogWidget::storeSettings);
    connect(ui->warningCheckBox, &QCheckBox::clicked, this, &LogWidget::storeSettings);
    connect(ui->criticalCheckBox, &QCheckBox::clicked, this, &LogWidget::storeSettings);
    connect(ui->fatalCheckBox, &QCheckBox::clicked, this, &LogWidget::storeSettings);
    connect(ui->statusCheckBox, &QCheckBox::clicked, this, &LogWidget::storeSettings);
    connect(ui->scriptingCheckBox, &QCheckBox::clicked, this, &LogWidget::storeSettings);

    ui->logTextEdit->installEventFilter(this);
    ui->buttonFrame->installEventFilter(this);
#endif
}

LogWidget::~LogWidget() {
#ifndef INTEGRATION_TESTS
    delete ui;
#endif
}

/**
 * Returns the text of the log widget
 */
QString LogWidget::getLogText() const {
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
    SettingsService settings;
    settings.setValue(QStringLiteral("LogWidget/debugLog"), ui->debugCheckBox->isChecked());
    settings.setValue(QStringLiteral("LogWidget/infoLog"), ui->infoCheckBox->isChecked());
    settings.setValue(QStringLiteral("LogWidget/warningLog"), ui->warningCheckBox->isChecked());
    settings.setValue(QStringLiteral("LogWidget/criticalLog"), ui->criticalCheckBox->isChecked());
    settings.setValue(QStringLiteral("LogWidget/fatalLog"), ui->fatalCheckBox->isChecked());
    settings.setValue(QStringLiteral("LogWidget/statusLog"), ui->statusCheckBox->isChecked());
    settings.setValue(QStringLiteral("LogWidget/scriptingLog"), ui->scriptingCheckBox->isChecked());
#endif
}

/**
 * Adds a log entry
 */
void LogWidget::log(LogWidget::LogType logType, const QString &text) {
    // ignore "libpng sRGB profile", "QXcbConnection: XCB error: 8" and
    // "QFileSystemWatcher::removePaths" warnings
    if ((logType == WarningLogType || logType == InfoLogType) &&
        (text.contains(QLatin1String("libpng warning: iCCP: known incorrect sRGB profile")) ||
         text.contains(QLatin1String("fromIccProfile: failed minimal tag size sanity")) ||
         text.contains(QLatin1String("QXcbConnection: XCB error:")) ||
         text.contains(QLatin1String("failed to create compose table")) ||
         text.contains(QLatin1String("OpenType support missing for")) ||
         text.contains(QLatin1String("Could not find accessible on path:")) ||
         text.contains(QLatin1String("Accessible invalid: QAccessibleInterface")) ||
         text.contains(QLatin1String("Using QCharRef with an index pointing outside")) ||
         text.contains(QLatin1String("load glyph failed err=")) ||
         text.contains(QLatin1String("[Botan Error]  Invalid CBC padding")) ||
         text.contains(QLatin1String("Invalid version or not a cyphertext")) ||
         text.contains(QLatin1String("QTextCursor::setPosition: Position")) ||
         text.contains(
             QLatin1String("QFont::setPointSizeF: Point size <= 0")) ||    // we don't even use that
                                                                           // method directly
         text.contains(QLatin1String("QFont::setPointSize: Point size <= 0")) ||
         text.contains(QLatin1String(
             "scroll event from unregistered device")) ||    // when YubiKey is plugged in or out
         text.contains(QLatin1String("QFileSystemWatcher::removePaths: list is empty")))) {
        return;
    }

#ifndef INTEGRATION_TESTS
    // log to the log file
    logToFileIfAllowed(logType, text);

    // return if logging wasn't enabled or if widget is not visible
    if (!isVisible()) {
        return;
    }

    QString type = logTypeText(logType);
    QColor color = QColor(Qt::black);

#ifdef Q_OS_MAC
    // Try to fix crash when quitting the app with turned on log panel on macOS with Qt6 on Apple
    // Silicon https://github.com/pbek/QOwnNotes/issues/2912#issuecomment-3066625378
    const bool darkMode = QSettings().value(QStringLiteral("darkMode")).toBool();
#else
    const bool darkMode = SettingsService().value(QStringLiteral("darkMode")).toBool();
#endif

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
            color = QColor(darkMode ? Qt::yellow : Qt::darkBlue);
            break;
        case WarningLogType:
            if (!ui->warningCheckBox->isChecked()) {
                return;
            }

            // this is a "fix" for crashes that occur when a network goes away
            // and this message should be printed, I haven't managed to get
            // around this crash with other methods
            if (text.contains(QLatin1String("/org/freedesktop/NetworkManager/ActiveConnection"))) {
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
            color = QColor(0, darkMode ? 162 : 128, 0);
            break;
        case ScriptingLogType:
            if (!ui->scriptingCheckBox->isChecked()) {
                return;
            }
            // blue
            color = QColor(0, 102, 255);
            break;
    }

    const QDateTime dateTime = QDateTime::currentDateTime();
    //    text.prepend("[" + dateTime.toString("hh:mm:ss") + "] [" + type + "]
    //    "); text.append("\n");

    const QString html = QStringLiteral("<div style=\"color: %1\">[%2] [%3] %4</div>")
                             .arg(color.name(), dateTime.toString(QStringLiteral("hh:mm:ss")), type,
                                  text.toHtmlEscaped());

    QScrollBar *scrollBar = ui->logTextEdit->verticalScrollBar();

    // we want to scroll down later if the scrollbar is near the bottom
    const bool scrollDown = scrollBar->value() >= (scrollBar->maximum() - scrollBar->singleStep());

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
    QString type;

    switch (logType) {
        case DebugLogType:
            type = QStringLiteral("debug");
            break;
        case InfoLogType:
            type = QStringLiteral("info");
            break;
        case WarningLogType:
            type = QStringLiteral("warning");
            break;
        case CriticalLogType:
            type = QStringLiteral("critical");
            break;
        case FatalLogType:
            type = QStringLiteral("fatal");
            break;
        case StatusLogType:
            type = QStringLiteral("status");
            break;
        case ScriptingLogType:
            type = QStringLiteral("scripting");
            break;
        default:
            type = QStringLiteral("unknown");
            break;
    }

    return type;
}
/**
 * Custom log output
 */
void LogWidget::logMessageOutput(QtMsgType type, const QMessageLogContext &context,
                                 const QString &msg) {
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
        case QtInfoMsg:
            fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file,
                    context.line, context.function);
            logType = LogType::InfoLogType;
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file,
                    context.line, context.function);
            logType = LogType::WarningLogType;
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file,
                    context.line, context.function);
            logType = LogType::CriticalLogType;
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file,
                    context.line, context.function);
            logType = LogType::FatalLogType;
    }

#ifndef INTEGRATION_TESTS

    if (s_logWidget) {
        // Use auto connection to handle the case if a message is coming in from a different thread.
        QMetaObject::invokeMethod(s_logWidget, "log", Qt::AutoConnection,
                                  Q_ARG(LogWidget::LogType, logType), Q_ARG(QString, msg));
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
void LogWidget::logToFileIfAllowed(LogType logType, const QString &msg) {
    // There was a segmentation fault when quitting the application on ARM macOS
    // when using SettingsService, see https://github.com/pbek/QOwnNotes/issues/3290
#if defined(Q_OS_MACOS) && defined(Q_PROCESSOR_ARM)
    QSettings settings;
#else
    SettingsService settings;
#endif

    if (settings.value(QStringLiteral("Debug/fileLogging")).toBool()) {
        QFile logFile(Utils::Misc::logFilePath());
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            QTextStream out(&logFile);
            QDateTime dateTime = QDateTime::currentDateTime();
            QString typeStr = logTypeText(logType);
            QString text = QStringLiteral("[%1] [%2]: %3\n")
                               .arg(dateTime.toString(QStringLiteral("MMM dd hh:mm:ss"))
                                        .remove(QStringLiteral(".")),
                                    typeStr, msg);
            out << text;
            logFile.close();
        }
    }
}

/**
 * Clears all log entries
 */
void LogWidget::on_clearButton_clicked() {
#ifndef INTEGRATION_TESTS
    ui->logTextEdit->clear();
#endif
}

/**
 * Shows a context menu with more options
 *
 * @param pos
 */
void LogWidget::on_logTextEdit_customContextMenuRequested(QPoint pos) {
#ifndef INTEGRATION_TESTS
    QPoint globalPos = ui->logTextEdit->mapToGlobal(pos);
    QMenu *menu = ui->logTextEdit->createStandardContextMenu();

    menu->addSeparator();

    QString actionText = ui->buttonFrame->isHidden() ? tr("Show options") : tr("Hide options");
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
