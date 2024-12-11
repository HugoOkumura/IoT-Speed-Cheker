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

#define TIMEOUTRECV 1000000  //us
#define TIMEOUTSEND 6000     //us

#define ACK 1
#define RTS 2
#define CTS 3
#define DATA 4


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
uint8_t distancia = 10;

// Comms comms(origem, &radio);

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
  
  
  radio.setPayloadSize(sizeof(recebe));

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

    // recebeu1 = comms.receivePackage(&recebe[0], 4, destino1);
    recebeu1 = receivePackage(&recebe[0], 4, 16);
    if(recebeu1){
      buffer_sensor1[s1_index] = micros();
      Serial.println("recebeu 1");
      send_to_serial(&recebe[0], 4);
      // Serial.println(recebe[3]);
      Serial.println();
      s1_index++;
    }

    // recebeu2 = comms.receivePackage(&recebe[0], 4, destino2);
    recebeu2 = receivePackage(&recebe[0], 4, destino2);
    if(recebeu2){
      buffer_sensor2[s2_index]  = micros();
      s2_index++;
    }

    // Serial.println("fim 2");
    // delay(500);
    // if(s1_index == s2_index && buffer_sensor1[0] != -1 && buffer_sensor2[0] != -1){
    //   char* payload[4];
    //   payload[0] = buffer_sensor2[s2_index] - buffer_sensor1[s1_index];  // tempo
    //   payload[1] = distancia;                                           // distancia
    //   payload[2] = distancia / d_tempo;                                // velocidade
    //   bool limite = false;
    //   if(payload[2] > 10){
    //     limite = false
    //   }
    //   payload[3] = limite
    //   send_to_serial(&payload[0], 4);
    //   // Serial.print(origem);
    //   // Serial.print(velocidade);

    //   Serial.println();

    // }

}

bool sendPackage(char* package, uint8_t packageSize, uint8_t destino){
    char controle[3];
    uint8_t ctrl_size = 3;
    bool enviou = false;
    bool recebeu = false;

    enviou = _send(&controle[0], ctrl_size, RTS, destino, TIMEOUTSEND);
    if(enviou){
        recebeu = _receive(&controle[0], ctrl_size, CTS, destino, TIMEOUTRECV);
    } else{
        return false;
    }
    if(recebeu){
        enviou = _send(&package[0], packageSize, DATA, destino, TIMEOUTSEND);
    } else{
        return false;
    }
    if(enviou){
        recebeu = _receive(&controle[0], ctrl_size, ACK, destino, TIMEOUTRECV);
    } else {
        return false;
    }

    return true;
}

bool _send(char* package, uint8_t packageSize, uint8_t controle, uint8_t destino, unsigned long timeout){
    package[0] = destino;
    package[1] = origem;
    package[2] = controle;
    unsigned long start_timer = micros();
    while(micros() - start_timer < timeout){
        radio.startListening();
        delayMicroseconds(70);
        radio.stopListening();
        if (!radio.testCarrier()){
            radio.write(&package[0], packageSize);
            // Serial.println("R");
            return true;
        } else{
            // Serial.println("O");
            delayMicroseconds(150);
        }
    }
    return false;
}

bool receivePackage( char* package, uint8_t packageSize, uint8_t destino){
    char controle[3];
    uint8_t ctrl_size = 3;
    bool recebeu = false;
    bool enviou = false;

    recebeu = _receive(&controle[0], ctrl_size, RTS, destino, TIMEOUTRECV);
    if(recebeu){
        enviou = _send(&controle[0], ctrl_size, CTS, destino, TIMEOUTSEND);
    } else{
        return false;
    }
    if (enviou){
        recebeu = _receive(&package[0], packageSize, DATA, destino,TIMEOUTRECV);
    } else{
        return false;
    }
    if(recebeu){
        enviou = _send(&controle[0], ctrl_size, ACK, destino, TIMEOUTSEND);
    } else{
        return false;
    }
    return true;

}

bool _receive(char* package, uint8_t packageSize, uint8_t controle, uint8_t destino, unsigned long timeout){
    unsigned long start_timer = micros();
    radio.startListening();

    while(micros() - start_timer < timeout){
        if (radio.available()){
            radio.read(&package[0], packageSize);
            if(package[0]==origem && package[1]==destino && package[2]==controle){
            // Serial.println(package[2]);
            return true;
            } 
            radio.flush_rx();
        }
    }

    return false;
}

void send_to_serial(char* payload, uint8_t size){
  for(int i = 0; i < size; i++){
    Serial.print(payload[i]);
  }
}