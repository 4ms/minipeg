# Makefile by Dan Green <danngreen1@gmail.com>
BINARYNAME 		= main

ifeq ($(MAKECMDGOALS),g431)
STARTUP 	:= startup_stm32g431xx.s
SYSTEM 		:= system_stm32g4xx.c
LOADFILE 	:= STM32G431C8Tx_FLASH.ld
CHIP 		:= STM32G431xx
CORTEXMATH	:= ARM_MATH_CM4
HALDIR 		:= stm32g4
CMSISDIR 	:= stm32g431
else ifeq ($(MAKECMDGOALS),f746)
STARTUP 	:= startup_stm32f746xx.s
SYSTEM 		:= system_stm32f7xx.c
LOADFILE 	:= STM32F746ZGTx_FLASH.ld
CHIP 		:= STM32F746xx
CORTEXMATH	:= ARM_MATH_CM7
HALDIR	 	:= stm32f7
CMSISDIR 	:= stm32f746
else
$(error Build with `make g431` or `make f746`)
endif

DEVICE 			= stm32/device/$(CMSISDIR)
CORE 			= stm32/CMSIS
PERIPH 			= stm32/HAL/$(HALDIR)

BUILDDIR 		= build

SOURCES  += $(wildcard src/*.c)
SOURCES  += $(wildcard src/*.cc)
SOURCES  += $(wildcard src/hardware_tests/*.cc)
SOURCES  += $(wildcard libhwtests/src/*.cpp)
SOURCES  += $(wildcard libhwtests/src/*.cc)
SOURCES  += $(wildcard $(PERIPH)/Src/*.c)
SOURCES  += $(DEVICE)/Source/Templates/gcc/$(STARTUP)
SOURCES  += $(DEVICE)/Source/Templates/$(SYSTEM)

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))
DEPS = $(OBJECTS:.o=.d)

INCLUDES += -I$(DEVICE)/Include \
			-I$(CORE)/Include \
			-I$(PERIPH)/Inc \
			-I src \
			-I src/hardware_tests \
			-I libhwtests/inc \

ELF 	= $(BUILDDIR)/$(BINARYNAME).elf
HEX 	= $(BUILDDIR)/$(BINARYNAME).hex
BIN 	= $(BUILDDIR)/$(BINARYNAME).bin

ARCH 	= arm-none-eabi
CC 		= $(ARCH)-gcc
CXX 	= $(ARCH)-g++
LD 		= $(ARCH)-g++
AS 		= $(ARCH)-as
OBJCPY 	= $(ARCH)-objcopy
OBJDMP 	= $(ARCH)-objdump
GDB 	= $(ARCH)-gdb
SZ 		= $(ARCH)-size

SZOPTS 	= -d

CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)


ARCH_CFLAGS = -D$(CORTEXMATH) -D'__FPU_PRESENT=1' 
ARCH_CFLAGS = -D$(CHIP) -DUSE_HAL_DRIVER

OPTFLAG = -O3

CFLAGS = -g3 \
	$(ARCH_CFLAGS) $(MCU) \
	-I. $(INCLUDES) \
	-fno-common \
	-fdata-sections -ffunction-sections \
	-specs=nano.specs \

DEPFLAGS = -MMD -MP -MF $(BUILDDIR)/$(basename $<).d

CXXFLAGS=$(CFLAGS) \
	-std=c++17 \
	-fno-rtti \
	-fno-exceptions \
	-ffreestanding \
	-Werror=return-type \
	-Wdouble-promotion \
	-Wno-register \


AFLAGS = $(MCU)
#	-x assembler-with-cpp

LDSCRIPT = $(DEVICE)/$(LOADFILE)

LFLAGS =  -Wl,-Map,build/main.map,--cref \
	-Wl,--gc-sections \
	$(MCU) -specs=nano.specs  -T $(LDSCRIPT)

#-----------------------------------
# Uncomment to compile unoptimized:

# build/src/main.o: OPTFLAG = -O0
# build/src/params.o: OPTFLAG = -O0
# build/src/envelope_calcs.o: OPTFLAG = -O0
# build/src/envelope_out.o: OPTFLAG = -O0
# build/src/dac.o: OPTFLAG = -O0
# build/src/debounced_digins.o: OPTFLAG = -O0
# build/src/dig_inouts.o: OPTFLAG = -O0
# build/src/env_transition.o: OPTFLAG = -O0
# build/src/analog_conditioning.o: OPTFLAG = -O0
# build/src/adc.o: OPTFLAG = -O0
# build/src/hardware_tests/%.o: OPTFLAG = -O0
# build/src/dac.o: OPTFLAG = -O0
# build/libhwtests/%.o: OPTFLAG = -O0
# build/src/leds.o: OPTFLAG = -O0
# build/src/pwm.o: OPTFLAG = -O0
# build/src/debounced_digins.o: OPTFLAG = -O0
# build/src/flash_user.o: OPTFLAG = -O0
# build/src/flash.o: OPTFLAG = -O0
# $(BUILDDIR)/$(PERIPH)/Src/%.o: OPTFLAG = -O0

g431: all

f746: all

all: Makefile $(BIN) $(HEX)

$(BIN): $(ELF)
	$(OBJCPY) -O binary $< $@
	$(OBJDMP) -x --syms $< > $(addsuffix .dmp, $(basename $<))
	ls -l $@ $<

$(HEX): $(ELF)
	$(OBJCPY) --output-target=ihex $< $@
	$(SZ) $(SZOPTS) $(ELF)

$(ELF): $(OBJECTS)
	@echo "Linking..."
	@$(LD) $(LFLAGS) -o $@ $(OBJECTS)

$(BUILDDIR)/%.o: %.c $(BUILDDIR)/%.d
	@mkdir -p $(dir $@)
	@echo "Compiling $< at $(OPTFLAG)"
	@$(CC) -c $(DEPFLAGS) $(OPTFLAG) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.cpp $(BUILDDIR)/%.d
	@mkdir -p $(dir $@)
	@echo "Compiling $< at $(OPTFLAG)"
	@$(CXX) -c $(DEPFLAGS) $(OPTFLAG) $(CXXFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.cc $(BUILDDIR)/%.d
	@mkdir -p $(dir $@)
	@echo "Compiling $< at $(OPTFLAG)"
	@$(CXX) -c $(DEPFLAGS) $(OPTFLAG) $(CXXFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(AS) $(AFLAGS) $< -o $@ > $(addprefix $(BUILDDIR)/, $(addsuffix .lst, $(basename $<)))

flash: $(BIN)
	st-flash write $(BIN) 0x8000000

clean:
	rm -rf $(BUILDDIR)

#-include $(wildcard $(BUILDDIR)/*.d)
%.d: ;

ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif

TESTFW_DIR = ../Unity/src
TEST_DIR = tests
TEST_BUILD_DIR_NAME = build
TEST_BUILD_DIR = $(TEST_DIR)/$(TEST_BUILD_DIR_NAME)

TESTFW_SRC = unity.c

TEST_SOURCES =  \
				tests_main.c \
				test_flash_user.c \
				mocks/mock_flash.c \


TESTEE_DIR = src
TESTEE_SOURCES = flash.c \

TESTFW_OBJ = $(TEST_BUILD_DIR)/$(addsuffix .o, $(basename $(TESTFW_SRC)))

TEST_OBJECTS = $(TESTFW_OBJ) \
				$(addprefix $(TEST_BUILD_DIR)/, $(addsuffix .o, $(basename $(TEST_SOURCES))))

TESTEE_OBJECTS = $(addprefix $(TEST_BUILD_DIR)/, $(addsuffix .o, $(basename $(TESTEE_SOURCES))))


TEST_INC =  -I$(TESTFW_DIR) \
			-I$(TEST_DIR) \
			-I$(DEVICE)/Include \

TEST_CFLAGS = -D$(CHIP)


$(TESTFW_OBJ): $(TESTFW_DIR)/$(TESTFW_SRC)
	mkdir -p $(dir $@)
	gcc $(TEST_INC) -c $< -o $@

$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(TESTFW_OBJ)
	mkdir -p $(dir $@)
	gcc $(TEST_INC) $(TEST_CFLAGS) -c $< -o $@

$(TESTEE_OBJECTS): $(TESTEE_DIR)/$(TESTEE_SOURCES)
	mkdir -p $(dir $@)
	gcc $(TEST_INC) $(TEST_CFLAGS) -c $< -o $@


# TEST_OBJECTS = $(addprefix $(TEST_BUILD_DIR)/, $(addsuffix .o, $(basename $(SOURCES))))

test: $(TEST_OBJECTS) $(TESTEE_OBJECTS)
	gcc -o $(TEST_BUILD_DIR)/do_test $(TEST_OBJECTS) $(TESTEE_OBJECTS)
	$(TEST_BUILD_DIR)/do_test

