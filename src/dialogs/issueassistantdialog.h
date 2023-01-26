#pragma once

#include <QDialog>

#include "masterdialog.h"

namespace Ui {
class IssueAssistantDialog;
}

class IssueAssistantDialog : public MasterDialog {
    Q_OBJECT

   public:
    enum IssueAssistantPages { IssuePage, LogOutputPage, DebugSettingsPage, SubmitPage };

    enum IssueTypes {
        ProblemIssueType,
        FeatureRequestIssueType,
        QuestionIssueType,
    };

    explicit IssueAssistantDialog(QWidget *parent = nullptr);
    ~IssueAssistantDialog();

   private slots:
    void on_nextButton_clicked();

    void on_backButton_clicked();

    void on_issueTypeComboBox_currentIndexChanged(int index);

    void on_refreshLogButton_clicked();

    void on_postButton_clicked();

    void allowIssuePageNextButton() const;

    void allowLogPageNextButton() const;

    void allowDebugSettingsPageNextButton() const;

    void on_cancelButton_clicked();

    void on_searchIssueButton_clicked();

    void on_newIssueButton_clicked();

   private:
    Ui::IssueAssistantDialog *ui;

    void refreshLogOutput() const;

    void generateSubmitPageContent() const;

    void refreshPage(int index) const;
};
