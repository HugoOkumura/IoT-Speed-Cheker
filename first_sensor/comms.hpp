#include <SPI.h>
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8

#define TIMEOUTRECV 1000000  //us
#define TIMEOUTSEND 6000     //us

#define ACK 1
#define RTS 2
#define CTS 3
#define DATA 4

class Comms{
    public:
    /*
    Construtor:
    uint8_t origem: número identificador da placa
    RF24 radio: objeto para a comunicação sem fio
    */
    Comms(uint8_t origem, RF24* radio);
    bool sendPackage(char* package, uint8_t packageSize, uint8_t destino);
    bool receivePackage( char* package, uint8_t packageSize, uint8_t destino);

    private:
    uint8_t origem;
    RF24* radio;
    bool _send(char* package, uint8_t packageSize, uint8_t controle, uint8_t destino, unsigned long timeout);
    bool _receive(char* package, uint8_t packageSize, uint8_t controle, uint8_t destino, unsigned long timeout);
};

