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
//  NetatmoGetWindDelegate
void
TaskBotGetMeasures::onNetatmoWindSucceeded(int httpCode,QByteArray& contentResult ) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    m_deviceWindRequestsCount++;

    QVector<QSharedPointer<WindStrengthMeasure>>        arrayWindStrength;
    QVector<QSharedPointer<WindAngleMeasure>>           arrayWindAngle;
    QVector<QSharedPointer<GustStrengthMeasure>>        arrayGustStrength;
    QVector<QSharedPointer<GustAngleMeasure>>           arrayGustAngle;

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

        double valueWindStrength = arrayOfValues[0].toDouble();
        arrayWindStrength << QSharedPointer<WindStrengthMeasure>(new WindStrengthMeasure(timestamp,valueWindStrength)) ;

        double valueWindAngle = arrayOfValues[1].toDouble();
        arrayWindAngle << QSharedPointer<WindAngleMeasure>(new WindAngleMeasure(timestamp,valueWindAngle)) ;

        double valueGustStrength = arrayOfValues[2].toDouble();
        arrayGustStrength << QSharedPointer<GustStrengthMeasure>(new GustStrengthMeasure(timestamp,valueGustStrength)) ;

        double valueGustAngle = arrayOfValues[3].toDouble();
        arrayGustAngle << QSharedPointer<GustAngleMeasure>(new GustAngleMeasure(timestamp,valueGustAngle)) ;
    }
    if ( lastTimestamp == 0L ) {
        gConfig.getModuleNetatmoWind()->setStartDate(gConfig.getModuleNetatmoWind()->startDate() + WIND_MEASURE_INTERVAL);
        lastTimestamp = gConfig.getModuleNetatmoWind()->startDate();

    } else {
        gConfig.getModuleNetatmoWind()->setStartDate(lastTimestamp) ;

        m_db.transaction();
        qSort(arrayWindStrength.begin(),arrayWindStrength.end(),windStrengthMeasureLessThan);
        {
            QVector<QSharedPointer<WindStrengthMeasure>>::iterator itValues = arrayWindStrength.begin();
            while ( itValues != arrayWindStrength.constEnd() ) {
                auto measure = *itValues++;
                QString tableName = gConfig.getModuleNetatmoWind()->sqlPrefix() + "wind_strength";
                insertInto(tableName,measure->getTimestamp(),measure->getValue());
            }
        }
        m_db.commit();

        m_db.transaction();
        qSort(arrayWindAngle.begin(),arrayWindAngle.end(),windAngleMeasureLessThan);
        {
            QVector<QSharedPointer<WindAngleMeasure>>::iterator itValues = arrayWindAngle.begin();
            while ( itValues != arrayWindAngle.constEnd() ) {
                auto measure = *itValues++;
                QString tableName = gConfig.getModuleNetatmoWind()->sqlPrefix() + "wind_angle";
                insertInto(tableName,measure->getTimestamp(),measure->getValue());
            }
        }
        m_db.commit();

        m_db.transaction();
        qSort(arrayGustStrength.begin(),arrayGustStrength.end(),gustStrengthMeasureLessThan);
        {
            QVector<QSharedPointer<GustStrengthMeasure>>::iterator itValues = arrayGustStrength.begin();
            while ( itValues != arrayGustStrength.constEnd() ) {
                auto measure = *itValues++;
                QString tableName = gConfig.getModuleNetatmoWind()->sqlPrefix() + "gust_strength";
                insertInto(tableName,measure->getTimestamp(),measure->getValue());
            }
        }
        m_db.commit();

        m_db.transaction();
        qSort(arrayGustAngle.begin(),arrayGustAngle.end(),gustAngleMeasureLessThan);
        {
            QVector<QSharedPointer<GustAngleMeasure>>::iterator itValues = arrayGustAngle.begin();
            while ( itValues != arrayGustAngle.constEnd() ) {
                auto measure = *itValues++;
                QString tableName = gConfig.getModuleNetatmoWind()->sqlPrefix() + "gust_angle";
                insertInto(tableName,measure->getTimestamp(),measure->getValue());
            }
        }
        m_db.commit();
    }

    QSharedPointer<ModuleLogs> moduleLogs = m_currentModuleLogs[gConfig.getModuleNetatmoWind()->id()];
    if ( moduleLogs.isNull() ) {
        ModuleLogs* pModuleLogs = new ModuleLogs();
        pModuleLogs->idt = gConfig.getModuleNetatmoWind()->id();
        pModuleLogs->begin_timestamp = firstTimestamp;
        pModuleLogs->end_timestamp = lastTimestamp;
        pModuleLogs->nb_measures_inserted = realCount ;

        m_currentModuleLogs[gConfig.getModuleNetatmoWind()->id()] = QSharedPointer<ModuleLogs>(pModuleLogs);
        m_currentBotLogs.wind_module_count = pModuleLogs->nb_measures_inserted;

    } else {
        moduleLogs->end_timestamp = lastTimestamp;
        moduleLogs->nb_measures_inserted += realCount ;
        m_currentBotLogs.wind_module_count = moduleLogs->nb_measures_inserted;
    }

    QDateTime dFirst = QDateTime::fromTime_t(firstTimestamp);
    QDateTime dLast  = QDateTime::fromTime_t(lastTimestamp);

    qDebug() << "Count add(" << arrayWindStrength.size() << ") real(" << realCount << ") " ;
    qDebug() << "First : " << QString::number(firstTimestamp) << "(" << dFirst.toString("yyyy-MM-dd hh:mm:ss") << ")"  ;
    qDebug() << "Last  : " << QString::number(lastTimestamp) << "(" << dLast.toString("yyyy-MM-dd hh:mm:ss") << ")" << endl;

    currentDateTime = QDateTime::currentDateTime();
    uint t = currentDateTime.toTime_t();

    if ( lastTimestamp <= t - 3600 * 12 ) {
        m_bCanContinueWithWind = true;
    } else {
        m_bCanContinueWithWind = false;
    }

    if ( (lastTimestamp <= t - 3600 * 12) && (m_deviceWindRequestsCount < m_maxDeviceWindRequests) ) {
        cout << "    Get next wind measures from " << gConfig.getModuleNetatmoWind()->startDate() << endl ;
        m_pNetatmoWindWS = QSharedPointer<NetatmoGetWindWS>( new NetatmoGetWindWS(m_token,
                                                                                    gConfig.getModuleNetatmoWind()->startDate(),
                                                                                    gConfig.getModuleNetatmoMain()->id(),
                                                                                    gConfig.getModuleNetatmoWind()->id(),
                                                                                    this) );
        m_pNetatmoWindWS->start();

    } else {
        if ( gConfig.getModuleNetatmoIndoorSize() > 0 ) {
            cout << "IndoorAdditionnal module is not present" << endl ;
        } else {
            cout << "IndoorAdditionnal module is present" << endl ;
        }

        if ( gConfig.getModuleNetatmoIndoorSize() > 0 ) {
            QSharedPointer<ModuleLogs> moduleLogs = m_previousModuleLogs[gConfig.getModuleNetatmoIndoor(0)->id()] ;
            if ( ! moduleLogs.isNull() ) {
                gConfig.getModuleNetatmoIndoor(0)->setStartDate(moduleLogs->end_timestamp);
            }

            cout << "Get initial module addtionnel (0) measures from " << gConfig.getModuleNetatmoIndoor(0)->startDate() << endl ;

            m_pNetatmoModuleAdditionnelWS = QSharedPointer<NetatmoGetModuleAdditionnelWS>( new NetatmoGetModuleAdditionnelWS(m_token,
                                                                                                                              gConfig.getModuleNetatmoIndoor(0)->startDate(),
                                                                                                                              gConfig.getModuleNetatmoMain()->id(),
                                                                                                                              gConfig.getModuleNetatmoIndoor(0)->id(),
                                                                                                                              0,
                                                                                                                              this) );
            m_pNetatmoModuleAdditionnelWS->start();

        } else {
            endTaskOrContinue();

        }
    }

}

void
TaskBotGetMeasures::onNetatmoWindFailed(int httpCode,QByteArray& contentResult) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    QString content = contentResult ;

    cout << "TaskBotGetMeasures::onNetatmoWindFailed " << httpCode << " " << content.toStdString() << endl ;

    m_currentBotLogs.stop_timestamp = QDateTime::currentDateTime().toTime_t();
    persistCurrentLogs();

    emit finished();
}
