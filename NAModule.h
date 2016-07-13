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

#ifndef NAMODULE_H
#define NAMODULE_H

#include <QString>
#include <QDateTime>
#include <iostream>
using namespace std;

QString timestamp2StringDate(const long timestamp) ;

enum class ModuleType : int {
    Main, Outdoor, Wind, Rain, Opt
} ;

inline QString moduleTypeAsString(ModuleType moduleType) {
    switch( moduleType) {
    case ModuleType::Main :     return "Base";
    case ModuleType::Outdoor :  return "Module Extérieur";
    case ModuleType::Wind :     return "Anémomètre";
    case ModuleType::Rain :     return "Pluviomètre";
    case ModuleType::Opt :      return "Module Intérieur Optionnel";
    }
    return "";
}

class NAModule
{
private:
    QString                     m_id;
    QString                     m_moduleName;
    ModuleType                  m_moduleType;

    long                        m_firmware;

    long                        m_lastSetup;
    long                        m_lastSeen;
    long                        m_lastMessage;

public:
    NAModule(QString id, QString moduleName, ModuleType moduleType);
    ~NAModule();

    void setFirmware(long d)                    { m_firmware = d; }

    void setLastSetup(long d)                   { m_lastSetup = d; }
    void setLastSeen(long d)                    { m_lastSeen = d; }
    void setLastMessage(long d)                 { m_lastMessage = d; }

    friend
    ostream& operator<<(ostream& o, const NAModule& rModule);

};

inline
ostream& operator<<(ostream& o, const NAModule& rModule) {
    o << "    " << moduleTypeAsString(rModule.m_moduleType).toStdString() << " " << rModule.m_moduleName.toStdString() << " " << rModule.m_id.toStdString() << " firmware(" << rModule.m_firmware << ")" << endl ;
    o << "        lastSetup   (" << timestamp2StringDate(rModule.m_lastSetup).toStdString() << ")" << endl ;
    o << "        lastSeen    (" << timestamp2StringDate(rModule.m_lastSeen).toStdString() << ")" << endl ;
    o << "        lastMessage (" << timestamp2StringDate(rModule.m_lastMessage).toStdString() << ")" << endl ;
    return o;
}

inline
QString
timestamp2StringDate(const long timestamp) {
    QDateTime d;

    d = QDateTime::fromTime_t(timestamp);
    return d.toString("yyyy-MM-dd hh:mm:ss");
}

#endif // NAMODULE_H
