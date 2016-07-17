//
// This file is part of the Netatmo Downloader project
//
// Copyright (C)   2016  Patrice Trognon
//
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE VERSION 3 as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU LESSER GENERAL PUBLIC LICENSE VERSION 3 for more details.
//
// You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE VERSION 3
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//

#include "AbstractWS.h"
#include "WSLimitManager.h"
#include "Config.h"
#include <iostream>
using namespace std;

AbstractWS::AbstractWS(QObject *parent) : QObject(parent),m_reply(NULL) {
    connect(&m_networkTimer,SIGNAL(timeout()),this,SLOT(downloadTimeout()));
}

AbstractWS::~AbstractWS() {
    if (NULL!= m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = NULL;
    }
}

void AbstractWS::destroy() {
    if (NULL!= m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = NULL;
    }
    deleteLater();
}

void AbstractWS::start() {
    QUrl url(getURL());
    QNetworkRequest request(url);
    QString jsonString;
    QByteArray jsonContent;
    QByteArray postDataSize;
    QMap<QString,QString> headers;

    if (NULL!=m_reply) {
        QByteArray empty;

        onFailed(0,empty);
        return;
    }

    jsonString = getRequestContent();
    jsonContent=jsonString.toUtf8();

    postDataSize = QByteArray::number(jsonContent.size());
    request.setRawHeader("Content-Length", postDataSize);

    headers=getHttpHeaders();
    foreach( QString key, headers.keys() ) {
        request.setRawHeader(key.toUtf8(), headers.value( key ).toUtf8());
    }

    pgLimitManager->addWebService(QDateTime::currentDateTime().toTime_t(),
                                  getHttpMethod(),
                                  getURL(),
                                  jsonContent);

    long waitLimit10Seconds = pgLimitManager->waitTime10SecondsLimit();
    if ( waitLimit10Seconds > 0 ) {
        cout << "Limit "
             << gConfig.getNetatmoPerUserLimitEvery10Seconds()
             << " requests every 10 seconds expected, sleep "
             << waitLimit10Seconds
             << "seconds"
             << endl ;
        QThread::sleep(waitLimit10Seconds);
    } else {
        long waitLimitHour = pgLimitManager->waitTimeHourLimit();
        if ( waitLimitHour > 0 ) {
            cout << "Limit "
                 << gConfig.getNetatmoPerUserLimitEveryHour()
                 << " requests every hour expected, sleep "
                 << waitLimitHour
                 << "seconds"
                 << endl ;
            QThread::sleep(waitLimitHour);
        }
    }

    switch(getHttpMethod()) {
        case Get:
            m_reply = m_networkManager.get(request);
        break;

        case Post:
            m_reply = m_networkManager.post(request, jsonContent);
        break;

        case Put:
            m_reply = m_networkManager.put(request, jsonContent);
        break;

        case Delete:
            m_reply = m_networkManager.deleteResource(request);
        break;
    }

    connect(m_reply, SIGNAL(sslErrors ( const QList<QSslError> & ) ), this, SLOT(sslErrors ( const QList<QSslError> & ) ));
    connect(m_reply, SIGNAL(finished()), this, SLOT(downLoadFinished()));
    connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
    connect(m_reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(uploadProgress(qint64, qint64)));
}

HttpMethod
AbstractWS::getHttpMethod() const {
    return Get;
}

QMap<QString,QString>
AbstractWS::getHttpHeaders() const {
    QMap<QString,QString> result;
    result.insert("Accept-Encoding","gzip");
    return result;
}

QString
AbstractWS::getRequestContent() const {
    return QByteArray();
}

void
AbstractWS::enableNetworkTimeout() {
    m_networkTimer.stop();
    m_networkTimer.start(1000*NETWORK_TIMEOUT);
}

void
AbstractWS::disableNetworkTimeout() {
    m_networkTimer.stop();
}

void
AbstractWS::downLoadFinished() {
    QVariant statusCode;
    QByteArray contentResult;

    disableNetworkTimeout();

    statusCode = m_reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );

    if (statusCode.isValid() ) {
        int httpCode;

        httpCode = statusCode.toInt();

        QByteArray encoding=m_reply->rawHeader("Content-Encoding");

        QString sba = QString(encoding);

        contentResult = m_reply->readAll();

        if ((httpCode== 200) || (httpCode== 201) || (httpCode== 204) ) {
            onSucceeded(httpCode,contentResult);
        } else {
            onFailed(httpCode,contentResult);
        }
    } else {
        if ( m_reply ) {
            QNetworkReply::NetworkError error=m_reply->error();
            qDebug() << "NetworkError :" << error << endl ;
        } else {

        }
        onFailed(0,contentResult);
    }
}

void
AbstractWS::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    Q_UNUSED(bytesReceived)
    Q_UNUSED(bytesTotal)
    enableNetworkTimeout();
}

void
AbstractWS::uploadProgress(qint64 bytesSent, qint64 bytesTotal) {
    Q_UNUSED(bytesSent)
    Q_UNUSED(bytesTotal)
    enableNetworkTimeout();
}

void
AbstractWS::sslErrors ( const QList<QSslError> &errors ) {
    int size=errors.size();
    for (int i=0;i<size;i++) {
        qDebug() << "Erreur SSL : " << errors.at(i).errorString().toUtf8().data() << endl ;
    }
}

void
AbstractWS::downloadTimeout() {
    m_reply->abort();
}


