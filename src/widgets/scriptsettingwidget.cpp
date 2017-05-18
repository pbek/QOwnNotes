#include "scriptsettingwidget.h"
#include "ui_scriptsettingwidget.h"
#include <QDebug>

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
    QString defaultValue = variableMap["default"].toString();

    ui->descriptionLabel->setText(description);
    ui->descriptionLabel->setHidden(description.isEmpty());

    // hide all widgets in the control frame
    ui->integerSpinBox->hide();
    ui->stringLineEdit->hide();
    ui->textEdit->hide();
    ui->filePathButton->hide();
    ui->filePathLineEdit->hide();

//    foreach(QWidget *w, ui->controlFrame->layout()->findChildren<QWidget *>()) {
//            w->hide();
////            qDebug() << __func__ << " - 'w': " << w->objectName();
//        }

    if (type == "integer") {
        ui->integerSpinBox->show();
    } else if (type == "string") {
        ui->stringLineEdit->show();
    } else if (type == "text") {
        ui->textEdit->show();
    } else if (type == "file") {
        ui->filePathButton->show();
        ui->filePathLineEdit->show();
    }
}

ScriptSettingWidget::~ScriptSettingWidget()
{
    delete ui;
}
