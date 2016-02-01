#if not defined(__RfSwitch_Brennenstuhl_h__)
#define __RfSwitch_Brennenstuhl_h__

#include <Arduino.h>

class RfSwitch_Brennenstuhl {
  public:
    RfSwitch_Brennenstuhl(int pinSND, int pinRCV);

    void begin();
    void end();
    void loop();

    void send(unsigned int code, unsigned int repeat = 1, unsigned int preDelay = 0);
    bool getReceivedValue(unsigned int* value);

  public:
    static const uint8_t NO_PIN = 0xff;
};

#endif
