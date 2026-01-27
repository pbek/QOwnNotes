#include "imagedialog.h"

#include <utils/misc.h>

#include <QClipboard>
#include <QDebug>
#include <QRubberBand>
#include <QTemporaryFile>
#include <utility>

#include "filedialog.h"
#include "services/settingsservice.h"
#include "ui_imagedialog.h"

ImageDialog::ImageDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::ImageDialog) {
    _imageFile = nullptr;
    _tempFile = nullptr;
    _rubberBand = nullptr;

    ui->setupUi(this);
    afterSetupUI();
    ui->fileEdit->setFocus();
    ui->previewFrame->setVisible(false);
    ui->toolFrame->hide();

    SettingsService settings;
    ui->disableCopyingCheckBox->setChecked(
        settings.value(QStringLiteral("ImageDialog/disableCopying")).toBool());

    QClipboard *clipboard = QApplication::clipboard();
    QPixmap pixmap = clipboard->pixmap();

    if (!pixmap.isNull()) {    // set image from clipboard
        ui->fileEdit->setDisabled(true);
        ui->disableCopyingCheckBox->setChecked(false);
        ui->disableCopyingCheckBox->setDisabled(true);
        setPixmap(pixmap, true);
    } else {
        QString text = clipboard->text().trimmed();
        QUrl url(text);

        // set text from clipboard
        if (url.isValid()) {
            ui->fileEdit->setText(text);
        }
    }

    connect(ui->graphicsView, SIGNAL(scrolledContentsBy(int, int)), this,
            SLOT(scrolledGraphicsViewContentsBy(int, int)));
    connect(ui->graphicsView, SIGNAL(resizedBy(int, int)), this,
            SLOT(resizedGraphicsViewBy(int, int)));
}

ImageDialog::~ImageDialog() {
    SettingsService settings;
    settings.setValue(QStringLiteral("ImageDialog/disableCopying"),
                      ui->disableCopyingCheckBox->isChecked());

    delete _imageFile;
    delete _tempFile;
    delete ui;
}

bool ImageDialog::isDisableCopying() { return ui->disableCopyingCheckBox->isChecked(); }

QFile *ImageDialog::getImageFile() { return _imageFile; }

QString ImageDialog::getFilePathOrUrl() { return ui->fileEdit->text().trimmed(); }

QString ImageDialog::getImageTitle() { return ui->titleEdit->text(); }

void ImageDialog::on_openButton_clicked() {
    QStringList nameFilters;
    nameFilters << tr("Image files") +
                       " (*.jpg *.jpeg *.png *.gif *.svg *.bmp *.pbm *.pgm "
                       "*.ppm *.xbm *.xpm *.webp)"
                << tr("Any files") + " (*)";

    FileDialog dialog(QStringLiteral("InsertImage"));
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilters(nameFilters);
    dialog.setWindowTitle(tr("Select image to insert"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString filePath = dialog.selectedFile();

        if (!filePath.isEmpty()) {
            ui->disableCopyingCheckBox->setEnabled(true);
            ui->fileEdit->setEnabled(true);
            // the pixmap will be updated by the textChanged handler
            ui->fileEdit->setText(filePath.trimmed());
        }
    }
}

void ImageDialog::setPixmap(const QPixmap &pixmap, bool updateBase) {
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
    if (ui->fileEdit->text().trimmed().isEmpty() || _imageWasCropped || _imageWasDownloaded ||
        ui->widthSpinBox->value() != _basePixmap.width()) {
        _tempFile =
            new QTemporaryFile(QDir::tempPath() + QDir::separator() + "qownnotes-media-XXXXXX.png");

        if (_tempFile->open()) {
            // save temporary png image
            _pixmap.save(_tempFile->fileName(), "PNG");
            _imageFile = new QFile(_tempFile->fileName());
        }
    } else {
        _imageFile = new QFile(ui->fileEdit->text().trimmed());
    }
}

void ImageDialog::on_widthSpinBox_valueChanged(int arg1) {
    double factor = (double)arg1 / _basePixmap.width();

    const QSignalBlocker blocker(ui->widthScaleHorizontalSlider);
    Q_UNUSED(blocker)
    ui->widthScaleHorizontalSlider->setValue((int)(factor * 10));
    updateWidthScaleLabelValue();

    QPixmap pixmap = _basePixmap.scaledToWidth(arg1, Qt::SmoothTransformation);
    setPixmap(pixmap);
}

void ImageDialog::updateWidthScaleLabelValue() const {
    double factor = (double)ui->widthScaleHorizontalSlider->value() / 10;
    ui->widthScaleLabel->setText(QString::number(factor) + "x");
}

void ImageDialog::on_widthScaleHorizontalSlider_valueChanged(int value) {
    updateWidthScaleLabelValue();

    int width = (int)(_basePixmap.width() * value / 10);
    ui->widthSpinBox->setValue(width);
}

void ImageDialog::on_fileEdit_textChanged(const QString &arg1) {
    auto pathOrUrl = arg1.trimmed();

    // Clear preview if path is empty
    if (pathOrUrl.isEmpty()) {
        ui->previewFrame->setVisible(false);
        ui->verticalSpacer->changeSize(0, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        _pixmap = QPixmap();
        _basePixmap = QPixmap();
        return;
    }

    auto url = QUrl(pathOrUrl);

    if (!url.isValid()) {
        return;
    }

    // download remote images
    if (url.scheme().startsWith(QLatin1String("http"))) {
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
        _imageWasDownloaded = true;

        return;
    }

    _imageWasDownloaded = false;

    if (url.scheme() == QLatin1String("file")) {
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

void ImageDialog::on_graphicsView_rubberBandChanged(QRect viewportRect, QPointF fromScenePoint,
                                                    QPointF toScenePoint) {
    if (viewportRect.isEmpty()) {    // dragging has stopped
        _rubberBand = new QRubberBand(QRubberBand::Rectangle, ui->graphicsView);

        QMargins margin = ui->graphicsView->contentsMargins();

        // we need to adapt the rubberband by the content margins of the
        // graphics view
        _lastRubberBandViewportRect.adjust(margin.left(), margin.top(), margin.left(),
                                           margin.top());

        _rubberBand->setGeometry(_lastRubberBandViewportRect);
        _rubberBand->show();
    } else {    // currently dragging
        // close rubber band from previous dragging
        if (_rubberBand != nullptr && _rubberBand->isVisible()) {
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
    if (_rubberBand == nullptr || _rubberBand->isHidden()) {
        return;
    }

    // move rubberband if graphics view was scrolled
    _rubberBand->move(_rubberBand->x() + dx, _rubberBand->y() + dy);
}

void ImageDialog::resizedGraphicsViewBy(int dw, int dh) {
    if (_rubberBand == nullptr || _rubberBand->isHidden()) {
        return;
    }

    // move rubberband if graphics view was resized
    // TODO: does not work in all cases
    //    _rubberBand->move(_rubberBand->x() + dw / 2, _rubberBand->y() + dh /
    //    2);
    Q_UNUSED(dw)
    Q_UNUSED(dh)

    // we will close the rubberband until we can do something better
    _rubberBand->close();
}
