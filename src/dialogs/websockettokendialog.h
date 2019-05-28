#ifndef WEBSOCKETTOKENDIALOG_H
#define WEBSOCKETTOKENDIALOG_H

#include <QDialog>

namespace Ui {
class WebSocketTokenDialog;
}

class WebSocketTokenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WebSocketTokenDialog(QWidget *parent = 0);
    ~WebSocketTokenDialog();

private slots:
    void on_copyButton_clicked();

    void on_generateButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::WebSocketTokenDialog *ui;

    QString generateToken() const;
};

#endif // WEBSOCKETTOKENDIALOG_H
