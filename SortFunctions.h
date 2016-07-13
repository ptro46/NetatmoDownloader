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

#ifndef SORTFUNCTIONS_H
#define SORTFUNCTIONS_H

#include <QSharedPointer>

class TemperatureMeasure;
class CO2Measure;
class HumidityMeasure;
class NoiseMeasure;
class PluviometrieMeasure;
class PressureMeasure;
class WindStrengthMeasure;
class WindAngleMeasure;
class GustStrengthMeasure;
class GustAngleMeasure;

bool temperatureMeasureLessThan(QSharedPointer<TemperatureMeasure> p1, QSharedPointer<TemperatureMeasure> p2) ;

bool co2MeasureLessThan(QSharedPointer<CO2Measure> p1, QSharedPointer<CO2Measure> p2) ;

bool humidityMeasureLessThan(QSharedPointer<HumidityMeasure> p1, QSharedPointer<HumidityMeasure> p2) ;

bool noiseMeasureLessThan(QSharedPointer<NoiseMeasure> p1, QSharedPointer<NoiseMeasure> p2) ;

bool pluvioMeasureLessThan(QSharedPointer<PluviometrieMeasure> p1, QSharedPointer<PluviometrieMeasure> p2) ;

bool pressureMeasureLessThan(QSharedPointer<PressureMeasure> p1, QSharedPointer<PressureMeasure> p2) ;


bool windStrengthMeasureLessThan(QSharedPointer<WindStrengthMeasure> p1, QSharedPointer<WindStrengthMeasure> p2) ;
bool windAngleMeasureLessThan(QSharedPointer<WindAngleMeasure> p1, QSharedPointer<WindAngleMeasure> p2) ;
bool gustStrengthMeasureLessThan(QSharedPointer<GustStrengthMeasure> p1, QSharedPointer<GustStrengthMeasure> p2) ;
bool gustAngleMeasureLessThan(QSharedPointer<GustAngleMeasure> p1, QSharedPointer<GustAngleMeasure> p2) ;

#endif // SORTFUNCTIONS_H
