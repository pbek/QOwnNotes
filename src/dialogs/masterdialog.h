#ifndef MASTERDIALOG_H
#define MASTERDIALOG_H

#include <QDialog>

class MasterDialog : public QDialog
{
public:
    explicit MasterDialog(QWidget *parent = 0);
public Q_SLOTS:
    virtual int exec();
};

#endif // MASTERDIALOG_H