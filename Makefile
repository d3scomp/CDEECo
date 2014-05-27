# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME=cdeeco++

BUILD_DIR=build
SRC_DIR=src
CMSIS_DIR=stm32f40xx/CMSIS
PERIPH_DIR=stm32f40xx/STM32F4xx_StdPeriph_Driver
DISCOVERY_DIR=stm32f4discovery
FREERTOS_DIR=FreeRTOS
DRIVERS_DIR=${SRC_DIR}/drivers
CDEECO_DIR=${SRC_DIR}/cdeeco

OPENOCD_DIR=C:/Apps/openocd-0.7.0

# System
SRCS +=	${SRC_DIR}/main.cpp
SRCS += $(CDEECO_DIR)/FreeRTOSMutex.cpp

# Drivers
SRCS +=	${SRC_DIR}/drivers/LED.cpp
SRCS +=	${SRC_DIR}/drivers/gmd1602.cpp
SRCS +=	${SRC_DIR}/drivers/Timer.cpp
SRCS += ${SRC_DIR}/drivers/Console.cpp
SRCS += ${SRC_DIR}/drivers/UART.cpp
SRCS += ${SRC_DIR}/drivers/stm32f4xx_it.cpp
SRCS += ${SRC_DIR}/drivers/MRF24J40.cpp


# STM32F4 code
SRCS +=	${DISCOVERY_DIR}/system_stm32f4xx.c
SRCS +=	${DISCOVERY_DIR}/startup_stm32f40xx.s
SRCS +=	${PERIPH_DIR}/src/stm32f4xx_rcc.c
SRCS +=	${PERIPH_DIR}/src/stm32f4xx_exti.c
SRCS +=	${PERIPH_DIR}/src/stm32f4xx_gpio.c
SRCS +=	${PERIPH_DIR}/src/stm32f4xx_syscfg.c
SRCS +=	${PERIPH_DIR}/src/stm32f4xx_usart.c
SRCS +=	${PERIPH_DIR}/src/stm32f4xx_spi.c
SRCS +=	${PERIPH_DIR}/src/stm32f4xx_tim.c
SRCS +=	${PERIPH_DIR}/src/misc.c

# FreeRTOS sources
SRCS += $(FREERTOS_DIR)/Source/portable/MemMang/heap_3.c
SRCS += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F/port.c
SRCS += $(FREERTOS_DIR)/Source/list.c
SRCS += $(FREERTOS_DIR)/Source/queue.c
SRCS += $(FREERTOS_DIR)/Source/tasks.c
SRCS += $(FREERTOS_DIR)/Source/timers.c

# Normally you shouldn't need to change anything below this line!
#######################################################################################

CC=armv7m-hardfloat-eabi-gcc
CXX=armv7m-hardfloat-eabi-g++
OBJCOPY=armv7m-hardfloat-eabi-objcopy
SIZE=armv7m-hardfloat-eabi-size

# For some reason -O2 generates wrong code for interrupts. At least the USER_BUTTON interrupt handler get called twice
CFLAGS  = -mcpu=cortex-m4 -g -Og -Wall -pipe
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MMD -MP
CFLAGS += -I$(SRC_DIR)
CFLAGS += -I$(DRIVERS_DIR)
CFLAGS += -I$(DISCOVERY_DIR)
CFLAGS += -I$(CMSIS_DIR)/Include
CFLAGS += -I$(PERIPH_DIR)/inc
CFLAGS += -I$(FREERTOS_DIR)/Source/include
CFLAGS += -I$(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F

CPPFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -std=c++11

LDFLAGS = -Tstm32_flash.ld -Wl,-Map,$(BUILD_DIR)/cdeeco++.map $(CFLAGS)  

# Add startup file to build
OBJS = $(patsubst %.c,build/%.o,$(SRCS))
OBJS := $(patsubst %.cpp,build/%.o,$(OBJS))
OBJS := $(patsubst %.s,build/%.o,$(OBJS))

DEPS = $(patsubst %.c,build/%.d,$(SRCS))
DEPS := $(patsubst %.cpp,build/%.d,$(DEPS))
DEPS := $(patsubst %.s,,$(DEPS))


build/%.o: %.c
	${CC} $(CFLAGS) -c -o "$@" "$<"

build/%.o: %.cpp
	${CXX} $(CPPFLAGS) -c -o "$@" "$<"

build/%.o: %.s
	${CC} $(CFLAGS) -c -o "$@" "$<"

build/%.dep: %.cpp
	${CXX} -M $(CPPFLAGS) "$<" > "$@"

.PHONY: proj

all: init $(BUILD_DIR)/$(PROJ_NAME).elf

init:
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)/drivers
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)/cdeeco
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)/core
	mkdir -p $(BUILD_DIR)/$(DISCOVERY_DIR)
	mkdir -p $(BUILD_DIR)/$(PERIPH_DIR)/src
	mkdir -p $(BUILD_DIR)/$(FREERTOS_DIR)/Source
	mkdir -p $(BUILD_DIR)/$(FREERTOS_DIR)/Source/portable/MemMang
	mkdir -p $(BUILD_DIR)/$(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F

$(BUILD_DIR)/$(PROJ_NAME).elf: $(OBJS)
	${CXX} $(LDFLAGS) -o "$@" $(OBJS)
	${OBJCOPY} -O ihex "$@" $(BUILD_DIR)/$(PROJ_NAME).hex 
	${SIZE} --format=berkeley "$@"

clean:
	rm -rf build
	
openocd:
	openocd -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg

flash: all
	openocd -f board/stm32f4discovery.cfg -c "program $(BUILD_DIR)/$(PROJ_NAME).elf verify reset"

-include $(DEPS)