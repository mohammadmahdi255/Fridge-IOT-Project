#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H
#include <SegmentDisplay.h>
#include <Relay.h>
#include "Sensors.h"
#include "Setting.h"

#define HOLDING -3

#define EMERGENCY -2
#define FETAL_ERROR -1
#define OFF 0
#define INIT 1
#define COOLING 2
#define DOOR_OPEN 3
#define SUSPEND 4

#define UPDATE_TEMP 0
#define ERROR_DISPLAY 1
#define TEMP_DISPLAY 2
#define OFF_SEVSEG 3

class ControlUnit : public SensorUnit, public SettingUnit
{
private:
      int currentState = OFF;
      Relay compressor;
      Relay lamp;
      Relay fan;
      Relay ionizer;
      Relay buzzer;

      bool state[3] = {false, false, false};

      float lastStateTemp = 0.0f;

      int globalState = OFF_SEVSEG;
      int localState = TEMP_DISPLAY;

      bool shouldRest = false;
      bool sound = false;

      void updateSystem(int nextState);
      int nextState(int reading);
      void enQueue(int value);
      int deQueue();
      SegmentDisplay sev[2];

public:
      ControlUnit();
      ControlUnit(int pinCompressor, int pinLamp, int pinFan, int pinIonizer, int pinBuzzer, bool isNormallyOpen, int pinKey, int pinLm35, int pinDoor, const int pinSev[], int pinSev0, int pinSev1);
      int getState();
      void process();
      void displayTemp(float temp);
};
#endif
