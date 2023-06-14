PREFIX := "$(GNU_INSTALL_ROOT)"arm-none-eabi-
PROJ_DIR := ./
OUTPUT_DIR := _build
SRC_DIR := src
DRIVER_DIR := drivers
RTOS_DIR := freertos
NRFX_DIR := nrfx
CMSIS_DIR := CMSIS_5
LINKER_SCRIPT:= linker.ld
NRF_DEV_NUM ?= 52833


LIB_SRC_FILES += \
  $(SRC_DIR)/startup.c \
  $(SRC_DIR)/thresholds.c \
  $(SRC_DIR)/printf.c \
  $(SRC_DIR)/radio.c \
  $(SRC_DIR)/ble.c \
  $(SRC_DIR)/i2c.c \
  $(SRC_DIR)/max20361.c \
  $(SRC_DIR)/am1805.c \
  $(SRC_DIR)/timing.c \
  $(SRC_DIR)/gpint.c \
  $(SRC_DIR)/uart.c \
  $(SRC_DIR)/spic.c \
  $(SRC_DIR)/runtime.c \
	$(SRC_DIR)/nvm.c \
	$(SRC_DIR)/adc.c \
	$(SRC_DIR)/stella.c \
	$(DRIVER_DIR)/shtc3.c \
  $(RTOS_DIR)/queue.c \
  $(RTOS_DIR)/list.c \
  $(RTOS_DIR)/tasks.c \
  $(RTOS_DIR)/event_groups.c \
  $(RTOS_DIR)/portable/GCC/ARM_CM4F/port.c

APP_SRC_FILES += \
  $(SRC_DIR)/main.c

APP_OBJS = $(addprefix $(OUTPUT_DIR)/, $(addsuffix .o, $(APP_SRC_FILES)))
LIB_OBJS = $(addprefix $(OUTPUT_DIR)/, $(addsuffix .o, $(LIB_SRC_FILES)))

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/include \
  $(DRIVER_DIR) \
  $(RTOS_DIR)/include \
  $(RTOS_DIR)/portable/GCC/ARM_CM4F \
  $(NRFX_DIR) \
  $(NRFX_DIR)/hal \
  $(NRFX_DIR)/mdk \
  $(NRFX_DIR)/templates \
  $(CMSIS_DIR)/CMSIS/Core/Include

INCLUDES = $(INC_FOLDERS:%=-I%)

OPT = -O3 -g3

CFLAGS = ${INCLUDES}
CFLAGS += $(OPT)
CFLAGS += -DNRF${NRF_DEV_NUM}_XXAA
CFLAGS += -DARM_MATH_CM4
CFLAGS += -DFLOAT_ABI_HARD
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

CPPFLAGS = ${CFLAGS}

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
LDFLAGS += -L$(OUTPUT_DIR)
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections,-Map=${OUTPUT_DIR}/build.map
# use newlib in nano version and system call stubs
LDFLAGS += --specs=nano.specs

LIB_FILES += -lm -lriotee

ARFLAGS = -rcs

.PHONY: clean flash erase lib app

all: lib app

lib: ${OUTPUT_DIR}/libriotee.a
app: ${OUTPUT_DIR}/build.hex


${OUTPUT_DIR}/%.c.o: %.c
	@mkdir -p $(@D)
	@${PREFIX}gcc ${CFLAGS} -c $< -o $@
	@echo "CC $<"

${OUTPUT_DIR}/%.c.o: %.cpp
	@mkdir -p $(@D)
	@${PREFIX}c++ ${CPPFLAGS} -c $< -o $@
	@echo "CC $<"

${OUTPUT_DIR}/build.elf: $(APP_OBJS) ${OUTPUT_DIR}/libriotee.a
	@${PREFIX}c++ ${LDFLAGS} $^ -o $@ ${LIB_FILES}
	@${PREFIX}size $@

${OUTPUT_DIR}/libriotee.a: $(LIB_OBJS)
	@echo "Preparing $@"
	@${PREFIX}ar ${ARFLAGS} $@ $^

${OUTPUT_DIR}/build.hex: ${OUTPUT_DIR}/build.elf
	@echo "Preparing $@"
	@${PREFIX}objcopy -O ihex $< $@


clean:
	rm -rf _build/*

# Flash the program
flash: ${OUTPUT_DIR}/build.hex
	@echo Flashing: $<
	pyocd load -t nrf52 $<
