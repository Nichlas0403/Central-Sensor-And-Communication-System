#ifndef SensorService_h
#define SensorService_h
#include "Arduino.h"

class SensorService
{
    private:
        int AnalogGPIO;
        int PhotoresistorGPIO;

    public:
        SensorService(int analogGPIO, int photoresistorGPIO);
        unsigned int GetPhotoresistorReading();
};

#endif