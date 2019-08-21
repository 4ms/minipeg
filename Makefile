SRCS  = $(wildcard src/*.c)
BUILDDIR = build
PROJ_NAME=$(BUILDDIR)/main
STD_PERIPH_LIB=stm32
LDSCRIPT_INC=stm32/
STD_PERIPH_SRCS=$(STD_PERIPH_LIB)/std_periph/src

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump
SIZE=arm-none-eabi-size

CFLAGS  = -Wall -g -std=c99 
CFLAGS += -O0
CFLAGS += -mlittle-endian -mcpu=cortex-m0  -march=armv6-m -mthumb
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wl,--gc-sections -Wl,-Map=$(PROJ_NAME).map
CFLAGS += --specs=nano.specs

vpath %.c src

ROOT=$(shell pwd)

CFLAGS += -I inc -I $(STD_PERIPH_LIB) -I $(STD_PERIPH_LIB)/CMSIS/Device/ST/STM32F0xx/Include
CFLAGS += -I $(STD_PERIPH_LIB)/CMSIS/Include -I $(STD_PERIPH_LIB)/std_periph/inc
CFLAGS += -include $(STD_PERIPH_LIB)/stm32f0xx_conf.h 

SRCS += stm32/startup_stm32f0xx.s
SRCS += $(STD_PERIPH_SRCS)/stm32f0xx_adc.c \
	$(STD_PERIPH_SRCS)/stm32f0xx_dma.c \
	$(STD_PERIPH_SRCS)/stm32f0xx_flash.c \
	$(STD_PERIPH_SRCS)/stm32f0xx_gpio.c \
	$(STD_PERIPH_SRCS)/stm32f0xx_misc.c \
	$(STD_PERIPH_SRCS)/stm32f0xx_rcc.c  \
	$(STD_PERIPH_SRCS)/stm32f0xx_syscfg.c \
	$(STD_PERIPH_SRCS)/stm32f0xx_tim.c

OBJS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SRCS))))

.PHONY: all

all: $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $^ -o $@ -L$(LDSCRIPT_INC) -Tstm32f030k6.ld
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
	$(OBJDUMP) -St $(PROJ_NAME).elf >$(PROJ_NAME).lst
	$(SIZE) $(PROJ_NAME).elf

program: $(PROJ_NAME).bin
	st-flash --reset write ${PROJ_NAME}.bin 0x08000000
	
clean:
	find ./ -name '*~' | xargs rm -f
	rm -f $(BUILDDIR)/*.o
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
	rm -f $(PROJ_NAME).map
	rm -f $(PROJ_NAME).lst
