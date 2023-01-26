#include "scriptsettingwidget.h"

#include <dialogs/filedialog.h>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <limits>

#include "ui_scriptsettingwidget.h"

ScriptSettingWidget::ScriptSettingWidget(QWidget *parent, Script script,
                                         QMap<QString, QVariant> variableMap)
    : QWidget(parent), ui(new Ui::ScriptSettingWidget) {
    ui->setupUi(this);

    _variableMap = variableMap;
    _script = script;

    QString name = variableMap["name"].toString();
    QString type = variableMap["type"].toString();
    QString description = variableMap["description"].toString();
    QString identifier = variableMap["identifier"].toString();
    QString text = variableMap["text"].toString();

    ui->nameLabel->setText("<b>" + name + "</b>");
    ui->descriptionLabel->setText(description);
    ui->descriptionLabel->setHidden(description.isEmpty());
    ui->integerSpinBox->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    // hide all widgets in the control frame
    ui->integerSpinBox->hide();
    ui->stringLineEdit->hide();
    ui->textEdit->hide();
    ui->filePathButton->hide();
    ui->filePathLineEdit->hide();
    ui->booleanCheckBox->hide();
    ui->selectionComboBox->hide();

    QJsonObject jsonObject = script.getSettingsVariablesJsonObject();

    if (type == "integer") {
        int value = jsonObject.value(identifier).toInt();

        if (jsonObject.value(identifier).isUndefined()) {
            value = variableMap["default"].toInt();
        }

        ui->integerSpinBox->setValue(value);
        ui->integerSpinBox->show();
    } else if (type == "boolean") {
        bool value = jsonObject.value(identifier).toBool();

        if (jsonObject.value(identifier).isUndefined()) {
            value = variableMap["default"].toBool();
        }

        if (text.isEmpty()) {
            // fallback if no text was set
            ui->booleanCheckBox->setText(description.isEmpty() ? name : description);
            ui->descriptionLabel->hide();
        } else {
            ui->booleanCheckBox->setText(text);
        }

        ui->booleanCheckBox->setChecked(value);
        ui->booleanCheckBox->show();
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
    } else if (type == "file" || type == "directory") {
        QString value = jsonObject.value(identifier).toString();

        if (jsonObject.value(identifier).isUndefined()) {
            value = variableMap["default"].toString();
        }

        ui->filePathLineEdit->setText(value);
        ui->filePathButton->show();
        ui->filePathLineEdit->show();
    } else if (type == "selection") {
        QMap<QString, QVariant> items = variableMap["items"].toMap();
        QString value = jsonObject.value(identifier).toString();

        if (jsonObject.value(identifier).isUndefined()) {
            value = variableMap["default"].toString();
        }

        ui->selectionComboBox->clear();

        QMapIterator<QString, QVariant> i(items);
        int index = 0;
        int currentIndex = 0;

        while (i.hasNext()) {
            i.next();
            ui->selectionComboBox->addItem(i.value().toString(), i.key());

            if (i.key() == value) {
                currentIndex = index;
            }

            index++;
        }

        ui->selectionComboBox->setCurrentIndex(currentIndex);
        ui->selectionComboBox->show();
    }
}

ScriptSettingWidget::~ScriptSettingWidget() { delete ui; }

/**
 * Stores a setting variable
 *
 * @param value
 */
void ScriptSettingWidget::storeSettingsVariable(const QJsonValue &value) {
    // we need to fetch the data again so we are not overwriting data of other
    // ScriptSettingWidget instances
    _script.refetch();

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
void ScriptSettingWidget::on_integerSpinBox_valueChanged(int arg1) { storeSettingsVariable(arg1); }

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
    const QString identifier = _variableMap["identifier"].toString();
    const QString description = _variableMap["description"].toString();
    const QString type = _variableMap["type"].toString();
    const bool isDirectory = type == "directory";

    FileDialog dialog("ScriptSettingsFile-" + _script.getIdentifier() + "-" + identifier);
    dialog.setFileMode(isDirectory ? QFileDialog::Directory : QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setWindowTitle(isDirectory ? tr("Please select a directory")
                                      : tr("Please select a file"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            ui->filePathLineEdit->setText(QDir::toNativeSeparators(fileName));
        }
    }
}

/**
 * Stores the settings variable from booleanCheckBox
 *
 * @param checked
 */
void ScriptSettingWidget::on_booleanCheckBox_toggled(bool checked) {
    storeSettingsVariable(checked);
}

void ScriptSettingWidget::on_selectionComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index);
    storeSettingsVariable(ui->selectionComboBox->currentData().toString());
}
