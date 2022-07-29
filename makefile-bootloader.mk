# Makefile by Dan Green <danngreen1@gmail.com>
binaryname 		= bootloader

ifeq ($(MAKECMDGOALS),g431)
linkscript 	:= STM32G431C8Tx_FLASH_BOOTLOADER.ld
chip_define	:= STM32G431xx
fam_define	:= STM32G4
devicefam 	:= stm32g4
shortchip	:= g431
shortfam    := g4
cortexmath	:= ARM_MATH_CM4
mcuflags 	:= -mcpu=cortex-m4 \
			   -mthumb \
			   -mfpu=fpv4-sp-d16 \
			   -mfloat-abi=hard
SOURCES 	= $(wildcard src/g431-drivers/*.c) \
			  $(wildcard src/g431-drivers/*.cc)

else ifeq ($(MAKECMDGOALS),f746)
linkscript 	:= STM32F746ZGTx_FLASH_BOOTLOADER.ld
chip_define	:= STM32F746xx
fam_define	:= STM32F7
devicefam	:= stm32f7
shortchip	:= f746
shortfam    := f7
cortexmath	:= ARM_MATH_CM7
mcuflags 	:= -mcpu=cortex-m7 \
			   -mthumb \
			   -mfpu=fpv5-sp-d16 \
			   -mfloat-abi=hard
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
			src/f746-drivers/system.cc \
			src/shareddrv/flash.cc

else ifeq ($(MAKECMDGOALS),f423)
linkscript 	:= STM32F423VHHx_FLASH_BOOTLOADER.ld
chip_define	:= STM32F423xx
fam_define	:= STM32F4
devicefam	:= stm32f4
shortchip	:= f423
shortfam    := f4
cortexmath	:= ARM_MATH_CM4
mcuflags 	:= -mcpu=cortex-m4 \
			   -mthumb \
			   -mfpu=fpv4-sp-d16 \
			   -mfloat-abi=hard
mdrivlibdir := lib/mdrivlib
INCLUDES =  -I$(mdrivlibdir) \
			-I$(mdrivlibdir)/drivers \
			-I$(mdrivlibdir)/target/stm32f4xx \
			-I$(mdrivlibdir)/target/stm32f4xx/drivers \
			-Ilib/cpputil 
SOURCES = 	$(mdrivlibdir)/drivers/pin.cc \
			$(mdrivlibdir)/drivers/timekeeper.cc \
			$(mdrivlibdir)/drivers/tim.cc \
			$(mdrivlibdir)/target/stm32f4xx/drivers/interrupt_handler.cc \
			src/f423-drivers/system.cc \
			src/shareddrv/flash.cc
else ifeq ($(MAKECMDGOALS),clean)
else
$(error Build with `make g431` or `make f746` or `make f423`)
endif

cmsisdevicedir 	= stm32/device/$(devicefam)
periphdir 		= stm32/HAL/$(devicefam)
coredir 		= stm32/CMSIS
builddir 		= build/$(shortchip)

SOURCES  += src/bootloader/bootloader.cc
SOURCES  += src/bootloader/leds.cc
SOURCES  += src/bootloader/animation.cc
SOURCES  += src/bootloader/bl_utils.cc
SOURCES  += src/bootloader/stm_audio_bootloader/fsk/packet_decoder.cc

SOURCES  += src/$(shortchip)-drivers/startup_stm32$(shortchip)xx.s
SOURCES  += $(cmsisdevicedir)/Source/Templates/system_$(devicefam)xx.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_hal.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_hal_cortex.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_hal_flash.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_hal_flash_ex.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_hal_gpio.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_hal_pwr.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_hal_pwr_ex.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_hal_rcc.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_hal_rcc_ex.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_hal_tim.c
SOURCES  += $(periphdir)/Src/stm32$(shortfam)xx_ll_tim.c
SOURCES  += src/libc_stub.c
SOURCES  += src/libcpp_stub.cc

OBJECTS = $(addprefix $(builddir)/, $(addsuffix .o, $(basename $(SOURCES))))
DEPS = $(OBJECTS:.o=.d)

INCLUDES += -I$(cmsisdevicedir)/Include \
			-I$(coredir)/Include \
			-I$(periphdir)/Inc \
			-I src \
			-I src/bootloader \
			-I src/$(shortchip)-drivers \
			-I src/bootloader/stmlib \

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


arch_cflags = -D$(cortexmath) -D'__FPU_PRESENT=1' 
arch_cflags = -D$(chip_define) -D$(fam_define) -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER

optflag = -O3

CFLAGS = -g3 \
	$(arch_cflags) $(mcuflags) \
	-I. $(INCLUDES) \
	-fno-common \
	-fdata-sections -ffunction-sections \
	-nostdlib \

DEPFLAGS = -MMD -MP -MF $(builddir)/$(basename $<).d

CXXFLAGS=$(CFLAGS) \
	-std=c++17 \
	-fno-rtti \
	-fno-exceptions \
	-ffreestanding \
	-Werror=return-type \
	-Wdouble-promotion \
	-Wno-register \


AFLAGS = $(mcuflags)
#	-x assembler-with-cpp

LDSCRIPT = $(cmsisdevicedir)/$(linkscript)

LFLAGS =  	-Wl,-Map,build/main.map,--cref \
			-Wl,--gc-sections \
			$(mcuflags) \
			-T $(LDSCRIPT) \
			-nostdlib \

#-----------------------------------
# Uncomment to compile unoptimized:

# $(builddir)/src/bootloader/bootloader.o: optflag = -O0

#TODO: change the g431 etc targets to build like this;
# g431: $(g431-bin) $(g431-hex)
#
# all: g431 f746 f423

g431: all

f746: all

f423: all

all: Makefile $(bin) $(hex)

$(bin): $(elf)
	$(OBJCPY) -O binary $< $@
	$(OBJDMP) -x --syms $< > $(addsuffix .dmp, $(basename $<))
	ls -l $@ $<

%.hex : %.elf
	$(OBJCPY) --output-target=ihex $< $@
	$(SZ) -d $<

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

