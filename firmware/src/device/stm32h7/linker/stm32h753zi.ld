/*
 * Linker script for STM32H753ZIT6U
 *
 * Memory map:
 *   ITCM RAM:    64KB  @ 0x00000000
 *   FLASH:       2MB   @ 0x08000000
 *   DTCM RAM:   128KB  @ 0x20000000
 *   AXI SRAM:   512KB  @ 0x24000000
 *   SRAM1:      128KB  @ 0x30000000
 *   SRAM2:      128KB  @ 0x30020000
 *   SRAM4:       64KB  @ 0x38000000
 *   Backup SRAM:  4KB  @ 0x38800000
 */

ENTRY(Reset_Handler)

_estack = ORIGIN(DTCM) + LENGTH(DTCM);

_Min_Heap_Size = 0x1000;
_Min_Stack_Size = 0x1000;

MEMORY
{
  ITCM   (xrw) : ORIGIN = 0x00000000, LENGTH = 64K
  FLASH  (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
  DTCM   (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
  RAM    (xrw) : ORIGIN = 0x24000000, LENGTH = 512K
  SRAM1  (xrw) : ORIGIN = 0x30000000, LENGTH = 128K
  SRAM2  (xrw) : ORIGIN = 0x30020000, LENGTH = 128K
  SRAM4  (xrw) : ORIGIN = 0x38000000, LENGTH = 64K
  BKPSRAM(rw)  : ORIGIN = 0x38800000, LENGTH = 4K
}

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)

    KEEP(*(.init))
    KEEP(*(.fini))

    . = ALIGN(4);
    _etext = .;
  } >FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);
  } >FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);
  } >FLASH

  _sidata = LOADADDR(.data);

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    *(.RamFunc)
    *(.RamFunc*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT> FLASH

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM

  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    PROVIDE(_end = .);
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >DTCM

  /DISCARD/ :
  {
    libc.a:*( * )
    libm.a:*( * )
    libgcc.a:*( * )
    *(.ARM.exidx*)
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
