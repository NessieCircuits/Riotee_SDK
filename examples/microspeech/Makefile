RIOTEE_SDK_ROOT ?= ../..
GNU_INSTALL_ROOT ?=
PRJ_ROOT := .
OUTPUT_DIR := _build

# Size of the user stack in bytes. Must be multiple of 4.
RIOTEE_STACK_SIZE:= 8192
# Size of retained memory in bytes including STACK_SIZE.
RIOTEE_RAM_RETAINED_SIZE:= 65536

ifndef RIOTEE_SDK_ROOT
  $(error RIOTEE_SDK_ROOT is not set)
endif

SRC_FILES = \
  $(PRJ_ROOT)/src/models/audio_preprocessor_int8_model_data.cpp \
  $(PRJ_ROOT)/src/models/micro_speech_quantized_model_data.cpp \
  $(PRJ_ROOT)/src/inference.cpp \
  $(PRJ_ROOT)/src/main.cpp

INC_DIRS = \
  $(PRJ_ROOT)/include \
  $(PRJ_ROOT)/src

LIB_DIRS = $(RIOTEE_SDK_ROOT)/external/tflm-cmsis

LIB_FILES = -ltensorflow-microlite

CFLAGS += -DTF_LITE_STATIC_MEMORY

include $(RIOTEE_SDK_ROOT)/Makefile
