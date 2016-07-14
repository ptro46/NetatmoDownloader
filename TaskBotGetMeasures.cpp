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

#include "TaskBotGetMeasures.h"

#include <QSqlQuery>
#include <QSqlError>

#include <iostream>
using namespace std;

#include "Config.h"

#include "NADevice.h"
#include "NAModule.h"

Config gConfig;

TaskBotGetMeasures::TaskBotGetMeasures(bool showConfig, QString configFileName, QObject *parent)
    :QObject(parent) {

    m_showConfig = showConfig;

    m_configFileName = configFileName.trimmed() ;
    if ( m_configFileName.length() == 0 ) {
        m_configFileName = "config.json";
    }

    cout << "Using " << m_configFileName.toStdString() << endl ;

    m_limitCount = 0 ;
    initCounters();

    gConfig.loadConfig(m_configFileName);
    cout << gConfig << endl ;

    m_pNetatmoAuthWS = QSharedPointer<NetatmoAuthWS>( new NetatmoAuthWS(gConfig.getLogin(),
                                                                        gConfig.getPassword(),
                                                                        this) );
}

TaskBotGetMeasures::~TaskBotGetMeasures() {

    if ( m_db.isOpen() ) {
        m_db.close();
    }
    QSqlDatabase::removeDatabase("QSQLITE");
}

void
TaskBotGetMeasures::initCounters() {
    m_maxPluviometrieRequests = 3 ;
    m_pluviometrieRequestsCount = 0 ;

    m_maxDeviceInDoorRequests = 3 ;
    m_deviceInDoorRequestsCount = 0 ;

    m_maxDeviceOutDoorRequests = 3 ;
    m_deviceOutDoorRequestsCount = 0 ;

    m_maxDeviceWindRequests = 3 ;
    m_deviceWindRequestsCount = 0 ;

    m_maxDeviceAddInDoorRequests = 3 ;
    m_deviceAddInDoorRequestsCount = 0 ;

    m_bCanContinueWithRain = false ;
    m_bCanContinueWithInDoor = false ;
    m_bCanContinueWithOutDoor = false ;
    m_bCanContinueWithWind = false ;
    m_bCanContinueWithAddInDoor = false ;
}


void
TaskBotGetMeasures::run() {
    if (QSqlDatabase::connectionNames().isEmpty()) {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(gConfig.getDbName());
    }

    if ( ! m_db.open() ) {
        cout << "Cannot open database" << endl ;
        emit finished();
        return ;
    } else {
        QSqlQuery checkDbQuery(m_db);
        if ( false == checkDbQuery.exec("select idt,start_timestamp from botlogs") ) {
            cout << "Create DB" << endl ;
            QSqlQuery query(m_db);
            if ( (true == query.exec("create table botlogs(idt integer primary key autoincrement,start_timestamp int8,start_date text,stop_timestamp int8,stop_date text,main_device_count int,outdoor_device_count int,rain_module_count int,wind_module_count int,opt_module_count int)")) &&
                 (true == query.exec("create table module_logs(idt text,begin_timestamp int8,begin_date text,end_timestamp int8,end_date text,nb_measures_inserted int)"))) {

            } else {
                QSqlError error = checkDbQuery.lastError() ;
                qDebug() << error << endl;

                cout << "CAN NOT create DB" << endl ;
                emit finished();
                return ;
            }
        } else {
            cout << "DB Exist" << endl ;
        }

        m_pNetatmoAuthWS->start();

        QEventLoop loop ;
        connect(this,SIGNAL(finished()),&loop,SLOT(quit()));
    }

}

bool
TaskBotGetMeasures::readPreviousLogs() {
    bool res = false ;
    QSqlError error;
    QSqlQuery query(m_db);


    if ( query.exec("select "+ BotLogs::botlogsColumnNamesWithIdt  + " from botlogs order by idt desc limit 1") ) {
        if ( query.next() ) {
            res = true ;
            int i=0;
            m_previousBotLogs.idt = query.value(i++).toInt();

            m_previousBotLogs.start_timestamp = query.value(i++).toInt();
            m_previousBotLogs.start_date = query.value(i++).toString();
            m_previousBotLogs.stop_timestamp = query.value(i++).toInt();
            m_previousBotLogs.stop_date = query.value(i++).toString();

            m_previousBotLogs.main_device_count = query.value(i++).toInt();
            m_previousBotLogs.outdoor_device_count = query.value(i++).toInt();
            m_previousBotLogs.rain_module_count = query.value(i++).toInt();
            m_previousBotLogs.wind_module_count = query.value(i++).toInt();
            m_previousBotLogs.opt_module_count = query.value(i++).toInt();
        }

        if ( query.exec("select distinct(idt) from module_logs") ) {
            QVector<QString> arrayDistinctIdt ;
            while ( query.next() ) {
                arrayDistinctIdt << query.value(0).toString();
            }

            QVector<QString>::iterator itIdt = arrayDistinctIdt.begin();
            while ( itIdt != arrayDistinctIdt.end() ) {
                QString idt = *itIdt++;
                QString sqlStatement = "select "+ ModuleLogs::moduleColumnNamesWithIdt  + " from module_logs where idt='" + idt + "' order by begin_timestamp desc limit 1";
                qDebug() << sqlStatement ;
                if ( query.prepare("select "+ ModuleLogs::moduleColumnNamesWithIdt  + " from module_logs where idt=:idt order by begin_timestamp desc limit 1") ) {
                    query.bindValue(":idt",idt);
                    if ( query.exec() ) {
                        if ( query.next() ) {
                            ModuleLogs* pModuleLogs = new ModuleLogs() ;

                            int i=0;

                            pModuleLogs->idt = query.value(i++).toString(); ;
                            pModuleLogs->begin_timestamp = query.value(i++).toInt();
                            pModuleLogs->begin_date = query.value(i++).toString();
                            pModuleLogs->end_timestamp = query.value(i++).toInt();
                            pModuleLogs->end_date = query.value(i++).toString();
                            pModuleLogs->nb_measures_inserted = query.value(i++).toInt();

                            m_previousModuleLogs[idt] = QSharedPointer<ModuleLogs>(pModuleLogs) ;
                        }
                    } else {
                        res = false ;
                        error = query.lastError() ;
                        qDebug() << error << endl;

                    }
                } else {
                    res = false ;
                    error = query.lastError() ;
                    qDebug() << error << endl;

                }
            }

        } else {
            res = false ;
            error = query.lastError() ;
            qDebug() << error << endl;

        }
    } else {
        res = false ;
        error = query.lastError() ;
        qDebug() << error << endl;

    }
    return res;
}

bool
TaskBotGetMeasures::persistCurrentLogs() {
    QDateTime d;

    d = QDateTime::fromTime_t(m_currentBotLogs.start_timestamp);
    m_currentBotLogs.start_date = d.toString("yyyy-MM-dd hh:mm:ss");
    d = QDateTime::fromTime_t(m_currentBotLogs.stop_timestamp);
    m_currentBotLogs.stop_date = d.toString("yyyy-MM-dd hh:mm:ss");

    QSqlQuery query(m_db);
    QString sql = "insert into botlogs(" + BotLogs::botlogsColumnNamesWithoutIdt + ") values(" + BotLogs::botlogsColumnBindsWithoutIdt + ")";
    query.prepare(sql);

    query.bindValue(":start_timestamp",m_currentBotLogs.v_start_timestamp());
    query.bindValue(":start_date",m_currentBotLogs.start_date);
    query.bindValue(":stop_timestamp",m_currentBotLogs.v_stop_timestamp());
    query.bindValue(":stop_date",m_currentBotLogs.stop_date);

    query.bindValue(":main_device_count",m_currentBotLogs.v_main_device_count());
    query.bindValue(":outdoor_device_count",m_currentBotLogs.v_outdoor_device_count());
    query.bindValue(":rain_module_count",m_currentBotLogs.v_rain_module_count());
    query.bindValue(":wind_module_count",m_currentBotLogs.v_wind_module_count());
    query.bindValue(":opt_module_count",m_currentBotLogs.v_opt_module_count());
    query.exec();

    foreach( QString idt, m_currentModuleLogs.keys() ) {
        QSharedPointer<ModuleLogs> module = m_currentModuleLogs[idt];
        persistModulesLogs(module);
    }

    cout << "current logs " << m_currentBotLogs << endl ;
    foreach( QString idt, m_currentModuleLogs.keys() ) {
        cout << (*m_currentModuleLogs[idt]) << endl ;
    }

    return true ;
}

bool
TaskBotGetMeasures::persistModulesLogs(QSharedPointer<ModuleLogs> module) {
    QDateTime d;

    d = QDateTime::fromTime_t(module->begin_timestamp);
    module->begin_date = d.toString("yyyy-MM-dd hh:mm:ss");
    d = QDateTime::fromTime_t(module->end_timestamp);
    module->end_date = d.toString("yyyy-MM-dd hh:mm:ss");

    QSqlQuery query(m_db);
    QString sql = "insert into module_logs(" + ModuleLogs::moduleColumnNamesWithIdt + ") values(" + ModuleLogs::moduleColumnBindsWithIdt + ")";
    query.prepare(sql);

    query.bindValue(":idt",module->idt);
    query.bindValue(":begin_timestamp",module->v_begin_timestamp());
    query.bindValue(":begin_date",module->begin_date);
    query.bindValue(":end_timestamp",module->v_end_timestamp());
    query.bindValue(":end_date",module->end_date);
    query.bindValue(":nb_measures_inserted",module->v_nb_measures_inserted());
    query.exec();

    return true;
}

void
TaskBotGetMeasures::createMeasuresTables() {
    QSqlQuery query(m_db);

    QString indoorMainPrefix = gConfig.getModuleNetatmoMain()->sqlPrefix() ;
    {
        QString sql = "create table if not exists " + indoorMainPrefix + "temp(timestamp int8,value real)";
        cout << sql.toStdString() << endl ;
        query.exec(sql);
    }
    {
        QString sql = "create table if not exists " + indoorMainPrefix + "co2(timestamp int8,value real)";
        cout << sql.toStdString() << endl ;
        query.exec(sql);
    }
    {
        QString sql = "create table if not exists " + indoorMainPrefix + "humidity(timestamp int8,value real)";
        cout << sql.toStdString() << endl ;
        query.exec(sql);
    }
    {
        QString sql = "create table if not exists " + indoorMainPrefix + "noise(timestamp int8,value real)";
        cout << sql.toStdString() << endl ;
        query.exec(sql);
    }
    {
        QString sql = "create table if not exists " + indoorMainPrefix + "pressure(timestamp int8,value real)";
        cout << sql.toStdString() << endl ;
        query.exec(sql);
    }


    QString outdoorPrefix = gConfig.getModuleNetatmoOutdoor()->sqlPrefix() ;
    {
        QString sql = "create table if not exists " + outdoorPrefix + "temp(timestamp int8,value real)";
        cout << sql.toStdString() << endl ;
        query.exec(sql);
    }
    {
        QString sql = "create table if not exists " + outdoorPrefix + "humidity(timestamp int8,value real)";
        cout << sql.toStdString() << endl ;
        query.exec(sql);
    }

    if ( ! gConfig.getModuleNetatmoRain().isNull() ) {
        QString rainPrefix = gConfig.getModuleNetatmoRain()->sqlPrefix() ;
        {
            QString sql = "create table if not exists " + rainPrefix + "rain(timestamp int8,value real)";
            cout << sql.toStdString() << endl ;
            query.exec(sql);
        }
    }

    if ( ! gConfig.getModuleNetatmoWind().isNull() ) {
        QString windPrefix = gConfig.getModuleNetatmoWind()->sqlPrefix() ;
        {
            QString sql = "create table if not exists " + windPrefix + "wind_strength(timestamp int8,value real)";
            cout << sql.toStdString() << endl ;
            query.exec(sql);
        }
        {
            QString sql = "create table if not exists " + windPrefix + "wind_angle(timestamp int8,value real)";
            cout << sql.toStdString() << endl ;
            query.exec(sql);
        }
        {
            QString sql = "create table if not exists " + windPrefix + "gust_strength(timestamp int8,value real)";
            cout << sql.toStdString() << endl ;
            query.exec(sql);
        }
        {
            QString sql = "create table if not exists " + windPrefix + "gust_angle(timestamp int8,value real)";
            cout << sql.toStdString() << endl ;
            query.exec(sql);
        }
    }

    for(int i=0; i<gConfig.getModuleNetatmoIndoorSize(); i++) {
        QString indoorPrefix = gConfig.getModuleNetatmoIndoor(i)->sqlPrefix() ;
        {
            QString sql = "create table if not exists " + indoorPrefix + "temp(timestamp int8,value real)";
            cout << sql.toStdString() << endl ;
            query.exec(sql);
        }
        {
            QString sql = "create table if not exists " + indoorPrefix + "co2(timestamp int8,value real)";
            cout << sql.toStdString() << endl ;
            query.exec(sql);
        }
        {
            QString sql = "create table if not exists " + indoorPrefix + "humidity(timestamp int8,value real)";
            cout << sql.toStdString() << endl ;
            query.exec(sql);
        }

    }
}

void
TaskBotGetMeasures::insertInto(QString tablename, long timestamp, double value) {
    QVariant vTimestamp = (unsigned long long)timestamp;
    QVariant vValue = value ;
    QSqlQuery query(m_db);
    QString sql = "insert into " + tablename + "(timestamp,value) values(:timestamp,:value)";
    query.prepare(sql);
    query.bindValue(":timestamp",vTimestamp);
    query.bindValue(":value",vValue);
    query.exec();
}

//---
//  NetatmoAuthWSDelegate
void
TaskBotGetMeasures::onNetatmoAuthSucceeded(int httpCode,QByteArray& contentResult ) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    QJsonDocument d = QJsonDocument::fromJson(contentResult);
    QJsonObject map = d.object();

    QJsonValue valueExpiresIn = map["expires_in"];
    int iValueExpiresIn = -1 ;
    if ( (! valueExpiresIn.isNull()) && valueExpiresIn.isDouble() ) {
        iValueExpiresIn = valueExpiresIn.toInt();
    } else {
        cout << "expires_in is null or not double" << endl ;
    }

    QJsonValue valueAccessToken = map["access_token"];
    QString stringAccessToken ;
    if ( (! valueAccessToken.isNull()) && valueAccessToken.isString() ) {
        stringAccessToken = valueAccessToken.toString();
    } else {
        cout << "access_token is null or not string" << endl ;
    }

    QJsonValue valueRefreshToken = map["refresh_token"];
    QString stringRefreshToken ;
    if ( (! valueRefreshToken.isNull()) && valueRefreshToken.isString() ) {
        stringRefreshToken = valueRefreshToken.toString();
    } else {
        cout << "refresh_token is null or not string" << endl ;
    }

    cout << "Token:" << stringAccessToken.toStdString()
         << " RefreshToken:" << stringRefreshToken.toStdString()
         << " expiresIn:" << iValueExpiresIn
         << endl ;

    m_token = stringAccessToken;

    if ( m_showConfig == true ) {
        m_pNetatmoStationsDataWS = QSharedPointer<NetatmoGetStationsDataWS>( new NetatmoGetStationsDataWS(stringAccessToken,
                                                                                                          this) );
    } else {
        m_pNetatmoStationsDataWS = QSharedPointer<NetatmoGetStationsDataWS>( new NetatmoGetStationsDataWS(stringAccessToken,
                                                                                                          gConfig.getMainDeviceId(),
                                                                                                          this) );
    }
    m_pNetatmoStationsDataWS->start();
    //    emit finished();
}

void
TaskBotGetMeasures::onNetatmoAuthFailed(int httpCode,QByteArray& contentResult)  {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    cout << "TaskBotGetMeasures::onNetatmoAuthFailed " << httpCode << endl ;

    emit finished();
}

QString
TaskBotGetMeasures::normName(QString moduleName) const {
    QString r = moduleName.toLower().trimmed().simplified() ;

    r = r.replace(QRegExp("[àáâäæãåā]"),"a");
    r = r.replace(QRegExp("[èéêëēėę]"),"e");
    r = r.replace(QRegExp("[îïíīįì]"),"i");
    r = r.replace(QRegExp("[ôöòóœøōõ]"),"o");
    r = r.replace(QRegExp("[ûüùúū]"),"u");
    r = r.replace(QRegExp("[ÿ]"),"y");
    r = r.replace(QRegExp("[çćč]"),"c");
    r = r.replace(QRegExp(" "),"");

    return r;
}

NAModule*
TaskBotGetMeasures::moduleFromJSON(QJsonObject& dictOfOneModule) {
    NAModule *module = NULL;
    QString id = dictOfOneModule["_id"].toString();
    QString moduleType = dictOfOneModule["type"].toString();
    QString moduleName = dictOfOneModule["module_name"].toString();

    if ( moduleType == "NAModule1" ) {
        module = new NAModule(id,moduleName,ModuleType::Outdoor);

    } else if ( moduleType == "NAModule2" ) {
        module = new NAModule(id,moduleName,ModuleType::Wind);

    } else if ( moduleType == "NAModule3" ) {
        module = new NAModule(id,moduleName,ModuleType::Rain);

    } else if ( moduleType == "NAModule4" ) {
        module = new NAModule(id,moduleName,ModuleType::Opt);

    }

    module->setFirmware(dictOfOneModule["firmware"].toDouble());
    module->setLastSetup(dictOfOneModule["last_setup"].toDouble());
    module->setLastSeen(dictOfOneModule["last_seen"].toDouble());
    module->setLastMessage(dictOfOneModule["last_message"].toDouble());

    return module;
}

NADevice*
TaskBotGetMeasures::deviceFromJSON(QJsonObject& dictOfOneDevice) {
    NADevice* device;

    QString id = dictOfOneDevice["_id"].toString();
    QString deviceName = dictOfOneDevice["module_name"].toString();
    QString stationName = dictOfOneDevice["station_name"].toString();

    device = new NADevice(id, deviceName, stationName) ;
    device->setFirmware(dictOfOneDevice["firmware"].toDouble());
    device->setDateSetup(dictOfOneDevice["date_setup"].toDouble());
    device->setLastSetup(dictOfOneDevice["last_setup"].toDouble());
    device->setLastStatusStore(dictOfOneDevice["last_status_store"].toDouble());
    device->setLastUpgrade(dictOfOneDevice["last_upgrade"].toDouble());

    QJsonObject placeDict = dictOfOneDevice["place"].toObject() ;
    device->setAltitude(placeDict["altitude"].toDouble());
    device->setCity(placeDict["city"].toString());
    device->setCountry(placeDict["country"].toString());

    QJsonArray locationArray = placeDict["location"].toArray() ;
    device->setLatitude(locationArray[0].toDouble());
    device->setLongitude(locationArray[1].toDouble());

    QJsonArray arrayOfModules = dictOfOneDevice["modules"].toArray() ;
    for(auto item:arrayOfModules) {
        QJsonObject dictOfOneModule = item.toObject();

        (*device) << moduleFromJSON(dictOfOneModule) ;
    }
    return device;
}

void
TaskBotGetMeasures::onNetatmoStationsDataShowConfig(int httpCode,QByteArray& contentResult ) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    QSharedPointer<NADevice>                mainDevice;
    QVector<QSharedPointer<NADevice>>       vectFriendDevices;

    QJsonDocument d = QJsonDocument::fromJson(contentResult);
    QJsonObject map = d.object();

    QJsonObject bodyDict = map["body"].toObject();
    QJsonArray arrayDevices = bodyDict["devices"].toArray();
    for(auto oneDevice : arrayDevices) {
        QJsonObject dictOfOneDevice = oneDevice.toObject();

        if ( dictOfOneDevice.contains("friend_users") ) {
            mainDevice = QSharedPointer<NADevice>(deviceFromJSON(dictOfOneDevice)) ;

        } else {
            vectFriendDevices << QSharedPointer<NADevice>(deviceFromJSON(dictOfOneDevice)) ;

        }
    }

    cout << "MainDevice " << endl ;
    cout << (*mainDevice.data()) << endl ;

    QVector<QSharedPointer<NADevice>>::const_iterator itDevices = vectFriendDevices.begin();
    while ( itDevices != vectFriendDevices.end() ) {
        QSharedPointer<NADevice> device = *itDevices++;
        cout << *device << endl ;
    }

    emit finished();
}

void
TaskBotGetMeasures::onNetatmoStationsDataDownloadMeasures(int httpCode,QByteArray& contentResult ) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    QString stringDeviceId;
    long mainDeviceSetup;

    QJsonDocument d = QJsonDocument::fromJson(contentResult);
    QJsonObject map = d.object();

    QJsonObject bodyDict = map["body"].toObject();
    QJsonArray arrayDevices = bodyDict["devices"].toArray();
    QJsonObject mainDevice = arrayDevices[0].toObject();
    stringDeviceId = mainDevice["_id"].toString();
    mainDeviceSetup = (long)mainDevice["date_setup"].toDouble();
    QString mainDeviceName = normName(mainDevice["module_name"].toString());

    gConfig.setModuleNetatmoMain(gConfig.getConfigMainDeviceModule(),
                                 stringDeviceId,
                                 mainDeviceName,
                                 mainDeviceSetup);

    QJsonArray arrayOfModules = mainDevice["modules"].toArray() ;
    for(auto item:arrayOfModules) {
        QJsonObject dictOfOneModule = item.toObject();
        QString moduleType = dictOfOneModule["type"].toString();
        QString moduleName = normName(dictOfOneModule["module_name"].toString());
        if ( moduleType == "NAModule1" ) {
            QString stringOutdoorModuleId = dictOfOneModule["_id"].toString();
            long outdoorLastSetup = (long)dictOfOneModule["last_setup"].toDouble();
            gConfig.setModuleNetatmoOutdoor(gConfig.getConfigOutdoorModule(),
                                            stringOutdoorModuleId,
                                            moduleName,
                                            outdoorLastSetup);

        } else if ( moduleType == "NAModule2" ) {
            QString stringWindModuleId = dictOfOneModule["_id"].toString();
            long windLastSetup = (long)dictOfOneModule["last_setup"].toDouble();
            gConfig.setModuleNetatmoWind(gConfig.getConfigWindModule(),
                                         stringWindModuleId,
                                         moduleName,
                                         windLastSetup);

        } else if ( moduleType == "NAModule3" ) {
            QString stringRainModuleId = dictOfOneModule["_id"].toString();
            long rainLastSetup = (long)dictOfOneModule["last_setup"].toDouble();
            gConfig.setModuleNetatmoRain(gConfig.getConfigRainModule(),
                                         stringRainModuleId,
                                         moduleName,
                                         rainLastSetup);


        } else if ( moduleType == "NAModule4" ) {
            QString stringIndoorAddModuleId = dictOfOneModule["_id"].toString();
            long indoorAddLastSetup = (long)dictOfOneModule["last_setup"].toDouble();
            gConfig.addModuleNetatmoIndoor(gConfig.getConfigIndoorModule(),
                                           stringIndoorAddModuleId,
                                           moduleName,
                                           indoorAddLastSetup);

        }
    }

    QDateTime currentDateTime = QDateTime::currentDateTime();

    createMeasuresTables();

    if ( true == readPreviousLogs() ) {
        cout << "Found previous logs " << m_previousBotLogs << endl ;
        foreach( QString idt, m_previousModuleLogs.keys() ) {
            cout << (*m_previousModuleLogs[idt]) << endl ;
        }
    } else {
        cout << "Previous logs does't exist start with modules dates" << endl ;
    }

    QSharedPointer<ConfigModule> configMainModule = gConfig.getModuleNetatmoMain();
    QSharedPointer<ModuleLogs>   previousMainState = m_previousModuleLogs[configMainModule->id()] ;
    if ( ! previousMainState.isNull() ) {
        configMainModule->setStartDate(previousMainState->end_timestamp);
    }

    m_currentBotLogs.start_timestamp = currentDateTime.toTime_t();

    QSharedPointer<ModuleLogs> moduleLogs = m_currentModuleLogs[gConfig.getModuleNetatmoMain()->id()] ;
    if ( ! moduleLogs.isNull() ) {
        gConfig.getModuleNetatmoMain()->setStartDate(moduleLogs->end_timestamp);
    }

    cout << gConfig.getModuleNetatmoMain()->id().toStdString() << " Get initial indoor measures from " << gConfig.getModuleNetatmoMain()->startDate() << endl ;
    m_pNetatmoDeviceWS = QSharedPointer<NetatmoGetDeviceMeasuresWS>( new NetatmoGetDeviceMeasuresWS(m_token,
                                                                                                    gConfig.getModuleNetatmoMain()->startDate(),
                                                                                                    gConfig.getModuleNetatmoMain()->id(),
                                                                                                    this) );

    m_pNetatmoDeviceWS->start();
}

//---
//  NetatmoGetStationsDataDelegate
void
TaskBotGetMeasures::onNetatmoStationsDataSucceeded(int httpCode,QByteArray& contentResult ) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    if ( m_showConfig == true ) {
        onNetatmoStationsDataShowConfig(httpCode, contentResult);

    } else {
        onNetatmoStationsDataDownloadMeasures(httpCode, contentResult);
    }
}

void
TaskBotGetMeasures::onNetatmoStationsDataFailed(int httpCode,QByteArray& contentResult)  {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    cout << "TaskBotGetMeasures::onNetatmoStationsDataFailed " << httpCode << endl ;

    emit finished();
}


