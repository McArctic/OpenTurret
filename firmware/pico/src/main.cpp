#include <stdio.h>

#include "pico/stdlib.h"
#include "payloadHandler.h"

#ifndef PAN_STEPPER_PINS
#define DIR_PIN_PAN 14
#define STEP_PIN_PAN 15
#define PAN_UART_ADDR 0x0
#endif

#ifndef TILT_STEPPER_PINS
#define DIR_PIN_TILT 10
#define STEP_PIN_TILT 11
#define TILT_UART_ADDR 0x1
#endif

#ifndef UART_CONF
#define UART_ID uart0
#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define BAUD_RATE 115200
#endif

#ifndef STEP_PULSE_US
#define STEP_PULSE_US 2
#endif

void initUart() {
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

    //Drive wants to be stupid and have only one uart port so I slapped a 1k resistor on the tx line.
    gpio_set_drive_strength(UART0_TX_PIN, GPIO_DRIVE_STRENGTH_2MA);

    uart_init(UART_ID, BAUD_RATE);

    uart_set_fifo_enabled(UART_ID, true);
}

bool configDriver(uart_inst_t *uart, uint8_t address, const char* name) {

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


    while (!writeReg(uart, address, GCONF, modified) && !writeReg(uart, address, IHOLD_IRUN, currentConfig)) {
        printf("%s: Failed to config", name);
        sleep_ms(500);
    }
    printf("%s: Configured", name);
    return true;
}



void initPins() {
    gpio_init(DIR_PIN_PAN);
    gpio_init(STEP_PIN_PAN);

    gpio_init(DIR_PIN_TILT);
    gpio_init(STEP_PIN_TILT);

    gpio_set_dir(DIR_PIN_PAN, GPIO_OUT);
    gpio_set_dir(STEP_PIN_PAN, GPIO_OUT);
    gpio_set_dir(DIR_PIN_TILT, GPIO_OUT);
    gpio_set_dir(STEP_PIN_TILT, GPIO_OUT);
}

void setup() {
    stdio_init_all();

    initUart();
    initPins();

    configDriver(UART_ID, PAN_UART_ADDR, "pan");
    configDriver(UART_ID, TILT_UART_ADDR, "tilt");
}


int main() {
    setup();
}
