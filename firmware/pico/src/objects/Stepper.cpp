//
// Created by micha on 7/30/2026.
//

#include "Stepper.h"

#include <complex>

#include "../payloadHandler.h"

#include <cstdio>

#include "pico/time.h"


Stepper::Stepper(uart_inst_t *uart, int directionPin, int stepPin, uint8_t address, const char* name) {
    this->uart = uart;
    this->directionPin = directionPin;
    this->stepPin = stepPin;
    this->address = address;
    this->name = name;
    this->direction = false;
    this->stepRes = 0x0;
    this->gconf = 0x00000101;
    this->ihold_irun = 0x00001F00;
    this->chopconf = 0x10000053;
}

bool Stepper::getDirection() const {
    return this->direction;
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
const char *Stepper::getName() const {
    return this->name;
}

int Stepper::getStepRes() const {
    //if its 0x0 we know that means 256
    const unsigned int raw = this->stepRes;
    return 1 << (8-raw);
}

bool Stepper::setStepRes(int microsteps) {
    uint8_t mres = static_cast<uint8_t>(8 - round(log2(microsteps))) & 0xFu;
    uint32_t newChopconf = (this->chopconf & ~(0xFu << 24)) | (static_cast<uint32_t>(mres) << 24);

    bool ok = writeReg(uart, address, CHOPCONF, newChopconf);
    if (!ok) {
        printf("%s: CHOPCONF write not accepted (setStepRes)\n", name);
        return false;
    }

    this->stepRes = mres;
    this->chopconf = newChopconf;

    return true;
}


void Stepper::setDirection(bool direction) {
    this->direction = direction;
    gpio_put(directionPin, direction);
}

void Stepper::initGPIO() const {
    gpio_init(directionPin);
    gpio_init(stepPin);
    gpio_set_dir(directionPin, GPIO_OUT);
    gpio_set_dir(stepPin, GPIO_OUT);
    gpio_put(directionPin, direction);
    gpio_put(stepPin, false);
}

bool Stepper::configure() {

    initGPIO();

    uint32_t gconfModified = this->gconf & ~(1u << 0);
    gconfModified = gconfModified | (1u << 2); // en_spreadcycle: force SpreadCycle instead of StealthChop for bring-up testing
    gconfModified = gconfModified | (1u << 6);
    gconfModified = gconfModified | (1u << 7);

    uint32_t irun = 21; //1.21 A
    uint32_t ihold = 13;
    uint32_t iholDdelay = 4;

    uint32_t currentConfig = iholDdelay << 16;
    currentConfig = currentConfig | (irun << 8);
    currentConfig = currentConfig | (ihold << 0);

    // MRES=8 (bits 24-27) selects full-step mode (no microstepping) for visible bring-up testing.
    // TOFF=3 (bits 0-3) keeps the chopper enabled, matching the chip's reset default.

    //0x10000053 DEFAULT
    //0000000
    //1100 | 101   | 0000 |  0000 | 00 | 0 | 000000 | 0000 |    0   |   0    |    0   |    1    |
    //TOFF | HSTRT | HEND |       |TBL |VS |        | MRES | intpol | dedge  | diss2g | diss2vs |
    uint32_t chopConf = this->chopconf | (this->stepRes << 24);

    bool gconfOk = writeReg(uart, address, GCONF, gconfModified);
    bool currentOk = writeReg(uart, address, IHOLD_IRUN, currentConfig);
    bool chopConfOk = writeReg(uart, address, CHOPCONF, chopConf);

    if (gconfOk == false) {
        printf("%s: GCONF write not accepted\n", name);
    }
    this->gconf = gconfModified;
    if (currentOk == false) {
        printf("%s: IHOLD_IRUN write not accepted\n", name);
    }
    this->ihold_irun = currentConfig;
    if (chopConfOk == false) {
        printf("%s: CHOPCONF write not accepted\n", name);
    }
    this->chopconf = chopConf;

    return gconfOk && currentOk && chopConfOk;
}

void Stepper::step() const {
    gpio_put(stepPin, true);
    sleep_us(STEP_PULSE_US);
    gpio_put(stepPin, false);
}

void Stepper::step(int count) const {
    for (int i = 0; i < count; i++) {
        step();
    }
}
