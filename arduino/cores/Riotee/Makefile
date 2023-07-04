PREFIX := "$(GNU_INSTALL_ROOT)"arm-none-eabi-
SDK_ROOT ?= /home/kai/workspace/Riotee_Runtime
PRJ_ROOT := .
OUTPUT_DIR := _build

ifndef SDK_ROOT
  $(error SDK_ROOT is not set)
endif

SRC_FILES = \
  $(PRJ_ROOT)/main.cpp \
  $(PRJ_ROOT)/wiring_digital.c \
  $(PRJ_ROOT)/wiring_analog.c \
  $(PRJ_ROOT)/external/ArduinoCore-API/api/Common.cpp \
  $(PRJ_ROOT)/external/ArduinoCore-API/api/IPAddress.cpp \
  $(PRJ_ROOT)/external/ArduinoCore-API/api/Print.cpp \
  $(PRJ_ROOT)/external/ArduinoCore-API/api/String.cpp \
  $(PRJ_ROOT)/Serial.cpp \
  $(PRJ_ROOT)/../../libraries/Wire/Wire.cpp \
  $(PRJ_ROOT)/test.cpp

INC_FOLDERS = \
  $(PRJ_ROOT)/external/ArduinoCore-API/api \
  $(PRJ_ROOT)/../../libraries/Wire

include $(SDK_ROOT)/Makefile