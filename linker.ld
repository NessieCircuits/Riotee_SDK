/* This linker script must first run through gcc preprocessor */

#if __has_include("riotee_config.h")
#include "riotee_config.h"
#endif

#ifndef RIOTEE_RAM_RETAINED_SIZE
#define RIOTEE_RAM_RETAINED_SIZE 8192
#endif

SEARCH_DIR(.)

ENTRY(c_startup)
EXTERN(vectors)

/* 128k total RAM */
RAM_SIZE = 131072;
RAM_ORIGIN = 0x20000000;

RAM_VOLATILE_SIZE = RAM_SIZE - RIOTEE_RAM_RETAINED_SIZE;
RAM_VOLATILE_ORIGIN = RAM_ORIGIN + RIOTEE_RAM_RETAINED_SIZE;

MEMORY {
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 512k
  RAM_RETAINED (rwx) :  ORIGIN = 0x20000000, LENGTH = RIOTEE_RAM_RETAINED_SIZE
  RAM (rwx) :  ORIGIN = RAM_VOLATILE_ORIGIN, LENGTH = RAM_VOLATILE_SIZE
}


SECTIONS {
        .text :
        {
                KEEP(*(.isr_vector))
                *(.text*)

                KEEP(*(.init))
                KEEP(*(.fini))

                /* .ctors */
                *crtbegin.o(.ctors)
                *crtbegin?.o(.ctors)
                *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
                *(SORT(.ctors.*))
                *(.ctors)

                /* .dtors */
                *crtbegin.o(.dtors)
                *crtbegin?.o(.dtors)
                *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
                *(SORT(.dtors.*))
                *(.dtors)

                *(.rodata*)

                KEEP(*(.eh_frame*))

        } > FLASH

        .ARM.extab :
        {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        } > FLASH

        __exidx_start = .;
        .ARM.exidx :
        {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        } > FLASH
        __exidx_end = .;

        . = ALIGN(4);
        __etext = .;

        .data : {
                /* Exclude all system variables from retained data */
                . = ALIGN(4);
                __data_start__ = .;
                *(.volatile.data)
                *runtime.c.o(.data .data.*)
                *tasks.c.o(.data .data.*)
                *port.c.o(.data .data.*)
                *ble.c.o(.data .data.*)
                *radio.c.o(.data .data.*)
                *stella.c.o(.data .data.*)
                *shtc.c.o(.data .data.*)
                *i2c.c.o(.data .data.*)
                *spic.c.o(.data .data.*)
                *adc.c.o(.data .data.*)
                *nvm.c.o(.data .data.*)
                *bma400.c.o(.data .data.*)
                *gpint.c.o(.data .data.*)
                *(vtable)
                *lib_a-impure.o(.data .data.*)
                *lib_a-__call_atexit.o(.data .data.*)

                . = ALIGN(4);
                KEEP(*(.igot.plt))
                . = ALIGN(4);
                /* preinit data */
                PROVIDE_HIDDEN (__preinit_array_start = .);
                KEEP(*(.preinit_array))
                PROVIDE_HIDDEN (__preinit_array_end = .);

                . = ALIGN(4);
                /* init data */
                PROVIDE_HIDDEN (__init_array_start = .);
                KEEP(*(SORT(.init_array.*)))
                KEEP(*(.init_array))
                PROVIDE_HIDDEN (__init_array_end = .);


                . = ALIGN(4);
                /* finit data */
                PROVIDE_HIDDEN (__fini_array_start = .);
                KEEP(*(SORT(.fini_array.*)))
                KEEP(*(.fini_array))
                PROVIDE_HIDDEN (__fini_array_end = .);

                KEEP(*(.jcr*))
                /* All data end */
                __data_end__ = .;

        } >RAM AT>FLASH

        .data_retained : {
                . = ALIGN(4);
                __data_retained_start__ = .;
                *(.data)
                *(.data.*)
                __data_retained_end__ = .;
        } >RAM_RETAINED AT>FLASH

        .bss (NOLOAD) : {
                /* Exclude all system variables from retained data */
                . = ALIGN(4);
                __bss_start__ = .;
                *(.volatile_bss)
                *runtime.c.o(.bss .bss.*)
                *tasks.c.o(.bss .bss.*)
                *port.c.o(.bss .bss.*)
                *ble.c.o(.bss .bss.*)
                *radio.c.o(.bss .bss.*)
                *stella.c.o(.bss .bss.*)
                *shtc.c.o(.bss .bss.*)
                *i2c.c.o(.bss .bss.*)
                *spic.c.o(.bss .bss.*)
                *adc.c.o(.bss .bss.*)
                *nvm.c.o(.bss .bss.*)
                *bma400.c.o(.bss .bss.*)
                *gpint.c.o(.bss .bss.*)
                *crtbegin.o(.bss .bss.*)
                *lib_a-reent.o(.bss .bss.*)
                *lib_a-lock.o(.bss .bss.*)
                /* Stores pointers to teardown functions */
               . = ALIGN(4);
                __teardown_start__ = .;
                *(.teardown)
                __teardown_end__ = .;
                __bss_end__ = .;
        } >RAM

        .bss_retained (NOLOAD) : {
                . = ALIGN(4);
                __bss_retained_start__ = .;
                *(COMMON)
                *(.bss)
                *(.retained_bss)
                *(.bss.*)
                __bss_retained_end__ = .;
        } >RAM_RETAINED

    
}
