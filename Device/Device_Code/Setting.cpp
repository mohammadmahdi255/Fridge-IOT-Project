#include "Setting.h"
SettingUnit::SettingUnit(){}

void SettingUnit::save(){}

void SettingUnit::setTemperature(float temperature)
{
      this->temperature = temperature;
      save();
}

void SettingUnit::incTemperature()
{
      temperature++;
}

void SettingUnit::decTemperature()
{
      temperature--;
}

void SettingUnit::SuperCoolingSwitch()
{
      superCooling = !superCooling;
      save();
}

void SettingUnit::load(){}

float SettingUnit::getTemperature()
{
      return temperature > 7.0 && superCooling ?  temperature - 3.0 : temperature;
}

bool SettingUnit::isSuperCooling()
{
      return superCooling;
}
