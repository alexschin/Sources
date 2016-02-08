#if not defined(__RfSwitch_Brennenstuhl_h__)
#define __RfSwitch_Brennenstuhl_h__

#include <Arduino.h>

#define RfSwitch_Brennenstuhl_CODE(senderId, deviceId, command) \
          (((((int)senderId) & 0b11111) << 7) | ((((int)deviceId) & 0b11111) << 2) | (((int)command) & 0b11))
          
#define RfSwitch_Brennenstuhl_CODE_ON(senderId, deviceId) \
          RfSwitch_Brennenstuhl_CODE(senderId, deviceId, RfSwitch_Brennenstuhl::COMMAND_ON)

#define RfSwitch_Brennenstuhl_CODE_OFF(senderId, deviceId) \
          RfSwitch_Brennenstuhl_CODE(senderId, deviceId, RfSwitch_Brennenstuhl::COMMAND_OFF)


class RfSwitch_Brennenstuhl {
  public:
    RfSwitch_Brennenstuhl(int pinSND, int pinRCV);


  public:
    void begin();
    void end();
    void loop();


  public:
    bool send(int code, int repeat, int pre_delay);
    inline bool sendSwitchOn(int senderId, int deviceId) __attribute__((always_inline)) {
      return send(RfSwitch_Brennenstuhl_CODE_ON(senderId, deviceId), _send_repeat, _send_pre_delay);
    };
    inline bool sendSwitchOff(int senderId, int deviceId) __attribute__((always_inline)) {
      return send(RfSwitch_Brennenstuhl_CODE_OFF(senderId, deviceId), _send_repeat, _send_pre_delay);
    };


  public:
    int _send_repeat    = 5; // *** resend 5 times ***
    inline int getSendRepeat() const __attribute__((always_inline)) { return _send_repeat; };
    inline int setSendRepeat(int repeat) __attribute__((always_inline)) { _send_repeat = repeat; };
    
    int _send_pre_delay = 10; // *** 10 ms ***
    inline int getSendPreDelay() const __attribute__((always_inline)) { return _send_pre_delay; };
    inline int setSendPreDelay(int pre_delay) __attribute__((always_inline)) { _send_pre_delay = pre_delay; };


  public:
    bool recv(int* code_value, int* code_repeat = NULL);
    inline int getRecvSenderId(int code_value) const __attribute__((always_inline)) { return ((code_value >> 7) & 0b11111); };
    inline int getRecvDeviceId(int code_value) __attribute__((always_inline)) { return ((code_value >> 2) & 0b11111); };
    inline int getRecvCommand(int code_value)  __attribute__((always_inline)) { return (code_value & 0b11); };


  public:
    int _recv_timeout = 150;
    inline int getRecvTimeout() const __attribute__((always_inline))           { return _recv_timeout; };
    inline int setRecvTimeout(int recv_timeout) __attribute__((always_inline)) { _recv_timeout = recv_timeout; };


  public:
    static const int NO_PIN      = 0xff;
    static const int COMMAND_ON  = 0b10;
    static const int COMMAND_OFF = 0b01;
};

#endif
