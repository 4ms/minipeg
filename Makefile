# Makefile by Dan Green <danngreen1@gmail.com>
binaryname 		= main

ifeq ($(MAKECMDGOALS),g431)
startup 	:= startup_stm32g431xx.s
system 		:= system_stm32g4xx.c
linkscript 	:= STM32G431C8Tx_FLASH.ld
chip_define	:= STM32G431xx
fam_define	:= STM32G4
shortchip	:= g431
cortexmath	:= ARM_MATH_CM4
devicefam 	:= stm32g4
devicename 	:= stm32g431
SOURCES 	= $(wildcard src/g431-drivers/*.c) \
			  $(wildcard src/g431-drivers/*.cc)

else ifeq ($(MAKECMDGOALS),f746)
startup 	:= startup_stm32f746xx.s
system 		:= system_stm32f7xx.c
linkscript 	:= STM32F746ZGTx_FLASH.ld
chip_define	:= STM32F746xx
fam_define	:= STM32F7
shortchip	:= f746
cortexmath	:= ARM_MATH_CM7
devicefam	:= stm32f7
devicename 	:= stm32f746
mdrivlibdir := lib/mdrivlib
INCLUDES =  -I$(mdrivlibdir) \
			-I$(mdrivlibdir)/drivers \
			-I$(mdrivlibdir)/target/stm32f7xx \
			-I$(mdrivlibdir)/target/stm32f7xx/drivers \
			-Ilib/cpputil 

SOURCES = 	$(mdrivlibdir)/drivers/pin.cc \
			$(mdrivlibdir)/drivers/timekeeper.cc \
			$(mdrivlibdir)/drivers/tim.cc \
			$(mdrivlibdir)/target/stm32f7xx/drivers/interrupt_handler.cc \
			$(wildcard src/f746-drivers/*.c) \
			$(wildcard src/f746-drivers/*.cc)

else ifeq ($(MAKECMDGOALS),clean)
else
$(error Build with `make g431` or `make f746`)
endif

cmsisdevicedir 	= stm32/device/$(devicename)
periphdir 		= stm32/HAL/$(devicefam)
coredir 		= stm32/CMSIS

builddir 		= build/$(shortchip)

SOURCES  += $(wildcard src/*.c)
SOURCES  += $(wildcard src/*.cc)
SOURCES  += $(wildcard src/hardware_tests/*.cc)
SOURCES  += $(wildcard libhwtests/src/*.cpp)
SOURCES  += $(wildcard libhwtests/src/*.cc)
SOURCES  += $(wildcard $(periphdir)/Src/*.c)
SOURCES  += src/$(shortchip)-drivers/$(startup)
SOURCES  += $(cmsisdevicedir)/Source/Templates/$(system)

OBJECTS = $(addprefix $(builddir)/, $(addsuffix .o, $(basename $(SOURCES))))
DEPS = $(OBJECTS:.o=.d)

INCLUDES += -I$(cmsisdevicedir)/Include \
			-I$(coredir)/Include \
			-I$(periphdir)/Inc \
			-I src \
			-I src/$(shortchip)-drivers \
			-I src/hardware_tests \
			-I libhwtests/inc \

elf 	= $(builddir)/$(binaryname).elf
hex 	= $(builddir)/$(binaryname).hex
bin 	= $(builddir)/$(binaryname).bin

ARCH 	= arm-none-eabi
CC 		= $(ARCH)-gcc
CXX 	= $(ARCH)-g++
LD 		= $(ARCH)-g++
AS 		= $(ARCH)-as
OBJCPY 	= $(ARCH)-objcopy
OBJDMP 	= $(ARCH)-objdump
GDB 	= $(ARCH)-gdb
SZ 		= $(ARCH)-size

cpu = -mcpu=cortex-m4
fpu = -mfpu=fpv4-sp-d16
floatabi = -mfloat-abi=hard
mcu = $(cpu) -mthumb $(fpu) $(floatabi)


arch_cflags = -D$(cortexmath) -D'__FPU_PRESENT=1' 
arch_cflags = -D$(chip_define) -D$(fam_define) -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER

optflag = -O3

CFLAGS = -g3 \
	$(arch_cflags) $(mcu) \
	-I. $(INCLUDES) \
	-fno-common \
	-fdata-sections -ffunction-sections \
	-specs=nano.specs \

DEPFLAGS = -MMD -MP -MF $(builddir)/$(basename $<).d

CXXFLAGS=$(CFLAGS) \
	-std=c++17 \
	-fno-rtti \
	-fno-exceptions \
	-ffreestanding \
	-Werror=return-type \
	-Wdouble-promotion \
	-Wno-register \


AFLAGS = $(mcu)
#	-x assembler-with-cpp

LDSCRIPT = $(cmsisdevicedir)/$(linkscript)

LFLAGS =  -Wl,-Map,build/main.map,--cref \
	-Wl,--gc-sections \
	$(mcu) -specs=nano.specs  -T $(LDSCRIPT)

#-----------------------------------
# Uncomment to compile unoptimized:

# $(builddir)/src/main.o: optflag = -O0
# $(builddir)/src/params.o: optflag = -O0
# $(builddir)/src/envelope_calcs.o: optflag = -O0
# $(builddir)/src/envelope_out.o: optflag = -O0
# $(builddir)/src/dac.o: optflag = -O0
# $(builddir)/src/debounced_digins.o: optflag = -O0
# $(builddir)/src/dig_inouts.o: optflag = -O0
# $(builddir)/src/env_transition.o: optflag = -O0
# $(builddir)/src/analog_conditioning.o: optflag = -O0
# $(builddir)/src/adc.o: optflag = -O0
# $(builddir)/src/hardware_tests/%.o: optflag = -O0
# $(builddir)/src/dac.o: optflag = -O0
# $(builddir)/libhwtests/%.o: optflag = -O0
# $(builddir)/src/leds.o: optflag = -O0
# $(builddir)/src/pwm.o: optflag = -O0
# $(builddir)/src/debounced_digins.o: optflag = -O0
# $(builddir)/src/flash_user.o: optflag = -O0
# $(builddir)/src/flash.o: optflag = -O0
# $(builddir)/$(periphdir)/Src/%.o: optflag = -O0

g431: all

f746: all

all: Makefile $(bin) $(hex)

$(bin): $(elf)
	$(OBJCPY) -O binary $< $@
	$(OBJDMP) -x --syms $< > $(addsuffix .dmp, $(basename $<))
	ls -l $@ $<

$(hex): $(elf)
	$(OBJCPY) --output-target=ihex $< $@
	$(SZ) -d $(elf)

$(elf): $(OBJECTS)
	@echo "Linking..."
	@$(LD) $(LFLAGS) -o $@ $(OBJECTS)

$(builddir)/%.o: %.c $(builddir)/%.d
	@mkdir -p $(dir $@)
	@echo "Compiling $< at $(optflag)"
	@$(CC) -c $(DEPFLAGS) $(optflag) $(CFLAGS) $< -o $@

$(builddir)/%.o: %.cpp $(builddir)/%.d
	@mkdir -p $(dir $@)
	@echo "Compiling $< at $(optflag)"
	@$(CXX) -c $(DEPFLAGS) $(optflag) $(CXXFLAGS) $< -o $@

$(builddir)/%.o: %.cc $(builddir)/%.d
	@mkdir -p $(dir $@)
	@echo "Compiling $< at $(optflag)"
	@$(CXX) -c $(DEPFLAGS) $(optflag) $(CXXFLAGS) $< -o $@

$(builddir)/%.o: %.s
	mkdir -p $(dir $@)
	$(AS) $(AFLAGS) $< -o $@ > $(addprefix $(builddir)/, $(addsuffix .lst, $(basename $<)))

flash: $(bin)
	st-flash write $(bin) 0x8000000

clean:
	rm -rf $(builddir)

#-include $(wildcard $(builddir)/*.d)
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
			-I$(cmsisdevicedir)/Include \

TEST_CFLAGS = -D$(chip_define) -D$(fam_define)


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

