/* Brennenstuhl RF-Switch Example
 *
 */

#include "RfSwitch_Brennenstuhl.h"

#define RF_SWITCH_ON(g, s)  (((g & 0b11111) << 7) | ((s & 0b11111) << 2) | 0b01)
#define RF_SWITCH_OFF(g, s) (((g & 0b11111) << 7) | ((s & 0b11111) << 2) | 0b10)

RfSwitch_Brennenstuhl rfSwitch(D4, D3);

void setup() {
  Serial.begin(115200); delay(100);
  Serial.println(); Serial.println(F("## RfSwitch_Brennenstuhl"));

  pinMode(13, OUTPUT); digitalWrite(13, LOW);

  rfSwitch.begin();
}


void loop() {
  unsigned int code, repeat;

  if (rfSwitch.recv(&code, &repeat) && repeat > 1) {
    Serial.print(F("0b")); Serial.print(code, BIN); Serial.write(' '); Serial.print(repeat, DEC); Serial.println();

    switch (code) {
      case RF_SWITCH_ON( 0b10111, 0b10000):  { rfSwitch.send(RF_SWITCH_ON(0b11110, 0b00100)); } break;
      case RF_SWITCH_OFF(0b10111, 0b10000): { rfSwitch.send(RF_SWITCH_OFF(0b11110, 0b00100)); } break;
      
      case 0b101110001010:
      case 0b111100001010: {
        Serial.println(F("ON"));
        rfSwitch.send(0b111100010010);
        rfSwitch.send(0b111100100010);
        rfSwitch.send(0b111101000010);
      } break;
      
      case 0b101110001001:
      case 0b111100001001: {
        Serial.println(F("OFF"));
        rfSwitch.send(0b111100010001);
        rfSwitch.send(0b111100100001);
        rfSwitch.send(0b111101000001);
      } break;
    }
  }

  rfSwitch.loop();
}

