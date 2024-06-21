   .syntax unified
   .cpu cortex-a5
   .thumb
   .section	.text.arms_lock,"ax",%progbits
   .align	2
   .global	arms_lock
   .thumb
   .thumb_func
   .equ LOCKED, 1
   .type	arms_lock, %function
   /* void arms_lock(lock_t* pAddr) */

sleep:
   movw	r0, #10
   b    usleep

arms_lock:
   /* is locked ? */
   ldrex    r1, [r0]				/* Check if locked */
   cmp      r1, #LOCKED             /* Compare with "locked" */
   beq      sleep                    /* If LOCKED, try again */
   
   /* Attempt to lock */
   mov      r1, #LOCKED
   strex    r2, r1, [r0]            /* Attempt to lock */           
   cmp      r2, #0x0                /* Check whether store completed */
   bne      sleep               /* if store failed, try again */
   dmb
   
   bx  lr
   .end
