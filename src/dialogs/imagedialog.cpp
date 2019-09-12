#include "filedialog.h"
#include "imagedialog.h"
#include "ui_imagedialog.h"
#include <QClipboard>
#include <QSettings>
#include <utility>
#include <utils/misc.h>

ImageDialog::ImageDialog(QWidget *parent) :
    MasterDialog(parent),
    ui(new Ui::ImageDialog) {
    _imageFile = nullptr;
    _tempFile = nullptr;
    _rubberBand = nullptr;

    ui->setupUi(this);
    ui->previewFrame->setVisible(false);
    ui->toolFrame->hide();

    QClipboard *clipboard = QApplication::clipboard();
    QPixmap pixmap = clipboard->pixmap();

    if (!pixmap.isNull()) { // set image from clipboard
        ui->fileEdit->setDisabled(true);
        setPixmap(pixmap, true);
    } else {
        QString text = clipboard->text();
        QUrl url(text);

        // set text from clipboard
        if (url.isValid()) {
            ui->fileEdit->setText(text);
        }
    }

    QSettings settings;
    ui->disableCopyingCheckBox->setChecked(settings.value("ImageDialog/disableCopying").toBool());

    connect(ui->graphicsView, SIGNAL(scrolledContentsBy(int, int)),
            this, SLOT(scrolledGraphicsViewContentsBy(int, int)));
    connect(ui->graphicsView, SIGNAL(resizedBy(int, int)),
            this, SLOT(resizedGraphicsViewBy(int, int)));
}

ImageDialog::~ImageDialog() {
    QSettings settings;
    settings.setValue("ImageDialog/disableCopying", ui->disableCopyingCheckBox->isChecked());

    delete _imageFile;
    delete _tempFile;
    delete ui;
}

bool ImageDialog::isDisableCopying() {
    return ui->disableCopyingCheckBox->isChecked();
}

QFile *ImageDialog::getImageFile() {
    return _imageFile;
}

QString ImageDialog::getFilePathOrUrl() {
    return ui->fileEdit->text();
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
            ui->fileEdit->setEnabled(true);
            // the pixmap will be updated by the textChanged handler
            ui->fileEdit->setText(filePath);
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
    if (ui->fileEdit->text().isEmpty() || _imageWasCropped ||
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

void ImageDialog::on_fileEdit_textChanged(const QString &arg1) {
    auto pathOrUrl = arg1;
    auto url = QUrl(pathOrUrl);

    if (!url.isValid()) {
        return;
    }

    // download remote images
    if (url.scheme().startsWith("http")) {
        QByteArray data = Utils::Misc::downloadUrl(url);

        if (data.size() == 0) {
            return;
        }

        QPixmap pixmap;
        pixmap.loadFromData(data);

        if (pixmap.isNull()) {
            return;
        }

        setPixmap(pixmap, true);

        return;
    }

    if (url.scheme() == "file") {
        pathOrUrl = url.toLocalFile();
    }

    auto file = new QFile(pathOrUrl);

    // check if local file exists and not empty
    if (file->size() == 0) {
        return;
    }

    setPixmap(QPixmap(pathOrUrl), true);
}


void ImageDialog::on_disableCopyingCheckBox_toggled(bool checked) {
    ui->scaleFrame->setDisabled(checked);
    ui->graphicsView->setDragMode(checked ? QGraphicsView::NoDrag : QGraphicsView::RubberBandDrag);

    if (checked) {
        // reset scaling
        ui->widthScaleHorizontalSlider->setValue(10);
    }
}

void ImageDialog::on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint)
{
    if (viewportRect.isEmpty()) { // dragging has stopped
        _rubberBand = new QRubberBand(QRubberBand::Rectangle, ui->graphicsView);

        // for some reason the coordinates where off by 2 points
        _lastRubberBandViewportRect.adjust(2, 2, 2, 2);

        _rubberBand->setGeometry(_lastRubberBandViewportRect);
        _rubberBand->show();
    } else { // currently dragging
        // close rubber band from previous dragging
        if (_rubberBand != Q_NULLPTR && _rubberBand->isVisible()) {
            _rubberBand->close();
        }

        QPoint fromScenePointI = fromScenePoint.toPoint();
        QPoint toScenePointI = toScenePoint.toPoint();

        if (fromScenePointI.x() < 0) {
            fromScenePointI.setX(0);
        }

        if (fromScenePointI.y() < 0) {
            fromScenePointI.setY(0);
        }

        if (toScenePointI.x() < 0) {
            toScenePointI.setX(0);
        }

        if (toScenePointI.y() < 0) {
            toScenePointI.setY(0);
        }

        // swap coordinates if the drag was "reversed"
        if (fromScenePointI.x() > toScenePointI.x() && fromScenePointI.y() > toScenePointI.y()) {
            _rubberBandSceneRect = QRect(toScenePointI, fromScenePointI);
        } else {
            _rubberBandSceneRect = QRect(fromScenePointI, toScenePointI);
        }

        _lastRubberBandViewportRect = viewportRect;
        ui->toolFrame->show();
    }
}

void ImageDialog::on_cropButton_clicked() {
    if (!_rubberBandSceneRect.isEmpty()) {
        qDebug() << "Crop " << _rubberBandSceneRect;

        QPixmap cropped = _pixmap.copy(_rubberBandSceneRect);
        setPixmap(cropped, true);

        _rubberBandSceneRect = QRect(0, 0, 0, 0);
        _rubberBand->close();
        _imageWasCropped = true;
        ui->toolFrame->hide();
    }
}

void ImageDialog::scrolledGraphicsViewContentsBy(int dx, int dy) {
    if (_rubberBand == Q_NULLPTR || _rubberBand->isHidden()) {
        return;
    }

    // move rubberband if graphics view was scrolled
    _rubberBand->move(_rubberBand->x() + dx, _rubberBand->y() + dy);
}

void ImageDialog::resizedGraphicsViewBy(int dw, int dh) {
    if (_rubberBand == Q_NULLPTR || _rubberBand->isHidden()) {
        return;
    }

    // move rubberband if graphics view was resized
    // TODO: does not work in all cases
//    _rubberBand->move(_rubberBand->x() + dw / 2, _rubberBand->y() + dh / 2);

    // we will close the rubberband until we can do something better
    _rubberBand->close();
}
