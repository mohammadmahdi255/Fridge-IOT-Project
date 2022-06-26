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
      if(nextState == HOLDING)
            return;

      if(nextState == TURNOFF) {
            compressor.turnOff();
            lamp.turnOff();
            fan.turnOff();
            ionizer.turnOff();
      } else if(nextState == INITUP) {
            compressor.begin();
            lamp.begin();
            fan.begin();
            ionizer.begin();
      } else if(nextState == COOLING) {
            compressor.turnOn();
            lamp.turnOff();
            fan.turnOn();
            ionizer.turnOn();
            timerStart = millis();
      } else if(nextState == WAITING) {
            compressor.turnOff();
            lamp.turnOn();
            fan.turnOff();
            ionizer.turnOff();
            timerStart = millis();
      } else if(nextState == WARNING) {
            if(isDoorOpen()) {
                  compressor.turnOff();
                  lamp.turnOn();
                  fan.turnOff();
                  ionizer.turnOff();
                  buzzer.turnOn();
            } else {
                  compressor.turnOn();
                  lamp.turnOff();
                  fan.turnOn();
                  ionizer.turnOn();
            }
            timerStart = millis();
      } else if(nextState == CRITICAL) {
            if(isDoorOpen()) {
                  compressor.turnOff();
                  lamp.turnOn();
                  fan.turnOff();
                  ionizer.turnOff();
            } else {
                  compressor.turnOn();
                  lamp.turnOff();
                  fan.turnOn();
                  ionizer.turnOn();
            }
            if(!isMuted)
                  buzzer.turnOn();
            timerStart = millis();
      } else if(nextState == EMERGENCY) {
            compressor.turnOff();
            lamp.turnOff();
            fan.turnOff();
            ionizer.turnOff();
            buzzer.turnOn();
      }


      canMuted = nextState != EMERGENCY;
      isMuted = false;
      currentState = nextState;

}

int ControlUnit::getState()
{
      return currentState;
}

void ControlUnit::process()
{     
      update();
      int reading = readKey();

      switch (reading)
      {

      case POWER:
            currentState = TURNOFF;
            break;

      case MUTE:
            if(canMuted){
                  buzzer.turnOff();
                  isMuted = true;
            }
            break;
      
      case UP:
            incTemperature();
            break;
      
      case DOWN:
            decTemperature();
            break;

      case SUPER:
            SuperCoolingSwitch();
            break;
      }

      updateSystem(nextState(reading));

}

int ControlUnit::nextState(int reading)
{
      switch (currentState)
      {
      case TURNOFF:
            if(reading == POWER) 
                  return INITUP;
            break;

      case INITUP:
            if(getCurrentTemperature() > getTemperature() + tolerance && !isDoorOpen()) 
                  return COOLING;

            if(getCurrentTemperature() > criticalTemp)
                  return CRITICAL;

            if(getCurrentTemperature() > warningTemp) 
                  return WARNING;

            return WAITING;

      case COOLING:
            if(getCurrentTemperature() > criticalTemp && millis() - timerStart > workTime)
                  return CRITICAL;

            if(getCurrentTemperature() > warningTemp && millis() - timerStart > workTime) 
                  return WARNING;

            if(getCurrentTemperature() <= getTemperature() || isDoorOpen() || millis() - timerStart > workTime)
                  return WAITING;
            break;

      case WAITING:
            if(isDoorOpen() && millis() - timerStart > openDoor)
                  return WARNING;

            if(getCurrentTemperature() > getTemperature() + tolerance && !isDoorOpen() && millis() - timerStart > restTime) 
                  return COOLING;

            break;

      case WARNING:
            if(!isDoorOpen() && !compressor.getState())
                  return WARNING;

            if(getCurrentTemperature() > warningTemp && isDoorOpen() && millis() - timerStart > openDoor)
                  return CRITICAL;

            if(getCurrentTemperature() > warningTemp && !isDoorOpen() && millis() - timerStart > longWorkTime)
                  return CRITICAL;

            if(getCurrentTemperature() > getTemperature() + tolerance && !isDoorOpen() && millis() - timerStart > workTime) 
                  return COOLING;

            break;
      
      case CRITICAL:
            if(!isDoorOpen() && !compressor.getState())
                  return CRITICAL;

            if(getCurrentTemperature() > criticalTemp && isDoorOpen() && millis() - timerStart > openDoor) {
                  buzzer.turnOn();
                  canMuted = false;
            }

            if(getCurrentTemperature() > criticalTemp && !isDoorOpen() && millis() - timerStart > longWorkTime)
                  return EMERGENCY;

            if(getCurrentTemperature() > getTemperature() + tolerance && !isDoorOpen() && millis() - timerStart > longWorkTime) 
                  return COOLING;

            break;
      
      case EMERGENCY:
            if(isDoorOpen())
                  return EMERGENCY;

            if(!isDoorOpen())
                  return CRITICAL;
                  
            break;
      }
      return HOLDING;
}
