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

#ifndef WSLIMITMANAGER_H
#define WSLIMITMANAGER_H

#include "AbstractWS.h"
#include <QLinkedList>
#include <QSharedPointer>
#include <QSqlDatabase>

class WSEntry;

class WSLimitManager {
private:
    int                                     m_every10SecondsLimit;
    int                                     m_everyHourLimit;
    QLinkedList<QSharedPointer<WSEntry>>    m_webServices;

public:
    WSLimitManager(int every10SecondsLimit, int everyHourLimit);
    ~WSLimitManager();

    int         addWebService(QSqlDatabase* db, long timestamp, HttpMethod method, QString url, QString content) ;

    long        waitTime10SecondsLimit() const ;
    long        waitTimeHourLimit() const ;
};

// create table webservices_logs(idt integer primary key autoincrement,
//      ws_timestamp int8,
//      method char(6),
//      url text,
//      content text,
//      status int,
//      response text
//  )
class WSEntry {
private:
    long            m_timestamp;
    HttpMethod      m_method;
    QString         m_url;
    QString         m_content;

    friend int WSLimitManager::addWebService(QSqlDatabase*,long,HttpMethod,QString,QString);
    WSEntry(long timestamp, HttpMethod method, QString url, QString content);

public:
    ~WSEntry();

    long            getTimestamp() const    { return m_timestamp; }
    HttpMethod      getMethod() const       { return m_method; }
    QString         getURL() const          { return m_url; }
    QString         getContent() const      { return m_content; }
};

extern QSharedPointer<WSLimitManager>   pgLimitManager;

#endif // WSLIMITMANAGER_H
