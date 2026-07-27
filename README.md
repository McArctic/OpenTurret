# OpenTurret

An open-source motorised turret built around a Raspberry Pi Pico (RP2040) and
BIGTREETECH TMC2209 stepper drivers.

## Repository layout

| Path | Contents |
| --- | --- |
| [`firmware/pico/`](firmware/pico) | Pico firmware — the C++/CMake project that drives the steppers |

Additional parts of the project (mechanical design, host-side software, docs)
will be added as sibling top-level folders.

## Getting started

The firmware is the only buildable component today. See
[`firmware/pico/README.md`](firmware/pico/README.md) for toolchain requirements
and build instructions.

## Hardware

- Raspberry Pi Pico (RP2040, non-W)
- BIGTREETECH TMC2209 V1.3 stepper driver
- Separate 12–24 V supply for the motor rail (VM) — the Pico cannot power it

All grounds (Pico, driver, motor supply) must be common.
