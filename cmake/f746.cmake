enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(ELF ${PROJECT_NAME}.elf)

# Directories with source files shared between projects
set(THIRDPARTY_DIR ../../third-party)
set(SHARED_DIR ../shared)

# Defines and flags
set(DEFAULT_DEFINES
	-DUSE_FULL_LL_DRIVER
	-DSTM32MP157Cxx
	-DSTM32MP1
	-DCORE_CA7
	)

set(MCU_FLAGS 
 	-mcpu=cortex-m7
	-mfpu=fpv4-sp-d16
	-mlittle-endian
	-mfloat-abi=hard
	-mthumb
	)

set(ARCH_FLAGS
	-DARM_MATH_CM7
	-D'__FPU_PRESENT=1'
	-DSTM32F746xx
	-DSTM32F7
	-DUSE_HAL_DRIVER 
	-DUSE_FULL_LL_DRIVER
	)

set(DEFAULT_COMPILE_OPTIONS
	${MCU_FLAGS}
	${ARCH_FLAGS}
	-fno-common
	-fdata-sections
	-ffunction-sections
	-ffreestanding
	-nostartfiles
	-nostdlib
	-g3
	$<$<COMPILE_LANGUAGE:CXX>:@${PROJECT_SOURCE_DIR}/../../cmake/gcc-compile-options-cxx.txt>
	$<$<CONFIG:Debug>:-O0>
	$<$<CONFIG:Release>:-O3>

	)
