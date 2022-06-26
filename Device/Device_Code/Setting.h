#ifndef SETTING_UNIT_H
#define SETTING_UNIT_H

class SettingUnit
{
      private:
            float temperature = 8.0f;
            bool superCooling = false;
            void save();
      protected:
            void setTemperature(float temperature);
            void SuperCoolingSwitch();
            void load();
      public:
            SettingUnit();
            float getTemperature();
            bool isSuperCooling();
};

#endif
