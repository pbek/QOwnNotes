#include "filedialog.h"
#include "imagedialog.h"
#include "ui_imagedialog.h"
#include <QClipboard>
#include <utility>

ImageDialog::ImageDialog(QWidget *parent) :
    MasterDialog(parent),
    ui(new Ui::ImageDialog) {
    _imageFile = nullptr;
    _tempFile = nullptr;

    ui->setupUi(this);
    ui->previewFrame->setVisible(false);

    QClipboard *clipboard = QApplication::clipboard();

    QPixmap pixmap = clipboard->pixmap();
    if (!pixmap.isNull()) {
        ui->fileEdit->setDisabled(true);
        setPixmap(pixmap, true);
    }
}

ImageDialog::~ImageDialog() {
    delete _imageFile;
    delete _tempFile;
    delete ui;
}

QFile *ImageDialog::getImageFile() {
    return _imageFile;
}

QString ImageDialog::getImageTitle() {
    return ui->titleEdit->text();
}

void ImageDialog::on_openButton_clicked() {
    FileDialog dialog("InsertImage");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("Image files") + " (*.jpg *.png *.gif)");
    dialog.setWindowTitle(tr("Select image to insert"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString filePath = dialog.selectedFile();

        if (!filePath.isEmpty()) {
            ui->fileEdit->setText(filePath);
            ui->fileEdit->setEnabled(true);
            setPixmap(QPixmap(filePath), true);
        }
    }
}

void ImageDialog::setPixmap(const QPixmap& pixmap, bool updateBase) {
    if (pixmap.isNull()) {
        return;
    }

    _pixmap = pixmap;
    auto *scene = new QGraphicsScene(this);
    scene->addPixmap(pixmap);
    ui->graphicsView->setScene(scene);

    if (updateBase) {
        _basePixmap = pixmap;

        const QSignalBlocker blocker(ui->widthSpinBox);
        Q_UNUSED(blocker)
        const QSignalBlocker blocker2(ui->widthScaleHorizontalSlider);
        Q_UNUSED(blocker2)

        ui->widthSpinBox->setValue(pixmap.width());
        ui->widthScaleHorizontalSlider->setValue(10);
        updateWidthScaleLabelValue();
    }

    ui->previewFrame->setVisible(true);
    ui->verticalSpacer->changeSize(0, 0);
}

void ImageDialog::on_buttonBox_accepted() {
    // if the image was manipulated or from the clipboard we will store it into
    // a temporary file
    if (ui->fileEdit->text().isEmpty() ||
        ui->widthSpinBox->value() != _basePixmap.width()) {

        _tempFile = new QTemporaryFile(QDir::tempPath() + QDir::separator() +
            "qownnotes-media-XXXXXX.png");

        if (_tempFile->open()) {
            // save temporary png image
            _pixmap.save(_tempFile->fileName(), "PNG");
            _imageFile = new QFile(_tempFile->fileName());
        }
    } else {
        _imageFile = new QFile(ui->fileEdit->text());
    }
}

void ImageDialog::on_widthSpinBox_valueChanged(int arg1) {
    double factor = (double) arg1 / _basePixmap.width();
    updateWidthScaleLabelValue();

    const QSignalBlocker blocker(ui->widthScaleHorizontalSlider);
    Q_UNUSED(blocker)
    ui->widthScaleHorizontalSlider->setValue((int) (factor * 10));
}

void ImageDialog::updateWidthScaleLabelValue() const {
    double factor = (double) ui->widthScaleHorizontalSlider->value() / 10;
    ui->widthScaleLabel->setText(QString::number(factor) + "x");
}

void ImageDialog::on_widthScaleHorizontalSlider_valueChanged(int value) {
    updateWidthScaleLabelValue();

    const QSignalBlocker blocker(ui->widthSpinBox);
    Q_UNUSED(blocker)

    int width = (int) (_basePixmap.width() * value / 10);
    ui->widthSpinBox->setValue(width);

    QPixmap pixmap = _basePixmap.scaledToWidth(width, Qt::SmoothTransformation);
    setPixmap(pixmap);
}
