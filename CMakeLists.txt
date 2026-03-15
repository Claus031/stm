cmake_minimum_required(VERSION 3.20)

project(stm32f4_blink C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MCU_DEFINES STM32F407xx)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker/stm32f407vg.ld)
set(CMSIS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/STM32CubeF4/Drivers/CMSIS/Include)
set(CMSIS_DEVICE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include)
set(OPENOCD_CONFIG ${CMAKE_SOURCE_DIR}/scripts/openocd/stm32f4discovery.cfg)

set(CPU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

set(COMMON_FLAGS
    ${CPU_FLAGS}
    -ffunction-sections
    -fdata-sections
    -fno-common
    -Wall
    -Wextra
    -Wpedantic
)

string(JOIN " " CPU_FLAGS_STR ${CPU_FLAGS})
string(JOIN " " COMMON_FLAGS_STR ${COMMON_FLAGS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS_STR}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${COMMON_FLAGS_STR} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CPU_FLAGS_STR} -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs")

set(COMMON_FIRMWARE_SOURCES
    external/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
    external/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f407xx.s
)

function(add_stm32_target target_name app_source)
    add_executable(${target_name}.elf
        ${app_source}
        ${COMMON_FIRMWARE_SOURCES}
    )

    target_compile_definitions(${target_name}.elf PRIVATE ${MCU_DEFINES})

    target_include_directories(${target_name}.elf PRIVATE
        ${CMSIS_INCLUDE_DIR}
        ${CMSIS_DEVICE_INCLUDE_DIR}
    )

    target_link_options(${target_name}.elf PRIVATE
        -T${LINKER_SCRIPT}
        -Wl,-Map=${target_name}.map
    )

    add_custom_command(TARGET ${target_name}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${target_name}.elf ${target_name}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${target_name}.elf ${target_name}.bin
        COMMAND ${CMAKE_SIZE} --format=berkeley ${target_name}.elf
    )
endfunction()

add_stm32_target(blink src/main.c)
add_stm32_target(blink_all_leds src/main_all_leds.c)

find_program(OPENOCD_EXE NAMES openocd)
if(OPENOCD_EXE)
    add_custom_target(flash-blink
        COMMAND ${OPENOCD_EXE} -f ${OPENOCD_CONFIG}
                -c "program ${CMAKE_BINARY_DIR}/blink.elf verify reset exit"
        DEPENDS blink.elf
        COMMENT "Flashing blink.elf via ST-Link/OpenOCD"
        VERBATIM
    )

    add_custom_target(flash-all-leds
        COMMAND ${OPENOCD_EXE} -f ${OPENOCD_CONFIG}
                -c "program ${CMAKE_BINARY_DIR}/blink_all_leds.elf verify reset exit"
        DEPENDS blink_all_leds.elf
        COMMENT "Flashing blink_all_leds.elf via ST-Link/OpenOCD"
        VERBATIM
    )

    add_custom_target(debug-openocd
        COMMAND ${OPENOCD_EXE} -f ${OPENOCD_CONFIG}
                -c "init"
                -c "reset halt"
        COMMENT "Launching OpenOCD debug server for STM32F4Discovery"
        VERBATIM
    )
else()
    message(WARNING "openocd not found; flash and debug targets are unavailable.")
endif()

find_program(CPPCHECK_EXE NAMES cppcheck)
if(CPPCHECK_EXE)
    add_custom_target(lint
        COMMAND ${CPPCHECK_EXE}
            --enable=warning,style,performance,portability
            --inline-suppr
            --suppress=missingIncludeSystem
            --suppress=preprocessorErrorDirective
            --suppress=comparePointers:*/external/STM32CubeF4/*
            --suppress=*:*/external/STM32CubeF4/*
            --error-exitcode=1
            --std=c11
            -DSTM32F407xx
            -D__GNUC__
            -I${CMSIS_INCLUDE_DIR}
            -I${CMSIS_DEVICE_INCLUDE_DIR}
            ${CMAKE_SOURCE_DIR}/src/main.c
            ${CMAKE_SOURCE_DIR}/src/main_all_leds.c
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running cppcheck"
        VERBATIM
    )
else()
    message(WARNING "cppcheck was not found. The 'lint' target is unavailable.")
endif()
