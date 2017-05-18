#ifndef SCRIPTSETTINGWIDGET_H
#define SCRIPTSETTINGWIDGET_H

#include <QWidget>
#include <QMap>
#include <QVariant>
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

private:
    Ui::ScriptSettingWidget *ui;
    QMap<QString, QVariant> _variableMap;
    Script _script;
};

#endif // SCRIPTSETTINGWIDGET_H
