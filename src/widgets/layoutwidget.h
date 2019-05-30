#ifndef LAYOUTWIDGET_H
#define LAYOUTWIDGET_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class LayoutWidget;
}

class LayoutWidget : public QWidget
{
    Q_OBJECT
Q_SIGNALS:
    void settingsStored();

public:
    explicit LayoutWidget(QWidget *parent = 0);
    ~LayoutWidget();

    void resizeLayoutImage() const;

    void setManualSettingsStoring(bool enabled);

    void storeSettings();

private slots:
    void on_layoutComboBox_currentIndexChanged(int index);

    void resizeEvent(QResizeEvent* event);

    void on_useLayoutPushButton_clicked();

private:
    Ui::LayoutWidget *ui;

    QSettings *_layoutSettings;

    bool _manualSettingsStoring;

    void loadLayouts();

    void updateCurrentLayout();

    static QString getLayoutName(const QString& layoutIdentifier);

    static QString getLayoutDescription(const QString& layoutIdentifier);
};

#endif // LAYOUTWIDGET_H
