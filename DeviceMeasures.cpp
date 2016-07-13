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

#include "DeviceMeasures.h"

#include <iostream>
using namespace std ;

TemperatureMeasure::TemperatureMeasure(long timestamp, double value)
    :m_timestamp(timestamp), m_value(value)
{

}

TemperatureMeasure::~TemperatureMeasure() {
//    cout << "TemperatureMeasure::~TemperatureMeasure()" << endl ;
}

CO2Measure::CO2Measure(long timestamp, double value)
    :m_timestamp(timestamp), m_value(value)
{

}

CO2Measure::~CO2Measure() {
//    cout << "CO2Measure::~CO2Measure()" << endl ;
}


HumidityMeasure::HumidityMeasure(long timestamp, double value)
    :m_timestamp(timestamp), m_value(value)
{

}

HumidityMeasure::~HumidityMeasure() {
//    cout << "HumidityMeasure::~HumidityMeasure()" << endl ;
}

NoiseMeasure::NoiseMeasure(long timestamp, double value)
    :m_timestamp(timestamp), m_value(value)
{

}

NoiseMeasure::~NoiseMeasure() {
//    cout << "NoiseMeasure::~NoiseMeasure()" << endl ;
}

PressureMeasure::PressureMeasure(long timestamp, double value)
    :m_timestamp(timestamp), m_value(value)
{

}

PressureMeasure::~PressureMeasure() {
//    cout << "PressureMeasure::~PressureMeasure()" << endl ;
}


//---
//  Rain
PluviometrieMeasure::PluviometrieMeasure(long timestamp, double value)
    :m_timestamp(timestamp), m_value(value)
{

}

PluviometrieMeasure::~PluviometrieMeasure() {
//    cout << "PluviometrieMeasure::~PluviometrieMeasure()" << endl ;
}


//---
//  Wind
WindStrengthMeasure::WindStrengthMeasure(long timestamp, double value)
    :m_timestamp(timestamp), m_value(value)
{

}

WindStrengthMeasure::~WindStrengthMeasure() {
//    cout << "WindStrengthMeasure::~WindStrengthMeasure()" << endl ;
}



WindAngleMeasure::WindAngleMeasure(long timestamp, double value)
    :m_timestamp(timestamp), m_value(value)
{

}

WindAngleMeasure::~WindAngleMeasure() {
//    cout << "WindAngleMeasure::~WindAngleMeasure()" << endl ;
}



GustStrengthMeasure::GustStrengthMeasure(long timestamp, double value)
    :m_timestamp(timestamp), m_value(value)
{

}

GustStrengthMeasure::~GustStrengthMeasure() {
//    cout << "GustStrengthMeasure::~GustStrengthMeasure()" << endl ;
}



GustAngleMeasure::GustAngleMeasure(long timestamp, double value)
    :m_timestamp(timestamp), m_value(value)
{

}

GustAngleMeasure::~GustAngleMeasure() {
//    cout << "GustAngleMeasure::~GustAngleMeasure()" << endl ;
}
