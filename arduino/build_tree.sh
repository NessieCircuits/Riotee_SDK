#!/bin/bash

# Copy all required files from git submodules into core tree. This keeps the
# core size small and prevents Arduino from trying to compile everything.

SCRIPT_PATH=$(dirname "$SCRIPT")
RIOTEE_SDK_ROOT=${SCRIPT_PATH}/../
EXTERNAL_PATH=${RIOTEE_SDK_ROOT}/external

copy_paths=(
    ArduinoCore-API/api
    CMSIS_5/CMSIS/Core/Include
    nrfx/nrfx.h
    nrfx/hal
    nrfx/soc
    nrfx/templates
    nrfx/drivers/include
    nrfx/drivers/nrfx_common.h
    nrfx/drivers/nrfx_errors.h
    nrfx/mdk/nrf.h
    nrfx/mdk/nrf52833.h
    nrfx/mdk/system_nrf52833.h
    nrfx/mdk/nrf52833_bitfields.h
    nrfx/mdk/nrf52_to_nrf52833.h
    nrfx/mdk/nrf51_to_nrf52.h
    nrfx/mdk/nrf_peripherals.h
    nrfx/mdk/nrf52833_peripherals.h
    nrfx/mdk/compiler_abstraction.h
    freertos/list.c
    freertos/queue.c
    freertos/tasks.c
    freertos/event_groups.c
    freertos/portable/GCC/ARM_CM4F
    freertos/include
)

# Cleanup
rm -rf ${SCRIPT_PATH}/cores/Riotee/external/*
mkdir -p ${SCRIPT_PATH}/cores/Riotee/external/

cp -r ${RIOTEE_SDK_ROOT}/core ${SCRIPT_PATH}/cores/Riotee/external/
cp -r ${RIOTEE_SDK_ROOT}/drivers ${SCRIPT_PATH}/cores/Riotee/external/
cp -r ${RIOTEE_SDK_ROOT}/linker.ld ${SCRIPT_PATH}/cores/Riotee/external/linker.ld

# Copy files, creating paths as necessary
for item in "${copy_paths[@]}"
do
	src_path="${EXTERNAL_PATH}/${item}"
	dst_path="${SCRIPT_PATH}/cores/Riotee/external/${item}"
    dst_dir="$(dirname -- $dst_path)"
    mkdir -p $dst_dir
    cp -r $src_path $dst_path
done