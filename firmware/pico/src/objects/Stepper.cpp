//
// Created by micha on 7/30/2026.
//

#include "Stepper.h"
#include "../payloadHandler.h"

#include <cstdio>

#include "pico/time.h"


Stepper::Stepper(uart_inst_t *uart, int directionPin, int stepPin, uint8_t address, char* name) {
    this->uart = uart;
    this->directionPin = directionPin;
    this->stepPin = stepPin;
    this->address = address;
    this->name = name;
}

int Stepper::getDirectionPin() const {
    return this->directionPin;
}
int Stepper::getStepPin() const {
    return this->stepPin;
}
uint8_t Stepper::getAddress() const {
    return this->address;
}
char *Stepper::getName() const {
    return this->name;
}

bool Stepper::configure() const {

    uint32_t defaultConfig =  0x101;
    uint32_t modified = defaultConfig & ~(1u << 0);
    modified = modified | (1u << 6);
    modified = modified | (1u << 7);

    uint32_t irun = 21; //1.21 A
    uint32_t ihold = 13;
    uint32_t iholDdelay = 4;

    uint32_t currentConfig = iholDdelay << 16;
    currentConfig = currentConfig | (irun << 8);
    currentConfig = currentConfig | (ihold << 0);

    bool test1 = writeReg(uart, address, GCONF, modified);
    bool test2 = writeReg(uart, address, IHOLD_IRUN, currentConfig);

    printf("%hhd", test1);
    printf("%hhd", test2);

    return test1 && test2;
}
