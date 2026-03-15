cmake_minimum_required(VERSION 3.20)

project(stm32f4_blink C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET blink)
set(MCU_DEFINES STM32F407xx)

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
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CPU_FLAGS_STR} -Wl,--gc-sections -Wl,-Map=${TARGET}.map -specs=nano.specs -specs=nosys.specs")

add_executable(${TARGET}.elf
    src/main.c
    external/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
    external/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f407xx.s
)

target_compile_definitions(${TARGET}.elf PRIVATE ${MCU_DEFINES})

target_include_directories(${TARGET}.elf PRIVATE
    external/STM32CubeF4/Drivers/CMSIS/Include
    external/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include
)

target_link_options(${TARGET}.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/linker/stm32f407vg.ld
)

add_custom_command(TARGET ${TARGET}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${TARGET}.elf ${TARGET}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET}.elf ${TARGET}.bin
    COMMAND ${CMAKE_SIZE} --format=berkeley ${TARGET}.elf
)

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
            -I${CMAKE_SOURCE_DIR}/external/STM32CubeF4/Drivers/CMSIS/Include
            -I${CMAKE_SOURCE_DIR}/external/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include
            ${CMAKE_SOURCE_DIR}/src/main.c
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running cppcheck"
        VERBATIM
    )
else()
    message(WARNING "cppcheck was not found. The 'lint' target is unavailable.")
endif()
