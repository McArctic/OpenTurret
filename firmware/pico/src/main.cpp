#include <stdio.h>

#include "pico/stdlib.h"
#include "payloadHandler.h"
#include "objects/Stepper.h"

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


void initUart() {
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

    //Drive wants to be stupid and have only one uart port so I slapped a 1k resistor on the tx line.
    gpio_set_drive_strength(UART0_TX_PIN, GPIO_DRIVE_STRENGTH_2MA);

    uart_init(UART_ID, BAUD_RATE);

    uart_set_fifo_enabled(UART_ID, true);
}

void waitForDriverPower(uart_inst_t *uart) {
    printf("Waiting for driver power...\n");
    while (true) {
        for (uint8_t addr = 0; addr <= 3; addr++) {
            uint32_t discard;
            if (readReg(uart, addr, GCONF, &discard)) {
                printf("Driver power detected.\n");
                return;
            }
        }
    }
}

void setup(Stepper &panStepper, Stepper &tiltStepper) {
    printf("setup \n");
    stdio_init_all();
    initUart();
    waitForDriverPower(UART_ID);
    printf("Allowing driver to start up \n");
    sleep_ms(5000);

    if (!panStepper.configure()) {
        printf("Failed To Configure Pan Stepper \n");
    }
    printf("Configured pan \n");
    if (!tiltStepper.configure()) {
        printf("Failed To Configure Tilt Stepper \n");
    }
    printf("Configured step \n");
}

int main() {
    Stepper panStepper = Stepper(UART_ID, DIR_PIN_PAN, STEP_PIN_PAN, PAN_UART_ADDR, "pan");
    Stepper tiltStepper = Stepper(UART_ID, DIR_PIN_TILT, STEP_PIN_TILT, TILT_UART_ADDR, "tilt");
    setup(panStepper, tiltStepper);

    while (true) {
        tiltStepper.setStepRes(0);
        tiltStepper.step(200);
        tiltStepper.setDirection(true);
        tiltStepper.step(200);
        tiltStepper.setDirection(false);

        tiltStepper.setStepRes(128);
        tiltStepper.step(200);
        tiltStepper.setDirection(false);
        tiltStepper.step(200);

        tiltStepper.setDirection(true);
        tiltStepper.setStepRes(0);
        tiltStepper.step(180);
        tiltStepper.setDirection(true);
        tiltStepper.step(120);
        tiltStepper.setDirection(false);
    }

}
