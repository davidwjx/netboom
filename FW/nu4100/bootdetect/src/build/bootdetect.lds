OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* Entry Point */
ENTRY(bootdetect_init)

/* Specify the memory areas */
MEMORY
{  
  CRAM (xrw)    : ORIGIN = 0x1000000, LENGTH = 0x10000 /*64K*/
  LRAM_LOADER (xrw)    : ORIGIN = 0x2040000, LENGTH = 0x8000 /*32K*/
}


/* define stack size and heap size here */
VCS_PRINT_RESERVED = 0x100;
C_STACK_SIZE = 0x1000;
STACK_MAX_SIZE = C_STACK_SIZE;

  
/* Define output sections */  
SECTIONS
{
  /* The program code and other data goes into FLASH */
 .boot :
 {    
 . = ALIGN(4);
 __app_code_start__ = .;  
  *(.boot*)
 } > LRAM_LOADER
 .text :
 {    
 . = ALIGN(4); 
  *(.text*)
 } > LRAM_LOADER
 . = ALIGN(4);
 .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) } > LRAM_LOADER
  . = ALIGN(4);
  __app_code_end__ = .;
 .data : { * (.data); } > LRAM_LOADER
 . = ALIGN(4);
/* Uninitialized data section, that should not be zeroed (persistent BSS)*/  
  .pbss(NOLOAD): 
  {
  	. = ALIGN(4);
  	__app_pbss_start__ = .;
  	*(.pbss*)
  	. = ALIGN(4);
  	__app_pbss_end__ = .;
  } > LRAM_LOADER
 /* Uninitialized data section */    
  .bss :
  {
	. = ALIGN(4);
    /*  Used by the startup in order to initialize the .bss secion */    
    __app_bss_start__ = .; /* define a global symbol at bss start */   
    *(.bss*)
    *(COMMON) 
    . = ALIGN(4);    
    __app_bss_end__ = .; /* define a global symbol at bss end */
  } > LRAM_LOADER
}

INCLUDE "nu4000-rom.sym.ld"
