PREFIX := "$(GNU_INSTALL_ROOT)"arm-none-eabi-
SDK_ROOT ?= ../..
PRJ_ROOT := .
OUTPUT_DIR := _build

ifndef SDK_ROOT
  $(error SDK_ROOT is not set)
endif

SRC_FILES = \
  $(PRJ_ROOT)/src/led.cpp \
  $(PRJ_ROOT)/src/main.cpp

INC_FOLDERS = \
  $(PRJ_ROOT)/include

include $(SDK_ROOT)/Makefile