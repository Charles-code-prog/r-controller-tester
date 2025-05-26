#include <Arduino.h>

//ENTRADAS
#define S11                  35 //(BERÇO 1 RECUADO)
#define S12                  36 //(BERÇO 1 AVANÇADO)
#define S21                  37 //(BERÇO 2 RECUADO)
#define S22                  38 //(BERÇO 2 AVANÇADO)
#define S31                  39 //(PISTÃO CAMERA RECUADO)
#define S32                  40 //(PISTÃO CAMERA AVANÇADO)
#define BT1                  41 //(BOTÃO BERÇO 1)
#define BT2                  42 //(BOTÃO BERÇO 2)
#define BTN_RESET            43     
#define RELE_DE_SEGURANCA    44 

//SÁIDAS
#define MODULE_PWM           18 //(SOLENOIDE Y7 - AVANÇO PRESSORES)
#define KRST                 19 //(RELÉ RESET)
#define K1                   20 //(SOLENOIDE Y2 - AVANÇO BERÇO 1)
#define K2                   21 //(SOLENOIDE Y3 - AVANÇO BERÇO 2)
#define K3                   22 //(SOLENOIDE Y4 - AVANÇA TRAVA CONTROLE BERÇO 1)
#define K4                   23 //(SOLENOIDE Y5 - AVANÇA TRAVA CONTROLE BERÇO 2)
#define K5                   24 //(SOLENOIDE Y6 - AVANA TRAVA CAMERAS)
#define K6                   25 //(SOLENOIDE Y8 - AVANÇO PILHAS BERÇO 1)
#define K7                   45 //(SOLENOIDE Y9 - AVANÇO PILHAS BERÇO 2) Antigo pino 26
#define K8                   27 //(3V PILHAS BERÇO 1)
#define K9                   28 //(3V PILHAS BERÇO 2)
#define K10                  29 //(5V IR BERÇO 1)
#define K11                  30 //(5V IR BERÇO 2)
#define K12                  31 //(LUZ VERDE)
#define K13                  32 //(LUZ AMARELA)
#define K14                  33 //(LUZ VERMELHA)
#define RESET_ARDUINOS_NANOS 34 

bool estadoAnteriorBT1 = LOW;
bool estadoAnteriorBT2 = LOW;
int start1 = false;
int start2 = false;
bool lastS11 = HIGH;
bool lastS12 = HIGH;
bool lastS21 = HIGH;
bool lastS22 = HIGH;
bool lastS31 = HIGH;
bool lastS32 = HIGH;

String cmd = "";

void SerialRead() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') { // comando terminou (newline)
      cmd.trim(); // remove espaços e quebras extras
    } else {
      cmd += c;
    }
  }
  alternarPino(cmd);
}

void return_sensors() {
  bool currentS11 = digitalRead(S11);
  bool currentS12 = digitalRead(S12);
  bool currentS21 = digitalRead(S21);
  bool currentS22 = digitalRead(S22);
  bool currentS31 = digitalRead(S31);
  bool currentS32 = digitalRead(S32);

  if (currentS11 != lastS11 && currentS11 == LOW) Serial.println("BED1_OFF");
  if (currentS12 != lastS12 && currentS12 == LOW) Serial.println("BED1_ON");
  if (currentS21 != lastS21 && currentS21 == LOW) Serial.println("BED2_OFF");
  if (currentS22 != lastS22 && currentS22 == LOW) Serial.println("BED2_ON");
  if (currentS31 != lastS31 && currentS31 == LOW) Serial.println("PIST_OFF");
  if (currentS32 != lastS32 && currentS32 == LOW) Serial.println("PIST_ON");

  // Atualiza os estados anteriores
  lastS11 = currentS11;
  lastS12 = currentS12;
  lastS21 = currentS21;
  lastS22 = currentS22;
  lastS31 = currentS31;
  lastS32 = currentS32;
}
void alternarPino(String cmd) {
  delay(10);
  if (cmd == "MODULE_PWM") {
    bool estado = !digitalRead(MODULE_PWM);
    delay(10);
    digitalWrite(MODULE_PWM, estado);
    Serial.println(estado ? "SOLENOIDE Y7 - AVANÇO PRESSORES ON" : "SOLENOIDE Y7 - AVANÇO PRESSORES OFF");
  }
  else if (cmd == "KRST") {
    bool estado = !digitalRead(KRST);
    digitalWrite(KRST, estado);
    Serial.println(estado ? "RELÉ RESET ON" : "RELÉ RESET OFF");
  }
  else if (cmd == "K1") {
    bool estado = !digitalRead(K1);
    digitalWrite(K1, estado);
    Serial.println(estado ? "SOLENOIDE Y2 - AVANÇO BERÇO 1 ON" : "SOLENOIDE Y2 - AVANÇO BERÇO 1 OFF");
  }
  else if (cmd == "K2") {
    bool estado = !digitalRead(K2);
    digitalWrite(K2, estado);
    Serial.println(estado ? "SOLENOIDE Y3 - AVANÇO BERÇO 2 ON" : "SOLENOIDE Y3 - AVANÇO BERÇO 2 OFF");
  }
  else if (cmd == "K3") {
    bool estado = !digitalRead(K3);
    digitalWrite(K3, estado);
    Serial.println(estado ? "SOLENOIDE Y4 - TRAVA BERÇO 1 ON" : "SOLENOIDE Y4 - TRAVA BERÇO 1 OFF");
  }
  else if (cmd == "K4") {
    bool estado = !digitalRead(K4);
    digitalWrite(K4, estado);
    Serial.println(estado ? "SOLENOIDE Y5 - TRAVA BERÇO 2 ON" : "SOLENOIDE Y5 - TRAVA BERÇO 2 OFF");
  }
  else if (cmd == "K5") {
    bool estado = !digitalRead(K5);
    digitalWrite(K5, estado);
    Serial.println(estado ? "SOLENOIDE Y6 - TRAVA CÂMERAS ON" : "SOLENOIDE Y6 - TRAVA CÂMERAS OFF");
  }
  else if (cmd == "K6") {
    bool estado = !digitalRead(K6);
    digitalWrite(K6, estado);
    Serial.println(estado ? "SOLENOIDE Y8 - PILHAS BERÇO 1 ON" : "SOLENOIDE Y8 - PILHAS BERÇO 1 OFF");
  }
  else if (cmd == "K7") {
    bool estado = !digitalRead(K7);
    digitalWrite(K7, estado);
    Serial.println(estado ? "SOLENOIDE Y9 - PILHAS BERÇO 2 ON" : "SOLENOIDE Y9 - PILHAS BERÇO 2 OFF");
  }
  else if (cmd == "K8") {
    bool estado = !digitalRead(K8);
    digitalWrite(K8, estado);
    Serial.println(estado ? "3V PILHAS BERÇO 1 ON" : "3V PILHAS BERÇO 1 OFF");
  }
  else if (cmd == "K9") {
    bool estado = !digitalRead(K9);
    digitalWrite(K9, estado);
    Serial.println(estado ? "3V PILHAS BERÇO 2 ON" : "3V PILHAS BERÇO 2 OFF");
  }
  else if (cmd == "K10") {
    bool estado = !digitalRead(K10);
    digitalWrite(K10, estado);
    Serial.println(estado ? "5V IR BERÇO 1 ON" : "5V IR BERÇO 1 OFF");
  }
  else if (cmd == "K11") {
    bool estado = !digitalRead(K11);
    digitalWrite(K11, estado);
    Serial.println(estado ? "5V IR BERÇO 2 ON" : "5V IR BERÇO 2 OFF");
  }
  else if (cmd == "K12") {
    bool estado = !digitalRead(K12);
    digitalWrite(K12, estado);
    Serial.println(estado ? "LUZ VERDE ON" : "LUZ VERDE OFF");
  }
  else if (cmd == "K13") {
    bool estado = !digitalRead(K13);
    digitalWrite(K13, estado);
    Serial.println(estado ? "LUZ AMARELA ON" : "LUZ AMARELA OFF");
  }
  else if (cmd == "K14") {
    bool estado = !digitalRead(K14);
    digitalWrite(K14, estado);
    Serial.println(estado ? "LUZ VERMELHA ON" : "LUZ VERMELHA OFF");
  }
  else if (cmd == "RESET_ARDUINOS_NANOS") {
    bool estado = !digitalRead(RESET_ARDUINOS_NANOS);
    digitalWrite(RESET_ARDUINOS_NANOS, estado);
    Serial.println(estado ? "RESET NANOS ATIVO" : "RESET NANOS INATIVO");
  }
}


void btnStart(int pin) {
  int counter = 0;
  bool holding = digitalRead(pin);  
  if (holding == true && start1 == false) {
    while(holding){
      delay(100);
      counter++;
      Serial.println(counter);
      holding = digitalRead(pin);
      if(counter >= 10) break;
    }
    if (counter >= 3) {
      if (pin == BT1) Serial.print("START1");
      else Serial.print("START2");
    }
  }
}

void setup(){
  Serial.begin(115200);
  Serial.println("Ativo");
  // ENTRADAS
  for(int i = 35; i <= 40; i++){
    pinMode(i,INPUT_PULLUP);
  }
  for(int i = 41; i<45 ; i++){
    pinMode(i,INPUT);
  }
  //SAIDAS
  for(int i = 18; i<35; i++){
    pinMode(i, OUTPUT);
  }
  pinMode(45,OUTPUT);
}

void loop(){
  SerialRead();
  return_sensors();
  btnStart(BT1);
  btnStart(BT2);

  return_sensors();
  cmd = "";
}