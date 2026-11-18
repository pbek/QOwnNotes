#include "directorycopy.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QTemporaryDir>

namespace {
bool copyContents(const QString &sourcePath, const QString &destinationPath, QString *error) {
    const QDir source(sourcePath);
    if (!source.isReadable()) {
        *error = QObject::tr("Cannot read directory: %1").arg(sourcePath);
        return false;
    }
    const auto entries =
        source.entryInfoList(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
    for (const auto &entry : entries) {
        const QString target = QDir(destinationPath).filePath(entry.fileName());
        if (entry.isSymLink() || (!entry.isDir() && !entry.isFile())) {
            *error = QObject::tr("Cannot copy symbolic links or special files: %1")
                         .arg(entry.filePath());
            return false;
        }
        if (entry.isDir()) {
            if (!QDir().mkdir(target)) {
                *error = QObject::tr("Cannot create directory: %1").arg(target);
                return false;
            }
            if (!copyContents(entry.filePath(), target, error)) {
                return false;
            }
        } else if (!QFile::copy(entry.filePath(), target)) {
            *error = QObject::tr("Cannot copy file: %1").arg(entry.filePath());
            return false;
        }
    }
    return true;
}
}    // namespace

bool Utils::copyDirectory(const QString &sourcePath, const QString &destinationPath,
                          QString *error) {
    const QFileInfo source(sourcePath);
    const QFileInfo destination(destinationPath);
    const QString canonicalSource = source.canonicalFilePath();
    const QString canonicalParent = destination.dir().canonicalPath();
    const QString target = QDir(canonicalParent).filePath(destination.fileName());
    if (!source.isDir() || source.isSymLink() || canonicalSource.isEmpty() ||
        canonicalParent.isEmpty() || destination.exists() || destination.isSymLink() ||
        target == canonicalSource || target.startsWith(canonicalSource + QLatin1Char('/'))) {
        *error = QObject::tr("Cannot copy folder to an existing or invalid destination: %1")
                     .arg(destinationPath);
        return false;
    }
    // Stage beside the destination so a failed copy never leaves a partially copied folder.
    QTemporaryDir staging(QDir(canonicalParent).filePath(QStringLiteral(".qownnotes-copy-XXXXXX")));
    if (!staging.isValid()) {
        *error = QObject::tr("Cannot create temporary directory in: %1").arg(canonicalParent);
        return false;
    }
    if (!copyContents(canonicalSource, staging.path(), error)) {
        return false;
    }
    if (!QDir().rename(staging.path(), target)) {
        *error = QObject::tr("Cannot create copied folder: %1").arg(destinationPath);
        return false;
    }
    staging.setAutoRemove(false);
    return true;
}
