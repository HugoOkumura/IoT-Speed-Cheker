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

uint8_t origem = 16;
uint8_t destino = 22;
char envio[BUFFER_SIZE];
char recebe[BUFFER_SIZE];

#define TRIG 14
#define ECHO 15

#define SIGNAL 0;

Ultrasonic ultrassom(TRIG, ECHO);
uint8_t buffer[BUFFER_SIZE];
uint8_t s_index = 0; //send index
uint8_t r_index = 0; //read index
uint8_t distancia = 0;
char last_state;
long leitura = 0;

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


}

void loop(void) {

  distancia = (ultrassom.Ranging(CM));
  if(distancia != last_state && (millis()- leitura) > 50){
    leitura = millis();

    if(distancia > 0 && distancia < 10){
      buffer[r_index] = distancia;
      r_index++;
      r_index = r_index % BUFFER_SIZE;
    }

    last_state = distancia;
  }

  if(s_index < r_index){
    Serial.println("enviando");
    envio[4] = buffer[s_index];
    envio[5] = SIGNAL;
    s_index++;
    comms.sendPackage(&envio[0], BUFFER_SIZE, destino);
  }
  
  delay(500);
}



