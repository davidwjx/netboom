/* c_lib.c
 *
 * Description: Wrappers for C Library functions
 */

/*****************************************************************************
* Copyright (c) 2007-2018 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://customersupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#include "implementation_defs.h"
#include "clib.h"
#include <stdlib.h>
#include <ctype.h>


void *
c_memcpy(void * restrict s1,
         const void * restrict s2,
         size_t n)
{
    return memcpy(s1, s2, n);
}


void *
c_memmove(void * s1,
          const void * s2,
          size_t n)
{
    return memmove(s1, s2, n);
}


void *
c_memset(void * s,
         int c,
         size_t n)
{
    return memset(s, c, n);
}


int
c_memcmp(const void * s1,
         const void * s2,
         size_t n)
{
    return memcmp(s1, s2, n);
}


int
c_strcmp(const char * s1,
         const char * s2)
{
    return strcmp(s1, s2);
}


int
c_strncmp(const char * s1,
          const char * s2,
          size_t n)
{
    return strncmp(s1, s2, n);
}


char *
c_strcpy(char * restrict s1,
         const char * restrict s2)
{
    return strcpy(s1, s2);
}


char *
c_strncpy(char * dest,
          const char * src,
          size_t n)
{
    return strncpy(dest, src, n);
}


char *
c_strcat(char * dest,
         const char * src)
{
    return strcat(dest, src);
}


size_t
c_strlen(const char * s)
{
    return strlen(s);
}


char *
c_strstr(const char * str1,
         const char * str2)
{
    return strstr(str1, str2);
}


long
c_strtol(const char * str,
         char ** endptr,
         int16_t radix)
{
    return strtol(str, endptr, radix);
}


char *
c_strchr(const char * str,
         int32_t c)
{
    return strchr(str, c);
}


int
c_tolower(int c)
{
    return tolower(c);
}


int
c_toupper(int c)
{
    return toupper(c);
}


int *
c_memchr(const void * buf,
         int32_t ch,
         size_t num)
{
    return memchr(buf, ch, num);
}


/* end of file c_lib.c */
