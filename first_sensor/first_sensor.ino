// #include <RF24_config.h>
// #include <nRF24L01.h>
// #include <printf.h>
// #include "comms.hpp"

#include <Ultrasonic.h>
#include <SPI.h>
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// instantiate an object for the nRF24L01 transceiver

#define CE_PIN 7
#define CSN_PIN 8

#define TIMEOUTRECV 2000000  //us
#define TIMEOUTSEND 5000000   //us

#define ACK 1
#define RTS 2
#define CTS 3
#define DATA 4

RF24 radio(CE_PIN, CSN_PIN);
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL};

#define BUFFER_SIZE 10

uint8_t origem = 50; //sensor1: 16 - sensor2: 50
uint8_t destino = 22;
uint8_t envio[4];
// char recebe[4];

#define TRIG 14
#define ECHO 15

#define SIGNAL 0;

Ultrasonic ultrassom(TRIG, ECHO);
uint8_t buffer_distancia[BUFFER_SIZE];
uint8_t buffer_tempo[BUFFER_SIZE];
uint8_t s_index = 0; //send index
uint8_t r_index = 0; //read index
uint8_t distancia = 0;
uint8_t last_state;
long leitura = 0;

// Comms comms(origem, &radio);

void setup(void) {
  Serial.begin(115200);
  
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
  
  
  radio.setPayloadSize(sizeof(envio));

  radio.openWritingPipe(address[0]);  // always uses pipe 0
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  radio.setChannel(37);
  
  printf_begin();
  radio.printPrettyDetails();
  
  radio.startListening();
  radio.stopListening();
  radio.flush_rx();

  for(int i = 0; i < BUFFER_SIZE; i++){
    buffer_distancia[i] = -1;

  }
}

void loop(void) {

  distancia = (ultrassom.Ranging(CM));
  // Serial.println(distancia);
  if(distancia != last_state && (millis()- leitura) > 50){
    leitura = millis();

    if(distancia > 0 && distancia < 10){
      int i = r_index % BUFFER_SIZE;
      buffer_distancia[i] = distancia;
      // Serial.println(i);
      r_index++;
    }

    last_state = distancia;
  }

  if(s_index < r_index && buffer_distancia[s_index] !=-1){
     Serial.println("enviando");
    bool enviou = false;
    int j = s_index % BUFFER_SIZE;
    Serial.println(buffer_distancia[j]);
    envio[3] = buffer_distancia[j];

    enviou = sendPackage(&envio[0], 4, destino);
    if(enviou){
      s_index++;
      Serial.println("enviou");
    } 
  }
  
}

bool sendPackage(uint8_t* package, uint8_t packageSize, uint8_t destino){
    uint8_t controle[3];
    uint8_t ctrl_size = 3;
    bool enviou = false;
    bool recebeu = false;

    enviou = send(&controle[0], ctrl_size, RTS, destino, TIMEOUTSEND);
    if(enviou){
        Serial.println("RTS");
        recebeu = receive(&controle[0], ctrl_size, CTS, destino, TIMEOUTRECV);
    } else{
        return false;
    }
    if(recebeu){
        Serial.println("CTS");
      enviou = send(&package[0], packageSize, DATA, destino, TIMEOUTSEND);
    } else{
        return false;
    }
    if(enviou){
        recebeu = receive(&controle[0], ctrl_size, ACK, destino, TIMEOUTRECV);
    } else {
       return false;
    }

    return true;
}

bool send(uint8_t* package, uint8_t packageSize, uint8_t controle, uint8_t destino, unsigned long timeout) {
    package[0] = origem;  // <-- Inverter se necessário
    package[1] = destino; // <-- Inverter se necessário
    package[2] = controle;

    unsigned long start_timer = micros();
    while (micros() - start_timer < timeout) {
        radio.startListening();
        delayMicroseconds(70);
        radio.stopListening();
        if (!radio.testCarrier()) {
            radio.write(&package[0], packageSize);
            Serial.println("v");
            return true;
        } else {
            delayMicroseconds(100);
        }
    }
    return false;
}

bool receive(uint8_t* package, uint8_t packageSize, uint8_t controle, uint8_t destino, unsigned long timeout){
    unsigned long start_timer = micros();
    radio.startListening();

    while(micros() - start_timer < timeout){
        if (radio.available()){
            radio.read(&package[0], packageSize);
            if(package[0]==origem && package[1]==destino && package[2]==controle){
              return true;
            }
            radio.flush_rx();
        }
    }



    return false;
} 