/* sbif_tools_convert.h
 *
 * Description: Tools for dealing with different SBIF enabled image formats.
 *              This file defines API for conversion function between
 *              between Secure Boot Image formats.
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

#ifndef INCLUDE_GUARD_SBIF_TOOLS_H
#define INCLUDE_GUARD_SBIF_TOOLS_H

#include "public_defs.h"
#include "cfg_sbif.h"
#include "sbif_ecdsa.h"
#include "sbif_vector.h"

/** Define error codes possible from conversion. */
typedef enum
{
    SBIFTC_SUCCESS,             /** Returned when a call has been successful.*/

    SBIFTC_ERROR_ARGUMENTS,     /** Error: Illegal arguments to function. */
    SBIFTC_ERROR_CRYPTO,        /** Error: Error performing cryptographic op.*/
    SBIFTC_ERROR_IMAGE_VERSION, /** Error: Wrong image version. */
    SBIFTC_ERROR_IMAGE_TYPE     /** Error: Unsupported image type. */
} SBIFTC_Result_t;

/** Define options that can be specified to conversion function. */
typedef enum
{
    /** BLe/BLx: specify encryption key to use (default == use random). */
    SBIFTOOLS_CONVERT_OPTION_AES_ENCRYPTION_KEY = 0x6501,

    /** Is encryption of data needed for operation?
        (true for conversion from BLp to BLe/BLx/BLw) */
    SBIFTOOLS_CONVERT_OPTION_NEED_AES_ENCRYPTION_OUT = 0x01006502,

    /** BLw: specify wrapping key to use (default == use built-in). */
    SBIFTOOLS_CONVERT_OPTION_AES_WRAPPING_KEY = 0x6503,

    /** Is decryption of data needed for operation?
        (true for conversion from BLe/BLx/BLw to BLp) */
    SBIFTOOLS_CONVERT_OPTION_NEED_AES_DECRYPTION_OUT = 0x01007002,

    /** BLx options. */
    /** Use this option to provide encryption key. */
    SBIFTOOLS_CONVERT_OPTION_AES_DECRYPTION_KEY = 0x7801,

    /** BLe/BLx output options: Use to get resulted encryption key. */
    SBIFTOOLS_CONVERT_OPTION_AES_ENCRYPTION_KEY_OUT = 0x01007801,

    /** For internal use by the functions, do not use directly.
        This option may only be specified as the last option. */
    SBIFTOOLS_CONVERT_OPTION_NEXT_OPTIONS = 0xFFFF,

} SBIFTOOLS_ConvertOptionType_t;

/** Option length is set to this value on error situations: */
#define SBIFTOOLS_CONVERT_OPTION_ERROR ((uint32_t)-1)

/** The structure for convert options provided to image conversion.

    There are three kinds of options, unknown options,
    input options and output options.
    (see definitions of SBIFTOOLS_ConvertOptionType_t, above.
    * Input options must always contain valid ConvertOptionPointer.
    * Output options may contain ConvertOptionPointer == NULL. In this case,
      the functions only updates the ConvertOptionLength values for these
      options and returns with SBIFTC_SUCCESS.
    * If all output options have ConvertOptionPointer != NULL and adequate
      length is provided for the options, the function,
      as part of its processing shall fill in the options if it returns
      SBIFTC_SUCCESS. If some of output options do not apply to the processing,
      the function must not return success.

    * Unknown or in-appropriate options have their length field modified
      to (uint32_t)-1 for signalling error.
 */
typedef struct
{
    SBIFTOOLS_ConvertOptionType_t ConvertOption;
    void *ConvertOptionPointer;
    uint32_t ConvertOptionLength;
} SBIFTOOLS_ConvertOption_t;

/** Convert SecureBoot image to another format.
    Format convertions supported:
    * BLx => BLe (keeps the same encryption key)
    * BLe => BLx (keeps the same encryption key)
    * BLp => BLe/BLx (uses new key, either random or specified key)
    * BLp => BLw (uses new random key)
    * BLe/BLx => BLp (removes encryption)

    When new key is being used or encryption is removed the data located in
    DataVectors_p array is modified in-place to new key or to remove key.

    Only conversions from current version to current version are allowed by
    default. Additional code might be required for other conversions.
    Notice that different formats offer different confidentiality protection
    levels (or do not offer confidentiality protection at all).

    @param Header_p
    Pointer to secure boot header structure.

    @param NewSbifType
    Desired resulting secure boot image format, one of SBIF_IMAGE_BLT*.

    @param NewSbifVersion
    Desired resulting secure boot image format version.
    Should be <= SBIF_VERSION.

    @param HeaderNew_p
    Pointer to preallocated secure boot header structure.

    @param HeaderNewSize_p
    Pointer to allocated size for HeaderNew_p.
    The error shall result if this size is too small.
    This parameter shall return used size of HeaderNew_p when
    SBIFTC_SUCCESS. On error situations, this value is updated to
    expected header size if applicable.

    @param DataVectors_p
    Must point to an initialised array of
    SBIF_SGVector_t structures. The sum of DataLen fields in the array
    shall match the ImageLen field in the header.

    @param VectorCount
    The number of entries in the array pointed to by DataVectors_p.

    @param ConvertOptions_p
    The options for conversion process. These options are used to
    provide extra parameters or to request extra data. Examine the
    options themselves for details.
    Most notable is that SBIFTOOL_CONVERT_OPTION_AES_DECRYPTION_KEY must be
    provided for BLx image format as input and equivalently
    SBIFTOOL_CONVERT_OPTION_AES_ENCRYPTION_KEY_OUT is provided when BLx image
    format is the result.
    If there is a problem with any of the options, the length field of
    the options is modified to (uint32_t) -1 and SBIFTC_ERROR_ARGUMENTS
    is returned.

    @param ConvertOptionCount
    Number of options provided to conversion.

    @param Workspace_p
    Workspace that has been successfully initialized with
    SBIFTOOLS_Image_Convert_InitWorkspace().

    @return
    Returns SBIFTC_SUCCESS on success. All other values indicate that
    an error has occurred.
 */
SBIFTC_Result_t
SBIFTOOLS_Image_Convert(
    const SBIF_ECDSA_Header_t *       Header_p,
    uint32_t                          NewSbifType,
    uint8_t                           NewSbifVersion,
    SBIF_ECDSA_Header_t *             HeaderNew_p,
    size_t * const                    HeaderNewSize_p,
    const SBIF_SGVector_t *           DataVectorsInput_p,
    const SBIF_SGVector_t *           DataVectorsOutput_p,
    uint32_t                          VectorCount,
    SBIFTOOLS_ConvertOption_t *       ConvertOptions_p,
    uint32_t                          ConvertOptionCount,
    void * const                      Workspace_p);


/*** SBIFTOOLS_Image_ConvertHeader

   This function is the same than SBIFTOOLS_Image_Convert, but
   conversion of the actual image data is not performed. For details,
   see SBIFTOOLS_Image_Convert.

   @sideeffect A notable side effect of SBIFTOOLS_Image_ConvertHeader
               is that the AES_IF processing is prepared for decryption,
               correct key and IV are loaded.
 */
SBIFTC_Result_t
SBIFTOOLS_Image_ConvertHeader(
    const SBIF_ECDSA_Header_t * const Header_p,
    const uint32_t                    NewSbifType,
    const uint8_t                     NewSbifVersion,
    SBIF_ECDSA_Header_t * const       HeaderNew_p,
    size_t * const                    HeaderNewSize_p,
    SBIFTOOLS_ConvertOption_t * const ConvertOptions_p,
    const uint32_t                    ConvertOptionCount,
    void * const                      Workspace_p);

/** Initialize workspace for SBIFTOOLS_Image_Convert()
    or query required length of workspace.

    @param Workspace_p
    Pointer to memory allocated for workspace or NULL.
    Similarly than malloc() allocated memory, the pointer Workspace_p must be
    suitably aligned for any kind of variable.

    @param Workspace_size_p
    Pointer to size of workspace provided.
    If Workspace_p is NULL, this parameter is used to store required
    workspace size.

    @return
    Returns SBIFTC_SUCCESS on success. All other values indicate that
    an error has occurred.

    @note
    As SBIFTOOLS_Image_Convert usually uses AES_IF via the workspace,
    it is required that there is only one initialized workspace.
 */
SBIFTC_Result_t
SBIFTOOLS_Image_Convert_InitWorkspace(
    void * const     Workspace_p,
    uint32_t * const Workspace_size_p);

/** Uninitialize workspace initialized with SBIFTOOLS_Image_Convert()
    and release associated resources.

    @param Workspace_p
    Pointer to workspace initialized with
    SBIFTOOLS_Image_Convert_InitWorkspace.
 */
void
SBIFTOOLS_Image_Convert_UninitWorkspace(
    void * const Workspace_p);

#endif /* Include Guard */

/* end of file sbif_tools_convert.h */
