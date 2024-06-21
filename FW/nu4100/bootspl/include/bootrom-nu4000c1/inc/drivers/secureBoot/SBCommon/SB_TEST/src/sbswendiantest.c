/**
*  File: sbswendiantest.c
*
*  Description: Tests sb_sw_endian.h.
*
* Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
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
*/

#include "sb_sw_endian.h"
#include "sfzutf.h"
#include "c_lib.h"

/* workspace of 8 bytes, for testing read/write operations. */
typedef union
{
    uint32_t data_u32[2];
    uint16_t data_u16[4];
    uint8_t  data_u8[8];
} test_workspace_t;

typedef union
{
    void     * voidptr;
    uint32_t * u32ptr;
    uint16_t * u16ptr;
} test_convertpointer_t;

START_TEST(test_SB_SW_BYTEORDER_SWAP32)
{
    uint32_t x = 0xF1E2D3C4;

    fail_if(SB_SW_BYTEORDER_SWAP32(x) != 0xC4D3E2F1, "variable");
    fail_if(SB_SW_BYTEORDER_SWAP32(0xE1D2C3B4) != 0xB4C3D2E1, "constant");
}
END_TEST

START_TEST(test_SB_SW_BYTEORDER_SWAP16)
{
    uint16_t x = 0xF1E2;

    fail_if(SB_SW_BYTEORDER_SWAP16(x) != 0xE2F1, "variable");
    fail_if(SB_SW_BYTEORDER_SWAP16(0xE1D2) != 0xD2E1, "constant");
}
END_TEST

START_TEST(test_SB_SW_BYTEORDER_SWAP32_CONSTANT)
{
    fail_if(SB_SW_BYTEORDER_SWAP32(0xF1E2D3C4) != 0xC4D3E2F1, "constant");
}
END_TEST

START_TEST(test_SB_SW_BYTEORDER_SWAP16_CONSTANT)
{
    fail_if(SB_SW_BYTEORDER_SWAP16(0xF1E2) != 0xE2F1, "constant");
}
END_TEST

/* Helper function for setting detection word into t and
   filling rest with 0x55. */
static void set_t_8192A3B4(test_workspace_t * const t,
                           int offs)
{
    t->data_u8[0] = 0x55;
    t->data_u8[1] = 0x55;
    t->data_u8[2] = 0x55;
    t->data_u8[3] = 0x55;
    t->data_u8[4] = 0x55;
    t->data_u8[5] = 0x55;
    t->data_u8[6] = 0x55;
    t->data_u8[7] = 0x55;

    if (offs >= 0 && offs <= 4)
    {
        t->data_u8[0 + offs] = 0x81;
        t->data_u8[1 + offs] = 0x92;
        t->data_u8[2 + offs] = 0xA3;
        t->data_u8[3 + offs] = 0xB4;
    }
}

START_TEST(test_SB_SW_BE32_TO_CPU)
{
    test_workspace_t t;
    set_t_8192A3B4(&t, 0);

    fail_if(SB_SW_BE32_TO_CPU(t.data_u32[0]) != 0x8192A3B4,
            "Macro SB_SW_BE32_TO_CPU does not work.");
}
END_TEST

START_TEST(test_SB_SW_BE16_TO_CPU)
{
    test_workspace_t t;
    set_t_8192A3B4(&t, 0);

    fail_if(SB_SW_BE16_TO_CPU(t.data_u16[0]) != 0x8192,
            "Macro SB_SW_BE16_TO_CPU does not work.");

    fail_if(SB_SW_BE16_TO_CPU(t.data_u16[1]) != 0xA3B4,
            "Macro SB_SW_BE16_TO_CPU does not work.");
}
END_TEST

START_TEST(test_SB_SW_LE32_TO_CPU)
{
    test_workspace_t t;
    set_t_8192A3B4(&t, 0);

    fail_if(SB_SW_LE32_TO_CPU(t.data_u32[0]) != 0xB4A39281,
            "Macro SB_SW_LE32_TO_CPU does not work.");
}
END_TEST

START_TEST(test_SB_SW_LE16_TO_CPU)
{
    test_workspace_t t;
    set_t_8192A3B4(&t, 0);

    fail_if(SB_SW_LE16_TO_CPU(t.data_u16[0]) != 0x9281,
            "Macro SB_SW_LE16_TO_CPU does not work.");

    fail_if(SB_SW_LE16_TO_CPU(t.data_u16[1]) != 0xB4A3,
            "Macro SB_SW_LE16_TO_CPU does not work.");
}
END_TEST

START_TEST(test_SB_SW_CPU_TO_BE32)
{
    test_workspace_t t;
    uint32_t cpu_x = 0x8192A3B4;

    set_t_8192A3B4(&t, 0);

    fail_if(t.data_u32[0] != SB_SW_CPU_TO_BE32(cpu_x),
            "Macro SB_SW_CPU_TO_BE32 does not work.");

    fail_if(t.data_u32[0] != SB_SW_CPU_TO_BE32(0x8192A3B4),
            "Macro SB_SW_CPU_TO_BE32 does not work.");
}
END_TEST

START_TEST(test_SB_SW_CPU_TO_BE16)
{
    test_workspace_t t;
    uint16_t cpu_x = 0x8192;

    set_t_8192A3B4(&t, 0);

    fail_if(t.data_u16[0] != SB_SW_CPU_TO_BE16(cpu_x),
            "Macro SB_SW_CPU_TO_BE16 does not work.");

    fail_if(t.data_u16[0] != SB_SW_CPU_TO_BE16(0x8192),
            "Macro SB_SW_CPU_TO_BE16 does not work.");
}
END_TEST

START_TEST(test_SB_SW_CPU_TO_LE32)
{
    test_workspace_t t;
    uint32_t cpu_x = 0xB4A39281;

    set_t_8192A3B4(&t, 0);

    fail_if(t.data_u32[0] != SB_SW_CPU_TO_LE32(cpu_x),
            "Macro SB_SW_CPU_TO_LE32 does not work.");

    fail_if(t.data_u32[0] != SB_SW_CPU_TO_LE32(0xB4A39281),
            "Macro SB_SW_CPU_TO_LE32 does not work.");
}
END_TEST

START_TEST(test_SB_SW_CPU_TO_LE16)
{
    test_workspace_t t;
    uint16_t cpu_x = 0x9281;

    set_t_8192A3B4(&t, 0);

    fail_if(t.data_u16[0] != SB_SW_CPU_TO_LE16(cpu_x),
            "Macro SB_SW_CPU_TO_LE16 does not work.");

    fail_if(t.data_u16[0] != SB_SW_CPU_TO_LE16(0x9281),
            "Macro SB_SW_CPU_TO_LE16 does not work.");
}
END_TEST

START_TEST(test_SB_SW_CPU_TO_BE32_CONSTANT)
{
    test_workspace_t t;

    set_t_8192A3B4(&t, 0);

    fail_if(t.data_u32[0] != SB_SW_CPU_TO_BE32_CONSTANT(0x8192A3B4),
            "Macro SB_SW_CPU_TO_BE32_CONSTANT does not work.");
}
END_TEST

START_TEST(test_SB_SW_CPU_TO_BE16_CONSTANT)
{
    test_workspace_t t;

    set_t_8192A3B4(&t, 0);

    fail_if(t.data_u16[0] != SB_SW_CPU_TO_BE16_CONSTANT(0x8192),
            "Macro SB_SW_CPU_TO_BE16_CONSTANT does not work.");
}
END_TEST

START_TEST(test_SB_SW_CPU_TO_LE32_CONSTANT)
{
    test_workspace_t t;

    set_t_8192A3B4(&t, 0);

    fail_if(t.data_u32[0] != SB_SW_CPU_TO_LE32_CONSTANT(0xB4A39281),
            "Macro SB_SW_CPU_TO_LE32_CONSTANT does not work.");
}
END_TEST

START_TEST(test_SB_SW_CPU_TO_LE16_CONSTANT)
{
    test_workspace_t t;

    set_t_8192A3B4(&t, 0);

    fail_if(t.data_u16[0] != SB_SW_CPU_TO_LE16_CONSTANT(0x9281),
            "Macro SB_SW_CPU_TO_LE16_CONSTANT does not work.");
}
END_TEST

START_TEST(test_SB_SW_BE32_READ_ALIGNED)
{
    test_workspace_t t;
    set_t_8192A3B4(&t, 0);

    fail_if(SB_SW_BE32_READ_ALIGNED(&t) != 0x8192A3B4,
            "Macro SB_SW_BE32_READ_ALIGNED does not work.");
}
END_TEST

START_TEST(test_SB_SW_BE16_READ_ALIGNED)
{
    test_workspace_t t;
    set_t_8192A3B4(&t, 0);

    fail_if(SB_SW_BE16_READ_ALIGNED(&t) != 0x8192,
            "Macro SB_SW_BE16_READ_ALIGNED does not work.");

    fail_if(SB_SW_BE16_READ_ALIGNED(&(t.data_u16[1])) != 0xA3B4,
            "Macro SB_SW_BE16_READ_ALIGNED does not work.");
}
END_TEST

START_TEST(test_SB_SW_LE32_READ_ALIGNED)
{
    test_workspace_t t;
    set_t_8192A3B4(&t, 0);

    fail_if(SB_SW_LE32_READ_ALIGNED(&t) != 0xB4A39281,
            "Macro SB_SW_LE32_READ_ALIGNED does not work.");
}
END_TEST

START_TEST(test_SB_SW_LE16_READ_ALIGNED)
{
    test_workspace_t t;
    set_t_8192A3B4(&t, 0);

    fail_if(SB_SW_LE16_READ_ALIGNED(&t) != 0x9281,
            "Macro SB_SW_LE16_READ_ALIGNED does not work.");

    fail_if(SB_SW_LE16_READ_ALIGNED(&(t.data_u16[1])) != 0xB4A3,
            "Macro SB_SW_LE16_READ_ALIGNED does not work.");
}
END_TEST

START_TEST(test_SB_SW_BE32_READ)
{
    int i;
    test_workspace_t t;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);

        p.voidptr = &t.data_u8[i];
        fail_if(SB_SW_BE32_READ(p.u32ptr) != 0x8192A3B4,
                "Macro SB_SW_BE32_READ does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_BE16_READ)
{
    int i;
    test_workspace_t t;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);

        p.voidptr = &t.data_u8[i];
        fail_if(SB_SW_BE16_READ(p.u16ptr) != 0x8192,
                "Macro SB_SW_BE16_READ does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_LE32_READ)
{
    int i;
    test_workspace_t t;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);

        p.voidptr = &t.data_u8[i];
        fail_if(SB_SW_LE32_READ(p.u32ptr) != 0xB4A39281,
                "Macro SB_SW_LE32_READ does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_LE16_READ)
{
    int i;
    test_workspace_t t;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);

        p.voidptr = &t.data_u8[i];
        fail_if(SB_SW_LE16_READ(p.u16ptr) != 0x9281,
                "Macro SB_SW_LE16_READ does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_CPU32_READ)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, 0);

        p.voidptr = &t.data_u8[i];
        fail_if(SB_SW_CPU32_READ(p.u32ptr) != t2.data_u32[0],
                "Macro SB_SW_CPU32_READ does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_CPU16_READ)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, 0);

        p.voidptr = &t.data_u8[i];
        fail_if(SB_SW_CPU16_READ(p.u16ptr) != t2.data_u16[0],
                "Macro SB_SW_CPU16_READ does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_BE32_WRITE)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, -1);

        p.voidptr = &t2.data_u8[i];
        SB_SW_BE32_WRITE(p.u32ptr, 0x8192A3B4);

        fail_if(c_memcmp(&t, &t2, sizeof(t)) != 0,
                "Macro SB_SW_BE32_WRITE does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_BE16_WRITE)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_convertpointer_t p1;
    test_convertpointer_t p2;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, -1);

        p1.voidptr = &t2.data_u8[i];
        p2.voidptr = &t2.data_u8[i + 2];
        SB_SW_BE16_WRITE(p1.u16ptr, 0x8192);
        SB_SW_BE16_WRITE(p2.u16ptr, 0xA3B4);

        fail_if(c_memcmp(&t, &t2, sizeof(t)) != 0,
                "Macro SB_SW_BE16_WRITE does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_LE32_WRITE)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, -1);

        p.voidptr = &t2.data_u8[i];
        SB_SW_LE32_WRITE(p.u32ptr, 0xB4A39281);

        fail_if(c_memcmp(&t, &t2, sizeof(t)) != 0,
                "Macro SB_SW_LE32_WRITE does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_LE16_WRITE)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_convertpointer_t p1;
    test_convertpointer_t p2;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, -1);

        p1.voidptr = &t2.data_u8[i];
        p2.voidptr = &t2.data_u8[i + 2];
        SB_SW_LE16_WRITE(p1.u16ptr, 0x9281);
        SB_SW_LE16_WRITE(p2.u16ptr, 0xB4A3);

        fail_if(c_memcmp(&t, &t2, sizeof(t)) != 0,
                "Macro SB_SW_LE16_WRITE does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_CPU32_WRITE)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_workspace_t t3;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, -1);
        set_t_8192A3B4(&t3, 0);

        p.voidptr = &t2.data_u8[i];
        SB_SW_CPU32_WRITE(p.u32ptr, t3.data_u32[0]);

        fail_if(c_memcmp(&t, &t2, sizeof(t)) != 0,
                "Macro SB_SW_CPU32_WRITE does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_CPU16_WRITE)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_workspace_t t3;
    test_convertpointer_t p1;
    test_convertpointer_t p2;

    for(i = 0; i <= 4; i++)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, -1);
        set_t_8192A3B4(&t3, 0);

        p1.voidptr = &t2.data_u8[i];
        p2.voidptr = &t2.data_u8[i + 2];
        SB_SW_CPU16_WRITE(p1.u16ptr, t3.data_u16[0]);
        SB_SW_CPU16_WRITE(p2.u16ptr, t3.data_u16[1]);

        fail_if(c_memcmp(&t, &t2, sizeof(t)) != 0,
                "Macro SB_SW_CPU16_WRITE does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_BE32_WRITE_ALIGNED)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i += 4)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, -1);

        p.voidptr = &t2.data_u8[i];
        SB_SW_BE32_WRITE_ALIGNED(p.u32ptr, 0x8192A3B4);

        fail_if(c_memcmp(&t, &t2, sizeof(t)) != 0,
                "Macro SB_SW_BE32_WRITE_ALIGNED does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_BE16_WRITE_ALIGNED)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_convertpointer_t p1;
    test_convertpointer_t p2;

    for(i = 0; i <= 4; i += 2)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, -1);

        p1.voidptr = &t2.data_u8[i];
        p2.voidptr = &t2.data_u8[i + 2];
        SB_SW_BE16_WRITE_ALIGNED(p1.u16ptr, 0x8192);
        SB_SW_BE16_WRITE_ALIGNED(p2.u16ptr, 0xA3B4);

        fail_if(c_memcmp(&t, &t2, sizeof(t)) != 0,
                "Macro SB_SW_BE16_WRITE_ALIGNED does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_LE32_WRITE_ALIGNED)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_convertpointer_t p;

    for(i = 0; i <= 4; i += 4)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, -1);

        p.voidptr = &t2.data_u8[i];
        SB_SW_LE32_WRITE_ALIGNED(p.u32ptr, 0xB4A39281);

        fail_if(c_memcmp(&t, &t2, sizeof(t)) != 0,
                "Macro SB_SW_LE32_WRITE_ALIGNED does not work.");
    }
}
END_TEST

START_TEST(test_SB_SW_LE16_WRITE_ALIGNED)
{
    int i;
    test_workspace_t t;
    test_workspace_t t2;
    test_convertpointer_t p1;
    test_convertpointer_t p2;

    for(i = 0; i <= 4; i += 2)
    {
        set_t_8192A3B4(&t, i);
        set_t_8192A3B4(&t2, -1);

        p1.voidptr = &t2.data_u8[i];
        p2.voidptr = &t2.data_u8[i + 2];
        SB_SW_LE16_WRITE_ALIGNED(p1.u16ptr, 0x9281);
        SB_SW_LE16_WRITE_ALIGNED(p2.u16ptr, 0xB4A3);

        fail_if(c_memcmp(&t, &t2, sizeof(t)) != 0,
                "Macro SB_SW_LE16_WRITE_ALIGNED does not work.");
    }
}
END_TEST

/* Build actual test suite. */
void
build_suite (void)
{
    sfzutf_suite_create ("sb_sw_endian_suite");
    sfzutf_tcase_create ("byte_order_swap");
    sfzutf_test_add(test_SB_SW_BYTEORDER_SWAP32);
    sfzutf_test_add(test_SB_SW_BYTEORDER_SWAP16);
    sfzutf_test_add(test_SB_SW_BYTEORDER_SWAP32_CONSTANT);
    sfzutf_test_add(test_SB_SW_BYTEORDER_SWAP16_CONSTANT);
    sfzutf_tcase_create ("conversion_to_specific_byteorder");
    sfzutf_test_add(test_SB_SW_BE32_TO_CPU);
    sfzutf_test_add(test_SB_SW_BE16_TO_CPU);
    sfzutf_test_add(test_SB_SW_LE32_TO_CPU);
    sfzutf_test_add(test_SB_SW_LE16_TO_CPU);
    sfzutf_test_add(test_SB_SW_CPU_TO_BE32);
    sfzutf_test_add(test_SB_SW_CPU_TO_BE16);
    sfzutf_test_add(test_SB_SW_CPU_TO_LE32);
    sfzutf_test_add(test_SB_SW_CPU_TO_LE16);
    sfzutf_test_add(test_SB_SW_CPU_TO_BE32_CONSTANT);
    sfzutf_test_add(test_SB_SW_CPU_TO_BE16_CONSTANT);
    sfzutf_test_add(test_SB_SW_CPU_TO_LE32_CONSTANT);
    sfzutf_test_add(test_SB_SW_CPU_TO_LE16_CONSTANT);
    sfzutf_tcase_create ("read_and_convert");
    sfzutf_test_add(test_SB_SW_BE32_READ_ALIGNED);
    sfzutf_test_add(test_SB_SW_BE16_READ_ALIGNED);
    sfzutf_test_add(test_SB_SW_LE32_READ_ALIGNED);
    sfzutf_test_add(test_SB_SW_LE16_READ_ALIGNED);
    sfzutf_test_add(test_SB_SW_BE32_READ);
    sfzutf_test_add(test_SB_SW_BE16_READ);
    sfzutf_test_add(test_SB_SW_LE32_READ);
    sfzutf_test_add(test_SB_SW_LE16_READ);
    sfzutf_test_add(test_SB_SW_CPU32_READ);
    sfzutf_test_add(test_SB_SW_CPU16_READ);
    sfzutf_tcase_create ("convert_and_write");
    sfzutf_test_add(test_SB_SW_BE32_WRITE);
    sfzutf_test_add(test_SB_SW_BE16_WRITE);
    sfzutf_test_add(test_SB_SW_LE32_WRITE);
    sfzutf_test_add(test_SB_SW_LE16_WRITE);
    sfzutf_test_add(test_SB_SW_CPU32_WRITE);
    sfzutf_test_add(test_SB_SW_CPU16_WRITE);
    sfzutf_test_add(test_SB_SW_BE32_WRITE_ALIGNED);
    sfzutf_test_add(test_SB_SW_BE16_WRITE_ALIGNED);
    sfzutf_test_add(test_SB_SW_LE32_WRITE_ALIGNED);
    sfzutf_test_add(test_SB_SW_LE16_WRITE_ALIGNED);
}
