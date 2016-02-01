/* 
 *  
 */

#include "RfSwitch_Brennenstuhl.h"

RfSwitch_Brennenstuhl rfSwitch(4, 3);

void setup() {
  Serial.begin(115200); delay(100);
  Serial.println(F("## RfSwitch_Brennenstuhl"));

  pinMode(13, OUTPUT); digitalWrite(13, LOW);

  rfSwitch.begin();
}


void loop() {
  static unsigned long _last_millis = 0;
  static unsigned int  _count = 0;
  static bool          _switchOn = false;


  static unsigned int _last_code = 0;
  static unsigned int _last_code_delay = 0;

  /*--*/
  unsigned int code;
  if (rfSwitch.getReceivedValue(&code)) {
    // Serial.println(code, BIN);

    if (code & 0b1111100) {
      _last_code = code;
    }
    _last_code_delay = 100;
  }
  //-*/

  if (_last_millis != millis()) {
    
    if (_last_code_delay && --_last_code_delay == 0) {
      // Serial.print(F("0b")); Serial.println(_last_code, BIN);
      
      switch (_last_code) {
        case 0b100010001010: if (true) {
          _switchOn = true;
          //if (fnc != NULL) fnc();
          Serial.println(F("ON"));
          rfSwitch.send(0b111100100010, 5);
        } break;
        
        case 0b100010001001: if (true) {
          _switchOn = false;
          //if (fnc != NULL) fnc();
          Serial.println(F("OFF"));
          rfSwitch.send(0b111100100001, 5);
        } break;
      }

      _last_code = 0;
    }

    _last_millis = millis();
  }


  /*
  */
  
  rfSwitch.loop();
}

