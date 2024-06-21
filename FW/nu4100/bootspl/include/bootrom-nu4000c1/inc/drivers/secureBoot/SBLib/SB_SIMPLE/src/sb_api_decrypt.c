/* sb_api_decrypt.c
 *
 * Description: Secure boot APIs
 */

/*****************************************************************************
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
*****************************************************************************/

#include "sb_ecdsa.h"
#include "sb_sw_hash.h"
#include "sb_sw_ecdsa.h"
#include "sb_sw_endian.h"
#include "sbif_ecdsa.h"
#include "sbif_attributes.h"
#include "cfg_sblib.h"
#include "c_lib.h"
#include "sbif_tools_convert.h"

static bool
sgvector_cpy(const SBIF_SGVector_t * const DataVectorsOutput_p,
             const SBIF_SGVector_t * const DataVectorsInput_p,
             const uint32_t VectorCount)
{
    uint32_t i;
    for (i = 0; i < VectorCount; i++)
    {
        void * out_p = DataVectorsOutput_p[i].Data_p;
        const void * in_p = DataVectorsInput_p[i].Data_p;
        uint32_t size = DataVectorsOutput_p[i].DataLen;

        /* Check sizes match */
        if (DataVectorsInput_p[i].DataLen != size)
        {
            return false;
        }

        /* Copy the data */
        if (out_p != in_p)
        {
            c_memcpy(out_p, in_p, size);
        }
    }
    return true;
}

static void
sgvector_clear(const SBIF_SGVector_t * const DataVectorsOutput_p,
               const uint32_t VectorCount)
{
    uint32_t i;

    for (i = 0; i < VectorCount; i++)
    {
        c_memset(DataVectorsOutput_p[i].Data_p, 0,
                 DataVectorsOutput_p[i].DataLen);
    }
}

SB_Result_t
SB_ECDSA_Image_DecryptAndVerify(SB_StorageArea_t * const             Storage_p,
                                void *                               PollParam_p,
                                const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
                                const uint8_t * const                Hash_p,
                                const SBIF_ECDSA_Header_t *          Header_p,
                                const SBIF_SGVector_t *              DataVectorsInput_p,
                                const SBIF_SGVector_t *              DataVectorsOutput_p,
                                uint32_t VectorCount)
{
    /* This function processes BLe/BLw images into BLp using SBIF Tools.
       After conversion, standard SB_ECDSA_Image_Verify is used.
       In case of BLp image, optional copying takes place, but nothing else. */

    /* Struct for temporary BLp header to pass to SB_ECDSA_Image_Verify. */
    struct
    {
        SBIF_ECDSA_Header_t Header_p;
#if SBLIB_CFG_CERTIFICATES_MAX > 0
        SBIF_ECDSA_Certificate_t Certificates[SBLIB_CFG_CERTIFICATES_MAX];
#endif /* SBLIB_CFG_CERTIFICATES_MAX > 0 */
    } sbifHeaderStorage;

    const SBIF_ECDSA_Header_t * header_p = Header_p;
    SB_Result_t sbres = SB_ERROR_ARGUMENTS;
    SBIFTC_Result_t sbiftcres;
    uint32_t image_type;

    image_type = SB_SW_BE32_TO_CPU(Header_p->Type);
    image_type = SBIF_TYPE_TYPE(image_type);

    /* Check if this is encrypted image.
       In case of encrypted image, we process it via
       SBIFTOOLS_Image_Convert. */
    if (image_type != SBIF_IMAGE_BLTp)
    {
        uint32_t storage_size = SBLIB_CFG_STORAGE_SIZE;
        size_t newHeaderSize = sizeof(sbifHeaderStorage);

        sbiftcres = SBIFTOOLS_Image_Convert_InitWorkspace(Storage_p, &storage_size);
        if (sbiftcres == SBIFTC_SUCCESS)
        {
            sbiftcres = SBIFTOOLS_Image_Convert(Header_p,
                                                SBIF_IMAGE_BLTp,
                                                SBIF_VERSION,
                                                &(sbifHeaderStorage.Header_p),
                                                &newHeaderSize,
                                                DataVectorsInput_p,
                                                DataVectorsOutput_p,
                                                VectorCount,
                                                NULL,
                                                0,
                                                Storage_p);

            SBIFTOOLS_Image_Convert_UninitWorkspace(Storage_p);
        }

        if (sbiftcres != SBIFTC_SUCCESS)
        {
            /* Error from SBIFTOOLS, convert error into SB_Result_t. */
            switch (sbiftcres)
            {
            case SBIFTC_ERROR_ARGUMENTS:
                return SB_ERROR_ARGUMENTS;
            case SBIFTC_ERROR_CRYPTO:
                return SB_ERROR_HARDWARE;
            case SBIFTC_ERROR_IMAGE_VERSION:
                return SB_ERROR_IMAGE_VERSION;
            case SBIFTC_ERROR_IMAGE_TYPE:
                return SB_ERROR_IMAGE_TYPE;
            default:
                return SB_ERROR_HARDWARE;
            }
        }

        header_p = &(sbifHeaderStorage.Header_p);
    }
    else
    {
        if (sgvector_cpy(DataVectorsOutput_p, DataVectorsInput_p, VectorCount) == false)
        {
            /* Input and output vectors do not match. */
            return SB_ERROR_ARGUMENTS;
        }
    }

    sbres = SB_ECDSA_Image_Verify(Storage_p,
                                  PollParam_p,
                                  PublicKey_p,
                                  Hash_p,
                                  header_p,
                                  DataVectorsOutput_p,
                                  VectorCount);

    /* Clear output if verification failed. */
    if (sbres != SB_SUCCESS)
    {
        sgvector_clear(DataVectorsOutput_p, VectorCount);
    }

    return sbres;
}

SB_Result_t
SB_ECDSA_Image_DecryptWithKeyAndVerify(SB_StorageArea_t * const             Storage_p,
                                       void *                               PollParam_p,
                                       const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
                                       const uint8_t * const                Hash_p,
                                       const SBIF_ECDSA_Header_t *          Header_p,
                                       const SBIF_SGVector_t *              DataVectorsInput_p,
                                       const SBIF_SGVector_t *              DataVectorsOutput_p,
                                       uint32_t                             VectorCount,
                                       SB_ConfidentialityKeyType_t          ConfidentialityKeyType,
                                       const uint8_t *                      ConfidentialityKey)
{
    PARAMETER_NOT_USED(Storage_p);
    PARAMETER_NOT_USED(PollParam_p);
    PARAMETER_NOT_USED(PublicKey_p);
    PARAMETER_NOT_USED(Hash_p);
    PARAMETER_NOT_USED(Header_p);
    PARAMETER_NOT_USED(DataVectorsInput_p);
    PARAMETER_NOT_USED(DataVectorsOutput_p);
    PARAMETER_NOT_USED(VectorCount);
    PARAMETER_NOT_USED(ConfidentialityKeyType);
    PARAMETER_NOT_USED(ConfidentialityKey);
    return SB_ERROR_IMAGE_TYPE;
}

/* end of file sb_api_decrypt.c */
