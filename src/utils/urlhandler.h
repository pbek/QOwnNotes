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

    static bool isUrlSchemeLocal(const QUrl& url);

    void openUrl(QString urlString, bool openInNewTab = false);

   private:
    static void handleNoteIdUrl(const QString& urlString, bool openInNewTab = false);
    static void handleNoteUrl(const QString& urlString, const QString& fragment,
                              bool openInNewTab = false);
    void handleCheckboxUrl(const QString& urlString);
    static void handleNextcloudDeckUrl(const QString& urlString);
    static void handleFileUrl(QString urlString);
    static void handleFileAttachmentUrl(QString urlString);
};
