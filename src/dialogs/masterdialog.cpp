#include "masterdialog.h"
#include "services/metricsservice.h"

MasterDialog::MasterDialog(QWidget *parent) : QDialog(parent)
{
}

int MasterDialog::exec() {
    MetricsService::instance()->sendAppView(objectName());

    return QDialog::exec();
}
