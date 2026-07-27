# Setup

Complete setup for a new Windows machine.

At the end you will be able to clone the repository, build the firmware, and flash a
`.uf2` to the Pico.

## Requirements

Four components:

| Component | Purpose |
| --- | --- |
| Arm GNU toolchain | The compiler. The Pico is ARM and the host is x86, so the build cross compiles |
| Pico SDK | The libraries, and the CMake configuration that directs CMake to the ARM compiler |
| CMake | Generates the build |
| Ninja | Executes the build |

CLion includes its own CMake and Ninja. If you intend to work exclusively in CLion those
two are optional, though installing them is recommended for command line builds.

## 1. Install the tools

Two options. Choose one.

### Option A, the Raspberry Pi installer

Recommended for a machine being configured from scratch. Download the current release from
[pico-setup-windows](https://github.com/raspberrypi/pico-setup-windows/releases) and run it.

It installs the SDK, the Arm toolchain, CMake, Ninja, OpenOCD and picotool, and configures
`PICO_SDK_PATH` automatically. Continue at step 3.

### Option B, winget

Offers more control, and is preferable if some components are already present. Run each
command in PowerShell:

```bash
winget install Kitware.CMake
```

```bash
winget install Ninja-build.Ninja
```

```bash
winget install Arm.GnuArmEmbeddedToolchain
```

This does not install the SDK. Complete step 2 afterwards.

On a managed or work machine, CMake and the Arm toolchain install system wide and require
administrator rights. Append `--scope user` if administrator access is unavailable. Ninja
already installs per user.

## 2. Install the SDK

Skip this step if you used Option A.

Either download the SDK installer from
[pico-setup-windows](https://github.com/raspberrypi/pico-setup-windows/releases), or clone
it directly:

```bash
git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git
```

`--recurse-submodules` is required. Without it the core SDK is present but tinyusb is not,
and any later use of USB serial fails with missing header errors that do not identify the
cause.

Prefer an install path without spaces. Paths containing spaces are supported but interact
poorly with some tooling.

## 3. Configure environment variables

Two paths differ between machines: the SDK and the compiler. Both are supplied through
environment variables so that no path is hardcoded in the repository.

### PICO_SDK_PATH

Required. Set it to the directory containing `pico_sdk_init.cmake`.

PowerShell, user scope, no administrator rights required:

```bash
[Environment]::SetEnvironmentVariable('PICO_SDK_PATH','C:\Program Files\Raspberry Pi\Pico SDK v1.5.1\pico-sdk','User')
```

Alternatively, press Win, search for "Edit the system environment variables", open
Environment Variables, and add it under User variables.

Restart any open terminal or IDE afterwards. Applications read the environment at launch,
so a running process retains the previous value. This accounts for most cases where a
build succeeds in one window and fails in another.

Confirm the value:

```bash
echo $env:PICO_SDK_PATH
```

### PICO_TOOLCHAIN_PATH

Only required if the Arm toolchain is not on `PATH`. Check first:

```bash
(Get-Command arm-none-eabi-gcc).Source
```

If that prints a path, the toolchain is discoverable and nothing further is needed. The Arm
installer normally adds its `bin` directory to `PATH`, so this is the usual case.

If it reports that the command is not found, the compiler is installed somewhere the build
cannot see. Locate it:

```bash
Get-ChildItem "C:\Program Files*\Arm GNU Toolchain*" -Recurse -Filter arm-none-eabi-gcc.exe -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName
```

Then set `PICO_TOOLCHAIN_PATH` to the toolchain root, meaning the directory that contains
`bin`, not `bin` itself:

```bash
[Environment]::SetEnvironmentVariable('PICO_TOOLCHAIN_PATH','C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.2 rel1','User')
```

The version number is part of the path and differs between installations. Determine the
value on each machine rather than copying it.

The SDK searches `PICO_TOOLCHAIN_PATH` first and falls back to `PATH`, so setting this
variable also allows a specific toolchain version to be selected when several are present.

## 4. Clone and build

```bash
git clone https://github.com/McArctic/OpenTurret.git
cd OpenTurret/firmware/pico
cmake -B build -G Ninja
cmake --build build
```

Producing `build/OpenTurret.uf2` confirms the toolchain is installed correctly.

Complete this before configuring CLion. A command line failure reports the underlying
error directly, whereas the IDE presents the same failure with less detail.

## 5. Flash the firmware

1. Disconnect the Pico
2. Hold the BOOTSEL button
3. Reconnect it, then release the button
4. It mounts as a USB drive named RPI-RP2
5. Copy `OpenTurret.uf2` to the drive

The drive dismounts automatically and the Pico restarts into the new firmware. This is
expected behaviour.

## 6. CLion

### Open the correct directory

File, then Open, and select `firmware/pico`. Do not open the repository root.

CLion searches for `CMakeLists.txt` in the directory it opens, and the root does not
contain one. Opening the root produces a file browser with no build configuration.

### Toolchain

Settings, then Build, Execution, Deployment, then Toolchains.

Leave the C and C++ compiler fields at their defaults. Do not set them to
`arm-none-eabi-gcc`.

This is intentional, and is the step most often assumed to be missing. The cross compiler
is selected by the SDK, not by CLion. During configuration the SDK loads
`cmake/preload/toolchains/pico_arm_gcc.cmake`, which locates `arm-none-eabi-gcc` and assigns
`CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` directly. That occurs before the project is
processed and takes precedence regardless of the toolchain settings in CLion.

The compiler fields in CLion's toolchain apply to CLion's own validation, which runs against
the host. Setting them to the cross compiler causes CLion to build a host test program with
a compiler that cannot produce host binaries, and configuration fails with "the C compiler
is not able to compile a simple test program" before the project is reached.

If the compiler cannot be located the failure is different and explicit:
"Compiler 'arm-none-eabi-gcc' not found, you can specify search path with
PICO_TOOLCHAIN_PATH". That is resolved with the environment variable in step 3, not in this
dialog.

Confirm that CMake and Build Tool are populated. The bundled versions are sufficient.

### CMake profile

Settings, then Build, Execution, Deployment, then CMake.

- Generator: Ninja
- Build directory: `build`
- Environment: add `PICO_SDK_PATH`, and `PICO_TOOLCHAIN_PATH` if it was needed, here if
  they were not set system wide in step 3

Then select Tools, CMake, Reset Cache and Reload Project.

### Build

Select the OpenTurret target and build. The `.uf2` is written to `build/`. Flash it as
described in step 5.

The Run action does not apply. The binary targets the Pico rather than the host, and CLion
may report the target as not runnable. Use build only.

## 7. VS Code

Install the
[Raspberry Pi Pico extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico),
then choose Import Project and select `firmware/pico`.

The extension provisions its own copy of the SDK, toolchain, CMake and Ninja under your
user profile. Steps 1 through 3 are not required for it.

Note when using both editors: these are a separate toolchain from the one CLion uses and
the two can diverge in version. A build that succeeds in one editor and fails in the other
is usually explained by this.

## Working across multiple machines

All machine specific state is already excluded from version control:

- `build/` and `cmake-build-*` are ignored as generated output
- `.idea/` is ignored, as it records CLion paths valid on one machine only
- The SDK is external to the repository and located through `PICO_SDK_PATH`

On an additional machine, complete steps 1 through 3 once, then clone and build. The SDK and
the toolchain may reside at different paths on each machine with no repository changes.
Toolchain paths in particular include a version number, so they rarely match between
installations. Resolve them per machine using the commands in step 3 and never copy a path
from another system.

Pull before starting work and push when finished, otherwise changes will have to be merged
between machines.

## Troubleshooting

**"SDK location was not specified"**
`PICO_SDK_PATH` is unset, or the IDE was started before it was configured. Restart the IDE.

**"Directory does not appear to contain the Raspberry Pi Pico SDK"**
The variable resolves one level away from the correct directory. It must reference the
directory containing `pico_sdk_init.cmake`.

**"Compiler 'arm-none-eabi-gcc' not found"**
The Arm toolchain is not installed, or is installed outside `PATH`. Set
`PICO_TOOLCHAIN_PATH`, see step 3. Do not set the compiler in CLion's toolchain settings.

**"The C compiler is not able to compile a simple test program"**
The ARM compiler has been set in CLion's toolchain settings. Clear those fields, see step 6.

**A configuration change has no effect**
`CMakeCache.txt` retains the previous value. Delete the `build` directory and configure again.

**Builds on the command line but not in CLion**
The environments differ. Verify that the CMake profile defines `PICO_SDK_PATH`, and that
CLion was restarted after the variable was set.
