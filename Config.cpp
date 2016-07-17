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

#include "Config.h"

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

Config::Config() {

}

Config::~Config() {

}

void
Config::loadConfig(QString configFilename) {
    m_configFilename = configFilename ;

    QFile file(m_configFilename);
    if ( ! file.open(QIODevice::ReadOnly) ) {
        qDebug() << "can not open " << m_configFilename ;
    } else {
        qDebug() << m_configFilename<< "is Open" ;
    }
    QString s;

    QTextStream stream(&file);
    s.append(stream.readAll());

    QByteArray content(s.toUtf8(), s.length());

    QJsonDocument d = QJsonDocument::fromJson(content);
    QJsonObject map = d.object();

    QJsonObject dictAccount = map["account"].toObject();
    m_login = dictAccount["login"].toString();
    m_password = dictAccount["password"].toString();
    m_clientId = dictAccount["clientId"].toString();
    m_clientSecret = dictAccount["clientSecret"].toString();
    m_dbName =  dictAccount["dbName"].toString();
    m_mainDeviceId = dictAccount["mainDevice"].toString();

    QJsonObject dictDownloader = map["downloader"].toObject();
    m_limit = dictDownloader["limit"].toInt() ;
    m_netatmoPerUserLimitEvery10Seconds = dictDownloader["netatmoPerUserLimitEvery10Seconds"].toInt() ;
    m_netatmoPerUserLimitEveryHour = dictDownloader["netatmoPerUserLimitEveryHour"].toInt() ;


    QJsonArray arrayDevices = map["devices"].toArray();
    int index=0;
    for(auto item:arrayDevices) {
        QJsonObject dictOneDevice = item.toObject();
        QString moduleSqlPrefix = dictOneDevice["sql_prefix"].toString();
        QString moduleType = dictOneDevice["type"].toString();

        QSharedPointer<ConfigModule> module(new ConfigModule("-1","moduleName",moduleSqlPrefix));

        if ( moduleType == "rain" ) {
            m_indexRainModule = index;
        } else if ( moduleType == "main" ) {
            m_indexMainDevice = index;
            module->setIsMainDevice(true);
        } else if ( moduleType == "outdoor" ) {
            m_indexOutdoorModule = index;
        } else if ( moduleType == "wind") {
            m_indexWindModule = index;
        } else if ( moduleType == "indoor" ) {
            m_indexIndoorModule = index;
        }
        addConfigModule(module);
        index++;
    }

}


void
Config::addConfigModule(QSharedPointer<ConfigModule> module) {
    m_vectConfigModules << module ;
}

ConfigModule::ConfigModule(QString id,QString name, QString sqlPrefix)
    :m_id(id),m_name(name),m_sqlPrefix(sqlPrefix),m_isMainDevice(false)
{
}

ConfigModule::~ConfigModule() {

}

