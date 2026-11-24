#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include <memory>

#include "masterdialog.h"

class MainWindow;
class Note;
class QAbstractButton;
class QJSValue;
class QListWidgetItem;
class QPushButton;
class QSplitter;
class QTextBrowser;

namespace Ui {
class VersionDialog;
}

class VersionDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit VersionDialog(const QJSValue &versions, QWidget *parent = nullptr);
    explicit VersionDialog(QWidget *parent = nullptr);
    ~VersionDialog();

    void setVersions(const QJSValue &versions);
    void setGitVersions(const QJSValue &versions, const Note &note, const QString &currentNoteText);
    void enableLocalComparison(const Note &note, const QString &editorText, bool decrypted);

   private slots:
    void storeSettings();
    void on_versionListWidget_currentRowChanged(int currentRow);
    void dialogButtonClicked(QAbstractButton *button);
    void on_searchLineEdit_textChanged(const QString &text);

   private:
    enum ButtonRole {
        Unset,    // nothing was selected
        Restore,
        Cancel
    };

    Ui::VersionDialog *ui;
    QSplitter *versionSplitter;
    QPushButton *_restoreButton;
    QStringList *_diffList;
    QStringList *_dataList;
    QListWidgetItem *_currentNoteItem = nullptr;
    std::unique_ptr<Note> _localComparisonNote;
    QString _localComparisonText;
    bool _localComparisonDecrypted = false;
    QTextBrowser *_localComparisonBrowser = nullptr;
    void setVersions(const QJSValue &versions, const Note &note, const QString *currentNoteText);
    void setupMainSplitter();
    void updateVersionPreview();
    void updateLocalComparison();
    void updateCountLabel();
};

#endif    // VERSIONDIALOG_H
