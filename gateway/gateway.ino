#include <SPI.h>
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

#define CE_PIN 7
#define CSN_PIN 8

#define TRIG_PIN 14
#define ECHO_PIN 15

unsigned long startTime = 0; // início do cronômetro
unsigned long stopTime = 0;  // término do cronômetro
bool cronometro = false; // funciona ou n

#define TIMEOUTRECV 1000000 //us
#define TIMEOUTSEND 6000 //us

#define ACK 1
#define RTS 2
#define CTS 3
#define DATA 4


// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL};

char payloadT[5]="Hello";
char payloadR[5];
uint8_t origem[] = {50, 16, 22};

bool envia(char* pacote, uint8_t destino, uint8_t tamanho, uint8_t controle, unsigned long timeout){
  pacote[0]=22;
  pacote[1]=16;
  pacote[2]=ACK;
  unsigned long start_timer = micros();    
  while(micros()-start_timer<timeout){
    radio.startListening();
    delayMicroseconds(70);
    radio.stopListening();
    if (!radio.testCarrier()) {
      radio.write(&pacote[0], tamanho);
      Serial.println("Not Receive");
      return true;
    }else{
      Serial.println("Receive");
      delayMicroseconds(1);
    }
  }
  return false;
}
bool recebe(char* pacote, uint8_t destino, uint8_t tamanho, uint8_t controle, unsigned long timeout){
  unsigned long start_timer = micros();  
  radio.startListening();
  while(micros()-start_timer<timeout){
    if (radio.available()) { 
      radio.read(&pacote[0],tamanho);
      if(pacote[0]==origem && pacote[1]==destino && pacote[2] == controle){
        return true;
      }
      radio.flush_rx();
    }
  }
  return false;
}

bool enviaTrem(char* pacote, uint8_t tamanho, uint8_t destino){
  char controle[3];
  bool enviou=false;
  bool recebeu=false;
  
  enviou = envia(&controle[0], destino, 3, RTS, TIMEOUTSEND);
  if (enviou){
    recebeu = recebe(&controle[0], destino, 3, CTS, TIMEOUTRECV);
  }else{
    return false;
  }
  if(recebeu){
    enviou = envia(&pacote[0], destino, tamanho, DATA, TIMEOUTSEND);
  }else{
    return false;
  }
  if (enviou){
    recebeu = recebe(&controle[0], destino, 3, ACK, TIMEOUTRECV);
  }else{
    return false;
  }
  return true;
}

bool recebeTrem(char* pacote, uint8_t tamanho, uint8_t destino){
  char controle[3];
  bool recebeu = false;
  bool enviou = false;
  recebeu = recebe(&controle[0], destino, 3, RTS, TIMEOUTRECV);
  if(recebeu){
    enviou = envia(&controle[0], destino, 3, CTS, TIMEOUTSEND);
  }else{
    return false;
  }
  if (enviou){
    recebeu = recebe(&pacote[0], destino, tamanho, DATA, TIMEOUTRECV);
  }else{
    return false;
  }
  if(recebeu){
    enviou = envia(&controle[0], destino, 3, ACK, TIMEOUTSEND);
  }else{
    return false;
  }
  return true;
}

void setup(void) {

  Serial.begin(500000);

  // Setup and configure rf radio
  if (!radio.begin()) {
    Serial.println(F("radio hardware not responding!"));
    while (true) {
      // hold in an infinite loop
    }
  }
  
  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
  radio.setAutoAck(false);   // Don't acknowledge arbitrary signals
  radio.disableCRC();        // Accept any signal we find
  radio.setDataRate(RF24_1MBPS);
  
  radio.setPayloadSize(sizeof(payloadT));

  radio.openWritingPipe(address[0]);  // always uses pipe 0
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  radio.setChannel(100);
  
  printf_begin();
  radio.printPrettyDetails();
  
  radio.startListening();
  radio.stopListening();
  radio.flush_rx();

}

void loop(void) {
/*
  uint8_t destino=0;
  if (Serial.available()){
    destino=int(Serial.read());
    Serial.print("Destino = ");
    Serial.println(destino);
  }*/

  bool sucesso = recebeTrem(&payloadR[0],5,destino);
  if(sucesso){
    //printAula(&payloadR[0],5);
    if(destino == 50 && !cronometro){
      startTime = micros();
      Serial.println("Presença detectada pelo Arduino!");
      cronometro = true;
    }else if(destino == 16 && cronometro){
      stopTime = micros();
      Serial.println("Presença detectada pelo segundo Arduino!");
      cronometro = false;
      unsigned long tempo = stopTime - startTime;
      startTime = 0;
    }
    if(tempo < 10){
      Serial.println("Acima da velocidade -> ", tempo);
    }

/*
    if (strncmp(mensagem, "PRES", 4) == 0) {
      startTime = micro();
      Serial.println("Presença detectada pelo outro Arduino!");
      cronometro = true;
    }else if (strncmp(mensagem, "PRES", 4) == 0 && cronometroAtivo) {
      stopTime = micros(); 
      cronometroAtivo = false; 
      long duracao = stopTime - startTime; 
    }
*/

  }
}
