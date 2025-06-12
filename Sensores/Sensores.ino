#include <Arduino.h>

//ENTRADAS
#define S11                  35 //(BERÇO 2 RECUADO)
#define S12                  36 //(BERÇO 2 AVANÇADO)
#define S21                  37 //(BERÇO 1 RECUADO)
#define S22                  38 //(BERÇO 1 AVANÇADO)
#define S31                  39 //(PISTÃO CAMERA RECUADO)
#define S32                  40 //(PISTÃO CAMERA AVANÇADO)
#define BT2                  41 //(BOTÃO BERÇO 2)
#define BT1                  42 //(BOTÃO BERÇO 1)
#define BTN_RESET            43 //(BOTÃO RST)    
#define RELE_DE_SEGURANCA    44 

//SÁIDAS
#define PRESS                18 //(Y7 - AVANÇO PRESSORES)
#define KRST                 19 //(RELÉ RESET)
#define K1                   20 //(Y2 - AV_B 2)
#define K2                   21 //(Y3 - AV_B 1)
#define K3                   22 //(Y4 - AVANÇA TRAVA CONTROLE BERÇO 2)
#define K4                   23 //(Y5 - AVANÇA TRAVA CONTROLE BERÇO 1)
#define K5                   24 //(Y6 - AVANA TRAVA CAMERAS)
#define K6                   25 //(Y8 - AVANÇO PILHAS BERÇO 2)
#define K7                   45 //(Y9 - AVANÇO PILHAS BERÇO 1) Antigo pino 26
#define K8                   27 //(3V PILHAS BERÇO 2)
#define K9                   28 //(3V PILHAS BERÇO 1)
#define K10                  29 //(5V IR BERÇO 1)
#define K11                  30 //(5V IR BERÇO 2)
#define K12                  31 //(GREEN LED)
#define K13                  32 //(YELLOW LED)
#define K14                  33 //(RED LED)
#define RST_NANO             34 //(RESET NANOS)

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
void SerialRead();
void alternarPino(String cmd);
void resetStarts();
void return_sensors();
void btnRST();

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

  if (currentS11 != lastS11 && currentS11 == LOW) Serial.println("BED2_OFF;");
  if (currentS12 != lastS12 && currentS12 == LOW) Serial.println("BED2_ON;");
  if (currentS21 != lastS21 && currentS21 == LOW) Serial.println("BED1_OFF;");
  if (currentS22 != lastS22 && currentS22 == LOW) Serial.println("BED1_ON;");
  if (currentS31 != lastS31 && currentS31 == LOW) Serial.println("PIST_OFF;");
  if (currentS32 != lastS32 && currentS32 == LOW) Serial.println("PIST_ON;");

  // Atualiza os estados anteriores
  lastS11 = currentS11;
  lastS12 = currentS12;
  lastS21 = currentS21;
  lastS22 = currentS22;
  lastS31 = currentS31;
  lastS32 = currentS32;
}

struct ComandoPino {
  String nome;
  uint8_t pino;
  const char* msgOn;
  const char* msgOff;
};

// Lista de todos os comandos
ComandoPino comandos[] = {
  {"P",        PRESS,      "PRE_ON;"     , "PRE_OFF;"}, //(Y7 -  Pistão de Precionar Tecla)
  {"KR",       KRST,       "RST_ON;"     , "RST_OFF;"}, //(RELÉ RESET)
  {"K1",       K1,         "AV2_ON;"     , "AV2_OFF;"}, //(Y2 - Avanço berço 2)
  {"K2",       K2,         "AV1_ON;"     , "AV1_OFF;"}, //(Y3 - Avanço berço 1)
  {"K3",       K3,         "TR2_ON;"     , "TR2_OFF;"}, //(Y4 - Trava berço 2)
  {"K4",       K4,         "TR1_ON;"     , "TR1_OFF;"}, //(Y5 - Trava berço 1)
  {"K5",       K5,         "TRC_ON;"     , "TRC_OFF;"}, //(Y6 - Trava Câmera)
  {"K6",       K6,         "BT2_ON;"     , "BT2_OFF;"}, //(Y8 - Aciona pilhas 2)
  {"K7",       K7,         "BT1_ON;"     , "BT1_OFF;"}, //(Y9 - Aciona pilhas 1)
  {"K8",       K8,         "3V1_ON;"     , "3V1_OFF;"}, //(3V Ligar berço 2)
  {"K9",       K9,         "3V2_ON;"     , "3V2_OFF;"}, //(3V Ligar berço 1)
  {"K10",      K10,        "IR1_ON;"     , "IR1_OFF;"}, //(5V IR Ligar 1)
  {"K11",      K11,        "IR2_ON;"     , "IR2_OFF;"}, //(5V IR Ligar 2)
  {"K12",      K12,        "GRE_ON;"     , "GRE_OFF;"}, //(GREEN LED)
  {"K13",      K13,        "YEL_ON;"     , "YEL_OFF;"}, //(YELLOW LED)
  {"K14",      K14,        "RED_ON;"     , "RED_OFF;"}, //(RED LED)
  {"RNA", RST_NANO,        "RNA_ON;"     , "RNA_OFF;"}  //(RESET NANOS)
};

const int numComandos = sizeof(comandos) / sizeof(comandos[0]);

void alternarPino(String cmd) {
  delay(10);

  for (int i = 0; i < numComandos; i++) {
    if (cmd == comandos[i].nome + "_1") {
      Serial.println(comandos[i].msgOn);
      digitalWrite(comandos[i].pino, HIGH);
      return;
    }
    if (cmd == comandos[i].nome + "_0") {
      Serial.println(comandos[i].msgOff);
      digitalWrite(comandos[i].pino, LOW);
      return;
    }
  }
}


void btnRST(){
  if(digitalRead(BTN_RESET) == LOW){
    Serial.println("BTN RST");
    digitalWrite(KRST,HIGH);
    delay(200);
    digitalWrite(KRST,LOW);
    delay(300);
  }
}

void btnStart(int pin) {
  int counter = 0;
  bool holding = digitalRead(pin);  
  if (holding == false && start1 == false && start2 == false) {
    while(!holding){
      delay(100);
      counter++;
      //Serial.println(counter);
      holding = digitalRead(pin);
      if (counter == 5){
        if (pin == BT1){
          Serial.println("START1");
          start1 = true;
        }
        else { 
          Serial.println("START2");
          start2 = true;
        }
        break;
      }
    }
  holding = true;
  }
}
void resetStarts(){
  start1 = false; start2 = false;
}
void setup(){
  Serial.begin(115200);
  Serial.println("Ativo");
  // ENTRADAS
  for(int i = 35; i <= 40; i++){
    pinMode(i,INPUT_PULLUP);
  }
  for(int i = 41; i<45 ; i++){
    pinMode(i,INPUT_PULLUP);
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
  btnRST();
  return_sensors();
  cmd = "";
}