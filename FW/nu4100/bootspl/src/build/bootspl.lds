OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* Entry Point */
ENTRY(spl_init)

/* Specify the memory areas */
MEMORY
{  
  LRAM (xrw)    : ORIGIN = 0x2030000, LENGTH = 0x30000 /*96K*/
}

__LRAM_ADDRESS = ORIGIN(LRAM);
__LRAM_SIZE = LENGTH(LRAM);

/* define stack size and heap size here */
VCS_PRINT_RESERVED = 0x100;
C_STACK_SIZE = 0x2000;
STACK_MAX_SIZE = C_STACK_SIZE;
TLB_MAX_SIZE = 0x4000;

/* For future uses */
PM_RES_SIZE = 0x1c;
PM_CONFIG_ADDR = 4;
PM_CONFIG_SIZE = 20;

/* Approximately 2000 bytes */
PM_TRAINING_SIZE = 2000;
  
/* Define output sections */  
SECTIONS
{
  /* The program code and other data goes into FLASH */
 .boot :
 {    
 . = ALIGN(4);
 __app_code_start__ = .;  
 /* PM_RES_SIZE size should be kept unchanged as Linux' PM uses abs address */
  *(.boot*)
    . += PM_RES_SIZE;
   __pm_config_address__ = .; 
   . += PM_CONFIG_ADDR;
    KEEP(*(.boot*))
 } > LRAM
 
 .ep_section :
 {    
	 . = ALIGN(4);
	 __pm_config__ = .;
	 . += PM_CONFIG_SIZE;
	 __pm_training_start__ = .;  
	 . += PM_TRAINING_SIZE;
	 . = ALIGN(4);
	 *ddr/boot*/*.o (.text)
	 . = ALIGN(4);
	 *pm/*.o (.text .text*)
	 *pm/ddr_utils/*.o (.text .text*)
 } > LRAM
 
 .text :
 {    
 . = ALIGN(4); 
  *(.text*)
 } > LRAM
 . = ALIGN(4);
 .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) } > LRAM
  . = ALIGN(4);
  __app_code_end__ = .;
 .data : { * (.data); } > LRAM
 . = ALIGN(4);
/* Uninitialized data section, that should not be zeroed (persistent BSS)*/  
  .pbss(NOLOAD): 
  {
  	. = ALIGN(4);
  	__app_pbss_start__ = .;
  	*(.pbss*)
  	. = ALIGN(4);
  	__app_pbss_end__ = .;
  } > LRAM
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
  } > LRAM
   /* We add tlb section for enabling MMU and cache. If disabled, this can be removed */
 .tlb :
 {  
 . = ALIGN(0x4000); 
   __tlb_start__ = .;
   . += TLB_MAX_SIZE; 
    __tlb_end__ = .;
 } > LRAM
}

INCLUDE "nu3000-rom.sym.ld"
