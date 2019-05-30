#ifndef SCRIPTSETTINGWIDGET_H
#define SCRIPTSETTINGWIDGET_H

#include <QWidget>
#include <QMap>
#include <QVariant>
#include <QJsonValue>
#include <entities/script.h>

namespace Ui {
class ScriptSettingWidget;
}

class ScriptSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptSettingWidget(QWidget *parent, Script script,
                                 QMap<QString, QVariant> variableMap);
    ~ScriptSettingWidget();

private slots:
    void on_stringLineEdit_textChanged(const QString &arg1);

    void on_integerSpinBox_valueChanged(int arg1);

    void on_textEdit_textChanged();

    void on_filePathLineEdit_textChanged(const QString &arg1);

    void on_filePathButton_clicked();

    void on_booleanCheckBox_toggled(bool checked);

    void on_selectionComboBox_currentIndexChanged(int index);

private:
    Ui::ScriptSettingWidget *ui;
    QMap<QString, QVariant> _variableMap;
    Script _script;

    void storeSettingsVariable(const QJsonValue& value);
};

#endif // SCRIPTSETTINGWIDGET_H
