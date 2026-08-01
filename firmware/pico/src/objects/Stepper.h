#ifndef OPENTURRET_STEPPER_H
#define OPENTURRET_STEPPER_H

#ifndef PAN_STEPPER_PINS
#define DIR_PIN_PAN 14
#define STEP_PIN_PAN 15
#define PAN_UART_ADDR 0x0
#include <cstdint>

#include "hardware/uart.h"
#endif

#ifndef TILT_STEPPER_PINS
#define DIR_PIN_TILT 10
#define STEP_PIN_TILT 11
#define TILT_UART_ADDR 0x1
#endif

class Stepper {
private:
    int directionPin;
    int stepPin;
    uint8_t address;
    uart_inst_t *uart;
    char* name;

public:
    Stepper(uart_inst_t *uart, int directionPin, int stepPin, uint8_t address, char* name);

    int getDirectionPin() const;
    int getStepPin() const;
    uint8_t getAddress() const;
    char* getName() const;
    bool configure() const;
};


#endif