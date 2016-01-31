#include "masterdialog.h"
#include <services/analyticsservice.h>

MasterDialog::MasterDialog(QWidget *parent) : QDialog(parent)
{
}

int MasterDialog::exec() {
    AnalyticsService* analyticsService =
            qApp->property("analyticsService").value<AnalyticsService*>();

    analyticsService->sendAppView(objectName());

    return QDialog::exec();
}
