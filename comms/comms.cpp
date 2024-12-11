#include "./comms.hpp"


Comms::Comms(uint8_t origem, RF24* radio){
    this->origem = origem;
    this->radio = radio;
}

bool Comms::sendPackage(char* package, uint8_t packageSize, uint8_t destino){
    char controle[3];
    bool enviou = false;
    bool recebeu = false;

    enviou = this->_send(&controle[0], 3, RTS, destino, TIMEOUTSEND);
    if(enviou){
        recebeu = this->_receive(&controle[0], 3, CTS, destino, TIMEOUTRECV);
    } else{#define CE_PIN 7
#define CSN_PIN 8

#define TIMEOUTRECV 1000000  //us
#define TIMEOUTSEND 6000     //us

#define ACK 1
#define RTS 2
#define CTS 3
#define DATA 4
        return false;
    }
    if(recebeu){
        enviou = this->_send(&package[0], packageSize, DATA, destino, TIMEOUTSEND);
    } else{
        return false;
    }
    if(enviou){
        recebeu = this->_receive(&controle[0], 3, ACK, destino, TIMEOUTRECV);
    } else {
        return false;
    }

    return true;
}

bool Comms::receivePackage( char* package, uint8_t packageSize, uint8_t destino){
    char controle[3];
    bool recebeu = false;
    bool enviou = false;

    recebeu = this->_receive(&controle[0], 3, RTS, destino, TIMEOUTRECV);
    if(recebeu){
        enviou = this->_send(&controle[0], 3, CTS, destino, TIMEOUTSEND);
    } else{
        return false;
    }
    if (enviou){
        recebeu = this->_receive(&package[0], packageSize, DATA, destino,TIMEOUTRECV);
    } else{
        return false;
    }
    if(recebeu){
        enviou = this->_send(&controle[0], 3, ACK, destino, TIMEOUTSEND);
    } else{
        return false;
    }
    return true;

}

bool Comms::_send(char* package, uint8_t packageSize, uint8_t controle, uint8_t destino, unsigned long timeout){
    package[0] = destino;
    package[1] = this->origem;
    package[2] = controle;
    unsigned long start_timer = micros();
    while(micros() - start_timer < timeout){
        this->radio->startListening();
        delayMicroseconds(70);
        this->radio->stopListening();
        if (!this->radio->testCarrier()){
            this->radio->write(&package[0], packageSize);
            // Serial.println("R");
            return true;
        } else{
            // Serial.println("O");
            delayMicroseconds(150);
        }
    }
    return false;
}

bool Comms::_receive(char* package, uint8_t packageSize, uint8_t controle, uint8_t destino, unsigned long timeout){
    unsigned long start_timer = micros();
    this->radio->startListening();

    while(micros() - start_timer < timeout){
        if (this->radio->available()){
            this->radio->read(&package[0], packageSize);
            if(package[0]==this->origem && package[1]==destino && package[2]==controle) return true;

            this->radio->flush_rx();
        }
    }

    return false;
}