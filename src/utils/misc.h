/*
 * Copyright (c) 2014-2025 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#pragma once

#include <QHash>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QVector>

struct TerminalCmd;
class QFile;
class QDataStream;
class QPrinter;
class QJSValue;

/*  Miscellaneous functions that can be useful */
namespace Utils {
namespace Misc {
struct ExternalImageHashItem {
    QString imageTag;
    int imageWidth;
};

typedef QHash<QString, ExternalImageHashItem> ExternalImageHash;
}    // namespace Misc

// this adds const to non-const objects (like cxx-17 std::as_const)
// always use it with cxx-11 range-for when the container being iterated over is
// a temporary or non-const
template <typename T>
Q_DECL_CONSTEXPR typename std::add_const<T>::type &asConst(T &t) noexcept {
    return t;
}
// prevent rvalue arguments:
template <typename T>
void asConst(const T &&) = delete;
}    // namespace Utils

Q_DECLARE_METATYPE(Utils::Misc::ExternalImageHash *)

namespace Utils {
namespace Misc {

struct SearchEngine {
    QString name;
    QString searchUrl;
    int id;
};

void openPath(const QString &absolutePath);
void openFolderSelect(const QString &absolutePath);
QString removeIfStartsWith(QString text, const QString &removeString);
QString removeIfEndsWith(QString text, const QString &removeString);
QString prependIfDoesNotStartWith(QString text, const QString &startString);
QString appendIfDoesNotEndWith(QString text, const QString &endString);
bool startDetachedProcess(const QString &executablePath,
                          const QStringList &parameters = QStringList(),
                          QString workingDirectory = QLatin1String(""));
bool openFilesWithApplication(const QString &executablePath,
                              const QStringList &files = QStringList(),
                              QString workingDirectory = QLatin1String(""));
QString shorten(QString text, int length, const QString &sequence = QStringLiteral("..."));
QString cycleTextCase(const QString &text);
QString toSentenceCase(const QString &text);
QString toStartCase(const QString &text);
QString defaultNotesPath();
char dirSeparator();
void waitMsecs(int msecs);
QString portableDataPath();
bool isInPortableMode();
bool isAppImage();
QString applicationPath();
QString prependPortableDataPathIfNeeded(QString path, bool ifNotEmptyOnly = false);
QString makePathRelativeToPortableDataPathIfNeeded(QString path);
QString htmlToMarkdown(QString text);
QString parseTaskList(const QString &html, bool clickable);
QByteArray startSynchronousProcess(const QString &executablePath, const QStringList &parameters,
                                   const QByteArray &data = QByteArray(),
                                   const QString &workingDirectory = "");
bool startSynchronousResultProcess(TerminalCmd &cmd);
QList<QObject *> getParents(QObject *object);
QString appDataPath();
QString logFilePath();
QString transformLineFeeds(QString text);
QString replaceOwnCloudText(QString text, bool useShortText = false);
void restartApplication();
QString appendSingleAppInstanceTextIfNeeded(QString text = "");
void needRestart();
bool downloadUrlToFile(const QUrl &url, QFile *file);
QByteArray downloadUrl(const QUrl &url, bool usePost = false, QByteArray postData = nullptr);
QByteArray downloadUrlWithStatusCode(const QUrl &url, int &returnStatusCode, bool usePost = false,
                                     QByteArray postData = nullptr);
QString genericCSS();
QHash<int, SearchEngine> getSearchEnginesHashMap();
int getDefaultSearchEngineId();
void presetDisableAutomaticUpdateDialog();
QList<int> getSearchEnginesIds();
QDataStream &dataStreamWrite(QDataStream &os, const QPrinter &printer);
QDataStream &dataStreamRead(QDataStream &is, QPrinter &printer);
void storePrinterSettings(QPrinter *printer, const QString &settingsKey);
void loadPrinterSettings(QPrinter *printer, const QString &settingsKey);
bool isNoteEditingAllowed();
bool areMenuIconsHidden();
bool useInternalExportStylingForPreview();
bool isSocketServerEnabled();
bool isWebAppSupportEnabled();
bool isRestoreCursorPosition();
QString unescapeHtml(QString html, bool soft = false);
QString htmlspecialchars(QString text);
void printInfo(const QString &text);
bool doAutomaticNoteFolderDatabaseClosing();
bool isNoteListPreview();
bool isEnableNoteTree();
QString indentCharacters();
int indentSize();
QString toHumanReadableByteSize(qint64 size);
QString prepareDebugInformationLine(const QString &headline, QString data,
                                    bool withGitHubLineBreaks = true,
                                    const QString &typeText = QString());
QString generateDebugInformation(bool withGitHubLineBreaks = true);
bool regExpInListMatches(const QString &text, const QStringList &regExpList);
bool isDarkModeIconTheme();
void transformNextcloudPreviewImages(QString &html, int maxImageWidth,
                                     ExternalImageHash *externalImageHash);
void transformRemotePreviewImages(QString &html, int maxImageWidth,
                                  ExternalImageHash *externalImageHash);
QString remotePreviewImageTagToInlineImageTag(QString imageTag, int &imageWidth);
QString createUuidString();
QString localDictionariesPath();
QByteArray generateFileSha1Signature(const QString &path);
bool isSameFile(const QString &path1, const QString &path2);
QString generateRandomString(int length = 8);
QString makeFileNameRandom(const QString &fileName, const QString &overrideSuffix = "");
QString findAvailableFileName(const QString &filePath, const QString &directoryPath,
                              const QString &overrideSuffix = "");
bool fileNameExists(const QString &fileName, const QString &directoryPath);
QString fileNameForPath(const QString &filePath);
QString rstrip(const QString &str);
bool isPreviewUseEditorStyles();
QString previewFontString();
QString previewCodeFontString();
bool fileExists(const QString &path);
QString removeAcceleratorMarker(const QString &label_);
QString fileExtensionForMimeType(const QString &mimeType);
QByteArray friendlyUserAgentString();
QLatin1String platform();
void switchToDarkOrLightMode(bool darkMode);
void switchToDarkMode();
void switchToLightMode();
void unescapeEvernoteImportText(QString &content);
void transformEvernoteImportText(QString &content, bool withCleanup = false);
void cleanupEvernoteImportText(QString &content);
QString testEvernoteImportText(const QString &data);
void logToFileIfAllowed(QtMsgType msgType, const QString &msg);
QString logMsgTypeText(QtMsgType logType);
bool isSimilar(const QString &str1, const QString &str2, int threshold = 3);
QString getBaseUrlFromUrlString(const QString &urlString, bool withBasePath = false);
QString createAbsolutePathsInHtml(const QString &html, const QString &url);
int getPreviewRefreshDebounceTime();
int getMaximumNoteFileSize();
QString encodeFilePath(const QString &filePath);
QString detectFileFormat(const QString &text);
QString jsValueToJsonString(const QJSValue &value);
}    // namespace Misc
}    // namespace Utils

QDataStream &operator<<(QDataStream &os, const QPrinter &printer);
QDataStream &operator>>(QDataStream &is, QPrinter &printer);
