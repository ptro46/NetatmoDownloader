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
#ifndef ABSTRACTWS_H
#define ABSTRACTWS_H

#include <QObject>
#include <QNetworkReply>
#include <qtimer.h>

#define NETWORK_TIMEOUT 15

typedef enum {
    Get = 0,
    Post = 1,
    Put = 2,
    Delete = 3
} HttpMethod ;

class AbstractWS : public QObject
{
    Q_OBJECT
public:
    explicit AbstractWS(QObject *parent = 0);
    virtual ~AbstractWS();
    virtual void destroy();
    virtual void start();

protected:
    virtual const QString           getURL() const = 0;
    virtual void                    onSucceeded(int httpCode,QByteArray& contentResult ) const = 0;
    virtual void                    onFailed(int httpCode,QByteArray& contentResult) const = 0;

    virtual HttpMethod              getHttpMethod() const;
    virtual QMap<QString,QString>   getHttpHeaders() const;
    virtual QString                 getRequestContent() const;

private:
    void                            enableNetworkTimeout();
    void                            disableNetworkTimeout();

private slots:
    void downLoadFinished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void sslErrors ( const QList<QSslError> &errors );
    void downloadTimeout();

private:
    QNetworkAccessManager m_networkManager;
    QNetworkReply *m_reply;
    QTimer m_networkTimer;
};

#endif // ABSTRACTWS_H
