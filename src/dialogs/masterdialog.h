#ifndef MASTERDIALOG_H
#define MASTERDIALOG_H

#include <QDialog>

class MasterDialog : public QDialog
{
public:
    explicit MasterDialog(QWidget *parent = 0);

public Q_SLOTS:
    virtual int exec();

protected:
    void resizeEvent(QResizeEvent *event);

    const QString getGeometrySettingKey() const;

    void storeGeometrySettings() const;

    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MASTERDIALOG_H