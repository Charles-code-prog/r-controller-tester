#include <Arduino.h>
#include <EEPROM.h>

// Endereço da EEPROM onde o valor será salvo
const int pressTimeAddress = 0;

//ENTRADAS
#define S11 35        //(BERÇO 1 RECUADO)
#define S12 36        //(BERÇO 1 AVANÇADO)
#define S21 37        //(BERÇO 2 RECUADO)
#define S22 38        //(BERÇO 2 AVANÇADO)
#define S31 39        //(PISTÃO CAMERA RECUADO)
#define S32 40        //(PISTÃO CAMERA AVANÇADO)
#define BT1 41        //(BOTÃO BERÇO 1)
#define BT2 42        //(BOTÃO BERÇO 2)
#define BTN_RESET 43  //(BOTÃO RST)
#define KSEC 44       //(RELÉ DE SEGURANÇA)

//SÁIDAS
#define PRESS 18      //(Y7 - AVANÇO PRESSORES)
#define KRST 19       //(RELÉ RESET)
#define K1 20         //(Y2 - AV_B 2)
#define K2 21         //(Y3 - AV_B 1)
#define K3 22         //(Y4 - AVANÇA TRAVA CONTROLE BERÇO 2)
#define K4 23         //(Y5 - AVANÇA TRAVA CONTROLE BERÇO 1)
#define K5 24         //(Y6 - AVANA TRAVA CAMERAS)
#define K6 25         //(Y8 - AVANÇO PILHAS BERÇO 2)
#define K7 45         //(Y9 - AVANÇO PILHAS BERÇO 1) Antigo pino 26
#define K8 27         //(3V PILHAS BERÇO 2)
#define K9 28         //(3V PILHAS BERÇO 1)
#define K10 29        //(5V IR BERÇO 1)
#define K11 30        //(5V IR BERÇO 2)
#define K12 31        //(GREEN LED)
#define K13 32        //(YELLOW LED)
#define K14 33        //(RED LED)
#define RNA 34        //(RESET NANOS)


// Defina os arrays de pinos
const int   B1_ON[]  = { K4,  K7, K8, K10};
const int   B2_ON[]  = { K3,  K6, K9, K11};
const int  B1_OFF[]  = {K5, K10, K8, K7, K4, K2};
const int  B2_OFF[]  = {K5, K11, K9, K6, K3, K1};
const int SEC_OFF[]  = { K3, K4, K5, K6, K7, K8, K9, K10, K11};
const int all_sen[]  = { S11, S12, S21, S22, S31, S32, KSEC};

bool EMERG_state = false;
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
void autoAction(String cmd);
void pressTime();
int timeReturn();
void secure();
void lerEntradas();

int Pressionar = timeReturn(); // Salva o tempo de precionamento dos botões
void SerialRead() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {  // comando terminou (newline)
      cmd.trim();     // remove espaços e quebras extras
    } else {
      cmd += c;
    }
  }
}

void return_sensors() { // Retorno dos sensores
  bool currentS11 = digitalRead(S11);
  bool currentS12 = digitalRead(S12);
  bool currentS21 = digitalRead(S21);
  bool currentS22 = digitalRead(S22);
  bool currentS31 = digitalRead(S31);
  bool currentS32 = digitalRead(S32);

  if (currentS11 != lastS11 && currentS11 == LOW) Serial.println("BED1_OFF;");
  if (currentS12 != lastS12 && currentS12 == LOW) Serial.println("BED1_ON;");
  if (currentS21 != lastS21 && currentS21 == LOW) Serial.println("BED2_OFF;");
  if (currentS22 != lastS22 && currentS22 == LOW) Serial.println("BED2_ON;");
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

void lerEntradas() { // Checa todos os sensores
  Serial.println("SENSORS STATES:");
  for (int i = 0; i < sizeof(all_sen) / sizeof(int); i++) {
    int estado = digitalRead(all_sen[i]);
    Serial.print("Pino ");
    Serial.print(all_sen[i]);
    Serial.print(" = ");
    Serial.println(estado);
  }
}

struct ComandoPino {
  String nome;
  uint8_t pino;
  const char* msgOn;
  const char* msgOff;
};

// Lista de todos os comandos de acionamento dos Relés
ComandoPino comandos[] = {
  { "P", PRESS, "PRE_ON;", "PRE_OFF;" },  //(Pistão de Precionar Tecla)
  { "KR", KRST, "RST_ON;", "RST_OFF;" },  //(RELÉ RESET)
  { "K1",   K1, "AV2_ON;", "AV2_OFF;" },  //(Avanço berço 2)
  { "K2",   K2, "AV1_ON;", "AV1_OFF;" },  //(Avanço berço 1)
  { "K3",   K3, "TR2_ON;", "TR2_OFF;" },  //(Trava berço 2)
  { "K4",   K4, "TR1_ON;", "TR1_OFF;" },  //(Trava berço 1)
  { "K5",   K5, "TRC_ON;", "TRC_OFF;" },  //(Trava Câmera)
  { "K6",   K6, "BT2_ON;", "BT2_OFF;" },  //(Aciona pilhas 2)
  { "K7",   K7, "BT1_ON;", "BT1_OFF;" },  //(Aciona pilhas 1)
  { "K8",   K8, "3V2_ON;", "3V2_OFF;" },  //(3V Ligar berço 2)
  { "K9",   K9, "3V1_ON;", "3V1_OFF;" },  //(3V Ligar berço 1)
  { "K10", K10, "IR1_ON;", "IR1_OFF;" },  //(5V Ligar IR  1)
  { "K11", K11, "IR2_ON;", "IR2_OFF;" },  //(5V Ligar IR  2)
  { "K12", K12, "GRE_ON;", "GRE_OFF;" },  //(GREEN LED)
  { "K13", K13, "YEL_ON;", "YEL_OFF;" },  //(YELLOW LED)
  { "K14", K14, "RED_ON;", "RED_OFF;" },  //(RED LED)
  { "RNA", RNA, "RNA_ON;", "RNA_OFF;" }   //(RESET NANOS)
};

const int numComandos = sizeof(comandos) / sizeof(comandos[0]);

void alternarPino(String cmd) { // Função para acionamento dos Relés
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
  if (cmd == "P_2") { // Pressionar botão
    digitalWrite(PRESS, HIGH);
    delay(Pressionar);
    digitalWrite(PRESS, LOW);
    Serial.println("PRESS;");
  }
}


void pressTime(String cmd) { // Salva valor na EEPROM
  if (cmd.startsWith("PT_")) {
    // Extrai o valor após "PT_"
    int valor = cmd.substring(3).toInt();

    // Salva o valor na EEPROM (como int = 2 bytes)
    EEPROM.put(pressTimeAddress, valor);

    Serial.print("Tempo salvo: ");
    Serial.println(valor);
    Pressionar = timeReturn();
  }
}

int timeReturn() { // Retorna o valor salvo na eeprom
  int valorSalvo;
  EEPROM.get(pressTimeAddress, valorSalvo);
  return valorSalvo;
}


void btnRST() { // Botão de Reset
  if (digitalRead(BTN_RESET) == LOW) {
    Serial.println("BTN_RST;");
    digitalWrite(KRST, HIGH);
    delay(300);
    digitalWrite(KRST, LOW);
    secure();
    EMERG_state = false;
  }
}

void secure(){ // Rotina de segurança para evitar acidentes nos berços
  // Evita que o berço bata
  if(digitalRead(S11))  digitalWrite(K2, HIGH); // Berço 1
  if(digitalRead(S21))  digitalWrite(K1, HIGH); // Berço 2 
  delay(1000);
  digitalWrite(K1, LOW);
  digitalWrite(K2, LOW);

  // Desativar todas as travas e alimentações
  acionarPinos(SEC_OFF, sizeof(SEC_OFF) / sizeof(int), LOW);
  Serial.println("ALL_OFF;");
}

void btnStart(int pin) { // Botões de START 1&2
  int counter = 0;
  bool holding = digitalRead(pin);
  if (holding == false && start1 == false && start2 == false) {
    while (!holding) {
      delay(100);
      counter++;
      //Serial.println(counter);
      holding = digitalRead(pin);
      if (counter == 5) {
        if (pin == BT1) {
          Serial.println("START1;");
          start1 = true;
        } else {
          Serial.println("START2;");
          start2 = true;
        }
        break;
      }
    }
    holding = true;
  }
}

void resetStarts(){ //Permite um novo acionamento dos botões
  start1 = false;
  start2 = false;
  Serial.println("ENA_STARTS;");
}

// Função para acionar os pinos com delay
void acionarPinos(const int pinos[], int tamanho, int estado) {
  for (int i = 0; i < tamanho; i++) {
    digitalWrite(pinos[i], estado);
    delay(100);
  }
}


void autoAction(String cmd) { // Função que trava, liga 3v e 5v e aciona pilhas
  if (cmd == "B1_1") {
    Serial.println("B1_ON;");
    acionarPinos(B1_ON, sizeof(B1_ON) / sizeof(int), HIGH);
  } 
  if (cmd == "B2_1") {
    Serial.println("B2_ON;");
    acionarPinos(B2_ON, sizeof(B2_ON) / sizeof(int), HIGH);
  } 
  if (cmd == "B1_0") {
    Serial.println("B1_OFF;");
    acionarPinos(B1_OFF, sizeof(B1_OFF) / sizeof(int), LOW);
    resetStarts();
  }
  if (cmd == "B2_0") {
    Serial.println("B2_OFF;");
    acionarPinos(B2_OFF, sizeof(B2_OFF) / sizeof(int), LOW);
    resetStarts();
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Ativo");
  // ENTRADAS
  for (int i = 35; i <= 44; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  //SAIDAS
  for (int i = 18; i < 35; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(45, OUTPUT);
}

void loop() {
  SerialRead();
  alternarPino(cmd);
  autoAction(cmd);
  return_sensors();
  btnStart(BT1);
  btnStart(BT2);
  return_sensors();
  pressTime(cmd);
  if(digitalRead(KSEC) && !EMERG_state){
    Serial.println("EMERG;");
    EMERG_state = true;
  }
  if(EMERG_state){
    btnRST();
  }
  if(cmd == "ENA"){ 
    resetStarts();
  }
  
  if(cmd=="CH"){
    lerEntradas();
  }
  cmd = "";
}