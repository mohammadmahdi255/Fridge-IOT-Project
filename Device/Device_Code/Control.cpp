#include "Control.h"

ControlUnit::ControlUnit(){}

ControlUnit::ControlUnit(int pinCompressor, int pinLamp, int pinFan, int pinIonizer, int pinBuzzer, bool isNormallyOpen, int pinKey, int pinLm35, int pinDoor, const int pinSev[])
{
      compressor = Relay(pinCompressor, isNormallyOpen);
      lamp = Relay(pinLamp, isNormallyOpen);
      fan = Relay(pinFan, isNormallyOpen);
      ionizer = Relay(pinIonizer, isNormallyOpen);
      buzzer = Relay(pinBuzzer, isNormallyOpen);
      SensorUnit(pinLm35, pinDoor, pinKey);
      SettingUnit();
      sev[1] = SegmentDisplay(pinSev[0], pinSev[1], pinSev[2], pinSev[3], pinSev[4], pinSev[5], pinSev[6], pinSev[7]);
      sev[0] = SegmentDisplay(pinSev[8], pinSev[9], pinSev[10], pinSev[11], pinSev[12], pinSev[13], pinSev[14], pinSev[15]);
}

void ControlUnit::updateSystem(int nextState)
{
      if(currentState < COOLING && nextState == INITUP) {
            compressor.begin();
            lamp.begin();
            fan.begin();
            ionizer.begin();
      } else if(currentState != TURNOFF && nextState == COOLING) {
            compressor.turnOn();
            lamp.turnOff();
            fan.turnOn();
            ionizer.turnOn();
      } else if(currentState != TURNOFF && nextState == WAITING) {
            compressor.turnOff();
            lamp.turnOn();
            fan.turnOff();
            ionizer.turnOff();
      } else if(nextState == TURNOFF) {
            compressor.turnOff();
            lamp.turnOff();
            fan.turnOff();
            ionizer.turnOff();
      }

      currentState = nextState;

}

int ControlUnit::getState()
{
      return currentState;
}

void ControlUnit::process()
{
      if(currentState == TURNOFF) {
            int a = readKey();
           if(a>-1)
             Serial.println(a);
      }
}
