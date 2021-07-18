#pragma once

#include "masterdialog.h"

namespace Ui {
class JoplinImportDialog;
}

class Note;
class QXmlQuery;
class QTreeWidgetItem;

class JoplinImportDialog : public MasterDialog {
    Q_OBJECT

    struct MediaFileData {
        QString data;
        QString suffix;
        QString fileName;
    };

   public:
    explicit JoplinImportDialog(QWidget *parent = 0);
    ~JoplinImportDialog() override;

    int getImportCount() const;

   private slots:
    void on_directoryButton_clicked();

    void on_importButton_clicked();

   private:
    Ui::JoplinImportDialog *ui;
    int _importCount;
    QHash <QString, QString> _noteData;
    QHash <QString, QString> _tagData;
    QHash <QString, QStringList> _tagAssignmentData;
    QHash <QString, QString> _imageData;
    QHash <QString, QString> _attachmentData;

    bool importNote(const QString& id, const QString& text,
                    const QString& dirPath);
    void tagNote(const QString& id, const Note& note);
};
