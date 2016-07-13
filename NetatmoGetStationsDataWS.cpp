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

#include "NetatmoGetStationsDataWS.h"

NetatmoGetStationsDataWS::NetatmoGetStationsDataWS(QString token,
                                                   QString deviceId,
                                                   NetatmoGetStationsDataDelegate* delegate)
    :AbstractWS(),m_token(token),m_deviceId(deviceId),m_pStationsDataDelegate(delegate)
{

}

NetatmoGetStationsDataWS::NetatmoGetStationsDataWS(QString token,
                                                   NetatmoGetStationsDataDelegate* delegate)
    :AbstractWS(),m_token(token),m_deviceId(""),m_pStationsDataDelegate(delegate)
{

}

NetatmoGetStationsDataWS::~NetatmoGetStationsDataWS() {

}

const QString NetatmoGetStationsDataWS::getURL() const {
    return "https://api.netatmo.com/api/getstationsdata";
}

void NetatmoGetStationsDataWS::onSucceeded(int httpCode,QByteArray& contentResult ) const {
    if ( NULL != m_pStationsDataDelegate ) {
        m_pStationsDataDelegate->onNetatmoStationsDataSucceeded(httpCode,contentResult);
    }
}

void NetatmoGetStationsDataWS::onFailed(int httpCode,QByteArray& contentResult) const {
    if ( NULL != m_pStationsDataDelegate ) {
        m_pStationsDataDelegate->onNetatmoStationsDataFailed(httpCode,contentResult);
    }
}

HttpMethod NetatmoGetStationsDataWS::getHttpMethod() const {
    return Post;
}

QMap<QString,QString>
NetatmoGetStationsDataWS::getHttpHeaders() const {
    QMap<QString,QString> headers ;

    headers["Content-Type"] = "application/x-www-form-urlencoded;charset=UTF-8";

    return headers ;
}

QString
NetatmoGetStationsDataWS::getRequestContent() const {
    QString requestContent ;

    requestContent += "access_token=";
    requestContent += m_token ;
    if ( m_deviceId.length() > 0 ) {
        requestContent += "&device_id=";
        requestContent += m_deviceId ;
    }
    return requestContent ;
}
