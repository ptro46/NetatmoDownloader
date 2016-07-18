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

#include "SortFunctions.h"

#include "Config.h"

//---
//  NetatmoGetModuleMeasuresDelegate
void
TaskBotGetMeasures::onNetatmoModuleMeasuresSucceeded(int httpCode,QByteArray& contentResult ) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    m_deviceOutDoorRequestsCount++;

    QVector<QSharedPointer<TemperatureMeasure>>     arrayTemperatureMeasures;
    QVector<QSharedPointer<HumidityMeasure>>        arrayHumidityMeasures;

    QJsonDocument d = QJsonDocument::fromJson(contentResult);
    QJsonObject map = d.object();

    QJsonObject bodyDict = map["body"].toObject();
    QStringList allKeys = bodyDict.keys();
    QStringList::const_iterator constIt = allKeys.constBegin();
    QDateTime currentDateTime = QDateTime::currentDateTime();
    long firstTimestamp = currentDateTime.toTime_t();
    long lastTimestamp = 0L;
    int realCount = 0;
    while ( constIt != allKeys.constEnd() ) {
        realCount++;
        QString key = *constIt++;
        long timestamp = key.toLong();

        if ( timestamp > lastTimestamp ) {
            lastTimestamp = timestamp ;
        }

        if ( timestamp < firstTimestamp ) {
            firstTimestamp = timestamp ;
        }

        QJsonArray arrayOfValues = bodyDict[key].toArray();

        double valueTemperature = arrayOfValues[0].toDouble();
        arrayTemperatureMeasures << QSharedPointer<TemperatureMeasure>(new TemperatureMeasure(timestamp,valueTemperature)) ;

        double valueHumidity = arrayOfValues[1].toDouble();
        arrayHumidityMeasures << QSharedPointer<HumidityMeasure>(new HumidityMeasure(timestamp,valueHumidity)) ;

    }

    if ( lastTimestamp == 0L ) {
        gConfig.getModuleNetatmoOutdoor()->setStartDate(gConfig.getModuleNetatmoOutdoor()->startDate() + MODULE_MEASURE_INTERVAL);
        lastTimestamp = gConfig.getModuleNetatmoOutdoor()->startDate();
    } else {
        gConfig.getModuleNetatmoOutdoor()->setStartDate(lastTimestamp) ;

        m_db.transaction();
        qSort(arrayTemperatureMeasures.begin(),arrayTemperatureMeasures.end(),temperatureMeasureLessThan);
        {
            QVector<QSharedPointer<TemperatureMeasure>>::iterator itValues = arrayTemperatureMeasures.begin();
            while ( itValues != arrayTemperatureMeasures.constEnd() ) {
                auto measure = *itValues++;
                QString tableName = gConfig.getModuleNetatmoOutdoor()->sqlPrefix() + "temp";
                insertInto(tableName,measure->getTimestamp(),measure->getValue());
            }
        }
        m_db.commit();

        m_db.transaction();
        qSort(arrayHumidityMeasures.begin(),arrayHumidityMeasures.end(),humidityMeasureLessThan);
        {
            QVector<QSharedPointer<HumidityMeasure>>::iterator itValues = arrayHumidityMeasures.begin();
            while ( itValues != arrayHumidityMeasures.constEnd() ) {
                auto measure = *itValues++;
                QString tableName = gConfig.getModuleNetatmoOutdoor()->sqlPrefix() + "humidity";
                insertInto(tableName,measure->getTimestamp(),measure->getValue());
            }
        }
        m_db.commit();
    }

    QSharedPointer<ModuleLogs> moduleLogs = m_currentModuleLogs[gConfig.getModuleNetatmoOutdoor()->id()];
    if ( moduleLogs.isNull() ) {
        ModuleLogs* pModuleLogs = new ModuleLogs();
        pModuleLogs->idt = gConfig.getModuleNetatmoOutdoor()->id();
        pModuleLogs->begin_timestamp = firstTimestamp;
        pModuleLogs->end_timestamp = lastTimestamp;
        pModuleLogs->nb_measures_inserted = realCount ;

        m_currentModuleLogs[gConfig.getModuleNetatmoOutdoor()->id()] = QSharedPointer<ModuleLogs>(pModuleLogs);
        m_currentBotLogs.outdoor_device_count = pModuleLogs->nb_measures_inserted;

    } else {
        moduleLogs->end_timestamp = lastTimestamp;
        moduleLogs->nb_measures_inserted += realCount ;
        m_currentBotLogs.outdoor_device_count = moduleLogs->nb_measures_inserted;
    }

    QDateTime dFirst = QDateTime::fromTime_t(firstTimestamp);
    QDateTime dLast  = QDateTime::fromTime_t(lastTimestamp);

    qDebug() << "Count add(" << realCount << ")" ;
    qDebug() << "First : " << QString::number(firstTimestamp) << "(" << dFirst.toString("yyyy-MM-dd hh:mm:ss") << ")" ;
    qDebug() << "Last  : " << QString::number(lastTimestamp)  << "(" << dLast.toString("yyyy-MM-dd hh:mm:ss") << ")" << endl;

    currentDateTime = QDateTime::currentDateTime();
    uint t = currentDateTime.toTime_t();

    if ( lastTimestamp <= t - 3600 * 12 ) {
        m_bCanContinueWithOutDoor = true;
    } else {
        m_bCanContinueWithOutDoor = false;
    }

    if ( (lastTimestamp <= t - 3600 * 12) && (m_deviceOutDoorRequestsCount < m_maxDeviceOutDoorRequests) ) {
        cout << gConfig.getModuleNetatmoOutdoor()->id().toStdString() << "    Get next outdoor measures from " << gConfig.getModuleNetatmoOutdoor()->startDate() << endl ;
        m_pNetatmoModulesWS = QSharedPointer<NetatmoGetModuleMeasuresWS>( new NetatmoGetModuleMeasuresWS(m_token,
                                                                                                         gConfig.getModuleNetatmoOutdoor()->startDate(),
                                                                                                         gConfig.getModuleNetatmoMain()->id(),
                                                                                                         gConfig.getModuleNetatmoOutdoor()->id(),
                                                                                                         this) );
        m_pNetatmoModulesWS->start(&m_db);

    } else {
        cout << "Finish get outdoor measures " << endl ;

        if ( gConfig.getModuleNetatmoRain().isNull() ) {
            cout << "Rain module is not present" << endl ;
        } else {
            cout << "Rain module is present" << endl ;
        }

        if ( gConfig.getModuleNetatmoWind().isNull() ) {
            cout << "Wind module is not present" << endl ;
        } else {
            cout << "Wind module is present" << endl ;
        }

        if ( gConfig.getModuleNetatmoIndoorSize() > 0 ) {
            cout << "IndoorAdditionnal module is not present" << endl ;
        } else {
            cout << "IndoorAdditionnal module is present" << endl ;
        }

        if ( ! gConfig.getModuleNetatmoRain().isNull() ) {
            QSharedPointer<ModuleLogs> moduleLogs = m_previousModuleLogs[gConfig.getModuleNetatmoRain()->id()] ;
            if ( ! moduleLogs.isNull() ) {
                gConfig.getModuleNetatmoRain()->setStartDate(moduleLogs->end_timestamp);
            }

            cout << gConfig.getModuleNetatmoRain()->id().toStdString() << " Get initial rain measures from " << gConfig.getModuleNetatmoRain()->startDate() << endl ;

            m_pNetatmoPluviometrieWS = QSharedPointer<NetatmoGetPluviometrieWS>( new NetatmoGetPluviometrieWS(m_token,
                                                                                                                gConfig.getModuleNetatmoRain()->startDate(),
                                                                                                                gConfig.getModuleNetatmoMain()->id(),
                                                                                                                gConfig.getModuleNetatmoRain()->id(),
                                                                                                                this) );
            m_pNetatmoPluviometrieWS->start(&m_db);

        } else if ( ! gConfig.getModuleNetatmoWind().isNull() ) {
            QSharedPointer<ModuleLogs> moduleLogs = m_previousModuleLogs[gConfig.getModuleNetatmoWind()->id()] ;
            if ( ! moduleLogs.isNull() ) {
                gConfig.getModuleNetatmoWind()->setStartDate(moduleLogs->end_timestamp);
            }

            cout << gConfig.getModuleNetatmoWind()->id().toStdString() << " Get initial wind measures from " << gConfig.getModuleNetatmoWind()->startDate() << endl ;

            m_pNetatmoWindWS = QSharedPointer<NetatmoGetWindWS>( new NetatmoGetWindWS(m_token,
                                                                                        gConfig.getModuleNetatmoWind()->startDate(),
                                                                                        gConfig.getModuleNetatmoMain()->id(),
                                                                                        gConfig.getModuleNetatmoWind()->id(),
                                                                                        this) );
            m_pNetatmoWindWS->start(&m_db);

        } else if ( gConfig.getModuleNetatmoIndoorSize() > 0 ) {
            QSharedPointer<ModuleLogs> moduleLogs = m_previousModuleLogs[gConfig.getModuleNetatmoIndoor(0)->id()] ;
            if ( ! moduleLogs.isNull() ) {
                gConfig.getModuleNetatmoIndoor(0)->setStartDate(moduleLogs->end_timestamp);
            }

            cout << gConfig.getModuleNetatmoIndoor(0)->id().toStdString() << " Get initial module addtionnel (0) measures from " << gConfig.getModuleNetatmoIndoor(0)->startDate() << endl ;

            m_pNetatmoModuleAdditionnelWS = QSharedPointer<NetatmoGetModuleAdditionnelWS>( new NetatmoGetModuleAdditionnelWS(m_token,
                                                                                                                              gConfig.getModuleNetatmoIndoor(0)->startDate(),
                                                                                                                              gConfig.getModuleNetatmoMain()->id(),
                                                                                                                              gConfig.getModuleNetatmoIndoor(0)->id(),
                                                                                                                              0,
                                                                                                                              this) );
            m_pNetatmoModuleAdditionnelWS->start(&m_db);

        } else {
            endTaskOrContinue();

        }

    }
}

void
TaskBotGetMeasures::onNetatmoModuleMeasuresFailed(int httpCode,QByteArray& contentResult) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    QString content = contentResult ;

    cout << "TaskBotGetMeasures::onNetatmoModuleMeasuresFailed " << httpCode << " " << content.toStdString() << endl ;

    m_currentBotLogs.stop_timestamp = QDateTime::currentDateTime().toTime_t();
    persistCurrentLogs();

    emit finished();
}

#define SLEEP_TIME_TWO_TASKS (10)

void
TaskBotGetMeasures::endTaskOrContinue() {
    if ( gConfig.getLimit() > 0 ) {
        m_limitCount++;

        if ( m_limitCount < gConfig.getLimit() ) {
            initCounters();

            cout << endl << endl << "Pass " << (m_limitCount+1) << "/" << gConfig.getLimit() << " sleep " << SLEEP_TIME_TWO_TASKS << " seconds" << endl ;
            QThread::sleep(SLEEP_TIME_TWO_TASKS);
            cout << gConfig.getModuleNetatmoMain()->id().toStdString() << " Get initial indoor measures from " << gConfig.getModuleNetatmoMain()->startDate() << endl ;
            m_pNetatmoDeviceWS = QSharedPointer<NetatmoGetDeviceMeasuresWS>( new NetatmoGetDeviceMeasuresWS(m_token,
                                                                                                            gConfig.getModuleNetatmoMain()->startDate(),
                                                                                                            gConfig.getModuleNetatmoMain()->id(),
                                                                                                            this) );

            m_pNetatmoDeviceWS->start(&m_db);

        } else {
            m_currentBotLogs.stop_timestamp = QDateTime::currentDateTime().toTime_t();
            persistCurrentLogs();

            emit finished();

        }
    } else {

        if ( (true == m_bCanContinueWithRain) ||
             (true == m_bCanContinueWithInDoor) ||
             (true == m_bCanContinueWithOutDoor) ||
             (true == m_bCanContinueWithWind) ||
             (true == m_bCanContinueWithAddInDoor) ) {

            m_limitCount++;
            initCounters();

            cout << endl << endl << "Pass " << (m_limitCount+1) << " sleep " << SLEEP_TIME_TWO_TASKS << " seconds" << endl ;
            QThread::sleep(SLEEP_TIME_TWO_TASKS);
            cout << gConfig.getModuleNetatmoMain()->id().toStdString() << " Get initial indoor measures from " << gConfig.getModuleNetatmoMain()->startDate() << endl ;
            m_pNetatmoDeviceWS = QSharedPointer<NetatmoGetDeviceMeasuresWS>( new NetatmoGetDeviceMeasuresWS(m_token,
                                                                                                            gConfig.getModuleNetatmoMain()->startDate(),
                                                                                                            gConfig.getModuleNetatmoMain()->id(),
                                                                                                            this) );

            m_pNetatmoDeviceWS->start(&m_db);

        } else {
            cout << endl << endl << "Pass " << (m_limitCount+1) << " End Of Download" << endl ;
            m_currentBotLogs.stop_timestamp = QDateTime::currentDateTime().toTime_t();
            persistCurrentLogs();

            emit finished();

        }
    }
}

