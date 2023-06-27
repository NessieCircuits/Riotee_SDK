RIOTEE_SDK_ROOT ?= ../..
GNU_INSTALL_ROOT ?=

PRJ_ROOT := .
OUTPUT_DIR := _build

ifndef RIOTEE_SDK_ROOT
  $(error RIOTEE_SDK_ROOT is not set)
endif

SRC_FILES = \
  $(PRJ_ROOT)/src/main.c

INC_DIRS = \
  $(PRJ_ROOT)/include \
  $(PRJ_ROOT)/include/cmsis-dsp \


LIB_DIRS = \
  $(PRJ_ROOT)

LIB_FILES = -larm_cortexM4lf_math

include $(RIOTEE_SDK_ROOT)/Makefile