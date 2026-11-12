#include "screenshotscenario.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QScrollBar>
#include <QSettings>
#include <QTabWidget>
#include <QTextDocument>
#include <QTimer>
#include <QTreeWidget>

#include "dialogs/passworddialog.h"
#include "entities/notefolder.h"
#include "mainwindow.h"

void ScreenshotScenario::initialize(MainWindow *window) {
    const QString scene = qEnvironmentVariable("QOWNNOTES_SCREENSHOT_SCENE");
    const QString root = QDir(qEnvironmentVariable("QOWNNOTES_SCREENSHOT_ROOT")).canonicalPath();
    if (scene.isEmpty() || root.isEmpty()) {
        return;
    }
    // Never automate a personal profile, even if the environment was accidentally inherited.
    const QString prefix = root + QLatin1Char('/');
    if (!QFileInfo(QSettings().fileName()).canonicalFilePath().startsWith(prefix) ||
        !QDir(NoteFolder::currentLocalPath()).canonicalPath().startsWith(prefix) ||
        !QFile::exists(root + QStringLiteral("/screenshot-profile"))) {
        return;
    }

    auto ready = [root]() {
        QFile file(root + QStringLiteral("/scene-ready"));
        if (file.open(QIODevice::WriteOnly)) {
            file.write("ready\n");
        }
    };
    QTimer::singleShot(2000, window, [window, scene, root, ready]() {
        if (scene == QStringLiteral("document-overview")) {
            auto *edit = window->findChild<QPlainTextEdit *>(QStringLiteral("noteTextEdit"));
            if (!edit) {
                return;
            }
            // Show a viewport in the middle of the long note rather than at its beginning.
            QTimer::singleShot(500, window, [edit, ready]() {
                auto *scroll = edit->verticalScrollBar();
                scroll->setValue(scroll->maximum() / 2);
                QTimer::singleShot(500, edit, ready);
            });
            return;
        }
        if (scene == QStringLiteral("backlinks")) {
            auto *timer = new QTimer(window);
            timer->setInterval(200);
            QObject::connect(timer, &QTimer::timeout, window, [window, timer, ready]() {
                auto *tree = window->findChild<QTreeWidget *>(QStringLiteral("backlinkWidget"));
                auto *tabs = window->findChild<QTabWidget *>(QStringLiteral("navigationTabWidget"));
                auto *tab = window->findChild<QWidget *>(QStringLiteral("backlinkTab"));
                if (!tree || !tabs || !tab || tree->topLevelItemCount() < 3 ||
                    tabs->indexOf(tab) < 0) {
                    return;
                }
                tabs->setCurrentWidget(tab);
                tree->expandAll();
                timer->stop();
                timer->deleteLater();
                ready();
            });
            timer->start();
            return;
        }
        if (scene == QStringLiteral("export-print")) {
            // Use only the PDF paint engine. No system printers or native print dialog.
            auto *printer = new QPrinter(QPrinter::HighResolution);
            printer->setOutputFormat(QPrinter::PdfFormat);
            printer->setOutputFileName(root + QStringLiteral("/demo.pdf"));
            auto *preview = new QPrintPreviewDialog(printer, window);
            preview->setAttribute(Qt::WA_DeleteOnClose);
            preview->setWindowTitle(QStringLiteral("PDF export preview"));
            auto *document = new QTextDocument(preview);
            Note note = window->getCurrentNote();
            document->setHtml(note.toMarkdownHtml(NoteFolder::currentLocalPath(), 800, true));
            QObject::connect(preview, &QPrintPreviewDialog::paintRequested, document,
                             [document, ready](QPrinter *target) {
                                 document->print(target);
                                 ready();
                             });
            QObject::connect(preview, &QObject::destroyed, window, [printer]() { delete printer; });
            preview->show();
            return;
        }
        if (scene == QStringLiteral("encrypted") || scene == QStringLiteral("encrypted-unlocked")) {
            // Exercise the real encryption action and password dialog inside the demo profile.
            auto *passwordTimer = new QTimer(window);
            passwordTimer->setInterval(100);
            QObject::connect(passwordTimer, &QTimer::timeout, window, [window, passwordTimer]() {
                for (auto *dialog : window->findChildren<PasswordDialog *>()) {
                    if (!dialog->isVisible()) {
                        continue;
                    }
                    for (auto *edit : dialog->findChildren<QLineEdit *>()) {
                        edit->setText(QStringLiteral("screenshot-demo-password"));
                    }
                    dialog->accept();
                    passwordTimer->stop();
                    passwordTimer->deleteLater();
                    break;
                }
            });
            passwordTimer->start();
            auto *action = window->findChild<QAction *>(QStringLiteral("action_Encrypt_note"));
            if (!action) {
                return;
            }
            action->trigger();
            QTimer::singleShot(1000, window, [window, scene, ready]() {
                Note note = window->getCurrentNote();
                note.refetch();
                if (!note.hasEncryptedNoteText()) {
                    return;
                }
                if (scene == QStringLiteral("encrypted-unlocked")) {
                    auto *editAction =
                        window->findChild<QAction *>(QStringLiteral("actionEdit_encrypted_note"));
                    if (!editAction || !note.canDecryptNoteText()) {
                        return;
                    }
                    editAction->trigger();
                } else {
                    note.setCryptoPassword(QString());
                    note.store();
                    window->setCurrentNote(note);
                }
                QTimer::singleShot(500, window, ready);
            });
            return;
        }
        // Wait for real network responses rather than photographing loading overlays.
        if (scene == QStringLiteral("versioning")) {
            // The startup action can run before the initial note selection is restored.
            auto *action = window->findChild<QAction *>(QStringLiteral("actionShow_versions"));
            if (action) {
                action->trigger();
            }
        }
        auto *timer = new QTimer(window);
        timer->setInterval(200);
        QObject::connect(timer, &QTimer::timeout, window, [window, scene, timer, ready]() {
            const QString listName = scene == QStringLiteral("trash")
                                         ? QStringLiteral("trashListWidget")
                                         : QStringLiteral("versionListWidget");
            if (scene == QStringLiteral("trash") || scene == QStringLiteral("versioning")) {
                auto *list = window->findChild<QListWidget *>(listName);
                if (!list || !list->isVisible() || list->count() < 3) {
                    return;
                }
                list->setCurrentRow(0);
            } else if (scene == QStringLiteral("todo") ||
                       scene == QStringLiteral("nextcloud-deck")) {
                auto *tree = window->findChild<QTreeWidget *>(
                    scene == QStringLiteral("todo") ? QStringLiteral("todoItemTreeWidget")
                                                    : QStringLiteral("cardItemTreeWidget"));
                if (!tree || !tree->isVisible() || tree->topLevelItemCount() < 3) {
                    return;
                }
                tree->setCurrentItem(tree->topLevelItem(0));
            } else if (scene != QStringLiteral("portable")) {
                return;
            }
            timer->stop();
            timer->deleteLater();
            ready();
        });
        timer->start();
    });
}
