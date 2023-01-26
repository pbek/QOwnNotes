#pragma once

#include "masterdialog.h"

namespace Ui {
class ImageDialog;
}

class QRubberBand;
class QTemporaryFile;
class QFile;

class ImageDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit ImageDialog(QWidget *parent = nullptr);

    ~ImageDialog() override;

    QFile *getImageFile();

    QString getImageTitle();

    bool isDisableCopying();

    QString getFilePathOrUrl();

   private slots:
    void on_openButton_clicked();

    void on_buttonBox_accepted();

    void on_widthSpinBox_valueChanged(int arg1);

    void on_widthScaleHorizontalSlider_valueChanged(int value);

    void on_fileEdit_textChanged(const QString &arg1);

    void on_disableCopyingCheckBox_toggled(bool checked);

    void on_graphicsView_rubberBandChanged(QRect viewportRect, QPointF fromScenePoint,
                                           QPointF toScenePoint);

    void on_cropButton_clicked();

    void scrolledGraphicsViewContentsBy(int dx, int dy);

    void resizedGraphicsViewBy(int dw, int dh);

   private:
    Ui::ImageDialog *ui;
    QPixmap _basePixmap;
    QPixmap _pixmap;
    QFile *_imageFile;
    QTemporaryFile *_tempFile;
    QRubberBand *_rubberBand;
    bool _imageWasCropped = false;
    bool _imageWasDownloaded = false;
    QRect _rubberBandSceneRect;
    QRect _lastRubberBandViewportRect;

    void setPixmap(const QPixmap &pixmap, bool updateBase = false);

    void updateWidthScaleLabelValue() const;
};
