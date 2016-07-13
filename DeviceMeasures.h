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

#ifndef DEVICEMEASURES_H
#define DEVICEMEASURES_H

class TemperatureMeasure
{
private:
    long        m_timestamp;
    double      m_value;

public:
    TemperatureMeasure(long timestamp, double value);
    ~TemperatureMeasure();

    long        getTimestamp() const    { return m_timestamp; }
    double      getValue() const        { return m_value; }
};

class CO2Measure
{
private:
    long        m_timestamp;
    double      m_value;

public:
    CO2Measure(long timestamp, double value);
    ~CO2Measure();

    long        getTimestamp() const    { return m_timestamp; }
    double      getValue() const        { return m_value; }
};

class HumidityMeasure
{
private:
    long        m_timestamp;
    double      m_value;

public:
    HumidityMeasure(long timestamp, double value);
    ~HumidityMeasure();

    long        getTimestamp() const    { return m_timestamp; }
    double      getValue() const        { return m_value; }
};

class NoiseMeasure
{
private:
    long        m_timestamp;
    double      m_value;

public:
    NoiseMeasure(long timestamp, double value);
    ~NoiseMeasure();

    long        getTimestamp() const    { return m_timestamp; }
    double      getValue() const        { return m_value; }
};

class PressureMeasure
{
private:
    long        m_timestamp;
    double      m_value;

public:
    PressureMeasure(long timestamp, double value);
    ~PressureMeasure();

    long        getTimestamp() const    { return m_timestamp; }
    double      getValue() const        { return m_value; }
};

class PluviometrieMeasure
{
private:
    long        m_timestamp;
    double      m_value;

public:
    PluviometrieMeasure(long timestamp, double value);
    ~PluviometrieMeasure();

    long        getTimestamp() const    { return m_timestamp; }
    double      getValue() const        { return m_value; }
};


//---
//   Wind
class WindStrengthMeasure
{
private:
    long        m_timestamp;
    double      m_value;

public:
    WindStrengthMeasure(long timestamp, double value);
    ~WindStrengthMeasure();

    long        getTimestamp() const    { return m_timestamp; }
    double      getValue() const        { return m_value; }
};

class WindAngleMeasure
{
private:
    long        m_timestamp;
    double      m_value;

public:
    WindAngleMeasure(long timestamp, double value);
    ~WindAngleMeasure();

    long        getTimestamp() const    { return m_timestamp; }
    double      getValue() const        { return m_value; }
};

class GustStrengthMeasure
{
private:
    long        m_timestamp;
    double      m_value;

public:
    GustStrengthMeasure(long timestamp, double value);
    ~GustStrengthMeasure();

    long        getTimestamp() const    { return m_timestamp; }
    double      getValue() const        { return m_value; }
};

class GustAngleMeasure
{
private:
    long        m_timestamp;
    double      m_value;

public:
    GustAngleMeasure(long timestamp, double value);
    ~GustAngleMeasure();

    long        getTimestamp() const    { return m_timestamp; }
    double      getValue() const        { return m_value; }
};
#endif // DEVICEMEASURES_H
