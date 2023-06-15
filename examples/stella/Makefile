PREFIX := "$(GNU_INSTALL_ROOT)"arm-none-eabi-
SDK_ROOT ?= ../..
PRJ_ROOT := .
OUTPUT_DIR := _build

ifndef SDK_ROOT
  $(error SDK_ROOT is not set)
endif

SRC_FILES = \
  $(PRJ_ROOT)/src/main.c

include $(SDK_ROOT)/Makefile