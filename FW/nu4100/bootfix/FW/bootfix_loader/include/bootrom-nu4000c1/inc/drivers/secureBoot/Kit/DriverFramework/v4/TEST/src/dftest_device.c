/* dftest_device.c
 *
 * Description: test Device APIs of Driver Framework.
 */

/*****************************************************************************
* Copyright (c) 2011-2018 INSIDE Secure B.V. All Rights Reserved.
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

#include "basic_defs.h"
#include "c_lib.h"
#include "sfzutf.h"

#include "device_types.h"
#include "device_mgmt.h"
#include "device_rw.h"
#include "device_swap.h"
#include "cs_dftest.h"

#ifndef DFTEST_REMOVE_DEVICE_APIS

#ifdef DFTEST_REMOVE_DEVICE_UNINITIALIZE
// implementation does not provide an Device_UnInitialize function
// provide a dummy here, instead
#define Device_UnInitialize()
#endif /* DFTEST_REMOVE_DEVICE_UNINITIALIZE */


START_TEST(test_device_init_uninit)
{
    uint32_t InitData;
    int Ret;

    /* call Device_Initialize with NULL pointer */
    Ret = Device_Initialize(NULL);
    fail_unless(Ret == 0,
                "failed to work with NULL pointer");
    Device_UnInitialize();
    /* call Device_Initialize with non-NULL pointer */
    Ret = Device_Initialize((void *)&InitData);
    fail_unless(Ret == 0,
                "failed to work with non-NULL pointer");
    Device_UnInitialize();
}
END_TEST

START_TEST(test_device_find)
{
    Device_Handle_t Device;
    Device_Handle_t PrevDevice;
    uint32_t Index;
    char LongDevName[256] = {0};

    fail_unless(Device_Initialize(NULL) == 0, "Device_initialize failed");

    /* Check if it handles NULL pointer parameter well */
    Device_Find(NULL);

#ifndef DFTEST_REMOVE_DEVICE_KNOWN_UNSUPPORTED_DEV_NAMES

    Device = Device_Find("a");
    fail_unless(Device == NULL, "Should not find a device by invalid name");

    Device = Device_Find("Device1");
    fail_unless(Device == NULL, "Should not find a device by invalid name");

    Device = Device_Find("EIP123456");
    fail_unless(Device == NULL, "Should not find a device by invalid name");

    Device = Device_Find("EIP123 EIP122");
    fail_unless(Device == NULL, "Should not find a device by invalid name");

    c_memset((void *)LongDevName, 'a', sizeof(LongDevName) - 1);
    Device = Device_Find(LongDevName);
    fail_unless(Device == NULL, "Should not find a device by invalid name");
#endif
    /* Call the function repeatly,it should return same handle */
    PrevDevice = Device_Find(DFTEST_VALID_DEV_NAME);
    fail_if(PrevDevice == NULL, "Failed to find device");

    for (Index = 0; Index < 4; Index++)
    {
        Device = Device_Find(DFTEST_VALID_DEV_NAME);
        fail_if(Device == NULL, "Failed to find device");
        fail_unless(Device == PrevDevice,
                    "Got different handles for same string");
    }
    Device_UnInitialize();

}
END_TEST

#if DFTEST_DEVICE_WRDEV_CAPS & \
        (DFTEST_DEVICE_DEV_WR_CAP | DFTEST_DEVICE_DEV_READ_ONLY_CAP)
START_TEST(test_device_read32)
{
    Device_Handle_t Device;
    size_t ByteOffset;
    size_t MaxByteOffset;

    fail_unless(Device_Initialize(NULL) == 0, "Device_initialize failed");
    // check if the function can handle invalid paramenters
    (void)Device_Read32(NULL, 0);
    (void)Device_Read32(NULL, 0xFFFFFFFF);
    (void)Device_Read32((Device_Handle_t)0xFFFF, 0);
    (void)Device_Read32((Device_Handle_t)0xFFFFFFFF, 0);
    (void)Device_Read32((Device_Handle_t)0x12345678, 0);

    // try to read some data from valid device, valid offset
    Device = Device_Find(DFTEST_DEVICE_DEV_RD_NAME);
    fail_if(Device == NULL, "Failed to find device");

    MaxByteOffset = DFTEST_DEVICE_DEV_RD_START_BYTES + DFTEST_DEVICE_DEV_RD_SIZE_BYTES;
    for (ByteOffset = DFTEST_DEVICE_DEV_RD_START_BYTES;
            ByteOffset < MaxByteOffset;
            ByteOffset += 4)
    {
        (void)Device_Read32(Device, ByteOffset);
    }

    // try to read some data from valid device, unaligned offset
    for (ByteOffset = 1;
            ByteOffset < 4;
            ByteOffset++)
    {
        (void)Device_Read32(Device, DFTEST_DEVICE_DEV_RD_START_BYTES + ByteOffset);
    }

#ifndef DFTEST_REMOVE_DEVICE_INVALID_LARGE_OFFSET
    // read data from invalid large offset
    (void)Device_Read32(Device, 0xFFFFFFFF);
    (void)Device_Read32(Device, 0xF0000000);
    (void)Device_Read32(Device, 0x80000000);
#endif

    Device_UnInitialize();

}
END_TEST

START_TEST(test_device_read32array)
{
    uint32_t WordArray[DFTEST_DEVICE_DEV_RD_SIZE_BYTES / sizeof(uint32_t)];
    Device_Handle_t Device;
    size_t ByteOffset;
    size_t MaxByteOffset;
    size_t WordCount;
    size_t Index;


    fail_unless(Device_Initialize(NULL) == 0, "Device_initialize failed");
    /* Check if then function handles invalid device handler */
    Device_Read32Array(NULL, 0, WordArray, 1);
    Device_Read32Array(NULL, 0xFFFFFFFF, WordArray, 1);
    Device_Read32Array((Device_Handle_t)0xFFFF, 0, WordArray, 1);
    Device_Read32Array((Device_Handle_t)0xFFFFFFFF, 0, WordArray, 1);
    Device_Read32Array((Device_Handle_t)0x12345678, 0, WordArray, 1);
    Device = Device_Find(DFTEST_DEVICE_DEV_RD_NAME);
    fail_if(Device == NULL, "Failed to find device");

    /* Check if the function handles invalid output buffer */
#ifndef DFTEST_REMOVE_DEVICE_INVALID_OUTPUT_BUF_TEST
    Device_Read32Array(Device, 0, NULL, 4);
#endif
    MaxByteOffset = DFTEST_DEVICE_DEV_RD_START_BYTES + DFTEST_DEVICE_DEV_RD_SIZE_BYTES;
    for (ByteOffset = DFTEST_DEVICE_DEV_RD_START_BYTES;
            ByteOffset < MaxByteOffset;
            ByteOffset += 4)
    {
        for (WordCount = 0;
                WordCount < ((MaxByteOffset - ByteOffset) >> 2);
                WordCount++)
        {
            uint8_t InitPattern;

            InitPattern = (uint8_t)(WordCount & 0xFF);
            c_memset(WordArray, InitPattern, sizeof(WordArray));
            Device_Read32Array(
                Device,
                ByteOffset,
                WordArray,
                WordCount);
            /* Check if overwriting occurs */
            for (Index = WordCount; Index < (sizeof(WordArray) >> 2); Index++)
            {
                fail_if(WordArray[Index] !=
                        (((uint32_t)InitPattern << 24) | ((uint32_t)InitPattern << 16)
                         | ((uint32_t)InitPattern << 8) | (uint32_t)InitPattern),
                        "Some data was overwritten, index %d", (int)Index);
            }
        }
    }

    //some negative tests
#ifndef DFTEST_REMOVE_DEVICE_UNALIGNED_OFF_TEST
    /* Check unaligned offsets */
    for (Index = 1; Index < 4; Index++)
    {
        Device_Read32Array(
            Device,
            DFTEST_DEVICE_DEV_RD_START_BYTES + Index,
            WordArray,
            1);
    }
#endif

    Device_Read32Array(Device, DFTEST_DEVICE_DEV_RD_START_BYTES, WordArray, 0);

#ifndef DFTEST_REMOVE_DEVICE_INVALID_LARGE_OFFSET
    Device_Read32Array(Device, 0xFFFFFFFF, WordArray, 1);
    Device_Read32Array(Device, 0xF0000000, WordArray, 1);
    Device_Read32Array(Device, 0x80000000, WordArray, 1);
#endif

#ifndef DFTEST_REMOVE_DEVICE_INVALID_OUTPUT_BUF_TEST
    Device_Read32Array(Device, DFTEST_DEVICE_DEV_RD_START_BYTES, NULL, 0);
#endif
    Device_UnInitialize();
}
END_TEST
#endif /* DFTEST_DEVICE_WRDEV_CAPS */

#if DFTEST_DEVICE_WRDEV_CAPS & \
        (DFTEST_DEVICE_DEV_WR_CAP | DFTEST_DEVICE_DEV_WRITE_ONLY_CAP)
START_TEST(test_device_write32)
{

    Device_Handle_t Device;
    size_t ByteOffset;
    size_t MaxByteOffset;
#ifndef DFTEST_REMOVE_DEVICE_MEM_WR_VERIFY
    uint32_t WordRead;
#endif

    fail_unless(Device_Initialize(NULL) == 0, "Device_initialize failed");
    // check if the function can handle invalid paramenters
    Device_Write32(NULL, 0, 0);
    Device_Write32(NULL, 0xFFFFFFFF, 0);
    Device_Write32((Device_Handle_t)0xFFFF, 0, 0);
    Device_Write32((Device_Handle_t)0xFFFFFFFF, 0, 0);
    Device_Write32((Device_Handle_t)0x12345678, 0, 0);

    // try to read some data from valid device, valid offset
    Device = Device_Find(DFTEST_DEVICE_DEV_WR_NAME);
    fail_if(Device == NULL, "Failed to find device");
    MaxByteOffset = DFTEST_DEVICE_DEV_WR_START_BYTES + DFTEST_DEVICE_DEV_WR_SIZE_BYTES;
    for (ByteOffset = DFTEST_DEVICE_DEV_WR_START_BYTES;
            ByteOffset < MaxByteOffset;
            ByteOffset += 4)
    {
        Device_Write32(Device, ByteOffset, ByteOffset);
    }

#ifndef DFTEST_REMOVE_DEVICE_MEM_WR_VERIFY
    for (ByteOffset = DFTEST_DEVICE_DEV_WR_START_BYTES;
            ByteOffset < MaxByteOffset;
            ByteOffset += 4)
    {
        WordRead = Device_Read32(Device, ByteOffset);
        fail_unless(WordRead == ByteOffset, "Device write error");
    }
#endif

    // try to write some data to valid device, unaligned offset
    for (ByteOffset = 1;
            ByteOffset < 4;
            ByteOffset++)
    {
        Device_Write32(
            Device,
            DFTEST_DEVICE_DEV_WR_START_BYTES + ByteOffset,
            ByteOffset);
    }

#ifndef DFTEST_REMOVE_DEVICE_INVALID_LARGE_OFFSET

    // write data to invalid large offset
    Device_Write32(Device, 0xFFFFFFFF, 0xAAAAAAAA);
    Device_Write32(Device, 0xF0000000, 0xAAAAAAAA);
    Device_Write32(Device, 0x80000000, 0xAAAAAAAA);

#endif

    Device_UnInitialize();
}
END_TEST

START_TEST(test_device_write32array)
{
    uint32_t WordArray[32 / sizeof(uint32_t)];
    Device_Handle_t Device;
    size_t ByteOffset;
    size_t WordCount;
    size_t WordCountLimit;
#ifndef DFTEST_REMOVE_DEVICE_UNALIGNED_OFF_TEST
    size_t Index;
#endif
    fail_unless(Device_Initialize(NULL) == 0, "Device_initialize failed");
    /* Check if then function handles invalid device handler */
    Device_Write32Array(NULL, 0, WordArray, 1);
    Device_Write32Array(NULL, 0xFFFFFFFF, WordArray, 1);
    Device_Write32Array((Device_Handle_t)0xFFFF, 0, WordArray, 1);
    Device_Write32Array((Device_Handle_t)0xFFFFFFFF, 0, WordArray, 1);
    Device_Write32Array((Device_Handle_t)0x12345678, 0, WordArray, 1);

    Device = Device_Find(DFTEST_DEVICE_DEV_WR_NAME);
    fail_if(Device == NULL, "Failed to find device");

#ifndef DFTEST_REMOVE_DEVICE_INVALID_OUTPUT_BUF_TEST
    /* Check if the function handles invalid output buffer */
    Device_Write32Array(Device, DFTEST_DEVICE_DEV_WR_START_BYTES, NULL, 4);
#endif

    WordCountLimit = DFTEST_DEVICE_DEV_WR_START_BYTES + DFTEST_DEVICE_DEV_WR_SIZE_BYTES;
    for (ByteOffset = DFTEST_DEVICE_DEV_WR_START_BYTES;
            ByteOffset < WordCountLimit;
            ByteOffset += 4)
    {
        for (WordCount = 0;
                WordCount < ((WordCountLimit - ByteOffset) >> 2);
                WordCount++)
        {
            Device_Write32Array(
                Device,
                ByteOffset,
                WordArray,
                WordCount);
        }
    }

    // some negative tests
#ifndef DFTEST_REMOVE_DEVICE_UNALIGNED_OFF_TEST
    /* Check unaligned offsets */
    for (Index = 1; Index < 4; Index++)
    {
        Device_Write32Array(
            Device,
            DFTEST_DEVICE_DEV_WR_START_BYTES + Index,
            WordArray,
            1);
    }
#endif
    Device_Write32Array(Device, DFTEST_DEVICE_DEV_WR_START_BYTES, WordArray, 0);

#ifndef DFTEST_REMOVE_DEVICE_INVALID_LARGE_OFFSET
    Device_Write32Array(Device, 0xFFFFFFFF, WordArray, 1);
    Device_Write32Array(Device, 0xF0000000, WordArray, 1);
    Device_Write32Array(Device, 0x80000000, WordArray, 1);
#endif

#ifndef DFTEST_REMOVE_DEVICE_INVALID_OUTPUT_BUF_TEST
    Device_Write32Array(Device, DFTEST_DEVICE_DEV_WR_START_BYTES, NULL, 0);
#endif

    Device_UnInitialize();
}
END_TEST
#endif /* DFTEST_DEVICE_WRDEV_CAPS */

START_TEST(test_device_swap_endian32)
{
    uint32_t NewWord;
    uint32_t Index;
    uint32_t OriginWord[] =
    {
        0x00000000U, 0x000000A5U, 0xA5000000U, 0x0000A5B4U,
        0xB4A50000U, 0x00C300D2U, 0x12345678U, 0xFFFFFFFFU
    };
    uint32_t SwappedWord[] =
    {
        0x00000000U, 0xA5000000U, 0x000000A5U, 0xB4A50000U,
        0x0000A5B4U, 0xD200C300U, 0x78563412U, 0xFFFFFFFFU
    };

    for (Index = 0; Index < sizeof(OriginWord) / sizeof(uint32_t); Index++)
    {
        NewWord = Device_SwapEndian32(OriginWord[Index]);
        fail_unless(NewWord == SwappedWord[Index],
                    "Swap endian32 error on word %d: 0x%x", Index, OriginWord[Index]);
    }

}
END_TEST

START_TEST(test_device_get_reference)
{
#ifndef DFTEST_REMOVE_DEVICE_GET_REFERENCE_TEST
    Device_Handle_t Device;
    Device_Reference_t DeviceReference;

    fail_unless(Device_Initialize(NULL) == 0, "Device_initialize failed");
    // check if the function can handle invalid paramenters
    DeviceReference = Device_GetReference(NULL);
    fail_unless(DeviceReference == NULL,
                "Should not get reference from invalid device handle");

    // try to get reference from valid device
    Device = Device_Find(DFTEST_VALID_DEV_NAME);
    fail_if(Device == NULL, "Failed to find device");
    DeviceReference = Device_GetReference(Device);
    fail_if(DeviceReference == NULL, "Reference was invalid");
    Device_UnInitialize();
#endif /* !DFTEST_REMOVE_DEVICE_GET_REFERENCE_TEST */

}
END_TEST


void suite_add_test_device_apis(void)
{
    sfzutf_tcase_create("Device_API_Tests");
    sfzutf_test_add(test_device_init_uninit);
    sfzutf_test_add(test_device_find);
#if DFTEST_DEVICE_WRDEV_CAPS & \
        (DFTEST_DEVICE_DEV_WR_CAP | DFTEST_DEVICE_DEV_READ_ONLY_CAP)
    sfzutf_test_add(test_device_read32);
    sfzutf_test_add(test_device_read32array);
#endif
#if DFTEST_DEVICE_WRDEV_CAPS & \
        (DFTEST_DEVICE_DEV_WR_CAP | DFTEST_DEVICE_DEV_WRITE_ONLY_CAP)
    sfzutf_test_add(test_device_write32);
    sfzutf_test_add(test_device_write32array);
#endif
    sfzutf_test_add(test_device_swap_endian32);
    sfzutf_test_add(test_device_get_reference);
}

#endif /* DFTEST_REMOVE_DEVICE_APIS */

/* end of file dftest_device.c */
