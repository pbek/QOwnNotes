#pragma once

#include <QDialog>
#include <entities/note.h>
#include "masterdialog.h"

namespace Ui {
class LinkDialog;
}

class LinkDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit LinkDialog(const QString &dialogTitle = QString(), QWidget *parent = nullptr);
    ~LinkDialog();

    QString getSelectedNoteName() const;
    Note getSelectedNote() const;
    QString getURL() const;
    QString getLinkName() const;
    void setLinkName(const QString &text);
    QString getLinkDescription() const;
    QString getTitleForUrl(const QUrl& url);

private slots:
    void on_searchLineEdit_textChanged(const QString &arg1);
    void on_notesListWidget_doubleClicked(const QModelIndex &index);
    void on_urlEdit_textChanged(const QString &arg1);
    void addFileUrl();
    void addDirectoryUrl();
private:
    Ui::LinkDialog *ui;
    int firstVisibleNoteListRow;
    bool eventFilter(QObject *obj, QEvent *event);
    QString selectedNoteText;
    void setupFileUrlMenu();
};
