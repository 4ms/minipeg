cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi-gcc.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(
  minipeg
  C
  CXX
  ASM
)

add_subdirectory(src/f423-drivers f423)
add_subdirectory(src/f746-drivers f746)

# ############### Target-specific #####################

# set(TARGETLIST f423 f746)

# if(${TARGET} STREQUAL "f423") message("Configuring target STM32F423")

# set_hal_sources(HAL_SOURCES f4) set(PROJECT_DRIVER_DIR src/f423-drivers) set(MDRIVLIB_TARGET_DIR
# lib/mdrivlib/target/stm32f4xx) set(FAMILY_NAME stm32f4) set(TARGET_LINK_SCRIPT
# ${CMAKE_SOURCE_DIR}/linker/STM32F423VHHx_FLASH.ld)

# set(ARCH_FLAGS -mcpu=cortex-m4 -mfpu=fpv4-sp-d16) set(ARCH_DEFINES STM32F423xx STM32F4 ARM_MATH_CM4)

# set_bootloader_hal_sources(BOOTLOADER_HAL_SOURCES f4) set(TARGET_BOOTLOADER_SOURCES src/bootloader/gate_input_f423.cc
# src/f423-drivers/adc.cc stm32/HAL/stm32f4/Src/stm32f4xx_hal_adc.c stm32/HAL/stm32f4/Src/stm32f4xx_hal_dma.c )
# set(TARGET_BOOTLOADER_LINK_SCRIPT ${CMAKE_SOURCE_DIR}/linker/STM32F423VHHx_FLASH_BOOTLOADER.ld)
# set(WAV_ENCODE_PYTHON_CMD # Note: flash sector size is 128kB, but we arbitrarily limit app size with -g in order to
# make the # wave file shorter # cmake-format: off python
# ${CMAKE_SOURCE_DIR}/src/bootloader/stm_audio_bootloader/fsk/encoder.py -s 22050 -b 16 -n 8 -z 4 -p 256 -g 40960 -k
# 1000 $<TARGET_FILE_DIR:${TARGET}.elf>/${TARGET}.bin # cmake-format: on )

# elseif(${TARGET} STREQUAL "f746") message("Target is STM32F746")

# set_hal_sources(HAL_SOURCES f7) set(PROJECT_DRIVER_DIR src/f746-drivers) set(MDRIVLIB_TARGET_DIR
# lib/mdrivlib/target/stm32f7xx) set(FAMILY_NAME stm32f7) set(TARGET_LINK_SCRIPT
# ${CMAKE_SOURCE_DIR}/linker/STM32F746IEKx_FLASH.ld)

# set(ARCH_FLAGS -mcpu=cortex-m7 -mfpu=fpv5-sp-d16) set(ARCH_DEFINES STM32F746xx STM32F7 ARM_MATH_CM7)

# set_bootloader_hal_sources(BOOTLOADER_HAL_SOURCES f7) set(TARGET_BOOTLOADER_SOURCES src/bootloader/gate_input_f746.cc)
# set(TARGET_BOOTLOADER_LINK_SCRIPT ${CMAKE_SOURCE_DIR}/linker/STM32F746IEKx_FLASH_BOOTLOADER.ld)
# set(WAV_ENCODE_PYTHON_CMD # Note: flash sector size is 128kB, but we arbitrarily limit app size with -g, in order to
# make the # wave file shorter # cmake-format: off python
# ${CMAKE_SOURCE_DIR}/src/bootloader/stm_audio_bootloader/fsk/encoder.py -s 22050 -b 16 -n 8 -z 4 -p 256 -g 43520 -k
# 1000 $<TARGET_FILE_DIR:${TARGET}.elf>/${TARGET}.bin # cmake-format: on )

# else() message("Unknown TARGET ${TARGET}, run with `cmake -B build -DTARGET=[f423 | f746]`") endif()
