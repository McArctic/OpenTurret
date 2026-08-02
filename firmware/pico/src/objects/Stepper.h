#ifndef OPENTURRET_STEPPER_H
#define OPENTURRET_STEPPER_H

#include <cstdint>

#include "hardware/uart.h"

#ifndef STEP_PULSE_US
#define STEP_PULSE_US 1000
#endif

class Stepper {
private:
    int directionPin;
    int stepPin;
    uint8_t address;
    uart_inst_t *uart;
    const char* name;
    bool direction; //True is reversed
    uint8_t stepRes; //Can go to 256
    uint32_t gconf;
    uint32_t ihold_irun;
    uint32_t chopconf;

public:
    Stepper(uart_inst_t *uart, int directionPin, int stepPin, uint8_t address, const char* name);

    int getDirectionPin() const;
    int getStepPin() const;
    uint8_t getAddress() const;
    const char* getName() const;
    bool getDirection() const;
    int getStepRes() const;
    bool setStepRes(int microsteps);
    void setDirection(bool direction);
    void initGPIO() const;
    bool configure();
    void step() const;
    void step(int count) const;

};


#endif