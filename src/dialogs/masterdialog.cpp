#include "masterdialog.h"
#include "services/metricsservice.h"

MasterDialog::MasterDialog(QWidget *parent) : QDialog(parent)
{
}

int MasterDialog::exec() {
    MetricsService * metricsService =
            qApp->property("metricsService").value<MetricsService *>();

    metricsService->sendAppView(objectName());

    return QDialog::exec();
}
