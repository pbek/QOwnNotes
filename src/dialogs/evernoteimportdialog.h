#pragma once

#include <QDialog>
#include <QXmlQuery>
#include <entities/note.h>
#include "masterdialog.h"

namespace Ui {
class EvernoteImportDialog;
}

class EvernoteImportDialog : public MasterDialog
{
    Q_OBJECT

    struct ImageFileData {
        QString data;
        QString suffix;
    };

public:
    explicit EvernoteImportDialog(QWidget *parent = 0);
    ~EvernoteImportDialog();

    int getImportCount();

private slots:
    void on_fileButton_clicked();

private:
    Ui::EvernoteImportDialog *ui;
    int _importCount;

    void importNotes(QString data);

    int countNotes(QString data);

    void initNoteCount(QString data);

    QString importImages(QString content, QXmlQuery query);

    QString getMarkdownForImageFileData(ImageFileData &imageFileData);

    void tagNote(QXmlQuery &query, Note &note);
};
