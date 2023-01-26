#include "aboutdialog.h"

#include <utils/misc.h>

#include <QDate>
#include <QFile>
#include <QTextStream>

#include "build_number.h"
#include "release.h"
#include "ui_aboutdialog.h"
#include "version.h"

AboutDialog::AboutDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::AboutDialog) {
    ui->setupUi(this);

    // load the about.html
    QFile f(QStringLiteral(":/html/about.html"));
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream istream(&f);
        QString html = istream.readAll();
        QDate date = QDate::currentDate();

        QString release = qApp->property("release").toString();
        if (release.isEmpty()) {
            release = QLatin1String("generic");
        }

        // fill in the variables
        html.replace(QLatin1String("QT_VERSION_STR"), QStringLiteral(QT_VERSION_STR));
        html.replace(QLatin1String("BUILD_NUMBER"), QString::number(BUILD));
        html.replace(QLatin1String("BUILD_DATE"), __DATE__);
        html.replace(QLatin1String("VERSION"), QStringLiteral(VERSION));
        html.replace(QLatin1String("RELEASE"), release);
        html.replace(QLatin1String("CURRENT_YEAR"), QString::number(date.year()));

        ui->textBrowser->document()->setDefaultStyleSheet(Utils::Misc::genericCSS());

        // put the html to the text browser in the about dialog
        ui->textBrowser->setHtml(html);
        f.close();
    }
}

AboutDialog::~AboutDialog() { delete ui; }
