/*
 * armboot - Startup Code for OMAP3530/ARM Cortex CPU-core
 *
 * Copyright (c) 2004   Texas Instruments <r-woodruff2@ti.com>
 *
 * Copyright (c) 2001   Marius Gröger <mag@sysgo.de>
 * Copyright (c) 2002   Alex Züpke <azu@sysgo.de>
 * Copyright (c) 2002   Gary Jennejohn <garyj@denx.de>
 * Copyright (c) 2003   Richard Woodruff <r-woodruff2@ti.com>
 * Copyright (c) 2003   Kshitij <kshitij@ti.com>
 * Copyright (c) 2006-2008 Syed Mohammed Khasim <x0khasim@ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <asm/system.h>
#include <asm/linkage.h>

.globl _start
_start: b   reset
   ldr   pc, _undefined_instruction
   ldr   pc, _software_interrupt
   ldr   pc, _prefetch_abort
   ldr   pc, _data_abort
   ldr   pc, _not_used
   ldr   pc, _irq
   ldr   pc, _fiq

_undefined_instruction: .word undefined_instruction
_software_interrupt:   .word software_interrupt
_prefetch_abort:   .word prefetch_abort
_data_abort:      .word data_abort
_not_used:      .word not_used
_irq:         .word irq
_fiq:         .word fiq
_pad:         .word 0x12345678 /* now 16*4=64 */
.global _end_vect
_end_vect: .balignl 16,0xdeadbeef

/* version string */
.global _major_version
_major_version: .word MAJOR_VERSION  
.global _minor_version
_minor_version: .word MINOR_VERSION  
.global _build_version
_build_version: .word BUILD_VERSION  


/*************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * setup Memory and board specific bits prior to relocation.
 * relocate armboot to ram
 * setup stack
 *
 *************************************************************************/

/*
 * These are defined in the board-specific linker script.
 */
 
/* start address for the .bss section. defined in linker script */
.word __bss_start__
/* end address for the .bss section. defined in linker script */
.word __bss_end__
/* Define stacks */
.word   __c_stack_top__

.globl HEAP_START
HEAP_START:
   .word   __heap_start__
.globl HEAP_END
HEAP_END:
   .word   __heap_end__

/*
 * the actual reset code
 */

reset:   
   /*
    * disable interrupts (FIQ and IRQ), also set the cpu to SVC32 mode,
    * except if in HYP mode already
    */   
   mrs   r0, cpsr
   and   r1, r0, #0x1f      @ mask mode bits
   teq   r1, #0x1a         @ test for HYP mode
   bicne   r0, r0, #0x1f      @ clear all mode bits
   orrne   r0, r0, #0x13      @ set SVC mode
   orr   r0, r0, #0xc0      @ disable FIQ and IRQ
   msr   cpsr,r0
   
   /* Set V=0 in CP15 SCTRL register - for VBAR to point to vector */
   mrc   p15, 0, r0, c1, c0, 0   @ Read CP15 SCTRL Register
   bic   r0, #CR_V      @ V = 0
   mcr   p15, 0, r0, c1, c0, 0   @ Write CP15 SCTRL Register

   /* Set vector address in CP15 VBAR register */
   ldr   r0, =_start
   mcr   p15, 0, r0, c12, c0, 0   @Set VBAR

   /* the mask ROM code should have PLL and others stable */
   bl   cpu_init_cp15

 /* Init mandatory CPU registers abd jump to lowlevel_init C function (in main/main.c)*/
   bl   cpu_init_crit   

#ifndef RELEASE
   /* enable async data abort*/
   bl enable_async_abort
#endif   

   /* Read gme_general_status for por or usb resume indication (bit23)  */

   mov   r0, #248           @ 0xf8
   movt  r0, #2050          @ 0x802
   ldr   r0, [r0]
   and   r0, r0, #8388608   @ 0x800000
   lsr   r0, r0, #23
   cmp   r0, #0
   
   /* branch to usb resume or power on reset  */
   beq   bootrom_por
   b     usb_resume

   /* WARNING: No execution beyond this point */

/*************************************************************************
 *
 * cpu_init_cp15
 *
 * Setup CP15 registers (cache, MMU, TLBs). The I-cache is turned on unless
 * CONFIG_SYS_ICACHE_OFF is defined.
 *
 *************************************************************************/
ENTRY(cpu_init_cp15)
   /*
    * Invalidate L1 I/D
    */
   mov   r0, #0         @ set up for MCR
   mcr   p15, 0, r0, c8, c7, 0   @ invalidate TLBs
   mcr   p15, 0, r0, c7, c5, 0   @ invalidate icache
   mcr   p15, 0, r0, c7, c5, 6   @ invalidate BP array
   mcr   p15, 0, r0, c7, c10, 4   @ DSB
   mcr   p15, 0, r0, c7, c5, 4   @ ISB

   /*
    * disable MMU stuff and caches
    */
   mrc   p15, 0, r0, c1, c0, 0
   bic   r0, r0, #0x00002000   @ clear bits 13 (--V-)
   bic   r0, r0, #0x00000007   @ clear bits 2:0 (-CAM)
   orr   r0, r0, #0x00000002   @ set bit 1 (--A-) Align
   orr   r0, r0, #0x00000800   @ set bit 11 (Z---) BTB
//#ifdef CONFIG_SYS_ICACHE_OFF
   bic   r0, r0, #0x00001000   @ clear bit 12 (I) I-cache
/*#else
   orr   r0, r0, #0x00001000   @ set bit 12 (I) I-cache  
#endif*/
   mcr   p15, 0, r0, c1, c0, 0

   mov   pc, lr         @ back to my caller
ENDPROC(cpu_init_cp15)

/*************************************************************************
 *
 * cpu_enable_lram 
 * enable lram in gme
 *
 *************************************************************************/
/*************************************************************************
 *
 * usb_resume
 *
 * Jump to usb resume address in GME save and restore 0.
 *
 *************************************************************************/
ENTRY(usb_resume)   
   mov     r1, #260        @ 0x104
   movt    r1, #2050       @ 0x802
   ldr     r1, [r1]
/*#ifndef CONFIG_SYS_ICACHE_OFF
   // invalidate cache and dsb
   mov     r0, #0
   mcr     p15, 0, r0, c7, c5, 0
   mov     r0, #0
   mcr     p15, 0, r0, c7, c5, 6
   dsb     sy
#endif*/
   /* isb before jump to resume address*/
   isb     sy
   bx      r1
ENDPROC(usb_resume)

/*************************************************************************
 *
 * enable_async_abort
 *
 *************************************************************************/
ENTRY(enable_async_abort)
   mrs   r4, CPSR
   bic   r4, r4, #256    @ 0x100
   msr   CPSR_x, r4
   mov   pc, lr         @ back to my caller   
ENDPROC(enable_async_abort)
 
/*************************************************************************
 *
 * CPU_init_critical registers
 * setup stacks
 * setup important registers
 * setup memory timing
 *
 *************************************************************************/
 // Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs
.set  MODE_USR, 0x10            // User Mode
.set  MODE_FIQ, 0x11            // FIQ Mode
.set  MODE_IRQ, 0x12            // IRQ Mode
.set  MODE_SVC, 0x13            // Supervisor Mode
.set  MODE_ABT, 0x17            // Abort Mode
.set  MODE_UND, 0x1B            // Undefined Mode
.set  MODE_SYS, 0x1F            // System Mode

.equ  I_BIT, 0x80               // when I bit is set, IRQ is disabled
.equ  F_BIT, 0x40               // when F bit is set, FIQ is disabled


ENTRY(cpu_init_crit)
   /*
    * Jump to board specific initialization...
    * The Mask ROM will have already initialized
    * basic memory. Go here to bump up clock rate and handle
    * wake up conditions.
    */

   /*
    * Setup stacks and initialize registers for all modes
    */   
    ldr   r0,=__abt_stack_top__
    msr   CPSR_c,#MODE_UND|I_BIT|F_BIT // Undefined Instruction Mode
    mov   sp,r0   
    mov   lr,#0
    
    ldr   r0,=__abt_stack_top__
    msr   CPSR_c,#MODE_ABT|I_BIT|F_BIT // Abort Mode
    mov   sp,r0
    mov   lr,#0
    
    ldr   r0,=__abt_stack_top__
    msr   CPSR_c,#MODE_FIQ|I_BIT|F_BIT // FIQ Mode
    mov   sp,r0
    mov   lr, #0
    mov   r8, #0
    mov   r9, #0
    mov   r10, #0
    mov   r11, #0
    mov   r12, #0    
    
    ldr   r0,=__irq_stack_top__
    msr   CPSR_c,#MODE_IRQ|I_BIT|F_BIT // IRQ Mode
    mov   sp,r0
    mov   lr,#0
    
    ldr   r0,=__c_stack_top__
    msr   CPSR_c,#MODE_SYS|I_BIT|F_BIT // System/User Mode
    mov   sp,r0
    mov   lr,#0
    
    ldr   r0,=__c_stack_top__
    msr   CPSR_c,#MODE_SVC|I_BIT|F_BIT// Supervisor Mode
    mov   sp, r0
    mov   r1, #0
    mov   r2, #0
    mov   r3, #0
    mov   r4, #0
    mov   r5, #0
    mov   r6, #0
    mov   r7, #0
    mov   r8, #0
    mov   r9, #0
    mov   r10, #0
    mov   r11, #0
    mov   r11, #0
    mov   r12, #0
    
    /*
     * NOTE: All stacks are 8 byte alligned (see linker script)
    */

    mov   pc, lr         @ back to my caller

ENDPROC(cpu_init_crit)


/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */
@
@ IRQ stack frame.
@
#define S_FRAME_SIZE   72

#define S_OLD_R0   68
#define S_PSR      64
#define S_PC      60
#define S_LR      56
#define S_SP      52

#define S_IP      48
#define S_FP      44
#define S_R10      40
#define S_R9      36
#define S_R8      32
#define S_R7      28
#define S_R6      24
#define S_R5      20
#define S_R4      16
#define S_R3      12
#define S_R2      8
#define S_R1      4
#define S_R0      0

#define MODE_SVC 0x13
#define I_BIT    0x80

/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

   .macro   bad_save_user_regs
   sub   sp, sp, #S_FRAME_SIZE      @ carve out a frame on current
                  @ user stack
   stmia   sp, {r0 - r12}         @ Save user registers (now in
                  @ svc mode) r0-r12
   ldr   r2, =__abt_stack_top__      @ set base 2 words into abort
                  @ stack
   ldmia   r2, {r2 - r3}         @ get values for "aborted" pc
                  @ and cpsr (into parm regs)
   add   r0, sp, #S_FRAME_SIZE      @ grab pointer to old stack

   add   r5, sp, #S_SP
   mov   r1, lr
   stmia   r5, {r0 - r3}         @ save sp_SVC, lr_SVC, pc, cpsr
   mov   r0, sp            @ save current stack into r0
                  @ (param register)
   .endm

   .macro   irq_save_user_regs
   sub   sp, sp, #S_FRAME_SIZE
   stmia   sp, {r0 - r12}         @ Calling r0-r12
   add   r8, sp, #S_PC         @ !! R8 NEEDS to be saved !!
                  @ a reserved stack spot would
                  @ be good.
   stmdb   r8, {sp, lr}^         @ Calling SP, LR
   str   lr, [r8, #0]         @ Save calling PC
   mrs   r6, spsr
   str   r6, [r8, #4]         @ Save CPSR
   str   r0, [r8, #8]         @ Save OLD_R0
   mov   r0, sp
   .endm

   .macro   irq_restore_user_regs
   ldmia   sp, {r0 - lr}^         @ Calling r0 - lr
   mov   r0, r0
   ldr   lr, [sp, #S_PC]         @ Get PC
   add   sp, sp, #S_FRAME_SIZE
   subs   pc, lr, #4         @ return & move spsr_svc into
                  @ cpsr
   .endm

   .macro get_bad_stack
   ldr   r13, =__abt_stack_top__      @ setup our mode stack (enter
                  @ in banked mode)

   str   lr, [r13]         @ save caller lr in position 0
                  @ of saved stack
   mrs   lr, spsr         @ get the spsr
   str   lr, [r13, #4]         @ save spsr in position 1 of
                  @ saved stack

   mov   r13, #MODE_SVC         @ prepare SVC-Mode
   @ msr   spsr_c, r13
   msr   spsr, r13         @ switch modes, make sure
                  @ moves will execute
   mov   lr, pc            @ capture return pc
   movs   pc, lr            @ jump to next instruction &
                  @ switch modes.
   .endm

   .macro get_bad_stack_swi
   sub   r13, r13, #4         @ space on current stack for
                  @ scratch reg.
   str   r0, [r13]         @ save R0's value.
   ldr   r0, =__abt_stack_top__      @ get data regions start
                  @ spots for abort stack
   str   lr, [r0]         @ save caller lr in position 0
                  @ of saved stack
   mrs   lr, spsr         @ get the spsr
   str   lr, [r0, #4]         @ save spsr in position 1 of
                  @ saved stack
   ldr   lr, [r0]         @ restore lr
   ldr   r0, [r13]         @ restore r0
   add   r13, r13, #4         @ pop stack entry
   .endm

/*
 * exception handlers
 */
   .align   5
undefined_instruction:
   get_bad_stack
   bad_save_user_regs
   bl   do_undefined_instruction

   .align   5
software_interrupt:
   get_bad_stack_swi
   bad_save_user_regs
   bl   do_software_interrupt

   .align   5
prefetch_abort:
   get_bad_stack
   bad_save_user_regs
   bl   do_prefetch_abort 

   .align   5
data_abort:
   get_bad_stack
   bad_save_user_regs
   bl   do_data_abort

   .align   5
not_used:
   get_bad_stack
   bad_save_user_regs
   bl   do_not_used

   .align   5
irq:
   irq_save_user_regs
   bl   do_irq
   irq_restore_user_regs

   .align   5
fiq:
   get_bad_stack
   bad_save_user_regs
   bl   do_fiq
