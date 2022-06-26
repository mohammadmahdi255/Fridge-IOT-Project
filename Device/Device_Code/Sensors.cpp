#include "Sensors.h"
#include "math.h"

SensorUnit::SensorUnit(){}

SensorUnit::SensorUnit(int pinLm35, int pinDoor, int pinKey)
{
    this->pinLm35 = pinLm35;
    this->pinDoor = pinDoor;
    this->pinKey = pinKey;
}

void SensorUnit::begin()
{
    pinMode(pinDoor, INPUT);
}

void SensorUnit::update()
{
    DoorOpen = digitalRead(pinDoor);
    currentTemperature = map(analogRead(pinLm35), 0, 325, 0, 40);
}

float SensorUnit::getCurrentTemperature()
{
    return currentTemperature;
}

bool SensorUnit::isDoorOpen()
{
    return DoorOpen;
}

int SensorUnit::readKey()
{
      while(true)
      {
            int reading = calculateKey();
            
            if(reading != lastButtonState)
                  lastDebounceTime = millis();
            
            if ((millis() - lastDebounceTime) > debounceDelay[reading+1]) {
                  if (reading != buttonState) {
                        buttonState = reading;
                        return buttonState;
                  } else {
                        return -1;
                  }
            }
            
            lastButtonState = reading;
      }
}

int SensorUnit::calculateKey()
{
      float VR_Calculated, RT, voltageMinimum, voltageMaximum, voltage = sourceVoltage * analogRead(pinKey) / 1023;
    
      for(unsigned int i=pushbuttonsCount; i > 0; i--)
      { 
      
            RT = 1/(i * resitorValue) + 1/resitorToGround;
            RT = 1/RT;
            
            VR_Calculated =(sourceVoltage * RT) / ((pushbuttonsCount - i + 1) * resitorValue + RT);
            
            voltageMinimum = (1 - V_tolerance) * VR_Calculated;
            voltageMaximum = (1 + V_tolerance) * VR_Calculated;
            
            if(voltageMinimum < voltage && voltage < voltageMaximum)
            return pushbuttonsCount - i;
      }
      
      return -1;
}