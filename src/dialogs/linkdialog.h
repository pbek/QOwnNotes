#pragma once

#include <QListWidgetItem>

#include "masterdialog.h"

namespace Ui {
class LinkDialog;
}

class Note;

class LinkDialog : public MasterDialog {
    Q_OBJECT

   public:
    enum LinkDialogPages { TextLinkPage, NoteLinkPage };

    explicit LinkDialog(int page, const QString &dialogTitle = QString(),
                        QWidget *parent = nullptr);
    ~LinkDialog();

    QString getSelectedNoteName() const;
    Note getSelectedNote() const;
    QString getURL() const;
    QString getLinkName() const;
    void setLinkName(const QString &text);
    QString getLinkDescription() const;
    static QString getTitleForUrl(const QUrl &url);
    QString getSelectedHeading() const;

   private slots:
    void on_buttonBox_accepted();
    void on_searchLineEdit_textChanged(const QString &arg1);
    void on_notesListWidget_doubleClicked(const QModelIndex &index);
    void on_urlEdit_textChanged(const QString &arg1);
    void addFileUrl();
    void addDirectoryUrl();

    void on_headingSearchLineEdit_textChanged(const QString &arg1);

    void on_notesListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_headingListWidget_doubleClicked(const QModelIndex &index);

    void on_tabWidget_currentChanged(int index);

   private:
    Ui::LinkDialog *ui;
    int firstVisibleNoteListRow;
    bool eventFilter(QObject *obj, QEvent *event) override;
    QString selectedNoteText;
    void setupFileUrlMenu();
    void loadNoteHeadings() const;
    void doAccept();
};
