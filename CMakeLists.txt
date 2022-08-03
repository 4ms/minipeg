cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi-gcc.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(minipeg C CXX ASM)

## TODO: Consider this method
# cmake -B build -DTARGET=f423
# Then we use ${TARGET} everywhere instead of f423.elf
# and we use if(${TARGET} EQUAL f423) \ set(arch_sources f423-drivers/system.cc etc..


set(
  COMMON_SOURCES
  lib/mdrivlib/drivers/pin.cc
  lib/mdrivlib/drivers/tim.cc
  lib/mdrivlib/drivers/timekeeper.cc
  libhwtests/src/AdcChecker.cc
  libhwtests/src/AdcRangeChecker.cc
  libhwtests/src/ButtonChecker.cc
  libhwtests/src/CodecCallbacks.cc
  libhwtests/src/GateInChecker.cc
  libhwtests/src/GateOutChecker.cc
  libhwtests/src/GateOutput.cc
  libhwtests/src/LEDTester.cc
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
  libhwtests/inc
  stm32/CMSIS/Include
  ./
  lib/mdrivlib
  lib/mdrivlib/drivers
  lib/cpputil
)

function(hal_sources sources family)
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

function(bootloader_hal_sources sources family)
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
  src/bootloader/gate_input.cc
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

set(COMMON_LINK_OPTS -Wl,--gc-sections -nostdlib -mthumb -mfloat-abi=hard)

# ############### Common commands #####################

function(target_link_script target_base link_script)
  target_link_options(
    ${target_base}.elf
    PRIVATE
    -Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${target_base}.map,--cref
    -T ${link_script}
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
    COMMAND arm-none-eabi-objcopy -O ihex $<TARGET_FILE_NAME:${target_base}.elf>
    $<TARGET_FILE_DIR:${target_base}.elf>/${target_base}.hex
    COMMAND arm-none-eabi-objcopy -O binary $<TARGET_FILE_NAME:${target_base}.elf>
    $<TARGET_FILE_DIR:${target_base}.elf>/${target_base}.bin
  )
  set_target_properties(
    ${target_base}.elf
    PROPERTIES
    ADDITIONAL_CLEAN_FILES
    "$<TARGET_FILE_DIR:${target_base}.elf>/${target_base}.hex;$<TARGET_FILE_DIR:${target_base}.elf>/${target_base}.bin;f423.map"
  )
endfunction()


# ########### f423 ##############


# Common f423 settings are in the f423 interface library
#TODO: this for each target:
set(F423_ARCH_FLAGS -mcpu=cortex-m4 -mfpu=fpv4-sp-d16)
add_library(f423 INTERFACE)
target_compile_definitions(f423 INTERFACE ${COMMON_DEFINES} STM32F423xx STM32F4 ARM_MATH_CM4)
target_compile_options(f423 INTERFACE ${COMMON_COMPILE_OPTIONS} ${F423_ARCH_FLAGS})
target_link_options(f423 INTERFACE ${COMMON_LINK_OPTS} ${F423_ARCH_FLAGS})

if(${TARGET} STREQUAL "f423")
  message("Target is F423")

  hal_sources(F4_HAL_SOURCES f4)
  set(
    TARGET_SOURCES
    lib/mdrivlib/target/stm32f4xx/drivers/interrupt_handler.cc
    src/f423-drivers/adc.cc
    src/f423-drivers/system.cc
    src/f423-drivers/startup_stm32f423xx.s
    stm32/device/stm32f4/Source/Templates/system_stm32f4xx.c
    ${F4_HAL_SOURCES}
  )
  set(
    TARGET_INCLUDES
    stm32/device/stm32f4/Include
    stm32/HAL/stm32f4/Inc
    src/f423-drivers
    lib/mdrivlib/target/stm32f4xx
    lib/mdrivlib/target/stm32f4xx/drivers
  )
  set(
    TARGET_LINK_SCRIPT
    ${CMAKE_SOURCE_DIR}/stm32/device/stm32f4/STM32F423VHHx_FLASH.ld
  )

  bootloader_hal_sources(F4_BOOTLOADER_HAL_SOURCES f4)
  set(
    TARGET_BOOTLOADER_SOURCES
    lib/mdrivlib/target/stm32f4xx/drivers/interrupt_handler.cc
    src/f423-drivers/system.cc
    src/f423-drivers/adc.cc
    src/analog_conditioning.cc
    stm32/HAL/stm32f4/Src/stm32f4xx_hal_adc.c
    stm32/HAL/stm32f4/Src/stm32f4xx_hal_dma.c
    src/f423-drivers/startup_stm32f423xx.s
    stm32/device/stm32f4/Source/Templates/system_stm32f4xx.c
    ${F4_BOOTLOADER_HAL_SOURCES}
  )
  set(
    TARGET_BOOTLOADER_INCLUDES
    stm32/device/stm32f4/Include
    stm32/HAL/stm32f4/Inc
    src/f423-drivers
    lib/mdrivlib/target/stm32f4xx
    lib/mdrivlib/target/stm32f4xx/drivers
  )
  set(
    TARGET_BOOTLOADER_LINK_SCRIPT
    ${CMAKE_SOURCE_DIR}/stm32/device/stm32f4/STM32F423VHHx_FLASH_BOOTLOADER.ld
  )
else()
  message("Unknown TARGET ${TARGET}, run with `cmake -B build -DTARGET=f423` or other valid target")
endif()

add_executable(${TARGET}.elf ${COMMON_SOURCES} ${TARGET_SOURCES})
target_include_directories(${TARGET}.elf PRIVATE ${COMMON_INCLUDES} ${TARGET_INCLUDES})
target_link_script(${TARGET} ${TARGET_LINK_SCRIPT})
target_link_libraries(${TARGET}.elf PRIVATE ${TARGET})
add_bin_hex_command(${TARGET})

add_executable(${TARGET}-bootloader.elf ${BOOTLOADER_COMMON_SOURCES} ${TARGET_BOOTLOADER_SOURCES})
target_include_directories(${TARGET}-bootloader.elf PRIVATE ${BOOTLOADER_COMMON_INCLUDES} ${TARGET_BOOTLOADER_INCLUDES})
target_link_script(${TARGET}-bootloader.elf ${TARGET_BOOTLOADER_LINK_SCRIPT})
target_link_libraries(f423-bootloader.elf PRIVATE f423)

# TODO: different parameters for each target?
add_custom_target(
  ${TARGET}.wav
  COMMAND
  export PYTHONPATH="${CMAKE_SOURCE_DIR}/src/bootloader" && python
  ${CMAKE_SOURCE_DIR}/src/bootloader/stm_audio_bootloader/fsk/encoder.py -s 22050 -b 16 -n 8 -z 4 -p 256 -g 65536
  -k 1000 $<TARGET_FILE_DIR:${TARGET}.elf>/${TARGET}.bin
  #Note: flash sector size is 128kB, but we arbitrarily limit app size to 64kB (-g 65536), in order to make the wave file shorter
)

add_custom_target(make_compdb COMMAND ln -snf ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/.)
