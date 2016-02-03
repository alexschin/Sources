#if not defined(__RfSwitch_Brennenstuhl_cpp__)
#define __RfSwitch_Brennenstuhl_cpp__

#include <Arduino.h>
#include "RfSwitch_Brennenstuhl.h"

#define PULSE_US       300
#define PULSE_US_DELTA (PULSE_US / 4)


static int8_t _pinSND = RfSwitch_Brennenstuhl::NO_PIN;
static int8_t _pinRCV = RfSwitch_Brennenstuhl::NO_PIN;


#define TIMINGS_MAX 50
static volatile int16_t timings[TIMINGS_MAX];
static volatile int8_t  timings_position = 0;

static inline void timings_push(int v) {
  timings[timings_position++] = v;
  if (timings_position >= TIMINGS_MAX) timings_position = 0;
}

static inline int timings_pop() {
  timings_position = (timings_position != 0) ? (timings_position - 1) : (TIMINGS_MAX - 1);
  return timings[timings_position];
}

static inline int timings_clear() {
  timings_position = 0;
  memset((void*)timings, 0, sizeof(timings));
}


#define RCV_STATE_DISABLED  0x01
#define RCV_STATE_CHECKCODE 0x02

static volatile uint16_t rcv_code  = 0;
static volatile uint8_t  rcv_state = 0;

static inline bool isPulseShort(uint16_t us) {
  return (us >= 1 * (PULSE_US - PULSE_US_DELTA)) && (us <= 1 * (PULSE_US + PULSE_US_DELTA));
}

static inline bool isPulseLong(uint16_t us) {
  return (us >= 3 * (PULSE_US - PULSE_US_DELTA)) && (us <= 3 * (PULSE_US + PULSE_US_DELTA));
}

static void rcv_handleInterrupt() {
  static volatile uint32_t lastMicros = 0;

  if (!rcv_state) {
    uint32_t nowMicros = micros();
    uint32_t duration = nowMicros - lastMicros;

    if (duration > 5000) {
      if (isPulseShort(timings_pop())) {
        rcv_state |= RCV_STATE_CHECKCODE;
      }
    } else {
      timings_push(duration);
    }
    
    lastMicros = nowMicros;
  }
}

static void rcv_enable() {
  if (_pinRCV != RfSwitch_Brennenstuhl::NO_PIN) {
    attachInterrupt(digitalPinToInterrupt(_pinRCV), rcv_handleInterrupt, CHANGE);
  }
  rcv_state &= ~RCV_STATE_DISABLED;
}

static void rcv_disable() {
  rcv_state |= RCV_STATE_DISABLED;
  if (_pinRCV != RfSwitch_Brennenstuhl::NO_PIN) {
    detachInterrupt(digitalPinToInterrupt(_pinRCV));
  }
}


static bool rcv_getReceivedValue(unsigned int* value) {
  if (!(rcv_state & RCV_STATE_CHECKCODE)) return false;

  rcv_disable();
  unsigned int code = 0;
  
  for (unsigned char i = 0; i < 12; i++) {
    int p3 = timings_pop();
    int p2 = timings_pop();
    int p1 = timings_pop();
    int p0 = timings_pop();

    code >>= 1;
    if (isPulseShort(p0) && isPulseLong(p1)) {
      if (isPulseShort(p2)) {
        code |= 2048;
      } else if (!isPulseLong(p2)) {
        code = 0;
        break;
      }
    } else {
      code = 0;
      break;
    }
  }
  
  *value = code;  
  timings_push(0);
  
  rcv_state &= ~RCV_STATE_CHECKCODE;
  rcv_enable();

  return code != 0;
}



#define SEND_BUFFER_SIZE  10
static struct {
  unsigned int repeat;
  unsigned int code;
  unsigned int preDelay;
} _send_buffer[SEND_BUFFER_SIZE];


inline static void _send_on() {
  digitalWrite(_pinSND, HIGH);
  delayMicroseconds(PULSE_US * 1);
  digitalWrite(_pinSND, LOW);
  delayMicroseconds(PULSE_US * 3);
}

inline static void _send_off() {
  digitalWrite(_pinSND, HIGH);
  delayMicroseconds(PULSE_US * 3);
  digitalWrite(_pinSND, LOW);
  delayMicroseconds(PULSE_US * 1);
}

inline static void _send_sync() {
  digitalWrite(_pinSND, HIGH);
  delayMicroseconds(PULSE_US * 1);
  digitalWrite(_pinSND, LOW);
  delayMicroseconds(PULSE_US * 31);
}

static void _send(uint16_t code) {
  rcv_disable();
  if (_pinSND != RfSwitch_Brennenstuhl::NO_PIN) {
    for (uint8_t i = 12; i != 0; i--, code <<= 1) {
      _send_on();
      
      if (code & 2048) {
        _send_on();
      } else {
        _send_off();
      }
    }
    _send_sync();
  }
  rcv_enable();
}





RfSwitch_Brennenstuhl::RfSwitch_Brennenstuhl(int pinSND, int pinRCV) {
  _pinSND = pinSND;
  _pinRCV = pinRCV;
}

void RfSwitch_Brennenstuhl::begin() {
  memset(_send_buffer, 0, sizeof(_send_buffer));

  timings_position = 0;
  memset((void*)timings, 0, sizeof(timings));
  
  if (_pinSND != NO_PIN) {
    pinMode(_pinSND, OUTPUT);
    digitalWrite(_pinSND, LOW);
  }

  if (_pinRCV != NO_PIN) {
    pinMode(_pinRCV, INPUT_PULLUP);
    rcv_enable();
  }
}

void RfSwitch_Brennenstuhl::end() {
  if (_pinRCV != NO_PIN) {
    rcv_disable();
    pinMode(_pinRCV, INPUT);
    _pinRCV = NO_PIN;
  }

  memset(_send_buffer, 0, sizeof(_send_buffer));
  
  if (_pinSND != NO_PIN) {
    digitalWrite(_pinSND, LOW);
    pinMode(_pinSND, INPUT);
    _pinSND = NO_PIN;
  }
}

static unsigned long _last_millis = 0;

void RfSwitch_Brennenstuhl::loop() {
  if (_last_millis != millis()) {
    
    if (_send_buffer[0].repeat != 0) {
      if (_send_buffer[0].preDelay == 0) {
        _send(_send_buffer[0].code);
  
        _send_buffer[0].repeat--;
        if (_send_buffer[0].repeat == 0) {
          for (uint8_t i = 1; i < SEND_BUFFER_SIZE; i++) {
            _send_buffer[i - 1] = _send_buffer[i];
            if (_send_buffer[i].repeat == 0) break;
            _send_buffer[i].repeat = 0;
          }        
        }
      } else {
        _send_buffer[0].preDelay--;
      }
    }
    
    _last_millis = millis();
  }
}


void RfSwitch_Brennenstuhl::send(unsigned int code, unsigned int repeat, unsigned int preDelay) {
  for (uint8_t i = 0; i < SEND_BUFFER_SIZE; i++) {
    if (_send_buffer[i].code == code)
      break;
    if (_send_buffer[i].repeat == 0) {
      _send_buffer[i].code = code;
      _send_buffer[i].repeat = repeat;
      _send_buffer[i].preDelay = preDelay;
      break;
    }
  }
}


bool RfSwitch_Brennenstuhl::getReceivedValue(unsigned int* value) {
  return rcv_getReceivedValue(value);
}

#endif
