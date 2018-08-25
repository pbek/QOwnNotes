#pragma once

#include "masterdialog.h"
#include <QDialog>

namespace Ui {
class IssueAssistantDialog;
}

class IssueAssistantDialog : public MasterDialog
{
    Q_OBJECT

public:
    enum IssueAssistantPages {
        IssuePage,
        LogOutputPage,
        DebugSettingsPage,
        SubmitPage
    };

    enum IssueTypes {
        QuestionIssueType,
        FeatureRequestIssueType,
        ProblemIssueType
    };

    explicit IssueAssistantDialog(QWidget *parent = nullptr);
    ~IssueAssistantDialog();

private slots:
    void on_nextButton_clicked();

    void on_backButton_clicked();

    void on_finishButton_clicked();

    void on_issueTypeComboBox_currentIndexChanged(int index);

    void on_refreshLogButton_clicked();

private:
    Ui::IssueAssistantDialog *ui;

    void refreshLogOutput() const;

    void generateSubmitPageContent() const;
};
