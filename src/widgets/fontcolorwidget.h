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

private:
    Ui::FontColorWidget *ui;
};

#endif // FONTCOLORWIDGET_H
