cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi-gcc.cmake)
project(minipeg C CXX ASM)

set (COMMON_SOURCES 
	src/analog_conditioning.cc
	./src/calibration.cc
	./src/dig_inouts.cc
	./src/env_transition.cc
	./src/env_update.cc
	./src/envelope_calcs.cc
	./src/flash_user.cc
	./src/leds.cc
	./src/libc_stub.c
	./src/libcpp_stub.cc
	./src/main.cc
	./src/params.cc
	./src/timers.cc
	./src/trigout.cc
	./src/pingable_env.cc
	./src/system_mode.cc
	./src/hardware_tests/hardware_test_adc.cc
	./src/hardware_tests/hardware_test_dac.cc
	./src/hardware_tests/hardware_test_gates.cc
	./src/hardware_tests/hardware_test_leds_buttons.cc
	./src/hardware_tests/hardware_test_util.cc
	./src/hardware_tests/hardware_tests.cc
	./libhwtests/src/AdcChecker.cc
	./libhwtests/src/AdcRangeChecker.cc
	./libhwtests/src/ButtonChecker.cc
	./libhwtests/src/CodecCallbacks.cc
	./libhwtests/src/GateInChecker.cc
	./libhwtests/src/GateOutChecker.cc
	./libhwtests/src/GateOutput.cc
	./libhwtests/src/LEDTester.cc
	./lib/mdrivlib/drivers/pin.cc
	./lib/mdrivlib/drivers/timekeeper.cc
	./lib/mdrivlib/drivers/tim.cc
	./src/shareddrv/dac.cc
	./src/shareddrv/debounced_digins.cc
	./src/shareddrv/flash.cc
	./src/shareddrv/pwm.cc
	)	

set(COMMON_INCLUDES
	./src
	./src/hardware_tests
	./libhwtests/inc
	./stm32/CMSIS/Include
	)

function(hal_sources sources family)
	set(${sources} 
		./stm32/HAL/stm32${family}/Src/stm32f4xx_hal.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_adc.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_adc_ex.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_cortex.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_dac.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_dma.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_exti.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_flash.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_flash_ex.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_gpio.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_pwr.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_pwr_ex.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_rcc.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_rcc_ex.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_hal_tim.c
		./stm32/HAL/stm32${family}/Src/stm32${family}xx_ll_tim.c
		PARENT_SCOPE
		)
endfunction()
hal_sources(F4_HAL_SOURCES f4)
hal_sources(F7_HAL_SOURCES f7)
hal_sources(G4_HAL_SOURCES g4)

set(COMMON_DEFINES
	USE_HAL_DRIVER
	USE_FULL_LL_DRIVER
	)

set(COMMON_COMPILE_OPTIONS
	-g3
	-std=c++2a
	-fdata-sections
	-ffunction-sections
	-fno-common
	-ffreestanding
	-fno-exceptions
	-fno-rtti
	-fno-unwind-tables
	-fno-threadsafe-statics
	-mfloat-abi=hard
	-mthumb
	-nostartfiles
	-nostdlib
	-Wdouble-promotion
	-Werror=return-type
	-Wno-register
	-Wno-volatile
	$<$<CONFIG:Debug>:-O0>
	$<$<CONFIG:Release>:-O3>
	)

set(COMMON_LINK_OPTS
	-Wl,--gc-sections
	-nostdlib
	-mthumb
	-mfloat-abi=hard
	)

add_executable(f423.elf
	${COMMON_SOURCES}
	./lib/mdrivlib/target/stm32f4xx/drivers/interrupt_handler.cc
	./src/f423-drivers/adc.cc
	./src/f423-drivers/system.cc
	./src/f423-drivers/startup_stm32f423xx.s
	./stm32/device/stm32f4/Source/Templates/system_stm32f4xx.c
	${F4_HAL_SOURCES}
	)

target_include_directories(f423.elf PRIVATE
	${COMMON_INCLUDES}
	./stm32/device/stm32f4/Include
	./stm32/HAL/stm32f4/Inc
	./src/f423-drivers
	)

target_compile_definitions(f423.elf PRIVATE
	${COMMON_DEFINES}
	STM32F423xx
	STM32F4
	ARM_MATH_CM4
	)

target_compile_options(f423.elf PRIVATE
	${COMMON_COMPILE_OPTIONS}
	-mcpu=cortex-m4
	-mfpu=fpv4-sp-d16
	)

target_link_options(f423.elf PRIVATE
	${COMMON_LINK_OPTS}
	-mcpu=cortex-m4
	-mfpu=fpv4-sp-d16
	-Wl,-Map,build/f423.map,--cref
	-T stm32/device/stm32f4/STM32F423VHHx_FLASH.ld
	)

add_custom_command(TARGET f423.elf 
	POST_BUILD
	COMMAND arm-none-eabi-objcopy --output-target=ihex f423.elf f423.hex
	COMMAND arm-none-eabi-objcopy -O binary f423.elf f423.bin
	)

set_target_properties(f423.elf PROPERTIES ADDITIONAL_CLEAN_FILES
	"f423.bin;f423.hex;f423.map"
	)
