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

#include "WSLimitManager.h"

QSharedPointer<WSLimitManager>   pgLimitManager;

WSEntry::WSEntry(long timestamp, HttpMethod method, QString url, QString content)
    :m_timestamp(timestamp), m_method(method), m_url(url), m_content(content) {
}

WSEntry::~WSEntry() {

}

WSLimitManager::WSLimitManager(int every10SecondsLimit, int everyHourLimit)
    :m_every10SecondsLimit(every10SecondsLimit), m_everyHourLimit(everyHourLimit) {

}

WSLimitManager::~WSLimitManager() {

}

void
WSLimitManager::addWebService(long timestamp, HttpMethod method, QString url, QString content) {
    m_webServices << QSharedPointer<WSEntry>(new WSEntry(timestamp,method,url,content)) ;
}

long
WSLimitManager::waitTime10SecondsLimit() const {
    long waitDuration = 0L ;
    int  count=0;
    QSharedPointer<WSEntry> lastWebService = m_webServices.last();

    auto it = m_webServices.rbegin();
    while ( (waitDuration == 0) && (it != m_webServices.rend()) ) {
        auto wsEntry = *it++;
        count++;
        long timeElapsed = lastWebService->getTimestamp() - wsEntry->getTimestamp() ;
        if ( timeElapsed > 10L && count > m_every10SecondsLimit ) {
            waitDuration = 10L ;
        }
    }
    return waitDuration;
}

long
WSLimitManager::waitTimeHourLimit() const {
    long waitDuration = 0L ;
    int  count=0;
    QSharedPointer<WSEntry> lastWebService = m_webServices.last();

    auto it = m_webServices.rbegin();
    while ( (waitDuration == 0) && (it != m_webServices.rend()) ) {
        auto wsEntry = *it++;
        count++;
        long timeElapsed = lastWebService->getTimestamp() - wsEntry->getTimestamp() ;
        if ( timeElapsed > 3600L && count > m_everyHourLimit ) {
            waitDuration = 3600L / 2 ;
        }
    }
    return waitDuration;
}
