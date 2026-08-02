#include "payloadHandler.h"

#include <cstring>

#include "pico/time.h"

//One byte at 115200 8N1 takes ~87us. Waiting a couple of byte times before
//draining means bytes still on the wire land in the FIFO first, so we
//actually discard them instead of reading them as the start of the reply.
static constexpr uint32_t DRAIN_GUARD_US = 200;

//The driver needs to finish latching a write before it will answer anything
//else on the bus. Without this the next datagram gets dropped.
static constexpr uint32_t WRITE_SETTLE_US = 2000;

static void flushRx(uart_inst_t *uart) {
    sleep_us(DRAIN_GUARD_US);
    while (uart_is_readable(uart)) {
        uart_getc(uart);
    }
}

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

    flushRx(uart);

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

bool writeReg(uart_inst_t* uart, uint8_t addr, Register reg, uint32_t data, bool verify) {

    uint32_t countBefore = 0;
    if (verify && readReg(uart, addr, IFCNT, &countBefore) == false) {
        return false;
    }

    flushRx(uart);

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

    //Reading the echo only proves our own bytes made it back around the
    //single wire bus, not that the driver did anything with them.
    sleep_us(WRITE_SETTLE_US);

    if (verify == false) {
        return true;
    }

    uint32_t countAfter = 0;
    if (readReg(uart, addr, IFCNT, &countAfter) == false) {
        return false;
    }

    return ((countAfter - countBefore) & 0xFF) == 1;
}

