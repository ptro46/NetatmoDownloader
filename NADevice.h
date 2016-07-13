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

#ifndef NADEVICE_H
#define NADEVICE_H

#include <QString>
#include <QVector>
#include <QSharedPointer>
#include <iostream>
using namespace std;

#include "NAModule.h"

class NADevice
{
private:
    QString                     m_id;
    QString                     m_deviceName;
    QString                     m_stationName;

    long                        m_firmware;

    long                        m_dateSetup;
    long                        m_lastSetup;
    long                        m_lastStatusStore;
    long                        m_lastUpgrade;

    long                        m_altitude;
    QString                     m_city;
    QString                     m_country;
    double                      m_latitude;
    double                      m_longitude;

    QVector<QSharedPointer<NAModule>>     m_vectModules;

    friend
    ostream& operator<<(ostream& o, const NADevice& rDevice);

public:
    NADevice(QString id, QString deviceName, QString stationName);
    virtual ~NADevice();

    void setFirmware(long d)                    { m_firmware = d; }

    void setDateSetup(long d)                   { m_dateSetup = d; }
    void setLastSetup(long d)                   { m_lastSetup = d; }
    void setLastStatusStore(long d)             { m_lastStatusStore = d; }
    void setLastUpgrade(long d)                 { m_lastUpgrade = d; }

    void setAltitude(long d)                    { m_altitude = d; }
    void setCity(QString city)                  { m_city = city; }
    void setCountry(QString country)            { m_country = country; }
    void setLatitude(double latitude)           { m_latitude = latitude; }
    void setLongitude(double longitude)         { m_longitude = longitude; }

    NADevice& operator<<(NAModule* pModule);

};

inline
NADevice& NADevice::operator<<(NAModule* pModule) {
    m_vectModules << QSharedPointer<NAModule>(pModule);
    return *this;
}

inline
ostream& operator<<(ostream& o, const NADevice& rDevice) {

    o <<  rDevice.m_stationName.toStdString() << " " << rDevice.m_deviceName.toStdString() << " " << rDevice.m_id.toStdString() << " firmware(" << rDevice.m_firmware << ")" << endl ;
    o << "    " << rDevice.m_country.toStdString() << " " << rDevice.m_city.toStdString() << " lat,lng,alt(" << rDevice.m_latitude << "," << rDevice.m_longitude << "," << rDevice.m_altitude << ")" << endl ;
    o << "    setup           (" << timestamp2StringDate(rDevice.m_dateSetup).toStdString() << ")" << endl ;
    o << "    lastSetup       (" << timestamp2StringDate(rDevice.m_lastSetup).toStdString() << ")" << endl ;
    o << "    lastStatusStore (" << timestamp2StringDate(rDevice.m_lastStatusStore).toStdString() << ")" << endl ;
    o << "    lastUpgrade     (" << timestamp2StringDate(rDevice.m_lastUpgrade).toStdString() << ")" << endl ;

    QVector<QSharedPointer<NAModule>>::const_iterator itModules = rDevice.m_vectModules.begin();
    while ( itModules != rDevice.m_vectModules.end() ) {
        QSharedPointer<NAModule> module = *itModules++;
        o << *module ;
    }
    return o;
}


#endif // NADEVICE_H
