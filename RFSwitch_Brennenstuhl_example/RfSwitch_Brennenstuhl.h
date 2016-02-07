#if not defined(__RfSwitch_Brennenstuhl_h__)
#define __RfSwitch_Brennenstuhl_h__

#include <Arduino.h>

class RfSwitch_Brennenstuhl {
  public:
    RfSwitch_Brennenstuhl(int pinSND, int pinRCV);

    void begin();
    void end();
    void loop();

    bool send(unsigned int code, unsigned int repeat = 5, unsigned int preDelay = 0);
    bool recv(unsigned int* code, unsigned int* repeat = NULL);

  public:
    static const uint8_t NO_PIN = 0xff;
};

#endif
