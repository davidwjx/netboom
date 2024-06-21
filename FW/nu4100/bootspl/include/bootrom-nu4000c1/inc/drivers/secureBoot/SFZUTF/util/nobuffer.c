/* This code is borrowed from
   http://fixunix.com/linux/6684-unbuffering-stdout.html */
/* Copyright assumed to be free to use.
   Notice: This code shall never get linked against INSIDE Secure code,
           but instead only against varioussimulators/emulators. */

#include <stdio.h>

void __attribute__((constructor)) nobuffer_init(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
}
