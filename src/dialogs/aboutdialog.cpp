#include "aboutdialog.h"

#include <utils/misc.h>

#include <QClipboard>
#include <QDate>
#include <QFile>
#include <QGuiApplication>
#include <QPushButton>
#include <QTextStream>

#include "build_number.h"
#include "release.h"
#include "ui_aboutdialog.h"
#include "version.h"

AboutDialog::AboutDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::AboutDialog) {
    ui->setupUi(this);
    afterSetupUI();

    // load the about.html
    QFile f(QStringLiteral(":/html/about.html"));
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream istream(&f);
        QString html = istream.readAll();
        QDate date = QDate::currentDate();

        // fill in the variables
        html.replace(QLatin1String("QT_VERSION_STR"), QStringLiteral(QT_VERSION_STR));
        html.replace(QLatin1String("BUILD_NUMBER"), QString::number(BUILD));
        html.replace(QLatin1String("BUILD_DATE"), __DATE__);
        html.replace(QLatin1String("VERSION"), QStringLiteral(VERSION));
        html.replace(QLatin1String("RELEASE"), release());
        html.replace(QLatin1String("CURRENT_YEAR"), QString::number(date.year()));

        ui->textBrowser->document()->setDefaultStyleSheet(Utils::Misc::genericCSS());

        // put the html to the text browser in the about dialog
        ui->textBrowser->setHtml(html);
        f.close();
    }

    // Allow copying the version information, for example for bug reports
    const QString versionInformation = QStringLiteral("QOwnNotes %1 (build %2, %3), Qt %4")
                                           .arg(QStringLiteral(VERSION), QString::number(BUILD),
                                                release(), QStringLiteral(QT_VERSION_STR));
    QPushButton *copyButton =
        ui->buttonBox->addButton(tr("&Copy version information"), QDialogButtonBox::ActionRole);
    copyButton->setIcon(
        QIcon::fromTheme(QStringLiteral("edit-copy"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-copy.svg"))));
    copyButton->setToolTip(versionInformation);
    connect(copyButton, &QPushButton::clicked, this, [copyButton, versionInformation]() {
        QGuiApplication::clipboard()->setText(versionInformation);
        copyButton->setText(tr("Version information copied"));
    });

    ui->buttonBox->button(QDialogButtonBox::Close)->setDefault(true);
}

/**
 * Returns the name of the release, like the package or installation type
 */
QString AboutDialog::release() {
    QString release = qApp->property("release").toString();
    if (release.isEmpty()) {
        release = QLatin1String("generic");
    }
#ifdef QON_KDE_INTEGRATION
    release += QStringLiteral(" (KDE integration)");
#endif

    return release;
}

AboutDialog::~AboutDialog() { delete ui; }
