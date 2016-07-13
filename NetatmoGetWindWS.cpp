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

#include "NetatmoGetWindWS.h"

#include <iostream>
using namespace std;

NetatmoGetWindWS::NetatmoGetWindWS(QString token,
                                   long startDate,
                                   QString deviceId,
                                   QString moduleId,
                                   NetatmoGetWindDelegate* delegate)
    :AbstractWS(),m_token(token),m_startDate(startDate),m_deviceId(deviceId),m_moduleId(moduleId),m_pWindDelegate(delegate)
{

}

NetatmoGetWindWS::~NetatmoGetWindWS() {

}

const QString
NetatmoGetWindWS::getURL() const {
    return "https://api.netatmo.com/api/getmeasure";
}

void
NetatmoGetWindWS::onSucceeded(int httpCode,QByteArray& contentResult ) const {
    if ( NULL != m_pWindDelegate ) {
        m_pWindDelegate->onNetatmoWindSucceeded(httpCode,contentResult);
    }
}

void
NetatmoGetWindWS::onFailed(int httpCode,QByteArray& contentResult) const {
    if ( NULL != m_pWindDelegate ) {
        m_pWindDelegate->onNetatmoWindFailed(httpCode,contentResult);
    }
}

HttpMethod
NetatmoGetWindWS::getHttpMethod() const {
    return Post;
}

QMap<QString,QString>
NetatmoGetWindWS::getHttpHeaders() const {
    QMap<QString,QString> headers ;

    headers["Content-Type"] = "application/x-www-form-urlencoded;charset=UTF-8";

    return headers ;
}

QString
NetatmoGetWindWS::getRequestContent() const {
    QString requestContent ;

    requestContent += "access_token=";
    requestContent += m_token ;
    requestContent += "&device_id=";
    requestContent += m_deviceId ;
    requestContent += "&module_id=";
    requestContent += m_moduleId ;
    requestContent += "&scale=max&type=WindStrength,WindAngle,GustStrength,GustAngle&date_begin=";
    requestContent += QString::number(m_startDate) ;
    requestContent += "&date_end=";
    requestContent += QString::number(m_startDate + WIND_MEASURE_INTERVAL) ;
    requestContent += "&optimize=false&real_time=true";

    QDateTime dStart = QDateTime::fromTime_t(m_startDate);
    QDateTime dEnd   = QDateTime::fromTime_t(m_startDate + WIND_MEASURE_INTERVAL);

    /*
    qDebug() << "POST " << requestContent ;
    qDebug() << "From : " << m_startDate << "(" << dStart.toString("yyyy-MM-dd hh:mm:ss") << ")" ;
    qDebug() << "To   : " << (m_startDate + Wind_MEASURE_INTERVAL) << "(" << dEnd.toString("yyyy-MM-dd hh:mm:ss") << ")" ;
    */
    return requestContent ;
}
