#include "urlhandler.h"
#include "mainwindow.h"
#include "widgets/qownnotesmarkdowntextedit.h"
#include "entities/note.h"
#include "entities/notefolder.h"
#include "entities/notesubfolder.h"
#include "utils/gui.h"
#include "widgets/notesubfoldertree.h"

#include <QDesktopServices>
#include <QFileInfo>

UrlHandler::UrlHandler(MainWindow *mainWindow)
    : _mw(mainWindow)
{}

bool UrlHandler::isUrlSchemeLocal(const QUrl &url)
{
    const QString scheme = url.scheme();
    return scheme == QLatin1String("note")
    || scheme == QLatin1String("noteid")
    || scheme == QLatin1String("task")
    || scheme == QLatin1String("checkbox")
    || (scheme == QLatin1String("file") &&
        Note::fileUrlIsNoteInCurrentNoteFolder(url));
}

void UrlHandler::openUrl(QString urlString)
{
    /* examples:
     * - <note://MyNote> opens the note "MyNote"
     * - <note://my-note-with-spaces-in-the-name> opens the note "My Note with
     * spaces in the name"
     * - <https://www.qownnotes.org> opens the web page
     * - <file:///path/to/my/note/folder/subfolder/My%20note.pdf> opens the note
     * "My note" in the subfolder "subfolder"
     * - <file:///path/to/my/file/QOwnNotes.pdf> opens the file
     * "/path/to/my/file/QOwnNotes.pdf" if the operating system supports that
     * handler
     */
    if (urlString.isEmpty()) {
        return;
    }

    QUrl url = QUrl(urlString);
    const bool isExistingNoteFileUrl = Note::fileUrlIsExistingNoteInCurrentNoteFolder(url);
    const bool isNoteFileUrl = Note::fileUrlIsNoteInCurrentNoteFolder(url);
    const QString scheme = url.scheme();

    UrlHandler *handler = nullptr;

    bool urlWasNotValid = !QOwnNotesMarkdownTextEdit::isValidUrl(urlString);
    if (urlWasNotValid) {
        urlString = _mw->getCurrentNote().getFileURLFromFileName(urlString, true);
    }

    if (urlString.startsWith(QStringLiteral("file://..")) && !isExistingNoteFileUrl) {
        handleFileUrl(urlString);
    } else if (urlString.startsWith(QStringLiteral("file://attachments"))) {
        handleFileAttachmentUrl(urlString);
    } else if (scheme == QStringLiteral("noteid")) {
        handleNoteIdUrl(urlString);
    } else if (scheme == QStringLiteral("note") || isNoteFileUrl) {
        handleNoteUrl(urlString);
    } else if (scheme == QStringLiteral("task")) {
        _mw->openTodoDialog(url.host());
    } else if (scheme == QStringLiteral("checkbox")) {
        handleCheckboxUrl(urlString);
    } else if (scheme == QStringLiteral("file") && urlWasNotValid) {
        QDesktopServices::openUrl(QUrl(urlString));
    }
}

void UrlHandler::handleNoteIdUrl(QString urlString)
{
    static const QRegularExpression re(QStringLiteral(R"(^noteid:\/\/note-(\d+)$)"));
    QRegularExpressionMatch match = re.match(urlString);

    if (match.hasMatch()) {
        int noteId = match.captured(1).toInt();
        Note note = Note::fetch(noteId);
        if (note.isFetched()) {
            // set current note
            _mw->setCurrentNote(std::move(note));
        }
    } else {
        qWarning() << "NoteIdUrlHandler malformed url: " << urlString;
    }
}

void UrlHandler::handleNoteUrl(QString urlString) {
    Note note;
    const QUrl url(urlString);
    const auto &currentNote = _mw->getCurrentNote();

    QString fragment = url.fragment();
    if (!QOwnNotesMarkdownTextEdit::isValidUrl(urlString)) {
        fragment = Note::getURLFragmentFromFileName(urlString);
    }

    const bool isNoteFileUrl = Note::fileUrlIsNoteInCurrentNoteFolder(url);

    if (isNoteFileUrl) {
        note = Note::fetchByFileUrl(url);
    } else {
        // try to fetch a note from the url string
        note = Note::fetchByUrlString(urlString);
    }

    // does this note really exist?
    if (note.isFetched()) {
        // set current note
        _mw->setCurrentNote(std::move(note));

        // jump to the Markdown heading in the note that is represented by the url fragment
        if (!fragment.isEmpty()) {
            _mw->doSearchInNote("\"## " + fragment + "\"");
            _mw->activeNoteTextEdit()->searchWidget()->deactivate();
        }
    } else {
        QString fileName;
        QUrl filePath;

        if (!isNoteFileUrl) {
            // if the name of the linked note only consists of numbers we cannot
            // use host() to get the filename, it would get converted to an
            // ip-address
            static const QRegularExpression re(QStringLiteral(R"(^\w+:\/\/(\d+)$)"));
            QRegularExpressionMatch match = re.match(urlString);
            fileName =
                match.hasMatch() ? match.captured(1) : url.host();

            // try to generate a useful title for the note
            fileName = Utils::Misc::toStartCase(
                        fileName.replace(QStringLiteral("_"), QStringLiteral(" ")));
        } else {
            fileName = url.fileName();
            filePath = url.adjusted(QUrl::RemoveFilename);
        }

        // remove file extension
        QFileInfo fileInfo(fileName);
        fileName = fileInfo.baseName();
        QString relativeFilePath =
                Note::fileUrlInCurrentNoteFolderToRelativePath(filePath);
        QString currentNoteRelativeSubFolderPath =
            currentNote.getNoteSubFolder().relativePath();

        // remove the current relative sub-folder path from the relative path
        // of the future note to be able to create the correct path afterwards
        if (!currentNoteRelativeSubFolderPath.isEmpty()) {
            relativeFilePath.remove(QRegularExpression(
                "^" +
                QRegularExpression::escape(currentNoteRelativeSubFolderPath) +
                "\\/"));
        }

        // Open attachments with extensions that are used for notes externally
        if (relativeFilePath.contains(QStringLiteral("attachments"))) {
            if (QDesktopServices::openUrl(url)) {
                return;
            }
        }

        if (!relativeFilePath.isEmpty() && !NoteFolder::isCurrentHasSubfolders()) {
            Utils::Gui::warning(
                nullptr, QObject::tr("Note was not found"),
                QObject::tr("Could not find note.<br />Unable to automatically "
                    "create note at location, because subfolders are "
                    "disabled for the current note folder."),
                "cannot-create-note-not-has-subfolders");
            return;
        }

        QString promptQuestion;

        if (relativeFilePath.isEmpty()) {
            promptQuestion = QObject::tr("Note was not found, create new note "
                                "<strong>%1</strong>?")
                    .arg(fileName);
        } else {
            promptQuestion = QObject::tr("Note was not found, create new note "
                                "<strong>%1</strong> at path <strong>%2</strong>?")
                    .arg(fileName, relativeFilePath);
        }

        // ask if we want to create a new note if note wasn't found
        if (Utils::Gui::questionNoSkipOverride(nullptr, QObject::tr("Note was not found"),
                                                promptQuestion,
                                                QStringLiteral("open-url-create-note")) == QMessageBox::Yes) {

            NoteSubFolder noteSubFolder = currentNote.getNoteSubFolder();
            bool subFolderCreationFailed(false);

            if (!relativeFilePath.isEmpty()) {
                for (const QString& folderName : relativeFilePath.split("/")) {
                    if (folderName.isEmpty()) {
                        break;
                    }

                    NoteSubFolder subFolder = NoteSubFolder::fetchByNameAndParentId(folderName, noteSubFolder.getId());
                    if (!subFolder.isFetched()) {
                        _mw->createNewNoteSubFolder(folderName);
                        noteSubFolder = NoteSubFolder::fetchByNameAndParentId(folderName, noteSubFolder.getId());
                        if (!noteSubFolder.isFetched()) {
                            qWarning() << "Failed to create subfolder: " << folderName <<
                                            "when attempting to create path: " << relativeFilePath;
                            subFolderCreationFailed = true;
                            break;
                        }
                    } else {
                        noteSubFolder = subFolder;
                    }

                    noteSubFolder.setAsActive();
                }
            }

            if (!subFolderCreationFailed) {
                if (!relativeFilePath.isEmpty()) {
                    _mw->noteSubFolderTree()->reset();
                    _mw->jumpToNoteSubFolder(noteSubFolder.getId());
                }
                _mw->createNewNote(fileName, false);
            } else {
                Utils::Gui::warning(
                    nullptr, QObject::tr("Failed to create note"),
                    QObject::tr("Note creation failed"),
                    "note-create-failed");
            }
            return;
        }
    }
}

void UrlHandler::handleCheckboxUrl(QString urlString)
{
    const auto text = _mw->noteTextEdit()->toPlainText();
    const QUrl url(urlString);

    int index = url.host().midRef(1).toInt();
#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
    QRegExp re(R"((^|\n)\s*[-*+]\s\[([xX ]?)\])", Qt::CaseInsensitive);
#else
    static const QRegularExpression re(R"((^|\n)\s*[-*+]\s\[([xX ]?)\])", QRegularExpression::CaseInsensitiveOption);
#endif
    int pos = 0;
    while (true) {

#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
        pos = re.indexIn(text, pos);
#else
        QRegularExpressionMatch match;
        pos = text.indexOf(re, pos, &match);
#endif
        if (pos == -1)    // not found
            return;
        auto cursor = _mw->noteTextEdit()->textCursor();

#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
        int matchedLength = re.matchedLength();
        cursor.setPosition(pos + re.matchedLength() - 1);
#else
        int matchedLength = match.capturedLength();
        qDebug() << __func__ << "match.capturedLength(): " << match.capturedLength();
        cursor.setPosition(pos + match.capturedLength() - 1);
#endif
        if (cursor.block().userState() ==
            MarkdownHighlighter::HighlighterState::List) {
            if (index == 0) {

#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
                auto ch = re.cap(2);
#else
                auto ch = match.captured(2);
#endif
                if (ch.isEmpty())
                    cursor.insertText(QStringLiteral("x"));
                else {
                    cursor.movePosition(QTextCursor::PreviousCharacter,
                                        QTextCursor::KeepAnchor);
                    cursor.insertText(ch == QStringLiteral(" ")
                                            ? QStringLiteral("x")
                                            : QStringLiteral(" "));
                }

                // refresh instantly
                _mw->refreshNotePreview();
                break;
            }
            --index;
        }
        pos += matchedLength;
    }
}

void UrlHandler::handleFileUrl(QString urlString)
{
    QString windowsSlash = QString();

#ifdef Q_OS_WIN32
    // we need another slash for Windows
    windowsSlash = QStringLiteral("/");
#endif

    urlString.replace(QLatin1String("file://.."),
                        QStringLiteral("file://") + windowsSlash +
                            NoteFolder::currentLocalPath() +
                            QStringLiteral("/.."));

    QDesktopServices::openUrl(QUrl(urlString));
}

void UrlHandler::handleFileAttachmentUrl(QString urlString)
{
    QString windowsSlash = QString();

#ifdef Q_OS_WIN32
    // we need another slash for Windows
    windowsSlash = QStringLiteral("/");
#endif

    urlString.replace(QLatin1String("file://attachments"),
                        QStringLiteral("file://") + windowsSlash +
                            NoteFolder::currentLocalPath() +
                            QStringLiteral("/attachments"));

    QDesktopServices::openUrl(QUrl(urlString));
    return;
}
