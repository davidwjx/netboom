/* sfzutf-utils.h
 *
 * Description: SFZUTF utility routines header.
 */

/*****************************************************************************
* Copyright (c) 2008-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_SFZUTF_UTILS_H
#define INCLUDE_GUARD_SFZUTF_UTILS_H

#include "public_defs.h"
#include "sfzutf.h"

/* To supplement common need. */
uint32_t sfzutf_strlen(const char *str);

enum SfzUtfPtrSizeFormat
{
    SFZUTF_PTRSIZE_STRING_MSB_FIRST,
    SFZUTF_PTRSIZE_STRING_LSB_FIRST,
    SFZUTF_PTRSIZE_STRING_TEXT,
};

/* Short notations. */
#define SUPS_MSBF SFZUTF_PTRSIZE_STRING_MSB_FIRST
#define SUPS_LSBF SFZUTF_PTRSIZE_STRING_LSB_FIRST
#define SUPS_TEXT SFZUTF_PTRSIZE_STRING_TEXT

typedef struct sfzutf_ptrsize
{
    void *ptr;
    uint32_t size;
    uint32_t len;
    enum SfzUtfPtrSizeFormat format;
} SfzUtfPtrSize;

/* Identifier for ptrsize purpose. */
typedef unsigned short SfzUtfUtilsID;

typedef struct sfzutf_ptrsize_ext
{
    SfzUtfPtrSize base;
    struct sfzutf_ptrsize_ext *next;
    SfzUtfEvent livetime;
    SfzUtfUtilsID *purpose;
    unsigned long long foralignment;
} SfzUtfPtrSizeExt;

SfzUtfPtrSize sfzutf_ptrsize_blank(uint32_t size);
char *sfzutf_ptrsize_to_str(SfzUtfPtrSize sups);
SfzUtfPtrSize sfzutf_ptrsize_from_str(const char *string,
                                      enum SfzUtfPtrSizeFormat format);
SfzUtfPtrSize sfzutf_ptrsize_from_mem(const void *mem,
                                      uint32_t memlength,
                                      enum SfzUtfPtrSizeFormat format);
void sfzutf_ptrsize_free(SfzUtfPtrSize sups);

SfzUtfPtrSize sfzutf_ptrsize_fill_with_ptrsize(SfzUtfPtrSize empty_target,
        SfzUtfPtrSize filler);

#define sfzutf_ptrsize_eq_ptrsize(ptrsize_1, ptrsize_2) \
  (sfzutf_ptrsize_cmp_ptrsize(ptrsize_1, ptrsize_2) == 0)

#define sfzutf_ptrsize_eq_str(ptrsize_1, str, format)                  \
  (sfzutf_ptrsize_cmp_str(ptrsize_1, str, format) == 0)

#define sfzutf_ptrsize_eq_mem(ptrsize_1, mem, len, format)             \
  (sfzutf_ptrsize_cmp_mem(ptrsize_1, mem, len, format) == 0)

int sfzutf_ptrsize_cmp_ptrsize(SfzUtfPtrSize ptrsize_1,
                               SfzUtfPtrSize ptrsize_2);

/* SFZUTF ptrsize default livetime is single test.
   This function sets the livetime
   (until end of test, testcase, suite or global). */
void sfzutf_ptrsize_set_livetime(SfzUtfPtrSize ps,
                                 SfzUtfEvent livetime);

/* Extended ptrsize allocator. Avoid using the function directly. */
void *sfzutf_ptrsize_ext_alloc(size_t size,
                               bool clear,
                               SfzUtfUtilsID * purpose,
                               SfzUtfEvent livetime);

/* Entrypoint called by sfzutf core on various events.
   Do not call directly. */
void sfzutf_utils_event(SfzUtfEvent event,
                        const char *name,
                        const void *struct_ptr);

/* Functions for discovering a ptrsize.
   Do not call directly. */
SfzUtfPtrSizeExt *sfzutf_find_ptrsize_ext_by_address(void *memaddress);
SfzUtfPtrSizeExt *sfzutf_find_ptrsize_ext_by_purpose(SfzUtfUtilsID *purpose);
SfzUtfPtrSizeExt *sfzutf_find_ptrsize_ext_by_livetime(SfzUtfEvent livetime);

#define SFZUTF_UTILS_NEED_TEMPORARY(name, scope)                        \
    do {                                                                \
        static SfzUtfUtilsID name##_id;                                 \
        SfzUtfPtrSizeExt * ex_T = sfzutf_find_ptrsize_ext_by_purpose(   \
                &name##_id);                                            \
        if (ex_T) name = ex_T->base.ptr; else name = NULL;              \
        if (name == NULL) name = sfzutf_ptrsize_ext_alloc(sizeof(*name),\
                                                          1,            \
                                                          &name##_id,   \
                                                          scope);       \
        name = sfzutf_AssertNotNull(name);                              \
} while(0)

/* Same than SFZUTF_NEED_TEMPORARY, but reservation is skipped if space
   is not available. (Need to check pointer for null.) */
#define SFZUTF_UTILS_TEMPORARY_BEGIN(name, scope)                       \
    do {                                                                \
        static SfzUtfUtilsID name##_id;                                 \
        SfzUtfPtrSizeExt * ex_T = sfzutf_find_ptrsize_ext_by_purpose(   \
                &name##_id);                                            \
        if (ex_T) name = ex_T->base.ptr; else name = NULL;              \
        if (name == NULL) name = sfzutf_ptrsize_ext_alloc(sizeof(*name),\
                                                          1,            \
                                                          &name##_id,   \
                                                          scope);       \
        if (!name) { unsupported_quick("Memory Not Available"); break; }

#define SFZUTF_UTILS_TEMPORARY_END \
} while(0)

#endif /* Include Guard */

/* end of file sfzutf-utils.h */
