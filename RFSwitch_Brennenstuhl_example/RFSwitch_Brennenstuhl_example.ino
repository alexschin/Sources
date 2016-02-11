/*
  RF-Switch Brennenstuhl RCS 1000 N example
  uses Arduino library for remote control Brennenstuhl switches.
  
  Copyright (c) 2016 Alexander Schindowski.  All right reserved.
  
  Project home: http://github.com/alexschin/Sources

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "RfSwitch_Brennenstuhl.h"

#if defined(ESP8266)
  #define PIN_SEND  D4
  #define PIN_RECV  D5
#else
  #define PIN_SEND  4
  #define PIN_RECV  3
#endif

RfSwitch_Brennenstuhl rfSwitch(PIN_SEND, PIN_RECV);


void setup() {
  Serial.begin(115200); delay(100);
  
  Serial.println(); Serial.println(F("## RfSwitch_Brennenstuhl"));
  Serial.print(F("## PIN_SEND = ")); Serial.println(PIN_SEND, DEC);
  Serial.print(F("## PIN_RECV = ")); Serial.println(PIN_RECV, DEC);

  rfSwitch.begin();
  //rfSwitch.setSendRepeat(5);
  //rfSwitch.setSendPreDelay(10);
  //rfSwitch.setRecvTimeout(200);
  
  Serial.print(F("## rfSwitch.getSendRepeat()   = ")); Serial.println(rfSwitch.getSendRepeat(), DEC);
  Serial.print(F("## rfSwitch.getSendPreDelay() = ")); Serial.println(rfSwitch.getSendPreDelay(), DEC);
  Serial.print(F("## rfSwitch.getRecvTimeout()  = ")); Serial.println(rfSwitch.getRecvTimeout(), DEC);
}


void loop() {
  int code, repeat;

  if (rfSwitch.recv(&code, &repeat)) {
    Serial.print(F("## code = 0b")); Serial.print(code, BIN); Serial.print(F(", repeat = ")); Serial.print(repeat, DEC); Serial.println();
    Serial.print(F("## senderId = 0b")); Serial.print(rfSwitch.getRecvSenderId(code), BIN); Serial.print(F(", deviceId = 0b")); Serial.print(rfSwitch.getRecvDeviceId(code), BIN); Serial.print(F(", command = 0b")); Serial.print(rfSwitch.getRecvCommand(code), BIN); Serial.println();
    Serial.print(F("## getRecvPulseWidthUS = ")); Serial.println(rfSwitch.getRecvPulseWidthUS(), DEC);

    switch (code) {
      case RfSwitch_Brennenstuhl_CODE(0b10111, 0b10000, RfSwitch_Brennenstuhl::COMMAND_ON): {
        Serial.println(F("ON - SWITCH_A"));
        rfSwitch.sendSwitchOn(0b10111, 0b11000); 
        rfSwitch.sendSwitchOn(0b11110, 0b00100); 
      } break;
      
      case RfSwitch_Brennenstuhl_CODE(0b10111, 0b10000, RfSwitch_Brennenstuhl::COMMAND_OFF): {
        Serial.println(F("OFF - SWITCH_A"));
        rfSwitch.sendSwitchOff(0b10111, 0b11000); 
        rfSwitch.sendSwitchOff(0b11110, 0b00100);
      } break;
      
      case 0b101110001010:
      case 0b111100001010: {
        Serial.println(F("ON"));
        rfSwitch.sendSwitchOn(0b11110, 0b00100);
        rfSwitch.sendSwitchOn(0b11110, 0b01000);
        rfSwitch.sendSwitchOn(0b11110, 0b10000);
      } break;
      
      case 0b101110001001:
      case 0b111100001001: {
        Serial.println(F("OFF"));
        rfSwitch.sendSwitchOff(0b11110, 0b00100);
        rfSwitch.sendSwitchOff(0b11110, 0b01000);
        rfSwitch.sendSwitchOff(0b11110, 0b10000);
      } break;
    }
  }

  rfSwitch.loop();
}

