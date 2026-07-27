#include "pico/stdlib.h"

#ifndef PAN_STEPPER_PINS
#define DIR_PIN_PAN 14
#define STEP_PIN_PAN 15
#define UART0_TX_PIN_PAN 0
#define UART0_RX_PIN_PAN 1
#define PAN_UART_ID uart0
#define LED_PIN PICO_DEFAULT_LED_PIN
#endif

#ifndef TILT_STEPPER_PINS
#define DIR_PIN_TILT 10
#define STEP_PIN_TILT 11
#define UART1_TX_PIN_TILT 4
#define UART1_RX_PIN_TILT 5
#define TILT_UART_ID uart1
#endif

#ifndef UART_CONF
#define BAUD_RATE 115200
#endif

#ifndef STEP_PULSE_US
#define STEP_PULSE_US 2
#endif



void initUart() {
    uart_init(PAN_UART_ID, BAUD_RATE);
    uart_init(TILT_UART_ID, BAUD_RATE);

    gpio_set_function(UART0_TX_PIN_PAN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN_PAN, GPIO_FUNC_UART);

    gpio_set_function(UART1_TX_PIN_TILT, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN_TILT, GPIO_FUNC_UART);
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
    initUart();
    initPins();

    //Config Drivers
}


int main() {
}
