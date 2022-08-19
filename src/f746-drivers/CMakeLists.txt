cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi-gcc.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(
  f746
  C
  CXX
  ASM
)

message("Configuring f746")
include(../../cmake/common.cmake)

set(target f746)
set(FAMILY_NAME stm32f7)
set(PROJECT_DRIVER_DIR ../../src/f746-drivers)
set(MDRIVLIB_TARGET_DIR ../../lib/mdrivlib/target/stm32f7xx)
set(TARGET_LINK_SCRIPT ${CMAKE_SOURCE_DIR}/linker/STM32F746IEKx_FLASH.ld)
set(ARCH_FLAGS -mcpu=cortex-m7 -mfpu=fpv5-sp-d16)
set(ARCH_DEFINES STM32F746xx STM32F7 ARM_MATH_CM7)

set(
  TARGET_BOOTLOADER_SOURCES
  ../../src/bootloader/gate_input_f746.cc
)
set(TARGET_BOOTLOADER_LINK_SCRIPT ${CMAKE_SOURCE_DIR}/linker/STM32F746IEKx_FLASH_BOOTLOADER.ld)

set(
  WAV_ENCODE_PYTHON_CMD
  # Note: flash sector size is 128kB, but we arbitrarily limit app size with -g in order to
  # make the  wave file shorter
  # cmake-format: off
  python ${CMAKE_SOURCE_DIR}/src/bootloader/stm_audio_bootloader/fsk/encoder.py -s 22050 -b 16 -n 6 -z 3 -p 256 -g 50000 -k 1000 $<TARGET_FILE_DIR:${target}.elf>/${target}.bin
  # python ${CMAKE_SOURCE_DIR}/src/bootloader/stm_audio_bootloader/fsk/encoder.py -s 22050 -b 16 -n 8 -z 4 -p 256 -g 40960 -k 1000 $<TARGET_FILE_DIR:${target}.elf>/${target}.bin
  # cmake-format: on
)

set_target_sources_includes(${PROJECT_DRIVER_DIR} ${MDRIVLIB_TARGET_DIR} ${FAMILY_NAME})
create_target(${target})
