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

    void on_layoutComboBox_currentIndexChanged(int index);

    void resizeEvent(QResizeEvent* event);

private:
    Ui::WelcomeDialog *ui;
    bool _allowFinishButton;
    QString _notesPath;
    QSettings *_layoutSettings;

    bool handleNoteFolderSetup();

    void showNoteFolderErrorMessage(QString message);

    void storeNoteFolderSettings();

    void closeEvent(QCloseEvent *event);

    void loadLayouts();

    void updateCurrentLayout() const;

    static QString getLayoutName(QString layoutIdentifier);

    static QString getLayoutDescription(QString layoutIdentifier);

    void resizeLayoutImage() const;
};
