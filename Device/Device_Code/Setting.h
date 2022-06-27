#ifndef SETTING_UNIT_H
#define SETTING_UNIT_H

#define WORKTIME 1800000 // 30 min
#define RESTTIME 600000 // 10 min
#define OPENDOOR 30000 // 30 sec
#define TIMEOUT  60000 // 60 sec
#define SEGTIME   500

class SettingUnit
{
      private:
            float temperature = 8.0f;
            bool superCooling = false;
            void save();
      protected:
            float tolerance = 3.0f;
            unsigned long longWorkTime = 2 * WORKTIME;
            unsigned long workTime = WORKTIME;
            unsigned long restTime = RESTTIME;
            unsigned long openDoor = OPENDOOR;
            unsigned long timeOut = TIMEOUT;
            float warningTemp = 16.0f;
            float criticalTemp = 20.0f;
            void setTemperature(float temperature);
            void incTemperature();
            void decTemperature();
            void SuperCoolingSwitch();
            void load();
      public:
            SettingUnit();
            float getTemperature();
            bool isSuperCooling();
};

#endif
