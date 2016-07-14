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
//  NetatmoGetModuleAdditionnelDelegate
void
TaskBotGetMeasures::onNetatmoModuleAdditionnelSucceeded(int httpCode,QByteArray& contentResult,int currentModuleIndex) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    m_deviceAddInDoorRequestsCount++;

    QVector<QSharedPointer<TemperatureMeasure>>     arrayTemperatureMeasures;
    QVector<QSharedPointer<CO2Measure>>             arrayCO2Measures;
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

        double valueCO2 = arrayOfValues[1].toDouble();
        arrayCO2Measures << QSharedPointer<CO2Measure>(new CO2Measure(timestamp,valueCO2)) ;

        double valueHumidity = arrayOfValues[2].toDouble();
        arrayHumidityMeasures << QSharedPointer<HumidityMeasure>(new HumidityMeasure(timestamp,valueHumidity)) ;
    }

    if ( lastTimestamp == 0L ) {
        gConfig.getModuleNetatmoIndoor(currentModuleIndex)->setStartDate(gConfig.getModuleNetatmoIndoor(currentModuleIndex)->startDate() + INDOOR_MEASURE_INTERVAL);
        lastTimestamp = gConfig.getModuleNetatmoIndoor(currentModuleIndex)->startDate();
    } else {
        gConfig.getModuleNetatmoIndoor(currentModuleIndex)->setStartDate( lastTimestamp ) ;

        m_db.transaction();
        qSort(arrayTemperatureMeasures.begin(),arrayTemperatureMeasures.end(),temperatureMeasureLessThan);
        {
            QVector<QSharedPointer<TemperatureMeasure>>::iterator itValues = arrayTemperatureMeasures.begin();
            while ( itValues != arrayTemperatureMeasures.constEnd() ) {
                auto measure = *itValues++;
                QString tableName = gConfig.getModuleNetatmoIndoor(currentModuleIndex)->sqlPrefix() + "temp";
                insertInto(tableName,measure->getTimestamp(),measure->getValue());
            }
        }
        m_db.commit();

        m_db.transaction();
        qSort(arrayCO2Measures.begin(),arrayCO2Measures.end(),co2MeasureLessThan);
        {
            QVector<QSharedPointer<CO2Measure>>::iterator itValues = arrayCO2Measures.begin();
            while ( itValues != arrayCO2Measures.constEnd() ) {
                auto measure = *itValues++;
                QString tableName = gConfig.getModuleNetatmoIndoor(currentModuleIndex)->sqlPrefix() + "co2";
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
                QString tableName = gConfig.getModuleNetatmoIndoor(currentModuleIndex)->sqlPrefix() + "humidity";
                insertInto(tableName,measure->getTimestamp(),measure->getValue());
            }
        }
        m_db.commit();

    }

    QSharedPointer<ModuleLogs> moduleLogs = m_currentModuleLogs[gConfig.getModuleNetatmoIndoor(currentModuleIndex)->id()];
    if ( moduleLogs.isNull() ) {
        ModuleLogs* pModuleLogs = new ModuleLogs();
        pModuleLogs->idt = gConfig.getModuleNetatmoIndoor(currentModuleIndex)->id();
        pModuleLogs->begin_timestamp = firstTimestamp;
        pModuleLogs->end_timestamp = lastTimestamp;
        pModuleLogs->nb_measures_inserted = realCount ;

        m_currentModuleLogs[gConfig.getModuleNetatmoIndoor(currentModuleIndex)->id()] = QSharedPointer<ModuleLogs>(pModuleLogs);
        m_currentBotLogs.opt_module_count = pModuleLogs->nb_measures_inserted;

    } else {
        moduleLogs->end_timestamp = lastTimestamp;
        moduleLogs->nb_measures_inserted += realCount ;
        m_currentBotLogs.opt_module_count = moduleLogs->nb_measures_inserted;

    }

    QDateTime dFirst = QDateTime::fromTime_t(firstTimestamp);
    QDateTime dLast  = QDateTime::fromTime_t(lastTimestamp);

    qDebug() << "Count add(" << realCount << ")" ;
    qDebug() << "First : " << QString::number(firstTimestamp) << "(" << dFirst.toString("yyyy-MM-dd hh:mm:ss") << ")" ;
    qDebug() << "Last  : " << QString::number(lastTimestamp)  << "(" << dLast.toString("yyyy-MM-dd hh:mm:ss") << ")" << endl;

    currentDateTime = QDateTime::currentDateTime();
    uint t = currentDateTime.toTime_t();

    if ( lastTimestamp <= t - 3600 * 12 ) {
        m_bCanContinueWithAddInDoor = true;
    } else {
        m_bCanContinueWithAddInDoor = false;
    }

    if ( (lastTimestamp <= t - 3600 * 12) && (m_deviceAddInDoorRequestsCount < m_maxDeviceAddInDoorRequests) ) {
        cout << "currentIndex(" << currentModuleIndex << "/" << gConfig.getModuleNetatmoIndoorSize() << ")" << endl ;
        cout << gConfig.getModuleNetatmoIndoor(currentModuleIndex)->id().toStdString() << "    Get next indoor measures from " << gConfig.getModuleNetatmoIndoor(currentModuleIndex)->startDate() << endl ;
        m_pNetatmoModuleAdditionnelWS = QSharedPointer<NetatmoGetModuleAdditionnelWS>( new NetatmoGetModuleAdditionnelWS(m_token,
                                                                                                                          gConfig.getModuleNetatmoIndoor(currentModuleIndex)->startDate(),
                                                                                                                          gConfig.getModuleNetatmoMain()->id(),
                                                                                                                          gConfig.getModuleNetatmoIndoor(currentModuleIndex)->id(),
                                                                                                                          currentModuleIndex,
                                                                                                                          this) );
        m_pNetatmoModuleAdditionnelWS->start();

    } else {
        int nextModuleIndex = currentModuleIndex + 1 ;
        cout << "currentIndex(" << currentModuleIndex << "/" << gConfig.getModuleNetatmoIndoorSize() << ")" << endl ;
        if ( nextModuleIndex < gConfig.getModuleNetatmoIndoorSize() ) {
            QSharedPointer<ModuleLogs> moduleLogs = m_previousModuleLogs[gConfig.getModuleNetatmoIndoor(nextModuleIndex)->id()] ;
            if ( ! moduleLogs.isNull() ) {
                gConfig.getModuleNetatmoIndoor(nextModuleIndex)->setStartDate(moduleLogs->end_timestamp);
            }

            m_deviceAddInDoorRequestsCount = 0;
            cout << gConfig.getModuleNetatmoIndoor(nextModuleIndex)->id().toStdString() << " Get initial outdoor measures from " << gConfig.getModuleNetatmoIndoor(nextModuleIndex)->startDate() << endl ;
            m_pNetatmoModuleAdditionnelWS = QSharedPointer<NetatmoGetModuleAdditionnelWS>( new NetatmoGetModuleAdditionnelWS(m_token,
                                                                                                                              gConfig.getModuleNetatmoIndoor(nextModuleIndex)->startDate(),
                                                                                                                              gConfig.getModuleNetatmoMain()->id(),
                                                                                                                              gConfig.getModuleNetatmoIndoor(nextModuleIndex)->id(),
                                                                                                                              nextModuleIndex,
                                                                                                                              this) );
            m_pNetatmoModuleAdditionnelWS->start();

        } else {
            endTaskOrContinue();

        }

    }

}

void
TaskBotGetMeasures::onNetatmoModuleAdditionnelFailed(int httpCode,QByteArray& contentResult,int currentModuleIndex) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)
    Q_UNUSED(currentModuleIndex)

    QString content = contentResult ;

    cout << "TaskBotGetMeasures::onNetatmoModuleAdditionnelFailed " << httpCode << " " << content.toStdString() << endl ;

    m_currentBotLogs.stop_timestamp = QDateTime::currentDateTime().toTime_t();
    persistCurrentLogs();

    emit finished();

}
