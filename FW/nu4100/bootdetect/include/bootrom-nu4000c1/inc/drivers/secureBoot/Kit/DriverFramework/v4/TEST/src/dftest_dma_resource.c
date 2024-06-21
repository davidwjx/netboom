/* dftest_dma_resource.c
 *
 * Description: test DMA resource APIs of Driver Framework.
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
#include "dmares_mgmt.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"
#include "device_swap.h"
#include "sfzutf.h"
#include "cs_dftest.h"

#define ELEMENTS_COUNT(_x) (sizeof(_x) / sizeof(_x[0]))

#ifndef DFTEST_REMOVE_DMA_RESOURCE_APIS

/* Test DMAResource_Init and DMAResource_UnInit */
START_TEST(test_dr_init_uninit)
{
    bool Ret;

    Ret = DMAResource_Init();
    fail_unless(Ret, "Failed to initialize DMA resource");
    /* Check if calling DMAResource_Init again will cause any problem */
    DMAResource_Init();
    DMAResource_UnInit();

    Ret = DMAResource_Init();
    fail_unless(Ret,
                "Failed to initialize DMA resource after uninitialized");
    DMAResource_UnInit();
    /* Check if calling DMAResource_UnInit again will cause any problem */
    DMAResource_UnInit();

}
END_TEST

#ifndef DFTEST_REMOVE_DMARES_RECORD_APIS
static bool
test_dr_create_destroy_record_helper()
{
    DMAResource_Handle_t Handle[DFTEST_DMARES_RECORDS_LIMIT];
    size_t Index;
    size_t RecordNum;

    // create all records
    for (Index = 0; Index < DFTEST_DMARES_RECORDS_LIMIT; Index++)
    {
        Handle[Index] = DMAResource_CreateRecord();
        if (Handle[Index] == NULL)
        {
            // We used up all records here
            break;
        }
    }
    if (Index == 0)
    {
        return false;
    }
    RecordNum = Index;
    // destroy the records in reverse order
    for (Index = 0; Index < RecordNum - 1; Index++)
    {
        DMAResource_DestroyRecord(Handle[RecordNum - 1 - Index]);
    }

    // create all records again
    for (Index = 0; Index < DFTEST_DMARES_RECORDS_LIMIT; Index++)
    {
        Handle[Index] = DMAResource_CreateRecord();
        if (Handle[Index] == NULL)
        {
            // We use up all records here
            break;
        }
    }

    RecordNum = Index;
    // destroy the selected records (skip 1, destroy 1)
    for (Index = 0; Index < RecordNum / 2; Index++)
    {
        DMAResource_DestroyRecord(Handle[Index * 2 + 1]);
    }

    // destroy the remaining records
    for (Index = 0; Index < RecordNum / 2; Index++)
    {
        DMAResource_DestroyRecord(Handle[Index * 2]);
    }

    //destroy the last record if the counter of record is odd
    if (RecordNum & 0x00000001)
    {
        DMAResource_DestroyRecord(Handle[RecordNum - 1]);
    }

    return true;
}

/* Test DMAResource_CreateRecord, DMAResource_DestroyRecord and
   test_dr_is_valid_handle */
START_TEST(test_dr_create_destroy_valid_record)
{
    DMAResource_Handle_t Handle = NULL;
    DMAResource_Record_t * RecordPtr;
    bool IsValidHandle;
    bool IsInitOK;
    uint32_t RecordCount;
    int32_t InvalidHandleContent[] = { -1, -255};

    /* Test creating a record before initialization  */
    Handle = DMAResource_CreateRecord();
    fail_unless(Handle == NULL, "Create Record failed");

    /* Test destroying record by garbage handle */
    DMAResource_DestroyRecord(NULL);
    DMAResource_DestroyRecord(DFTEST_DMARES_BAD_HANDLE_0);
    DMAResource_DestroyRecord(DFTEST_DMARES_BAD_HANDLE_1);
    DMAResource_DestroyRecord(DFTEST_DMARES_BAD_HANDLE_2);
    DMAResource_DestroyRecord((DMAResource_Handle_t)&InvalidHandleContent[0]);
    DMAResource_DestroyRecord((DMAResource_Handle_t)&InvalidHandleContent[1]);

#ifndef DFTEST_REMOVE_DMARES_IS_VALID_HANDLE
    /* Test DMAResource_IsValidHandle by garbage handle */
    IsValidHandle = DMAResource_IsValidHandle(NULL);
    fail_if(IsValidHandle, "NULL should not be valid handle");

    IsValidHandle = DMAResource_IsValidHandle(DFTEST_DMARES_BAD_HANDLE_0);
    fail_if(IsValidHandle, "Should not be valid handle");

    IsValidHandle = DMAResource_IsValidHandle(DFTEST_DMARES_BAD_HANDLE_1);
    fail_if(IsValidHandle, "Should not be valid handle");

    IsValidHandle = DMAResource_IsValidHandle(DFTEST_DMARES_BAD_HANDLE_2);
    fail_if(IsValidHandle, "Should not be valid handle");

    IsValidHandle =
        DMAResource_IsValidHandle((DMAResource_Handle_t)&InvalidHandleContent[0]);
    fail_if(IsValidHandle, "&InvalidHandleContent[0] should not be valid handle");

    IsValidHandle =
        DMAResource_IsValidHandle((DMAResource_Handle_t)&InvalidHandleContent[1]);
    fail_if(IsValidHandle, "&InvalidHandleContent[1] should not be valid handle");
#endif

    IsInitOK = DMAResource_Init();
    fail_unless(IsInitOK, "Failed to initialize DMA resource");
    Handle = DMAResource_CreateRecord();
    fail_if(Handle == NULL, "Create Record failed");
#ifndef DFTEST_REMOVE_DMARES_IS_VALID_HANDLE
    /* Check if the handle is valid */
    IsValidHandle = DMAResource_IsValidHandle(Handle);
    fail_unless(IsValidHandle, "Not valid handle");
#endif
    RecordPtr = DMAResource_Handle2RecordPtr(Handle);
    fail_if(RecordPtr == NULL, "Record pointer was not valid");
    DMAResource_DestroyRecord(Handle);
    /* Check if it will cause problems when we call the function again */
    DMAResource_DestroyRecord(Handle);

#ifndef DFTEST_REMOVE_DMARES_IS_VALID_HANDLE
    /* Check if the handle is NOT valid after destroyed */
    IsValidHandle = DMAResource_IsValidHandle(Handle);
    fail_if(IsValidHandle, "Handle should not valid after the record was destroyed");
#endif
    // find record limit by allocating records until running out.
    RecordCount = 0;
    do
    {
        Handle = DMAResource_CreateRecord();
        RecordCount++;
    }
    while ((RecordCount < DFTEST_DMARES_MAX_RECORDS_TO_CHECK) &&
            (Handle != NULL));

    if (Handle == NULL)
    {
        RecordCount--;
        // Report limit found.
        L_DEBUG(LF_DFTEST,
                "Limit was found. The maximum supported numbers of records are %d",
                (int)RecordCount);
    }
    DMAResource_UnInit();

    // test implementation of records
    IsInitOK = DMAResource_Init();
    fail_unless(IsInitOK, "Failed to initialize DMA resource");
    test_dr_create_destroy_record_helper();
    DMAResource_UnInit();
}
END_TEST

/* Test DMAResource_Handle2RecordPtr */
START_TEST(test_dr_handle2record)
{
    DMAResource_Handle_t Handle[8];
    DMAResource_Record_t * RecordPtr;
    uint32_t Index;
    uint32_t ByteOffset;

    fail_unless(DMAResource_Init(), "Failed to initialize DMA resource");
    RecordPtr = DMAResource_Handle2RecordPtr(NULL);
    fail_unless(RecordPtr == NULL,
                "NULL pointer should not be converted to valid record pointer");
    for (Index = 0; Index < ELEMENTS_COUNT(Handle); Index++)
    {
        Handle[Index] = DMAResource_CreateRecord();
        fail_if(Handle[Index] == NULL, "Create Record failed");
        RecordPtr = DMAResource_Handle2RecordPtr(Handle[Index]);
        fail_if(RecordPtr == NULL, "Record pointer was not valid");
        // Write the record memory
        c_memset(RecordPtr, (uint8_t)Index, sizeof(DMAResource_Record_t));
    }

    // now check that each record is still uniquely filled with the pattern
    for (Index = 0; Index < ELEMENTS_COUNT(Handle); Index++)
    {
        RecordPtr = DMAResource_Handle2RecordPtr(Handle[Index]);
        fail_if(RecordPtr == NULL, "Record pointer was not valid");

        for (ByteOffset = 0; ByteOffset < sizeof(DMAResource_Record_t); ByteOffset++)
        {
            fail_if(((uint8_t *)RecordPtr)[ByteOffset] != Index,
                    "Content of record is not valid");
        }
        DMAResource_DestroyRecord(Handle[Index]);
    }

    DMAResource_UnInit();
}
END_TEST
#endif /* DFTEST_REMOVE_DMARES_RECORD_APIS */

static int
test_dr_alloc_valid_parameters(void)
{
    DMAResource_Handle_t Handle;
    DMAResource_AddrPair_t AddrPair;
    const DMAResource_Properties_t DMAResProps[] = DFTEST_VALID_PROPS;
    const DMAResource_AddrDomain_t ValidDomain[] = {DFTEST_DMARES_ALLOC_ADDRDOMAINS};
    size_t Index;
    size_t DomainIdx;
    size_t PropsNum;
    int Result;

    PropsNum = ELEMENTS_COUNT(DMAResProps);
    for (Index = 0; Index < PropsNum; Index++)
    {
        Result = DMAResource_Alloc(
                     DMAResProps[Index],
                     &AddrPair,
                     &Handle);
        if (Result != 0)
        {
            return -1;
        }

#ifndef DFTEST_REMOVE_DMARES_IS_VALID_HANDLE
        // check if Handle is valid
        if (!DMAResource_IsValidHandle(Handle))
        {
            return -2;
        }
#endif
        // check if AddrPair is valid
        for (DomainIdx = 0; DomainIdx < ELEMENTS_COUNT(ValidDomain); DomainIdx++)
        {
            if (AddrPair.Domain == ValidDomain[DomainIdx])
            {
                break;
            }
        }

        // if the output domain of AddrPair was not in valid domain list,
        // return false.
        if (DomainIdx >= ELEMENTS_COUNT(ValidDomain))
        {
            return -3;
        }

        DMAResource_Release(Handle);
    }
    return 0;
}

#ifndef DFTEST_REMOVE_DMARES_INVALID_PROPS
static bool
test_dr_alloc_invalid_parameters(void)
{
    DMAResource_Handle_t Handle;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps[] = DFTEST_INVALID_PROPS;
    size_t Index;
    size_t PropsNum;
    int Result;

    PropsNum = ELEMENTS_COUNT(DMAResProps);
    for (Index = 0; Index < PropsNum; Index++)
    {
        Result = DMAResource_Alloc(
                     DMAResProps[Index],
                     &AddrPair,
                     &Handle);
        if (Result == 0)
        {
            return false;
        }
    }
    return true;
}
#endif /* DFTEST_REMOVE_DMARES_INVALID_PROPS */

#ifndef DFTEST_REMOVE_DMARES_RECORD_APIS
static bool
test_dr_alloc_wo_avaliable_record(void)
{
    DMAResource_Handle_t Handle;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps[] = DFTEST_VALID_PROPS;
    int Result;

    // use up all the free records
    do
    {
        Handle = DMAResource_CreateRecord();
    }
    while (Handle != NULL);

    Result = DMAResource_Alloc(
                 DMAResProps[0],
                 &AddrPair,
                 &Handle);

    return (Result == 0) ? false : true;
}
#endif /* DFTEST_REMOVE_DMARES_RECORD_APIS */

#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
/* Remove specified AddrPairs from record */
static void
dr_remove_addrpair(
    const DMAResource_Handle_t Handle,
    const DMAResource_AddrDomain_t Domain)
{
    DMAResource_Record_t * RecordPtr;
    size_t Index;
    DMAResource_AddrPair_t * AddrPair;

    if (Handle == NULL)
    {
        return;
    }
    RecordPtr = DMAResource_Handle2RecordPtr(Handle);
    fail_if(
        RecordPtr == NULL,
        "Cannot remove Domain %d due to bad handle\n", (int)Domain);

    AddrPair = RecordPtr->AddrPairs;

    for (Index = 0; Index < DMARES_ADDRPAIRS_CAPACITY; Index++)
    {
        if (AddrPair->Domain == Domain)
        {
            AddrPair->Domain = 0;
            AddrPair->Address_p = NULL;
        }
        AddrPair++;
    }
}

/* Overwrite the magic of record for negative test purpose */
static uint32_t
dr_overwrite_record_magic(
    const DMAResource_Handle_t Handle,
    const uint32_t NewMagic)
{
    DMAResource_Record_t * RecordPtr;
    uint32_t OriginMagic;

    if (Handle == NULL)
    {
        return 0;
    }

    RecordPtr = DMAResource_Handle2RecordPtr(Handle);
    fail_if(
        RecordPtr == NULL,
        "Cannot overwrite magic due to bad handle\n");
    OriginMagic = RecordPtr->Magic;
    RecordPtr->Magic = NewMagic;

    return OriginMagic;
}
#endif

/* Test DMAResource_Alloc */
START_TEST(test_dr_alloc)
{
    int Result;
#ifndef DFTEST_REMOVE_DMARES_INVALID_PROPS
    bool IsTestPassed;
#endif
    DMAResource_Handle_t Handle;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};

    fail_unless(DMAResource_Init(), "Failed to initialize DMA resource");

    /* Check if it handles invalid parameters */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 NULL,
                 &Handle);
    fail_if(Result == 0, "Should not work when AddrPair_p is NULL");

    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 NULL);
    fail_if(Result == 0, "Should not work when Handle_p is NULL");

    /* Test the function with valid parameters */
    Result = test_dr_alloc_valid_parameters();
    fail_if(Result == -1, "Should work with valid parameters");
    fail_if(Result == -2, "Invalid handle");
    fail_if(Result == -3, "Invalid domain");

#ifndef DFTEST_REMOVE_DMARES_INVALID_PROPS
    /* Test the function with invalid parameters */
    IsTestPassed = test_dr_alloc_invalid_parameters();
    fail_unless(IsTestPassed, "Should work with invalid parameters");
#endif

#ifndef DFTEST_REMOVE_DMARES_RECORD_APIS
    /* Test the function when no record is avaliable */
    IsTestPassed = test_dr_alloc_wo_avaliable_record();
    fail_unless(IsTestPassed, "Should not work when records were not available");
#endif

    DMAResource_UnInit();
}
END_TEST

/* Test DMAResource_Release */
START_TEST(test_dr_release)
{
    DMAResource_Handle_t AllocHandle;
#ifndef DFTEST_REMOVE_DMARES_RELEASE_RES_FROM_ATTACH
    DMAResource_Handle_t AttachHandle;
#endif
    DMAResource_Handle_t RegHandle;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
    int Result;

    fail_unless(DMAResource_Init(), "Failed to initialize DMA resource");

    /* Check if the function releases resource from DMAResource_Alloc correctly */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &AllocHandle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");

    Result = DMAResource_Release(AllocHandle);
    fail_unless(Result == 0,
                "Failed to release DMA resource");
#ifndef DFTEST_REMOVE_DMARES_IS_VALID_HANDLE
    /* After DMA resource was released, the handle should not be valid */
    if (DMAResource_IsValidHandle(AllocHandle))
    {
        fail("The handle should not be valid");
    }
#endif
    /* Try to release the resource again */
    Result = DMAResource_Release(AllocHandle);
    fail_if(Result == 0,
            "Should not release DMA resource again which has been released");

    /* Check if the function releases resource from
       DMAResource_CheckAndRegister correctly */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &AllocHandle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");

    Result = DMAResource_CheckAndRegister(
                 DMAResProps,
                 AddrPair,
                 DFTEST_DMARES_GOOD_ALLOC_REF,
                 &RegHandle);
    fail_unless(Result == 0,
                "Failed to register DMA Resource");

    Result = DMAResource_Release(RegHandle);
    fail_unless(Result == 0,
                "Failed to release registered DMA resource");

    Result = DMAResource_Release(AllocHandle);
    fail_unless(Result == 0,
                "Failed to release allocated DMA resource");

#ifndef DFTEST_REMOVE_DMARES_IS_VALID_HANDLE
    /* After DMA resource was released, the handle should not be valid */
    if (DMAResource_IsValidHandle(RegHandle))
    {
        fail("The handle should not be valid");
    }
#endif
    /* Try to release the resource again */
    Result = DMAResource_Release(RegHandle);
    fail_if(Result == 0,
            "Should not release DMA resource again which has been released");

#ifndef DFTEST_REMOVE_DMARES_RELEASE_RES_FROM_ATTACH
    /* Check if the function releases resource from DMAResource_Attach correctly */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &AllocHandle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");

    Result = DMAResource_Translate(
                 AllocHandle,
                 DMARES_DOMAIN_BUS,
                 &AddrPair);
    fail_unless(Result == 0, "Failed to translate to DMARES_DOMAIN_BUS address");

    Result = DMAResource_Attach(
                 DMAResProps,
                 AddrPair,
                 &AttachHandle);
    fail_unless(Result == 0, "Failed to attach valid address");

    Result = DMAResource_Release(AttachHandle);
    fail_unless(Result == 0, "Failed to release attached DMA resource");

    Result = DMAResource_Release(AllocHandle);
    fail_unless(Result == 0, "Failed to release allocated DMA resource");

#ifndef DFTEST_REMOVE_DMARES_IS_VALID_HANDLE
    /* After DMA resource was released, the handle should not be valid */
    if (DMAResource_IsValidHandle(AttachHandle))
    {
        fail("The handle should not be valid");
    }
#endif
    /* Try to release the resource again */
    Result = DMAResource_Release(AttachHandle);
    fail_if(Result == 0,
            "Should not release DMA resource again which has been released");
#endif /* DFTEST_REMOVE_DMARES_RELEASE_RES_FROM_ATTACH */

    /* negative tests */
    Result = DMAResource_Release(NULL);
    fail_if(Result == 0,
            "Should not release a NULL handler");
    DMAResource_UnInit();
}
END_TEST

#ifndef DFTEST_REMOVE_DMARES_CHECKANDREGISTER
static int
test_dr_check_and_reg_invalid_paras(void)
{
    DMAResource_Handle_t Handle;
    DMAResource_Handle_t AllocHandle;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
    uint8_t LocalBuf[DFTEST_DMA_RES_BYTES];
    int Result;

    /* Check if the function handles invalid parameters well */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    AddrPair.Domain = DMARES_DOMAIN_HOST;
    AddrPair.Address.Native_p = (void *)LocalBuf;
    Result = DMAResource_CheckAndRegister(
                 DMAResProps,
                 AddrPair,
                 DFTEST_DMARES_GOOD_ALLOC_REF,
                 &Handle);
    if (Result == 0)
    {
        /* Should not accept invalid address */
        return -1;
    }

    AddrPair.Address.Native_p = NULL;
    Result = DMAResource_CheckAndRegister(
                 DMAResProps,
                 AddrPair,
                 DFTEST_DMARES_GOOD_ALLOC_REF,
                 &Handle);
    if (Result == 0)
    {
        /* Should not accept NULL address */
        return -2;
    }

    /* Allocate DMA Resource, so we have a valid DMA buffer */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &AllocHandle);
    if (Result != 0)
    {
        /* Failed to allocated DMA Resource */
        return -3;
    }

    /* Continue invalid parameters checking, AddrPair has an illegal domain */
    AddrPair.Domain = DFTEST_DMARES_INVALID_DOMAIN;
    Result = DMAResource_CheckAndRegister(
                 DMAResProps,
                 AddrPair,
                 DFTEST_DMARES_GOOD_ALLOC_REF,
                 &Handle);
    if (Result == 0)
    {
        /* Should not accept illegal domain */
        DMAResource_Release(AllocHandle);
        return -4;
    }
    /* Restore the domain */
    AddrPair.Domain = DMARES_DOMAIN_HOST;

#ifndef DFTEST_REMOVE_DMARES_REGISTER_DIFFERENT_BANK
    /* Register with different Bank */
    DMAResProps.Bank = DFTEST_DMARES_INVALID_BANK;
    Result = DMAResource_CheckAndRegister(
                 DMAResProps,
                 AddrPair,
                 DFTEST_DMARES_GOOD_ALLOC_REF,
                 &Handle);
    if (Result == 0)
    {
        /* Should not register buffer in another bank */
        DMAResource_Release(AllocHandle);
        return -5;
    }
    /* Restore Bank to 0 */
    DMAResProps.Bank = 0;
#endif

    /* If the length of buffer is 0 */
    DMAResProps.Size = 0;
    Result = DMAResource_CheckAndRegister(
                 DMAResProps,
                 AddrPair,
                 DFTEST_DMARES_GOOD_ALLOC_REF,
                 &Handle);
    if (Result == 0)
    {
        /* Should not accept 0 size buffer */
        DMAResource_Release(AllocHandle);
        return -6;
    }

    /* Restore size of DMA resource */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    /* Bad Allocator Ref */
    Result = DMAResource_CheckAndRegister(
                 DMAResProps,
                 AddrPair,
                 DFTEST_DMARES_BAD_ALLOC_REF,
                 &Handle);
    if (Result == 0)
    {
        /* Should not accept illegal Allocator Ref */
        DMAResource_Release(AllocHandle);
        return -7;
    }
    /* Unaligned address */
    AddrPair.Address.Native_p = (void *)(((uint8_t *)AddrPair.Address.Native_p) + 2);
    Result = DMAResource_CheckAndRegister(
                 DMAResProps,
                 AddrPair,
                 DFTEST_DMARES_GOOD_ALLOC_REF,
                 &Handle);
    /* Restore Address */
    AddrPair.Address.Native_p = (void *)(((uint8_t *)AddrPair.Address.Native_p) - 2);
    if (Result == 0)
    {
        /* Should not register unaligned buffer */
        DMAResource_Release(AllocHandle);
        return -8;
    }

    DMAResource_Release(AllocHandle);
    return 0;

}

/* Test DMAResource_CheckAndRegister */
START_TEST(test_dr_check_and_register)
{
    DMAResource_Handle_t Handle;
    DMAResource_Handle_t AllocHandle;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
    int Result;
    uint32_t Index;

    Result = DMAResource_Init();
    fail_unless(Result, "Failed to initialize DMA resource");

    /* Check some invalid parameters */
    Result = test_dr_check_and_reg_invalid_paras();
    fail_if(Result == -1, "Should not accept invalid address");
    fail_if(Result == -2, "Should not accept NULL address");
    fail_if(Result == -3, "Failed to allocated DMA Resource");
    fail_if(Result == -4, "Should not accept illegal domain");
    fail_if(Result == -5, "Should not register buffer in another bank");
    fail_if(Result == -6, "Should not accept 0 size buffer");
    fail_if(Result == -7, "Should not accept illegal Allocator Ref");
    fail_if(Result == -8, "Should not register unaligned buffer");

    /* Allocate DMA Resource, so we have a valid DMA buffer */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &AllocHandle);
    fail_unless(Result == 0, "Failed to allocated DMA Resource");

    /* Check if the function works with valid DMA buffers.
       The buffers might be a part of allocated DMA buffers */
    /* Register the whole buffer for 4 times */
    for (Index = 0; Index < 4; Index ++)
    {
        Result = DMAResource_CheckAndRegister(
                     DMAResProps,
                     AddrPair,
                     DFTEST_DMARES_GOOD_ALLOC_REF,
                     &Handle);
        fail_unless(Result == 0,
                    "Failed to register DMA Resource, Index = %d", Index);
#ifndef DFTEST_REMOVE_DMARES_IS_VALID_HANDLE
        /* Check Handle output by the function */
        fail_unless(DMAResource_IsValidHandle(Handle),
                    "The handle was invalid");
#endif
        Result = DMAResource_Release(Handle);
        fail_unless(Result == 0, "Failed to release registered DMA resource");
    }

    /* Register a part of an allocated buffer, start address is same as the buffer */
    for (Index = 0; Index < 4; Index ++)
    {
        DMAResProps.Size = DFTEST_DMA_RES_BYTES >> (Index + 1);
        /* make sure buffer size is at least 4 bytes */
        if (DMAResProps.Size < 4)
        {
            DMAResProps.Size = 4;
        }
        Result = DMAResource_CheckAndRegister(
                     DMAResProps,
                     AddrPair,
                     DFTEST_DMARES_GOOD_ALLOC_REF,
                     &Handle);
        fail_unless(Result == 0,
                    "Failed to register DMA Resource, Index = %d", Index);
#ifndef DFTEST_REMOVE_DMARES_IS_VALID_HANDLE
        /* Check Handle output by the function */
        fail_unless(DMAResource_IsValidHandle(Handle),
                    "The handle was invalid");
#endif
        Result = DMAResource_Release(Handle);
        fail_unless(Result == 0, "Failed to release registered DMA resource");
    }

    /* Register a part of an allocated buffer, start address is different with
       the buffer */
    for (Index = 0; Index < 4; Index ++)
    {
        DMAResProps.Size = DFTEST_DMA_RES_BYTES / 2;
        AddrPair.Address.Native_p = (void *)((char *)AddrPair.Address.Native_p + Index * 4);
        Result = DMAResource_CheckAndRegister(
                     DMAResProps,
                     AddrPair,
                     DFTEST_DMARES_GOOD_ALLOC_REF,
                     &Handle);
        fail_unless(Result == 0,
                    "Failed to register DMA Resource, Index = %d", Index);
#ifndef DFTEST_REMOVE_DMARES_IS_VALID_HANDLE
        /* Check Handle output by the function */
        fail_unless(DMAResource_IsValidHandle(Handle),
                    "The handle was invalid");
#endif
        Result = DMAResource_Release(Handle);
        fail_unless(Result == 0, "Failed to release registered DMA resource");
    }

    /* Other parameters are OK except Handle */
    Result = DMAResource_CheckAndRegister(
                 DMAResProps,
                 AddrPair,
                 DFTEST_DMARES_GOOD_ALLOC_REF,
                 NULL);
    fail_if(Result == 0, "Should not accept NULL pointer Handle");

    DMAResource_Release(AllocHandle);
    DMAResource_UnInit();
}
END_TEST
#endif /* DFTEST_REMOVE_DMARES_CHECKANDREGISTER */

#ifndef DFTEST_REMOVE_DMARES_ATTACH
/* Test DMAResource_Attach */
START_TEST(test_dr_attach)
{
    DMAResource_Handle_t HostAHandle;
    DMAResource_Handle_t HostBHandle;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
    int Result;
#ifndef DFTEST_DMARES_ATTACH_IS_STUBBED_TEST
    uint32_t BufferSize;
#endif

    if(!DMAResource_Init())
    {
        fail("Failed to initialize DMA resource");
    }

    /* Allocate DMA Resource, so we have a valid DMA buffer */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &HostAHandle);
    fail_unless(Result == 0, "Failed to allocated DMA Resource");

    /* Get an BUS domain address pair */
    Result = DMAResource_Translate(
                 HostAHandle,
                 DMARES_DOMAIN_BUS,
                 &AddrPair);
    fail_unless(Result == 0, "Failed to translate to DMARES_DOMAIN_BUS address");

#ifndef DFTEST_DMARES_ATTACH_IS_STUBBED_TEST
    /* Check if the buffer can be attached */
    Result = DMAResource_Attach(
                 DMAResProps,
                 AddrPair,
                 &HostBHandle);
    fail_unless(Result == 0, "Failed to attach valid address");

    Result = DMAResource_Release(HostBHandle);
    fail_unless(Result == 0, "Failed to release attached DMA resource");

    Result = DMAResource_Release(HostAHandle);
    fail_unless(Result == 0, "Failed to release allocated DMA resource");

    /* Check if the function can handle invalid parameters */
    Result = DMAResource_Attach(
                 DMAResProps,
                 AddrPair,
                 NULL);
    fail_if(Result == 0, "It should not work when Handle was NULL pointer");

    BufferSize = DMAResProps.Size;
    DMAResProps.Size = 0;
    Result = DMAResource_Attach(
                 DMAResProps,
                 AddrPair,
                 &HostBHandle);
    fail_if(Result == 0, "It should not work when buffer size is 0");

    DMAResProps.Size = BufferSize;
    AddrPair.Address.Native_p = NULL;
    Result = DMAResource_Attach(
                 DMAResProps,
                 AddrPair,
                 &HostBHandle);
    fail_if(Result == 0, "It should not work when buffer address is NULL");
#else
    (void)DMAResource_Attach(
        DMAResProps,
        AddrPair,
        &HostBHandle);

    Result = DMAResource_Release(HostAHandle);
    fail_unless(Result == 0, "Failed to release allocated DMA resource");
#endif /* DFTEST_DMARES_ATTACH_IS_STUBBED_TEST */
    DMAResource_UnInit();
}
END_TEST
#endif /* DFTEST_REMOVE_DMARES_ATTACH */

/* Test DMAResource_Read32 */
START_TEST(test_dr_read32)
{
    DMAResource_Handle_t Handle = NULL;
    uint32_t LocalArray[DFTEST_DMA_RES_WORDS];
    uint32_t Index;
    uint32_t ReadWord;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
    int Result;
#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    uint32_t SavedMagic;
#endif

    if(!DMAResource_Init())
    {
        fail("Failed to initialize DMA resource");
    }

    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &Handle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");
    fail_if(Handle == NULL, "Invalid handle from DMAResource_Alloc");

    /* Check if the function handles invalid parameters well */
    DMAResource_Read32(NULL, 0);
    DMAResource_Read32(Handle, 0xFFFFFFFF);

    /* Check if the function works as defined */
    for (Index = 0; Index < DFTEST_DMA_RES_WORDS; Index++)
    {
        LocalArray[Index] = DMAResource_Read32(Handle, Index);
    }
    /* Check if the data read from DMA buffer twice is identical */
    for (Index = 0; Index < DFTEST_DMA_RES_WORDS; Index++)
    {
        ReadWord = DMAResource_Read32(Handle, Index);
        fail_unless(LocalArray[Index] == ReadWord, "Data not identical at second read");
    }

#ifndef DFTEST_REMOVE_DMARES_READ_CHECK_ENDIAN
    /* Check if the function handles endianess correctly */
    Result = DMAResource_SwapEndianess_Set(Handle, true);
    fail_unless(Result == 0, "Failed to set endianess swapping");

    for (Index = 0; Index < DFTEST_DMA_RES_WORDS; Index++)
    {
        ReadWord = DMAResource_Read32(Handle, Index);
        if (Device_SwapEndian32(LocalArray[Index]) != ReadWord)
        {
            fail("Data not identical");
        }
    }
#endif

#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    /* Break the record magic, then try to read */
    SavedMagic = dr_overwrite_record_magic(Handle, 0);
    (void)DMAResource_Read32(Handle, 0);
    /* Restore the magic back so we can continue next tests */
    (void)dr_overwrite_record_magic(Handle, SavedMagic);
    /* Remove the Domain HOST, then try to read */
    dr_remove_addrpair(Handle, DMARES_DOMAIN_HOST);
    (void)DMAResource_Read32(Handle, 0);
#endif

    DMAResource_Release(Handle);
    DMAResource_UnInit();
}
END_TEST

/* Test DMAResource_Write32 */
START_TEST(test_dr_write32)
{
    DMAResource_Handle_t Handle = NULL;
    uint32_t Index;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
    int Result;
    uint32_t * pDmaBuf;

    if(!DMAResource_Init())
    {
        fail("Failed to initialize DMA resource");
    }

    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &Handle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");
    fail_if(Handle == NULL, "Invalid handle from DMAResource_Alloc");
    pDmaBuf = (uint32_t *)AddrPair.Address.Native_p;
    fail_if(pDmaBuf == NULL, "Invalid pointer from output DMAResource_AddrPair_t");

    /* Check if the function handles invalid parameters well */
    DMAResource_Write32(NULL, 0, 0);
    DMAResource_Write32(Handle, 0xFFFFFFFFU, 0);

    /* Initialize the local array */
    for (Index = 0; Index < DFTEST_DMA_RES_WORDS; Index++)
    {
        DMAResource_Write32(Handle, Index, Index);
        fail_unless(Index == pDmaBuf[Index], "DMA resource word write error");
    }

#ifndef DFTEST_REMOVE_DMARES_WRITE_CHECK_ENDIAN
    /* Check if the function works correctly when endianess need to be
       swapped */
    Result = DMAResource_SwapEndianess_Set(Handle, true);
    fail_unless(Result == 0, "Failed to set endianess swapping");

    for (Index = 0; Index < DFTEST_DMA_RES_WORDS; Index++)
    {
        DMAResource_Write32(Handle, Index, Index);
        if (Device_SwapEndian32(Index) != pDmaBuf[Index])
        {
            fail("DMA resource word write error (endianess mismatch)");
        }
    }
#endif

#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    dr_remove_addrpair(Handle, DMARES_DOMAIN_HOST);
    DMAResource_Write32(Handle, 0, 0);
#endif

    DMAResource_Release(Handle);
    DMAResource_UnInit();
}
END_TEST

#ifndef DFTEST_REMOVE_DMARES_READ32ARRAY
/* Test DMAResource_Read32Array */
START_TEST(test_dr_read32array)
{
    DMAResource_Handle_t Handle = NULL;
    uint32_t LocalArray[DFTEST_DMA_RES_WORDS];
    uint32_t WordOff;
    uint32_t WordOff2;
    uint32_t WordCount;
    uint32_t Index;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
    int Result;
    uint32_t *pDmaBuf;

    if(!DMAResource_Init())
    {
        fail("Failed to initialize DMA resource");
    }

    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &Handle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");
    fail_if(Handle == NULL, "Invalid handle from DMAResource_Alloc");
    pDmaBuf = (uint32_t *)AddrPair.Address.Native_p;
    fail_if(pDmaBuf == NULL, "Invalid pointer from output DMAResource_AddrPair_t");

    c_memset(LocalArray, 0, sizeof(LocalArray));

    /* Check if the function handles invalid parameters well */
    DMAResource_Read32Array(NULL, 0, 1, LocalArray);
#ifndef DFTEST_REMOVE_DMARES_INVALID_OUTPUT_BUF_TEST
    DMAResource_Read32Array(Handle, 0, 1, NULL);
#endif
    DMAResource_Read32Array(Handle, 0, 0, LocalArray);
    DMAResource_Read32Array(Handle, 0, 0x7FFFFFFFUL, LocalArray);
    DMAResource_Read32Array(Handle, 0xFFFFFFFF, 1, LocalArray);
    /* Check if the function can read correct values from whole buffer */
    for (Index = 0; Index < 4; Index++)
    {
        for (WordOff = 0; WordOff < DFTEST_DMA_RES_WORDS; WordOff++)
        {
            pDmaBuf[WordOff] = WordOff + (Index << 24);
        }

        DMAResource_Read32Array(
            Handle,
            0,
            DFTEST_DMA_RES_WORDS,
            LocalArray);

        for (WordOff = 0; WordOff < DFTEST_DMA_RES_WORDS; WordOff++)
        {
            fail_unless(LocalArray[WordOff] == WordOff +  (Index << 24),
                        "Data was not read correctly");
        }
    }

    /* Check if the function read correct values in correct range */
    for (Index = 0; Index < 4; Index++)
    {
        for (WordOff = 0; WordOff < DFTEST_DMA_RES_WORDS; WordOff++)
        {
            pDmaBuf[WordOff] = WordOff + (Index << 24);
        }

        for (WordOff = 0; WordOff < DFTEST_DMA_RES_WORDS; WordOff += 32)
        {
            c_memset(LocalArray, 0xEE, sizeof(LocalArray));

            for (WordCount = 1; WordCount <= DFTEST_DMA_RES_WORDS - WordOff; WordCount++)
            {
                DMAResource_Read32Array(
                    Handle,
                    WordOff,
                    WordCount,
                    &LocalArray[WordOff]);
                /* Check the data */
                for (WordOff2 = 0; WordOff2 < DFTEST_DMA_RES_WORDS; WordOff2++)
                {
                    if (WordOff2 < WordOff || WordOff2 >= WordOff + WordCount)
                    {
                        fail_if(LocalArray[WordOff2] != 0xEEEEEEEEUL,
                                "Read too more data than expected");
                    }
                    else
                    {
                        fail_if(LocalArray[WordOff2] != WordOff2 + (Index << 24),
                                "Data was not read correctly");
                    }
                }
            }
        }
    }

#ifndef DFTEST_REMOVE_DMARES_READ_CHECK_ENDIAN
    /* use endianess swapping function */
    Result = DMAResource_SwapEndianess_Set(Handle, true);
    fail_unless(Result == 0, "Failed to set endianess swapping");

    DMAResource_Read32Array(
        Handle,
        0,
        DFTEST_DMA_RES_WORDS,
        LocalArray);

    for (Index = 0; Index < DFTEST_DMA_RES_WORDS; Index++)
    {
        fail_unless(
            LocalArray[Index] == Device_SwapEndian32(pDmaBuf[Index]),
            "Endianass swap failed at index %u\n",
            Index);
    }

    Result = DMAResource_SwapEndianess_Set(Handle, false);
    fail_unless(Result == 0, "Failed to unset endianess swapping");
#endif /* !DFTEST_REMOVE_DMARES_READ_CHECK_ENDIAN */

#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    dr_remove_addrpair(Handle, DMARES_DOMAIN_HOST);
    DMAResource_Read32Array(Handle, 0, 1, &LocalArray[0]);
#endif

    DMAResource_Release(Handle);
    DMAResource_UnInit();
}
END_TEST
#endif /* DFTEST_REMOVE_DMARES_READ32ARRAY */

#ifndef DFTEST_REMOVE_DMARES_WRITE32ARRAY
/* Test DMAResource_Write32Array */
START_TEST(test_dr_write32array)
{
    DMAResource_Handle_t Handle = NULL;
    uint32_t LocalArray[DFTEST_DMA_RES_WORDS];
    uint32_t WordOff;
    uint32_t WordOff2;
    uint32_t WordCount;
    uint32_t Index;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
    int Result;
    uint32_t * pDmaBuf;

    if(!DMAResource_Init())
    {
        fail("Failed to initialize DMA resource");
    }

    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &Handle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");
    fail_if(Handle == NULL, "Invalid handle from DMAResource_Alloc");
    pDmaBuf = (uint32_t *)AddrPair.Address.Native_p;
    fail_if(pDmaBuf == NULL, "Invalid pointer from output DMAResource_AddrPair_t");

    /* Check if the function handles invalid parameters well */
    DMAResource_Write32Array(NULL, 0, 1, LocalArray);
    // DMAResource_Write32Array(Handle, 0, 1, NULL);
    DMAResource_Write32Array(Handle, 0, 0, LocalArray);
    DMAResource_Write32Array(Handle, 0, 0x7FFFFFFFUL, LocalArray);
    DMAResource_Write32Array(Handle, 0xFFFFFFFF, 1, LocalArray);

    /* Check if the function can write correct values to whole buffer */
    for (Index = 0; Index < 4; Index++)
    {
        for (WordOff = 0; WordOff < DFTEST_DMA_RES_WORDS; WordOff++)
        {
            LocalArray[WordOff] = WordOff + (Index << 24);
        }

        DMAResource_Write32Array(
            Handle,
            0,
            DFTEST_DMA_RES_WORDS,
            LocalArray);

        for (WordOff = 0; WordOff < DFTEST_DMA_RES_WORDS; WordOff++)
        {
            fail_unless(pDmaBuf[WordOff] == WordOff +  (Index << 24),
                        "Data was not written correctly");
        }
    }

    /* Check if the function write correct values in correct range */
    for (Index = 0; Index < 4; Index++)
    {
        for (WordOff = 0; WordOff < DFTEST_DMA_RES_WORDS; WordOff++)
        {
            LocalArray[WordOff] = WordOff + (Index << 24);
        }

        for (WordOff = 0; WordOff < DFTEST_DMA_RES_WORDS; WordOff += 32)
        {
            c_memset(pDmaBuf, 0xEE, DFTEST_DMA_RES_BYTES);

            for (WordCount = 1; WordCount <= DFTEST_DMA_RES_WORDS - WordOff; WordCount++)
            {
                DMAResource_Write32Array(
                    Handle,
                    WordOff,
                    WordCount,
                    &LocalArray[WordOff]);
                /* Check the data */
                for (WordOff2 = 0; WordOff2 < DFTEST_DMA_RES_WORDS; WordOff2++)
                {
                    if (WordOff2 >= WordOff && WordOff2 < WordOff + WordCount)
                    {
                        fail_if(pDmaBuf[WordOff2] != WordOff2 + (Index << 24),
                                "Data was not written correctly");
                    }
                    else
                    {
                        fail_if(pDmaBuf[WordOff2] != 0xEEEEEEEEU,
                                "Write too more data than expected");
                    }
                }
            }
        }
    }

#ifndef DFTEST_REMOVE_DMARES_WRITE_CHECK_ENDIAN
    /* Check if the function works correctly when endianess need to be
       swapped */
    Result = DMAResource_SwapEndianess_Set(Handle, true);
    fail_unless(Result == 0, "Failed to set endianess swapping");

    DMAResource_Write32Array(
        Handle,
        0,
        DFTEST_DMA_RES_WORDS,
        LocalArray);

    for (Index = 0; Index < DFTEST_DMA_RES_WORDS; Index++)
    {
        fail_unless(
            LocalArray[Index] == Device_SwapEndian32(pDmaBuf[Index]),
            "Endianass swap failed at index %u\n",
            Index);
    }

    Result = DMAResource_SwapEndianess_Set(Handle, false);
    fail_unless(Result == 0, "Failed to unset endianess swapping");
#endif /* !DFTEST_REMOVE_DMARES_WRITE_CHECK_ENDIAN */

#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    dr_remove_addrpair(Handle, DMARES_DOMAIN_HOST);
    DMAResource_Write32Array(Handle, 0, 1, &LocalArray[0]);
#endif

    DMAResource_Release(Handle);
    DMAResource_UnInit();
}
END_TEST
#endif /* DFTEST_REMOVE_DMARES_WRITE32ARRAY */

#ifndef DFTEST_REMOVE_PRE_POST_DMA
/* Test DMAResource_PreDMA */
START_TEST(test_dr_pre_dma)
{
    DMAResource_Handle_t Handle = NULL;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
    int Result;

    if(!DMAResource_Init())
    {
        fail("Failed to initialize DMA resource");
    }

    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &Handle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");
    fail_if(Handle == NULL, "Invalid handle from DMAResource_Alloc");

    /* check if the function handles invalid parameters well */
    DMAResource_PreDMA(NULL, 0, 0);
    DMAResource_PreDMA(Handle, 0, 0);
    DMAResource_PreDMA(Handle, 0, 2 * DFTEST_DMA_RES_BYTES);

    /* check if the function works as design */
    DMAResource_PreDMA(Handle, 0, 1);
    DMAResource_PreDMA(Handle, DFTEST_DMA_RES_BYTES - 1, 1);
    DMAResource_PreDMA(Handle, 0, DFTEST_DMA_RES_BYTES);
    DMAResource_PreDMA(Handle, 0, DFTEST_DMA_RES_BYTES / 4);
    DMAResource_PreDMA(Handle, DFTEST_DMA_RES_BYTES / 4, DFTEST_DMA_RES_BYTES / 2);

    DMAResource_Release(Handle);
    DMAResource_UnInit();
}
END_TEST
#endif /* DFTEST_REMOVE_PRE_POST_DMA */

#ifndef DFTEST_REMOVE_PRE_POST_DMA
/* Test DMAResource_PostDMA */
START_TEST(test_dr_post_dma)
{
    DMAResource_Handle_t Handle = NULL;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
    int Result;

    if(!DMAResource_Init())
    {
        fail("Failed to initialize DMA resource");
    }

    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &Handle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");
    fail_if(Handle == NULL, "Invalid handle from DMAResource_Alloc");

    /* check if the function handles invalid parameters well */
    DMAResource_PostDMA(NULL, 0, 0);
    DMAResource_PostDMA(Handle, 0, 0);
    DMAResource_PostDMA(Handle, 0, 2 * DFTEST_DMA_RES_BYTES);

    /* check if the function works as design */
    DMAResource_PostDMA(Handle, 0, 1);
    DMAResource_PostDMA(Handle, DFTEST_DMA_RES_BYTES - 1, 1);
    DMAResource_PostDMA(Handle, 0, DFTEST_DMA_RES_BYTES);
    DMAResource_PostDMA(Handle, 0, DFTEST_DMA_RES_BYTES / 4);
    DMAResource_PostDMA(Handle, DFTEST_DMA_RES_BYTES / 4, DFTEST_DMA_RES_BYTES / 2);

    DMAResource_Release(Handle);
    DMAResource_UnInit();
}
END_TEST
#endif /* DFTEST_REMOVE_PRE_POST_DMA */

#ifndef DFTEST_REMOVE_DMARES_SWAP_ENDIANESS_SET_GET
/* Test DMAResource_SwapEndianess_Get */
START_TEST(test_dr_swap_endianess_set_get)
{
    DMAResource_Handle_t Handle = NULL;
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Properties_t DMAResProps = {0};
#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    DMAResource_Record_t * RecordPtr;
#endif
    int Result;

    if(!DMAResource_Init())
    {
        fail("Failed to initialize DMA resource");
    }

    /* Some negative tests */

    Result = DMAResource_SwapEndianess_Set(NULL, true);
    fail_if(Result == 0, "Should not work with NULL handle");

    Result = DMAResource_SwapEndianess_Set(NULL, false);
    fail_if(Result == 0, "Should not work with NULL handle");

    Result = DMAResource_SwapEndianess_Get(NULL);
    fail_if(Result == 0, "Should not work with NULL handle");

    /* Allocate a buffer and get the handle */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &Handle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");
    fail_if(Handle == NULL, "Invalid handle from DMAResource_Alloc");

#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    RecordPtr = DMAResource_Handle2RecordPtr(Handle);
    fail_if(RecordPtr == NULL, "Record pointer was invalid");
#endif

    Result = DMAResource_SwapEndianess_Set(Handle, true);
    fail_unless(Result == 0, "Failed to set endianess swap");
#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    fail_unless(RecordPtr->fSwapEndianess == true,
                "Endianess swap was not enabled successfully");
#endif
    Result = DMAResource_SwapEndianess_Get(Handle);
    fail_unless(Result == 1, "Didn't get correct endianess swap");
    Result = DMAResource_SwapEndianess_Set(Handle, false);
    fail_unless(Result == 0, "Failed to set endianess swap");
#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    fail_unless(RecordPtr->fSwapEndianess == false,
                "Endianess swap was not disabled successfully");
#endif
    Result = DMAResource_SwapEndianess_Get(Handle);
    fail_unless(Result == 0, "Didn't get correct endianess swap");

    DMAResource_Release(Handle);
    DMAResource_UnInit();
}
END_TEST
#endif /* DFTEST_REMOVE_DMARES_SWAP_ENDIANESS_SET_GET */

#ifndef DFTEST_REMOVE_DMARES_TRANSLATE
/* Test DMAResource_Translate */
START_TEST(test_dr_translate)
{
    DMAResource_Handle_t Handle = NULL;
    DMAResource_AddrPair_t AddrPair = {0};
    int Result;
    size_t Index;
    size_t DomainNum;
#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    DMAResource_Record_t * RecordPtr;
    bool IsAddrPairInRecord;
    bool ShouldAddrPairStored;
    size_t AddrPairIndex;
    DMAResource_AddrDomain_t ShouldStoredDomain[] =
    { DFTEST_DMARES_STORED_ADDRDOMAINS };
#endif
    DMAResource_Properties_t DMAResProps = {0};
    const DMAResource_AddrDomain_t ValidTransDomain[] =
    { DFTEST_DMARES_VALID_TRANSLATE_ADDRDOMAINS };
    const DMAResource_AddrDomain_t InvalidTransDomain[] =
    { DFTEST_DMARES_INVALID_TRANSLATE_ADDRDOMAINS };


    if(!DMAResource_Init())
    {
        fail("Failed to initialize DMA resource");
    }

    /* Allocate a buffer and get the handle */
    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &Handle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");
    fail_if(Handle == NULL, "Invalid handle from DMAResource_Alloc");

    /* Check if it handles invalid parameters correctly */
    Result = DMAResource_Translate(NULL, DMARES_DOMAIN_HOST, &AddrPair);
    fail_if(Result == 0,
            "Should not get translated address with NULL handle");

    Result = DMAResource_Translate(
                 DFTEST_DMARES_BAD_HANDLE_0,
                 DMARES_DOMAIN_HOST,
                 &AddrPair);
    fail_if(Result == 0,
            "Should not get translated address with garbage handle");

    Result = DMAResource_Translate(Handle, DMARES_DOMAIN_HOST, NULL);
    fail_if(Result == 0,
            "Should not return OK when invalid output address was provided");

    /* Check if the function translates address domain it claims to support */
    DomainNum = ELEMENTS_COUNT(ValidTransDomain);
    for (Index = 0; Index < DomainNum; Index++)
    {
        c_memset((void *)&AddrPair, 0, sizeof(AddrPair));
        Result = DMAResource_Translate(Handle, ValidTransDomain[Index], &AddrPair);
        fail_unless(Result == 0, "Failed to translate address in the domain");
        fail_unless(ValidTransDomain[Index] == AddrPair.Domain,
                    "Address was translated to wrong domain");
#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
        RecordPtr = DMAResource_Handle2RecordPtr(Handle);
        fail_if(RecordPtr == NULL, "Invalid Record pointer");
        fail_if((RecordPtr->Props.Alignment - 1) & (size_t)AddrPair.Address.Native_p,
                "Bad address, not aligned");

        /* Should we check if the AddrPair is stored? */
        ShouldAddrPairStored = false;
        for (AddrPairIndex = 0;
                AddrPairIndex < ELEMENTS_COUNT(ShouldStoredDomain);
                AddrPairIndex++)
        {
            if (AddrPair.Domain == ShouldStoredDomain[AddrPairIndex])
            {
                ShouldAddrPairStored = true;
                break;
            }
        }

        if (ShouldAddrPairStored)
        {
            /* Check if addrpair appears in dmares_record */
            IsAddrPairInRecord = false;
            for (AddrPairIndex = 0;
                    AddrPairIndex < DMARES_ADDRPAIRS_CAPACITY;
                    AddrPairIndex++)
            {
                if (RecordPtr->AddrPairs[AddrPairIndex].Address_p == AddrPair.Address.Native_p &&
                    RecordPtr->AddrPairs[AddrPairIndex].Domain == AddrPair.Domain)
                {
                    IsAddrPairInRecord = true;
                    break;
                }
            }
            fail_unless(IsAddrPairInRecord, "AddrPairs were not in Record");
        }
#endif
    }

    /* check if the function handles the unsupported domains */
    DomainNum = ELEMENTS_COUNT(InvalidTransDomain);
    for (Index = 0; Index < DomainNum; Index++)
    {
        c_memset((void *)&AddrPair, 0, sizeof(AddrPair));
        Result = DMAResource_Translate(Handle, InvalidTransDomain[Index], &AddrPair);
        fail_if(Result == 0, "The domain %d should not be supported",
                InvalidTransDomain[Index]);
    }

#ifndef DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
    /* check the behavior of the function when the stored AddrPairs are all
       deleted */
    for (Index = 0; Index < DomainNum; Index++)
    {
        dr_remove_addrpair(Handle, ValidTransDomain[Index]);
    }

    for (Index = 0; Index < DomainNum; Index++)
    {
        c_memset((void *)&AddrPair, 0, sizeof(AddrPair));
        Result = DMAResource_Translate(Handle, ValidTransDomain[Index], &AddrPair);
        fail_if(Result == 0, "Should not translate the AddrPair");
    }
#endif


    DMAResource_Release(Handle);
    DMAResource_UnInit();
}
END_TEST
#endif /* DFTEST_REMOVE_DMARES_TRANSLATE */

#ifndef DFTEST_REMOVE_DMARES_ADDPAIR
/* Test DMAResource_AddPair */
START_TEST(test_dr_add_pair)
{
    DMAResource_Handle_t Handle = NULL;
    DMAResource_AddrPair_t AddrPair = {0};
    DMAResource_Properties_t DMAResProps = {0};
    int Result;

    if(!DMAResource_Init())
    {
        fail("Failed to initialize DMA resource");
    }

    /* Check if it handles invalid parameters correctly */
    Result = DMAResource_AddPair(NULL, AddrPair);
    fail_if(Result == 0, "NULL pointer handle should not work");

    Result = DMAResource_AddPair(DFTEST_DMARES_BAD_HANDLE_0, AddrPair);
    fail_if(Result == 0, "Garbage handle should not work");

    Result = DMAResource_AddPair(DFTEST_DMARES_BAD_HANDLE_1, AddrPair);
    fail_if(Result == 0, "Garbage handle should not work");

    DMAResProps.Size = DFTEST_DMA_RES_BYTES;
    DMAResProps.Alignment = 4;
    Result = DMAResource_Alloc(
                 DMAResProps,
                 &AddrPair,
                 &Handle);
    fail_unless(Result == 0, "Failed to allocate DMA resource");
    fail_if(Handle == NULL, "Invalid handle from DMAResource_Alloc");

    /* Check if it works with existed domain */
    Result = DMAResource_AddPair(Handle, AddrPair);
    fail_unless(Result == 0, "Failed to add existed address pair");

#ifndef DFTEST_REMOVE_DMARES_UNSUPPORTED_PARA_TEST
    AddrPair.Domain = DMARES_DOMAIN_HOST;
    AddrPair.Address.Native_p = (void *)NULL;
    Result = DMAResource_AddPair(Handle, AddrPair);
    fail_if(Result == 0, "It should not work without invalid address");
    AddrPair.Domain = DMARES_DOMAIN_HOST;
    AddrPair.Address.Native_p = (void *)3;
    Result = DMAResource_AddPair(Handle, AddrPair);
    fail_if(Result == 0, "It should not work without invalid address");
#endif

    /* Check if it works as definition */
    AddrPair.Domain = DMARES_DOMAIN_INTERHOST;
    AddrPair.Address.Native_p = (void *)DFTEST_DMARES_VALID_ADDRESS;
    Result = DMAResource_AddPair(Handle, AddrPair);
    fail_unless(Result == 0, "Failed to add address pair");

    /* Exhaust all slots for AddrPairs */
    Result = 0;
    AddrPair.Domain = DMARES_DOMAIN_HOST;
    while (Result == 0)
    {
        Result = DMAResource_AddPair(Handle, AddrPair);
        AddrPair.Domain++;
    }

    DMAResource_Release(Handle);
    DMAResource_UnInit();
}
END_TEST
#endif /* DFTEST_REMOVE_DMARES_ADDPAIR */

void suite_add_test_dma_resource_apis(void)
{
    sfzutf_tcase_create("DMA_Resource_Tests");
    sfzutf_test_add(test_dr_init_uninit);
#ifndef DFTEST_REMOVE_DMARES_RECORD_APIS
    sfzutf_test_add(test_dr_create_destroy_valid_record);
    sfzutf_test_add(test_dr_handle2record);
#endif
    sfzutf_test_add(test_dr_alloc);
    sfzutf_test_add(test_dr_release);
#ifndef DFTEST_REMOVE_DMARES_CHECKANDREGISTER
    sfzutf_test_add(test_dr_check_and_register);
#endif
#ifndef DFTEST_REMOVE_DMARES_ATTACH
    sfzutf_test_add(test_dr_attach);
#endif
#ifndef DFTEST_REMOVE_DMARES_SWAP_ENDIANESS_SET_GET
    sfzutf_test_add(test_dr_swap_endianess_set_get);
#endif
    sfzutf_test_add(test_dr_read32);
    sfzutf_test_add(test_dr_write32);
#ifndef DFTEST_REMOVE_DMARES_READ32ARRAY
    sfzutf_test_add(test_dr_read32array);
#endif
#ifndef DFTEST_REMOVE_DMARES_WRITE32ARRAY
    sfzutf_test_add(test_dr_write32array);
#endif
#ifndef DFTEST_REMOVE_PRE_POST_DMA
    sfzutf_test_add(test_dr_pre_dma);
    sfzutf_test_add(test_dr_post_dma);
#endif
#ifndef DFTEST_REMOVE_DMARES_TRANSLATE
    sfzutf_test_add(test_dr_translate);
#endif
#ifndef DFTEST_REMOVE_DMARES_ADDPAIR
    sfzutf_test_add(test_dr_add_pair);
#endif
}

#endif  /* DFTEST_REMOVE_DMA_RESOURCE_APIS */

/* end of file dftest_dma_resource.c */

