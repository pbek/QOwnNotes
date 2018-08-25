#include "QDebug"
#include <QSettings>
#include <QFileDialog>
#include <utils/misc.h>
#include "issueassistantdialog.h"
#include "ui_issueassistantdialog.h"
#include "settingsdialog.h"
#include "services/metricsservice.h"

IssueAssistantDialog::IssueAssistantDialog(QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::IssueAssistantDialog) {
    ui->setupUi(this);
    ui->backButton->setEnabled(false);

    ui->stackedWidget->setCurrentIndex(IssueAssistantPages::IssuePage);
    on_issueTypeComboBox_currentIndexChanged(QuestionIssueType);
    ui->logOutputPlainTextEdit->setHighlightingEnabled(false);
}

IssueAssistantDialog::~IssueAssistantDialog() {
    delete ui;
}

void IssueAssistantDialog::on_nextButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled("issue-assistant-dialog/next");

    int index = ui->stackedWidget->currentIndex();
    int maxIndex = ui->stackedWidget->count() - 1;

    if (index < maxIndex) {
        index++;
        ui->stackedWidget->setCurrentIndex(index);
    }

    if (index == IssueAssistantPages::LogOutputPage) {
        refreshLogOutput();
    }

    if (index == IssueAssistantPages::SubmitPage) {
        generateSubmitPageContent();
    }

    ui->backButton->setEnabled(true);
    ui->nextButton->setEnabled(index < maxIndex);
}

void IssueAssistantDialog::refreshLogOutput() const {
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow == nullptr) {
        return;
    }

    ui->logOutputPlainTextEdit->setPlainText(mainWindow->getLogText());
}

void IssueAssistantDialog::generateSubmitPageContent() const {
    QString title;

    switch(ui->issueTypeComboBox->currentIndex()) {
        case QuestionIssueType:
            title = "[Q]";
            break;
        case FeatureRequestIssueType:
            title = "[F]";
            break;
        case ProblemIssueType:
            title = "[P]";
            break;
    }

    title += " " + ui->titleLineEdit->text();
    ui->submitTitleLineEdit->setText(title);

    QString body;
    body += "#### Expected behaviour\n\n" +
            ui->expectedBehaviourPlainTextEdit->toPlainText().trimmed() +
            "\n\n";

    body += "#### Actual behaviour\n\n" +
            ui->actualBehaviourPlainTextEdit->toPlainText().trimmed() +
            "\n\n";

    body += "#### Steps to reproduce\n\n" +
            ui->stepsPlainTextEdit->toPlainText().trimmed() +
            "\n\n";

    body += "#### Relevant log output in the Log panel\n\n```" +
            ui->logOutputPlainTextEdit->toPlainText().trimmed() +
            "```\n\n";

    body += "#### Output from the debug section in the settings dialog\n\n" +
            ui->debugOutputPlainTextEdit->toPlainText().trimmed() +
            "\n\n";

    ui->bodyPlainTextEdit->setPlainText(body);
}

void IssueAssistantDialog::on_backButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled("issue-assistant-dialog/back");

    int index = ui->stackedWidget->currentIndex();

    if (index > 0) {
        index--;
        ui->stackedWidget->setCurrentIndex(index);
    }

    ui->nextButton->setEnabled(true);
    ui->backButton->setEnabled(index > 0);
}

void IssueAssistantDialog::on_finishButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled("issue-assistant-dialog/finished");
}

void IssueAssistantDialog::on_issueTypeComboBox_currentIndexChanged(int index) {
    switch(index) {
        case QuestionIssueType:
            ui->questionGroupBox->setVisible(true);
            ui->expectedBehaviourGroupBox->setVisible(false);
            ui->actualBehaviourGroupBox->setVisible(false);
            ui->stepsGroupBox->setVisible(false);
            break;
        default:
            ui->questionGroupBox->setVisible(false);
            ui->expectedBehaviourGroupBox->setVisible(true);
            ui->actualBehaviourGroupBox->setVisible(true);
            ui->stepsGroupBox->setVisible(true);
            break;
    }
}

void IssueAssistantDialog::on_refreshLogButton_clicked() {
    refreshLogOutput();
}
