# OpenTurret

An open-source turret

The firmware targets a Raspberry Pi Pico and is built with CMake and the Pico SDK.

## Setup

See [docs/SETUP.md](docs/SETUP.md) for the complete walkthrough, including CLion and VS Code.

With the toolchain installed and `PICO_SDK_PATH` configured:

```bash
cd firmware/pico
cmake -B build -G Ninja
cmake --build build
```

The output is `build/OpenTurret.uf2`. Hold BOOTSEL, connect the Pico, and copy the file to the drive that mounts.

In CLion, open `firmware/pico` rather than the repository root. That directory contains `CMakeLists.txt`.

## Requirements

- [Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [Arm GNU toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) (arm-none-eabi)
- [CMake](https://cmake.org/download/)
- [Ninja](https://github.com/ninja-build/ninja/releases)

On Windows the [Pico setup installer](https://github.com/raspberrypi/pico-setup-windows/releases) provides all of them in a single download.
