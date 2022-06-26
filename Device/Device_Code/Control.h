#include <Relay.h>

#define TURNOFF  -1
#define INITUP  0
#define COOLING  1
#define WAITING 2

class ControlUnit
{
      private:
            int lastState;
            Relay compressor;
            Relay lamp;
            Relay fan;
            Relay ionizer;
      public:
            ControlUnit(int pin_compressor, int pin_lamp, int pin_fan, int pin_ionizer, bool isNormallyOpen);
            void begin();
            void update(int nextState);
            void getState();
};
