/* dmares_types.h
 *
 * Driver Framework, DMAResource API, Type Definitions
 *
 * The document "Driver Framework Porting Guide" contains the detailed
 * specification of this API. The information contained in this header file
 * is for reference only.
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

#ifndef INCLUDE_GUARD_DMARES_TYPES_H
#define INCLUDE_GUARD_DMARES_TYPES_H

#include "basic_defs.h"         // bool, uint8_t, uint32_t, inline


/*----------------------------------------------------------------------------
 * DMAResource_Handle_t
 *
 * This handle represents a DMA Resource Record that holds information about
 * a memory resource that can be accessed by a device using DMA, from another
 * memory domain in the same host or from another host (CPU/DSP).
 */

typedef void * DMAResource_Handle_t;

typedef unsigned int DMAResource_AddrDomain_t;


/*----------------------------------------------------------------------------
 * DMAResource_Addr_t
 *
 * Address coupled with Domain. The caller is encouraged to store the address
 * with the domain information.
 * The type also avoid unsafe void-pointer function output parameters.
 */
typedef struct
{
    DMAResource_AddrDomain_t Domain;
    union
    {
        void * Native_p;                // Size is compiler dependent
        uint32_t Value32;               // Converted to 32-bits address value
        uint64_t Value64;               // Converted to 64-bits address value
    } Address;
} DMAResource_AddrPair_t;


/*----------------------------------------------------------------------------
 * DMAResource_Properties_t
 *
 * Buffer properties. When allocating a buffer, these are the requested
 * properties for the buffer. When registering or attaching to an externally
 * allocated buffer these properties describe the already allocated buffer.
 * The exact fields and values supported is implementation specific.
 *
 * For both uses, the data structure should be initialized to all zeros
 * before filling in some or all of the fields. This ensures forward
 * compatibility in case this structure is extended with more fields.
 *
 * Example usage:
 *     DMAResource_Properties_t Props = {0};
 *     Props.fIsCached = true;
 */
typedef struct
{
    uint32_t Size;       // size of the buffer in bytes
    int Alignment;       // buffer start address alignment
    // examples: 4 for 32bit
    uint8_t Bank;        // can be used to indicate on-chip memory
    bool fCached;        // true = SW needs to do coherency management
} DMAResource_Properties_t;


/*----------------------------------------------------------------------------
 * DMAResource_Record_t
 *
 * This type is the record that describes a DMAResource. The details of the
 * type are implementation specific and therefore in a separate header file.
 *
 * Several DMAResource API functions return a handle to a newly instantiated
 * record. Use DMAResource_Handle2RecordPtr to get a pointer to the actual
 * record and to access fields in the record.
 */

#include "dmares_record.h"   // DMAResource_Record_t definition


#endif /* Include Guard */

/* end of file dmares_types.h */
