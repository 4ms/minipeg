# ############### Common #####################

set(root ${CMAKE_SOURCE_DIR})

set(
  COMMON_SOURCES
  ${root}/lib/mdrivlib/drivers/pin.cc
  ${root}/lib/mdrivlib/drivers/tim.cc
  ${root}/lib/mdrivlib/drivers/timekeeper.cc
    ${root}/lib/libhwtests/src/AdcChecker.cc
    ${root}/lib/libhwtests/src/AdcRangeChecker.cc
    ${root}/lib/libhwtests/src/ButtonChecker.cc
    ${root}/lib/libhwtests/src/CodecCallbacks.cc
    ${root}/lib/libhwtests/src/GateInChecker.cc
    ${root}/lib/libhwtests/src/GateOutChecker.cc
    ${root}/lib/libhwtests/src/GateOutput.cc
    ${root}/lib/libhwtests/src/LEDTester.cc
    ${root}/src/analog_conditioning.cc
    ${root}/src/calibration.cc
    ${root}/src/dig_inouts.cc
    ${root}/src/env_transition.cc
    ${root}/src/env_update.cc
    ${root}/src/envelope_calcs.cc
    ${root}/src/flash_user.cc
    ${root}/src/hardware_tests/hardware_test_adc.cc
    ${root}/src/hardware_tests/hardware_test_dac.cc
    ${root}/src/hardware_tests/hardware_test_gates.cc
    ${root}/src/hardware_tests/hardware_test_leds_buttons.cc
    ${root}/src/hardware_tests/hardware_test_util.cc
    ${root}/src/hardware_tests/hardware_tests.cc
    ${root}/src/leds.cc
    ${root}/src/libc_stub.c
    ${root}/src/libcpp_stub.cc
    ${root}/src/main.cc
    ${root}/src/params.cc
    ${root}/src/pingable_env.cc
    ${root}/src/shareddrv/dac.cc
    ${root}/src/shareddrv/debounced_digins.cc
    ${root}/src/shareddrv/flash.cc
    ${root}/src/shareddrv/pwm.cc
    ${root}/src/system_mode.cc
    ${root}/src/timers.cc
    ${root}/src/trigout.cc
)

set(COMMON_INCLUDES
    ${root}/src
    ${root}/src/hardware_tests
    ${root}/lib/libhwtests/inc
    ${root}/stm32/CMSIS/Include
    ${root}/lib/mdrivlib
    ${root}/lib/mdrivlib/drivers
    ${root}/lib/cpputil
)

function(set_hal_sources sources family)
  set(${sources}
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_adc.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_adc_ex.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_cortex.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_dac.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_dma.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_exti.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_flash.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_flash_ex.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_gpio.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_pwr.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_pwr_ex.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_rcc.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_rcc_ex.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_tim.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_ll_tim.c
      PARENT_SCOPE
  )
endfunction()

function(set_bootloader_hal_sources sources family)
  set(${sources}
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_cortex.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_flash.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_flash_ex.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_gpio.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_pwr.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_pwr_ex.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_rcc.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_rcc_ex.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_tim.c
      ${root}/stm32/HAL/stm32${family}/Src/stm32${family}xx_ll_tim.c
      PARENT_SCOPE
  )
endfunction()

set(BOOTLOADER_COMMON_SOURCES
    ${root}/src/bootloader/bootloader.cc
    ${root}/src/bootloader/leds.cc
    ${root}/src/bootloader/animation.cc
    ${root}/src/bootloader/bl_utils.cc
    ${root}/src/bootloader/stm_audio_bootloader/fsk/packet_decoder.cc
    ${root}/src/libc_stub.c
    ${root}/src/libcpp_stub.cc
    ${root}/src/shareddrv/flash.cc
    ${root}/lib/mdrivlib/drivers/pin.cc
    ${root}/lib/mdrivlib/drivers/timekeeper.cc
    ${root}/lib/mdrivlib/drivers/tim.cc
)

set(BOOTLOADER_COMMON_INCLUDES
    ${root}/stm32/CMSIS/Include
    ${root}/src/bootloader
    ${root}/src/bootloader/stmlib
    ${root}/src
    ${root}/lib/mdrivlib
    ${root}/lib/mdrivlib/drivers
    ${root}/lib/cpputil
)

set(COMMON_DEFINES USE_HAL_DRIVER USE_FULL_LL_DRIVER)

set(COMMON_COMPILE_OPTIONS
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
    -Wl,-Map,$<TARGET_FILE_DIR:${target_base}.elf>/${target_base}.map,--cref
    -T
    ${link_script}
    ${ARGN}
  )
  set_target_properties(${target_base}.elf PROPERTIES LINK_DEPENDS ${link_script})
endfunction()

function(add_bin_hex_command target_base)
  set(BASENAME $<TARGET_FILE_DIR:${target_base}.elf>/${target_base})
  add_custom_command(
    TARGET ${target_base}.elf
    POST_BUILD
    COMMAND echo "Binaries are in $<TARGET_FILE_DIR:${target_base}.elf>"
    COMMAND echo "Target filename is $<TARGET_FILE:${target_base}.elf>"
    COMMAND arm-none-eabi-objcopy -O ihex $<TARGET_FILE:${target_base}.elf> ${BASENAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary $<TARGET_FILE:${target_base}.elf> ${BASENAME}.bin
  )
  set_target_properties(
    ${target_base}.elf
    PROPERTIES
    ADDITIONAL_CLEAN_FILES
    "${BASE_NAME}.hex;${BASE_NAME}.bin;${BASE_NAME}.map"
  )
endfunction()

function(
  set_target_sources_includes
  project_driver_dir
  mdrivlib_target_dir
  family_name
)
  string(TOLOWER ${family_name} family_name)

  # Add target-specific project files and paths:
  set(TARGET_SOURCES
      ${TARGET_SOURCES}
      ${project_driver_dir}/cmsis_system.c
      ${project_driver_dir}/startup.s
      ${project_driver_dir}/adc.cc
      ${project_driver_dir}/system.cc
      ${mdrivlib_target_dir}/drivers/interrupt_handler.cc
      PARENT_SCOPE
  )
  set(TARGET_INCLUDES
      ${TARGET_INCLUDES}
      ${project_driver_dir}
      ${mdrivlib_target_dir}
      ${mdrivlib_target_dir}/drivers
      ${root}/stm32/device/${family_name}/Include
      ${root}/stm32/HAL/${family_name}/Inc
      PARENT_SCOPE
  )
  set(TARGET_BOOTLOADER_SOURCES
      ${TARGET_BOOTLOADER_SOURCES}
      ${project_driver_dir}/cmsis_system.c
      ${project_driver_dir}/startup.s
      ${project_driver_dir}/system.cc
      ${mdrivlib_target_dir}/drivers/interrupt_handler.cc
      PARENT_SCOPE
  )

  string(
    REGEX MATCH
          "^stm32([fghlmuw]p?[0-9bl])_?(m0plus|m4|m7)?"
          family_name
          ${family_name}
  )
  set(short_family_name ${CMAKE_MATCH_1})

  set_hal_sources(HAL_SOURCES ${short_family_name})
  set(HAL_SOURCES
      ${HAL_SOURCES}
      PARENT_SCOPE
  )
  set_bootloader_hal_sources(BOOTLOADER_HAL_SOURCES ${short_family_name})
  set(BOOTLOADER_HAL_SOURCES
      ${BOOTLOADER_HAL_SOURCES}
      PARENT_SCOPE
  )
endfunction()

function(create_target target)
  message("Creating target ${target}")

  # Create <target>_ARCH: Interface library for defs/options common to all builds on this architecture
  add_library(${target}_ARCH INTERFACE)
  target_compile_definitions(${target}_ARCH INTERFACE ${COMMON_DEFINES} ${ARCH_DEFINES})
  target_compile_options(${target}_ARCH INTERFACE ${COMMON_COMPILE_OPTIONS} ${ARCH_FLAGS})
  target_link_options(
    ${target}_ARCH
    INTERFACE
    ${COMMON_LINK_OPTS}
    ${ARCH_FLAGS}
  )

  # Create main app elf file build target
  add_executable(${target}.elf ${COMMON_SOURCES} ${TARGET_SOURCES} ${HAL_SOURCES})
  target_include_directories(${target}.elf PRIVATE ${COMMON_INCLUDES} ${TARGET_INCLUDES})
  target_link_script(${target} ${TARGET_LINK_SCRIPT})
  target_link_libraries(${target}.elf PRIVATE ${target}_ARCH)
  add_bin_hex_command(${target})

  # Create bootloader elf file build target
  add_executable(
    ${target}-bootloader.elf ${BOOTLOADER_COMMON_SOURCES} ${TARGET_BOOTLOADER_SOURCES} ${BOOTLOADER_HAL_SOURCES}
  )
  target_include_directories(${target}-bootloader.elf PRIVATE ${BOOTLOADER_COMMON_INCLUDES} ${TARGET_INCLUDES})
  target_link_script(${target}-bootloader ${TARGET_BOOTLOADER_LINK_SCRIPT})
  target_link_libraries(${target}-bootloader.elf PRIVATE ${target}_ARCH)
  add_bin_hex_command(${target}-bootloader)

  # Create .wav file target for firmware upgrades
  add_custom_target(
    ${target}.wav
    DEPENDS ${target}.elf
    COMMAND export PYTHONPATH="${CMAKE_SOURCE_DIR}/src/bootloader" && ${WAV_ENCODE_PYTHON_CMD}
  )

  # Helper for letting lsp servers know what target we're using
  add_custom_target(${target}-compdb COMMAND ln -snf ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/.)

endfunction()
