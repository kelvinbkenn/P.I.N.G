cmake_minimum_required(VERSION 3.13)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ping C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

pico_sdk_init()

add_executable(ping
    main.c
    ping/console.c
    ping/display.c
    ping/radio.c
    ping/buzzer.c
    lib/nrf24.c
    lib/ssd1306.c
    protocol.c
)

target_include_directories(ping PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

target_link_libraries(ping
    pico_stdlib
    hardware_spi
    hardware_i2c
    hardware_gpio
)

pico_add_extra_outputs(ping)
