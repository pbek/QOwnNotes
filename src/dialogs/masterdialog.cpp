#include "masterdialog.h"
#include "services/metricsservice.h"

MasterDialog::MasterDialog(QWidget *parent) : QDialog(parent)
{
}

int MasterDialog::exec() {
    MetricsService::instance()->sendVisitIfEnabled("dialog/" + objectName());
    return QDialog::exec();
}
