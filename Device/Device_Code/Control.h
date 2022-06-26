#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H
#include <SegmentDisplay.h>
#include <Relay.h>
#include "Sensors.h"
#include "Setting.h"

#define TURNOFF       -1
#define INITUP            0
#define COOLING       1
#define WAITING        2

class ControlUnit : public SensorUnit, SettingUnit
{
      private:
            int currentState = TURNOFF;
            Relay compressor;
            Relay lamp;
            Relay fan;
            Relay ionizer;
            Relay buzzer;
            void updateSystem(int nextState);
      public:
            SegmentDisplay sev[2];
            ControlUnit();
            ControlUnit(int pinCompressor, int pinLamp, int pinFan, int pinIonizer, int pinBuzzer, bool isNormallyOpen, int pinKey, int pinLm35, int pinDoor, const int pinSev[]);
            int getState();
            void process();
};
#endif
