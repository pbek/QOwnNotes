#pragma once

#include <QDialog>
#include <QKeyEvent>

class MasterDialog : public QDialog
{
public:
    explicit MasterDialog(QWidget *parent = nullptr);

public Q_SLOTS:
    virtual int exec() override;

    virtual void show();

    virtual void open() override;

protected:
    bool _ignoreReturnKey = false;

    void resizeEvent(QResizeEvent *event) override;

    const QString getGeometrySettingKey() const;

    void storeGeometrySettings() const;

    bool eventFilter(QObject *obj, QEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

    void handleOpenDialog();

    virtual void keyPressEvent(QKeyEvent *keyEvent) override;

public:
    void setIgnoreReturnKey(bool ignore);
};
