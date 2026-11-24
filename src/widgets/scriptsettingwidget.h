#ifndef SCRIPTSETTINGWIDGET_H
#define SCRIPTSETTINGWIDGET_H

#include <entities/script.h>

#include <QJsonValue>
#include <QMap>
#include <QVariant>
#include <QWidget>

namespace Ui {
class ScriptSettingWidget;
}

class ScriptSettingWidget : public QWidget {
    Q_OBJECT

   public:
    explicit ScriptSettingWidget(QWidget *parent, const Script &script,
                                 QMap<QString, QVariant> variableMap);
    ~ScriptSettingWidget();

   private slots:
    void on_filePathButton_clicked();

   private:
    Ui::ScriptSettingWidget *ui;
    QMap<QString, QVariant> _variableMap;
    Script _script;

    void storeString(const QString &value);
    void storeSettingsVariable(const QJsonValue &value);
};

#endif    // SCRIPTSETTINGWIDGET_H
