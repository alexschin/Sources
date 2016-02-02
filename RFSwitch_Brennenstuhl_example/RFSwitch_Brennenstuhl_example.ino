/* 
 *  
 */

#include "RfSwitch_Brennenstuhl.h"

RfSwitch_Brennenstuhl rfSwitch(D4, D3);

void setup() {
  Serial.begin(115200); delay(100);
  Serial.println(F("## RfSwitch_Brennenstuhl"));

  pinMode(13, OUTPUT); digitalWrite(13, LOW);

  rfSwitch.begin();
}


void loop() {
  static unsigned long _last_millis = 0;

  static unsigned int _last_code = 0;
  static unsigned int _last_code_repeat = 0;
  static unsigned int _last_code_delay = 0;

  /*--*/
  unsigned int code;
  if (rfSwitch.getReceivedValue(&code)) {
    // Serial.println(code, BIN);

    if (code & 0b1111100) {
      if (_last_code != code) {
        _last_code = code;
        _last_code_repeat = 1;
      } else {
        _last_code_repeat++;
      }
    }
    
    _last_code_delay = 200;
  }
  //-*/

  /*--*/
  if (_last_millis != millis()) {
    
    if (_last_code_delay && --_last_code_delay == 0) {
      Serial.print(F("0b")); Serial.print(_last_code, BIN); Serial.write(' '); Serial.println(_last_code_repeat, DEC);
      
      switch (_last_code) {
        case 0b101110001010:
        case 0b111100001010:
        if (_last_code_repeat >= 1) {
          Serial.println(F("ON"));
          rfSwitch.send(0b111100010010, 10, 10);
          rfSwitch.send(0b111100100010, 10, 10);
          rfSwitch.send(0b111101000010, 10, 10);
        } break;
        
        case 0b101110001001:
        case 0b111100001001:
        if (_last_code_repeat >= 1) {
          Serial.println(F("OFF"));
          rfSwitch.send(0b111100010001, 10, 10);
          rfSwitch.send(0b111100100001, 10, 10);
          rfSwitch.send(0b111101000001, 10, 10);
        } break;
      }

      _last_code = 0;
    }

    _last_millis = millis();
  }
  //-*/

  /*
  */
  
  rfSwitch.loop();
}

