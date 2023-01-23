#include "SensorService.h"
#include "Arduino.h"

SensorService::SensorService(int analogGPIO, int photoresistorGPIO)
{
    AnalogGPIO = analogGPIO;
    PhotoresistorGPIO = photoresistorGPIO;
}

unsigned int SensorService::GetPhotoresistorReading()
{
    digitalWrite(PhotoresistorGPIO, HIGH);
    int value = analogRead(AnalogGPIO);
    digitalWrite(PhotoresistorGPIO, LOW);
    return value;
}