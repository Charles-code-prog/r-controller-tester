#include <Wire.h>
#include <IRremote.h>
#include <avr/wdt.h>

// Definindo endereços I2C para os slaves
#define SLAVE1_ADDRESS 1
#define SLAVE2_ADDRESS 2
#define SLAVE3_ADDRESS 3

// Pino do receptor IR
#define IR_RECEIVE_PIN 7
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;
void setup(){

  Serial.begin(38400);
  Serial.print("Ativo");
  irrecv.enableIRIn(); // Start the receiver
  wdt_enable(WDTO_500MS);
}

void loop(){
  wdt_reset();
  if(irrecv.decode(&results)){
    Serial.println("OK");
    irrecv.resume();
  }

}

void requestSlave(){

}