#include "payloadHandler.h"

#include <cstring>

//Ripped from datasheet. Not gonna pretend I know what's going on with the checksome. (I didn't read it)
uint8_t tmc_crc(const uint8_t *data, size_t len) {
    uint8_t crc = 0;
    for (size_t i = 0; i < len; i++) {
        uint8_t b = data[i];
        for (int j = 0; j < 8; j++) {
            if ((crc >> 7) ^ (b & 0x01)) crc = (crc << 1) ^ 0x07;
            else crc = (crc << 1);
            b >>= 1;
        }
    }
    return crc;
}

bool readUart(uart_inst_t *uart, uint8_t *bytes, size_t len, uint32_t timeout) {
    for (int i = 0; i < len; i++) {
        if (uart_is_readable_within_us(uart, timeout)) {
            bytes[i] = uart_getc(uart);
        } else {
            return false;
        }
    }
    return true;
}

bool readReg(uart_inst_t* uart, uint8_t addr, Register reg, uint32_t *out) {
    //make payload
    uint8_t payload[4];
    payload[0] = 0x05; //Header
    payload[1] = addr;
    payload[2] = reg;
    payload[3] = tmc_crc(payload, 3);

    //Clear the queue.
    while (uart_is_readable(uart)) {
        uart_getc(uart);
    }

    uart_write_blocking(uart, payload, 4);

    uint8_t echo[4];
    if (readUart(uart, echo, 4, 5000) == false) {
        return false;
    }

    uint8_t data[8];
    if (readUart(uart, data, 8, 5000) == false) {
        return false;
    }

    //verify
    if (data[1] != 0xFF) return false;
    if (data[2] != reg)  return false;
    uint8_t checksum = tmc_crc(data, 7);
    if (checksum != data[7]) {
        return false;
    }

    //Grab just the value
    *out = (static_cast<uint32_t>(data[3]) << 24)
     | (static_cast<uint32_t>(data[4]) << 16)
     | (static_cast<uint32_t>(data[5]) << 8)
     |  static_cast<uint32_t>(data[6]);

    return true;
}

bool writeReg(uart_inst_t* uart, uint8_t addr, Register reg, uint32_t data) {

    //Clear the queue. Stale bytes here shift the echo read below and every
    //byte after it reads garbage - readReg already does this, writeReg never did.
    while (uart_is_readable(uart)) {
        uart_getc(uart);
    }

    uint8_t payload[8];
    payload[0] = 0x05;
    payload[1] = addr;
    payload[2] = reg | 0x80;
    payload[3] = (data >> 24) & 0xFF;
    payload[4] = (data >> 16) & 0xFF;
    payload[5] = (data >> 8) & 0xFF;
    payload[6] = data & 0xFF;
    payload[7] = tmc_crc(payload, 7);

    uart_write_blocking(uart, payload, 8);

    uint8_t echo[8];
    if (readUart(uart, echo, 8, 5000) == false) {
        return false;
    }

    //verify
    uint32_t reading;
    if (readReg(uart, addr, reg, &reading) == false) return false;

    if (reading == data) return true;

    return false;

}

