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

#ifndef BOTLOGS_H
#define BOTLOGS_H

#include <QString>
#include <QVariant>

#include <iostream>
using namespace std;

/*
 *
 * create table botlogs(
 *      idt                     integer primary key autoincrement,
 *      start_timestamp         int8,
 *      start_date              text,
 *      stop_timestamp          int8,
 *      stop_date               text,
 *      main_device_count       int,
 *      outdoor_device_count    int,
 *      rain_module_count       int,
 *      wind_module_count       int,
 *      opt_module_count        int
 * );
 *
*/

class BotLogs
{
public:
    int         idt;

    long        start_timestamp;
    QString     start_date;
    long        stop_timestamp;
    QString     stop_date;

    long        main_device_count;
    long        outdoor_device_count;
    long        rain_module_count;
    long        wind_module_count;
    long        opt_module_count;

    QVariant    v_start_timestamp() const {
        return QVariant((unsigned long long)start_timestamp);
    }
    QVariant    v_stop_timestamp() const {
        return QVariant((unsigned long long)stop_timestamp);
    }

    QVariant    v_main_device_count() const {
        return QVariant((unsigned long long)main_device_count);
    }
    QVariant    v_outdoor_device_count() const {
        return QVariant((unsigned long long)outdoor_device_count);
    }
    QVariant    v_rain_module_count() const {
        return QVariant((unsigned long long)rain_module_count);
    }
    QVariant    v_wind_module_count() const {
        return QVariant((unsigned long long)wind_module_count);
    }
    QVariant    v_opt_module_count() const {
        return QVariant((unsigned long long)opt_module_count);
    }

    static const QString     botlogsColumnNamesWithIdt;
    static const QString     botlogsColumnNamesWithoutIdt;
    static const QString     botlogsColumnBindsWithoutIdt;

public:
    BotLogs();

    friend
    ostream& operator<<(ostream& ro, const BotLogs& botLogs) ;
};

/*
 * create table module_logs(
 *      idt                     text primary key,
 *      begin_timestamp         int8,
 *      begin_date              text,
 *      end_timestamp           int8,
 *      end_date                text,
 *      nb_measures_inserted    int
 * ) ;
*/
class ModuleLogs {
public:
    QString             idt;

    long                begin_timestamp;
    QString             begin_date;

    long                end_timestamp;
    QString             end_date;

    long                nb_measures_inserted;

    QVariant    v_begin_timestamp() const {
        return QVariant((unsigned long long)begin_timestamp);
    }

    QVariant    v_end_timestamp() const {
        return QVariant((unsigned long long)end_timestamp);
    }

    QVariant    v_nb_measures_inserted() const {
        return QVariant((unsigned long long)nb_measures_inserted);
    }

    static const QString     moduleColumnNamesWithIdt;
    static const QString     moduleColumnNamesWithoutIdt;
    static const QString     moduleColumnBindsWithIdt;

public:
    ModuleLogs();

    friend
    ostream& operator<<(ostream& ro, const ModuleLogs& botLogs) ;

};

inline
ostream& operator<<(ostream& ro, const BotLogs& botLogs) {
    Q_UNUSED(ro)
    Q_UNUSED(botLogs)

    return ro << "idt(" << botLogs.idt
              << ") start_date(" << botLogs.start_timestamp
              << ")::(" << botLogs.start_date.toStdString()
              << ") stop_date(" << botLogs.stop_timestamp
              << ")::(" << botLogs.stop_date.toStdString()
              << ") main_device_count(" << botLogs.main_device_count
              << ") outdoor_device_count(" << botLogs.outdoor_device_count
              << ") rain_module_count(" << botLogs.rain_module_count
              << ") wind_module_count(" << botLogs.wind_module_count
              << ") opt_module_count(" << botLogs.opt_module_count
              << ")" ;
}


inline
ostream& operator<<(ostream& ro, const ModuleLogs& moduleLogs) {
    Q_UNUSED(ro)
    Q_UNUSED(moduleLogs)

    return ro << "idt(" << moduleLogs.idt.toStdString()
              << ") begin_timestamp(" << moduleLogs.begin_timestamp
              << ")::(" << moduleLogs.begin_date.toStdString()
              << ") end_timestamp(" << moduleLogs.end_timestamp
              << ")::(" << moduleLogs.end_date.toStdString()
              << ") nb_measures_inserted(" << moduleLogs.nb_measures_inserted
              << ")" ;
}

#endif // BOTLOGS_H
