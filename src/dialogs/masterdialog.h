#pragma once

#include <QDialog>
#include <functional>
class QKeyEvent;
class LoadingOverlayWidget;

class MasterDialog : public QDialog {
   public:
    explicit MasterDialog(QWidget *parent = nullptr);

   public Q_SLOTS:
    virtual int exec() override;

    virtual void show();

    virtual void open() override;

    void reject() override;

   protected:
    bool _ignoreReturnKey = false;

    void resizeEvent(QResizeEvent *event) override;

    const QString getGeometrySettingKey() const;

    void storeGeometrySettings() const;

    bool eventFilter(QObject *obj, QEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

    void handleOpenDialog();

    virtual void keyPressEvent(QKeyEvent *keyEvent) override;

    void afterSetupUI();

   public:
    void setIgnoreReturnKey(bool ignore);

    void showLoadingOverlay(const QString &title, const QString &text = QString(),
                            const std::function<void()> &cancelCallback = {});

    void hideLoadingOverlay();

    void setLoadingProgress(qint64 bytesReceived, qint64 bytesTotal);

    void setLoadingCountProgress(int value, int maximum, const QString &statusText);

    bool isLoading() const;

   private:
    LoadingOverlayWidget *_loadingOverlay = nullptr;
    bool _loading = false;
    std::function<void()> _loadingCancelCallback;
    QSize _minimumSizeBeforeLoading;
    QSize _sizeBeforeLoading;
};
