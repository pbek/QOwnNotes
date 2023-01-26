#pragma once

#include <QString>
#include <QUrl>

class MainWindow;

/*
 * Handles note urls
 *
 * examples:
 * - <note://MyNote> opens the note "MyNote"
 * - <note://my-note-with-spaces-in-the-name> opens the note "My Note with
 * spaces in the name"
 */
class UrlHandler {
   public:
    UrlHandler();

    static bool isUrlSchemeLocal(const QUrl &url);

    void openUrl(QString urlString);

   private:
    void handleNoteIdUrl(QString urlString);
    void handleNoteUrl(QString urlString, const QString &fragment);
    void handleCheckboxUrl(QString urlString);
    void handleFileUrl(QString urlString);
    void handleFileAttachmentUrl(QString urlString);
};
