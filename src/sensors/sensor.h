/* 
 *  RRIV - Open Source Environmental Data Logging Platform
 *  Copyright (C) 20202  Zaven Arra  zaven.arra@gmail.com
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef WATERBEAR_SENSOR_DRIVER
#define WATERBEAR_SENSOR_DRIVER

#include <Arduino.h>
#include <Wire_slave.h>
#include <cJSON.h>

typedef enum protocol { analog, i2c } protocol_type;

#define SENSOR_CONFIGURATION_SIZE 64

typedef struct 
{
    // note needs to be 32 bytes total (multiple of 4)
    // rearrange in blocks of 4bytes for diagram
    // sensor.h
    unsigned short int sensor_type; // 2 bytes
    byte slot; // 1 byte
    byte burst_size; // 1 byte
    unsigned short int warmup; // 2 bytes, in seconds? (65535 max value/60=1092 min)
    char paddingOne[2]; // 2bytes
    char tag[6]; // 6 bytes
       
    char paddingTwo[18]; // 32-14
} common_config_sensor;

typedef struct 
{
  common_config_sensor common;
  char padding[32];
} generic_config;


class SensorDriver {

  //
  // SensorDriver Implementation
  //
  public:
    // Constructor
    SensorDriver();
    void configureFromJSON(cJSON * json);
    void configure(generic_config configuration);
    void setDefaults();

    void initializeBurst();
    void incrementBurst();
    bool burstCompleted();

    char * getCSVColumnNames();

  protected:
    char csvColumnHeaders[100] = "column_header";
    void configureCSVColumns();


  private:
    short burstCount = 0;


  //
  // Custom Driver Implementation Interface
  // Implemented in Child Classes
  //
  public: 
    virtual ~SensorDriver();
    virtual generic_config getConfiguration() = 0;
    virtual void setConfiguration(generic_config configuration) = 0;
    virtual cJSON * getConfigurationJSON() = 0; // returns unprotected pointer
    virtual void setup() = 0;
    virtual void stop() = 0;
    virtual bool takeMeasurement() = 0; // return true if measurement successful
    virtual char * getDataString() = 0;
    virtual protocol_type getProtocol() = 0;;
    virtual const char * getBaseColumnHeaders() = 0;
    virtual bool isWarmedUp();

    // Calibration
    virtual void initCalibration() = 0;;
    virtual void calibrationStep(char * step, int arg_cnt, char ** args) = 0;

  protected:
    // Implementation interface
    virtual void configureDriverFromJSON(cJSON * json) = 0;
    virtual void setDriverDefaults() = 0;

};

class AnalogSensorDriver : public SensorDriver {
  public:
    ~AnalogSensorDriver();
};

class I2CSensorDriver : public SensorDriver {
  public:
    ~I2CSensorDriver();
    void setWire(TwoWire * wire);

  protected:
    TwoWire * wire;
};


void getDefaultsCommon(common_config_sensor *fillValues);
void readCommonConfigOnly(common_config_sensor *readValues); //not made


#endif
