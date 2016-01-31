/*

  Copyright (c) 2014, Michael Tesch
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Michael Tesch nor the names of other
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  https://github.com/tesch1/qt-google-analytics-collector

  to enable debugging messages, '#define GANALYTICS_DEBUG 1' before including this file
  to get super verbose debugging, '#define GANALYTICS_DEBUG 2'

  To build GAnalytics with QtQuick application (QGuiApplication) instead of Desktop,
  define GANALYTICS_QTQUICK in your .pro file like this: 'DEFINES += GANALYTICS_QTQUICK',
  or in cmake project: 'add_definitions(-DGANALYTICS_QTQUICK)'
*/

#pragma once

#include <unistd.h>
#include <map>
#include <QCoreApplication>
#include <QSettings>

#if defined(GANALYTICS_QTQUICK)
#include <QGuiApplication>
#include <QScreen>
#else
#ifdef QT_GUI_LIB
#include <QApplication>
#include <QDesktopWidget>
#endif
#endif

#include <QUuid>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QProcess>
#include <QList>
#include <QEventLoop>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#define URL_QUERY QUrlQuery
#else
#define URL_QUERY QUrl
#endif

/*!
 * send google tracking data according to
 * https://developers.google.com/analytics/devguides/collection/protocol/v1/reference
 */

#ifndef GANALYTICS_DEBUG
#define GANALYTICS_DEBUG 0
#endif

class GAnalytics : public QObject {
Q_OBJECT
    Q_PROPERTY (bool isBusy READ isBusy NOTIFY busyChanged)
public:
    GAnalytics(QCoreApplication * parent,
               QString trackingID,
               QString clientID = "",
               bool useGET = false)
            : QObject(parent), _qnam(this), _trackingID(trackingID), _clientID(clientID), _useGET(useGET), _isFail(false)
            , _waitLoop()
    {
      if (parent) {
        setAppName(parent->applicationName());
        setAppVersion(parent->applicationVersion());
#ifdef QT_GUI_LIB
        parent->dumpObjectTree();
#endif
      }
      if (!_clientID.size()) {
        // load client id from settings
        QSettings settings;
        if (!settings.contains("GAnalytics-cid")) {
          settings.setValue("GAnalytics-cid", QUuid::createUuid().toString());
        }
        _clientID = settings.value("GAnalytics-cid").toString();
      }
      connect(&_qnam, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
#if QT_VERSION >= 0x040800
#if GANALYTICS_DEBUG
      if (!_qnam.networkAccessible())
      qDebug() << "error: network inaccessible\n";
#endif
#endif
    }

    ~GAnalytics() {
      // this generally happens after the event-loop is done, so no more network processing
#if GANALYTICS_DEBUG
      QList<QNetworkReply *> replies = _qnam.findChildren<QNetworkReply *>();
    for (QList<QNetworkReply *>::iterator it = replies.begin(); it != replies.end(); it++) {
      if ((*it)->isRunning())
        qDebug() << "~GAnalytics, request still running: " << (*it)->url().toString() << ", aborting.";
      //reply->deleteLater();
    }
#endif
    }

    // manual config of static fields
    void setClientID(QString clientID) { _clientID = clientID; }
    void setUserID(QString userID) { _userID = userID; }
    void setUserIPAddr(QString userIPAddr) { _userIPAddr = userIPAddr; }
    void setUserAgent(QString userAgent) { _userAgent = userAgent; }
    void setAppName(QString appName) { _appName = appName; }
    void setAppVersion(QString appVersion) { _appVersion = appVersion; }
    void setScreenResolution(QString screenResolution) { _screenResolution = screenResolution; }
    void setViewportSize(QString viewportSize) { _viewportSize = viewportSize; }
    void setUserLanguage(QString userLanguage) { _userLanguage = userLanguage; }
    QString getClientID() const { return _clientID; }
    QString getUserAgent() const { return _userAgent; }

    //
    // hit types
    //
    // - https://developers.google.com/analytics/devguides/collection/protocol/v1/devguide
    //

    // query processing in progress
    bool isBusy() const { return !_workingQueries.isEmpty(); }

public Q_SLOTS:

    // pageview
    void sendPageview(QString docHostname, QString page, QString title) {
      URL_QUERY params = build_metric("pageview");
      params.addQueryItem("dh", docHostname);    // document hostname
      params.addQueryItem("dp", page);            // page
      params.addQueryItem("dt", title);           // title
      send_metric(params);
    }

    // event
    void sendEvent(const QString& eventCategory = QString(),
                   const QString& eventAction = QString(),
                   const QString& eventLabel = QString(),
                   int eventValue = 0) {
      URL_QUERY params = build_metric("event");
      if (_appName.size()) params.addQueryItem("an", _appName); // mobile event app tracking
      if (_appVersion.size()) params.addQueryItem("av", _appVersion); // mobile event app tracking
      if (eventCategory.size()) params.addQueryItem("ec", eventCategory);
      if (eventAction.size()) params.addQueryItem("ea", eventAction);
      if (eventLabel.size()) params.addQueryItem("el", eventLabel);
      if (eventValue) params.addQueryItem("ev", QString::number(eventValue));
      send_metric(params);
    }

    // transaction
    void sendTransaction(QString transactionID, QString transactionAffiliation = "" /*...todo...*/) {
      URL_QUERY params = build_metric("transaction");
      params.addQueryItem("ti", transactionID);
      if (transactionAffiliation.size()) params.addQueryItem("ta", transactionAffiliation);
      send_metric(params);
    }

    // item
    void sendItem(QString itemName) {
      URL_QUERY params = build_metric("item");
      params.addQueryItem("in", itemName);
      //if (appName.size()) params.addQueryItem("an", appName);
      send_metric(params);
    }

    // social
    void sendSocial(QString socialNetwork, QString socialAction, QString socialActionTarget) {
      URL_QUERY params = build_metric("social");
      params.addQueryItem("sn", socialNetwork);
      params.addQueryItem("sa", socialAction);
      params.addQueryItem("st", socialActionTarget);
      send_metric(params);
    }

    // exception
    void sendException(QString exceptionDescription, bool exceptionFatal = true) {
      URL_QUERY params = build_metric("exception");
      if (exceptionDescription.size()) params.addQueryItem("exd", exceptionDescription);
      if (!exceptionFatal) params.addQueryItem("exf", "0");
      send_metric(params);
    }

    // timing
    void sendTiming(/* todo */) {
      URL_QUERY params = build_metric("timing");
      //if (appName.size()) params.addQueryItem("an", appName);
      send_metric(params);
    }

    // appview
    void sendAppview2(const QString& screenName, const QString& appName = QString(), const QString& appVersion = QString()) {
      URL_QUERY params = build_metric("appview");
      if (!appName.isEmpty()) params.addQueryItem("an", appName);
      else if (!_appName.isEmpty()) params.addQueryItem("an", _appName);
      if (!appVersion.isEmpty()) params.addQueryItem("av", appVersion);
      else if (!_appVersion.isEmpty()) params.addQueryItem("av", _appVersion);
      if (screenName.size()) params.addQueryItem("cd", screenName);
      send_metric(params);
    }

    // deprecated
    void sendAppview(const QString& appName, const QString& appVersion = QString(), const QString& screenName = QString()) {
      sendAppview2(screenName, appName, appVersion);
    }

    // custom dimensions / metrics
    // todo

    // experiment id / variant
    // todo

    //void startSession();

    // To ensure that query was sent before application quit, call waitForIdle()
    void endSession() {
      URL_QUERY params = build_metric("event");
      params.addQueryItem("sc", "end");
      send_metric(params);
    }

    // Waiting for any network operations complete. This method can be used with endSession
    // to ensure that query was completed before application was closed.
    void waitForIdle() {
      if (_waitLoop) {
        qCritical() << "Recursive call GAnalytics::waitForIdle";
        return;
      }

      QEventLoop loop;
      _waitLoop = &loop;
      loop.exec(QEventLoop::ExcludeUserInputEvents);
      _waitLoop = 0;
    }

signals:
    void busyChanged();

public:

    void generateUserAgentEtc() {
      QString locale = QLocale::system().name().toLower().replace("_", "-");
#ifdef __APPLE__
      QString machine = "Macintosh; Intel Mac OS X 10.9; ";
#endif
#ifdef __linux__
      QString machine = "X11; ";
      QProcess process;
      process.start("uname", QStringList() << "-sm");
      process.waitForFinished(-1); // wait forever until finished
      QString stdout = process.readAllStandardOutput();
      machine += stdout.simplified() + "; ";
#endif
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
      QString machine = "Windows; ";
#endif
      _userAgent = "Mozilla/5.0 (" + machine + locale + ") GAnalytics/1.0 (Qt/" QT_VERSION_STR " )";
      _userLanguage = locale;
#if defined(GANALYTICS_QTQUICK)
      QScreen* screen = qApp->primaryScreen();
    QString geom = QString::number(screen->geometry().width())
            + "x" + QString::number(screen->geometry().height());
    _screenResolution = geom;
#else
#ifdef QT_GUI_LIB
      QString geom = QString::number(QApplication::desktop()->screenGeometry().width())
                     + "x" + QString::number(QApplication::desktop()->screenGeometry().height());
      _screenResolution = geom;
#endif
#endif
#if GANALYTICS_DEBUG > 1
      qDebug() << "User-Agent:" << _userAgent;
    qDebug() << "Language:" << _userLanguage;
    qDebug() << "Screen Resolution:" << _screenResolution;
#endif
    }

private Q_SLOTS:

    void replyFinished(QNetworkReply * reply) {
      //qDebug() << "reply:" << reply->readAll().toHex();
      if (QNetworkReply::NoError != reply->error()) {
        qCritical() << "replyFinished error: " << reply->errorString() << "\n";
      }
      else {
        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        //qDebug() << "response code: " << httpStatus;
        if (httpStatus < 200 || httpStatus > 299) {
#if GANALYTICS_DEBUG
          qDebug() << "response code: " << httpStatus;
#endif
          _isFail = true;
        }
      }

      _workingQueries.removeAll(reply);
      if (_workingQueries.isEmpty()) {
        emit busyChanged();
        if (_waitLoop)
          _waitLoop->exit();
      }

      reply->deleteLater();
    }

    void replyError(QNetworkReply::NetworkError code) {
      qDebug() << "network error signal: " << code << "\n";
    }

private:
    GAnalytics(const GAnalytics &); // disable copy const constructor
    URL_QUERY build_metric(const QString& hitType) const {
      URL_QUERY params;
      // required in v1
      params.addQueryItem("v", "1" ); // version
      params.addQueryItem("tid", _trackingID);
      params.addQueryItem("cid", _clientID);
      params.addQueryItem("t", hitType);
      // optional
      if (_userID.size())
        params.addQueryItem("uid", _userID);
      if (_userIPAddr.size())
        params.addQueryItem("uip", _userIPAddr);
      if (_screenResolution.size())
        params.addQueryItem("sr", _screenResolution);
      if (_viewportSize.size())
        params.addQueryItem("vp", _viewportSize);
      if (_userLanguage.size())
        params.addQueryItem("ul", _userLanguage);
      //if (_userAgent.size())
      //  params.addQueryItem("ua", _userAgent);
      return params;
    }

    void send_metric(const URL_QUERY & params) {
      // when google has err'd us, then stop sending events!
      if (_isFail)
        return;
      QUrl collect_url("http://www.google-analytics.com/collect");
      QNetworkRequest request;
      if (_userAgent.size())
        request.setRawHeader("User-Agent", _userAgent.toUtf8());
      QNetworkReply * reply;
      if (_useGET) {
        // add cache-buster "z" to params
        //params.addQueryItem("z", QString::number(qrand()) );
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        collect_url.setQuery(params);
#else
        collect_url.setQueryItems(params.queryItems());
#endif
        request.setUrl(collect_url);
        reply = _qnam.get(request);
      }
      else {
        request.setUrl(collect_url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QByteArray postData = params.query(QUrl::FullyEncoded).toUtf8();
#else
        QByteArray postData = params.encodedQuery();
#endif
        reply = _qnam.post(request, postData);
      }
      _workingQueries << reply;
      if (_workingQueries.size() == 1)
              emit busyChanged();

      connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
              this, SLOT(replyError(QNetworkReply::NetworkError)));
#if GANALYTICS_DEBUG > 1
      qDebug() << "GAnalytics sent: " << params.toString();
#endif
      reply->setParent(&_qnam);
    }
    mutable QNetworkAccessManager _qnam;
    QString _trackingID;
    QString _clientID;
    bool _useGET; // true=GET, false=POST
    QString _userID;

    // various parameters:
    bool _anonymizeIP;
    bool _cacheBust;
    //
    QString _userIPAddr;
    QString _userAgent;
    QString _appName;
    QString _appVersion;
#if 0 // todo
    // traffic sources
  QString _documentReferrer;
  QString _campaignName;
  QString _campaignSource;
  QString _campaignMedium;
  QString _campaignKeyword;
  QString _campaignContent;
  QString _campaignID;
  QString _adwordsID;
  QString _displayAdsID;
#endif
    // system info
    QString _screenResolution;
    QString _viewportSize;
    QString _userLanguage;
    // etc...

    // internal
    bool _isFail;

    QList<QNetworkReply*> _workingQueries;
    QEventLoop* _waitLoop;
};
