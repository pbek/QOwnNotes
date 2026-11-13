#pragma once

#include "masterdialog.h"
class QListWidgetItem;
class QSettings;

namespace Ui {
class WelcomeDialog;
}

class WelcomeDialog : public MasterDialog {
    Q_OBJECT

   public:
    enum WelcomePages { NoteFolderPage, LayoutPresetPage, CloudPage, MetricsPage };

    explicit WelcomeDialog(QWidget *parent = 0);
    ~WelcomeDialog();

   private slots:
    void on_cancelButton_clicked();

    void on_nextButton_clicked();

    void on_backButton_clicked();

    void on_finishButton_clicked();

    void on_noteFolderButton_clicked();

    void on_cloudSettingsButton_clicked();

    void on_networkSettingsButton_clicked();

    void on_noteFolderCandidatesListWidget_itemClicked(QListWidgetItem *item);

    void on_noteFolderCandidatesListWidget_currentItemChanged(QListWidgetItem *current,
                                                              QListWidgetItem *previous);

   private:
    Ui::WelcomeDialog *ui;
    bool _allowFinishButton;
    QString _notesPath;
    QSettings *_layoutSettings;

    bool handleNoteFolderSetup();

    void initNoteFolderCandidates();

    void setNotesPath(const QString &path);

    void useNoteFolderCandidate(QListWidgetItem *item);

    void selectNoteFolderCandidate(const QString &path);

    void updateNoteFolderCandidateColors();

    QWidget *createNoteFolderCandidateWidget(const QString &path, const QString &details,
                                             bool isQOwnNotesFolder);

    bool confirmNewNoteFolderCreation();

    void updateStepInfo();

    void showNoteFolderErrorMessage(const QString &message);

    void storeNoteFolderSettings();

    void closeEvent(QCloseEvent *event);
};
