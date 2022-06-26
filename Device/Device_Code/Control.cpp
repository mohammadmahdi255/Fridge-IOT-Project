#include "Control.h"

ControlUnit::ControlUnit(int pin_compressor, int pin_lamp, int pin_fan, int pin_ionizer, bool isNormallyOpen)
{
  lastState = TURNOFF;
  compressor = Relay(pin_compressor, isNormallyOpen);
  lamp = Relay(pin_lamp, isNormallyOpen);
  fan = Relay(pin_fan, isNormallyOpen);
  ionizer = Relay(pin_ionizer, isNormallyOpen);
}

void ControlUnit::begin()
{
	compressor.begin();
  lamp.begin();
  fan.begin();
  ionizer.begin();
}

void ControlUnit::update(int nextState)
{
  switch (nextState)
  {
  case TURNOFF:
    /* code */
    break;
  case TURNOFF:
    /* code */
    break;
  case TURNOFF:
    /* code */
    break;
  case TURNOFF:
    /* code */
    break;
  default:
    /* code */
    break;
  }
}