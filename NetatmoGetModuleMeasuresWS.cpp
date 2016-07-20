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

#include "NetatmoGetModuleMeasuresWS.h"

#include <iostream>
using namespace std;

NetatmoGetModuleMeasuresWS::NetatmoGetModuleMeasuresWS(QString token,
                                                       long startDate,
                                                       QString deviceId,
                                                       QString moduleId,
                                                       NetatmoGetModuleMeasuresDelegate* delegate,
                                                       long measureInterval)
    :AbstractWS(),m_token(token),m_startDate(startDate),m_deviceId(deviceId),m_moduleId(moduleId),m_pModuleDelegate(delegate),m_measureInterval(measureInterval)
{

}

NetatmoGetModuleMeasuresWS::~NetatmoGetModuleMeasuresWS() {

}

const QString
NetatmoGetModuleMeasuresWS::getURL() const {
    return "https://api.netatmo.com/api/getmeasure";
}

void
NetatmoGetModuleMeasuresWS::onSucceeded(int httpCode,QByteArray& contentResult ) const {
    if ( NULL != m_pModuleDelegate ) {
        m_pModuleDelegate->onNetatmoModuleMeasuresSucceeded(httpCode,contentResult);
    }
}

void
NetatmoGetModuleMeasuresWS::onFailed(int httpCode,QByteArray& contentResult) const {
    if ( NULL != m_pModuleDelegate ) {
        m_pModuleDelegate->onNetatmoModuleMeasuresFailed(httpCode,contentResult);
    }
}

HttpMethod
NetatmoGetModuleMeasuresWS::getHttpMethod() const {
    return Post;
}

QMap<QString,QString>
NetatmoGetModuleMeasuresWS::getHttpHeaders() const {
    QMap<QString,QString> headers ;

    headers["Content-Type"] = "application/x-www-form-urlencoded;charset=UTF-8";

    return headers ;
}

QString
NetatmoGetModuleMeasuresWS::getRequestContent() const {
    QString requestContent ;

    requestContent += "access_token=";
    requestContent += m_token ;
    requestContent += "&device_id=";
    requestContent += m_deviceId ;
    requestContent += "&module_id=";
    requestContent += m_moduleId ;
    requestContent += "&scale=max&type=Temperature,Humidity&date_begin=";
    requestContent += QString::number(m_startDate) ;
    requestContent += "&date_end=";
    requestContent += QString::number(m_startDate + m_measureInterval) ;
    requestContent += "&optimize=false&real_time=true";

    QDateTime dStart = QDateTime::fromTime_t(m_startDate);
    QDateTime dEnd   = QDateTime::fromTime_t(m_startDate + m_measureInterval);

    /*
    qDebug() << "POST " << requestContent ;
    qDebug() << "From : " << m_startDate << "(" << dStart.toString("yyyy-MM-dd hh:mm:ss") << ")" ;
    qDebug() << "To   : " << (m_startDate + m_measureInterval) << "(" << dEnd.toString("yyyy-MM-dd hh:mm:ss") << ")" ;
    */
    return requestContent ;
}
