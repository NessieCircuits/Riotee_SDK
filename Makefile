PREFIX := "$(GNU_INSTALL_ROOT)"arm-none-eabi-
CORE_DIR := $(RIOTEE_SDK_ROOT)/core
DRIVER_DIR := $(RIOTEE_SDK_ROOT)/drivers
RTOS_DIR := $(RIOTEE_SDK_ROOT)/external/freertos
NRFX_DIR := $(RIOTEE_SDK_ROOT)/external/nrfx
CMSIS_DIR := $(RIOTEE_SDK_ROOT)/external/CMSIS_5
TFLITE_DIR := $(RIOTEE_SDK_ROOT)/external/tflm-cmsis
LINKER_SCRIPT:= $(RIOTEE_SDK_ROOT)/linker.ld
NRF_DEV_NUM := 52833

RIOTEE_STACK_SIZE ?= 2048
RIOTEE_RAM_RETAINED_SIZE ?= 8192

SDK_SRC_FILES += \
  $(CORE_DIR)/startup.c \
  $(CORE_DIR)/syscalls.c \
  $(CORE_DIR)/thresholds.c \
  $(CORE_DIR)/printf.c \
  $(CORE_DIR)/radio.c \
  $(CORE_DIR)/ble.c \
  $(CORE_DIR)/i2c.c \
  $(CORE_DIR)/max20361.c \
  $(CORE_DIR)/am1805.c \
  $(CORE_DIR)/timing.c \
  $(CORE_DIR)/gpio.c \
  $(CORE_DIR)/uart.c \
  $(CORE_DIR)/spic.c \
  $(CORE_DIR)/runtime.c \
	$(CORE_DIR)/nvm.c \
	$(CORE_DIR)/adc.c \
	$(CORE_DIR)/stella.c \
	$(DRIVER_DIR)/shtc3.c \
	$(DRIVER_DIR)/vm1010.c \
  $(RTOS_DIR)/queue.c \
  $(RTOS_DIR)/list.c \
  $(RTOS_DIR)/tasks.c \
  $(RTOS_DIR)/event_groups.c \
  $(RTOS_DIR)/portable/GCC/ARM_CM4F/port.c


OBJS = $(addprefix $(OUTPUT_DIR)/, $(addsuffix .o, $(SRC_FILES))) 
OBJS += $(subst $(RIOTEE_SDK_ROOT)/,$(OUTPUT_DIR)/, $(addsuffix .o, $(SDK_SRC_FILES)))


# Include folders common to all targets
INC_DIRS += \
  $(CORE_DIR) \
  $(CORE_DIR)/include \
  $(DRIVER_DIR)/include \
  $(RTOS_DIR)/include \
  $(RTOS_DIR)/portable/GCC/ARM_CM4F \
  $(NRFX_DIR) \
  $(NRFX_DIR)/hal \
  $(NRFX_DIR)/mdk \
  $(NRFX_DIR)/templates \
  $(CMSIS_DIR)/CMSIS/Core/Include \
  $(TFLITE_DIR) \
  $(TFLITE_DIR)/third_party/flatbuffers/include \
  $(TFLITE_DIR)/third_party/gemmlowp \
  $(TFLITE_DIR)/third_party/kissfft \
  $(TFLITE_DIR)/third_party/ruy \


INCLUDES = $(INC_DIRS:%=-I%)

LIBS = $(LIB_DIRS:%=-L%)

OPT ?= -O3 -g3

CFLAGS += ${INCLUDES}
CFLAGS += $(OPT)
# used to pass in defines from command line
CFLAGS += $(USER_DEFINES)
CFLAGS += -DNRF${NRF_DEV_NUM}_XXAA
CFLAGS += -DRIOTEE_STACK_SIZE=${RIOTEE_STACK_SIZE}
CFLAGS += -DARM_MATH_CM4
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DTF_LITE_STATIC_MEMORY
CFLAGS += -Wall
CFLAGS += -fno-builtin
CFLAGS += -mthumb
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mabi=aapcs
CFLAGS += -mfloat-abi=hard
CFLAGS += -mfpu=fpv4-sp-d16
CFLAGS += -fsingle-precision-constant
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections

CPPFLAGS = ${CFLAGS} -fno-exceptions

ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF${NRF_DEV_NUM}_XXAA

LDFLAGS += $(OPT)
LDFLAGS += -T$(LINKER_SCRIPT)
LDFLAGS += -mthumb -mabi=aapcs
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += $(LIBS)
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections,-Map=${OUTPUT_DIR}/build.map
# use newlib in nano version and system call stubs
LDFLAGS += --specs=nano.specs
LDFLAGS += -Wl,--defsym=RIOTEE_RAM_RETAINED_SIZE=${RIOTEE_RAM_RETAINED_SIZE}

LIB_FILES += -lm

ARFLAGS = -rcs

.PHONY: clean flash erase app

all: app

app: ${OUTPUT_DIR}/build.hex

${OUTPUT_DIR}/%.c.o: ${RIOTEE_SDK_ROOT}/%.c
	@mkdir -p $(@D)
	@${PREFIX}gcc ${CFLAGS} -c $< -o $@
	@echo "CC $<"

${OUTPUT_DIR}/%.c.o: ${PRJ_ROOT}/%.c
	@mkdir -p $(@D)
	@${PREFIX}gcc ${CFLAGS} -c $< -o $@
	@echo "CC $<"

${OUTPUT_DIR}/%.cpp.o: ${PRJ_ROOT}/%.cpp
	@mkdir -p $(@D)
	@${PREFIX}c++ ${CPPFLAGS} -c $< -o $@
	@echo "CC $<"

${OUTPUT_DIR}/build.elf: $(OBJS)
	@${PREFIX}c++ ${LDFLAGS} $(OBJS) -o $@ ${LIB_FILES}
	@${PREFIX}size $@


${OUTPUT_DIR}/build.hex: ${OUTPUT_DIR}/build.elf
	@echo "Preparing $@"
	@${PREFIX}objcopy -O ihex $< $@


clean:
	rm -rf _build/*

# Flash the program
flash: ${OUTPUT_DIR}/build.hex
	@echo Flashing: $<
	riotee-probe program -d nrf52 -f $<
