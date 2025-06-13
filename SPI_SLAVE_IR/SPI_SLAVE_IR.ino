#include <SPI.h>
#include <IRremote.hpp>

#define IR_PIN 7

volatile bool IR_recebido = false;

void setup() {
  Serial.begin(115200);
  //IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  pinMode(IR_PIN,INPUT);
  pinMode(MISO, OUTPUT);
  pinMode(10, INPUT);           // SS deve ser entrada para modo escravo
  SPCR |= _BV(SPE);             // Habilita SPI escravo
  SPI.attachInterrupt();        // Habilita interrupção SPI
}

ISR(SPI_STC_vect) {
  char comando = SPDR;

  if (comando == '1') {
    if (IR_recebido) {
      SPDR = '5';        // Envia '5' se sinal IR foi detectado
      IR_recebido = false; // Limpa o sinalizador (só responde uma vez)
    } else {
      SPDR = '0';        // Sem sinal IR, responde '0'
    }
  } else {
    SPDR = ' ';          // Qualquer outro comando → responde espaço
  }
}

void loop() {
  if (!digitalRead(IR_PIN)) {
    IR_recebido = true;            // Marca que recebeu sinal IR
    //IrReceiver.resume();           // Pronto para o próximo
  }
}
