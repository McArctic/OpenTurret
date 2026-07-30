# OpenTurret Pico firmware — build definition
# (write it yourself; see the walkthrough)
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(OpenTurret CXX C ASM)

pico_sdk_init()

add_executable(OpenTurret src/main.cpp
        src/payloadHandler.cpp
        src/payloadHandler.h
        src/debug.cpp                       # [OT_DEBUG] remove with debug.h/.cpp
        src/debug.h)                        # [OT_DEBUG]

pico_enable_stdio_usb(OpenTurret 1)
pico_enable_stdio_uart(OpenTurret 0)

target_link_libraries(OpenTurret pico_stdlib
        hardware_watchdog)                  # [OT_DEBUG] scratch regs only

pico_add_extra_outputs(OpenTurret)