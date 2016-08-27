#ifndef FONTCOLORWIDGET_H
#define FONTCOLORWIDGET_H

#include <QFrame>
#include <QTreeWidgetItem>
#include <QSettings>

namespace Ui {
class FontColorWidget;
}

class FontColorWidget : public QFrame
{
    Q_OBJECT

public:
    explicit FontColorWidget(QWidget *parent = 0);
    ~FontColorWidget();

private slots:
    void on_foregroundColorButton_clicked();

    void on_colorSchemeComboBox_currentIndexChanged(int index);

    void on_textTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_copySchemeButton_clicked();

    void on_backgroundColorButton_clicked();

private:
    Ui::FontColorWidget *ui;
    QStringList _defaultSchemaKeys;
    QString _currentSchemaKey;
    bool _currentSchemaIsDefault;

    void addTextTreeWidgetItem(QString text, int id);

    void initTextTreeWidgetItems();

    void initSchemaSelector();

    void updateSchemeEditFrame();

    QVariant getSchemaValue(QString key, QVariant defaultValue = QVariant());

    QSettings* getSchemaSettings();

    void setSchemaValue(QString key, QVariant value, QString schemaKey = "");

    QString textSettingsKey(QString key);
};

#endif // FONTCOLORWIDGET_H
