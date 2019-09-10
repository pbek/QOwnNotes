#pragma once

#include <entities/note.h>
#include <QTemporaryFile>
#include "masterdialog.h"

namespace Ui {
class ImageDialog;
}

class ImageDialog : public MasterDialog
{
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

private:
    Ui::ImageDialog *ui;
    QPixmap _basePixmap;
    QPixmap _pixmap;
    QFile *_imageFile;
    QTemporaryFile *_tempFile;

    void setPixmap(const QPixmap& pixmap, bool updateBase = false);

    void updateWidthScaleLabelValue() const;
};

