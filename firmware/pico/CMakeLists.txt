# OpenTurret Pico firmware — build definition
# (write it yourself; see the walkthrough)
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(OpenTurret CXX C ASM)

pico_sdk_init()

add_executable(OpenTurret src/main.cpp)

target_link_libraries(OpenTurret pico_stdlib)

pico_add_extra_outputs(OpenTurret)