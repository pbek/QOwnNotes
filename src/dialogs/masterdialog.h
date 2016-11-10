#ifndef MASTERDIALOG_H
#define MASTERDIALOG_H

#include <QDialog>

class MasterDialog : public QDialog
{
public:
    explicit MasterDialog(QWidget *parent = 0);

public Q_SLOTS:
    virtual int exec();

    virtual void show();

    virtual void open();

protected:
    void resizeEvent(QResizeEvent *event);

    const QString getGeometrySettingKey() const;

    void storeGeometrySettings() const;

    bool eventFilter(QObject *obj, QEvent *event);

    void closeEvent(QCloseEvent *event);

    void handleOpenDialog();
};

#endif // MASTERDIALOG_H