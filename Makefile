# Makefile by Dan Green <danngreen1@gmail.com>
#

BINARYNAME 		= main

STARTUP 		= startup_stm32g070xx.s
SYSTEM 			= system_stm32g0xx.c
LOADFILE 		= STM32G070RBTx_FLASH.ld

DEVICE 			= stm32/device
CORE 			= stm32/CMSIS
PERIPH 			= stm32/HAL

BUILDDIR 		= build

SOURCES  += $(wildcard $(PERIPH)/Src/*.c)
SOURCES  += $(DEVICE)/Source/Temaplates/gcc/$(STARTUP)
SOURCES  += $(DEVICE)/Source/Templates/$(SYSTEM)
SOURCES  += $(wildcard src/*.c)
# SOURCES  += $(wildcard src/*.cc)

OBJECTS   = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -I$(DEVICE)/Include \
			-I$(CORE)/Include \
			-I$(PERIPH)/Inc \
			-I src \

ELF 	= $(BUILDDIR)/$(BINARYNAME).elf
HEX 	= $(BUILDDIR)/$(BINARYNAME).hex
BIN 	= $(BUILDDIR)/$(BINARYNAME).bin

ARCH 	= arm-none-eabi
CC 		= $(ARCH)-gcc
LD 		= $(ARCH)-gcc
AS 		= $(ARCH)-as
OBJCPY 	= $(ARCH)-objcopy
OBJDMP 	= $(ARCH)-objdump
GDB 	= $(ARCH)-gdb
SZ 		= $(ARCH)-size

SZOPTS 	= -d

CPU = -mcpu=cortex-m0plus
MCU = $(CPU) -mthumb  

# ARCH_CFLAGS = -DARM_MATH_CM7 -D'__FPU_PRESENT=1' -DUSE_HAL_DRIVER -DSTM32F730xx
ARCH_CFLAGS = -DSTM32G070xx -DUSE_HAL_DRIVER

OPTFLAG = -O3

CFLAGS = -g2 -fno-common \
	$(ARCH_CFLAGS) $(MCU) \
	-I. $(INCLUDES) \
	-fdata-sections -ffunction-sections \
	-specs=nano.specs \

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
$(BUILDDIR)/$(PERIPH)/Src/%.o: OPTFLAG = -O0

all: Makefile $(BIN) $(HEX)

$(BIN): $(ELF)
	$(OBJCPY) -O binary $< $@
	$(OBJDMP) -x --syms $< > $(addsuffix .dmp, $(basename $<))
	ls -l $@ $<


$(HEX): $(ELF)
	$(OBJCPY) --output-target=ihex $< $@
	$(SZ) $(SZOPTS) $(ELF)

$(ELF): $(OBJECTS) 
	$(LD) $(LFLAGS) -o $@ $(OBJECTS)


$(BUILDDIR)/%.o: %.c $(wildcard src/*.h) $(wildcard src/drivers/*.h)
	mkdir -p $(dir $@)
	$(CC) -c $(OPTFLAG) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.cc $(wildcard src/*.h) $(wildcard src/drivers/*.h)
	mkdir -p $(dir $@)
	$(CC) -c $(OPTFLAG) $(CXXFLAGS) $< -o $@


$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(AS) $(AFLAGS) $< -o $@ > $(addprefix $(BUILDDIR)/, $(addsuffix .lst, $(basename $<)))


flash: $(BIN)
	st-flash write $(BIN) 0x8000000

clean:
	rm -rf build
