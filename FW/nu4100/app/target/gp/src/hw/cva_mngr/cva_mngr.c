/****************************************************************************
 *
 *   FileName: cva_mngr.c
 *
 *   Author:  Dima S.
 *
 *   Date:
 *
 *   Description: cva mngr
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu2_internal.h"
#include "inu_common.h"
#include "err_defs.h"
#include "cva_mngr.h"
#include "nu4k_defs.h"
#include "cde_mngr_new.h"
#include "cde_drv_new.h"
#include "nufld.h"
#include "gme_drv.h"
#include "cmem.h"
#include "assert.h"

#ifdef __cplusplus
   extern "C" {
#endif
//#define __CVA_DEBUG_SET_IIC_ZERO__
//#define __CVA_DEBUG_TEST_ZERO_LINE__
//#define __CVA_DEBUG_SEND_IIC_TO_HOST__

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

#define CVA_READER_OFFSET  (0x80)
#define CVA_REG_WR(_register_, _value_)           (*(UINT32*)(cvaBaseAddress + (_register_))) = (_value_)

#define CVA_READER_REG_WR(_reader_, _register_, _value_)    (*(UINT32*)(cvaBaseAddress + ((_register_) + ((_reader_)*(CVA_READER_OFFSET))))) = (_value_)

#define CVA_ENGINE_TYPE_IIC     (1)
#define CVA_ENGINE_TYPE_FREAK   (6)
#define CVA_ENGINE_TYPE_DOG     (7)


/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
    typedef struct
    {
       //uint32 buff[4];
       UINT32 timeStamp0;
       UINT32 timeStamp1;
       UINT32 frameId:16;
       UINT32 engineType:4;
       UINT32 engineInst:2;
       UINT32 kpWidth2:10;
       UINT32 kpWidth1:2;
       UINT32 kpInFrame:21;
       UINT32 dummy:9;
    }CVA_MNGRP_ControlStreamT;

   
   typedef union
   {
      UINT32 buff[4];
      CVA_MNGRP_ControlStreamT field;
   }CVA_MNGRP_ControlStreamU;



typedef struct
{
   UINT32 height;
   UINT32 width;
   UINT32 xStart;
   UINT32 yStart;
   UINT32 numImages;          // related to hybrid configuration of PPU
}CVA_MNGRP_frameDimentionsCfgT;

typedef struct
{
   CDE_MNGRG_channnelInfoT*      iicReader;   
   CDE_MNGRG_channnelInfoT*      controlReader;
   UINT32                        controlBuffAddress;   
   UINT32                        lastIndex;
}CVA_MNGRP_cvaDbT;


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static UINT32 cvaBaseAddress;

static CVA_MNGRP_cvaDbT cvaDbTemp;
static CVA_MNGRP_cvaDbT *cvaDb;// = {0};
static CVA_MNGRG_freakFrameParamsT currentFrameParams = {0};
static UINT32 iicDone = 0;
/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static void *CvaControlReader;
/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/
/****************************************************************************
*
*  Function Name: 
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE CVA_MNGRP_testIIC( UINT32 *buffVirtAddr )
{
    ERRG_codeE                ret = CVA_MNGR__RET_SUCCESS;   
    UINT32 i = 0, cnt = 0;
    
    CMEM_cacheInv((void*)(buffVirtAddr), 640);

    for(i = 0; i < 640; i++ )
    {
        if( buffVirtAddr[i] != 0 ) {cnt++;}
    }

    if(cnt == 0)
        printf("\t\t testIIC: First line in buffer %p is all zeros\n", buffVirtAddr);
    else
        printf("\t\t testIIC: First line in buffer %p has %d non-zeros\n", buffVirtAddr, cnt);
    return ret;
}


/****************************************************************************
*
*  Function Name: CVA_DRVG_dumpRegs
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE control
*
****************************************************************************/
void CVA_MNGRG_dumpRegs(void)
{
   UINT32 regsOffset[] = {cva_offset_tbl_values};
   UINT32 regsResetVal[] = {cva_regs_reset_val};
   UINT32 reg;


   //to dump all the registers
//   LOGG_PRINT(LOG_INFO_E, NULL, "GME registers (number of regs = %d):\n",sizeof(regsOffset));
//   for (reg = 0; reg < (sizeof(regsOffset)/4); reg+=4)
//   {
//      LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x, 0x%04x = 0x%08x, 0x%04x = 0x%08x, 0x%04x = 0x%08x\n"
//                                    , regsOffset[reg],   *(volatile UINT32 *)(GME_BASE + regsOffset[reg])
//                                    , regsOffset[reg+1], *(volatile UINT32 *)(GME_BASE + regsOffset[reg+1])
//                                    , regsOffset[reg+2], *(volatile UINT32 *)(GME_BASE + regsOffset[reg+2])
//                                    , regsOffset[reg+3], *(volatile UINT32 *)(GME_BASE + regsOffset[reg+3]));
//   }


   printf("Modified CVA registers:\n");
   //compare against reset val
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg++)
   {
      if (*(volatile UINT32 *)(CVA_BASE + regsOffset[reg]) != regsResetVal[reg])
      {
         printf("0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(CVA_BASE + regsOffset[reg]));
      }
   }
}

/***************************************************************************
***************     G L O B A L         F U N C T I O N S     **************
****************************************************************************/

/****************************************************************************
*
*  Function Name: 
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE CVA_MNGRG_init( UINT32 memVirtAddr )
{
   ERRG_codeE                 ret = CVA_MNGR__RET_SUCCESS;   

   cvaDb = &cvaDbTemp; 
   memset((void*)cvaDb, 0xFF, sizeof(CVA_MNGRP_cvaDbT));

   cvaBaseAddress = (UINT32)memVirtAddr;

   //LOGG_PRINT(LOG_INFO_E, NULL, "Init CVA done.\n");

   return ret;
}

UINT8 CVA_MNGRG_getIicReady()
{
   return CVA_FREAK_FREAK_STATUS0_INTEGRAL_IMAGE_READY_R;
}

/****************************************************************************
*
*  Function Name: CVA_MNGRG_cvaIicCb
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE CVA_MNGRG_updateIICReady( UINT32 frameNum, UINT32 address)
{
    ERRG_codeE                    ret = CVA_MNGR__RET_SUCCESS;   
    UINT8 isReady;

    if(((frameNum /*- 1*/) & 0x01) == 0)    //even frame    
    {
        CVA_FREAK_BASE_ADDRESS_IMG_EVEN_FRM_BASE_ADDRESS_IMG_EVEN_FRM_W(address);
    }
    else    //odd frame
    {
        CVA_FREAK_BASE_ADDRESS_IMG_ODD_FRM_BASE_ADDRESS_IMG_ODD_W(address); 
    }

    if(CVA_MNGRG_isLeakyModeConfigured())
    {   
        CVA_FREAK_RECYCLE_KEYPOINTS_CONFIG0_LEAKY_MARGIN_YLINE_W( 0 ); // default is 5
        CVA_FREAK_RECYCLE_KEYPOINTS_CONFIG0_LEAKY_MINIMAL_YLINE_W( 0 ); // default is 10
        CVA_FREAK_RECYCLE_KEYPOINTS_CONFIG1_LEAKY_OCTAVE_FACTOR_W( 0 ); // default is 1408
        iicDone = 1;
    }
    else
    {
        isReady = CVA_FREAK_FREAK_STATUS0_INTEGRAL_IMAGE_READY_R;

        if(isReady == 1)
            printf(RED("There is a problem with INTEGRAL_IMAGE_READY \n"));
        CVA_FREAK_INT_IMAGE_READY_IN_DDR_INT_IMAGE_READY_IN_DDR_W(1);
    }

    return ret;
}

/****************************************************************************
*
*  Function Name: CVA_MNGRG_cvaIicCb
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE CVA_MNGRG_getFreakFrameParams(CVA_MNGRG_freakFrameParamsT* frkParams)
{
   ERRG_codeE                ret = CVA_MNGR__RET_SUCCESS;    

   CDE_DRVG_channelHandleT channelH = ((CDE_MNGRG_channnelInfoT*)CvaControlReader)->dmaChannelHandle;
   UINT32 destAddr  =  CDE_DRVG_getChannelDestAddress( channelH );
   CVA_MNGRP_ControlStreamU *control =  (CVA_MNGRP_ControlStreamU *)destAddr;

   CMEM_cacheInv((void*)(control - 1), sizeof(CVA_MNGRP_ControlStreamU));

   if(CVA_MNGRG_isLeakyModeConfigured())
   {    
        CVA_FREAK_RECYCLE_KEYPOINTS_CONFIG0_LEAKY_MARGIN_YLINE_W( 5 ); // default is 5
        CVA_FREAK_RECYCLE_KEYPOINTS_CONFIG0_LEAKY_MINIMAL_YLINE_W( 10 ); // default is 10
        CVA_FREAK_RECYCLE_KEYPOINTS_CONFIG1_LEAKY_OCTAVE_FACTOR_W( 1408 ); // default is 1408
        iicDone = 0;
   }
 //printf(BLUE("\t\getFrkParam 0x%08x : Eng-%d, outCnt 0x%05x, outWidth 0x%x, frameId 0x%08x\n"), 
 //   destAddr, (control - 1)->field.engineType,  (control - 1)->field.kpInFrame, (control - 1)->field.kpWidth2, (control - 1)->field.frameId);
   if((control - 1)->field.engineType == CVA_ENGINE_TYPE_FREAK)
   {
      frkParams->frameCnt  = (control - 1)->field.frameId;
      frkParams->descrNum  = (control - 1)->field.kpInFrame;
   }
   else
   {  
        LOGG_PRINT(LOG_DEBUG_E, NULL, "Control is corrupted. Engine %d\n", (control - 1)->field.engineType );
      
      frkParams->descrNum  = 0xFFFF;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: CVA_MNGRG_cvaIicCb
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
void CVA_MNGRG_cvaIicCb(CDE_MNGRG_userCbParamsT *userParams, void *arg)
{
   ERRG_codeE                    ret = CVA_MNGR__RET_SUCCESS;
    CDE_MNGRG_channnelInfoT   *chanInfo = (CDE_MNGRG_channnelInfoT*)userParams->channelInfo;
    CDE_DRVG_channelHandleT    h = chanInfo->dmaChannelHandle;
    UINT32 phyAddress;

    (void)arg;
#ifdef __CVA_DEBUG_SET_IIC_ZERO__
    UINT8 *imageData;

    if(h->scenarioType ==  CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS)
    {   
        ret = MEM_MAPG_getVirtAddr(MEM_MAPG_REG_CRAM_E/*MEM_MAPG_IAE_LUT_MRS_E*/ , ((MEM_MAPG_addrT)&imageData));
        memset(imageData, 0x00, 0x000A0000);
    }
    else
    {   
        memset(bufDescP->dataP, 0x00, bufDescP->dataLen);
        CMEM_cacheWb((void*)bufDescP->dataP, bufDescP->dataLen);
    }
#endif

    if(h->scenarioType ==  CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS)
    {   
        inu__reuseFreakStartMsg(userParams->systemFrameCntr, 0);
    }
    else
    {
       MEM_POOLG_bufDescT *bufDescP = userParams->buffDescriptorP;
       ret = MEM_POOLG_getDataPhyAddr(bufDescP,&phyAddress);
       if (ERRG_FAILED(ret))
         assert(0);
        CVA_MNGRG_updateIICReady(userParams->systemFrameCntr, phyAddress);
    }

    //printf(YELLOW("CVA_MNGRG_cvaIicCb: Hello from Cb. Frame = %d, PhyAddr = 0x%x\n"), userParams->systemFrameCntr, phyAddress);
#ifdef  __CVA_DEBUG_TEST_ZERO_LINE__
    CVA_MNGRP_testIIC( userParams->currVirtPtr );
#endif
// printf("\t\CVA_MNGRG_cvaIicCb: Status0 - 0x%08x, Status1 - 0x%08x, Status2 - 0x%08x, IIC stat %x, frk stat %x, ctrl stat %x, cva stat 0x%08x \n",
//    CVA_FREAK_FREAK_STATUS0_VAL, CVA_FREAK_FREAK_STATUS1_VAL, CVA_FREAK_FREAK_STATUS2_VAL,
//    CVA_AXI_IF2_INT_VAL, CVA_AXI_IF3_INT_VAL, CVA_AXI_IF6_INT_VAL, CVA_TOP_INTERRUPT_STATUS_VAL);

    CVA_DOG_INT_0_CLEAR_VAL   = 0xFFF;
    CVA_FREAK_INT_0_CLEAR_VAL = 0xFFF;//TODO C0
    CVA_AXI_IF2_INT_CLEAR_VAL   = 0x07;
    CVA_AXI_IF3_INT_CLEAR_VAL   = 0x07;
    CVA_AXI_IF6_INT_CLEAR_VAL   = 0x07;

#ifdef  __CVA_DEBUG_SEND_IIC_TO_HOST__
    inu_cva__iicFrameDoneCb(userParams, arg);
#else
    if(h->scenarioType !=  CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS)
        MEM_POOLG_free(userParams->buffDescriptorP);        // Closed because of new DMA implementation
#endif
}

/****************************************************************************
*
*  Function Name: CVA_MNGRG_controlFrameDoneCb
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
void CVA_MNGRG_controlFrameDoneCb(CDE_MNGRG_userCbParamsT *userParams, void *arg)
{
   CVA_MNGRP_ControlStreamU *control =  (CVA_MNGRP_ControlStreamU *)userParams->currVirtPtr;
   UINT8 i = 0;
   (void)arg;

   printf(GREEN("\t\tCONTROL: Cntr-%d, -1- (Eng-%d, outCnt 0x%x, outWidth 0x%x) -2- (Eng-%d, outCnt 0x%x, outWidth 0x%x) -3- (Eng-%d, outCnt 0x%x, outWidth 0x%x)\n"), userParams->systemFrameCntr, 
                (control + 2)->field.engineType,  (control + 2)->field.kpInFrame, (control + 2)->field.kpWidth2,
                (control + 1)->field.engineType,  (control + 1)->field.kpInFrame, (control + 1)->field.kpWidth2,
                control->field.engineType,  control->field.kpInFrame, control->field.kpWidth2);

    for(i = 0; i < 3; i++)
    {
        if((control+i)->field.engineType == CVA_ENGINE_TYPE_FREAK)
        {
            currentFrameParams.descrNum = (control+i)->field.kpInFrame;
        }
    }

    currentFrameParams.frameCnt = userParams->systemFrameCntr;



   MEM_POOLG_free(userParams->buffDescriptorP);

  

}


void CVA_MNGRG_setControlReader(void *reader)
{
    CvaControlReader = reader;
}

void CVA_MNGRG_setEnableCva()
{
   CVA_FREAK_ENABLE_ENABLE_W(1);
   CVA_DOG_ENABLE_ENABLE_W(3);
   CVA_IIC_CONFIG_ENABLE_W(1);

   CVA_REGISTERS_READY_DONE_W(1);
   
   GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_CVA_E);
   CVA_REGISTERS_READY_DONE_W(1);
   
   LOGG_PRINT(LOG_INFO_E, NULL, "set enable CVA done\n");
}

void CVA_MNGRG_setDisableCva()
{
   CVA_IIC_CONFIG_ENABLE_W(0);
   CVA_DOG_ENABLE_ENABLE_W(0);
   CVA_FREAK_ENABLE_ENABLE_W(0);

   LOGG_PRINT(LOG_INFO_E, NULL, "set disable CVA done\n");
}



void* CVA_MNGRG_getControlReader(void)
{
    return CvaControlReader;
}

void CVA_MNGRG_setFreakGo(void)
{
    CVA_FREAK_INT_IMAGE_READY_IN_DDR_INT_IMAGE_READY_IN_DDR_W(1);
    printf("Setting Freak GO bit\n");
}

UINT8 CVA_MNGRG_isLeakyModeConfigured(void)		// Leaky mode is enabled by removing "0" from EARLY_RELEASE_LINES field
{
    return ( (CVA_DOG_REDUCED_LATENCY_EARLY_RELEASE_LINES_R == 0) ? (0) : (1) );
}

UINT8 CVA_MNGRG_isDdrlessConfigured(void)
{
    return CVA_FREAK_DDRLESS_ACTIVE_DDR_MODE_R; //mode = 0 -> DDR mode
}

void CVA_MNGRG_setLeakyOpen(UINT32 go)
{
    CVA_MNGRG_yLineE yLineVal;
    
    if(go == 0)
    {
        yLineVal = CVA_MNGRG_YLINE_CLOSE;
    }
    else
    {
        if(iicDone == 1)
        {
            yLineVal = CVA_MNGRG_YLINE_OPEN_IIC_READY;
        }
        else
        {
            yLineVal = CVA_MNGRG_YLINE_OPEN_IIC_NOT_READY;
        }
    }
    CVA_FREAK_RECYCLE_KEYPOINTS_CONFIG0_LEAKY_MINIMAL_YLINE_W( (UINT32)yLineVal ); // default is 10 
}

/****************************************************************************
*
*  Function Name: CVA_MNGRG_calcIICCompressedSize
*
*  Description:   Calc buff size ddrless
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: HW manager
*
****************************************************************************/
UINT32 CVA_MNGRG_calcIICCompressedSize(UINT32 width, UINT32 height, UINT8 hybrid)           
{
    UINT32 long_line_width;
    UINT32 short_line_width;
    UINT32 total_capacity;
    UINT32 mod_width, mod_height;

    mod_width = ((width + 1) % 3);
    mod_height = ((height) % 3);

    long_line_width  = 1 + (width + 1) * 3;


    if ( mod_width == 2 ) 
    {
        short_line_width  = ((UINT32)((width + 2) / 3)) * (1 + 1 + 3) + 0;
    } 
    else if ( mod_width == 1 ) 
    {
        short_line_width  = ((UINT32)((width + 1) / 3)) * (1 + 1 + 3) + 1 + 3;
    } 
    else if ( mod_width == 0 ) 
    {
        short_line_width  = ((UINT32)((width + 1) / 3)) * (1 + 1 + 3) + 3;
    } 
    else 
    {
        short_line_width  = 0;
    }


    if ( mod_height == 2 ) 
    {
        total_capacity  = ((UINT32)((height) / 3)) * ( 2 * short_line_width + long_line_width) + (short_line_width + 2 * long_line_width);
    } 
    else if ( mod_height == 1 ) 
    {
        total_capacity  = ((UINT32)((height) / 3)) * ( 2 * short_line_width + long_line_width) + short_line_width + long_line_width;
    } 
    else if ( mod_height == 0 ) 
    { 
        total_capacity  = ((UINT32)((height) / 3)) * ( 2 * short_line_width + long_line_width) + long_line_width;
    } 
    else 
    {
        total_capacity  = 0;
    }
    total_capacity = ( (hybrid == 0) ? (total_capacity) : (total_capacity * 2));

    return total_capacity; 
}

void CVA_MNGRG_getDdrlessInfo(UINT32 num, UINT32 *addressP, UINT32 *sizeP)
{
   switch(num)
   {
      case(0):    {*addressP = CVA_FREAK_BASE0ADDR_DDRLESS_VAL; *sizeP = CVA_FREAK_MEM_CAPACITY_0_BYTES_VAL;  break;}
      case(1):    {*addressP = CVA_FREAK_BASE1ADDR_DDRLESS_VAL; *sizeP = CVA_FREAK_MEM_CAPACITY_1_BYTES_VAL;  break;}
      case(2):    {*addressP = CVA_FREAK_BASE2ADDR_DDRLESS_VAL; *sizeP = CVA_FREAK_MEM_CAPACITY_2_BYTES_VAL;  break;}
      case(3):    {*addressP = CVA_FREAK_BASE3ADDR_DDRLESS_VAL; *sizeP = CVA_FREAK_MEM_CAPACITY_3_BYTES_VAL;  break;}
      case(4):    {*addressP = CVA_FREAK_BASE4ADDR_DDRLESS_VAL; *sizeP = CVA_FREAK_MEM_CAPACITY_4_BYTES_VAL;  break;}
      case(5):    {*addressP = CVA_FREAK_BASE5ADDR_DDRLESS_VAL; *sizeP = CVA_FREAK_MEM_CAPACITY_5_BYTES_VAL;  break;}      
   }

}

#ifdef __cplusplus
   }
#endif

