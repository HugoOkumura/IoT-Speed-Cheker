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

uint8_t origem = 22;
uint8_t destino1 = 16;
uint8_t destino2 = 50;

// char envio[4];
uint8_t recebe[4];

#define TRIG 14
#define ECHO 15

#define SIGNAL 0;

Ultrasonic ultrassom(TRIG, ECHO);
long int buffer_sensor1[BUFFER_SIZE];
long int buffer_sensor2[BUFFER_SIZE];
uint8_t s1_index = 0; //sensor1 index
uint8_t s2_index = 0; //sensor2 index
uint8_t send_index = 0;
uint8_t distancia = 100;

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
  radio.setDataRate(RF24_2MBPS);
  
  
  radio.setPayloadSize(sizeof(recebe));

  radio.openWritingPipe(address[0]);  // always uses pipe 0
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  radio.setChannel(37);
  
  printf_begin();
  // radio.printPrettyDetails();
  
  radio.startListening();
  radio.stopListening();
  radio.flush_rx();

}

void loop(void) {
    //bool recebeu1 = false;
    //bool recebeu2 = false;

    bool recebeu1 = receivePackage(&recebe[0], 4, destino1);
    if(recebeu1){
      s1_index++;
      int i = s1_index % BUFFER_SIZE;
      buffer_sensor1[i] = micros();
    }

    bool recebeu2 = receivePackage(&recebe[0], 4, destino2);
    if(recebeu2){
      s2_index++;
      int j = s2_index % BUFFER_SIZE;
      buffer_sensor2[s2_index]  = micros();
    }

    if(send_index < s1_index && send_index < s2_index){
      uint8_t payload[4];
      int k = send_index % BUFFER_SIZE
      payload[0] = (buffer_sensor2[k] - buffer_sensor1[k]) / 1000 ;     // tempo
      payload[1] = distancia;                                           // distancia
      payload[2] = distancia / payload[0];                              // velocidade
      
      bool limite = false;
      if(payload[2] > 60){
        limite = true;
      }

      payload[3] = limite;
      send_to_serial(&payload[0], 4);
      Serial.println();
      // Serial.print(origem);
      // Serial.print(velocidade);
    }

}

void send_to_serial(uint8_t* buff, uint8_t buffsize){
  for(int i = 0; i < buffsize; i++){
    Serial.print(buff[i]);
    Serial.print(" ");
  }
}

bool receivePackage( uint8_t* package, uint8_t packageSize, uint8_t destino){
    uint8_t controle[3];
    uint8_t ctrl_size = 3;
    bool enviou = false;
    bool recebeu = false;

    recebeu = receive(&controle[0], ctrl_size, RTS, destino, TIMEOUTRECV);
    if(recebeu){
        // Serial.println("RTS");
        enviou = send(&controle[0], ctrl_size, CTS, destino, TIMEOUTSEND);
    } else{
        return false;
    }
    if(enviou){
        // Serial.println("CTS");
      recebeu = send(&package[0], packageSize, DATA, destino, TIMEOUTRECV);
    } else{
        return false;
    }
    if(recebeu){
        enviou = receive(&controle[0], ctrl_size, ACK, destino, TIMEOUTSEND);
    } else {
       return false;
    }

    return true;
}

bool receive(uint8_t* package, uint8_t packageSize, uint8_t controle, uint8_t destino, unsigned long timeout){
    unsigned long start_timer = micros();
    radio.startListening();
    delayMicroseconds(100);
    while(micros() - start_timer < timeout){
        if (radio.available()){
            radio.read(&package[0], packageSize);
            if(package[0]==origem && package[1]==destino && package[2]==controle){
              // Serial.println("OK");
              radio.stopListening();
              radio.flush_rx();
              return true;
            }
            radio.flush_rx();
        }
    }

    return false;
} 

bool send(uint8_t* package, uint8_t packageSize, uint8_t controle, uint8_t destino, unsigned long timeout) {
    package[0] = destino;  // <-- Inverter se necessário
    package[1] = origem; // <-- Inverter se necessário
    package[2] = controle;
    unsigned long start_timer = micros();
    while (micros() - start_timer < timeout) {
        radio.startListening();
        delayMicroseconds(70);
        radio.stopListening();
        if (!radio.testCarrier()) {
            radio.write(&package[0], packageSize);
            return true;
        } else {
            delayMicroseconds(150);
        }
    }
    return false;
}