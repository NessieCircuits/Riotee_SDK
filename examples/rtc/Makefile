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
  $(PRJ_ROOT)/include

#CFLAGS = -DDISABLE_CAP_MONITOR

include $(RIOTEE_SDK_ROOT)/Makefile