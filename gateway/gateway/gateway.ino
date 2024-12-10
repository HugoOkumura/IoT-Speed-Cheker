// #include <RF24_config.h>
// #include <nRF24L01.h>
// #include <printf.h>
#include "comms.hpp"

#include <Ultrasonic.h>
// #include <SPI.h>
// #include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL};

#define BUFFER_SIZE 5

uint8_t origem = 22;
uint8_t destino1 = 16;
uint8_t destino2 = 50;

// char envio[4];
char recebe[4];

#define TRIG 14
#define ECHO 15

#define SIGNAL 0;

Ultrasonic ultrassom(TRIG, ECHO);
long int buffer_sensor1[BUFFER_SIZE];
long int buffer_sensor2[BUFFER_SIZE];
uint8_t s1_index = 0; //sensor1 index
uint8_t s2_index = 0; //sensor2 index
uint8_t distancia1 = 0;



Comms comms(origem, &radio);

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
  
  
  radio.setPayloadSize(sizeof(envio));

  radio.openWritingPipe(address[0]);  // always uses pipe 0
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  radio.setChannel(100);
  
  printf_begin();
  radio.printPrettyDetails();
  
  radio.startListening();
  radio.stopListening();
  radio.flush_rx();

  for(int i = 0; i < BUFFER_SIZE; i++){
    buffer_sensor1[i] = -1;
    buffer_sensor2[i] = -1;
  }

}

void loop(void) {
    bool recebeu1 = false;
    bool recebeu2 = false;

    recebeu1 = comms.receivePackage(&recebe[0], 4, destino1);
    if(recebeu1){
      buffer_sensor1[s1_index] = micros();
      s1_index++;
    }

    recebeu2 = comms.receivePackage(&recebe[0], 4, destino2);
    if(recebeu2){
      buffer_sensor2[s2_index]  = micros();
      s2_index++;
    }

    if(s1_index == s2_index && buffer_sensor1[0] != -1 && buffer_sensor2[0] != -1){
      long int d_tempo = buffer_sensor2[s2_index] - buffer_sensor1[s1_index];
      long velocidade = distancia1 / d_tempo;
      Serial.print(origem);
      Serial.print(velocidade);
      Serial.prinln();
    }

}



