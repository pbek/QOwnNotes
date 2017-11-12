#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "build_number.h"
#include "version.h"
#include "release.h"
#include <QFile>
#include <QDate>
#include <QTextStream>
#include <utils/misc.h>

AboutDialog::AboutDialog(QWidget *parent) :
        MasterDialog(parent),
        ui(new Ui::AboutDialog) {
    ui->setupUi(this);

    // load the about.html
    QFile f(":/html/about.html");
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream istream(&f);
        QString html = istream.readAll();
        QDate date = QDate::currentDate();

        QString release = qApp->property("release").toString();
        if (release.isEmpty()) {
            release = "generic";
        }

        // fill in the variables
        html.replace("QT_VERSION_STR", QString(QT_VERSION_STR));
        html.replace("BUILD_NUMBER", QString::number(BUILD));
        html.replace("BUILD_DATE", __DATE__);
        html.replace("VERSION", QString(VERSION));
        html.replace("RELEASE", release);
        html.replace("CURRENT_YEAR", QString::number(date.year()));

        ui->textBrowser->document()->setDefaultStyleSheet(
                Utils::Misc::genericCSS());

        // put the html to the text browser in the about dialog
        ui->textBrowser->setHtml(html);
        f.close();
    }
}

AboutDialog::~AboutDialog() {
    delete ui;
}
