   .syntax unified
   .cpu cortex-a5
   .thumb
   .section	.text.arms_unlock,"ax",%progbits
   .align	2
   .global	arms_unlock
   .thumb
   .thumb_func
   .equ UNLOCKED, 0
   .type	arms_unlock, %function
   /* void arms_unlock(lock_t* pAddr) */
arms_unlock:
   dmb                              /* To ensure accesses to protected resource have completed  */
   mov      r1, #UNLOCKED           /* Write "unlocked" into lock field  */
   str      r1, [r0]
   
   bx  lr
   .end
