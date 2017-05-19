#include "scriptsettingwidget.h"
#include "ui_scriptsettingwidget.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <dialogs/filedialog.h>

ScriptSettingWidget::ScriptSettingWidget(QWidget *parent, Script script,
                                         QMap<QString, QVariant> variableMap) :
    QWidget(parent),
    ui(new Ui::ScriptSettingWidget) {
    ui->setupUi(this);

    _variableMap = variableMap;
    _script = script;

    QString name = variableMap["name"].toString();
    QString type = variableMap["type"].toString();
    QString description = variableMap["description"].toString();
    QString identifier = variableMap["identifier"].toString();

    ui->descriptionLabel->setText(description);
    ui->descriptionLabel->setHidden(description.isEmpty());

    // hide all widgets in the control frame
    ui->integerSpinBox->hide();
    ui->stringLineEdit->hide();
    ui->textEdit->hide();
    ui->filePathButton->hide();
    ui->filePathLineEdit->hide();

    QJsonObject jsonObject = script.getSettingsVariablesJsonObject();

    if (type == "integer") {
        int value = jsonObject.value(identifier).toInt();

        if (jsonObject.value(identifier).isUndefined()) {
            value = variableMap["default"].toInt();
        }

        ui->integerSpinBox->setValue(value);
        ui->integerSpinBox->show();
    } else if (type == "string") {
        QString value = jsonObject.value(identifier).toString();

        if (jsonObject.value(identifier).isUndefined()) {
            value = variableMap["default"].toString();
        }

        ui->stringLineEdit->setText(value);
        ui->stringLineEdit->show();
    } else if (type == "text") {
        QString value = jsonObject.value(identifier).toString();

        if (jsonObject.value(identifier).isUndefined()) {
            value = variableMap["default"].toString();
        }

        ui->textEdit->setPlainText(value);
        ui->textEdit->show();
    } else if (type == "file") {
        QString value = jsonObject.value(identifier).toString();

        if (jsonObject.value(identifier).isUndefined()) {
            value = variableMap["default"].toString();
        }

        ui->filePathLineEdit->setText(value);
        ui->filePathButton->show();
        ui->filePathLineEdit->show();
    }
}

ScriptSettingWidget::~ScriptSettingWidget() {
    delete ui;
}

/**
 * Stores a setting variable
 *
 * @param value
 */
void ScriptSettingWidget::storeSettingsVariable(QJsonValue value) {
    QString identifier = _variableMap["identifier"].toString();
    QJsonObject jsonObject = _script.getSettingsVariablesJsonObject();
    jsonObject.insert(identifier, value);
    _script.setSettingsVariablesJson(jsonObject);
    _script.store();
}

/**
 * Stores the settings variable from the stringLineEdit
 *
 * @param arg1
 */
void ScriptSettingWidget::on_stringLineEdit_textChanged(const QString &arg1) {
    storeSettingsVariable(arg1);
}

/**
 * Stores the settings variable from the integerSpinBox
 *
 * @param arg1
 */
void ScriptSettingWidget::on_integerSpinBox_valueChanged(int arg1) {
    storeSettingsVariable(arg1);
}

/**
 * Stores the settings variable from the textEdit
 */
void ScriptSettingWidget::on_textEdit_textChanged() {
    storeSettingsVariable(ui->textEdit->toPlainText());
}

/**
 * Stores the settings variable from the filePathLineEdit
 *
 * @param arg1
 */
void ScriptSettingWidget::on_filePathLineEdit_textChanged(const QString &arg1) {
    storeSettingsVariable(arg1);
}

/**
 * Lets the user select a file
 */
void ScriptSettingWidget::on_filePathButton_clicked() {
    QJsonObject jsonObject = _script.getSettingsVariablesJsonObject();
    QString identifier = _variableMap["identifier"].toString();
    QString description = _variableMap["description"].toString();

    FileDialog dialog("ScriptSettingsFile-" + _script.getIdentifier() +
                              "-" + identifier);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setWindowTitle(tr("Please select a file"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            ui->filePathLineEdit->setText(QDir::toNativeSeparators(fileName));
        }
    }
}
