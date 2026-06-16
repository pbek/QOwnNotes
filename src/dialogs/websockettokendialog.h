#ifndef WEBSOCKETTOKENDIALOG_H
#define WEBSOCKETTOKENDIALOG_H

#include <QDialog>

#include "masterdialog.h"

class QShowEvent;

namespace Ui {
class WebSocketTokenDialog;
}

class WebSocketTokenDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit WebSocketTokenDialog(QWidget *parent = 0);
    ~WebSocketTokenDialog();

    void reject() override;

   protected:
    void showEvent(QShowEvent *event) override;

   private slots:
    void on_copyButton_clicked();

    void on_generateButton_clicked();

    void on_buttonBox_accepted();

   private:
    Ui::WebSocketTokenDialog *ui;
    QString _initialToken;

    QString generateToken() const;
    void loadTokenFromSettings();
};

#endif    // WEBSOCKETTOKENDIALOG_H
