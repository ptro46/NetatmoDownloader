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

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QVector>
#include <QSharedPointer>

#include <ostream>
using namespace std;

#include "TaskBotGetMeasures.h"

class ConfigModule {
private:
    QString             m_id;
    QString             m_name;
    long                m_startDate;
    QString             m_sqlPrefix;

    bool                m_isMainDevice;

public:
    ConfigModule(QString id,QString name, QString sqlPrefix);
    virtual ~ConfigModule();

    void                setId(QString id)               { m_id = id; }
    void                setName(QString name)           { m_name = name; }
    void                setStartDate(long startDate)    { m_startDate = startDate; }

    void                setIsMainDevice(bool b) { m_isMainDevice = b; }
    bool                isMainDevice() const    { return m_isMainDevice; }


    QString             id() const              { return m_id; }
    QString             name() const            { return m_name; }
    QString             sqlPrefix() const       { return m_sqlPrefix + m_name.toLower() + "_"; }
    long                startDate() const       { return m_startDate; }

    friend
    ostream& operator<<(ostream& o, const ConfigModule& rConfigModule);

};

inline
ostream& operator<<(ostream& o, const ConfigModule& rConfigModule) {
    o << "    id           : " << rConfigModule.m_id.toStdString() << endl;
    o << "    name         : " << rConfigModule.m_name.toStdString() << endl;
    o << "    sql_prefix   : " << rConfigModule.m_sqlPrefix.toStdString() << endl;
    o << "    main_device  : " << rConfigModule.m_isMainDevice << endl;

    return o;
}

class Config
{
private:
    QString                                 m_configFilename;

    QString                                 m_login;
    QString                                 m_password;
    QString                                 m_clientId;
    QString                                 m_clientSecret;
    QString                                 m_dbName;
    QString                                 m_mainDeviceId;

    int                                     m_indexMainDevice;
    int                                     m_indexOutdoorModule;
    int                                     m_indexRainModule;
    int                                     m_indexWindModule;
    int                                     m_indexIndoorModule;

    QVector<QSharedPointer<ConfigModule>>   m_vectConfigModules;

    QSharedPointer<ConfigModule>            m_moduleNetatmoMain;
    QSharedPointer<ConfigModule>            m_moduleNetatmoOutdoor;
    QSharedPointer<ConfigModule>            m_moduleNetatmoWind;
    QSharedPointer<ConfigModule>            m_moduleNetatmoRain;
    QVector<QSharedPointer<ConfigModule>>   m_vectModuleNetatmoIndoor;

    friend void TaskBotGetMeasures::onNetatmoStationsDataDownloadMeasures(int,QByteArray&);

    QSharedPointer<ConfigModule>    getConfigMainDeviceModule() const ;
    QSharedPointer<ConfigModule>    getConfigRainModule() const ;
    QSharedPointer<ConfigModule>    getConfigOutdoorModule() const ;
    QSharedPointer<ConfigModule>    getConfigWindModule() const ;
    QSharedPointer<ConfigModule>    getConfigIndoorModule() const ;

public:
    Config();
    virtual ~Config();

    QString                         getLogin() const ;
    QString                         getPassword() const ;
    QString                         getClientId() const ;
    QString                         getClientSecret() const ;
    QString                         getDbName() const ;
    QString                         getMainDeviceId() const ;

    void                            setModuleNetatmoMain(QSharedPointer<ConfigModule> from, QString id, QString name, long startDate);
    QSharedPointer<ConfigModule>    getModuleNetatmoMain() const ;

    void                            setModuleNetatmoOutdoor(QSharedPointer<ConfigModule> from, QString id, QString name, long startDate);
    QSharedPointer<ConfigModule>    getModuleNetatmoOutdoor() const ;

    void                            setModuleNetatmoWind(QSharedPointer<ConfigModule> from, QString id, QString name, long startDate);
    QSharedPointer<ConfigModule>    getModuleNetatmoWind() const ;

    void                            setModuleNetatmoRain(QSharedPointer<ConfigModule> from, QString id, QString name, long startDate);
    QSharedPointer<ConfigModule>    getModuleNetatmoRain() const ;

    void                            addModuleNetatmoIndoor(QSharedPointer<ConfigModule> from, QString id, QString name, long startDate);
    int                             getModuleNetatmoIndoorSize() const ;
    QSharedPointer<ConfigModule>    getModuleNetatmoIndoor(int index) const ;


    void    loadConfig(QString configFilename);
    void    addConfigModule(QSharedPointer<ConfigModule> module);

    int     configModuleCount() const { return m_vectConfigModules.size(); }
    QSharedPointer<ConfigModule> operator[](int index);

    friend
    ostream& operator<<(ostream& o, const Config& rConfig);
};

inline
QSharedPointer<ConfigModule>
Config::operator[](int index) {
    return m_vectConfigModules[index];
}

inline
QString
Config::getLogin() const {
    return m_login;
}

inline
QString
Config::getPassword() const {
    return m_password;
}

inline
QString
Config::getClientId() const {
    return m_clientId;
}

inline
QString
Config::getClientSecret() const {
    return m_clientSecret;
}

inline
QString
Config::getDbName() const {
    return m_dbName;
}

inline
QString
Config::getMainDeviceId() const {
    return m_mainDeviceId;
}

inline
QSharedPointer<ConfigModule>
Config::getConfigMainDeviceModule() const {
    return m_vectConfigModules[m_indexMainDevice];
}

inline
QSharedPointer<ConfigModule>
Config::getConfigRainModule() const {
    return m_vectConfigModules[m_indexRainModule];
}

inline
QSharedPointer<ConfigModule>
Config::getConfigOutdoorModule() const {
    return m_vectConfigModules[m_indexOutdoorModule];
}

inline
QSharedPointer<ConfigModule>
Config::getConfigWindModule() const {
    return m_vectConfigModules[m_indexWindModule];
}

inline
QSharedPointer<ConfigModule>
Config::getConfigIndoorModule() const {
    return m_vectConfigModules[m_indexIndoorModule];
}

inline
void
Config::setModuleNetatmoMain(QSharedPointer<ConfigModule> from, QString id, QString name, long startDate) {
    QSharedPointer<ConfigModule> module(new ConfigModule(*from.data()));
    m_moduleNetatmoMain = module ;
    m_moduleNetatmoMain->setId(id);
    m_moduleNetatmoMain->setIsMainDevice(true);
    m_moduleNetatmoMain->setName(name);
    m_moduleNetatmoMain->setStartDate(startDate);
}

inline
QSharedPointer<ConfigModule>
Config::getModuleNetatmoMain() const {
    return m_moduleNetatmoMain;
}

inline
void
Config::setModuleNetatmoOutdoor(QSharedPointer<ConfigModule> from, QString id, QString name, long startDate) {
    QSharedPointer<ConfigModule> module(new ConfigModule(*from.data()));
    m_moduleNetatmoOutdoor = module ;
    m_moduleNetatmoOutdoor->setId(id);
    m_moduleNetatmoOutdoor->setIsMainDevice(false);
    m_moduleNetatmoOutdoor->setName(name);
    m_moduleNetatmoOutdoor->setStartDate(startDate);
}

inline
QSharedPointer<ConfigModule>
Config::getModuleNetatmoOutdoor() const {
    return m_moduleNetatmoOutdoor;
}

inline
void
Config::setModuleNetatmoWind(QSharedPointer<ConfigModule> from, QString id, QString name, long startDate) {
    QSharedPointer<ConfigModule> module(new ConfigModule(*from.data()));
    m_moduleNetatmoWind = module ;
    m_moduleNetatmoWind->setId(id);
    m_moduleNetatmoWind->setIsMainDevice(false);
    m_moduleNetatmoWind->setName(name);
    m_moduleNetatmoWind->setStartDate(startDate);
}

inline
QSharedPointer<ConfigModule>
Config::getModuleNetatmoWind() const {
    return m_moduleNetatmoWind;
}

inline
void
Config::setModuleNetatmoRain(QSharedPointer<ConfigModule> from, QString id, QString name, long startDate) {
    QSharedPointer<ConfigModule> module(new ConfigModule(*from.data()));
    m_moduleNetatmoRain = module ;
    m_moduleNetatmoRain->setId(id);
    m_moduleNetatmoRain->setIsMainDevice(false);
    m_moduleNetatmoRain->setName(name);
    m_moduleNetatmoRain->setStartDate(startDate);
}

inline
QSharedPointer<ConfigModule>
Config::getModuleNetatmoRain() const {
    return m_moduleNetatmoRain;
}

inline
void
Config::addModuleNetatmoIndoor(QSharedPointer<ConfigModule> from, QString id, QString name, long startDate) {
    QSharedPointer<ConfigModule> module(new ConfigModule(*from.data()));
    module = module ;
    module->setId(id);
    module->setIsMainDevice(false);
    module->setName(name);
    module->setStartDate(startDate);
    m_vectModuleNetatmoIndoor << module ;
}

inline
int
Config::getModuleNetatmoIndoorSize() const {
    return m_vectModuleNetatmoIndoor.size();
}

inline
QSharedPointer<ConfigModule>
Config::getModuleNetatmoIndoor(int index) const {
    return m_vectModuleNetatmoIndoor[index];
}


inline
ostream& operator<<(ostream& o, const Config& rConfig) {
    o << "Login        : " << rConfig.m_login.toStdString() << endl;
    o << "Password     : " << rConfig.m_password.toStdString() << endl ;
    o << "clientId     : " << rConfig.m_clientId.toStdString() << endl ;
    o << "clientSecret : " << rConfig.m_clientSecret.toStdString() << endl ;
    o << "dbName       : " << rConfig.m_dbName.toStdString() << endl ;
    o << "mainDevice   : " << rConfig.m_mainDeviceId.toStdString() << endl ;

    QVector<QSharedPointer<ConfigModule>>::const_iterator itModules = rConfig.m_vectConfigModules.begin();
    while ( itModules != rConfig.m_vectConfigModules.end() ) {
        QSharedPointer<ConfigModule> module = *itModules++;
        o << *module ;
    }
    return o ;
}

#endif // CONFIG_H
