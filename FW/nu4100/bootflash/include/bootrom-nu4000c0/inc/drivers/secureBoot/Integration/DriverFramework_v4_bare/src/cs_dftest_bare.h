/* cs_dftest_bare.h
 *
 * Configuration Settings of test suite for bare driver framwwork.
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

//#define DFTEST_REMOVE_BASIC_DEFINITIONS
//#define DFTEST_REMOVE_CLIB
//#define DFTEST_REMOVE_DEVICE_APIS
//#define DFTEST_REMOVE_DMA_RESOURCE_APIS

/*----------------------------------------------------------------------------
* Device related test definitions
*/

/* Remove the test on Device_UnInitialize */
#define DFTEST_REMOVE_DEVICE_UNINITIALIZE

/* Remove the test on Device_GetReference */
#define DFTEST_REMOVE_DEVICE_GET_REFERENCE_TEST

/* Remove negative test by known unsupported device names */
//#define DFTEST_REMOVE_DEVICE_KNOWN_UNSUPPORTED_DEV_NAMES

/* Remove negative test by invalid large offset of addresses */
//#define DFTEST_REMOVE_DEVICE_INVALID_LARGE_OFFSET

/* Remove negative test by providing invalid output buffer */
#define DFTEST_REMOVE_DEVICE_INVALID_OUTPUT_BUF_TEST

/* Remove tests on unaligned offset */
#define DFTEST_REMOVE_DEVICE_UNALIGNED_OFF_TEST

/* Remove code of verifying written contents */
//#define DFTEST_REMOVE_DEVICE_MEM_WR_VERIFY

/* We assume "EIP130" is a valid device name in customer's implementation,
   the device is not used in read and write test */
#define DFTEST_VALID_DEV_NAME               ("EIP130")

/* The below two macros define the read and write capabilities of a device */
#define DFTEST_DEVICE_DEV_READ_ONLY_CAP     (BIT_0)
#define DFTEST_DEVICE_DEV_WRITE_ONLY_CAP    (BIT_1)
#define DFTEST_DEVICE_DEV_WR_CAP            (BIT_2)

/* The below macros define a device for read and write tests, they should be
   configured according to the actual hardware */
#define DFTEST_DEVICE_WRDEV_CAPS            (DFTEST_DEVICE_DEV_WR_CAP)

/* Edit below macros according to device you select */
#define DFTEST_DEVICE_DEV_RD_NAME            ("EIP130")
#define DFTEST_DEVICE_DEV_RD_START_BYTES     (0x0000) // OUT MAILBOX
#define DFTEST_DEVICE_DEV_RD_SIZE_BYTES      256

/* Edit below macros according to device you select */
#define DFTEST_DEVICE_DEV_WR_NAME           ("EIP130")
#define DFTEST_DEVICE_DEV_WR_START_BYTES    (0x0000)  // IN MAILBOX
#define DFTEST_DEVICE_DEV_WR_SIZE_BYTES     256


/*----------------------------------------------------------------------------
* DMA resource related test definitions
*/
/* Remove DMAResource_Attach function behavior test */
#define DFTEST_REMOVE_DMARES_ATTACH

/* Remove DMAResource records functions behavior test */
#define DFTEST_REMOVE_DMARES_RECORD_APIS

/* Remove DMAResource_IsValidHandle function behavior test */
#define DFTEST_REMOVE_DMARES_IS_VALID_HANDLE

/* Remove DMAResource_Read32Array function behavior test */
#define DFTEST_REMOVE_DMARES_READ32ARRAY

/* Remove DMAResource_Write32Array function behavior test */
#define DFTEST_REMOVE_DMARES_WRITE32ARRAY

/* Remove DMAResource_SwapEndianess_Set and DMAResource_SwapEndianess_Get
   function behavior test */
#define DFTEST_REMOVE_DMARES_SWAP_ENDIANESS_SET_GET

/* Remove DMAResource_AddPair function behavior test */
#define DFTEST_REMOVE_DMARES_ADDPAIR

/* Remove invalid properties test
   because DMAResource_Alloc does not check it */
//#define DFTEST_REMOVE_DMARES_INVALID_PROPS

/* Remove DMAResource_CheckAndRegister function behavior test
   because function is stubbed */
#define DFTEST_REMOVE_DMARES_CHECKANDREGISTER

/* Remove DMAResource_Translate function behavior test
   because function is stubbed */
#define DFTEST_REMOVE_DMARES_TRANSLATE

/* Remove Pre/post DMA function behavior test
   because function is stubbed */
#define DFTEST_REMOVE_PRE_POST_DMA

/* Remove endianess checking when test DMAResource_Read32 function */
#define DFTEST_REMOVE_DMARES_READ_CHECK_ENDIAN

/* Remove endianess checking when test DMAResource_Write32 function */
#define DFTEST_REMOVE_DMARES_WRITE_CHECK_ENDIAN

/* Some functions have output buffer as parameter, enable the below macro to
   test if they can work with invalid output buffer. */
#define DFTEST_REMOVE_DMARES_INVALID_OUTPUT_BUF_TEST

/* The below macro removes tests for unsupported parameters */
#define DFTEST_REMOVE_DMARES_UNSUPPORTED_PARA_TEST

/* For some reasons some test need check the record structure implemented by
   customers. The macao is commented out by default. */
#define DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK
//#define DFTEST_REMOVE_DMARES_RECORD_STRUCT_CHECK

/* Remove registering a buffer for a different bank test */
#define DFTEST_REMOVE_DMARES_REGISTER_DIFFERENT_BANK

/* Remove releasing resource from _Attach test */
#define DFTEST_REMOVE_DMARES_RELEASE_RES_FROM_ATTACH


#define DFTEST_DMARES_GOOD_ALLOC_REF    ('R')
#define DFTEST_DMARES_BAD_ALLOC_REF     ('X')

#define DFTEST_DMARES_GOOD_ALLOC_REF    ('R')
#define DFTEST_DMARES_BAD_ALLOC_REF     ('X')

#define DFTEST_DMARES_INVALID_DOMAIN    0xBADBAD
#define DFTEST_DMARES_INVALID_BANK      0x10

/* The maximum record counter the implementation supports. You should
   change the macro according to the reality */
#define DFTEST_DMARES_RECORDS_LIMIT     128
/* The below macro defines the maximum number of records we'll check, to
   avoid we spend to much time on that if the number of records is large */
#define DFTEST_DMARES_MAX_RECORDS_TO_CHECK     DFTEST_DMARES_RECORDS_LIMIT

#define IsValidAddrPair(pair) \
            ((pair.Domain == DMARES_DOMAIN_HOST) || \
             (pair.Domain == DMARES_DOMAIN_UNIFIED) || \
             (pair.Domain == DMARES_DOMAIN_EIP12xDMA) || \
             (pair.Domain == DMARES_DOMAIN_EIP13xDMA) || \
             (pair.Domain == DMARES_DOMAIN_EIP93DMA))

#define DFTEST_DMA_RES_BYTES            128
#define DFTEST_DMA_RES_WORDS            (DFTEST_DMA_RES_BYTES / sizeof(uint32_t))

/* the address domains that DMARes_Alloc is expected to fill in (1 or more)
   (in DMAResource_Record_t.AddrPairs) */
#define DFTEST_DMARES_ALLOC_ADDRDOMAINS  DMARES_DOMAIN_UNIFIED

/* the address domains that DMARes_Alloc is expected to set unused entries with
   (in DMAResource_Record_t.AddrPairs) */
#define DFTEST_DMARES_ALLOC_ADDRDOMAIN_UNUSED \
             DMARES_DOMAIN_HOST, DMARES_DOMAIN_EIP12xDMA, DMARES_DOMAIN_EIP13xDMA, DMARES_DOMAIN_EIP93DMA

#define DFTEST_DMARES_VALID_TRANSLATE_ADDRDOMAINS \
             DMARES_DOMAIN_UNIFIED

#define DFTEST_DMARES_INVALID_TRANSLATE_ADDRDOMAINS \
             DMARES_DOMAIN_HOST, DMARES_DOMAIN_EIP12xDMA, DMARES_DOMAIN_EIP13xDMA, DMARES_DOMAIN_EIP93DMA


/* {Size, Alignment, Bank, fCache} */
#define DFTEST_VALID_PROPS {\
    {4, 8, 0, false}, \
    {32, 1, 0, false}, \
    {33, 4, 0, false}, \
    {64, 2, 0, false}, \
    {128, 4, 0, false} \
}

/* {Size, Alignment, Bank, fCache} */
/* Only if HWPAL_BARE_DEBUGCHECKS is defined */
#define DFTEST_INVALID_PROPS {\
    {132, 4, 0, false} \
}


/*----------------------------------------------------------------------------
* Basic definitions related test definitions
*/
//So far no any definition


/*----------------------------------------------------------------------------
* Clib related test definitions
*/
//So far no any definition


/* end of file cs_dftest_bare.h */
