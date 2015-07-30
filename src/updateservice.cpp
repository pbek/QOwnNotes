#include "updateservice.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QObject>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include "build_number.h"
#include "version.h"

UpdateService::UpdateService(QObject *parent) : QObject(parent)
{
    this->parent = parent;
}

bool UpdateService::checkForUpdates()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onResult(QNetworkReply*)));

    // there are troubles with https on different platforms, we are using http everywhere for now
    QUrl url("http://www.qownnotes.org/api/v1/last_release/QOwnNotes/" + QString( PLATFORM ) + ".json?b=" + QString::number( BUILD ) + "&v=" + QString( VERSION ) + "&d=" + __DATE__ + " " + __TIME__ );

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
        QScriptValue releaseUrl = result.property("0").property("release").property("url");
//        qDebug() << releaseUrl.toString();
    }
}
