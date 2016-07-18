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
//  NetatmoGetPluviometrieDelegate
void
TaskBotGetMeasures::onNetatmoPluviometrieSucceeded(int httpCode,QByteArray& contentResult )  {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    m_pluviometrieRequestsCount++;

    QVector<QSharedPointer<PluviometrieMeasure>>        arrayPluvioMeasures;

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
        QJsonArray arrayOfValues = bodyDict[key].toArray();
        double value = arrayOfValues[0].toDouble();
        long timestamp = key.toLong();

        if ( timestamp > lastTimestamp ) {
            lastTimestamp = timestamp ;
        }

        if ( timestamp < firstTimestamp ) {
            firstTimestamp = timestamp ;
        }

        if ( value != 0 ) {
            arrayPluvioMeasures << QSharedPointer<PluviometrieMeasure>(new PluviometrieMeasure(timestamp,value)) ;
        }
    }
    if ( lastTimestamp == 0L ) {
        gConfig.getModuleNetatmoRain()->setStartDate(gConfig.getModuleNetatmoRain()->startDate() + PLUVIOMETRIE_MEASURE_INTERVAL);
        lastTimestamp = gConfig.getModuleNetatmoRain()->startDate();

    } else {
        gConfig.getModuleNetatmoRain()->setStartDate(lastTimestamp) ;

        m_db.transaction();
        qSort(arrayPluvioMeasures.begin(),arrayPluvioMeasures.end(),pluvioMeasureLessThan);
        QVector<QSharedPointer<PluviometrieMeasure>>::iterator itValues = arrayPluvioMeasures.begin();
        while ( itValues != arrayPluvioMeasures.constEnd() ) {
            auto measure = *itValues++;
            QString tableName = gConfig.getModuleNetatmoRain()->sqlPrefix() + "rain";
            insertInto(tableName,measure->getTimestamp(),measure->getValue());
        }
        m_db.commit();
    }

    QSharedPointer<ModuleLogs> moduleLogs = m_currentModuleLogs[gConfig.getModuleNetatmoRain()->id()];
    if ( moduleLogs.isNull() ) {
        ModuleLogs* pModuleLogs = new ModuleLogs();
        pModuleLogs->idt = gConfig.getModuleNetatmoRain()->id();
        pModuleLogs->begin_timestamp = firstTimestamp;
        pModuleLogs->end_timestamp = lastTimestamp;
        pModuleLogs->nb_measures_inserted = realCount ;

        m_currentModuleLogs[gConfig.getModuleNetatmoRain()->id()] = QSharedPointer<ModuleLogs>(pModuleLogs);
        m_currentBotLogs.rain_module_count = pModuleLogs->nb_measures_inserted;

    } else {
        moduleLogs->end_timestamp = lastTimestamp;
        moduleLogs->nb_measures_inserted += realCount ;
        m_currentBotLogs.rain_module_count = moduleLogs->nb_measures_inserted;
    }

    QDateTime dFirst = QDateTime::fromTime_t(firstTimestamp);
    QDateTime dLast  = QDateTime::fromTime_t(lastTimestamp);

    qDebug() << "Count add(" << arrayPluvioMeasures.size() << ") real(" << realCount << ") " ;
    qDebug() << "First : " << QString::number(firstTimestamp) << "(" << dFirst.toString("yyyy-MM-dd hh:mm:ss") << ")"  ;
    qDebug() << "Last  : " << QString::number(lastTimestamp) << "(" << dLast.toString("yyyy-MM-dd hh:mm:ss") << ")" << endl;

    currentDateTime = QDateTime::currentDateTime();
    uint t = currentDateTime.toTime_t();

    if ( lastTimestamp <= t - 3600 * 12 ) {
        m_bCanContinueWithRain = true;
    } else {
        m_bCanContinueWithRain = false;
    }

    if ( (lastTimestamp <= t - 3600 * 12) && (m_pluviometrieRequestsCount < m_maxPluviometrieRequests) ) {
        cout << gConfig.getModuleNetatmoRain()->id().toStdString() << "    Get next rain measures from " << gConfig.getModuleNetatmoRain()->startDate() << endl ;
        m_pNetatmoPluviometrieWS = QSharedPointer<NetatmoGetPluviometrieWS>( new NetatmoGetPluviometrieWS(m_token,
                                                                                                            gConfig.getModuleNetatmoRain()->startDate(),
                                                                                                            gConfig.getModuleNetatmoMain()->id(),
                                                                                                            gConfig.getModuleNetatmoRain()->id(),
                                                                                                            this) );
        m_pNetatmoPluviometrieWS->start(&m_db);

    } else {
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

        if ( ! gConfig.getModuleNetatmoWind().isNull() ) {
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
TaskBotGetMeasures::onNetatmoPluviometrieFailed(int httpCode,QByteArray& contentResult) {
    Q_UNUSED(httpCode)
    Q_UNUSED(contentResult)

    QString content = contentResult ;

    cout << "TaskBotGetMeasures::onNetatmoPluviometrieFailed " << httpCode << " " << content.toStdString() << endl ;
    m_currentBotLogs.stop_timestamp = QDateTime::currentDateTime().toTime_t();
    persistCurrentLogs();

    emit finished();
}
