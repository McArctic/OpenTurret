#ifndef PAYLOAD_HANDLER_H
#define PAYLOAD_HANDLER_H

#include "pico/stdlib.h"

enum Register {
    GCONF = 0x00, //Config
    GSTAT = 0x01, //Reset flags 1 to clr
    IFCNT = 0x02, // Counts successful wries
    IOIN = 0x06, // Live pin states
    IHOLD_IRUN = 0x10, // Motor current
    TPOWERDOWN = 0x11,
    TSPEP = 0x12, // Time between last steps
    TPWMTHRS = 0x13,
    TCOOLTHRS = 0x14,
    VACTUAL = 0x22,
    SGTHRS = 0x40,
    SG_RESULT = 0x41, // Load measurement
    CHOPCONF = 0x6C, // Conf for microsteps, interpol, and chopper timing
    DRV_STATUS = 0x6F, //Stats for overtemp, short, open coil, standstill
    PWMCONG = 0x70, // stealthChop tuning.
};

uint8_t tmc_crc(const uint8_t *data, size_t len);
bool readUart(uart_inst_t *uart, uint8_t *bytes, size_t len, uint32_t timeout);
bool readReg(uart_inst_t* uart, uint8_t addr, Register reg, uint32_t *out);
bool writeReg(uart_inst_t* uart, uint8_t addr, Register reg, uint32_t data, bool verify = true);

#endif
