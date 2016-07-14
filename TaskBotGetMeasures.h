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

#ifndef TASKBOTGETMEASURES_H
#define TASKBOTGETMEASURES_H

#include <QObject>
#include <QtCore>
#include <QSqlDatabase>

#include "NetatmoAuthWS.h"
#include "NetatmoGetStationsDataWS.h"

#include "NetatmoGetDeviceMeasuresWS.h"
#include "NetatmoGetModuleMeasuresWS.h"
#include "NetatmoGetPluviometrieWS.h"
#include "NetatmoGetWindWS.h"
#include "NetatmoGetModuleAdditionnelWS.h"

#include "DeviceMeasures.h"

#include "BotLogs.h"

class Config;
class NADevice;
class NAModule;

extern Config gConfig;

class TaskBotGetMeasures : public QObject,
        public NetatmoAuthWSDelegate,
        public NetatmoGetStationsDataDelegate,
        public NetatmoGetPluviometrieDelegate,
        public NetatmoGetDeviceMeasuresDelegate,
        public NetatmoGetModuleMeasuresDelegate,
        public NetatmoGetWindDelegate,
        public NetatmoGetModuleAdditionnelDelegate
{
    Q_OBJECT
public:
    explicit TaskBotGetMeasures(bool showConfig, QString configFileName, QObject *parent = 0);
    virtual ~TaskBotGetMeasures();

    //---
    //  NetatmoAuthWSDelegate
    void onNetatmoAuthSucceeded(int httpCode,QByteArray& contentResult ) ;
    void onNetatmoAuthFailed(int httpCode,QByteArray& contentResult)  ;

    //---
    //  NetatmoGetStationsDataDelegate
    void onNetatmoStationsDataSucceeded(int httpCode,QByteArray& contentResult ) ;
    void onNetatmoStationsDataFailed(int httpCode,QByteArray& contentResult)  ;

    //---
    //  NetatmoGetPluviometrieDelegate
    void onNetatmoPluviometrieSucceeded(int httpCode,QByteArray& contentResult )  ;
    void onNetatmoPluviometrieFailed(int httpCode,QByteArray& contentResult)  ;

    //---
    //  NetatmoGetDeviceMeasuresDelegate
    void onNetatmoDeviceMeasuresSucceeded(int httpCode,QByteArray& contentResult ) ;
    void onNetatmoDeviceMeasuresFailed(int httpCode,QByteArray& contentResult) ;

    //---
    //  NetatmoGetModuleMeasuresDelegate
    void onNetatmoModuleMeasuresSucceeded(int httpCode,QByteArray& contentResult ) ;
    void onNetatmoModuleMeasuresFailed(int httpCode,QByteArray& contentResult) ;

    //---
    //  NetatmoGetWindDelegate
    void onNetatmoWindSucceeded(int httpCode,QByteArray& contentResult ) ;
    void onNetatmoWindFailed(int httpCode,QByteArray& contentResult) ;

    //---
    //  NetatmoGetModuleAdditionnelDelegate
    void onNetatmoModuleAdditionnelSucceeded(int httpCode,QByteArray& contentResult, int currentModuleIndex) ;
    void onNetatmoModuleAdditionnelFailed(int httpCode,QByteArray& contentResult, int currentModuleIndex) ;

signals:
    void    finished();

public slots:
    void    run();

private:
    bool    readPreviousLogs();
    bool    persistCurrentLogs();
    bool    persistModulesLogs(QSharedPointer<ModuleLogs> module);
    void    createMeasuresTables();
    void    insertInto(QString tablename, long timestamp, double value);

private:
    bool                            m_showConfig;
    QString                         m_configFileName;
    QSqlDatabase                    m_db;

    QSharedPointer<NetatmoAuthWS>                  m_pNetatmoAuthWS;
    QSharedPointer<NetatmoGetStationsDataWS>       m_pNetatmoStationsDataWS;

    QSharedPointer<NetatmoGetDeviceMeasuresWS>     m_pNetatmoDeviceWS;
    QSharedPointer<NetatmoGetModuleMeasuresWS>     m_pNetatmoModulesWS;
    QSharedPointer<NetatmoGetPluviometrieWS>       m_pNetatmoPluviometrieWS;
    QSharedPointer<NetatmoGetWindWS>               m_pNetatmoWindWS;
    QSharedPointer<NetatmoGetModuleAdditionnelWS>  m_pNetatmoModuleAdditionnelWS;

    BotLogs                                         m_previousBotLogs;
    BotLogs                                         m_currentBotLogs;

    QMap<QString,QSharedPointer<ModuleLogs>>        m_previousModuleLogs;
    QMap<QString,QSharedPointer<ModuleLogs>>        m_currentModuleLogs;

    QString                                         m_token;

    int                                             m_limitCount;

    //---
    //  Pluviometrie
    int                                             m_pluviometrieRequestsCount;
    int                                             m_maxPluviometrieRequests;
    bool                                            m_bCanContinueWithRain;

    //---
    //  Indoor Device
    int                                             m_deviceInDoorRequestsCount;
    int                                             m_maxDeviceInDoorRequests;
    bool                                            m_bCanContinueWithInDoor;

    //---
    //  Outdoor Device
    int                                             m_deviceOutDoorRequestsCount;
    int                                             m_maxDeviceOutDoorRequests;
    bool                                            m_bCanContinueWithOutDoor;

    //---
    //  Wind Device
    int                                             m_deviceWindRequestsCount;
    int                                             m_maxDeviceWindRequests;
    bool                                            m_bCanContinueWithWind;

    //---
    //  Additionnal Indoor Device
    int                                             m_deviceAddInDoorRequestsCount;
    int                                             m_maxDeviceAddInDoorRequests;
    int                                             m_bCanContinueWithAddInDoor;

    void    endTaskOrContinue();
    void    initCounters();
    QString normName(QString moduleName) const ;

    void onNetatmoStationsDataShowConfig(int httpCode,QByteArray& contentResult ) ;

    NADevice* deviceFromJSON(QJsonObject& dictOfOneDevice);
    NAModule* moduleFromJSON(QJsonObject& dictOfOneModule);
public:
    void onNetatmoStationsDataDownloadMeasures(int httpCode,QByteArray& contentResult ) ;
};

#endif // TASKBOTGETMEASURES_H
