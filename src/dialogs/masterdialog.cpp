#include <QSettings>
#include "masterdialog.h"
#include "services/metricsservice.h"

MasterDialog::MasterDialog(QWidget *parent) : QDialog(parent) {
    installEventFilter(this);
}

void MasterDialog::resizeEvent(QResizeEvent *event) {
    // save the geometry of the dialog
    storeGeometrySettings();

    QWidget::resizeEvent(event);
}

/**
 * Stores the geometry of the dialog
 */
void MasterDialog::storeGeometrySettings() const {
    QSettings settings;
    settings.setValue(getGeometrySettingKey(), saveGeometry());
}

bool MasterDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Move) {
        // save the geometry of the dialog
        storeGeometrySettings();
    }

    return QDialog::eventFilter(obj, event);
}

/**
 * Returns the geometry settings key
 *
 * @return settings key
 */
const QString MasterDialog::getGeometrySettingKey() const {
    return objectName() + "/geometry";
}

int MasterDialog::exec() {
    // restore the geometry of the dialog
    QSettings settings;
    QByteArray geometryData =
            settings.value(getGeometrySettingKey()).toByteArray();

    // just restore the geometry if there is some data
    if (geometryData.length() > 0) {
        restoreGeometry(geometryData);
    }

    MetricsService::instance()->sendVisitIfEnabled("dialog/" + objectName());
    return QDialog::exec();
}
