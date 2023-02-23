/* details on syntax can be found in GCC linker manual */

SEARCH_DIR(.)

ENTRY(c_startup)
EXTERN(vectors)

MEMORY {
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 512k
  RAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 256k
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

        .data : AT ( __etext ) {         /* AT specifies the LMA (load memory address) */
                /* Exclude all system variables from retained data */
                . = ALIGN(4);
                __data_start__ = .;
                *runtime.o(.data .data.*)
                *tasks.o(.data .data.*)
                *port.o(.data .data.*)
                *ble.o(.data .data.*)
                *radio.o(.data .data.*)
                *shtc.o(.data .data.*)
                *i2c.o(.data .data.*)
                *nvm.o(.data .data.*)
                *bma400.o(.data .data.*)
                *gpint.o(.data .data.*)
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
                . = ALIGN(4);
                __data_retained_start__ = .;
                *(.data)
                *(.data.*)
                __data_retained_end__ = .;
        } >RAM

        .bss : {
                /* Exclude all system variables from retained data */
                . = ALIGN(4);
                __bss_start__ = .;
                *runtime.o(.bss .bss.*)
                *tasks.o(.bss .bss.*)
                *port.o(.bss .bss.*)
                *ble.o(.bss .bss.*)
                *radio.o(.bss .bss.*)
                *shtc.o(.bss .bss.*)
                *i2c.o(.bss .bss.*)
                *nvm.o(.bss .bss.*)
                *bma400.o(.bss .bss.*)
                *gpint.o(.bss .bss.*)
                *crtbegin.o(.bss .bss.*)
                *lib_a-reent.o(.bss .bss.*)
                *lib_a-lock.o(.bss .bss.*)
                __bss_end__ = .;
                . = ALIGN(4);
                __bss_retained_start__ = .;
                *(COMMON)
                *(.bss)
                *(.bss.*)
                __bss_retained_end__ = .;

        } >RAM

    
}