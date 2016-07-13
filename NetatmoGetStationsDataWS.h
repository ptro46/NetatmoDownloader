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

#ifndef NETATMOGETSTATIONSDATAWS_H
#define NETATMOGETSTATIONSDATAWS_H

#include "AbstractWS.h"

class NetatmoGetStationsDataDelegate
{
public:
    virtual void onNetatmoStationsDataSucceeded(int httpCode,QByteArray& contentResult )  = 0;
    virtual void onNetatmoStationsDataFailed(int httpCode,QByteArray& contentResult)  = 0;
};

class NetatmoGetStationsDataWS : public AbstractWS
{
    Q_OBJECT
public:
    explicit NetatmoGetStationsDataWS(QString token,
                                      QString deviceId,
                                      NetatmoGetStationsDataDelegate* delegate);
    explicit NetatmoGetStationsDataWS(QString token,
                                      NetatmoGetStationsDataDelegate* delegate);
    virtual ~NetatmoGetStationsDataWS();

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
    QString                             m_deviceId;
    NetatmoGetStationsDataDelegate   *  m_pStationsDataDelegate;

signals:
    void    finished();

public slots:

};

#endif // NETATMOGETSTATIONSDATAWS_H
