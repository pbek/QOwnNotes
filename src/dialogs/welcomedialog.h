#pragma once

#include "masterdialog.h"
#include <QDialog>
#include <QSettings>
#include <QResizeEvent>

namespace Ui {
class WelcomeDialog;
}

class WelcomeDialog : public MasterDialog
{
    Q_OBJECT

public:
    enum WelcomePages {
        NoteFolderPage,
        LayoutPage,
        OwnCloudPage,
        MetricsPage
    };

    explicit WelcomeDialog(QWidget *parent = 0);
    ~WelcomeDialog();

private slots:
    void on_cancelButton_clicked();

    void on_nextButton_clicked();

    void on_backButton_clicked();

    void on_finishButton_clicked();

    void on_noteFolderButton_clicked();

    void on_ownCloudSettingsButton_clicked();

    void on_networkSettingsButton_clicked();

private:
    Ui::WelcomeDialog *ui;
    bool _allowFinishButton;
    QString _notesPath;
    QSettings *_layoutSettings;

    bool handleNoteFolderSetup();

    void showNoteFolderErrorMessage(const QString& message);

    void storeNoteFolderSettings();

    void closeEvent(QCloseEvent *event);
};
