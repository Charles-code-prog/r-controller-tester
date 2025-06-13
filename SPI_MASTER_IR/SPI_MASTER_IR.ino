#include <SPI.h>
#include <IRremote.hpp>

#define IR_PIN 7
#define SS1 2
#define SS2 3
#define SS3 4

char resposta[3];
bool IR_recebido = false;
void setup() {
  Serial.begin(115200);
  SPI.begin();
  
  //IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  pinMode(IR_PIN, INPUT);

  pinMode(SS1, OUTPUT);
  pinMode(SS2, OUTPUT);
  pinMode(SS3, OUTPUT);

  digitalWrite(SS1, HIGH);
  digitalWrite(SS2, HIGH);
  digitalWrite(SS3, HIGH);

  delay(1000);
}
void loop() {
  if (!digitalRead(IR_PIN)) {
    IR_recebido = true;
    //IrReceiver.resume();
  }
  
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '1') {
      String linha = "";

      linha = IR_recebido ? "OK;" : "0;";
      IR_recebido = false;

      linha += requestFromSlave(SS1);
      linha += requestFromSlave(SS2);
      linha += requestFromSlave(SS3);
      linha += "II;";
      Serial.println(linha);
    }
  }

}
String requestFromSlave(int ssPin) {
  digitalWrite(ssPin, LOW);
  delayMicroseconds(10);  // tempo mínimo para ativar SS
  delay()
  resposta[0] = SPI.transfer('1');    // envia comando, recebe 1º byte
  delayMicroseconds(50);              // permite o escravo preparar o próximo byte
  resposta[1] = SPI.transfer(0x00);   // recebe 2º byte

  digitalWrite(ssPin, HIGH);

  // Verifica se qualquer um dos dois bytes é '5'
  if (resposta[0] == '5' || resposta[1] == '5') return "OK;";
  else return "0;";
}
