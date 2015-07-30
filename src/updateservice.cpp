#include "updateservice.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QObject>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QMessageBox>
#include <QLibraryInfo>
#include "build_number.h"
#include "updatedialog.h"
#include "version.h"

UpdateService::UpdateService(QObject *parent) : QObject(parent)
{
    this->parent = parent;
}

bool UpdateService::checkForUpdates()
{
    this->checkForUpdates( false );
}

bool UpdateService::checkForUpdates( bool isManual )
{
    this->isManual = isManual;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onResult(QNetworkReply*)));

    bool isDebug;

#ifdef QT_NO_DEBUG
    isDebug = false;
#else
    isDebug = true;
#endif

    // there are troubles with https on different platforms, we are using http everywhere for now
    QUrl url( "http://www.qownnotes.org/api/v1/last_release/QOwnNotes/" + QString( PLATFORM ) + ".json" +
                "?b=" + QString::number( BUILD ) +
                "&v=" + QString( VERSION ) +
                "&d=" + __DATE__ + " " + __TIME__ +
                "&debug=" + ( isDebug ? "1" : "0" ) );

//    qDebug() << __func__ << " - 'url': " << url;

    manager->get(QNetworkRequest(url));
}

void UpdateService::onResult(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        return;
    }

    // we have to add [], so the string can be parsed as JSON
    QString data = QString("[") + (QString) reply->readAll() + QString("]");
//    qDebug() << data;

    QScriptEngine engine;
    QScriptValue result = engine.evaluate(data);

    // get the information if we sould update our app
    QScriptValue shouldUpdate = result.property("0").property("should_update");
//    qDebug() << shouldUpdate.toBool();

    // check if we should update our app
    if ( shouldUpdate.toBool() )
    {
        // get the release url
        QString releaseUrl = result.property("0").property("release").property("assets").property("0").property("browser_download_url").toString();
//        qDebug() << releaseUrl;

        // get the release version string
        QString releaseVersionString = result.property("0").property("release_version_string").toString();
//        qDebug() << releaseVersionString;

        // get the release build number
        int releaseBuildNumber = result.property("0").property("release_build_number").toInteger();
//        qDebug() << releaseBuildNumber;

        // get the changes text
        QString changesText = result.property("0").property("changes").toString();
//        qDebug() << changesText;

        // open the update dialog
        UpdateDialog *dialog = new UpdateDialog( 0, changesText, releaseUrl, releaseVersionString, releaseBuildNumber );
        dialog->exec();

    }
    else if( this->isManual )
    {
        QMessageBox::information( 0, "No updates", "There are no updates available.\n" + QString( VERSION ) + " is the latest version." );
    }
}
