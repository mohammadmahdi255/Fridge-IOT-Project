#include "Setting.h"
SettingUnit::SettingUnit(){}

void SettingUnit::save(){}

void SettingUnit::load(){}

void SettingUnit::setTemperature(float temperature)
{
      this->temperature = temperature;
      save();
}

void SettingUnit::SuperCoolingSwitch()
{
      superCooling = !superCooling;
      save();
}

float SettingUnit::getTemperature()
{
      return temperature - 3.0 ? temperature > 7.0 : temperature;
}

bool SettingUnit::isSuperCooling()
{
      return superCooling;
}
