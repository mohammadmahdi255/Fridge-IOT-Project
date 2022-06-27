#include "Control.h"

ControlUnit::ControlUnit(){}

ControlUnit::ControlUnit(int pinCompressor, int pinLamp, int pinFan, int pinIonizer, int pinBuzzer, bool isNormallyOpen, int pinKey, int pinLm35, int pinDoor, const int pinSev[], int pinSev0, int pinSev1)
: SensorUnit(pinLm35, pinDoor, pinKey), SettingUnit()
{
      compressor = Relay(pinCompressor, isNormallyOpen);
      lamp = Relay(pinLamp, isNormallyOpen);
      fan = Relay(pinFan, isNormallyOpen);
      ionizer = Relay(pinIonizer, isNormallyOpen);
      buzzer = Relay(pinBuzzer, isNormallyOpen);
      sev[1] = SegmentDisplay(pinSev1, pinSev[0], pinSev[1], pinSev[2], pinSev[3], pinSev[4], pinSev[5], pinSev[6], pinSev[7]);
      sev[0] = SegmentDisplay(pinSev0, pinSev[8], pinSev[9], pinSev[10], pinSev[11], pinSev[12], pinSev[13], pinSev[14], pinSev[15]);
}

void ControlUnit::updateSystem(int nextState)
{
      if(nextState == HOLDING)
            return;
            
      if(nextState == TURNOFF || nextState == EMERGENCY) {
            compressor.change(false);
            lamp.change(false);
            fan.change(false);
            ionizer.change(false);
            buzzer.change(sound);
            timerStart = millis();
      } else if(nextState == INITUP) {
            compressor.begin();
            lamp.begin();
            fan.begin();
            ionizer.begin();
            buzzer.begin();
      } else if(nextState == COOLING || nextState == WAITING) {
            compressor.change(nextState == COOLING);
            lamp.change(nextState == WAITING);
            fan.change(nextState == COOLING);
            ionizer.change(nextState == COOLING);
            timerStart = millis();
      } else if(nextState == WARNING || nextState == CRITICAL) {
            compressor.change(isDoorClose());
            lamp.change(!isDoorClose());
            fan.change(isDoorClose());
            ionizer.change(isDoorClose());
            buzzer.change(sound);
            timerStart = millis();
      } 

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
      case MUTE:
            buzzer.change(!canMuted);
            sound = !canMuted;
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

      if(millis() - segTimer > SEGTIME) {
            showError = (showError + 1) % 4;
            segTimer = millis();
      }

      if(showError == 0) {
            switch (currentState) {
                  case TURNOFF:
                        sev[1].turnOff();
                        sev[0].turnOff();
                        break;
                  default:
                        displayTemp();
                        break;
            }
      } else if(showError == 2) {
           switch (currentState) {
                  case TURNOFF:
                        sev[1].turnOff();
                        sev[0].turnOff();
                        break;
                  case WARNING:
                        sev[1].displayHex(10, false);
                        sev[0].displayHex(17, false);
                        break;
                  case CRITICAL:
                        sev[1].displayHex(12, false);
                        sev[0].displayHex(17, false);
                        break;
                  case EMERGENCY:
                        sev[1].displayHex(15, false);
                        sev[0].displayHex(14, false);
                        break;
                  default:
                        displayTemp();
                        break;
            }
      }  else {
            switch (currentState) {
                  case TURNOFF:
                  case WARNING:
                  case CRITICAL:
                  case EMERGENCY:
                        sev[1].turnOff();
                        sev[0].turnOff();
                        break;
                  default:
                        displayTemp();
                        break;
            }
      } 
      
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
            if(reading == POWER) {
                  sound = false;
                  return TURNOFF;
            }

            if(getCurrentTemperature() > getTemperature() + tolerance && isDoorClose()) 
                  return COOLING;

            if(getCurrentTemperature() > criticalTemp) {
                  sound = true;
                  return CRITICAL;
            }

            if(getCurrentTemperature() > warningTemp) {
                  sound = true;
                  return WARNING;
            }

            return WAITING;

      case COOLING:
            if(reading == POWER) {
                  sound = false;
                  return TURNOFF;
            }

            if(getCurrentTemperature() > criticalTemp && (millis() - timerStart > workTime || !isDoorClose())) {
                  sound = true;
                  return CRITICAL;
            }

            if(getCurrentTemperature() > warningTemp && (millis() - timerStart > workTime || !isDoorClose())) {
                  sound = true;
                  return WARNING;
            }

            if(getCurrentTemperature() <= getTemperature() || !isDoorClose() || millis() - timerStart > workTime)
                  return WAITING;
            break;

      case WAITING:
            if(reading == POWER) {
                  sound = false;
                  return TURNOFF;
            }
            
            if(!isDoorClose() && millis() - timerStart > openDoor) {
                  sound = true;
                  return WARNING;
            }     

            if(getCurrentTemperature() > getTemperature() + tolerance && isDoorClose() && millis() - timerStart > restTime) 
                  return COOLING;

            break;

      case WARNING:
            if(reading == POWER) {
                  sound = false;
                  return TURNOFF;
            }
            
            if(isDoorClose() && !compressor.getState()) {
                  canMuted = true;
                  sound = false;
                  return WARNING;
            }

            if(getCurrentTemperature() > warningTemp && !isDoorClose() && millis() - timerStart > openDoor) {
                  canMuted = true;
                  sound = true;
                  return CRITICAL;
            }

            if(getCurrentTemperature() > warningTemp && isDoorClose() && millis() - timerStart > longWorkTime) {
                  canMuted = true;
                  sound = true;
                  return CRITICAL;
            }

            if(getCurrentTemperature() < warningTemp) {
                  canMuted = true;
                  sound = false;
                  return COOLING;
            }

            break;
      
      case CRITICAL:
            if(reading == POWER) {
                  canMuted = true;
                  sound = false;
                  return TURNOFF;
            }
            
            if(isDoorClose() && !compressor.getState()) {
                  canMuted = true;
                  sound = false;
                  return CRITICAL;
            }

            if(getCurrentTemperature() > criticalTemp && !isDoorClose()) {
                  canMuted = false;
                  sound = true;
                  return CRITICAL;
            }

            if(getCurrentTemperature() > criticalTemp && isDoorClose() && millis() - timerStart > longWorkTime) {
                  canMuted = false;
                  sound = true;
                  return EMERGENCY;
            }

            if(getCurrentTemperature() < criticalTemp) {
                  canMuted = true;
                  sound = false;
                  return WARNING;
            }

            break;
      
      case EMERGENCY:
            if(reading == POWER) {
                  canMuted = true;
                  sound = false;
                  return TURNOFF;
            }
            
            if(isDoorClose())
                  return CRITICAL;
            if(millis() - timerStart > timeOut)
                  return TURNOFF;
      }
      return HOLDING;
}

void ControlUnit::displayTemp()
{
      float temp = getCurrentTemperature();
      if(temp < -9 || temp > 99)
            return;

      if(fabs(temp) < 1) {
            sev[1].displayHex(temp < 0 ? 16 : 0, true);
            sev[0].displayHex(round(fabs(temp) * 10), false);
      } else if(temp < 0) {
            sev[1].displayHex(16, false);
            sev[0].displayHex(round(fabs(temp)), false);
      } else if (temp < 10) {
            sev[1].displayHex(temp, true);
            sev[0].displayHex(floor((temp - floor(temp)) * 10), false);
      } else {
            sev[1].displayHex(round(temp) / 10, false);
            sev[0].displayHex(round(temp) % 10, false);
      }
      
}
