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

#include "BotLogs.h"

const QString     BotLogs::botlogsColumnNamesWithIdt    = "idt,start_timestamp,start_date,stop_timestamp,stop_date,main_device_count,outdoor_device_count,rain_module_count,wind_module_count,opt_module_count";
const QString     BotLogs::botlogsColumnNamesWithoutIdt = "start_timestamp,start_date,stop_timestamp,stop_date,main_device_count,outdoor_device_count,rain_module_count,wind_module_count,opt_module_count";
const QString     BotLogs::botlogsColumnBindsWithoutIdt = ":start_timestamp,:start_date,:stop_timestamp,:stop_date,:main_device_count,:outdoor_device_count,:rain_module_count,:wind_module_count,:opt_module_count";

const QString     ModuleLogs::moduleColumnNamesWithIdt      = "idt,begin_timestamp,begin_date,end_timestamp,end_date,nb_measures_inserted";
const QString     ModuleLogs::moduleColumnNamesWithoutIdt   = "begin_timestamp,begin_date,end_timestamp,end_date,nb_measures_inserted";
const QString     ModuleLogs::moduleColumnBindsWithIdt      = ":idt,:begin_timestamp,:begin_date,:end_timestamp,:end_date,:nb_measures_inserted";

BotLogs::BotLogs()
    :idt(-1),start_timestamp(-1),stop_timestamp(-1),main_device_count(-1),outdoor_device_count(-1),rain_module_count(-1),wind_module_count(-1),opt_module_count(-1)
{

}

ModuleLogs::ModuleLogs()
    :idt(""),begin_timestamp(-1),end_timestamp(-1),nb_measures_inserted(-1)
{

}
