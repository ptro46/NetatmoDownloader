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

#ifndef NETATMOGETDEVICEMEASURESWS_H
#define NETATMOGETDEVICEMEASURESWS_H

#include "AbstractWS.h"

#define DEVICE_MEASURE_INTERVAL        (1024*300)

class NetatmoGetDeviceMeasuresDelegate
{
public:
    virtual void onNetatmoDeviceMeasuresSucceeded(int httpCode,QByteArray& contentResult )  = 0;
    virtual void onNetatmoDeviceMeasuresFailed(int httpCode,QByteArray& contentResult)  = 0;
};

class NetatmoGetDeviceMeasuresWS : public AbstractWS
{
    Q_OBJECT
public:
    explicit NetatmoGetDeviceMeasuresWS(QString token,
                                        long startDate,
                                        QString deviceId,
                                        NetatmoGetDeviceMeasuresDelegate* delegate,
                                        long measureInterval = DEVICE_MEASURE_INTERVAL);
    virtual ~NetatmoGetDeviceMeasuresWS();

    long    getStartDate() const            { return m_startDate; }
    long    getMeasureInterval() const      { return m_measureInterval; }

protected:
    virtual const QString getURL() const;

    // Override
    virtual void onSucceeded(int httpCode,QByteArray& contentResult ) const;
    virtual void onFailed(int httpCode,QByteArray& contentResult) const;

    HttpMethod              getHttpMethod() const;
    QMap<QString,QString>   getHttpHeaders() const;
    QString                 getRequestContent() const;

private:
    QString                                 m_token;
    long                                    m_startDate;
    QString                                 m_deviceId;
    NetatmoGetDeviceMeasuresDelegate   *    m_pDeviceDelegate;
    long                                    m_measureInterval;

signals:
    void    finished();

public slots:
};

#endif // NETATMOGETDEVICEMEASURESWS_H
