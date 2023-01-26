#include "issueassistantdialog.h"

#include <utils/gui.h>
#include <utils/misc.h>

#include <QClipboard>
#include <QDesktopServices>
#include <QFileDialog>
#include <QSettings>

#include "QDebug"
#include "mainwindow.h"
#include "services/metricsservice.h"
#include "settingsdialog.h"
#include "ui_issueassistantdialog.h"

IssueAssistantDialog::IssueAssistantDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::IssueAssistantDialog) {
    ui->setupUi(this);
    ui->backButton->setEnabled(false);
    ui->nextButton->setEnabled(false);

    ui->stackedWidget->setCurrentIndex(IssueAssistantPages::IssuePage);
    on_issueTypeComboBox_currentIndexChanged(ProblemIssueType);
    ui->logOutputPlainTextEdit->setHighlightingEnabled(false);

    QObject::connect(ui->titleLineEdit, SIGNAL(textChanged(QString)), this,
                     SLOT(allowIssuePageNextButton()));
    QObject::connect(ui->questionPlainTextEdit, SIGNAL(textChanged()), this,
                     SLOT(allowIssuePageNextButton()));
    QObject::connect(ui->expectedBehaviourPlainTextEdit, SIGNAL(textChanged()), this,
                     SLOT(allowIssuePageNextButton()));
    QObject::connect(ui->actualBehaviourPlainTextEdit, SIGNAL(textChanged()), this,
                     SLOT(allowIssuePageNextButton()));
    QObject::connect(ui->stepsPlainTextEdit, SIGNAL(textChanged()), this,
                     SLOT(allowIssuePageNextButton()));
    QObject::connect(ui->logOutputPlainTextEdit, SIGNAL(textChanged()), this,
                     SLOT(allowLogPageNextButton()));
    QObject::connect(ui->debugOutputPlainTextEdit, SIGNAL(textChanged()), this,
                     SLOT(allowDebugSettingsPageNextButton()));
}

IssueAssistantDialog::~IssueAssistantDialog() { delete ui; }

void IssueAssistantDialog::on_nextButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("issue-assistant-dialog/next"));

    int index = ui->stackedWidget->currentIndex();
    int maxIndex = ui->stackedWidget->count() - 1;

    if (index < maxIndex) {
        index++;
        ui->stackedWidget->setCurrentIndex(index);
    }

    refreshPage(index);

    ui->backButton->setEnabled(true);
    ui->nextButton->setEnabled(index < maxIndex);
}

void IssueAssistantDialog::allowIssuePageNextButton() const {
    bool allow = ui->titleLineEdit->text().length() >= 8;

    if (ui->questionPlainTextEdit->isVisible()) {
        allow = allow && ui->questionPlainTextEdit->toPlainText().length() >= 10;
    }

    if (ui->expectedBehaviourPlainTextEdit->isVisible()) {
        allow = allow && ui->expectedBehaviourPlainTextEdit->toPlainText().length() >= 10;
    }

    if (ui->actualBehaviourPlainTextEdit->isVisible()) {
        allow = allow && ui->actualBehaviourPlainTextEdit->toPlainText().length() >= 10;
    }

    if (ui->stepsPlainTextEdit->isVisible()) {
        allow = allow && ui->stepsPlainTextEdit->toPlainText().length() >= 10;
    }

    ui->nextButton->setEnabled(allow);
}

void IssueAssistantDialog::allowLogPageNextButton() const {
    bool allow = true;

    //    if (ui->issueTypeComboBox->currentIndex() == ProblemIssueType) {
    //        allow = ui->logOutputPlainTextEdit->toPlainText().length() >= 10;
    //    }

    ui->nextButton->setEnabled(allow);
}

void IssueAssistantDialog::allowDebugSettingsPageNextButton() const {
    bool allow = true;

    if (ui->issueTypeComboBox->currentIndex() == ProblemIssueType) {
        allow = ui->debugOutputPlainTextEdit->toPlainText().length() >= 100;
    }

    ui->nextButton->setEnabled(allow);
}

void IssueAssistantDialog::refreshPage(int index) const {
    switch (index) {
        case LogOutputPage:
            ui->nextButton->setEnabled(false);
            if (ui->logOutputPlainTextEdit->toPlainText().isEmpty()) {
                refreshLogOutput();
            }
            break;
        case DebugSettingsPage:
            if (ui->debugOutputPlainTextEdit->toPlainText().isEmpty()) {
                ui->debugOutputPlainTextEdit->setPlainText(
                    Utils::Misc::generateDebugInformation(true));
            }
            break;
        case SubmitPage:
            generateSubmitPageContent();
            break;
        default:
            break;
    }
}

void IssueAssistantDialog::refreshLogOutput() const {
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow == nullptr) {
        return;
    }

    mainWindow->turnOnDebugLogging();
    ui->logOutputPlainTextEdit->setPlainText(mainWindow->getLogText());
}

void IssueAssistantDialog::generateSubmitPageContent() const {
    QString title;

    switch (ui->issueTypeComboBox->currentIndex()) {
        case QuestionIssueType:
            title = QLatin1String("[Q]");
            break;
        case FeatureRequestIssueType:
            title = QLatin1String("[F]");
            break;
        case ProblemIssueType:
            title = QLatin1String("[I]");
            break;
        default:
            break;
    }

    title += " " + ui->titleLineEdit->text();
    ui->submitTitleLineEdit->setText(title);

    QString body;

    switch (ui->issueTypeComboBox->currentIndex()) {
        case QuestionIssueType:
            body += "#### General question\n\n" +
                    ui->questionPlainTextEdit->toPlainText().trimmed() + "\n\n";
            break;
        case FeatureRequestIssueType:
        case ProblemIssueType:
            body += "#### Expected behaviour\n\n" +
                    ui->expectedBehaviourPlainTextEdit->toPlainText().trimmed() + "\n\n";

            body += "#### Actual behaviour\n\n" +
                    ui->actualBehaviourPlainTextEdit->toPlainText().trimmed() + "\n\n";

            body += "#### Steps to reproduce\n\n" +
                    ui->stepsPlainTextEdit->toPlainText().trimmed() + "\n\n";
            break;
        default:
            break;
    }

    body +=
        "#### Relevant log output in the Log panel\n\n"
        "<details><summary>Expand</summary>\n\n```\n" +
        ui->logOutputPlainTextEdit->toPlainText().trimmed() + "\n```\n</details>\n\n";

    body +=
        "#### Information about the application, settings and "
        "environment\n\n<details><summary>Expand</summary>\n\n" +
        ui->debugOutputPlainTextEdit->toPlainText().trimmed() + "\n</details>\n\n";

    ui->bodyPlainTextEdit->setPlainText(body);
}

void IssueAssistantDialog::on_backButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("issue-assistant-dialog/back"));

    int index = ui->stackedWidget->currentIndex();

    if (index > 0) {
        index--;
        ui->stackedWidget->setCurrentIndex(index);
    }

    refreshPage(index);

    ui->nextButton->setEnabled(true);
    ui->backButton->setEnabled(index > 0);
}

void IssueAssistantDialog::on_issueTypeComboBox_currentIndexChanged(int index) {
    switch (index) {
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

void IssueAssistantDialog::on_refreshLogButton_clicked() { refreshLogOutput(); }

void IssueAssistantDialog::on_postButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("issue-assistant-dialog/post"));

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->bodyPlainTextEdit->toPlainText());

    QUrl url("https://github.com/pbek/QOwnNotes/issues/new?title=" +
             QUrl::toPercentEncoding(ui->submitTitleLineEdit->text()) + "&body=" +
             QUrl::toPercentEncoding("Please paste the text from the issue "
                                     "assistant here. It should be already in "
                                     "your clipboard."));

    // we cannot add the body, this would make the url too long
    //    + "&body=" +
    //    QUrl::toPercentEncoding(ui->bodyPlainTextEdit->toPlainText()));

    QDesktopServices::openUrl(url);
}

void IssueAssistantDialog::on_cancelButton_clicked() { close(); }

void IssueAssistantDialog::on_searchIssueButton_clicked() {
    QUrl url("https://github.com/pbek/QOwnNotes/issues?utf8=%E2%9C%93&q=" +
             QUrl::toPercentEncoding(ui->titleLineEdit->text()));

    QDesktopServices::openUrl(url);
}

void IssueAssistantDialog::on_newIssueButton_clicked() {
    if (Utils::Gui::question(this, QStringLiteral("Create new issue"),
                             QStringLiteral("Reset this dialog and create a new issue?"),
                             QStringLiteral("issue-dialog-new-issue")) != QMessageBox::Yes) {
        return;
    }

    ui->titleLineEdit->clear();
    ui->questionPlainTextEdit->clear();
    ui->expectedBehaviourPlainTextEdit->clear();
    ui->actualBehaviourPlainTextEdit->clear();
    ui->stepsPlainTextEdit->clear();
    ui->backButton->setEnabled(false);
    ui->nextButton->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(IssueAssistantPages::IssuePage);
    ui->issueTypeComboBox->setCurrentIndex(QuestionIssueType);
}
