#include "filedialog.h"

#include <QDebug>
#include <QSettings>

FileDialog::FileDialog(const QString& name) {
    if (!name.isEmpty()) {
        setObjectName(QStringLiteral("FileDialog-") + name);
        _generalSettingsKey = QStringLiteral("FileDialog/LastPath");
        _settingsKey = _generalSettingsKey + "-" + name;

        QSettings settings;
        QString path = settings.value(_settingsKey).toString();
        QFileInfo fileInfo(path);

        // if there are problems with the directory path use path from any
        // FileDialog
        if (!fileInfo.isDir() || !fileInfo.isReadable()) {
            path = settings.value(_generalSettingsKey).toString();
        }

        fileInfo = QFileInfo(path);

        //        qDebug() << __func__ << " - 'path': " << path;

        // if there are still problems with the directory path use the home
        // directory
        if (!fileInfo.isDir() || !fileInfo.isReadable()) {
            path = QDir::homePath();
        }

        setDirectory(path);

        // store the directory for the next time the dialog opens
        connect(this, SIGNAL(fileSelected(QString)), this, SLOT(storeDirectory(QString)));
        connect(this, SIGNAL(filesSelected(QStringList)), this, SLOT(storeDirectory(QStringList)));
    }
}

/**
 * Stores the directory for the next time the dialog opens
 *
 * @param path
 */
void FileDialog::storeDirectory(QString path) {
    QSettings settings;
    QFileInfo fileInfo(path);

    // get the directory path from the path if it is not a directory
    // we don't use "isFile()" because that will fail if the file doesn't
    // exist yet
    if (!fileInfo.isDir()) {
        path = fileInfo.dir().path();
    }

    // this is the path for just this dialog
    settings.setValue(_settingsKey, path);

    // this is the path for all FileDialog
    settings.setValue(_generalSettingsKey, path);
}

void FileDialog::storeDirectory(const QStringList& files) {
    if (files.count() > 0) {
        storeDirectory(files.at(0));
    }
}

/**
 * Returns the selected file or an empty string if none was selected
 */
QString FileDialog::selectedFile() {
    QStringList fileNames = selectedFiles();
    return fileNames.count() > 0 ? fileNames.at(0) : QString();
}
