cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi-gcc.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(
  minipeg
  C
  CXX
  ASM
)

# ############### Common #####################

  lib/mdrivlib/drivers/pin.cc
  lib/mdrivlib/drivers/tim.cc
  lib/mdrivlib/drivers/timekeeper.cc
  lib/libhwtests/src/AdcChecker.cc
  lib/libhwtests/src/AdcRangeChecker.cc
  lib/libhwtests/src/ButtonChecker.cc
  lib/libhwtests/src/CodecCallbacks.cc
  lib/libhwtests/src/GateInChecker.cc
  lib/libhwtests/src/GateOutChecker.cc
  lib/libhwtests/src/GateOutput.cc
  lib/libhwtests/src/LEDTester.cc
  src/analog_conditioning.cc
  src/calibration.cc
  src/dig_inouts.cc
  src/env_transition.cc
  src/env_update.cc
  src/envelope_calcs.cc
  src/flash_user.cc
  src/hardware_tests/hardware_test_adc.cc
  src/hardware_tests/hardware_test_dac.cc
  src/hardware_tests/hardware_test_gates.cc
  src/hardware_tests/hardware_test_leds_buttons.cc
  src/hardware_tests/hardware_test_util.cc
  src/hardware_tests/hardware_tests.cc
  src/leds.cc
  src/libc_stub.c
  src/libcpp_stub.cc
  src/main.cc
  src/params.cc
  src/pingable_env.cc
  src/shareddrv/dac.cc
  src/shareddrv/debounced_digins.cc
  src/shareddrv/flash.cc
  src/shareddrv/pwm.cc
  src/system_mode.cc
  src/timers.cc
  src/trigout.cc
)

set(
  COMMON_INCLUDES
  src
  src/hardware_tests
  lib/libhwtests/inc
  stm32/CMSIS/Include
  lib/mdrivlib
  lib/mdrivlib/drivers
  lib/cpputil
)

function(set_hal_sources sources family)
  set(
    ${sources}
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_adc.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_adc_ex.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_cortex.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_dac.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_dma.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_exti.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_flash.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_flash_ex.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_gpio.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_pwr.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_pwr_ex.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_rcc.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_rcc_ex.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_tim.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_ll_tim.c
    PARENT_SCOPE
  )
endfunction()

function(set_bootloader_hal_sources sources family)
  set(
    ${sources}
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_cortex.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_flash.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_flash_ex.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_gpio.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_pwr.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_pwr_ex.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_rcc.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_rcc_ex.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_tim.c
    stm32/HAL/stm32${family}/Src/stm32${family}xx_ll_tim.c
    PARENT_SCOPE
  )
endfunction()

set(
  BOOTLOADER_COMMON_SOURCES
  src/bootloader/bootloader.cc
  src/bootloader/leds.cc
  src/bootloader/animation.cc
  src/bootloader/bl_utils.cc
  src/bootloader/stm_audio_bootloader/fsk/packet_decoder.cc
  src/libc_stub.c
  src/libcpp_stub.cc
  src/shareddrv/flash.cc
  lib/mdrivlib/drivers/pin.cc
  lib/mdrivlib/drivers/timekeeper.cc
  lib/mdrivlib/drivers/tim.cc
)

set(
  BOOTLOADER_COMMON_INCLUDES
  ./stm32/CMSIS/Include
  ./src/bootloader
  ./src/bootloader/stmlib
  ./src
  ./lib/mdrivlib
  ./lib/mdrivlib/drivers
  ./lib/cpputil
)

set(COMMON_DEFINES USE_HAL_DRIVER USE_FULL_LL_DRIVER)

set(
  COMMON_COMPILE_OPTIONS
  -g3
  -fdata-sections
  -ffunction-sections
  -fno-common
  -ffreestanding
  -fno-unwind-tables
  -mfloat-abi=hard
  -mthumb
  -nostartfiles
  -nostdlib
  -Wdouble-promotion
  -Werror=return-type
  $<$<COMPILE_LANGUAGE:CXX>:
  -std=c++2a
  -fno-rtti
  -fno-threadsafe-statics
  -fno-exceptions
  -Wno-register
  -Wno-volatile
  >
  $<$<CONFIG:Debug>:-O0>
  $<$<CONFIG:Release>:-O3>
)

set(
  COMMON_LINK_OPTS
  -Wl,--gc-sections
  -nostdlib
  -mthumb
  -mfloat-abi=hard
)

# ############### Common commands #####################

function(target_link_script target_base link_script)
  target_link_options(
    ${target_base}.elf
    PRIVATE
    -Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${target_base}.map,--cref
    -T
    ${link_script}
    ${ARGN}
  )
  set_target_properties(${target_base}.elf PROPERTIES LINK_DEPENDS ${link_script})
endfunction()

function(add_bin_hex_command target_base)
  add_custom_command(
    TARGET ${target_base}.elf
    POST_BUILD
    COMMAND echo "Binaries are in $<TARGET_FILE_DIR:${target_base}.elf>"
    COMMAND echo "Target filename is $<TARGET_FILE:${target_base}.elf>"
    COMMAND arm-none-eabi-objcopy -O ihex $<TARGET_FILE:${target_base}.elf>
    $<TARGET_FILE_DIR:${target_base}.elf>/${target_base}.hex
    COMMAND arm-none-eabi-objcopy -O binary $<TARGET_FILE:${target_base}.elf>
    $<TARGET_FILE_DIR:${target_base}.elf>/${target_base}.bin
  )
  set_target_properties(
    ${target_base}.elf
    PROPERTIES
    ADDITIONAL_CLEAN_FILES
    "$<TARGET_FILE_DIR:${target_base}.elf>/${target_base}.hex;$<TARGET_FILE_DIR:${target_base}.elf>/${target_base}.bin;f423.map"
  )
endfunction()

# ############### Target-specific #####################

if(${TARGET} STREQUAL "f423")
  message("Target is STM32F423")

  set_hal_sources(HAL_SOURCES f4)
  set(PROJECT_DRIVER_DIR src/f423-drivers)
  set(MDRIVLIB_TARGET_DIR lib/mdrivlib/target/stm32f4xx)
  set(FAMILY_NAME stm32f4)
  set(TARGET_LINK_SCRIPT ${CMAKE_SOURCE_DIR}/linker/STM32F423VHHx_FLASH.ld)

  set(ARCH_FLAGS -mcpu=cortex-m4 -mfpu=fpv4-sp-d16)
  set(ARCH_DEFINES STM32F423xx STM32F4 ARM_MATH_CM4)

  set_bootloader_hal_sources(BOOTLOADER_HAL_SOURCES f4)
  set(
    TARGET_BOOTLOADER_SOURCES
    src/bootloader/gate_input_f423.cc
    src/f423-drivers/adc.cc
    stm32/HAL/stm32f4/Src/stm32f4xx_hal_adc.c
    stm32/HAL/stm32f4/Src/stm32f4xx_hal_dma.c
  )
  set(TARGET_BOOTLOADER_LINK_SCRIPT ${CMAKE_SOURCE_DIR}/linker/STM32F423VHHx_FLASH_BOOTLOADER.ld)
  set(
    WAV_ENCODE_PYTHON_CMD
    # Note: flash sector size is 128kB, but we arbitrarily limit app size with -g in order to make the
    # wave file shorter
    # cmake-format: off
    python ${CMAKE_SOURCE_DIR}/src/bootloader/stm_audio_bootloader/fsk/encoder.py
    -s 22050 -b 16 -n 8 -z 4 -p 256 -g 40960 -k 1000 $<TARGET_FILE_DIR:${TARGET}.elf>/${TARGET}.bin
    # cmake-format: on
  )

elseif(${TARGET} STREQUAL "f746")
  message("Target is STM32F746")

  set_hal_sources(HAL_SOURCES f7)
  set(PROJECT_DRIVER_DIR src/f746-drivers)
  set(MDRIVLIB_TARGET_DIR lib/mdrivlib/target/stm32f7xx)
  set(FAMILY_NAME stm32f7)
  set(TARGET_LINK_SCRIPT ${CMAKE_SOURCE_DIR}/linker/STM32F746IEKx_FLASH.ld)

  set(ARCH_FLAGS -mcpu=cortex-m7 -mfpu=fpv5-sp-d16)
  set(ARCH_DEFINES STM32F746xx STM32F7 ARM_MATH_CM7)

  set_bootloader_hal_sources(BOOTLOADER_HAL_SOURCES f7)
  set(TARGET_BOOTLOADER_SOURCES src/bootloader/gate_input_f746.cc)
  set(TARGET_BOOTLOADER_LINK_SCRIPT ${CMAKE_SOURCE_DIR}/linker/STM32F746IEKx_FLASH_BOOTLOADER.ld)
  set(
    WAV_ENCODE_PYTHON_CMD
    # Note: flash sector size is 128kB, but we arbitrarily limit app size with -g, in order to make the
    # wave file shorter
    # cmake-format: off
    python ${CMAKE_SOURCE_DIR}/src/bootloader/stm_audio_bootloader/fsk/encoder.py
    -s 22050 -b 16 -n 8 -z 4 -p 256 -g 43520 -k 1000 $<TARGET_FILE_DIR:${TARGET}.elf>/${TARGET}.bin
    # cmake-format: on
  )

else()
  message("Unknown TARGET ${TARGET}, run with `cmake -B build -DTARGET=[f423 | f746]`")
endif()

# Add target-specific project files and paths:
set(
  TARGET_SOURCES
  ${TARGET_SOURCES}
  ${PROJECT_DRIVER_DIR}/cmsis_system.c
  ${PROJECT_DRIVER_DIR}/startup.s
  ${PROJECT_DRIVER_DIR}/adc.cc
  ${PROJECT_DRIVER_DIR}/system.cc
  ${MDRIVLIB_TARGET_DIR}/drivers/interrupt_handler.cc
)
set(
  TARGET_INCLUDES
  ${TARGET_INCLUDES}
  ${PROJECT_DRIVER_DIR}
  ${MDRIVLIB_TARGET_DIR}
  ${MDRIVLIB_TARGET_DIR}/drivers
  stm32/device/${FAMILY_NAME}/Include
  stm32/HAL/${FAMILY_NAME}/Inc
)
set(
  TARGET_BOOTLOADER_SOURCES
  ${TARGET_BOOTLOADER_SOURCES}
  ${PROJECT_DRIVER_DIR}/cmsis_system.c
  ${PROJECT_DRIVER_DIR}/startup.s
  ${PROJECT_DRIVER_DIR}/system.cc
  ${MDRIVLIB_TARGET_DIR}/drivers/interrupt_handler.cc
)

# Create <TARGET>_ARCH: Interface library for defs/options common to all builds on this architecture
add_library(${TARGET}_ARCH INTERFACE)
target_compile_definitions(${TARGET}_ARCH INTERFACE ${COMMON_DEFINES} ${ARCH_DEFINES})
target_compile_options(${TARGET}_ARCH INTERFACE ${COMMON_COMPILE_OPTIONS} ${ARCH_FLAGS})
target_link_options(
  ${TARGET}_ARCH
  INTERFACE
  ${COMMON_LINK_OPTS}
  ${ARCH_FLAGS}
)

# Create main app elf file build target
add_executable(${TARGET}.elf ${COMMON_SOURCES} ${TARGET_SOURCES} ${HAL_SOURCES})
target_include_directories(${TARGET}.elf PRIVATE ${COMMON_INCLUDES} ${TARGET_INCLUDES})
target_link_script(${TARGET} ${TARGET_LINK_SCRIPT})
target_link_libraries(${TARGET}.elf PRIVATE ${TARGET}_ARCH)
add_bin_hex_command(${TARGET})

# Create bootloader elf file build target
add_executable(${TARGET}-bootloader.elf ${BOOTLOADER_COMMON_SOURCES} ${TARGET_BOOTLOADER_SOURCES} ${BOOTLOADER_HAL_SOURCES})
target_include_directories(${TARGET}-bootloader.elf PRIVATE ${BOOTLOADER_COMMON_INCLUDES} ${TARGET_INCLUDES})
target_link_script(${TARGET}-bootloader ${TARGET_BOOTLOADER_LINK_SCRIPT})
target_link_libraries(${TARGET}-bootloader.elf PRIVATE ${TARGET}_ARCH)
add_bin_hex_command(${TARGET}-bootloader)

# Create .wav file target for firmware upgrades
add_custom_target(${TARGET}.wav COMMAND export PYTHONPATH="${CMAKE_SOURCE_DIR}/src/bootloader" && ${WAV_ENCODE_PYTHON_CMD})

# Helper for letting lsp servers know what target we're using
add_custom_target(compdb COMMAND ln -snf ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/.)
