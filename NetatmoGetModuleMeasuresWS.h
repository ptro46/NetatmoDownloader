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

#ifndef NETATMOGETMODULEMEASURESWS_H
#define NETATMOGETMODULEMEASURESWS_H

#include "AbstractWS.h"

#define MODULE_MEASURE_INTERVAL        (1024*300)

class NetatmoGetModuleMeasuresDelegate
{
public:
    virtual void onNetatmoModuleMeasuresSucceeded(int httpCode,QByteArray& contentResult )  = 0;
    virtual void onNetatmoModuleMeasuresFailed(int httpCode,QByteArray& contentResult)  = 0;
};

class NetatmoGetModuleMeasuresWS : public AbstractWS
{
    Q_OBJECT
public:
    explicit NetatmoGetModuleMeasuresWS(QString token,
                                        long startDate,
                                        QString deviceId,
                                        QString moduleId,
                                        NetatmoGetModuleMeasuresDelegate* delegate,
                                        long measureInterval = MODULE_MEASURE_INTERVAL);
    virtual ~NetatmoGetModuleMeasuresWS();

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
    QString                             m_token;
    long                                m_startDate;
    QString                             m_deviceId;
    QString                             m_moduleId;
    NetatmoGetModuleMeasuresDelegate *  m_pModuleDelegate;
    long                                m_measureInterval;

signals:
    void    finished();

public slots:

};

#endif // NETATMOGETMODULEMEASURESWS_H
