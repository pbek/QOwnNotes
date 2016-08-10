#include <QSettings>
#include "masterdialog.h"
#include "services/metricsservice.h"

MasterDialog::MasterDialog(QWidget *parent) : QDialog(parent) {
}

void MasterDialog::resizeEvent(QResizeEvent *event) {
    // save the geometry of the dialog
    QSettings settings;
    settings.setValue(objectName() + "/geometry", saveGeometry());

    QWidget::resizeEvent(event);
}

int MasterDialog::exec() {
    // restore the geometry of the dialog
    QSettings settings;
    restoreGeometry(settings.value(objectName() + "/geometry").toByteArray());

    MetricsService::instance()->sendVisitIfEnabled("dialog/" + objectName());
    return QDialog::exec();
}
