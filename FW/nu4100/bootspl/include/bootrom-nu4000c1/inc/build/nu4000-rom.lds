OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* Entry Point */
ENTRY(_start)

/* Specify the memory areas */
MEMORY
{
  ROM (rx)      : ORIGIN = 0x0000000, LENGTH = 0x20000 /*128K*/
  LRAM (xrw)    : ORIGIN = 0x2000000, LENGTH = 0x60000 /*384K*/
}

__LRAM_ADDRESS = ORIGIN(LRAM);
__LRAM_SIZE = LENGTH(LRAM);

/* define stack size and heap size here */
IRQ_STACK_SIZE = 0x2000;
ABT_STACK_SIZE = 0x200;
C_STACK_SIZE = 0x2000;
STACK_MAX_SIZE = C_STACK_SIZE + IRQ_STACK_SIZE + ABT_STACK_SIZE;
BSS_MAX_SIZE = 0x4000;
PBSS_MAX_SIZE = 0x4000;
TLB_MAX_SIZE = 0x4000;
PAGETABLE_MAX_SIZE = 0x4000;
  
/* Define output sections */  
SECTIONS
{
 . = ALIGN(4);
  /* The program code and other data goes into FLASH */
 .text :
 {  
  *(.text*)
 } > ROM
 . = ALIGN(4);
 .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) } > ROM
 . = ALIGN(4);

 
  /* NOTE: No .data section will be used for simplicity, instead .bss + .rodata   */     
  .heap : {
	. = ALIGN(4);
	__heap_start__ = .;		
	. += __LRAM_SIZE - BSS_MAX_SIZE - PBSS_MAX_SIZE - STACK_MAX_SIZE - TLB_MAX_SIZE;
	__heap_end__ = .;	
  } >LRAM
 /* We add tlb section for enabling MMU and cache. If disabled, this can be removed */
 .tlb :
 {  
 . = ALIGN(0x4000); 
   __tlb_start__ = .;
   . += TLB_MAX_SIZE; 
    __tlb_end__ = .;
 } > LRAM

  /* Stack definition */  
  .stack : {	
	. = ALIGN(8);	
	__stack_start__ = . ;
	. += C_STACK_SIZE; 
	. = ALIGN (8); 
	__c_stack_top__ = . ; 	 
	. += ABT_STACK_SIZE; 
	. = ALIGN (8); 
	__abt_stack_top__ = . ;  
	. += IRQ_STACK_SIZE; 
	. = ALIGN (8); 
	__irq_stack_top__ = . ; 
	__stack_top__ = .; 
	. = ALIGN(8);	
  } > LRAM

 /* Uninitialized data section, that should not be zeroed (persistent BSS)*/  
  .pbss(NOLOAD): 
  {
  	. = ALIGN(4);
  	__pbss_start__ = .;
  	*(.pbss*)
  	. = ALIGN(4);
  	__pbss_end__ = .;
  } > LRAM
  /* Uninitialized data section */    
  .bss(NOLOAD) :
  {
	. = ALIGN(4);
    /*  Used by the startup in order to initialize the .bss secion */    
    __bss_start__ = .; /* define a global symbol at bss start */   

  	 __bss_patch__ = .;
	 *(.patch)

    *(.bss*)
    *(COMMON) 
    . = ALIGN(4);    
    __bss_end__ = .; /* define a global symbol at bss end */
  } > LRAM
}
