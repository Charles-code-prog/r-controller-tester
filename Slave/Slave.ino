#include <IRremote.hpp>
#include <Wire.h>
#include <avr/wdt.h>

#define IRPin 7

char IR[4] = "0;";

void setup(){
  Serial.begin(115200); // // Establish serial communication
  IrReceiver.begin(IRPin, ENABLE_LED_FEEDBACK); // Start the receiver
}

void loop(){
  if(IrReceiver.decode()){
    strcpy(IR, "OK;");
    IrReceiver.resume(); // Enable receiving of the next value
  }
  if(Serial.available()){
    char c = Serial.read();
    if(c == '1'){
      Serial.println(IR);
      strcpy(IR, "0;");
    }
  }
}