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

#include "NetatmoAuthWS.h"
#include "Config.h"

extern Config gConfig;

NetatmoAuthWS::NetatmoAuthWS(QString username,
                             QString password,
                             NetatmoAuthWSDelegate* delegate)
    :AbstractWS(),m_username(username),m_password(password),m_pAuthDelegate(delegate)
{

}

NetatmoAuthWS::~NetatmoAuthWS() {

}

const QString NetatmoAuthWS::getURL() const {
    return "https://api.netatmo.com/oauth2/token";
}

void NetatmoAuthWS::onSucceeded(int httpCode,QByteArray& contentResult ) const {
    if ( NULL != m_pAuthDelegate ) {
        m_pAuthDelegate->onNetatmoAuthSucceeded(httpCode,contentResult);
    }
}

void NetatmoAuthWS::onFailed(int httpCode,QByteArray& contentResult) const {
    if ( NULL != m_pAuthDelegate ) {
        m_pAuthDelegate->onNetatmoAuthFailed(httpCode,contentResult);
    }
}

HttpMethod NetatmoAuthWS::getHttpMethod() const {
    return Post;
}

QMap<QString,QString>
NetatmoAuthWS::getHttpHeaders() const {
    QMap<QString,QString> headers ;

    headers["Content-Type"] = "application/x-www-form-urlencoded;charset=UTF-8";

    return headers ;
}

QString
NetatmoAuthWS::getRequestContent() const {
    QString requestContent ;

    requestContent += "grant_type=password&scope=read_station&client_id=";
    requestContent += gConfig.getClientId() ;
    requestContent += "&client_secret=";
    requestContent += gConfig.getClientSecret() ;
    requestContent += "&username=";
    requestContent += m_username ;
    requestContent += "&password=";
    requestContent += m_password ;

    qDebug() << requestContent ;

    return requestContent ;
}
