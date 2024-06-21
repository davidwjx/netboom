/* eip28_ext.c
 *
 * Driver Library for the SafeXcel-EIP-28 Public Key Accelerator.
 * Hardware-specific implementation for EIP28_LNME.
 */

/*****************************************************************************
* Copyright (c) 2010-2018 INSIDE Secure B.V. All Rights Reserved.
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

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */

// EIP28 Driver Library API, functions:
// EIP28_GetWorkAreaSize
// EIP28_StartOp_ModExp_CupAmodB_CALLATOMIC
#include "eip28.h"


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "c_eip28.h"            // configuration options

#include "eip28_level0.h"       // EIP28 register access

#include "eip28_internal.h"     // Internal definitions.


/*----------------------------------------------------------------------------
 * EIP28_GetWorkAreaSize
 *
 * Output:
 *      ResultLen is in 32-bit words
 */
int
EIP28_GetWorkAreaSize(
        const EIP28_Operation_List_t op,
        const unsigned int NrOfOddPowers,
        const unsigned int ALen,
        const unsigned int ModLen)
{
    unsigned int ResultLen = 0;

    // Switch between the operation
    switch (op)
    {
        case MULTIPLY:
        case ADD:
        case SUBTRACT:
        case ADDSUB:
        case RIGHTSHIFT:
        case LEFTSHIFT:
        case DIVIDE:
        case MODULO:
        case COMPARE:
        case COPY:
            ResultLen = 0;
            break;

        case MODEXP:
        case MODEXP_CRT:
            // Work space with LNME is
            //    MAX(#of odd powers, 3) x (BLen + 2 - (BLen MOD 2))
            ResultLen = MAX(NrOfOddPowers, 3) * (ModLen + 2 - (ModLen & 1));
            break;

        case ECC_ADD_PROJECTIVE:
        case ECC_ADD_AFFINE:
        case ECC_MUL_PROJECTIVE:
        case ECC_MUL_AFFINE:
        case ECC_MUL_MONT:
            // Work space with LNME is
            // 18*(BLen + 2 + BLen MOD 2)
            ResultLen = 18 * (ModLen + 2 + (ModLen & 1));
            break;

         case MODINV:
            ResultLen = 4 * (MAX(ALen, ModLen) + 2 +
                                    (MAX(ALen, ModLen) & 1)) + 2;
            break;

      case DSA_SIGN:
            // Work space with PKCP is
            //   (2 * (BLen + 2 + BLen MOD 2)) +
            //      MAX(#of odd powers, 3) x (ALen + 2 - (ALen MOD 2))
            ResultLen = (2 * (ModLen + 2 + (ModLen & 1))) + MAX(ModLen, 3) * (ALen + 2 - (ALen & 1));
            break;

        case DSA_VERIFY:
            // Work space with PKCP is
            //   (2 * (BLen + 2 + BLen MOD 2)) + (ALen + 2 + ALen MOD 2) +
            //      (MAX(#of odd powers, 3) * (ALen + 2 - (ALen MOD 2)))
            ResultLen = (2 * (ModLen + 2 + (ModLen & 1))) + (ALen + 2 + (ALen & 1)) +
                            (MAX(ModLen, 3) * (ALen + 2 - (ALen & 1)));
            break;

        case ECCDSA_SIGN:
            // Work space with PKCP is
            //   19*(BLen + 2 + BLen MOD 2)
            ResultLen = 19 * (ModLen + 2 + (ModLen & 1));
            break;

        case ECCDSA_VERIFY:
            // Work space with PKCP is
            //   25*(BLen + 2 + BLen MOD 2)
            ResultLen = 25 * (ModLen + 2 + (ModLen & 1));
            break;

        default:
            ResultLen = 0;
            break;

    }

    return (int)ResultLen;
}


/*----------------------------------------------------------------------------
 * EIP28_StartOp_ModExp_CupAmodB_CALLATOMIC
 */
#ifndef EIP28_REMOVE_MODEXP
EIP28_Status_t
EIP28_StartOp_ModExp_CupAmodB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const EIP28_WordOffset_t C_wo,
        const unsigned int A_Len,
        const unsigned int BC_Len,
        const unsigned int NrOfOddPowers,
        const EIP28_WordOffset_t Result_wo)
{
    int Result_len;
    unsigned int Adj_BC_Len = BC_Len;
    EIP28_INSERTCODE_FUNCTION_ENTRY_CODE;
    // 0 < A_Len <= Max_Len
    EIP28_CHECK_INT_INRANGE(A_Len,  1, EIP28_VECTOR_MAXLEN);
    // 1 < B_Len <= Max_Len
    EIP28_CHECK_INT_INRANGE(BC_Len, 2, EIP28_VECTOR_MAXLEN);
    // 0 < NrOfOddPowers <= 16
    EIP28_CHECK_INT_INRANGE(NrOfOddPowers, 1, 16);

    Result_len = EIP28_GetWorkAreaSize(MODEXP,
                                       NrOfOddPowers,
                                       A_Len,
                                       BC_Len);

    // check the offset A_wo, B_wo, C_wo
    // The last X bytes of PKA RAM will be used as general scratchpad, and
    // should not overlap with any of input vectors and result vector
    EIP28_CHECK_INT_ATMOST(
            A_wo + A_Len,
            TrueIOArea_p->PKARamNrOfWords - EIP28_MODEXP_SCRATCHPAD_WORD_COUNT);
    EIP28_CHECK_INT_ATMOST(
            B_wo + BC_Len,
            TrueIOArea_p->PKARamNrOfWords - EIP28_MODEXP_SCRATCHPAD_WORD_COUNT);
    EIP28_CHECK_INT_ATMOST(
            C_wo + BC_Len,
            TrueIOArea_p->PKARamNrOfWords - EIP28_MODEXP_SCRATCHPAD_WORD_COUNT);
    EIP28_CHECK_INT_ATMOST(
            Result_wo + Result_len,
            (int) TrueIOArea_p->PKARamNrOfWords - EIP28_MODEXP_SCRATCHPAD_WORD_COUNT);

    // check offset to be 64-bit aligned
    EIP28_CHECK_64BIT_ALIGNMENT(A_wo);
    EIP28_CHECK_64BIT_ALIGNMENT(B_wo);
    EIP28_CHECK_64BIT_ALIGNMENT(C_wo);
    EIP28_CHECK_64BIT_ALIGNMENT(Result_wo);

    // Modulus B must be odd
    CHECK_PKARAM_WORD_IS_ODD(B_wo);

    // Base C < Modulus B

    // Modulus B > 2^32
    // BC_Len represents number of 32-bit words
    //if Length is less than one word report error
    EIP28_CHECK_INT_ATLEAST(BC_Len, 1);

    // Vector D may not overlap with any of the vectors
    // check for no overlap of result vector with vector A
    EIP28_CHECK_NO_OVERLAP(A_wo, A_Len, Result_wo, Result_len);
    // check for no overlap of result vector with vector B
    EIP28_CHECK_NO_OVERLAP(B_wo, BC_Len, Result_wo, Result_len);

    // Vector D may not overlap vector C,
    // except that PKA_CPTR = PKA_DPTR is allowed
    if (C_wo != Result_wo)
    {
        EIP28_CHECK_NO_OVERLAP(C_wo, BC_Len, Result_wo, Result_len);
    }

    // If the most significant word of B is zero, decrease BC_Len
    while ( Adj_BC_Len > 1 && EIP28_PKARAM_RD(Device, B_wo + Adj_BC_Len - 1) == 0 )
    {
        Adj_BC_Len --;
    }

    // write the offset of operand A, operand B, operand C and result
    EIP28_A_PTR_WR(Device, A_wo);
    EIP28_B_PTR_WR(Device, B_wo);
    EIP28_C_PTR_WR(Device, C_wo);
    EIP28_D_PTR_WR(Device, Result_wo);
    TrueIOArea_p->LastOperation_Result_wo = Result_wo;
    TrueIOArea_p->LastOperationUsedSequencer = true;
    TrueIOArea_p->LastOperation_ResultLen = Adj_BC_Len;

    // write the ALen, BLen
    EIP28_A_LEN_WR(Device, A_Len);
    EIP28_B_LEN_WR(Device, Adj_BC_Len);

    // write the NrofOddPowers
    EIP28_SHIFT_WR(Device, NrOfOddPowers);

    // Set the modexpvar bit & run bit in PKA_FUNCTION Register
    EIP28_FUNCTION_START_OPERATION(
            Device,
            EIP28_FUNCTION_SEQ_MODEXP);

            IDENTIFIER_NOT_USED(Result_len);

    EIP28_INSERTCODE_FUNCTION_EXIT_CODE;
}
#endif /* EIP28_REMOVE_MODEXP */


/*----------------------------------------------------------------------------
 * EIP28_CheckFsmError_LNME0Status_CALLATOMIC
 */
bool
EIP28_CheckFsmError_LNME0Status_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p)
{
    volatile EIP28_True_IOArea_t * const TrueIOArea_p = IOAREA(IOArea_p);
    Device_Handle_t Device = TrueIOArea_p->Device;
    bool rv = (EIP28_LNME0_STATUS_RD(Device) & EIP28_LNME0_STATUS_FSM_ERROR);

    if (rv)
    {
        // Reset fsm_error bit
        EIP28_LNME0_STATUS_FSM_ERROR_RESET(Device);
    }

    return rv;
}

/*----------------------------------------------------------------------------
 * EIP28_Status_SeqCntrl_CALLATOMIC
 */
uint32_t
EIP28_Status_SeqCntrl_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p)
{
    volatile EIP28_True_IOArea_t * const TrueIOArea_p = IOAREA(IOArea_p);
    Device_Handle_t Device = TrueIOArea_p->Device;

    // See if Sequencer status = 0x21 which means rejected command
    return EIP28_SEQUENCER_STATUS(Device);
}

/* end of file eip28_ext.c */
