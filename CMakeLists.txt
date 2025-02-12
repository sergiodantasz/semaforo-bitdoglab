cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(semaforo-bitdoglab C CXX ASM)

pico_sdk_init()

add_executable(semaforo-bitdoglab semaforo-bitdoglab.c inc/ssd1306_i2c.c)

pico_set_program_name(semaforo-bitdoglab "semaforo-bitdoglab")
pico_set_program_version(semaforo-bitdoglab "0.1")

pico_enable_stdio_uart(semaforo-bitdoglab 0)
pico_enable_stdio_usb(semaforo-bitdoglab 1)

target_link_libraries(semaforo-bitdoglab pico_stdlib hardware_i2c)

target_include_directories(semaforo-bitdoglab PRIVATE ${CMAKE_CURRENT_LIST_DIR})

pico_add_extra_outputs(semaforo-bitdoglab)
