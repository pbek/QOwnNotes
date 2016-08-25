#ifndef FONTCOLORWIDGET_H
#define FONTCOLORWIDGET_H

#include <QFrame>

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

private:
    Ui::FontColorWidget *ui;
    QStringList _defaultSchemaKeys;
    QString _currentSchemaKey;
    bool _currentSchemaIsDefault;

    void addTextTreeWidgetItem(QString text, int id);

    void initTextTreeWidgetItems();

    void initSchemaSelector();

    void updateSchemeEditFrame();
};

#endif // FONTCOLORWIDGET_H
