/* sbhybrid_api_decrypt.c
 *
 * Description: Implementation of SB_API for use with
 *              Software, EIP-130, EIP-123, EIP-93, and/or EIP-28/EIP-150.
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

#ifndef __MODULE__
#define __MODULE__ "sbhybrid_api_decrypt.c"
#endif /* !__MODULE__ */

#include "c_lib.h"
#include "sb_ecdsa.h"
#include "sbif_ecdsa.h"
#include "sbif_attributes.h"
#include "cfg_sblib.h"
#include "sbif_tools_convert.h"
#include "sbhybrid_internal.h"

#ifndef SBHYBRID_WITH_SW
/* On non-SW compilation DMAResource's shall be used. */
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"
#endif /* !SBHYBRID_WITH_SW */


#if defined(SBHYBRID_WITH_EIP123) || defined(SBHYBRID_WITH_EIP130)
/* Synchronize hash and cipher processing so that they are processed
   in turns. When parallelizing with flash loading, this is likely faster.
   When not parallizing, it is possibly faster to perform one large
   hash operation.
   It is not recommend to turn off this option, because otherwise hash
   operations are combined into one large hash operation it may not fit
   within DMA descriptor. */
#define SBHYBRID_SYNC_HASH_CIPHER
#endif /* SBHYBRID_WITH_EIP123 || SBHYBRID_WITH_EIP130 */

// ensure there is enough space in the caller-allocated buffer
COMPILE_GLOBAL_ASSERT(sizeof(SBHYBRID_Context_t)<= sizeof(SB_StorageArea_t));

static void
sgvector_clear(const SBIF_SGVector_t * const DataVectorsOutput_p,
               const uint32_t VectorCount)
{
    uint32_t i;

    for (i = 0; i < VectorCount; i++)
    {
        c_memset(DataVectorsOutput_p[i].Data_p,
                 0,
                 DataVectorsOutput_p[i].DataLen);
    }
}


/*----------------------------------------------------------------------------
 * SBHYBRID_Do_Asym_RunFsm
 *
 * Single step of asymmetric processing:
 * Performs asymmetric crypto RunFsm. If the current certificate gets
 * handled, starts processing next certificate.
 * Assumes all digests have been precalculated.
 * Calls polling as appropriate.
 */
static SB_Result_t
SBHYBRID_Do_Asym_RunFsm(SBHYBRID_Context_t * const Context_p,
                        void * PollParam_p,
                        const SBIF_ECDSA_Header_t * Header_p)
{
    SB_Result_t res;

    IDENTIFIER_NOT_USED(Header_p);
    IDENTIFIER_NOT_USED(PollParam_p);

    res = SBHYBRID_Verify_RunFsm(Context_p->EcdsaContext_p);
    if (res == SB_SUCCESS)
    {
#if defined(SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT) || defined(SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK)
        // This is success from the ECDSA Verify proccess
        return res;
#else /* !SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT && !SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK */
        // A certificate has been processed, move to the next
        // certificate, with the previous certificates public key.
        const SBIF_ECDSA_Certificate_t * Certificates_p;
        const SBIF_ECDSA_Certificate_t * Certificate_p;
        const SBIF_ECDSA_Certificate_t * PrevCertificate_p;
        int CertNr;

        Certificates_p    = (const SBIF_ECDSA_Certificate_t *)(Header_p + 1);
        PrevCertificate_p = Certificates_p + Context_p->CertNr;
        CertNr            = ++Context_p->CertNr;
        Certificate_p     = Certificates_p + CertNr;

        if (Context_p->CertNr == (Context_p->CertificateCount + 1))
        {
            // This is success from the final EcdsaVerify.
            return res;
        }

        if (Context_p->CertNr == Context_p->CertificateCount)
        {
            // Start the final EcdsaVerify
            L_TRACE_PUBKEY(&PrevCertificate_p->PublicKey);
            L_TRACE_SIGNATURE(&Header_p->Signature);

            SBHYBRID_Verify_Init(Context_p->EcdsaContext_p,
                                 &PrevCertificate_p->PublicKey,
                                 &Header_p->Signature);

            // Final digest is not necessarily available yet,
            // signal the FSM is ready for it.
            Context_p->NeedFinalDigest = 1;
        }
        else
        {
            // Continue with the next certificate
            L_TRACE_PUBKEY(&PrevCertificate_p->PublicKey);
            L_TRACE_DIGEST(Context_p->SymmContext.CertDigest[CertNr]);
            L_TRACE_SIGNATURE(&Certificate_p->Signature);

            SBHYBRID_Verify_Init(Context_p->EcdsaContext_p,
                                 &PrevCertificate_p->PublicKey,
                                 &Certificate_p->Signature);

            SBHYBRID_Verify_SetDigest(Context_p->EcdsaContext_p,
                                      Context_p->SymmContext.CertDigest[CertNr]);
        }
        // Operation pending, with the next ecdsa verify started
        return SBHYBRID_PENDING;
#endif /* SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT || SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK */
    }

#ifndef SBLIB_CF_REMOVE_POLLING_CALLS
    if (res == SBHYBRID_PENDING)
    {
        if (SB_Poll(PollParam_p) == false)
        {
            L_DEBUG(LF_SBHYBRID, "ERROR: SB_Poll failed");
            res = SB_ERROR_POLL;
        }
    }
#endif /* !SBLIB_CF_REMOVE_POLLING_CALLS */

    return res;
}


/*----------------------------------------------------------------------------
 * SB_ECDSA_Image_CopyOrDecrypt_Verify
 */
static SB_Result_t
SB_ECDSA_Image_CopyOrDecrypt_Verify(SB_StorageArea_t * const Storage_p,
                                    void * PollParam_p,
                                    const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
                                    const uint8_t * const Hash_p,
                                    const SBIF_ECDSA_Header_t * Header_p,
                                    const SBIF_SGVector_t * DataVectorsIn_p,
                                    const SBIF_SGVector_t * DataVectorsOut_p,
                                    uint32_t VectorCount,
                                    bool DoDecrypt)
{
    SBHYBRID_Context_t * const Context_p = (SBHYBRID_Context_t *)Storage_p;
    const uint32_t ImageLen = SBHYBRID_Load_BE32(&Header_p->ImageLen);
    const uint32_t CertificateCount = SBHYBRID_Load_BE32(&Header_p->CertificateCount);
    SB_Result_t res = SB_SUCCESS;

    L_TRACE(LF_SBHYBRID,
            "SB_ECDSA_Image_Verify, Storage_p = %p, PollParam_p = %p, "
            "PublicKey_p = %p, Header_p = %p, "
            "DataVectorsIn_p = %p, DataVectorsOut_p = %p, "
            "VectorCount = %u.",
            (void *)Storage_p,
            PollParam_p,
            (void *)PublicKey_p,
            (void *)Header_p,
            (void *)DataVectorsIn_p,
            (void *)DataVectorsOut_p,
            (unsigned int)VectorCount);

    PRECONDITION(Storage_p != NULL);
    PRECONDITION(PublicKey_p != NULL);
    PRECONDITION(Header_p != NULL);
    PRECONDITION(DataVectorsIn_p != NULL);
    PRECONDITION(DataVectorsOut_p != NULL);
    PRECONDITION(VectorCount <= SBLIB_CFG_DATAFRAGMENTS_MAX);

#ifndef SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
    // SecureBoot currently is only able to support up-to 255 certificates.
    COMPILE_STATIC_ASSERT(SBLIB_CFG_CERTIFICATES_MAX <= 255);
#endif /* !SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT */

    // Ensure image to be verified does not exceed maximum size
    if (ImageLen > SBLIB_CFG_DATASIZE_MAX)
    {
        // Too large image supplied for verification
        L_DEBUG(LF_SBHYBRID,
                "ERROR: Image too large (%u > %u).",
                (unsigned int)ImageLen, SBLIB_CFG_DATASIZE_MAX);
        return SB_ERROR_VERIFICATION;
    }

    if (CertificateCount > SBLIB_CFG_CERTIFICATES_MAX)
    {
        // Invalid certificate count
        L_DEBUG(LF_SBHYBRID,
                "ERROR: CertificateCount too high (%u > %u)",
                (unsigned int)CertificateCount, SBLIB_CFG_CERTIFICATES_MAX);
        return SB_ERROR_CERTIFICATE_COUNT;
    }

    if (SBIF_Attribute_Check(&Header_p->ImageAttributes) == false)
    {
        // Invalid attributes
        L_DEBUG(LF_SBHYBRID, "ERROR: Invalid attributes.");
        return SB_ERROR_VERIFICATION;
    }

    // All sanity checks done
    res = SBHYBRID_Initialize_HW(Context_p);
    if (res == SB_SUCCESS)
    {
#ifndef SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
        if (CertificateCount != 0)
        {
            const SBIF_ECDSA_Certificate_t * Certificate_p;
            unsigned int CertNr;
#ifdef SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK
            const SBIF_ECDSA_PublicKey_t * ChkPublicKey_p = PublicKey_p;

            res = SBHYBRID_SHA2XX_Init(&Context_p->SymmContext);

            Context_p->SymmContext.HashDataLength  = sizeof(Certificate_p->PublicKey);
            Context_p->SymmContext.HashTotalLength = sizeof(Certificate_p->PublicKey);
#endif /* SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK */

            Certificate_p = (const SBIF_ECDSA_Certificate_t *)(Header_p + 1);

            for (CertNr = 0; CertNr < CertificateCount; CertNr++, Certificate_p++)
            {
                L_TRACE(LF_SBHYBRID, "Certificate %d.", CertNr);

                // Calculate the hash over the public key in the certificate
                // Note: The public key is the first field
#ifdef SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK
                if (res == SB_SUCCESS)
                {
                    L_TRACE_PUBKEY(ChkPublicKey_p);
                    L_TRACE_SIGNATURE(&Certificate_p->Signature);

                    SBHYBRID_Verify_Init(Context_p->EcdsaContext_p,
                                         ChkPublicKey_p,
                                         &Certificate_p->Signature);

                    Context_p->SymmContext.HashData_p = (const uint8_t *)Certificate_p;
                    res = SBHYBRID_Do_Asym_RunFsm(Context_p, PollParam_p, Header_p);
                    if (res != SB_SUCCESS)
                    {
                        L_DEBUG(LF_SBHYBRID,
                                "ERROR: Certificate %u verify failed",
                                CertNr);
                        break;
                    }
                    ChkPublicKey_p = (const SBIF_ECDSA_PublicKey_t *)Certificate_p;
                }
                else
                {
                    L_DEBUG(LF_SBHYBRID, "ERROR: Hash initialization failed");
                    break;
                }
#else /* !SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK */
                res = SBHYBRID_SHA2XX_Init(&Context_p->SymmContext);
                if (res == SB_SUCCESS)
                {
                    res = SBHYBRID_SHA2XX_AddBlock(&Context_p->SymmContext,
                                                   (const uint8_t *)Certificate_p,
                                                   sizeof(SBIF_ECDSA_PublicKey_t),
                                                   true);
                }
                if (res == SB_SUCCESS)
                {
                    // Wait for the FSM to complete all the interactions
                    do
                    {
                        // the below function returns
                        // SB_SUCCESS when all processing has finished
                        // SB_<other> in case of fatal error
                        // SBHYBRID_PENDING when calculations are in progress
                        res = SBHYBRID_SHA2XX_RunFsm(&Context_p->SymmContext);

#ifndef SBLIB_CF_REMOVE_POLLING_CALLS
                        if (res == SBHYBRID_PENDING)
                        {
                            if (SB_Poll(PollParam_p) == false)
                            {
                                L_DEBUG(LF_SBHYBRID, "ERROR: SB_Poll failed");
                                res = SB_ERROR_POLL;
                            }
                        }
#endif /* !SBLIB_CF_REMOVE_POLLING_CALLS */
                    } while (res == SBHYBRID_PENDING);
                }

                // done with this certificate
                if (res != SB_SUCCESS)
                {
                    L_DEBUG(LF_SBHYBRID,
                            "ERROR: Certificate %u hash failed (%d)",
                            CertNr, res);
                    res = SB_ERROR_VERIFICATION;
                    break;              // break from the for
                }

                // Successfully calculated the digest
                SBHYBRID_SHA2XX_GetDigest(&Context_p->SymmContext,
                                          Context_p->SymmContext.CertDigest[CertNr]);
                if (CertNr == 0)
                {
                    L_TRACE_PUBKEY(PublicKey_p);
                    L_TRACE_DIGEST(Context_p->SymmContext.CertDigest[CertNr]);
                    L_TRACE_SIGNATURE(&Certificate_p->Signature);

                    // Set first ECDSA Verify target
                    SBHYBRID_Verify_Init(Context_p->EcdsaContext_p,
                                         PublicKey_p,
                                         &Certificate_p->Signature);

                    SBHYBRID_Verify_SetDigest(Context_p->EcdsaContext_p,
                                              Context_p->SymmContext.CertDigest[CertNr]);
                }
#endif /* SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK */
            }
#ifdef SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK
            if (res == SB_SUCCESS)
            {
                // Initialize the ECDSA Verify with the public key of the last
                // certificate and the image signature
                L_TRACE(LF_SBHYBRID,
                        "Initializing ECDSA verify for image signature.");
                L_TRACE_PUBKEY(ChkPublicKey_p);
                L_TRACE_SIGNATURE(&Header_p->Signature);

                SBHYBRID_Verify_Init(Context_p->EcdsaContext_p,
                                     ChkPublicKey_p,
                                     &Header_p->Signature);

                // Final digest not yet available, signal it is needed
                Context_p->NeedFinalDigest = 1;
            }
#endif /* SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK */
        }
        else
#endif /* !SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT */
        {
            // Initialize the ECDSA Verify with public key and the image
            // signature when no certificates are present
            L_TRACE(LF_SBHYBRID,
                    "Initializing ECDSA verify for image signature.");
            L_TRACE_PUBKEY(PublicKey_p);
            L_TRACE_SIGNATURE(&Header_p->Signature);

            SBHYBRID_Verify_Init(Context_p->EcdsaContext_p,
                                 PublicKey_p,
                                 &Header_p->Signature);

            // Final digest not yet available, signal it is needed
            Context_p->NeedFinalDigest = 1;
        }
    }

    // Keep track of ECDSA Verifies for certificates under processing
    Context_p->CertNr = 0;
    Context_p->CertificateCount = CertificateCount;

    // Start image processing
    // (with asymmetric processing running in parallel if possible)
    if ((res == SB_SUCCESS) && (Hash_p != NULL))
    {
        uint8_t Digest[SBIF_ECDSA_BYTES];

        L_TRACE(LF_SBHYBRID, "Verifying hash of public key.");
        res = SBHYBRID_SHA2XX_Init(&Context_p->SymmContext);
        if (res == SB_SUCCESS)
        {
            res = SBHYBRID_SHA2XX_AddBlock(&Context_p->SymmContext,
                                           (const uint8_t *)&Header_p->PublicKey,
                                           sizeof(SBIF_ECDSA_PublicKey_t),
                                           true);
        }
        if (res == SB_SUCCESS)
        {
            do
            {
                // the below function returns
                // SB_SUCCESS when all processing has finished
                // SB_<other> in case of fatal error
                // SBHYBRID_PENDING when calculations are in progress
                res = SBHYBRID_SHA2XX_RunFsm(&Context_p->SymmContext);
#ifndef SBLIB_CF_REMOVE_POLLING_CALLS
                if (res == SBHYBRID_PENDING)
                {
                    if (SB_Poll(PollParam_p) == false)
                    {
                        L_DEBUG(LF_SBHYBRID, "ERROR: SB_Poll failed");
                        res = SB_ERROR_POLL;
                    }
                }
#endif /* !SBLIB_CF_REMOVE_POLLING_CALLS */
            } while (res == SBHYBRID_PENDING);
        }
        if (res != SB_SUCCESS)
        {
            L_DEBUG(LF_SBHYBRID, "ERROR: Public key hash failed (%d)",  res);
            res = SB_ERROR_VERIFICATION;
        }
        else
        {
            // Successfully calculated the digest, so check it
            SBHYBRID_SHA2XX_GetDigest(&Context_p->SymmContext, Digest);
            if (c_memcmp(Digest, Hash_p, SBHYBRID_DIGEST_BYTES) != 0)
            {
                L_DEBUG(LF_SBHYBRID, "ERROR: Public key hash verification failed");
                res = SB_ERROR_VERIFICATION;
            }
        }
    }

    // Perform the symmetric crypto on the image
    if (res == SB_SUCCESS)
    {
        uint32_t ImageLenLeft = ImageLen;
        unsigned int idx;

        // Calculate the hash over the attributes and the image
        res = SBHYBRID_SHA2XX_Init(&Context_p->SymmContext);
        if (res == SB_SUCCESS)
        {
            res = SBHYBRID_SHA2XX_AddBlock(&Context_p->SymmContext,
                                           (const uint8_t *)&Header_p->ImageAttributes,
                                           sizeof(Header_p->ImageAttributes),
                                           false);
        }

        // Add the image blocks
        for (idx = 0; idx < VectorCount; idx++)
        {
            SBIF_SGVector_t VectorIn = DataVectorsIn_p[idx];
            SBIF_SGVector_t VectorOut = DataVectorsOut_p[idx];

            if (ImageLenLeft < VectorIn.DataLen ||
                VectorIn.DataLen != VectorOut.DataLen)
            {
                res = SB_ERROR_ARGUMENTS;
                break;
            }

            while (res == SB_SUCCESS && VectorIn.DataLen)
            {
                uint32_t Blocklen;
                uint32_t Blocklen_in = VectorIn.DataLen;

#ifdef SBHYBRID_MAX_SIZE_DATA_BLOCKS
                // Limit the size of data block to its maximum
                if (Blocklen_in > SBHYBRID_MAX_SIZE_DATA_BLOCKS)
                {
                    Blocklen_in = SBHYBRID_MAX_SIZE_DATA_BLOCKS;
                }
#endif /* SBHYBRID_MAX_SIZE_DATA_BLOCKS */

#ifdef SBHYBRID_MAX_SIZE_LAST_DATA_BLOCK
                if ((ImageLenLeft == Blocklen_in) &&
                    (Blocklen_in > (SBHYBRID_MAX_SIZE_LAST_DATA_BLOCK + SBHYBRID_HASH_BLOCK)))
                {
                    // Make sure that the last block has a limited size
                    Blocklen_in -= SBHYBRID_MAX_SIZE_LAST_DATA_BLOCK;
                    Blocklen_in &= ~((SBHYBRID_HASH_BLOCK) - 1); // align at HASH block size
                }
#endif /* SBHYBRID_MAX_SIZE_LAST_DATA_BLOCK */

#ifndef SBLIB_CF_REMOVE_POLLING_CALLS
                Blocklen = SB_ReadData(PollParam_p, VectorIn.Data_p, Blocklen_in);
                if (Blocklen == 0)
                {
                    L_DEBUG(LF_SBHYBRID, "ERROR: SB_ReadData failed");
                    res = SB_ERROR_POLL;
                    break;
                }

                L_TRACE(LF_SBHYBRID,
                        "Processing block %p:%d => %d bytes",
                        (const void *)VectorIn.Data_p, VectorIn.DataLen,
                        Blocklen);
#else /* SBLIB_CF_REMOVE_POLLING_CALLS */
                Blocklen = Blocklen_in;
#endif /* !SBLIB_CF_REMOVE_POLLING_CALLS */

                // Continue ECDSA Verify processing in parallel with symmetric
                // crypto processing
                res = SBHYBRID_Do_Asym_RunFsm(Context_p, PollParam_p, Header_p);
                if (res == SB_SUCCESS)
                {
                    L_DEBUG(LF_SBHYBRID, "ERROR: Unexpected ECDSA success");
                    res = SB_ERROR_HARDWARE;
                    break;
                }
                if (res != SBHYBRID_PENDING)
                {
                    break;
                }

                if (DoDecrypt)
                {
                    // Decrypt block
                    AES_IF_CBC_Decrypt(Context_p,
                                       (const uint8_t *)VectorIn.Data_p,
                                       (uint8_t *)VectorOut.Data_p,
                                       Blocklen);

#ifdef SBHYBRID_SYNC_HASH_CIPHER
                    // Synchronize hash processing with cipher processing
                    do
                    {
                        res = SBHYBRID_SHA2XX_RunFsm(&(Context_p->SymmContext));
                    } while (res == SBHYBRID_PENDING);
                    if (res != SB_SUCCESS)
                    {
                        break;
                    }

                    SBHYBRID_SHA2XX_DmaRelease(&(Context_p->SymmContext));
#endif /* SBHYBRID_SYNC_HASH_CIPHER */
                }
                else
                {
                    if (VectorIn.Data_p != VectorOut.Data_p)
                    {
                        c_memcpy(VectorOut.Data_p, VectorIn.Data_p, Blocklen);
                    }

                    // Synchronize hash processing before adding a new block.
                    // Note: This is neccessary, because SB_ReadData may return
                    //       the data in fairly small blocks
                    do
                    {
                        res = SBHYBRID_SHA2XX_RunFsm(&(Context_p->SymmContext));
                    } while (res == SBHYBRID_PENDING);
                    if (res != SB_SUCCESS)
                    {
                        break;
                    }

                    SBHYBRID_SHA2XX_DmaRelease(&(Context_p->SymmContext));
                }

                // Update remaining image length counter
                ImageLenLeft -= Blocklen;

                // Hash block
                res = SBHYBRID_SHA2XX_AddBlock(&Context_p->SymmContext,
                                               (const uint8_t *)VectorOut.Data_p,
                                               Blocklen,
                                               ImageLenLeft == 0);

                // Update data pointers
                VectorIn.Data_p   += Blocklen / sizeof(VectorIn.Data_p[0]);
                VectorIn.DataLen  -= Blocklen;
                VectorOut.Data_p  += Blocklen / sizeof(VectorOut.Data_p[0]);
                VectorOut.DataLen -= Blocklen;
            }
        }

        if ((res == SB_SUCCESS) &&
            DoDecrypt &&
            (AES_IF_Ctx_GetError(Context_p) != AES_IF_RESULT_SUCCESS))
        {
            L_DEBUG(LF_SBHYBRID, "ERROR: Image decrypt failed");
            res = SB_ERROR_HARDWARE;
        }

        // Check all bytes were consumed.
        if ((res == SB_SUCCESS) && SBHYBRID_TEST_ImageLenLeft(ImageLenLeft))
        {
            L_DEBUG(LF_SBHYBRID, "ERROR: Image hash failed (%d/%d)", res, ImageLenLeft);
            res = SB_ERROR_HARDWARE;
        }

        if (res == SB_SUCCESS)
        {
            // Wait for the FSM to complete all symmetric crypto interactions
            do
            {
                // The below function returns:
                // SB_SUCCESS when all processing has been finished successful,
                // SBHYBRID_PENDING(=SB_ERROR_COUNT) when calculations are
                //                                   still in progress,
                // SB_<other> in case of fatal error.
                res = SBHYBRID_SHA2XX_RunFsm(&Context_p->SymmContext);
                if (res != SBHYBRID_PENDING)
                {
                    break;
                }

                // Continue ECDSA Verify processing in parallel with the
                // symmetric processing
                res = SBHYBRID_Do_Asym_RunFsm(Context_p, PollParam_p, Header_p);
                if (res == SB_SUCCESS)
                {
                    L_DEBUG(LF_SBHYBRID, "ERROR: Unexpected ECDSA success");
                    res = SB_ERROR_HARDWARE;
                    break;
                }
            } while (res == SBHYBRID_PENDING);
            if (res == SB_SUCCESS)
            {
                // Successfully calculated the digest, so pass it on to the
                // ECDSA Verify Processing
                SBHYBRID_SHA2XX_GetDigest(&Context_p->SymmContext,
                                          Context_p->SymmContext.Digest);

                L_TRACE_DIGEST(Context_p->SymmContext.Digest);
            }
        }
    }

    // Finish ECDSA Verify
    if (res == SB_SUCCESS)
    {
        // Wait for the FSM to complete all ECDSA Verify calculations
        do
        {
            // Provide ECDSA Verify Processing with the final digest
            // as soon as it is needed
            if (Context_p->NeedFinalDigest)
            {
                SBHYBRID_Verify_SetDigest(Context_p->EcdsaContext_p,
                                          Context_p->SymmContext.Digest);
                Context_p->NeedFinalDigest = 0;
            }

            // The final call to SBHYBRID_Do_Asym_RunFsm will return with
            // SB_SUCCESS or any of ECDSA verifications may return with
            // SB_ERROR_VERIFICATION
            res = SBHYBRID_Do_Asym_RunFsm(Context_p, PollParam_p, Header_p);
        } while (res == SBHYBRID_PENDING);

        // Done with the final verification
        if (res == SB_SUCCESS)
        {
            L_TRACE(LF_SBHYBRID, "Image verify succcessful");
        }
        else if (res != SB_ERROR_POLL)
        {
            L_DEBUG(LF_SBHYBRID, "ERROR: Image verify failed (%d)", res);
            res = SB_ERROR_VERIFICATION;
        }
    }

    if (res == SB_SUCCESS)
    {
        L_TRACE(LF_SBHYBRID, "SB_ECDSA_Image_Verify: succcessful");
    }
    else
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: SB_ECDSA_Image_Verify failed (%d)", res);
    }

    SBHYBRID_Uninitialize_HW(Context_p);

    return res;
}


SB_Result_t
SB_ECDSA_Image_DecryptWithKeyAndVerify(SB_StorageArea_t * const Storage_p,
                                       void * PollParam_p,
                                       const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
                                       const uint8_t * const Hash_p,
                                       const SBIF_ECDSA_Header_t * Header_p,
                                       const SBIF_SGVector_t * DataVectorsInput_p,
                                       const SBIF_SGVector_t * DataVectorsOutput_p,
                                       uint32_t VectorCount,
                                       SB_ConfidentialityKeyType_t ConfidentialityKeyType,
                                       const uint8_t * ConfidentialityKey)
{
    /* This function processes BLe/BLw images into BLp using SBIF Tools.
       After conversion, standard SB_ECDSA_Image_Verify is used.
       In case of BLp image, optional copying takes place, but nothing else. */

    /* Struct for temporary BLp header to pass to SB_ECDSA_Image_Verify. */
    typedef struct
    {
        SBIF_ECDSA_Header_t Header;
#if SBLIB_CFG_CERTIFICATES_MAX > 0
        SBIF_ECDSA_Certificate_t Certificates[SBLIB_CFG_CERTIFICATES_MAX];
#endif /* SBLIB_CFG_CERTIFICATES_MAX > 0 */
    } sbifHeaderStorage_t;

#ifdef SBHYBRID_WITH_SW
    /* Use temporary local buffer. */
    sbifHeaderStorage_t NewHeaderStorage;
    sbifHeaderStorage_t * NewHeader_p = &NewHeaderStorage;
#else /* !SBHYBRID_WITH_SW */
    /* Use DMA handle. */
    sbifHeaderStorage_t * NewHeader_p;
    DMAResource_Handle_t NewHeader_DMAHandle;
#endif /* SBHYBRID_WITH_SW */

    uint32_t storage_size = SBLIB_CFG_STORAGE_SIZE;
    size_t newHeaderSize = sizeof(sbifHeaderStorage_t);

    const SBIF_ECDSA_Header_t * header_p = Header_p;
    SB_Result_t sbres = SB_ERROR_ARGUMENTS;
    SBIFTC_Result_t sbiftcres;
    uint32_t image_type;
    bool decryptFlag = false;
    uint32_t CurveType;
#if (SBIF_ECDSA_BYTES > 32)
    const uint32_t CurveBytes[3] = {0, 48, 66};
#endif
    SB_Result_t res = SB_SUCCESS;

#ifndef SBHYBRID_WITH_SW
    {
        // Allocate a buffer for the converted header
        int dmares;
        DMAResource_AddrPair_t AddrPair;
        DMAResource_Properties_t Props = { 0, 0, 0, 0 };

        Props.Alignment = 4;
        Props.Size = sizeof(sbifHeaderStorage_t);

        dmares = DMAResource_Alloc(Props, &AddrPair, &NewHeader_DMAHandle);
        if (dmares != 0)
        {
            L_DEBUG(LF_SBHYBRID, "ERROR: DMA Allocation");
            return SB_ERROR_HARDWARE;
        }

        NewHeader_p = (sbifHeaderStorage_t *)AddrPair.Address.Native_p;
    }
#endif /* !SBHYBRID_WITH_SW */

    CurveType = SBHYBRID_Load_BE32(&Header_p->PubKeyType) >> 24;

    if (CurveType > 2)
    {
        res = SB_ERROR_VERIFICATION;
    }
    else
    {
#if (SBIF_ECDSA_BYTES <= 32)
        if (CurveType != 0)
        {
            res = SB_ERROR_VERIFICATION;
        }
#else
        if (SBIF_ECDSA_BYTES != CurveBytes[CurveType])
        {
            res = SB_ERROR_VERIFICATION;
        }
#endif
    }
    if (res == SB_ERROR_VERIFICATION)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: Wrong curve type (%u)\n",
                CurveType);
        return SB_ERROR_VERIFICATION;
    }

    image_type = SBHYBRID_Load_BE32(&Header_p->Type);
    image_type = SBIF_TYPE_TYPE(image_type);

    // Ensure only supported image formats are processed.
#ifdef SBLIB_CF_REMOVE_IMAGE_TYPE_P
    if (image_type == SBIF_IMAGE_BLTp)
    {
        return SB_ERROR_IMAGE_TYPE;
    }
#endif

#ifdef SBLIB_CF_REMOVE_IMAGE_TYPE_E
    if (image_type == SBIF_IMAGE_BLTe)
    {
        return SB_ERROR_IMAGE_TYPE;
    }
#endif

#ifdef SBLIB_CF_REMOVE_IMAGE_TYPE_W
    if ((image_type == SBIF_IMAGE_BLTw) ||
        (ConfidentialityKeyType == SB_CONFIDENTIALITY_KEY_TYPE_BLW))
    {
        return SB_ERROR_IMAGE_TYPE;
    }
#else
    if ((image_type != SBIF_IMAGE_BLTw) &&
        (ConfidentialityKeyType == SB_CONFIDENTIALITY_KEY_TYPE_BLW))
    {
        return SB_ERROR_IMAGE_TYPE;
    }
#endif

#ifdef SBLIB_CF_REMOVE_IMAGE_TYPE_X
    if ((image_type == SBIF_IMAGE_BLTx) ||
        (ConfidentialityKeyType == SB_CONFIDENTIALITY_KEY_TYPE_BLX))
    {
        return SB_ERROR_IMAGE_TYPE;
    }
#else
    if ((image_type != SBIF_IMAGE_BLTx) &&
        (ConfidentialityKeyType == SB_CONFIDENTIALITY_KEY_TYPE_BLX))
    {
        return SB_ERROR_IMAGE_TYPE;
    }

    /* BLx is not supported in auto mode. */
    if ((image_type == SBIF_IMAGE_BLTx) &&
        (ConfidentialityKeyType == SB_CONFIDENTIALITY_KEY_TYPE_AUTO))
    {
        return SB_ERROR_IMAGE_TYPE;
    }
#endif

    sbiftcres = SBIFTOOLS_Image_Convert_InitWorkspace(Storage_p, &storage_size);
    if (sbiftcres != SBIFTC_SUCCESS)
    {
        goto sbiftc_error;
    }

    /* Check if this is an encrypted image.
     * In case of encrypted image, we process it via SBIFTOOLS_Image_Convert.
     */
    if (image_type != SBIF_IMAGE_BLTp)
    {
        if (ConfidentialityKeyType == SB_CONFIDENTIALITY_KEY_TYPE_AUTO)
        {
            sbiftcres = SBIFTOOLS_Image_ConvertHeader(Header_p,
                                                      SBIF_IMAGE_BLTp,
                                                      SBIF_VERSION,
                                                      &(NewHeader_p->Header),
                                                      &newHeaderSize,
                                                      NULL,
                                                      0,
                                                      Storage_p);
        }
        else
        {
            SBIFTOOLS_ConvertOption_t opt;

            opt.ConvertOption = (ConfidentialityKeyType == SB_CONFIDENTIALITY_KEY_TYPE_BLW) ?
                                SBIFTOOLS_CONVERT_OPTION_AES_WRAPPING_KEY :
                                SBIFTOOLS_CONVERT_OPTION_AES_DECRYPTION_KEY;
            opt.ConvertOptionPointer = (void *)ConfidentialityKey;
            opt.ConvertOptionLength = SBIF_CFG_CONFIDENTIALITY_BITS / 8;

            sbiftcres = SBIFTOOLS_Image_ConvertHeader(Header_p,
                                                      SBIF_IMAGE_BLTp,
                                                      SBIF_VERSION,
                                                      &(NewHeader_p->Header),
                                                      &newHeaderSize,
                                                      &opt,
                                                      1,
                                                      Storage_p);
        }

        if (sbiftcres != SBIFTC_SUCCESS)
        {
            SBIFTOOLS_Image_Convert_UninitWorkspace(Storage_p);
sbiftc_error:
            /* Error from SBIFTOOLS, convert error into SB_Result_t. */
            switch (sbiftcres)
            {
            case SBIFTC_ERROR_ARGUMENTS:
                return SB_ERROR_ARGUMENTS;
            case SBIFTC_ERROR_CRYPTO:
                L_DEBUG(LF_SBHYBRID, "ERROR: SBIFTOOLS_Image_Convert crypto");
                return SB_ERROR_HARDWARE;
            case SBIFTC_ERROR_IMAGE_VERSION:
                return SB_ERROR_IMAGE_VERSION;
            case SBIFTC_ERROR_IMAGE_TYPE:
                return SB_ERROR_IMAGE_TYPE;
            default:
                L_DEBUG(LF_SBHYBRID,
                        "ERROR: SBIFTOOLS_Image_Convert unknown error %d",
                        sbiftcres);
                return SB_ERROR_HARDWARE;
            }
        }

        header_p = &(NewHeader_p->Header);
        decryptFlag = true;             /* Note: all formats except */
    }

    sbres = SB_ECDSA_Image_CopyOrDecrypt_Verify(Storage_p,
                                                PollParam_p,
                                                PublicKey_p,
                                                Hash_p,
                                                header_p,
                                                DataVectorsInput_p,
                                                DataVectorsOutput_p,
                                                VectorCount,
                                                decryptFlag);

    SBIFTOOLS_Image_Convert_UninitWorkspace(Storage_p);

    /* Clear output if verification failed. */
    if (sbres != SB_SUCCESS)
    {
        sgvector_clear(DataVectorsOutput_p, VectorCount);
    }

#ifndef SBHYBRID_WITH_SW
    DMAResource_Release(NewHeader_DMAHandle);
#endif /* !SBHYBRID_WITH_SW */
    return sbres;
}

SB_Result_t
SB_ECDSA_Image_DecryptAndVerify(SB_StorageArea_t * const Storage_p,
                                void * PollParam_p,
                                const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
                                const uint8_t * const Hash_p,
                                const SBIF_ECDSA_Header_t * Header_p,
                                const SBIF_SGVector_t * DataVectorsInput_p,
                                const SBIF_SGVector_t * DataVectorsOutput_p,
                                uint32_t VectorCount)
{
    return SB_ECDSA_Image_DecryptWithKeyAndVerify(Storage_p,
                                                  PollParam_p,
                                                  PublicKey_p,
                                                  Hash_p,
                                                  Header_p,
                                                  DataVectorsInput_p,
                                                  DataVectorsOutput_p,
                                                  VectorCount,
                                                  SB_CONFIDENTIALITY_KEY_TYPE_AUTO,
                                                  NULL);
}

/* end of file sbhybrid_api_decrypt.c */
