#include "kdesharemenu.h"

#include <Purpose/AlternativesModel>
#include <Purpose/Menu>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QMimeDatabase>
#include <QUrl>

namespace {
// Returned by a share job if the user canceled it (KJob::KilledJobError)
const int canceledError = 1;
}    // namespace

KdeShareMenu::KdeShareMenu(const QString &title, QWidget *parent)
    : QObject(parent), _menu(new Purpose::Menu(parent)) {
    _menu->setTitle(title);
    _menu->setIcon(QIcon::fromTheme(QStringLiteral("document-share")));
    _menu->menuAction()->setVisible(false);
    _menu->model()->setPluginType(QStringLiteral("Export"));

    connect(_menu, &Purpose::Menu::aboutToShare, this, &KdeShareMenu::aboutToShare);
    connect(_menu, &Purpose::Menu::finished, this,
            [this](const QJsonObject &output, int error, const QString &errorMessage) {
                if (error == canceledError) {
                    return;
                }

                if (error != 0) {
                    emit finished(errorMessage.isEmpty()
                                      ? tr("Sharing failed")
                                      : tr("Sharing failed: %1").arg(errorMessage),
                                  true);
                    return;
                }

                // E.g. an upload returns the link to the file
                const QString url = output.value(QStringLiteral("url")).toString();
                emit finished(
                    url.isEmpty() ? tr("Sharing finished") : tr("Sharing finished: %1").arg(url),
                    false);
            });
}

QMenu *KdeShareMenu::menu() const { return _menu; }

bool KdeShareMenu::setFiles(const QStringList &filePaths) {
    if (filePaths.isEmpty()) {
        _menu->clear();
        _menu->menuAction()->setVisible(false);
        return false;
    }

    // Reloads the targets that accept the files
    _menu->model()->setInputData(inputData(filePaths));

    const bool hasTargets = !_menu->isEmpty();
    _menu->menuAction()->setVisible(hasTargets);
    return hasTargets;
}

QJsonObject KdeShareMenu::inputData(const QStringList &filePaths) {
    const QMimeDatabase mimeDatabase;
    QJsonArray urls;
    QString mimeType;
    bool allText = true;

    for (const QString &filePath : filePaths) {
        urls.append(QUrl::fromLocalFile(filePath).toString());

        const QMimeType fileMimeType = mimeDatabase.mimeTypeForFile(filePath);
        allText = allText && fileMimeType.inherits(QStringLiteral("text/plain"));

        if (mimeType.isEmpty()) {
            mimeType = fileMimeType.name();
        } else if (mimeType != fileMimeType.name()) {
            // Targets need to accept all files
            mimeType =
                allText ? QStringLiteral("text/plain") : QStringLiteral("application/octet-stream");
        }
    }

    QJsonObject data;
    data.insert(QStringLiteral("urls"), urls);
    data.insert(QStringLiteral("mimeType"), mimeType);

    if (filePaths.size() == 1) {
        data.insert(QStringLiteral("title"), QFileInfo(filePaths.first()).completeBaseName());
    }

    return data;
}
