
/****************************************************************************
 *
 *   FileName: sequence_mngr.c
 *
 *   Author:  Dima S.
 *
 *   Date:
 *
 *   Description: Control HW sequences
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/

#ifdef __cplusplus
   extern "C" {
#endif

#include "inu_common.h"
#include "inu_graph.h"
#include "sequence_mngr.h"
#include "ppe_mngr.h"
#include "ppe_drv.h"
#include "gme_drv.h"
#include "iae_mngr.h"
#include "isp_mngr.hpp"
#include "dpe_mngr.h"
#include "mipi_mngr.h"
#include "cva_mngr.h"
#include "depth_post_mngr.h"
#include "sensors_mngr.h"
#include "lut_mngr.h"
#include "cde_mngr_new.h"
#include "nufld.h"
#include "nu4k_defs.h"
#include "xml_path.h"
#include "xml_db.h"
#include "hcg_mngr.h"
#include "hca_mngr.h"
#include "assert.h"
#include "nu4100_gpv_regs.h"
#include "lut_mngr.h"
#ifdef CDE_DRVG_METADATA
#include "metadata_updater.h"
#include "metadata_target_config.h"
#include "gme_mngr.h"
#include "sensorsync_updater.h"
#include "helsinki.h"
#endif
/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define  SEQ_MNGRG_MAX_BLOCKS_NUM               (NUFLD_NUM_BLKS_E)
#define  SEQ_MNGRG_MAX_BLOCK_INSTANCES          (20)
#define  SEQ_MNGRG_MAX_SYSTEM_PATHS             (NU4K_MAX_DB_META_PATHS)
#define  SEQ_MNGRG_INVALID_ENTRY                (0xFF)
#define  NUM_OF_CVA_READERS                     (3)
#define  SEQ_MNGRG_MAX_CHANNEL_NUM              (12)
#define  SEQ_MNGRG_MAX_CHANNEL_NAME             (64)
#define  SEQ_MNGRG_NUM_PPUS_NOSCL               (4)
#define  SEQ_MNGRG_NUM_PPUS_SCL                 (2)
#define  SEQ_MNGRG_NUM_PPUS_HYBSCL              (2)
#define  SEQ_MNGRG_NUM_PPUS_HYB                 (3)
#define  SEQ_MNGRG_MAX_DBS_NUM                  (12)

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef enum
{
   SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E = 0,
   SEQ_MNGRG_STATUS_TYPE_CONFIGURED_E,
   SEQ_MNGRG_STATUS_TYPE_ENABLED_E,
   SEQ_MNGRG_STATUS_TYPE_INVALID_E,
}SEQ_MNGRG_statusTypeE;


typedef enum
{
   SEQ_MNGRG_COMMAND_TYPE_NONE_E,
   SEQ_MNGRG_COMMAND_TYPE_START_E,
   SEQ_MNGRG_COMMAND_TYPE_STOP_E,
   SEQ_MNGRG_COMMAND_TYPE_INVALID_E,
}SEQ_MNGRG_commandTypeE;


struct SEQ_MNGP_sequenceDbT_;

typedef ERRG_codeE (*SEQ_MNGRG_setConfigFunc)( struct SEQ_MNGP_sequenceDbT_ *dbP, NUFLD_blkE block, UINT32 blkInstance, void *arg );
typedef ERRG_codeE (*SEQ_MNGRG_setEnableFunc)( struct SEQ_MNGP_sequenceDbT_ *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle );
typedef ERRG_codeE (*SEQ_MNGRG_setDisableFunc)( struct SEQ_MNGP_sequenceDbT_ *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg );

typedef struct
{
   UINT32 height;
   UINT32 width;
   UINT32 bpp;
   UINT32 xStart;
   UINT32 yStart;
   UINT32 stride;
   UINT32 bufferHeght;
   UINT32 numImages; //number of interleaved images
   UINT32 numOfBuffers;
   UINT32 numLinesPerChunk; //number of lines per each DMA frame
}SEQ_MNGRG_streamReadCfgT;

typedef struct
{
   UINT32 height;
   UINT32 width;
   UINT32 bpp;
   UINT32 max_kp;
   UINT32 format;
   UINT32 numImages; //number of interleaved images
   UINT32 isDdrless;
   UINT32 isHybrid;
   UINT32 buffSize;
}SEQ_MNGRG_streamCvaReadCfgT;

typedef struct
{
   UINT32 height;
   UINT32 width;
   UINT32 bpp;
   UINT32 xStart;
   UINT32 yStart;
   UINT32 stride;
   UINT32 bufferHeght;
   UINT32 numImages; //number of interleaved images
   UINT32 numOfBuffers;
   UINT32 numLinesPerChunk; //number of lines per each DMA frame
}SEQ_MNGRG_streamIspReadCfgT;

typedef struct
{
   SEQ_MNGRG_setConfigFunc    setCfg;
   SEQ_MNGRG_setEnableFunc    enable;
   SEQ_MNGRG_setDisableFunc   disable;
} SEQ_MNGP_blockOperationT;


typedef struct
{
   UINT8 refCount;
   void* blockPrivData;
   UINT32 channelId;
   SEQ_MNGRG_xmlBlocksListT blockType;
} SEQ_MNGP_blockInfoT;

typedef struct
{
   SEQ_MNGP_blockInfoT blockInfo[SEQ_MNGRG_MAX_BLOCK_INSTANCES];
   SEQ_MNGP_blockOperationT blockOperation;
   UINT8              blockInstanceNum;
} SEQ_MNGP_blockVecT;

typedef struct
{
   SEQ_MNGRG_statusTypeE    status;
   UINT8                    block[SEQ_MNGRG_XML_BLOCK_LAST_E];
   void*                    channelCb;
   void*                    cbArgs;
   SEQ_MNGRG_xmlBlocksListT startBlock;
}SEQ_MNGRG_pathInfoT;

/* Extended DMA Interleaving mode params */
typedef struct
{
   UINT32 numDMAInfos;
   CDE_MNGRG_channnelInfoT *DMAInfos[4];
}SEQ_MNGRG_extIntDMAInfo;

typedef struct
{
    UINT32 extIntChs; /* Mask for extended DMA interleaving channels */
    SEQ_MNGRG_extIntDMAInfo extIntDMAInfos;
}SEQ_MNGRG_extIntInfoT;

typedef struct SEQ_MNGP_sequenceDbT_
{
   SEQ_MNGRG_pathInfoT        path[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   SEQ_MNGP_blockVecT         blockVec[SEQ_MNGRG_XML_BLOCK_LAST_E];
   XMLDB_dbH                  dbH;
   CDE_MNGRG_channnelInfoT    *cvaReaders[3];
   char                       *name;
   void                       *vcsIsrH[6];
   UINT32                     readersBitmap;
   UINT8                      configDone;
   UINT8                      injectInterleaveImage;
   UINT8                      isMultiGraphInj;
   UINT32                     setConfigDoneISPBitmap;
   UINT32                     setEnableDoneISPBitmap;
   SEQ_MNGRG_extIntInfoT      extIntInfo;
}SEQ_MNGP_sequenceDbT;

typedef struct
{
   UINT32                   used;
   HCG_MNGRG_voterHandle    voterHandle;
   HCA_MNGRG_consumerHandle consumerHandle;
   SEQ_MNGRG_xmlBlocksListT inputBlock;
   SEQ_MNGRG_xmlBlocksListT outputBlock;
   UINT32                   inputBlockInstance;
   UINT32                   outputBlockInstance;
   char                     name[SEQ_MNGRG_MAX_CHANNEL_NAME];
}SEQ_MNGP_channelDbT;

typedef struct
{
   UINT32                   deviceBaseAddress;
} GPV_DRVP_deviceDescT;


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static OS_LYRG_mutexT ppeCropMutex;

static OS_LYRG_mutexT calUpdateMutex;

#if 0
static CDE_DRVG_PeriphClientIdE ppeAxiBuffers[NU4K_NUM_AXI_READERS] =
      {CDE_DRVG_PERIPH_CLIENT_AXI_RD0_TX_E, CDE_DRVG_PERIPH_CLIENT_AXI_RD1_TX_E, CDE_DRVG_PERIPH_CLIENT_AXI_RD2_TX_E, CDE_DRVG_PERIPH_CLIENT_AXI_RD3_TX_E,
       CDE_DRVG_PERIPH_CLIENT_AXI_RD4_TX_E, CDE_DRVG_PERIPH_CLIENT_AXI_RD5_TX_E, CDE_DRVG_PERIPH_INVALID_E, CDE_DRVG_PERIPH_INVALID_E,
       CDE_DRVG_PERIPH_CLIENT_AXI_RD8_TX_E, CDE_DRVG_PERIPH_CLIENT_AXI_RD9_TX_E, CDE_DRVG_PERIPH_INVALID_E, CDE_DRVG_PERIPH_INVALID_E};
static CDE_DRVG_PeriphClientIdE ppeAxiWriterBuffers[NU4K_NUM_WRITERS] =
     {CDE_DRVG_PERIPH_CLIENT_AXI_WB0_RX_E, CDE_DRVG_PERIPH_CLIENT_AXI_WB1_RX_E, CDE_DRVG_PERIPH_CLIENT_AXI_WB2_RX_E,
      CDE_DRVG_PERIPH_CLIENT_CVJ0_HD_RX_E, CDE_DRVG_PERIPH_CLIENT_CVJ1_HD_RX_E, CDE_DRVG_PERIPH_CLIENT_CVJ2_KP_RX_E};
#endif
static SEQ_MNGP_channelDbT  channelDb[SEQ_MNGRG_MAX_CHANNEL_NUM];
/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/
int orderList[SEQ_MNGRG_XML_BLOCK_LAST_E]={
   SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E,
   SEQ_MNGRG_XML_BLOCK_SLU_E,
   SEQ_MNGRG_XML_BLOCK_GEN_E,
   SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E,
   SEQ_MNGRG_XML_BLOCK_SENS_E,
   SEQ_MNGRG_XML_BLOCK_MEDIATOR_E,
   SEQ_MNGRG_XML_BLOCK_MIPI_RX_E,
   SEQ_MNGRG_XML_BLOCK_PAR_RX_E,
   SEQ_MNGRG_XML_BLOCK_INJECTION_E,
   SEQ_MNGRG_XML_BLOCK_ISP_E,
   SEQ_MNGRG_XML_BLOCK_IAU_E,
   SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E,
   SEQ_MNGRG_XML_BLOCK_HIST_E,
   SEQ_MNGRG_XML_BLOCK_DPE_E,
   SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E,
   SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E,
   SEQ_MNGRG_XML_BLOCK_PPU_SCL_E,
   SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E,
   SEQ_MNGRG_XML_BLOCK_PPU_HYB_E,
   SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E,
   SEQ_MNGRG_XML_BLOCK_DPHY_TX_E,
   SEQ_MNGRG_XML_BLOCK_RD_E,
   SEQ_MNGRG_XML_BLOCK_RDOUT_E,
   SEQ_MNGRG_XML_BLOCK_CVA_E,
   SEQ_MNGRG_XML_BLOCK_CVA_RD_E,
   SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E,
   SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E
};

static HCG_MNGRG_hwUnitE  mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_LAST_E];
static HCA_MNGRG_hwUnitE  mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_LAST_E];


int orderListStart[SEQ_MNGRG_XML_BLOCK_LAST_E]={
   SEQ_MNGRG_XML_BLOCK_GEN_E,              // 0
   SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E,       // 1
   SEQ_MNGRG_XML_BLOCK_SENS_E,             // 2
   SEQ_MNGRG_XML_BLOCK_MEDIATOR_E,         // 3
   SEQ_MNGRG_XML_BLOCK_MIPI_RX_E,          // 4
   SEQ_MNGRG_XML_BLOCK_PAR_RX_E,           // 5
   SEQ_MNGRG_XML_BLOCK_INJECTION_E,        // 6
   SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E,     // 7
   SEQ_MNGRG_XML_BLOCK_SLU_E,              // 8
   SEQ_MNGRG_XML_BLOCK_ISP_E,              // 9
   SEQ_MNGRG_XML_BLOCK_IAU_E,              // 10
   SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E,        // 11
   SEQ_MNGRG_XML_BLOCK_HIST_E,             // 12
   SEQ_MNGRG_XML_BLOCK_DPE_E,              // 13
   SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E,       // 14
   SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E,        // 15
   SEQ_MNGRG_XML_BLOCK_PPU_SCL_E,          // 16
   SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E,       // 17
   SEQ_MNGRG_XML_BLOCK_PPU_HYB_E,          // 18
   SEQ_MNGRG_XML_BLOCK_RD_E,               // 19
   SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E,       // 20
   SEQ_MNGRG_XML_BLOCK_CVA_RD_E,           // 21
   SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E,        // 22
   SEQ_MNGRG_XML_BLOCK_CVA_E,              // 23
   SEQ_MNGRG_XML_BLOCK_DPHY_TX_E,          // 24
   SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E,        // 25
   SEQ_MNGRG_XML_BLOCK_RDOUT_E,            // 26
};


static GPV_DRVP_deviceDescT GPV_DRVP_deviceDesc;
static SEQ_MNGP_sequenceDbT *SEQ_MNGRP_dbsP[SEQ_MNGRG_MAX_DBS_NUM] = {0};

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static ERRG_codeE SEQ_MNGRP_getDmaWriterDimentions( XMLDB_dbH hwDb, UINT8 writerNum, SEQ_MNGRG_streamReadCfgT *cfg, UINT32 numWriters, UINT8 injectInterleaveImage );
static ERRG_codeE SEQ_MNGRP_writerSchedulerRegister( SEQ_MNGP_sequenceDbT *dbP );
static ERRG_codeE SEQ_MNGRP_writerSchedulerUnregister( SEQ_MNGP_sequenceDbT *dbP );

static ERRG_codeE SEQ_MNGRG_findDbInAccessTableISP(SEQ_MNGRG_handleT *handleP, char *name_1, char *name_2, char *name_3);
/****************************************************************************
*
*  Function Name: SEQ_MNGRP_convertFormat
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
static UINT32 SEQ_MNGRP_convertToMipiFormat(UINT16 nucfg_format)
{
   switch(nucfg_format)
   {
      case(NUCFG_FORMAT_YUV420_8BIT_LEGACY_E):
         return YUV420_8_LEGACY;
      case(NUCFG_FORMAT_YUV420_8BIT_E):
         return YUV420_8;
      case(NUCFG_FORMAT_YUV420_10BIT_E):
         return YUV420_10;
      case(NUCFG_FORMAT_RGB444_E):
         return RGB444;
      case(NUCFG_FORMAT_RGB555_E):
         return RGB555;
      case(NUCFG_FORMAT_RGB565_E):
         return RGB565;
      case(NUCFG_FORMAT_RGB666_E):
         return RGB666;
      case(NUCFG_FORMAT_RGB888_E):
         return RGB888;
      case(NUCFG_FORMAT_RAW6_E):
         return RAW6;
      case(NUCFG_FORMAT_RAW7_E):
         return RAW7;
      case(NUCFG_FORMAT_YUV422_8BIT_E):
         return YUV422_8;
      case(NUCFG_FORMAT_YUV422_10BIT_E):
         return YUV422_10;
      case(NUCFG_FORMAT_RAW8_E):
      case(NUCFG_FORMAT_GEN_8_E):
      case(NUCFG_FORMAT_GEN_16_E):
         return RAW8;
      case(NUCFG_FORMAT_RAW10_E):
         return RAW10;
      case(NUCFG_FORMAT_RAW12_E):
      case(NUCFG_FORMAT_GEN_12_E):
         return RAW12;
      case(NUCFG_FORMAT_RAW14_E):
         return RAW14;
      case(NUCFG_FORMAT_DISPARITY_E):
      case(NUCFG_FORMAT_DEPTH_E):
         return RGB565;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Unsupported format %d for MIPI TX\n",nucfg_format);
         assert(0);
   }
}


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
static ERRG_codeE SEQ_MNGRP_storeDbInAccessTable(SEQ_MNGP_sequenceDbT *dbP)
{
   UINT32 i;
   for(i = 0; i < SEQ_MNGRG_MAX_DBS_NUM; i++)
   {
      if (!SEQ_MNGRP_dbsP[i])
      {
         SEQ_MNGRP_dbsP[i] = dbP;
         return HW_MNGR__RET_SUCCESS;
      }
   }
   return HW_MNGR__ERR_OUT_OF_RSRCS;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_getSensorNumAndCalibMode
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
static ERRG_codeE SEQ_MNGRP_getSensorNumAndCalibMode(SEQ_MNGRG_handleT handle, UINT32 iau, CALIB_iauTosensor *iauToSensor)
{
   UINT32 iauInd;
   ERRG_codeE ret;
   UINT32 pathNum,bypassBitMap = 0;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;

   if (NUCFG_isBypass(dbP->dbH,&bypassBitMap) == TRUE)
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Bypass mode bitfield 0x%x\n",bypassBitMap);
      return HW_MNGR__ERR_OUT_OF_RSRCS;
   }

   for(pathNum = 0;pathNum < SEQ_MNGRG_MAX_SYSTEM_PATHS;pathNum++ )
   {
      iauInd = dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_IAU_E];
      if ((iauInd >= 4) )
      {
         iauInd = dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E];
         if (iauInd != 0xff)
            iauInd += 2;// colour iau starts from IAU 2
      }
      if (iau == iauInd)
      {
         if (dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_SENS_E] != 0xFF)
         {
            iauToSensor->sensorId =  dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_SENS_E];
            iauToSensor->calib=0xff; // to ignore empty xml values
            if (iauToSensor->sensorId != 0xFF)
            {
               XMLDB_getValue(dbP->dbH, NUFLD_calcPath(NUFLD_SENSOR_E, iauToSensor->sensorId, SENSORS_SENS_0_OP_MODE_E), &iauToSensor->calib);
               switch (iauToSensor->calib)
               {
                  case 1:
                  iauToSensor->calib=CALIB_LUT_MODE_BIN_E;
                  break;
                  case 2:
                  iauToSensor->calib=CALIB_LUT_MODE_VERTICAL_BINNING_E;
                  break;
                  case 3:
                  iauToSensor->calib=CALIB_LUT_MODE_FULL_E;
                  break;
                  default:
                  iauToSensor->calib=CALIB_NUM_LUT_MODES_E;
                  break;
               }
            }
         }
         else if (dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] != 0xFF)
         {
            UINT32 val,cnt = 0;
            //printf("at path %d found writer %d for iau %d\n",pathNum,dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E],iau);
            ret = XMLDB_getValue(dbP->dbH, NUFLD_calcPath(NUFLD_META_WRITERS_E, dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E], META_WRITERS_WT_0_SRC_SENSORS_E), &val);
            if (ERRG_SUCCEEDED(ret))
            {
               //TODO - interleaved image into same writer buffer
               while(val)
               {
                  if (val & 0x1)
                  {
                     iauToSensor->sensorId = cnt;
                  }
                  val = val >> 1;
                  cnt++;

               }
               ret = XMLDB_getValue(dbP->dbH, NUFLD_calcPath(NUFLD_META_WRITERS_E, dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E], META_WRITERS_WT_0_SENSOR_RESOLUTION_MODE_E), &val);
               if (ERRG_SUCCEEDED(ret))
               {
                  iauToSensor->calib = val;
               }
               else
               {
                  LOGG_PRINT(LOG_ERROR_E, NULL, "Can't find resolution mode for writer instance %d\n",dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E]);
                  assert(0);
               }
               //printf("writer %d: sensor %d, mode %d, iau %d\n",
               //        dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E],iauToSensor->sensorId,iauToSensor->calib,iau);
               return HW_MNGR__RET_SUCCESS;
            }
            else
            {
               //injection without calib
               return HW_MNGR__ERR_OUT_OF_RSRCS;
            }
         }
         else
         {
            printf("path %d had iau %d configured, but no input!\n",pathNum,iauInd);
            assert(0);
         }
      }
   }
   return HW_MNGR__RET_SUCCESS;
}



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
static char *SEQ_MNGRP_blkType2string(SEQ_MNGRG_xmlBlocksListT blockType)
{
   switch(blockType)
   {
      case(SEQ_MNGRG_XML_BLOCK_GEN_E):
      {
         return "Generator";
      }
      case(SEQ_MNGRG_XML_BLOCK_SENS_E):
      {
         return "Sensor";
      }
      case(SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E):
      {
         return "Sensor group";
      }
      case(SEQ_MNGRG_XML_BLOCK_MIPI_RX_E):
      {
         return "MIPI bus";
      }
      case(SEQ_MNGRG_XML_BLOCK_PAR_RX_E):
      {
         return "Parallel bus";
      }
      case(SEQ_MNGRG_XML_BLOCK_INJECTION_E):
      {
         return "AXI writer";
      }
      case(SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E):
      case(SEQ_MNGRG_XML_BLOCK_SLU_E):
      {
         return "SLU";
      }
      case(SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E):
      case(SEQ_MNGRG_XML_BLOCK_IAU_E):
      {
         return "IAU";
      }
      case(SEQ_MNGRG_XML_BLOCK_HIST_E):
      {
         return "Histogram";
      }
      case(SEQ_MNGRG_XML_BLOCK_DPE_E):
      {
         return "DPE";
      }
      case(SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E):
      {
         return "Depth post";
      }
      case(SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E):
      case(SEQ_MNGRG_XML_BLOCK_PPU_SCL_E):
      case(SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E):
      case(SEQ_MNGRG_XML_BLOCK_PPU_HYB_E):
      {
         return "PPU";
      }
      case(SEQ_MNGRG_XML_BLOCK_RD_E):
      {
         return "AXI reader";
      }
      case(SEQ_MNGRG_XML_BLOCK_RDOUT_E):
      {
         return "Image AXI";
      }
      case(SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E):
      {
         return "CVA DMA";
      }
      case(SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E):
      {
         return "VSC CSI";
      }
      case(SEQ_MNGRG_XML_BLOCK_DPHY_TX_E):
      {
         return "DPHY TX";
      }
      case(SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E):
      {
         return "ISP DMA";
      }
      case(SEQ_MNGRG_XML_BLOCK_ISP_E):
      {
         return "ISP";
      }
      default:
      return "unknown block type";
   }
}


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
static NUFLD_blkE SEQ_MNGRP_xmlblk2nufld(SEQ_MNGRG_xmlBlocksListT block)
{
   switch( block )
   {
      case SEQ_MNGRG_XML_BLOCK_GEN_E:        {return NUFLD_GEN_E;}
      case SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E: {return NUFLD_SENSOR_GROUP_E;}
      case SEQ_MNGRG_XML_BLOCK_SENS_E:       {return NUFLD_SENSOR_E;}
      case SEQ_MNGRG_XML_BLOCK_MEDIATOR_E:   {return NUFLD_MEDIATOR_E;}
      case SEQ_MNGRG_XML_BLOCK_MIPI_RX_E:    {return NUFLD_MIPI_RX_E;}
      case SEQ_MNGRG_XML_BLOCK_PAR_RX_E:     {return NUFLD_PAR_RX_E;}
      case SEQ_MNGRG_XML_BLOCK_INJECTION_E:  {return NUFLD_AXIWR_E;}
      case SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E:  {return NUFLD_SLU_PARALLEL_E;}
      case SEQ_MNGRG_XML_BLOCK_SLU_E:        {return NUFLD_SLU_E;}
      case SEQ_MNGRG_XML_BLOCK_IAU_E:        {return NUFLD_IAU_E;}
      case SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E:  {return NUFLD_IAU_COLOR_E;}
      case SEQ_MNGRG_XML_BLOCK_HIST_E:       {return NUFLD_HIST_E;}
      case SEQ_MNGRG_XML_BLOCK_DPE_E:        {return NUFLD_DPE_E;}
      case SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E: {return NUFLD_DPP_E;}
      case SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E:  {return NUFLD_PPU_NOSCL_E;}
      case SEQ_MNGRG_XML_BLOCK_PPU_SCL_E:    {return NUFLD_PPU_SCL_E;}
      case SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E: {return NUFLD_PPU_HYBSCL_E;}
      case SEQ_MNGRG_XML_BLOCK_PPU_HYB_E:    {return NUFLD_PPU_HYB_E;}
      case SEQ_MNGRG_XML_BLOCK_CVA_E:        {return NUFLD_CVA_E;}
      case SEQ_MNGRG_XML_BLOCK_RD_E:         {return NUFLD_AXIRD_E;}
      case SEQ_MNGRG_XML_BLOCK_RDOUT_E:      {return NUFLD_META_READERS_E;}
      case SEQ_MNGRG_XML_BLOCK_CVA_RD_E:     {return NUFLD_CVARD_E;}
      //should be meta, but issue in nucfg does not make CVA_RD in the path. so we use CVA_RDOUT
      case SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E:  {return NUFLD_CVARD_E;}
      case SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E: {return NUFLD_VSC_CSI_TX_E;}
      case SEQ_MNGRG_XML_BLOCK_DPHY_TX_E:    {return NUFLD_DPHY_TX_E;}
      case SEQ_MNGRG_XML_BLOCK_ISP_E:        {return NUFLD_ISP_E;}
      case SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E:  {return NUFLD_META_ISP_RD_E;}

      case SEQ_MNGRG_XML_BLOCK_LAST_E:
      default:
         return NUFLD_INVALID_E;
   }
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_getDmaCvaChannelDimentions
*
*  Description:   Access to XML DB and get reader configuration
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
static ERRG_codeE SEQ_MNGRP_getDmaCvaChannelDimentions( XMLDB_dbH hwDb, UINT8 readerNum, SEQ_MNGRG_streamCvaReadCfgT *cfg )
{
   // TODO: Rewrite for B0
   ERRG_codeE     ret = HW_MNGR__RET_SUCCESS;
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_CVA_RD_E, readerNum, META_CVA_READERS_CVA_RD_0_OUT_RES_WIDTH_E),  &cfg->width);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_CVA_RD_E, readerNum, META_CVA_READERS_CVA_RD_0_OUT_RES_HEIGHT_E), &cfg->height);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_CVA_RD_E, readerNum, META_CVA_READERS_CVA_RD_0_OUT_RES_BPP_E),    &cfg->bpp);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_CVA_RD_E, readerNum, META_CVA_READERS_CVA_RD_0_OUT_RES_MAX_KP_E), &cfg->max_kp);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_CVA_RD_E, readerNum, META_CVA_READERS_CVA_RD_0_FORMAT_E),         &cfg->format);

   ret = XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_CVA_E, 0, NU4100_CVA_FREAK_DDRLESS_ACTIVE_DDR_MODE_E),         &cfg->isDdrless);
   if(ret == XMLDB__EMPTY_ENTRY)
   {
        cfg->isDdrless = 0;
   }

   ret = XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_PPU_HYBSCL_E, 0, NU4100_PPE_PPU7_STITCH_CFG_BYPASS_E),         &cfg->isHybrid);
   if(ret == XMLDB__EMPTY_ENTRY)
   {
        cfg->isHybrid = 0;
   }
   cfg->isHybrid ^= 1;  // register indication is non hybrid

    if(cfg->isHybrid)      // hybrid mode
    {
        cfg->width += 2;
        cfg->max_kp *= 2;
    }
    else
    {
        cfg->width += 1;
    }
        cfg->height += 1;
    
   cfg->max_kp += 1;
   cfg->buffSize = CVA_MNGRG_calcIICCompressedSize( ((cfg->isHybrid == 1) ? (cfg->width/2 - 1) : (cfg->width - 1)), (cfg->height - 1), cfg->isHybrid );


   printf("w-%d, h-%d, b-%d, k-%d, f-%d isddrless %d isHybrid %d buffSize %d\n", cfg->width, cfg->height, cfg->bpp, cfg->max_kp, cfg->format, cfg->isDdrless, cfg->isHybrid, cfg->buffSize);

   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_getDmaChannelDimentions
*
*  Description:   Access to XML DB and get reader configuration
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
static ERRG_codeE SEQ_MNGRP_getDmaChannelDimentions( XMLDB_dbH hwDb, UINT8 readerNum, SEQ_MNGRG_streamReadCfgT *cfg )
{
   ERRG_codeE     ret = HW_MNGR__RET_SUCCESS;
   UINT32         intStrm;
   static UINT32  intStrmToNumBitsTbl[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_READERS_E, readerNum, META_READERS_RD_0_OUT_RES_WIDTH_E),              &cfg->width);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_READERS_E, readerNum, META_READERS_RD_0_OUT_RES_HEIGHT_E),             &cfg->height);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_READERS_E, readerNum, META_READERS_RD_0_OUT_RES_BPP_E),                &cfg->bpp);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_READERS_E, readerNum, META_READERS_RD_0_OUT_RES_X_START_E),            &cfg->xStart);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_READERS_E, readerNum, META_READERS_RD_0_OUT_RES_Y_START_E),            &cfg->yStart);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_READERS_E, readerNum, META_READERS_RD_0_NUMOFBUFFERS_E),               &cfg->numOfBuffers);
   //TODO REMOVE OVERRIDE BEFORE MERGE
   cfg->numOfBuffers = 20;
   ret = XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_READERS_E, readerNum, META_READERS_RD_0_OUT_RES_STRIDE_E),       &cfg->stride);
   if(ret == XMLDB__EMPTY_ENTRY) {cfg->stride = 0;}
   ret = XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_READERS_E, readerNum, META_READERS_RD_0_OUT_RES_BUFFERHEIGHT_E), &cfg->bufferHeght);
   if(ret == XMLDB__EMPTY_ENTRY) {cfg->bufferHeght = 0;}
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_AXIRD_E,        readerNum, NU4100_PPE_AXI_READ0_CTRL_INT_STRM_EN_E),      &intStrm);
   cfg->numImages = intStrmToNumBitsTbl[intStrm];
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_READERS_E, readerNum, META_READERS_RD_0_NUMLINESPERCHUNK_E),           &cfg->numLinesPerChunk);

   //printf("w-%d, h-%d, b-%d, x-%d, y-%d stride %d buff h %d\n", cfg->width, cfg->height, cfg->bpp, cfg->xStart, cfg->yStart, cfg->stride, cfg->bufferHeght);
   //printf("SEQ_MNGRP_getDmaChannelDimentions: set frames line to %d. height = %d, bufferHeght = %d\n",cfg->numLinesPerChunk,cfg->height,cfg->bufferHeght);

   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_getDmaWriterDimentions
*
*  Description:   Access to XML DB and get reader configuration
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
static ERRG_codeE SEQ_MNGRP_getDmaWriterDimentions( XMLDB_dbH hwDb, UINT8 writerNum, SEQ_MNGRG_streamReadCfgT *cfg, UINT32 numWriters, UINT8 injectInterleaveImage )
{
   ERRG_codeE     ret = HW_MNGR__RET_SUCCESS;
   UINT32 strideTmp;
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_AXIWR_E, writerNum, NU4100_PPE_AXI_WRITE0_SIZE_VSIZE_E ),            &cfg->height );
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_AXIWR_E, writerNum,NU4100_PPE_AXI_WRITE0_SIZE_HSIZE_E  ),           &cfg->width);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_AXIWR_E, writerNum, NU4100_PPE_AXI_WRITE0_CTRL_PXL_WIDTH_E),         &cfg->bpp);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_AXIWR_E, writerNum, NU4100_PPE_AXI_WRITE0_CTRL_CIIF_NUM_E),         &cfg->numImages);
   cfg->numImages++; //CIIF num field defines if the injection should be split into two streams. this is also how we know if the image is interleaved or not

   if (((cfg->numImages == 1) && (numWriters > 1) && (injectInterleaveImage == 1)) || (cfg->numImages == 2))
   {
      ret = XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_WRITERS_E, writerNum, META_WRITERS_WT_0_INPUT_RES_STRIDE_E), &strideTmp);
      if ((ret == HW_MNGR__RET_SUCCESS))
      {
         cfg->stride = strideTmp;
      }
      else
      {
         cfg->stride = (cfg->width + 1) * 2;
      }
      //printf ("SEQ_MNGRP_getDmaWriterDimentions cfg->stride %d numImages%d numWriters%d\n",cfg->stride,cfg->numImages,numWriters);
   }
   else
   {
      cfg->stride = cfg->width + 1;
      //printf ("SEQ_MNGRP_getDmaWriterDimentions cfg->stride %d numImages%d\n",cfg->stride,cfg->numImages);
   }

   switch(cfg->bpp)        // Convert from reg enumeration to bytes per pixel
   {
      case 0:
         cfg->bpp = 1;
      break;
      case 2:
         cfg->bpp = 2;
      break;
      case 3:
         cfg->bpp = 3;
      break;
      default:
         cfg->bpp = 0;
      break;
   }
   //printf ("SEQ_MNGRP_getDmaWriterDimentions cfg->width %d cfg->height %d cfg->bpp %d\n",cfg->width, cfg->height, cfg->bpp);

   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_getIspFuncEnables
*
*  Description:   Read all enables for ISP functions
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
static ERRG_codeE SEQ_MNGRP_getIspNumFramesToSkip( XMLDB_dbH hwDb, UINT8 ispNum, ISP_MNGRG_streamReadCfgT* cfg )
{
   ERRG_codeE     ret = XMLDB__RET_SUCCESS;
   UINT32 numSkip = 0;

   if(ispNum < 3)
   {
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_NUM_FRAMES_TO_SKIP_E, &cfg->numFramesToSkip );
      if(ret == XMLDB__EMPTY_ENTRY)
      {
         cfg->numFramesToSkip        = 0;
      }

   }
   else
   {
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_NUM_FRAMES_TO_SKIP_E, &cfg->numFramesToSkip );
      if(ret == XMLDB__EMPTY_ENTRY)
      {
         cfg->numFramesToSkip        = 0;
      }
   }
   LOGG_PRINT(LOG_DEBUG_E, NULL, "num frames to skip for isp num %d is %d\n",ispNum, numSkip);

   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_getIspFuncEnables
*
*  Description:   Read all enables for ISP functions
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
static ERRG_codeE SEQ_MNGRP_getIspFuncEnables( XMLDB_dbH hwDb, UINT8 ispNum, ISP_MNGRG_streamReadCfgT* cfg )
{
   ERRG_codeE     ret = XMLDB__RET_SUCCESS;

   if(ispNum < 3)
   {
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_GC_E		,	&cfg->f.gc		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.gc        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_LSC_E		,	&cfg->f.lsc 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.lsc       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_CAC_E		,	&cfg->f.cac 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.cac       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_DPF_E		,	&cfg->f.dpf 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.dpf       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_DPCC_E 	,	&cfg->f.dpcc     );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.dpcc      = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_CNR_E		,	&cfg->f.cnr 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.cnr       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_EE_E		,	&cfg->f.ee		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.ee        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_DCI_E		,	&cfg->f.dci 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.dci       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_GE_E		,	&cfg->f.ge		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.ge        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_CPROC_E	,	&cfg->f.cproc	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.cproc     = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_CA_E		,	&cfg->f.ca		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.ca        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_BLS_E		,	&cfg->f.bls 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.bls       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_DEMOSAIC_E  ,	&cfg->f.demosaic );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.demosaic  = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_IE_E		,	&cfg->f.ie		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.ie        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_FUNC_2DNR_E ,	&cfg->f.func_2dnr);
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.func_2dnr = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_FUNC_3DNR_E ,	&cfg->f.func_3dnr);
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.func_3dnr = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_AE_E		,	&cfg->f.ae		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.ae        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_AF_E		,	&cfg->f.af		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.af        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_AWB_E		,	&cfg->f.awb 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.awb       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_AVS_E		,	&cfg->f.avs 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.avs       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_WDR_E		,	&cfg->f.wdr 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.avs       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_FUNCTIONALITY_HDR_E		,	&cfg->f.hdr 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.hdr       = 0;}
   }
   else
   {
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_GC_E		,	&cfg->f.gc		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.gc        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_LSC_E		,	&cfg->f.lsc 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.lsc       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_CAC_E		,	&cfg->f.cac 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.cac       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_DPF_E		,	&cfg->f.dpf 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.dpf       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_DPCC_E 	,	&cfg->f.dpcc     );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.dpcc      = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_CNR_E		,	&cfg->f.cnr 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.cnr       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_EE_E		,	&cfg->f.ee		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.ee        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_DCI_E		,	&cfg->f.dci 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.dci       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_GE_E		,	&cfg->f.ge		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.ge        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_CPROC_E	,	&cfg->f.cproc	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.cproc     = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_CA_E		,	&cfg->f.ca		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.ca        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_BLS_E		,	&cfg->f.bls 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.bls       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_DEMOSAIC_E  ,	&cfg->f.demosaic );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.demosaic  = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_IE_E		,	&cfg->f.ie		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.ie        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_FUNC_2DNR_E ,	&cfg->f.func_2dnr);
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.func_2dnr = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_FUNC_3DNR_E ,	&cfg->f.func_3dnr);
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.func_3dnr = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_AE_E		,	&cfg->f.ae		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.ae        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_AF_E		,	&cfg->f.af		 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.af        = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_AWB_E		,	&cfg->f.awb 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.awb       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_AVS_E		,	&cfg->f.avs 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.avs       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_WDR_E		,	&cfg->f.wdr 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.avs       = 0;}
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_FUNCTIONALITY_HDR_E		,	&cfg->f.hdr 	 );
      if(ret == XMLDB__EMPTY_ENTRY) {cfg->f.hdr       = 0;}
   }

   LOGG_PRINT(LOG_DEBUG_E, NULL, "SEQ_MNGRP_getIspFuncEnables Done\n");

   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_getIspNumChannelsActivation
*
*  Description:
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
static ERRG_codeE SEQ_MNGRP_getIspNumChannelsActivation( XMLDB_dbH hwDb, UINT8 ispNum, UINT32* chCtr )
{
   ERRG_codeE     ret = HW_MNGR__RET_SUCCESS;

   UINT32 tmpEn;
   //printf("SEQ_MNGRP_getIspNumChannelsActivation ispNum %d\n", ispNum);
   (*chCtr) = 0;

   if (ispNum)
   {
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_OUTPUTS_MP_ENABLE_E, &tmpEn);
      printf("isp1_mp_en %d cnt %d ret %d\n",tmpEn,*chCtr, ret);
      if ((ret == XMLDB__RET_SUCCESS) && (tmpEn))
      {
         (*chCtr)++;
      }
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_OUTPUTS_SP1_ENABLE_E, &tmpEn);
      printf("isp1_sp1_en %d cnt %d ret %d\n",tmpEn,*chCtr, ret);
      if ((ret == XMLDB__RET_SUCCESS) && (tmpEn))
      {
         (*chCtr)++;
      }
      ret = XMLDB_getValue( hwDb, ISPS_ISP1_OUTPUTS_SP2_ENABLE_E, &tmpEn);
      printf("isp1_sp2_en %d cnt %d ret %d\n",tmpEn,*chCtr, ret);
      if ((ret == XMLDB__RET_SUCCESS) && (tmpEn))
      {
         (*chCtr)++;
      }
   }
   else
   {
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_OUTPUTS_MP_ENABLE_E, &tmpEn);
      printf("isp0_mp_en %d cnt %d ret %d\n",tmpEn,*chCtr, ret);
      if ((ret == XMLDB__RET_SUCCESS) && (tmpEn))
      {
         (*chCtr)++;
      }
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_OUTPUTS_SP1_ENABLE_E, &tmpEn);
      printf("isp0_sp1_en %d cnt %d ret %d\n",tmpEn,*chCtr, ret);
      if ((ret == XMLDB__RET_SUCCESS) && (tmpEn))
      {
         (*chCtr)++;
      }
      ret = XMLDB_getValue( hwDb, ISPS_ISP0_OUTPUTS_SP2_ENABLE_E, &tmpEn);
      printf("isp0_sp2_en %d cnt %d ret %d\n",tmpEn,*chCtr, ret);
      if ((ret == XMLDB__RET_SUCCESS) && (tmpEn))
      {
         (*chCtr)++;
      }
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setConfigDma
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
static ERRG_codeE SEQ_MNGRP_setConfigDma( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;                                 // TODO: build error codes for sequencer
   CDE_MNGRG_channnelInfoT   *reader = NULL;
   CDE_DRVG_channelCfgT       readerConfig;

   reader = dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RDOUT_E].blockInfo[blkInstance].blockPrivData;
   memset(&readerConfig,0,sizeof(readerConfig));
   readerConfig.srcCfg.peripheral                  = ppeAxiBuffers[blkInstance];
   readerConfig.dstCfg.peripheral                  = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;
   if( reader == NULL )
   {
      ret = CDE_MNGRG_openChannel( &reader, readerConfig.srcCfg.peripheral, readerConfig.dstCfg.peripheral );
      if(ERRG_FAILED(ret))
      {
         return ret;
      }
   }
   else
   {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      LOGG_PRINT(LOG_INFO_E, NULL, "SEQ_MNGRP_setConfigDma streaming from reader %d already configured on DMA reader %p (%d/%d)\n", blkInstance, (UINT32*)reader, reader->dmaChannelHandle->coreNum, reader->dmaChannelHandle->hwChannelNum);
#else
   LOGG_PRINT(LOG_INFO_E, NULL, "SEQ_MNGRP_setConfigDma streaming from reader %d already configured on DMA reader %p (%d/%d)\n", blkInstance, (UINT32*)reader, reader->dmaChannelHandle->coreNum, reader->dmaChannelHandle->channelNum);
#endif

   }
   *(UINT32*)arg = (UINT32)reader; // register dma handle to the path

   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableDma
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
static ERRG_codeE SEQ_MNGRP_setEnableDma( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE            ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   int blockI=0;
   static bool runOnce = false;
   UINT32 frrVecSize=0,frrInitGo=0;
   CDE_MNGRG_channnelInfoT*     reader;
#ifdef CDE_DRVG_METADATA
   CDE_DRVG_regToMemoryCopyConfigParams configParams;
   memset(&configParams,0,sizeof(configParams));
#endif
   UINT8 dmaChId = *((UINT8*)arg);
   CDE_DRVG_channelCfgT    readerConfig;
   CDE_MNGRG_chanCbInfoT     readerCbCfg;
   SEQ_MNGRG_streamReadCfgT   readerCfg;

   memset(&readerConfig,0,sizeof(readerConfig));
   memset(&readerCbCfg,0,sizeof(readerCbCfg));
   LOGG_PRINT(LOG_INFO_E, 0, "##### 0 ##### \n");
   if (block == NUFLD_META_READERS_E)
   {
     blockI=SEQ_MNGRG_XML_BLOCK_RDOUT_E;
   }

   if(( dbP->path[dmaChId].status == SEQ_MNGRG_STATUS_TYPE_CONFIGURED_E ) && (instanceList[0] != 0xFF))
   {
      reader = dbP->blockVec[blockI].blockInfo[dbP->path[dmaChId].block[blockI]].blockPrivData;

      if( reader->channelStatus < CDE_MNGRG_CHAN_STATUS_CONFIGURED_E)
      {
         SEQ_MNGRP_getDmaChannelDimentions( dbP->dbH, instanceList[0], &readerCfg );

         readerConfig.streamCfg.scenarioType           = CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_E;
         readerConfig.streamCfg.opMode                 = CDE_DRVG_CONTINIOUS_OP_MODE_E;
         readerConfig.streamCfg.frameMode              = CDE_DRVG_FRAME_MODE_NONE_INTERLEAVE_E;
         readerConfig.streamCfg.transferMode           = CDE_DRVG_1D_TRANSFER_MODE_E;                    // NOT implemented
         readerConfig.streamCfg.resolutionType         = CDE_DRVG_RESOLUTION_TYPE_FULL_E;                 // NOT implemented
         readerConfig.streamCfg.numOfBuffs             = readerCfg.numOfBuffers;

         readerConfig.srcCfg.peripheral               = ppeAxiBuffers[instanceList[0]];

         readerConfig.dstCfg.peripheral               = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;
         readerConfig.dstCfg.dimensionCfg.bufferHeight      = ( (readerCfg.bufferHeght == 0) ? (readerCfg.height):(readerCfg.bufferHeght) );
         readerConfig.dstCfg.dimensionCfg.bufferWidth       = ( (readerCfg.stride== 0) ? (readerCfg.width):(readerCfg.stride) );
         readerConfig.dstCfg.dimensionCfg.imageDim.imageWidth  = readerCfg.width;
         readerConfig.dstCfg.dimensionCfg.imageDim.imageHeight = readerCfg.height;
         readerConfig.dstCfg.dimensionCfg.imageDim.pixelSize = (readerCfg.bpp >> 3);                     // pixel size in bytes
         readerConfig.dstCfg.dimensionCfg.frameStartX   = readerCfg.xStart;
         readerConfig.dstCfg.dimensionCfg.frameStartY   = readerCfg.yStart;
         readerConfig.dstCfg.dimensionCfg.numLinesPerChunk = readerCfg.numLinesPerChunk;

         readerCbCfg.frameDoneCb.cb =  dbP->path[dmaChId].channelCb;
         readerCbCfg.frameDoneCb.arg = dbP->path[dmaChId].cbArgs;
         CDE_MNGRG_setSysId( reader, (INU_DEFSG_sysChannelIdE)instanceList[0]);              // bind readerID to dma channel
         UINT32 readerID = reader->systemChanId;
#ifdef CDE_DRVG_METADATA
         const INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
         LOGG_PRINT(LOG_INFO_E, NULL, "Updating the reader and give Event ID for dmaChID %lu,readerID %lu\n",dmaChId,readerID);
         /*The give event and wait event IDs have to be set for the dmaChannelHandle 
            and this will depend on the settings within metadata_target_config.c */
         METADATA_TARGET_CONFIG_updateReaderGiveEventAndWaitIDsByReaderID(modelType,readerID,reader);
         /*Update the number of metadata lines for this channel */
         METADATA_TARGET_CONFIG_updateNumberMetadataLinesByReaderID(modelType,readerID,&readerConfig);
#endif
         if (reader->extInfo.useExtIntMode)
         {
            CDE_MNGRG_fixExtIntAxiRdCfg(reader, &readerConfig);
            CDE_MNGRG_allocExtIntSubChannels(reader);
         }

         ret = CDE_MNGRG_setupChannel( reader, &readerConfig, &readerCbCfg );
         if(ERRG_FAILED(ret))
         {
            return ret;
         }
         XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_AXIRD_E, instanceList[0], (NU4100_PPE_AXI_READ0_FRR_INIT_GO_E)), &frrInitGo);
         if (frrInitGo != 0)
         {
            XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_AXIRD_E, instanceList[0], (NU4100_PPE_AXI_READ0_FRR_CFG_VEC_SIZE_E)), &frrVecSize);
            frrVecSize += 1;
         }
         CDE_MNGRG_startChannel(reader);
#ifndef CDE_DRVG_VIRTUAL_CHANNELS
         //register event per each chunk
         HCG_MNGRG_registerHwUnitDownEvent(voterHandle, HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (reader->dmaChannelHandle->coreNum * 8) + reader->dmaChannelHandle->channelNum, 
                                        (readerConfig.dstCfg.dimensionCfg.bufferHeight / readerConfig.dstCfg.dimensionCfg.numLinesPerChunk),frrVecSize);
         HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_DMA0 + reader->dmaChannelHandle->coreNum);
         HCG_MNGRG_registerEndEvent(voterHandle, HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (reader->dmaChannelHandle->coreNum * 8) + reader->dmaChannelHandle->channelNum);
#endif
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      //register event per each chunk
      HCG_MNGRG_registerHwUnitDownEvent(voterHandle, HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (reader->dmaChannelHandle->coreNum * 8) + reader->dmaChannelHandle->hwChannelNum,
                                     (readerConfig.dstCfg.dimensionCfg.bufferHeight / readerConfig.dstCfg.dimensionCfg.numLinesPerChunk),frrVecSize);
      HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_DMA0 + reader->dmaChannelHandle->coreNum);
      HCG_MNGRG_registerEndEvent(voterHandle, HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (reader->dmaChannelHandle->coreNum * 8) + reader->dmaChannelHandle->hwChannelNum);
#endif
   }

#ifdef CDE_DRVG_METADATA
   UINT32 phyAddress = 0;
   const INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
   UINT32 readerID = reader->systemChanId;
   LOGG_PRINT(LOG_INFO_E, NULL, "Opening metadata channel for reader ID %lu \n", readerID);
   ERRG_codeE configFound = METADATA_TARGET_CONFIG_getConfigParamsByReaderID(&configParams,modelType,readerID,reader);
   if(ERRG_SUCCEEDED(configFound))
   {
      CDE_MNGRG_metadataOpenChan(&reader, &configParams);     
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Registering DMA Channel handle %lu with DMA Channel Handle 0x%x, Metadata channel handle 0x%x, Reader ID %lu \n", dmaChId,reader->dmaChannelHandle,reader->metadataDMAChannelHandle, readerID);
      METADATA_UPDATER_storeDMAChannelPointerWithReaderID(readerID,modelType,reader);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "No configuration found for channelID: %lu, reader ID %lu \n",dmaChId,readerID);
   }

#endif
   }

   PPE_MNGRG_setEnableAxiRd(dbP->dbH,block,instanceList,arg,(void *)voterHandle);


   return ret;
}



/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableDma
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
static ERRG_codeE SEQ_MNGRP_setDisableDma( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;                                 // TODO: build error codes for sequencer
   UINT32 i;
   CDE_MNGRG_channnelInfoT *readerH=NULL;
   LOGG_PRINT(LOG_INFO_E, 0, "##### Disabling DMAs ##### \n");

   for(i = 0; i < NU4K_MAX_DB_META_PATHS; i++)
   {
      if (instanceList[i] != 0xFF)
      {
         readerH = dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RDOUT_E].blockInfo[instanceList[i]].blockPrivData;
         if( readerH != NULL )
         {
            ret = CDE_MNGRG_stopChannel(readerH);
            CDE_MNGRG_closeChannel(readerH);
            dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RDOUT_E].blockInfo[instanceList[i]].blockPrivData = NULL;
         }
      }
   }

   PPE_MNGRG_setDisableAxiRd(dbP->dbH,block,instanceList,arg);
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setConfigCvaDma
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
static ERRG_codeE SEQ_MNGRP_setConfigCvaDma( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;                                 // TODO: build error codes for sequencer
   CDE_MNGRG_channnelInfoT*   iicReader = NULL;
   CDE_DRVG_channelCfgT       iicReaderConfig;
   CDE_MNGRG_channnelInfoT*   freakReader = NULL;
   CDE_DRVG_channelCfgT       freakReaderConfig;
   CDE_MNGRG_channnelInfoT*   controlReader = NULL;
   CDE_DRVG_channelCfgT       controlReaderConfig;

   (void)block;
   memset(&iicReaderConfig,0,sizeof(CDE_DRVG_channelCfgT));
   memset(&freakReaderConfig,0,sizeof(CDE_DRVG_channelCfgT));
   memset(&controlReaderConfig,0,sizeof(CDE_DRVG_channelCfgT));

   //IIC DMA configuration
   iicReaderConfig.srcCfg.peripheral                  = CDE_DRVG_PERIPH_CLIENT_CVA4_IMG_TX_E;
   iicReaderConfig.dstCfg.peripheral                  = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;
   CDE_MNGRG_isPeriphInUse(&iicReader, iicReaderConfig.srcCfg.peripheral, CDE_MNGRG_PORT_SOURCE_E);

   if( iicReader == NULL )
   {
      ret = CDE_MNGRG_openChannel( &iicReader, iicReaderConfig.srcCfg.peripheral, iicReaderConfig.dstCfg.peripheral );
      if(ERRG_FAILED(ret))
      {
         return ret;
      }
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "SEQ_MNGRP_setConfigDma streaming from reader %d already configured on DMA reader %p   \n", blkInstance, (UINT32*)iicReader );
   }

   //Freak DMA configuration
   freakReaderConfig.srcCfg.peripheral                  = CDE_DRVG_PERIPH_CLIENT_CVA5_KP_DESC_TX_E;
   freakReaderConfig.dstCfg.peripheral                  = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;
   CDE_MNGRG_isPeriphInUse(&freakReader, freakReaderConfig.srcCfg.peripheral, CDE_MNGRG_PORT_SOURCE_E);

   if( freakReader == NULL )
   {
      ret = CDE_MNGRG_openChannel( &freakReader, freakReaderConfig.srcCfg.peripheral, freakReaderConfig.dstCfg.peripheral );
      if(ERRG_FAILED(ret))
      {
         return ret;
      }
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "SEQ_MNGRP_setConfigDma streaming from reader %d already configured on DMA reader %p   \n", blkInstance, (UINT32*)freakReader );
   }

   //IIC DMA configuration
   controlReaderConfig.srcCfg.peripheral                  = CDE_DRVG_PERIPH_CLIENT_CVA6_CTRL_TX_E;
   controlReaderConfig.dstCfg.peripheral                  = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;
   CDE_MNGRG_isPeriphInUse(&controlReader, controlReaderConfig.srcCfg.peripheral, CDE_MNGRG_PORT_SOURCE_E);

   if( controlReader == NULL )
   {
      ret = CDE_MNGRG_openChannel( &controlReader, controlReaderConfig.srcCfg.peripheral, controlReaderConfig.dstCfg.peripheral );
      if(ERRG_FAILED(ret))
      {
         return ret;
      }
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "SEQ_MNGRP_setConfigDma streaming from reader %d already configured on DMA reader %p   \n", blkInstance, (UINT32*)controlReader );
   }

   dbP->cvaReaders[0] = iicReader;
   dbP->cvaReaders[1] = freakReader;
   dbP->cvaReaders[2] = controlReader;

   *(UINT32*)arg = (UINT32)(&dbP->cvaReaders);       // register dma handle to the path


   LOGG_PRINT(LOG_INFO_E, NULL, "SEQ_MNGRP_setConfigCvaDma is configured\n");
   return ret;
}




/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableCvaDma
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
static ERRG_codeE SEQ_MNGRP_setEnableCvaDma( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle  )
{
   ERRG_codeE            ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   CDE_MNGRG_channnelInfoT*   iicReader = (CDE_MNGRG_channnelInfoT*)dbP->cvaReaders[0];
   CDE_DRVG_channelCfgT       iicReaderConfig;
   CDE_MNGRG_channnelInfoT*   freakReader = (CDE_MNGRG_channnelInfoT*)dbP->cvaReaders[1];
   CDE_DRVG_channelCfgT       freakReaderConfig;
   CDE_MNGRG_channnelInfoT*   controlReader = (CDE_MNGRG_channnelInfoT*)dbP->cvaReaders[2];
   CDE_DRVG_channelCfgT       controlReaderConfig;
   CDE_MNGRG_chanCbInfoT     iicReaderCbCfg;
   CDE_MNGRG_chanCbInfoT     freakReaderCbCfg;
   CDE_MNGRG_chanCbInfoT     controlReaderCbCfg;
#ifdef CDE_DRVG_METADATA
   CDE_DRVG_regToMemoryCopyConfigParams configParams;
   memset(&configParams,0,sizeof(configParams));
#endif

   UINT8 dmaChId = *((UINT8*)arg);
   volatile SEQ_MNGRG_streamCvaReadCfgT  readersCfg;
   (void)block;

   memset(&iicReaderConfig,0,sizeof(CDE_DRVG_channelCfgT));
   memset(&freakReaderConfig,0,sizeof(CDE_DRVG_channelCfgT));
   memset(&controlReaderConfig,0,sizeof(CDE_DRVG_channelCfgT));

   memset(&iicReaderCbCfg,0,sizeof(CDE_MNGRG_chanCbInfoT));
   memset(&freakReaderCbCfg,0,sizeof(CDE_MNGRG_chanCbInfoT));
   memset(&controlReaderCbCfg,0,sizeof(CDE_MNGRG_chanCbInfoT));

   LOGG_PRINT(LOG_INFO_E, 0, "##### EnableCvaDma ##### %d\n",instanceList[0]);

   SEQ_MNGRP_getDmaCvaChannelDimentions( dbP->dbH, instanceList[0], (SEQ_MNGRG_streamCvaReadCfgT*)&readersCfg );

   if(( dbP->path[dmaChId].status == SEQ_MNGRG_STATUS_TYPE_CONFIGURED_E ) && (instanceList[0] != 0xFF))
   {
      MEM_MAPG_addrT    memVirtAddr;
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GPV_E, (&memVirtAddr));

      GPV_DRVP_deviceDesc.deviceBaseAddress = (UINT32)memVirtAddr;
      /*
               There is a bug in the AXI interface in the PPE.
               The svalid from the CVA buffers have different pipeline than the PPE buffers.
               Therefore there is collision when toggling between them, and data is lost.
               To prevent this, we modify the DMA 2 (TODO - modify DMA core according to the CVA matrix) FN MOD.
           */
      GPV_DMA_2_FN_MOD_FN_MOD_W(1);

      if( readersCfg.isDdrless == 1)
      {
         iicReaderConfig.streamCfg.scenarioType            = CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS;//CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_E;
      }
      else
      {
         iicReaderConfig.streamCfg.scenarioType            = CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_E;
      }
      iicReaderConfig.streamCfg.opMode                      = CDE_DRVG_CONTINIOUS_OP_MODE_E;
      iicReaderConfig.streamCfg.frameMode                   = CDE_DRVG_FRAME_MODE_NONE_INTERLEAVE_E;
      iicReaderConfig.streamCfg.transferMode                = CDE_DRVG_1D_TRANSFER_MODE_E;                    // NOT implemented
      iicReaderConfig.streamCfg.resolutionType              = CDE_DRVG_RESOLUTION_TYPE_FULL_E;                 // NOT implemented
      iicReaderConfig.streamCfg.numOfBuffs                  = 20;

      iicReaderConfig.srcCfg.peripheral                     = CDE_DRVG_PERIPH_CLIENT_CVA4_IMG_TX_E;
      iicReaderConfig.dstCfg.peripheral                     = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;

      if( readersCfg.isDdrless == 1)
      {
        iicReaderConfig.dstCfg.dimensionCfg.bufferSize      = readersCfg.buffSize;
      }
      iicReaderConfig.dstCfg.dimensionCfg.bufferHeight      = (readersCfg.height + 1);
      iicReaderConfig.dstCfg.dimensionCfg.numLinesPerChunk  = iicReaderConfig.dstCfg.dimensionCfg.bufferHeight; //iic cannot be in chunk mode
      iicReaderConfig.dstCfg.dimensionCfg.bufferWidth       = (readersCfg.width + 1);
      iicReaderConfig.dstCfg.dimensionCfg.imageDim.imageWidth = iicReaderConfig.dstCfg.dimensionCfg.bufferWidth;
      iicReaderConfig.dstCfg.dimensionCfg.imageDim.pixelSize = 4;          //IIC pixel is always 4 bytes
      iicReaderConfig.dstCfg.dimensionCfg.frameStartX       = 0;              //2        NOTE: integral image have 1 row of zeros
      iicReaderConfig.dstCfg.dimensionCfg.frameStartY       = 1;

      iicReaderCbCfg.frameDoneCb.cb =  CVA_MNGRG_cvaIicCb;
      iicReaderCbCfg.frameDoneCb.arg = dbP->path[dmaChId].cbArgs;//NULL;

      freakReaderConfig.streamCfg.scenarioType              = CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_E;
      freakReaderConfig.streamCfg.opMode                    = CDE_DRVG_CONTINIOUS_OP_MODE_E;
      freakReaderConfig.streamCfg.frameMode                 = CDE_DRVG_FRAME_MODE_NONE_INTERLEAVE_E;
      freakReaderConfig.streamCfg.transferMode              = CDE_DRVG_1D_TRANSFER_MODE_E;                    // NOT implemented
      freakReaderConfig.streamCfg.resolutionType            = CDE_DRVG_RESOLUTION_TYPE_FULL_E;                 // NOT implemented
      freakReaderConfig.streamCfg.numOfBuffs                = 20;

      freakReaderConfig.srcCfg.peripheral                   = CDE_DRVG_PERIPH_CLIENT_CVA5_KP_DESC_TX_E;
      freakReaderConfig.dstCfg.peripheral                   = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;
      freakReaderConfig.dstCfg.dimensionCfg.bufferHeight       = readersCfg.max_kp;
      freakReaderConfig.dstCfg.dimensionCfg.numLinesPerChunk   = freakReaderConfig.dstCfg.dimensionCfg.bufferHeight;
      freakReaderConfig.dstCfg.dimensionCfg.bufferWidth        = (readersCfg.bpp >> 3);
      freakReaderConfig.dstCfg.dimensionCfg.imageDim.imageWidth = freakReaderConfig.dstCfg.dimensionCfg.bufferWidth;
      freakReaderConfig.dstCfg.dimensionCfg.imageDim.pixelSize = 1;

      freakReaderCbCfg.frameDoneCb.cb =  dbP->path[dmaChId].channelCb;
      freakReaderCbCfg.frameDoneCb.arg = dbP->path[dmaChId].cbArgs;


      controlReaderConfig.streamCfg.scenarioType               = CDE_DRVG_SCENARIO_TYPE_STREAM_CYCLIC_E;
      controlReaderConfig.streamCfg.opMode                     = CDE_DRVG_CONTINIOUS_OP_MODE_E;
      controlReaderConfig.streamCfg.frameMode                  = CDE_DRVG_FRAME_MODE_NONE_INTERLEAVE_E;
      controlReaderConfig.streamCfg.transferMode               = CDE_DRVG_1D_TRANSFER_MODE_E;                    // NOT implemented
      controlReaderConfig.streamCfg.resolutionType             = CDE_DRVG_RESOLUTION_TYPE_FULL_E;                 // NOT implemented
      controlReaderConfig.streamCfg.numOfBuffs                 = 20;

      controlReaderConfig.srcCfg.peripheral                      = CDE_DRVG_PERIPH_CLIENT_CVA6_CTRL_TX_E;
      controlReaderConfig.dstCfg.peripheral                      = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;
      controlReaderConfig.dstCfg.dimensionCfg.bufferHeight       = 24;
      controlReaderConfig.dstCfg.dimensionCfg.numLinesPerChunk   = 24;
      controlReaderConfig.dstCfg.dimensionCfg.bufferWidth        = 16;
      controlReaderConfig.dstCfg.dimensionCfg.imageDim.pixelSize = 1;

      controlReaderCbCfg.frameDoneCb.cb =  CVA_MNGRG_controlFrameDoneCb;
      controlReaderCbCfg.frameDoneCb.arg = NULL;


      CDE_MNGRG_setSysId( iicReader, 15);          // bind readerID to dma channel
      CDE_MNGRG_setSysId( freakReader, 16);        // bind readerID to dma channel
      CDE_MNGRG_setSysId( controlReader, 17);      // bind readerID to dma channel
      const UINT32 readerID = freakReader->systemChanId;
#ifdef CDE_DRVG_CVA_METADATA
         const INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
         LOGG_PRINT(LOG_INFO_E, NULL, "CVA:Updating the reader and give Event ID for dmaChID, reader ID %lu\n",dmaChId, readerID);
         /*The give event and wait event IDs have to be set for the dmaChannelHandle 
            and this will depend on the settings within metadata_target_config.c */
         METADATA_TARGET_CONFIG_updateReaderGiveEventAndWaitIDsByReaderID(modelType,readerID,freakReader);
         /*Update the number of metadata lines for this channel */
         METADATA_TARGET_CONFIG_updateNumberMetadataLinesByReaderID(modelType,readerID,&freakReaderConfig);
#endif

      CDE_MNGRG_setupChannel( iicReader, &iicReaderConfig, &iicReaderCbCfg );
      CDE_MNGRG_setupChannel( freakReader, &freakReaderConfig, &freakReaderCbCfg );
      CDE_MNGRG_setupChannel( controlReader, &controlReaderConfig, &controlReaderCbCfg );
      CVA_MNGRG_setControlReader((void *)controlReader);
#ifndef CDE_DRVG_VIRTUAL_CHANNELS
      HCG_MNGRG_registerHwUnitDownEvent(voterHandle, HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (freakReader->dmaChannelHandle->coreNum * 8) + freakReader->dmaChannelHandle->channelNum, 1, 0);
      HCG_MNGRG_registerEndEvent(voterHandle, HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (freakReader->dmaChannelHandle->coreNum * 8) + freakReader->dmaChannelHandle->channelNum);
#endif
      HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_DMA0 + iicReader->dmaChannelHandle->coreNum);
      HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_DMA0 + freakReader->dmaChannelHandle->coreNum);
      HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_DMA0 + controlReader->dmaChannelHandle->coreNum);
      HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_CVA);

      if(ERRG_FAILED(ret))
      {
         return ret;
      }

      CDE_MNGRG_startChannel(iicReader);
      CDE_MNGRG_startChannel(freakReader);

#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      HCG_MNGRG_registerHwUnitDownEvent(voterHandle, HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (freakReader->dmaChannelHandle->coreNum * 8) + freakReader->dmaChannelHandle->hwChannelNum, 1, 0);
      HCG_MNGRG_registerEndEvent(voterHandle, HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (freakReader->dmaChannelHandle->coreNum * 8) + freakReader->dmaChannelHandle->hwChannelNum);
#endif
      CDE_MNGRG_startChannel(controlReader);

#ifdef CDE_DRVG_CVA_METADATA
   UINT32 phyAddress = 0;
   const INU_DEFSG_moduleTypeE modelType_2 = GME_MNGRG_getModelType();
   LOGG_PRINT(LOG_INFO_E, NULL, "Opening CVA metadata channel for Reader ID %lu \n", readerID);
   ERRG_codeE configFound = METADATA_TARGET_CONFIG_getConfigParamsByReaderID(&configParams,modelType_2,readerID,freakReader);
   if(ERRG_SUCCEEDED(configFound))
   {
      CDE_MNGRG_metadataOpenChan(&freakReader, &configParams);     
      LOGG_PRINT(LOG_INFO_E,NULL,"Registering CVA DMA Channel handle %lu with DMA Channel Handle 0x%x, Metadata channel handle 0x%x \n", dmaChId,freakReader->dmaChannelHandle,freakReader->metadataDMAChannelHandle);
      METADATA_UPDATER_storeDMAChannelPointerWithReaderID(readerID,modelType_2,freakReader);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,"No configuration found for channelID %lu, reader ID %lu \n",dmaChId, readerID);
   }
#endif
  }

   LOGG_PRINT(LOG_INFO_E, 0, "##### EnableCvaDma Done ##### \n");
   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableCvaDma
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
static ERRG_codeE SEQ_MNGRP_setDisableCvaDma( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;                                 // TODO: build error codes for sequencer
   UINT32 i;
   LOGG_PRINT(LOG_INFO_E, 0, "##### Disabling DMAs ##### \n");
   (void)instanceList;(void)arg;(void)block;

   if((UINT32*)dbP->cvaReaders[0] != NULL)//it means the cva readers did not config yet (disable all at the begining)
   {
      for (i=0; i< NUM_OF_CVA_READERS;i++)
      {
         if(dbP->cvaReaders[i])
         {
           CDE_MNGRG_stopChannel((CDE_MNGRG_channnelInfoT*)dbP->cvaReaders[i]);
           CDE_MNGRG_closeChannel((CDE_MNGRG_channnelInfoT*)dbP->cvaReaders[i]);
           dbP->cvaReaders[i] = 0;
        }
      }
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableCVA
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
static ERRG_codeE SEQ_MNGRP_setDisableCVA( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;
   (void)instanceList;(void)arg;(void)block;
   CVA_MNGRG_setDisableCva();
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableCVA
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
static ERRG_codeE SEQ_MNGRP_setEnableCVA( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle  )
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;
   (void)instanceList;(void)arg;(void)block;(void)voterHandle;
   CVA_MNGRG_setEnableCva();
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_getNumOfWritersOnList
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
static UINT32 SEQ_MNGRP_getNumOfWritersOnList(UINT8* instanceList)
{
   UINT32 i, cnt = 0;

   for ( i = 0; i < NU4K_MAX_DB_META_PATHS; i++)
   {
      if (instanceList[i] != 0xFF)
      {
         cnt++;
      }

   }

   return cnt;
}

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
static ERRG_codeE SEQ_MNGRP_setConfigWriterDma( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;      // TODO: build error codes for sequencer
   CDE_MNGRG_channnelInfoT*   writer = NULL;
   (void)block;

   writer = dbP->blockVec[SEQ_MNGRG_XML_BLOCK_INJECTION_E].blockInfo[blkInstance].blockPrivData;
   if (!writer)
   {
      ret = CDE_MNGRG_openChannel( &writer, CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E, ppeAxiWriterBuffers[blkInstance] );
      if(ERRG_FAILED(ret))
      {
         return ret;
      }
   }
   *(UINT32*)arg = (UINT32)writer;       // register dma handle to the path
   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableWriterDma
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
static ERRG_codeE SEQ_MNGRP_setEnableWriterDma( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE            ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   int blockI=0;
   CDE_MNGRG_channnelInfoT*   writer;
   UINT8 dmaChId= *((UINT8*)arg);
   CDE_DRVG_channelCfgT       writerConfig;
   CDE_MNGRG_chanCbInfoT      writerCbCfg;
   SEQ_MNGRG_streamReadCfgT   writerCfg;
   UINT32 i, roleVal, startY, startX, totalHeight, totalWidth, widthRunner = 0;
   UINT32 lineWidth, burstLenInd, numOfLoopsMod = 0, tempBurstLen = 0, stride = 0;

   memset(&writerConfig,0,sizeof(writerConfig));
   memset(&writerCbCfg,0,sizeof(writerCbCfg));

   writerConfig.streamCfg.scenarioType          = CDE_DRVG_SCENARIO_TYPE_INJECTION_FIXEDSIZE_E;
   writerConfig.streamCfg.opMode                = CDE_DRVG_OME_SHOT_OP_MODE_E;
   writerConfig.streamCfg.frameMode             = CDE_DRVG_FRAME_MODE_NONE_INTERLEAVE_E;           // NOT implemented
   writerConfig.streamCfg.transferMode          = CDE_DRVG_1D_TRANSFER_MODE_E;                    // NOT implemented
   writerConfig.streamCfg.resolutionType        = CDE_DRVG_RESOLUTION_TYPE_FULL_E;                 // NOT implemented
   writerConfig.streamCfg.numOfBuffs            = 1;                      // TODO: should be parameter
   writerConfig.srcCfg.peripheral               = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;

   UINT32 numWriters = SEQ_MNGRP_getNumOfWritersOnList(instanceList);
   for ( i = 0; i < NU4K_MAX_DB_META_PATHS; i++)
   {
      if (instanceList[i] != 0xFF)
      {
         PPE_MNGRG_setEnableAxiWr(dbP->dbH,block,instanceList[i],arg,voterHandle);
         if (block == NUFLD_AXIWR_E)
         {
           blockI=SEQ_MNGRG_XML_BLOCK_INJECTION_E;
         }
         writer = dbP->blockVec[blockI].blockInfo[instanceList[i]].blockPrivData;
         SEQ_MNGRP_getDmaWriterDimentions( dbP->dbH, instanceList[i], &writerCfg, numWriters, dbP->injectInterleaveImage);
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         //LOGG_PRINT(LOG_INFO_E, 0, "SEQ_MNGRP_setEnableWriterDma %d W-%d,H-%d, BPP-%d numImages %d core/ch %d/%d numWriters %d stride %d bufferHeight %d bufferWidth %d\n",
         //                        instanceList[i],writerCfg.width, writerCfg.height, writerCfg.bpp, writerCfg.numImages,writer->dmaChannelHandle->coreNum, writer->dmaChannelHandle->hwChannelNum, numWriters, writerCfg.stride,writerConfig.srcCfg.dimensionCfg.bufferHeight, writerConfig.srcCfg.dimensionCfg.bufferWidth );

#else
         //LOGG_PRINT(LOG_INFO_E, 0, "SEQ_MNGRP_setEnableWriterDma %d W-%d,H-%d, BPP-%d numImages %d core/ch %d/%d numWriters %d stride %d bufferHeight %d bufferWidth %d\n",
         //                        instanceList[i],writerCfg.width, writerCfg.height, writerCfg.bpp, writerCfg.numImages,writer->dmaChannelHandle->coreNum, writer->dmaChannelHandle->channelNum, numWriters, writerCfg.stride,writerConfig.srcCfg.dimensionCfg.bufferHeight, writerConfig.srcCfg.dimensionCfg.bufferWidth );
#endif

         writerConfig.dstCfg.peripheral                        = ppeAxiWriterBuffers[instanceList[i]];
         writerConfig.srcCfg.dimensionCfg.bufferHeight         = writerCfg.height + 1;
         writerConfig.srcCfg.dimensionCfg.numLinesPerChunk     = writerConfig.srcCfg.dimensionCfg.bufferHeight;
         writerConfig.srcCfg.dimensionCfg.bufferWidth          = writerCfg.numImages * (writerCfg.width + 1); //writerCfg.stride;
         writerConfig.srcCfg.dimensionCfg.stride               = writerCfg.stride;
         writerConfig.srcCfg.dimensionCfg.imageDim.pixelSize   = writerCfg.bpp; // pixel size in bytes
         writerConfig.srcCfg.dimensionCfg.imageDim.imageWidth  = writerCfg.width+1;
         writerConfig.srcCfg.dimensionCfg.imageDim.imageHeight = writerCfg.height+1;
         writerConfig.srcCfg.dimensionCfg.frameStartX = 0;

         //check if exist value in section sensor role under the writer. if exist, set to right side writer start x of width.
         ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_WRITERS_E, instanceList[i], (META_WRITERS_WT_0_SENSOR_ROLE_E)), &roleVal);
         if(ERRG_SUCCEEDED(ret))
         {
            //in case we have a given stride from calibration data, overwrite it from the calculated one in 'SEQ_MNGRP_getDmaWriterDimentions'
            ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_WRITERS_E, instanceList[i], (META_WRITERS_WT_0_INPUT_RES_STRIDE_E)), &stride);
            if(ERRG_SUCCEEDED(ret))
            {
               writerConfig.srcCfg.dimensionCfg.stride = stride;
            }

            if(roleVal == 0)//0=right
            {
               writerConfig.srcCfg.dimensionCfg.frameStartX = stride / 2;
            }
            ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_WRITERS_E, instanceList[i],META_WRITERS_WT_0_INPUT_RES_Y_START_E  ),&startY);
            if(ret == XMLDB__EMPTY_ENTRY) {startY = 0;}
            writerConfig.srcCfg.dimensionCfg.frameStartY = startY;
            ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_WRITERS_E, instanceList[i],META_WRITERS_WT_0_INPUT_RES_HEIGHT_E),&totalHeight);
            if(ret != XMLDB__EMPTY_ENTRY)
            {
               writerConfig.srcCfg.dimensionCfg.imageDim.imageHeight = totalHeight;
            }
         }
         else
         {
            if (dbP->injectInterleaveImage)
            {
               writerConfig.srcCfg.dimensionCfg.frameStartX = widthRunner;
               widthRunner += writerCfg.width+1;
            }
         }

         //add to startX Horizontal crop offset, if exist
         ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_WRITERS_E, instanceList[i],META_WRITERS_WT_0_INPUT_RES_X_START_E  ),&startX);
         if(ERRG_SUCCEEDED(ret))
         {
            writerConfig.srcCfg.dimensionCfg.frameStartX += startX;
         }
         ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_WRITERS_E, instanceList[i],META_WRITERS_WT_0_INPUT_RES_WIDTH_E  ),&totalWidth);
         if(ret != XMLDB__EMPTY_ENTRY)
         {
            writerConfig.srcCfg.dimensionCfg.imageDim.imageWidth = totalWidth;
            writerConfig.srcCfg.dimensionCfg.bufferWidth = writerConfig.srcCfg.dimensionCfg.imageDim.imageWidth * writerCfg.numImages;
         }


         //optimize num loops for writer. find and update burdt len
         writer->dmaChannelHandle->ccr.field.dstBurstLen = 0;
         lineWidth = writerConfig.srcCfg.dimensionCfg.bufferWidth * writerConfig.srcCfg.dimensionCfg.imageDim.pixelSize;

         //Hack! this should be removed once the infrastructure for passing all the image input descriptor is ready
         if (strstr(dbP->name, "MIPI_TX_VC_") != NULL)
         {
            //writerConfig.srcCfg.dimensionCfg.stride = writerConfig.srcCfg.dimensionCfg.stride / 2;
            writerConfig.srcCfg.dimensionCfg.frameStartX = 0;

            printf("activate hack writer: %d, %d, %d, %d\n",
                   writerConfig.srcCfg.dimensionCfg.frameStartY,
                   writerConfig.srcCfg.dimensionCfg.frameStartX,
                   lineWidth,
                   writerConfig.srcCfg.dimensionCfg.stride);
         }

         for( burstLenInd = 0; burstLenInd < 16; burstLenInd++ )
         {
            numOfLoopsMod = lineWidth % ((burstLenInd + 1) * (1 << INJECTION_BURST_SIZE)) / (1 << INJECTION_BURST_SIZE);
            if(numOfLoopsMod == 0)
            {
               tempBurstLen = burstLenInd;
            }
         }
         writerConfig.srcCfg.dimensionCfg.numLoopsPerLine = (lineWidth) / ((tempBurstLen + 1) * (1 << INJECTION_BURST_SIZE));
         writer->dmaChannelHandle->ccr.field.dstBurstLen = tempBurstLen;
         writer->dmaChannelHandle->ccr.field.srcBurstLen = tempBurstLen;
         //LOGG_PRINT(LOG_DEBUG_E, 0, "dbName %s final burst len %d set on path %d num loops %d line width %d dstBurstSize %d\n",
         //dbP->name, tempBurstLen ,(UINT32)(NUFLD_calcPath( NUFLD_AXIWR_E, instanceList[i],NU4100_PPE_AXI_WRITE0_AXI_CFG_BURST_LEN_E)),writerConfig.srcCfg.dimensionCfg.numLoopsPerLine,lineWidth,INJECTION_BURST_SIZE);
         XMLDB_setValueAndUpdateFastDb(dbP->dbH,NUFLD_AXIWR_E, instanceList[i],NU4100_PPE_AXI_WRITE0_AXI_CFG_BURST_LEN_E,tempBurstLen);
         //we have to write this field here, because in injection without multi-graph at this point we already config the registers
         //in multi-graph we will write the register again in reconfig function, but in non multi-graph not.
         PPE_MNGRG_setWriterBurstLen(instanceList[i],tempBurstLen);

         //printf("SEQ_MNGRP_setEnableWriterDma writer inst %d startY %d totalHeight %d frameStartY %d imageHeight %d\n",instanceList[i],startY,totalHeight,writerConfig.srcCfg.dimensionCfg.frameStartY,writerConfig.srcCfg.dimensionCfg.imageDim.imageHeight);
         //printf ("channelCb=%p  cbArgs=%p\n",dbP->path[dmaChId].channelCb, dbP->path[dmaChId].cbArgs);
         //printf ("channelCb=%p  cbArgs=%p\n",dbP->path[dmaChId].channelCb, dbP->path[dmaChId].cbArgs);
         CDE_MNGRG_setSysId( writer, INU_DEFSG_SYS_CH_8_INJECT_ID_E);              // bind readerID to dma channel
         ret = CDE_MNGRG_setupChannel( writer, &writerConfig, &writerCbCfg );

         //register this block to our voter
         HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_DMA0 + writer->dmaChannelHandle->coreNum);

         if(ERRG_FAILED(ret))
         {
            return ret;
         }
      }
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableWriterDma
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
static ERRG_codeE SEQ_MNGRP_setDisableWriterDma( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;                                 // TODO: build error codes for sequencer
   UINT32 i;
   int blockI=SEQ_MNGRG_XML_BLOCK_INJECTION_E;
   CDE_MNGRG_channnelInfoT *writerH=NULL;
   LOGG_PRINT(LOG_INFO_E, 0, "##### Disabling DMAs ##### \n");
   for ( i = 0; i < NU4K_MAX_DB_META_PATHS; i++)
   {
      if (instanceList[i] != 0xFF)
      {
         PPE_MNGRG_setDisableAxiWr(dbP->dbH,block,instanceList[i],arg);
         if (block == NUFLD_GEN_E)
         {
            blockI=SEQ_MNGRG_XML_BLOCK_INJECTION_E;
         }
         writerH =dbP->blockVec[blockI].blockInfo[instanceList[i]].blockPrivData;
         if( (writerH!= NULL))
         {
            //injection mode the CDE channel is in running state only when the USB has activated it
            //we should wait here in case its in running state, but this is ok for now
            ret = CDE_MNGRG_stopChannel(writerH);
            ret = CDE_MNGRG_closeChannel(writerH);
            dbP->blockVec[blockI].blockInfo[instanceList[i]].blockPrivData = NULL;
         }
      }
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableHist
*
*  Description:
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
static ERRG_codeE SEQ_MNGRP_setEnableHist( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   (void)block;
   HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_IAE);
   HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_PPE);
   HCG_MNGRG_registerHwUnitDownEvent(voterHandle, HCG_MNGRG_HW_EVENT_IAE_IAU_HIST0_RDY_ISR_E + instanceList[0], 1,0);
   HCG_MNGRG_registerEndEvent(voterHandle, HCG_MNGRG_HW_EVENT_IAE_IAU_HIST0_RDY_ISR_E + instanceList[0]);

   return IAE_MNGRG_setEnableHist( instanceList, (IAE_MNGRG_histogramCb)dbP->path[*(UINT8*)arg].channelCb, dbP->path[*(UINT8*)arg].cbArgs );
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableHist
*
*  Description:
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
static ERRG_codeE SEQ_MNGRP_setDisableHist( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE     ret = HW_MNGR__RET_SUCCESS;
   (void)arg;(void)block;
   ret = IAE_MNGRG_setDisableHist( instanceList );

   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableGEN
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
static ERRG_codeE SEQ_MNGRP_setEnableGEN( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   return IAE_MNGRG_setEnableGen(dbP->dbH, block, instanceList, arg, voterHandle);
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableGEN
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
static ERRG_codeE SEQ_MNGRP_setDisableGEN( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   return IAE_MNGRG_setDisableGen(dbP->dbH, block, instanceList, arg);
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setConfigSensor
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
static ERRG_codeE SEQ_MNGRP_setConfigSensor( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   return SENSORS_MNGRG_setConfigSensor(dbP->dbH, block, blkInstance, arg);
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableSensor
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
static ERRG_codeE SEQ_MNGRP_setEnableSensor( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   return SENSORS_MNGRG_setEnableSensor(dbP->dbH, block, instanceList, arg, voterHandle);
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableSensor
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
static ERRG_codeE SEQ_MNGRP_setDisableSensor( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   return SENSORS_MNGRG_setDisableSensor(dbP->dbH, block, instanceList, arg);
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setConfigSLU
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
static ERRG_codeE SEQ_MNGRP_setConfigSLU( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   return IAE_MNGRG_setConfigSlu(dbP->dbH, block, blkInstance, arg);
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableSLU
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
static ERRG_codeE SEQ_MNGRP_setEnableSLU( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   return IAE_MNGRG_setEnableSlu(dbP->dbH, block, instanceList, arg, voterHandle);
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableSLU
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
static ERRG_codeE SEQ_MNGRP_setDisableSLU( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   return IAE_MNGRG_setDisableSlu(dbP->dbH, block, instanceList, arg);
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableIAE
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
static ERRG_codeE SEQ_MNGRP_setEnableIAU( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   return IAE_MNGRG_setEnableIau(dbP->dbH, block, instanceList, arg, voterHandle);
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableIAE
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
static ERRG_codeE SEQ_MNGRP_setDisableIAU( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   return IAE_MNGRG_setDisableIau(dbP->dbH, block, instanceList, arg);
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableDPE
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
static ERRG_codeE SEQ_MNGRP_setEnableDPE( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   UINT32 tmpVal;
   //IAE out control - when enabling DPE, configure IAE out to DPE
   IAE_DRVG_iaeParametersT params;

   //read
   params.accessIaeRegister.iaeRegisterAddress = 0x60;
   params.accessIaeRegister.readWrite = READ_ACCESS_E;
   IAE_DRVG_accessIaeRegister(&params);

   ///modiefied
   tmpVal = params.accessIaeRegister.value;
   tmpVal &= 0xFFFF8000; //out0_sel..out3_sel bits 0..14', write zeros
   params.accessIaeRegister.value = tmpVal;

   //write
   params.accessIaeRegister.readWrite = WRITE_ACCESS_E;
   IAE_DRVG_accessIaeRegister(&params);




   return DPE_MNGRG_setEnableFunc(dbP->dbH, block, instanceList, arg, voterHandle);
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableDPE
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
static ERRG_codeE SEQ_MNGRP_setDisableDPE( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   UINT32 tmpVal;
   //IAE out control - when disabling DPE, configure IAE out to NONE
   IAE_DRVG_iaeParametersT params;
   params.accessIaeRegister.iaeRegisterAddress = 0x60;
   params.accessIaeRegister.readWrite = READ_ACCESS_E;
   IAE_DRVG_accessIaeRegister(&params);


   ///modiefied
   tmpVal = params.accessIaeRegister.value;
   tmpVal &= 0xFFFF8000;//not necessary
   tmpVal |= 0x7777;
   params.accessIaeRegister.value = tmpVal;

   //write
   params.accessIaeRegister.readWrite = WRITE_ACCESS_E;
   IAE_DRVG_accessIaeRegister(&params);


   return DPE_MNGRG_setDisableFunc(dbP->dbH, block, instanceList, arg);
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableDPP
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
static ERRG_codeE SEQ_MNGRP_setEnableDPP( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   return DEPTH_POST_setEnableFunc(dbP->dbH, block, instanceList, arg, voterHandle);
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableDPP
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
static ERRG_codeE SEQ_MNGRP_setDisableDPP( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   return DEPTH_POST_setDisableFunc(dbP->dbH, block, instanceList, arg);
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnablePPU
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
static ERRG_codeE SEQ_MNGRP_setEnablePPU( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   return PPE_MNGRG_setEnablePpu(dbP->dbH, block, instanceList, arg, voterHandle);
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableStub
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
static ERRG_codeE SEQ_MNGRP_setDisablePPU( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   return PPE_MNGRG_setDisablePpu(dbP->dbH, block, instanceList, arg);
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setConfigPPEreader
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
static ERRG_codeE SEQ_MNGRP_setConfigPPEreader( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   return PPE_MNGRG_setConfigAxiRd(dbP->dbH, block, blkInstance, arg);
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableVscCsi
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
static ERRG_codeE SEQ_MNGRP_setEnableVscCsi( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8 *instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   UINT32      i, intMode, vscInstanceMap = 0, viInstance;
   //Map the possible activated VCSs (0,1,2,3) to their interleaver
   static INT32 viMapFromInstanceMap[13] = {-1, 0, 0, 0, 1, -1, -1, -1, 1, -1, -1, -1, 1};

   for( i = 0; i < (NU4K_MAX_DB_META_PATHS); i++)
   {
      if (instanceList[i] != 0xFF)
      {
         PPE_DRVG_mipiVscCsiTxEn(instanceList[i], (PPE_DRVG_isrCbT)dbP->path[*(UINT8*)arg].channelCb, dbP->path[*(UINT8*)arg].cbArgs, &dbP->vcsIsrH[instanceList[i]]);
         HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_PPE);
         vscInstanceMap |= 1 << instanceList[i];
      }
   }

   if (vscInstanceMap < 13)
   {
      viInstance = viMapFromInstanceMap[vscInstanceMap];
      switch(viInstance)
      {
         case(0):
         {
            ret = XMLDB_getValue(dbP->dbH, NU4100_PPE_PPE_MISC_MIPI_VI0_CONTROL_INT_MODE_E,&intMode);
         }
         break;
         case(1):
         {
            ret = XMLDB_getValue(dbP->dbH, NU4100_PPE_PPE_MISC_MIPI_VI1_CONTROL_INT_MODE_E,&intMode);
         }
         break;
         default:
            ret = HW_MNGR__ERR_UNEXPECTED;
         break;
      }
   }
   else
   {
      ret = HW_MNGR__ERR_UNEXPECTED;
   }

   if (ERRG_SUCCEEDED(ret))
   {
      PPE_DRVG_mipiViEn(viInstance,intMode);
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableVscCsi
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
static ERRG_codeE SEQ_MNGRP_setDisableVscCsi( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8 *instanceList, void *arg )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   UINT32      i;

   for( i = 0; i < 4; i++)
   {
      if ((instanceList[i] != 0xFF) && (instanceList[i] < 6))
      {
         PPE_DRVG_mipiVscCsiTxDis(instanceList[i], dbP->vcsIsrH[instanceList[i]]);
      }
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableDphyTx
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
static ERRG_codeE SEQ_MNGRP_setEnableDphyTx( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8 *instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;
   UINT32 i, tx_num, nucfg_format, width, height, bpp, lanes, pllMhz;
   mipiInstConfigT mipiCfg;
   (void)dbP;(void)arg;(void)voterHandle;

   for ( i = 0; i < NU4K_MAX_DB_META_PATHS; i++)
   {
      if (instanceList[i] != 0xFF)
      {
         ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_MIPI_TX_E, instanceList[i],META_MIPI_TX_MIPI_TX_0_OUT_RES_WIDTH_E  ), &width);
         if (ERRG_SUCCEEDED(ret))
            ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_MIPI_TX_E, instanceList[i],META_MIPI_TX_MIPI_TX_0_OUT_RES_HEIGHT_E  ),&height);
         if (ERRG_SUCCEEDED(ret))
            ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_MIPI_TX_E, instanceList[i],META_MIPI_TX_MIPI_TX_0_FORMAT_E  ),        &nucfg_format);
         if (ERRG_SUCCEEDED(ret))
            ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_MIPI_TX_E, instanceList[i],META_MIPI_TX_MIPI_TX_0_OUT_RES_BPP_E  ),   &bpp);
         if (ERRG_SUCCEEDED(ret))
            ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_MIPI_TX_E, instanceList[i],BUS_MIPI_DPHY_TX0_LANES_E  ),   &lanes);
         if (ERRG_SUCCEEDED(ret))
            ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_MIPI_TX_E, instanceList[i],BUS_MIPI_DPHY_TX0_PLL_E  ),   &pllMhz);

         if (ERRG_SUCCEEDED(ret))
         {
            mipiCfg.mipiInst            = instanceList[i];
            mipiCfg.svt.pktSizeBytes    = width * bpp / 8;
            mipiCfg.svt.mipiVideoFormat = SEQ_MNGRP_convertToMipiFormat(nucfg_format);
            mipiCfg.dphy.format         = SEQ_MNGRP_convertToMipiFormat(nucfg_format);
            mipiCfg.dphy.laneEnable     = lanes;
            mipiCfg.dphy.pllMhz         = pllMhz;
            ret = MIPI_DRVG_cfgTx(&mipiCfg);
         }

         if (ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "Failed to enable Dphy Tx %d\n", instanceList[i] );
         }
         else
         {
            LOGG_PRINT(LOG_INFO_E, NULL, "Dphy Tx %d enabled (lanes %d, clk %d[mhz], pktSizeBytes %d, format %d)\n",
                                          instanceList[i], lanes, pllMhz, (width * bpp / 8), nucfg_format);
         }
      }
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableDphyTx
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
static ERRG_codeE SEQ_MNGRP_setDisableDphyTx( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   UINT32      i;
   mipiInstConfigT mipiCfg;

   (void)dbP;(void)arg;(void)instanceList;
   for ( i = 0; i < 4; i++)
   {
      if (instanceList[i] != 0xFF)
      {
         mipiCfg.mipiInst = instanceList[i];
         MIPI_DRVG_resetTx(&mipiCfg);
      }
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setConfigStub
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
static ERRG_codeE SEQ_MNGRP_setConfigStub( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                        // TODO: build error codes for sequencer
   (void)dbP;(void)arg;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "Sequences don't know how to configure block/instance %d/%d   \n", block, blkInstance );
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableStub
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
static ERRG_codeE SEQ_MNGRP_setEnableStub( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   (void)dbP;(void)arg;(void)instanceList;(void)voterHandle;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "Sequencer don't know how ENABLE block/instance %d   \n", block );
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableStub
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
static ERRG_codeE SEQ_MNGRP_setDisableStub( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   (void)dbP;(void)arg;(void)instanceList;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "Sequencer don't know how DISABLE block/instance %d   \n", block );
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setConfigIsp
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
static ERRG_codeE SEQ_MNGRP_setConfigIsp( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                        // TODO: build error codes for sequencer
   (void)dbP;(void)arg;
   ISP_MNGRG_streamReadCfgT cfg;
   LOGG_PRINT(LOG_INFO_E, NULL, "SEQ_MNGRP_setConfigIsp block/instance %d/%d   \n", block, blkInstance );
   int i, sensorInst = -1;
   SENSORS_MNGRG_sensorInfoT *senInfo = NULL;
   char senModel[20]="";
   UINT32 sluInst, sluWidth, sluHeight, sluDimentionFound=1;

   if(dbP->setConfigDoneISPBitmap  &  (1<<blkInstance))
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Returning early, ISP Configuration done for ISP instance %lu \n", blkInstance);
      return HW_MNGR__RET_SUCCESS;
   }

   //serach on path, and find the sensor inst for getting sensor info
   //this loop does not support more then one sensor as input to same isp rd
   for ( i = 0; i < SEQ_MNGRG_MAX_SYSTEM_PATHS; i++)
   {
      if (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E] == blkInstance)
      {
         if (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SENS_E]!= 0xFF)
         {
            sensorInst = dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SENS_E];
            senInfo = SENSORS_MNGRG_getSensorInfo((INU_DEFSG_senSelectE)sensorInst);
            LOGG_PRINT(LOG_INFO_E, NULL, "found for isp inst %d sensor inst %d model %d\n", blkInstance, sensorInst,senInfo->sensorCfg.sensorModel);
         }
      }
   }
   if (senInfo == NULL)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "sensor not found on parh\n");
      return HW_MNGR__ERR_UNEXPECTED;
   }
   else
   {
      //update sensor info (width&height) according to SLU crop. ISP will use it to know which dimention will enter to him
      SEQ_MNGRG_getSluInstBySenInst(sensorInst, &sluInst);
      if (sluInst < 2)
      {
         ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_SLU_PARALLEL_E, sluInst ,NU4100_IAE_SLU0_FRAME_SIZE_HORZ_E  ),   &sluWidth);
         if (ERRG_FAILED(ret))
         {
            sluDimentionFound=0;
         }
         ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_SLU_PARALLEL_E, sluInst ,NU4100_IAE_SLU0_FRAME_SIZE_VERT_E  ),   &sluHeight);
         if (ERRG_FAILED(ret))
         {
            sluDimentionFound=0;
         }
      }
      else if (sluInst < 6)
      {
         sluInst = sluInst - 2;
         ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_SLU_E, sluInst ,NU4100_IAE_SLU2_FRAME_SIZE_HORZ_E  ),   &sluWidth);
         if (ERRG_FAILED(ret))
         {
            sluDimentionFound=0;
         }
         ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_SLU_E, sluInst ,NU4100_IAE_SLU2_FRAME_SIZE_VERT_E  ),   &sluHeight);
         if (ERRG_FAILED(ret))
         {
            sluDimentionFound=0;
         }
      }
      if (sluDimentionFound)
      {
         senInfo->sensorCfg.senWidth = sluWidth + 1;
         senInfo->sensorCfg.senHeight= sluHeight + 1;
         LOGG_PRINT(LOG_INFO_E, NULL, "dimention of sensor %d was updated according SLU crop sizes: W %d H %d\n",senInfo->sensorCfg.sensorModel, senInfo->sensorCfg.senWidth,senInfo->sensorCfg.senHeight);
      }


      ret =  SEQ_MNGRG_getIspConfig( dbP->dbH, blkInstance, &cfg );

      sprintf(senModel,"%d",senInfo->sensorCfg.sensorModel);

      ret =  ISP_MNGRG_setConfig(blkInstance, senModel, cfg);

      dbP->setConfigDoneISPBitmap|= (1<<blkInstance);
      return ret;
   }
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableIsp
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
static ERRG_codeE SEQ_MNGRP_setEnableIsp( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   (void)dbP;(void)instanceList;(void)voterHandle;(void)arg;
   ISP_MNGRG_streamReadCfgT cfg;
    if(dbP->setEnableDoneISPBitmap & (1<< instanceList[0]))
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Returning early, ISP Enable done for ISP instance %lu \n",instanceList[0] );
      return HW_MNGR__RET_SUCCESS;
   }

   LOGG_PRINT(LOG_DEBUG_E, NULL, "SEQ_MNGRP_setEnableIsp block/instance %d   \n", block );

   ret =  SEQ_MNGRG_getIspConfig( dbP->dbH, instanceList[0], &cfg );//TODO: instanceList?

   ret =  ISP_MNGRG_setEnable(instanceList[0], &cfg);
   dbP->setEnableDoneISPBitmap|= 1<< instanceList[0];
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableIsp
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
static ERRG_codeE SEQ_MNGRP_setDisableIsp( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;
   (void)dbP;(void)instanceList;(void)arg;

   //printf(RED("SEQ_MNGRP_setDisableIsp block/instance %d instanceList %p instance %d\n"), block, instanceList, *instanceList );
   dbP->setConfigDoneISPBitmap &= ~(1 << instanceList[0]);
   dbP->setEnableDoneISPBitmap &= ~(1 << instanceList[0]);
   ret =  ISP_MNGRG_setDisable(*instanceList);
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableIsp
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
static ERRG_codeE SEQ_MNGRP_setEnableIpe( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   (void)dbP;(void)instanceList;(void)voterHandle;(void)arg;

   ISP_MNGRG_setEnableIpe(*instanceList);
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableIsp
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
static ERRG_codeE SEQ_MNGRP_setDisableIpe( SEQ_MNGP_sequenceDbT *dbP, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;
   (void)dbP;(void)instanceList;(void)arg;

   ret =  ISP_MNGRG_setDisableIpe(*instanceList);
   return ret;
}

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
static ERRG_codeE SEQ_MNGRP_getPaths(SEQ_MNGP_sequenceDbT *dbP, UINT32 *pathMap )
{
   ERRG_codeE     ret = HW_MNGR__RET_SUCCESS;
   UINT32 tmpPathMap,i;

   for( i = 0; i < NU4K_NUM_AXI_INTERLEAVERS; i++)
   {
      tmpPathMap = 0;
      XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_READERS_E, i, (META_READERS_RD_0_PATH_MAP_E)), &tmpPathMap);
      *pathMap |= tmpPathMap;
   }

   for( i = 0; i < NU4K_NUM_HISTOGRAM_READER; i++)
   {
      tmpPathMap = 0;
      XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_HIST_E, i, (META_HISTOGRAMS_HIST_0_PATH_MAP_E)), &tmpPathMap);
      *pathMap |= tmpPathMap;
   }

   for( i = 0; i < NU4K_NUM_CVA_READERS; i++)
   {
      tmpPathMap = 0;
      XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_CVA_RD_E, i, (META_CVA_READERS_CVA_RD_0_PATH_MAP_E)), &tmpPathMap);
      *pathMap |= tmpPathMap;
   }

   for( i = 0; i < NU4K_NUM_DPHY_TX; i++)
   {
      tmpPathMap = 0;
      XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_MIPI_TX_E, i, (META_MIPI_TX_MIPI_TX_0_PATH_MAP_E)), &tmpPathMap);
      *pathMap |= tmpPathMap;
   }

      for( i = 0; i < NU4K_NUM_ISP_READERS; i++)
   {
      tmpPathMap = 0;
      XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_META_ISP_RD_E, i, (META_ISP_READERS_ISP_RD_0_PATH_MAP_E)), &tmpPathMap);
      *pathMap |= tmpPathMap;
   }

   return ret;
}


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
static ERRG_codeE SEQ_MNGRP_createPath( SEQ_MNGP_sequenceDbT *dbP, UINT8 pathNum )
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;

   UINT32 blockId = 0;
   UINT32 val = 0;
   UINT32 blockInstance = 0;
   XMLDB_pathE tmpPath;

   for(blockId = 0;blockId < (SEQ_MNGRG_XML_BLOCK_LAST_E);blockId++ )
   {
      tmpPath = NUFLD_calcPath( NUFLD_META_PATHS_E, pathNum, (META_PATHS_PATH_0_GEN_E + blockId));
      ret = XMLDB_getValue( dbP->dbH, tmpPath, &val);
      if(ret == XMLDB__EMPTY_ENTRY)
      {
         LOGG_PRINT(LOG_WARN_E,NULL,"seq mngr warn: empty entry in path for %d block- ignoring\n",blockId);
         continue;
      }
      blockInstance = ((val >> 24) & 0xFF);
//    printf ("SEQ_MNGRP_createPath pathnum=%d blockId = %d  val = %d  instance=%d \n", pathNum, blockId, val,blockInstance);
      dbP->path[pathNum].block[blockId] = blockInstance; // save block in the sequencer table

      //create fast write block
      if (blockInstance != 0xFF)
      {
         ret = XMLDB_fillFastWriteBlock(dbP->dbH, SEQ_MNGRP_xmlblk2nufld(blockId), blockInstance);
      }
   }
   return ret;
}


static ERRG_codeE SEQ_MNGRP_pausePaths(SEQ_MNGP_sequenceDbT *dbP, UINT8 *pausedPathList, UINT8 pausedPathListCtr)
{
   UINT8 instanceList[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   UINT8 instanceListParallel[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   UINT8 instanceListPosition = 0;
   UINT8 instanceListPositionParallel = 0;
   int pathI, pathToPause;
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;

   if (pausedPathListCtr)
   {
      memset (instanceList,0xFF,sizeof(instanceList));
      memset (instanceListParallel,0xFF,sizeof(instanceListParallel));
      for (pathI = 0; pathI < pausedPathListCtr; pathI++)
      {
         pathToPause = pausedPathList[pathI];

         if (dbP->path[pathToPause].block[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E] != 0xFF)
         {
            instanceListParallel[instanceListPositionParallel] = dbP->path[pathToPause].block[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E];
            instanceListPositionParallel++;
         }
         else if (dbP->path[pathToPause].block[SEQ_MNGRG_XML_BLOCK_SLU_E] != 0xFF)
         {
            instanceList[instanceListPosition] = dbP->path[pathToPause].block[SEQ_MNGRG_XML_BLOCK_SLU_E];
            instanceListPosition++;
         }
      }

      //we suspend the SLU's, the strobes might still occure (up event), but there will not be and DMA isr (down event)
      HCG_MNGRG_voteUnits(mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E]);
      HCG_MNGRG_voteUnits(mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_E]);
      /*If two SLUs share the same path they should be enabled and disabled atomically at the same time so I have fixed this below - William Bright*/
      ret = dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_E].blockOperation.disable(dbP, SEQ_MNGRP_xmlblk2nufld(SEQ_MNGRG_XML_BLOCK_SLU_E), instanceList, instanceListParallel);
      HCG_MNGRG_devoteUnits(mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E]);
      HCG_MNGRG_devoteUnits(mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_E]);

      OS_LYRG_usleep(25000);
   }
   return ret ;
}

static ERRG_codeE SEQ_MNGRP_resumePaths(SEQ_MNGP_sequenceDbT *dbP, UINT8 *pausedPathList, UINT8 pausedPathListCtr)
{
   UINT8 instanceList[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   UINT8 instanceListParallel[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   UINT8 instanceListPosition = 0;
   UINT8 instanceListPositionParallel = 0;
   int pathI, pathToPause;
   ERRG_codeE ret;

   memset (instanceList,0xFF,sizeof(instanceList));
   memset (instanceListParallel,0xFF,sizeof(instanceListParallel));
   for (pathI = 0; pathI < pausedPathListCtr; pathI++)
   {
      pathToPause = pausedPathList[pathI];

      if (dbP->path[pathToPause].block[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E] != 0xFF)
      {
         instanceListParallel[instanceListPositionParallel] = dbP->path[pathToPause].block[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E];
         instanceListPositionParallel++;
      }
      else if (dbP->path[pathToPause].block[SEQ_MNGRG_XML_BLOCK_SLU_E] != 0xFF)
      {
         instanceList[instanceListPosition] = dbP->path[pathToPause].block[SEQ_MNGRG_XML_BLOCK_SLU_E];
         instanceListPosition++;
      }
   }

   HCG_MNGRG_voteUnits(mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E]);
   HCG_MNGRG_voteUnits(mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_E]);
   /*If two SLUs share the same path they should be enabled and disabled atomically at the same time so I have fixed this below - William Bright*/
   ret = dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_E].blockOperation.enable(dbP, SEQ_MNGRP_xmlblk2nufld(SEQ_MNGRG_XML_BLOCK_SLU_E), instanceList, instanceListParallel, NULL);
   HCG_MNGRG_devoteUnits(mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E]);
   HCG_MNGRG_devoteUnits(mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_E]);


   return ret ;
}


static void SEQ_MNGRP_bindBlkOperations(SEQ_MNGP_sequenceDbT *dbP)
{
   UINT32 i;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_GEN_E].blockInstanceNum = NU4K_NUM_GENS;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_GEN_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_GEN_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_GEN_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_GEN_E].blockOperation.enable  = SEQ_MNGRP_setEnableGEN;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_GEN_E].blockOperation.disable = SEQ_MNGRP_setDisableGEN;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E].blockInstanceNum = NU4K_NUM_SENSORS;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigSensor;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E].blockOperation.enable  = SEQ_MNGRP_setEnableSensor;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E].blockOperation.disable = SEQ_MNGRP_setDisableSensor;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SENS_E].blockInstanceNum = NU4K_NUM_SENSORS;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SENS_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_SENS_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SENS_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SENS_E].blockOperation.enable  = SEQ_MNGRP_setEnableStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SENS_E].blockOperation.disable = SEQ_MNGRP_setDisableStub;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_MEDIATOR_E].blockInstanceNum = NU4K_NUM_MEDIATORS;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_MEDIATOR_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_MEDIATOR_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_MEDIATOR_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_MEDIATOR_E].blockOperation.enable  = SEQ_MNGRP_setEnableStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_MEDIATOR_E].blockOperation.disable = SEQ_MNGRP_setDisableStub;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_MIPI_RX_E].blockInstanceNum = NU4K_NUM_MIPI_RX;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_MIPI_RX_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_MIPI_RX_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_MIPI_RX_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub; //config mipi when configuring sensor
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_MIPI_RX_E].blockOperation.enable  = SEQ_MNGRP_setEnableStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_MIPI_RX_E].blockOperation.disable = SEQ_MNGRP_setDisableStub;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PAR_RX_E].blockInstanceNum = NU4K_NUM_PAR_RX;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PAR_RX_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_PAR_RX_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PAR_RX_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PAR_RX_E].blockOperation.enable  = SEQ_MNGRP_setEnableStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PAR_RX_E].blockOperation.disable = SEQ_MNGRP_setDisableStub;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PAR_RX_E].blockInstanceNum = NU4K_NUM_PAR_RX;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_INJECTION_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_INJECTION_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_INJECTION_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigWriterDma;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_INJECTION_E].blockOperation.enable  = SEQ_MNGRP_setEnableWriterDma;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_INJECTION_E].blockOperation.disable = SEQ_MNGRP_setDisableWriterDma;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_E].blockInstanceNum = NU4K_NUM_WRITERS;

   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigSLU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E].blockOperation.enable  = SEQ_MNGRP_setEnableSLU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E].blockOperation.disable = SEQ_MNGRP_setDisableSLU;

   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_SLU_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigSLU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_E].blockOperation.enable  = SEQ_MNGRP_setEnableSLU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_E].blockOperation.disable = SEQ_MNGRP_setDisableSLU;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_ISP_E].blockInstanceNum = NU4K_NUM_ISPS;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_ISP_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_ISP_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_ISP_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_ISP_E].blockOperation.enable  = SEQ_MNGRP_setEnableIpe;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_ISP_E].blockOperation.disable = SEQ_MNGRP_setDisableIpe;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_IAU_E].blockInstanceNum = NU4K_NUM_IAUS;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_IAU_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_IAU_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_IAU_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_IAU_E].blockOperation.enable  = SEQ_MNGRP_setEnableIAU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_IAU_E].blockOperation.disable = SEQ_MNGRP_setDisableIAU;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E].blockInstanceNum = NU4K_NUM_IAUS_COLOR;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E].blockOperation.enable  = SEQ_MNGRP_setEnableIAU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E].blockOperation.disable = SEQ_MNGRP_setDisableIAU;


   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_HIST_E].blockInstanceNum = NU4K_NUM_HIST;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_HIST_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_HIST_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_HIST_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_HIST_E].blockOperation.enable  = SEQ_MNGRP_setEnableHist;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_HIST_E].blockOperation.disable = SEQ_MNGRP_setDisableHist;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DPE_E].blockInstanceNum = NU4K_NUM_DPES;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DPE_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_DPE_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DPE_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DPE_E].blockOperation.enable  = SEQ_MNGRP_setEnableDPE;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DPE_E].blockOperation.disable = SEQ_MNGRP_setDisableDPE;


   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E].blockInstanceNum = 1;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E].blockOperation.enable  = SEQ_MNGRP_setEnableDPP;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E].blockOperation.disable = SEQ_MNGRP_setDisableDPP;


   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E].blockInstanceNum = NU4K_NUM_PPUS_NOSCL;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E].blockOperation.enable  = SEQ_MNGRP_setEnablePPU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E].blockOperation.disable = SEQ_MNGRP_setDisablePPU;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E].blockInstanceNum = NU4K_NUM_PPUS_NOSCL;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E].blockOperation.enable  = SEQ_MNGRP_setEnablePPU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E].blockOperation.disable = SEQ_MNGRP_setDisablePPU;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_SCL_E].blockInstanceNum = NU4K_NUM_PPUS_SCL;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_SCL_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_PPU_SCL_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_SCL_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_SCL_E].blockOperation.enable  = SEQ_MNGRP_setEnablePPU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_SCL_E].blockOperation.disable = SEQ_MNGRP_setDisablePPU;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E].blockInstanceNum = NU4K_NUM_PPUS_HYBSCL;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E].blockOperation.enable  = SEQ_MNGRP_setEnablePPU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E].blockOperation.disable = SEQ_MNGRP_setDisablePPU;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_HYB_E].blockInstanceNum = NU4K_NUM_PPUS_HYB;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_HYB_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_PPU_HYB_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_HYB_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_HYB_E].blockOperation.enable  = SEQ_MNGRP_setEnablePPU;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_PPU_HYB_E].blockOperation.disable = SEQ_MNGRP_setDisablePPU;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RD_E].blockInstanceNum = NU4K_NUM_AXI_READERS;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RD_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_RD_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RD_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigPPEreader;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RD_E].blockOperation.enable  = SEQ_MNGRP_setEnableStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RD_E].blockOperation.disable = SEQ_MNGRP_setDisableStub;

   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RDOUT_E].blockInstanceNum = NU4K_NUM_AXI_READERS;
   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RDOUT_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_RDOUT_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RDOUT_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigDma;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RDOUT_E].blockOperation.enable  = SEQ_MNGRP_setEnableDma;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RDOUT_E].blockOperation.disable = SEQ_MNGRP_setDisableDma;

   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_CVA_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_E].blockOperation.enable  = SEQ_MNGRP_setEnableCVA;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_E].blockOperation.disable = SEQ_MNGRP_setDisableCVA;

   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_RD_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_CVA_RD_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_RD_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_RD_E].blockOperation.enable  = SEQ_MNGRP_setEnableStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_RD_E].blockOperation.disable = SEQ_MNGRP_setDisableStub;

   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigCvaDma;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E].blockOperation.enable  = SEQ_MNGRP_setEnableCvaDma;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E].blockOperation.disable = SEQ_MNGRP_setDisableCvaDma;

   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E].blockOperation.enable  = SEQ_MNGRP_setEnableVscCsi;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E].blockOperation.disable = SEQ_MNGRP_setDisableVscCsi;

   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DPHY_TX_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_DPHY_TX_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DPHY_TX_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigStub;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DPHY_TX_E].blockOperation.enable  = SEQ_MNGRP_setEnableDphyTx;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_DPHY_TX_E].blockOperation.disable = SEQ_MNGRP_setDisableDphyTx;

   for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
   {
      dbP->blockVec[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E].blockInfo[i].blockType = SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E;
   }
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E].blockOperation.setCfg  = SEQ_MNGRP_setConfigIsp;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E].blockOperation.enable  = SEQ_MNGRP_setEnableIsp;
   dbP->blockVec[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E].blockOperation.disable = SEQ_MNGRP_setDisableIsp;
}

/****************************************************************************
***************      G L O B A L       F U N C T I O N S       ***************
****************************************************************************/
static ERRG_codeE SEQ_MNGRP_writerSchedulerInit( );
static void *sofInterruptCb = NULL;
static void *eofInterruptCb = NULL;

void SEQ_MNGRG_markExtIntChannel(void *handle, UINT32 chId)
{
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT *)handle;
   if (chId < SEQ_MNGRG_MAX_CHANNEL_NUM)
   {
      dbP->extIntInfo.extIntChs |= (1 << chId);
   }

   printf("***************Extended Interleaving Mode Channel Mask: 0x%x\n", dbP->extIntInfo.extIntChs);
}

 BOOLEAN SEQ_MNGRG_isExtIntChannel(void *handle, UINT32 chId)
{
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT *)handle;
   if (chId < SEQ_MNGRG_MAX_CHANNEL_NUM)
   {
      return (dbP->extIntInfo.extIntChs & (1 << chId))? TRUE:FALSE;
   }

   return FALSE;
}

ERRG_codeE SEQ_MNGRG_init( void )
{

   UINT32 i = 0;

   HCA_MNGRG_init();
   SEQ_MNGRP_writerSchedulerInit();
   memset((void*)&channelDb, 0, sizeof(channelDb));
   OS_LYRG_aquireMutex(&ppeCropMutex);
   OS_LYRG_aquireMutex(&calUpdateMutex);
   memset(&mapSeqToHwUnits,0,sizeof(mapSeqToHwUnits));
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SENS_E]        = (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E]  = (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_MEDIATOR_E]    = (0 ); // no HW
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_GEN_E]         = (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_MIPI_RX_E]     = (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_PAR_RX_E]      = (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_INJECTION_E]   = (1 << HCG_MNGRG_PPE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E]= (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_E]         = (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_ISP_E]         = (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_IAU_E]         = (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E]   = (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_HIST_E]        = (1 << HCG_MNGRG_IAE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_DPE_E]         = (1 << HCG_MNGRG_DPE) | (1 << HCG_MNGRG_IAE); //changing IAE out_control
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E]  = (1 << HCG_MNGRG_PPE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E]   = (1 << HCG_MNGRG_PPE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_PPU_SCL_E]     = (1 << HCG_MNGRG_PPE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E]  = (1 << HCG_MNGRG_PPE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_PPU_HYB_E]     = (1 << HCG_MNGRG_PPE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_CVA_RD_E]      = (0); //no HW
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E]  = (1 << HCG_MNGRG_PPE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_DPHY_TX_E]     = (1 << HCG_MNGRG_PPE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_CVA_E]         = (1 << HCG_MNGRG_PPE) | (1 << HCG_MNGRG_CVA);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_RD_E]          = (1 << HCG_MNGRG_PPE);
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E]   = (1 << HCG_MNGRG_PPE) | (1 << HCG_MNGRG_CVA) | (1 << HCG_MNGRG_DMA1) | (1 << HCG_MNGRG_DMA2); //iic+freak on dma1, control on dma2
   mapSeqToHwUnits[SEQ_MNGRG_XML_BLOCK_RDOUT_E]       = (1 << HCG_MNGRG_DMA0) | (1 << HCG_MNGRG_DMA1) | (1 << HCG_MNGRG_DMA2) | (1 << HCG_MNGRG_PPE);

   memset(&mapSeqToHcaHwUnits,0,sizeof(mapSeqToHcaHwUnits));
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_SENS_E]        = HCA_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E]  = HCA_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_GEN_E]         = HCA_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_MIPI_RX_E]     = HCA_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_PAR_RX_E]      = HCG_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_INJECTION_E]   = HCA_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E]= HCA_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_SLU_E]         = HCA_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_IAU_E]         = HCA_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E]   = HCA_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_HIST_E]        = HCA_MNGRG_IAE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_DPE_E]         = HCA_MNGRG_DPE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E]  = HCA_MNGRG_PPE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E]   = HCA_MNGRG_PPE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_PPU_SCL_E]     = HCA_MNGRG_PPE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E]  = HCA_MNGRG_PPE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_PPU_HYB_E]     = HCA_MNGRG_PPE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E]  = HCA_MNGRG_PPE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_DPHY_TX_E]     = HCA_MNGRG_PPE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_CVA_E]         = HCA_MNGRG_CVA;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_RD_E]          = HCA_MNGRG_PPE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E]   = HCA_MNGRG_PPE;
   mapSeqToHcaHwUnits[SEQ_MNGRG_XML_BLOCK_RDOUT_E]       = HCA_MNGRG_PPE;
#ifdef CDE_DRVG_METADATA
   int unused_i = 0;
   #ifdef CDE_DRVG_ENABLE_SENSOR_SYNC_AND_UPDATE
   IAE_DRVG_registerSnsrIsrCb(METADATA_UPDATER_sofInterruptCb,NULL,(void*)&sofInterruptCb);   
   IAE_DRVG_registerEofISRCb(SENSORSYNC_UPDATER_eofInterruptCb,NULL,(void*)&eofInterruptCb);
   IAE_DRVG_registerEofISRCb(METADATA_UPDATER_eofInterruptCb,NULL,(void*)&eofInterruptCb);
   #endif
#endif
   //LOGG_PRINT(LOG_INFO_E, NULL, "Sequence manager initialized. DB \n");
   return HW_MNGR__RET_SUCCESS;
}


ERRG_codeE SEQ_MNGRG_deinit( void )
{
   OS_LYRG_releaseMutex(&ppeCropMutex);
   OS_LYRG_releaseMutex(&calUpdateMutex);
#ifdef CDE_DRVG_METADATA
   IAE_DRVG_unregisterSnsrIsrCb(sofInterruptCb);
   IAE_DRVG_unregisterEofISRCb(eofInterruptCb);
#endif
   return HW_MNGR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name:
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE SEQ_MNGRG_initDb( SEQ_MNGRG_handleT *handleP, XMLDB_dbH hwDb, const char *name )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   UINT32 pathMap = 0;
   UINT32 i = 0;
   int blockI=0;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)malloc(sizeof(SEQ_MNGP_sequenceDbT));

   if (!dbP)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to alloc mem (%d) for seq db\n",sizeof(SEQ_MNGP_sequenceDbT));
      return HW_MNGR__ERR_OUT_OF_MEM;
   }

   memset(dbP, 0xFF, sizeof(SEQ_MNGP_sequenceDbT));
   dbP->isMultiGraphInj = 0;
   dbP->setConfigDoneISPBitmap = 0;
   dbP->setEnableDoneISPBitmap = 0;
   memset(dbP->cvaReaders, 0, sizeof(dbP->cvaReaders));
   memset(&dbP->extIntInfo, 0, sizeof(dbP->extIntInfo));
   for (blockI=0; blockI<SEQ_MNGRG_XML_BLOCK_LAST_E;++ blockI)
   {
      for (i=0;i<SEQ_MNGRG_MAX_BLOCK_INSTANCES;++i)
      {
         dbP->blockVec[blockI].blockInfo[i].blockPrivData =NULL;
         dbP->blockVec[blockI].blockInfo[i].refCount=0;
      }
   }

   SEQ_MNGRP_bindBlkOperations(dbP);
   dbP->dbH = hwDb;

   ret = SEQ_MNGRP_getPaths(dbP, &pathMap);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "SEQ_MNGRP_getPaths failed\n");
      return ret;
   }
   //printf ("pathMap%x \n",pathMap);
   for( i = 0; i < SEQ_MNGRG_MAX_SYSTEM_PATHS; i++ )
   {
      dbP->path[i].status = SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E;
      if (pathMap & 0x1)
      {
         //printf ("SEQ_MNGRG_createPaths i=%d \n",i);
         SEQ_MNGRP_createPath(dbP, i);
      }
      pathMap = pathMap >> 1;
   }

   if (ERRG_SUCCEEDED(ret))
   {
      //SEQ_MNGRG_showSysConfig(*handleP);
      //store for accessing
      for(i = 0; i < SEQ_MNGRG_MAX_DBS_NUM; i++)
      {
         if (!SEQ_MNGRP_dbsP[i])
         {
            SEQ_MNGRP_dbsP[i] = dbP;
         }
      }
      dbP->name = strdup(name);
      ret = SEQ_MNGRP_writerSchedulerRegister(dbP);
      if (ERRG_SUCCEEDED(ret))
      {
         *handleP = dbP;
         LOGG_PRINT(LOG_INFO_E, NULL, "created new sequence DB %s\n",dbP->name);
      }
   }
   else
   {
      free(dbP);
   }

   return ret ;

}

/****************************************************************************
*
*  Function Name:
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE SEQ_MNGRG_deinitDb( SEQ_MNGRG_handleT handle )
{
   ERRG_codeE ret;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;
   UINT32 i;

   if (!dbP)
      return HW_MNGR__ERR_NULL_PTR;

   ret = SEQ_MNGRP_writerSchedulerUnregister(dbP);
   if (ERRG_SUCCEEDED(ret))
   {
      for(i = 0; i < SEQ_MNGRG_MAX_DBS_NUM; i++)
      {
         if (SEQ_MNGRP_dbsP[i] == dbP)
         {
            SEQ_MNGRP_dbsP[i] = NULL;
         }
      }

      free(dbP->name);
      free(dbP);
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableDma
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
//#define SEQ_MNGRP_WRITER_SCHED_DEBUG
//#define SEQ_MNGRP_WRITER_GRANT_MUTEX_DEBUG //uncomment to enable step-by-step debug prints for writer mechanism
#define SEQ_MNGRP_WRITER_SCHED_WRITERS_MAX_NUM    (3)
#define SEQ_MNGRP_WRITER_SCHED_MAX_NUM_INJECTIONS (6)
#define SEQ_MNGRP_WRITER_SCHED_BLOCK_SLEEP_US     (100)
typedef struct
{
   SEQ_MNGP_sequenceDbT *dbP;
   UINT32 activeWritersBitmap;
   //OS_LYRG_mutexT activeWritersMutexmap[32];
   OS_LYRG_msgQueT activeWritersQueuemap[32];
   UINT32 doneWritersBitmap;
   int activeCannels[SEQ_MNGRG_MAX_CHANNEL_NUM];
   int doneCannels[SEQ_MNGRG_MAX_CHANNEL_NUM];
} SEQ_MNGRP_channelWritersT;

typedef struct
{
   SEQ_MNGRP_channelWritersT channelWritersDb[SEQ_MNGRP_WRITER_SCHED_MAX_NUM_INJECTIONS]; //max 6 types of injections
   SEQ_MNGP_sequenceDbT *activeDb;
   OS_LYRG_mutexT        mutex;
} SEQ_MNGRP_writerSchedulerDb;

static SEQ_MNGRP_writerSchedulerDb writerSchedulerDb;


#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
#define NUM_OF_PRINTS (4096)
int ind = 0;
char buf[NUM_OF_PRINTS][256];

void SEQ_MNGRG_addSchedulerDebugPrint(char *string)
{
   if (ind<NUM_OF_PRINTS)
   {
      //sprintf(buf[ind++], "%d: %s", ind, string);
      strcpy(buf[ind++],string);
   }
   else
   {
      ind=0;
      //memset(buf,0,sizeof(buf));
   }
}

void SEQ_MNGRG_printSchedulerDebugBuf()
{
   printf("printBuf function, total indexes is %d:\n",ind);
   for (int i=0; i<ind;i++)
   {
      printf("%s",buf[i]);
   }
}
#endif

static ERRG_codeE SEQ_MNGR_initGrantQueues(void)
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   uint32_t i, j;

   for(i = 0; i < SEQ_MNGRP_WRITER_SCHED_MAX_NUM_INJECTIONS; i++)
   {
      for(j = 0; j < 32; j++)
      {
         writerSchedulerDb.channelWritersDb[i].activeWritersQueuemap[j].maxMsgs = 1;
         writerSchedulerDb.channelWritersDb[i].activeWritersQueuemap[j].msgSize = sizeof(uint8_t);
         snprintf(writerSchedulerDb.channelWritersDb[i].activeWritersQueuemap[j].name, OS_LYRG_MAX_MQUE_NAME_LEN, "/wGQ%d-%d", i, j); //names of the queues will be from wGQ0-0 to wGQ5-31
         OS_LYRG_createMsgQue(&writerSchedulerDb.channelWritersDb[i].activeWritersQueuemap[j], OS_LYRG_EVENT_USER_E);
      }
   }

   #ifdef SEQ_MNGRP_WRITER_GRANT_MUTEX_DEBUG
      printf(GREEN("Initialized Grant queues\n"));
   #endif

   return ret;
}


static ERRG_codeE SEQ_MNGR_lockWriterGrantQueue(SEQ_MNGRP_channelWritersT *dbP, UINT32 index)
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   int msgRet = 0, tmpMsg = 0, tmpMsgLen = 1;
   pthread_t tid = pthread_self();

   #ifdef SEQ_MNGRP_WRITER_GRANT_MUTEX_DEBUG
      printf(GREEN("Attempting to lock queue %d\n"), index);
   #endif

   
   OS_LYRG_unlockMutex(&writerSchedulerDb.mutex); //whenever this function is called, this mutex is always locked
   /* We need to unlock writerSchedulerDb.mutex before trying to lock writer mutex so that when the thread goes to sleep, writerSchedulerDb.mutex is free to use by others*/
   msgRet = OS_LYRG_recvMsg(&dbP->activeWritersQueuemap[index], (UINT8 *)&tmpMsg, &tmpMsgLen, 1000);
   OS_LYRG_lockMutex(&writerSchedulerDb.mutex);
   dbP->activeWritersBitmap |=  (1 << index); //mark writer as active

   #ifdef SEQ_MNGRP_WRITER_GRANT_MUTEX_DEBUG
      printf(GREEN("From thread: %lu, locked mutex %d, return value: %d, activeWritersBitmap = 0x%x\n"), tid, index, msgRet, dbP->activeWritersBitmap);
   #endif

   return ret;
}

/* This function is currently not used anywhere */
static ERRG_codeE SEQ_MNGR_unlockWriterGrantQueue(SEQ_MNGRP_channelWritersT *dbP, UINT32 index)
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   int msgRet = 0, tmpMsg = 0, tmpMsgLen = 1;
   pthread_t tid = pthread_self();

   msgRet = OS_LYRG_sendMsg(&dbP->activeWritersQueuemap[index], (UINT8 *)&tmpMsg, tmpMsgLen);
   dbP->activeWritersBitmap &= ~(1 << index); //mark writer as inactive

   #ifdef SEQ_MNGRP_WRITER_GRANT_MUTEX_DEBUG
      printf(GREEN("From thread: %lu, unlocked queue %d, return value: %d\n"), tid, index, msgRet);
   #endif

   return ret;
}

static ERRG_codeE SEQ_MNGR_unlockWriterGrantQueue_All(SEQ_MNGRP_channelWritersT *dbP)
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   int msgRet = 0, tmpMsg = 0, tmpMsgLen = 1;
   uint32_t i;
   pthread_t tid = pthread_self();
   UINT8 temp = 0; 
   UINT32 temp1 = 1;

   #ifdef SEQ_MNGRP_WRITER_GRANT_MUTEX_DEBUG
      printf(CYAN("SEQ_MNGR_popWriterGrantQueue_All called\n"));
   #endif

   for(i = 0; i < 32; i++)
   {
      if (dbP->activeWritersBitmap & (1 << i))
      {
         msgRet = OS_LYRG_sendMsg(&dbP->activeWritersQueuemap[i], (UINT8 *)&tmpMsg, tmpMsgLen);
         dbP->activeWritersBitmap &= ~(1 << i); //mark writer as inactive
         #ifdef SEQ_MNGRP_WRITER_GRANT_MUTEX_DEBUG
            printf(GREEN("From thread: %lu, unlocked queue %d, return value: %d\n"), tid, i, msgRet);
         #endif
      }
   }
   
   return ret;
}


//TODO - needs to be without starvation...
static ERRG_codeE SEQ_MNGRP_writerSchedulerAskGrant( SEQ_MNGP_sequenceDbT *dbP, int writerId )
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   UINT32 i, grant = 0;
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
   UINT32 tryNum = 0;
#endif

   if (!dbP->isMultiGraphInj)
   {
      return ret;
   }

   UINT32 pathI ,numReadersOnPath=0, numUnusedReadersOnPath=0;
   //count how much readers fed from the same writer - once per db
   if(dbP->readersBitmap == 0)
   {
      for (pathI = 0; pathI < SEQ_MNGRG_MAX_SYSTEM_PATHS; pathI++)
      {
         if(dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] == writerId)
         {
            if ((dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_RDOUT_E] != 0xFF))//  && (dbP->path[pathI].status != SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E))
            {
               if(dbP->path[pathI].status != SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E)
               {
                  numReadersOnPath |= (1 << dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_RDOUT_E]);
                  //printf(CYAN("found on path %d writer %d reader %d numReadersOnPath 0x%x\n"),pathI,writerId,dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_RDOUT_E],numReadersOnPath );
               }
               else
               {
                  if(dbP->configDone == 0)
                  {
                     numReadersOnPath |= (1 << dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_RDOUT_E]);
                  }
                  //printf(CYAN("found UNUSED on path %d writer %d reader %d numReadersOnPath 0x%x\n"),pathI,writerId,dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_RDOUT_E],numReadersOnPath );
               }
            }
            else if ((dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E] != 0xFF))
            {
               UINT32 channelID = dbP->blockVec[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E].blockInfo[dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E]].channelId;
               if(dbP->path[pathI].status != SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E)
               {
                  numReadersOnPath |= (1 << channelID);                  
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
                  printf(CYAN("found on path %d writer %d reader %d numReadersOnPath 0x%x\n"),pathI,writerId,dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E],numReadersOnPath );
#endif
               }
               else
               {
                  if(dbP->configDone == 0)
                  {
                     numReadersOnPath |= (1 << channelID);
                  }                  
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
                  printf(CYAN("found UNUSED on path %d writer %d reader %d numReadersOnPath 0x%x\n"),pathI,writerId,dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E],numReadersOnPath );
#endif                  
               }
            }
         }
       }
   }   

  //printf("SEQ_MNGRP_writerSchedulerAskGrant 1 writer %d\n",writerId);
   for(i = 0; i < SEQ_MNGRP_WRITER_SCHED_MAX_NUM_INJECTIONS; i++)
   {
      //find entry
      if (writerSchedulerDb.channelWritersDb[i].dbP == dbP)
      {
         OS_LYRG_lockMutex(&writerSchedulerDb.mutex);
         if(dbP->readersBitmap == 0)
         {
            dbP->readersBitmap = numReadersOnPath;
         }
         //case 1 - no writers active from any seqDb
         if (!writerSchedulerDb.activeDb)
         {
            SEQ_MNGR_lockWriterGrantQueue(&writerSchedulerDb.channelWritersDb[i], writerId);
            writerSchedulerDb.channelWritersDb[i].activeCannels[0] = dbP->readersBitmap;//mark soc channel as active,  for release grant we wait for soc channel and writers done
            grant = 1;
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
            printf("%s: request grant approved writer %d\n",dbP->name,writerId);
#endif
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
            UINT64 time;
            char tmp[256]="";
            OS_LYRG_getTimeNsec(&time);
            sprintf(tmp,"%s: request grant approved writer %d time %llu\n",dbP->name,writerId,time);
            SEQ_MNGRG_addSchedulerDebugPrint(tmp);
#endif

         }
         //case 2 - some writer is active from this dbP, but not this writer
         else if ((writerSchedulerDb.activeDb == dbP) && (((writerSchedulerDb.channelWritersDb[i].activeWritersBitmap) & (1 << writerId)) == 0))
         {
            SEQ_MNGR_lockWriterGrantQueue(&writerSchedulerDb.channelWritersDb[i], writerId);
            writerSchedulerDb.channelWritersDb[i].activeCannels[0] = dbP->readersBitmap;
            grant = 1;
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
            printf("%s: request grant approved, already active but not for this writer %d active writers bitmap 0x%x\n", dbP->name, writerId, writerSchedulerDb.channelWritersDb[i].activeWritersBitmap);
#endif
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
                        //tmp[0] = '\n';
                        UINT64 time;
                        char tmp2[256]="";
                        OS_LYRG_getTimeNsec(&time);
                        sprintf(tmp2,"%s: request grant approved, already active but not for this writer %d active writers bitmap 0x%x %llu\n", dbP->name, writerId, writerSchedulerDb.channelWritersDb[i].activeWritersBitmap,time);
                        SEQ_MNGRG_addSchedulerDebugPrint(tmp2);
#endif

         }
         //case 3 - the writer we ask for is already active but now that we have mutex we can sleep while waiting it to be done
         else if ((writerSchedulerDb.activeDb == dbP) && (((writerSchedulerDb.channelWritersDb[i].activeWritersBitmap) & (1 << writerId)) != 0))
         {
            SEQ_MNGR_lockWriterGrantQueue(&writerSchedulerDb.channelWritersDb[i], writerId);
            writerSchedulerDb.channelWritersDb[i].activeCannels[0] = dbP->readersBitmap;
            grant = 1;
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
            printf("%s: request grant approved writer %d\n",dbP->name,writerId);
#endif
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
                        //tmp[0] = '\n';
                        UINT64 time;
                        char tmp3[256]="";
                        OS_LYRG_getTimeNsec(&time);
                        sprintf(tmp3,"%s: request grant approved writer %d time %llu\n",dbP->name,writerId,time);
                        SEQ_MNGRG_addSchedulerDebugPrint(tmp3);
#endif
         }

         if(grant == 1)
         {
            writerSchedulerDb.activeDb = dbP; //set db as active
         }

         OS_LYRG_unlockMutex(&writerSchedulerDb.mutex);
      break;
      }
   }

   return ret;
}

ERRG_codeE SEQ_MNGRG_writerSchedulerReleaseGrantComplete( SEQ_MNGRG_handleT handle, void *entryP )
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   SEQ_MNGRP_channelWritersT *grantEntryP = (SEQ_MNGRP_channelWritersT*)entryP;
   SEQ_MNGP_sequenceDbT* dbP = (SEQ_MNGP_sequenceDbT*)handle;


   if (!dbP->isMultiGraphInj)
   {
      return ret;
   }

   //mutex lock
   OS_LYRG_lockMutex(&writerSchedulerDb.mutex);
   SEQ_MNGR_unlockWriterGrantQueue_All(grantEntryP);
   grantEntryP->activeCannels[0] = 0;
   grantEntryP->doneWritersBitmap = 0;
   grantEntryP->doneCannels [0] = 0;
   writerSchedulerDb.activeDb = NULL;
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
   printf("%s: release grant complete\n",dbP->name);
#endif
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
   UINT64 time;
   char tmp[256]="";
   OS_LYRG_getTimeNsec(&time);
   sprintf(tmp,"%s: R grant complete %llu\n",dbP->name,time);
   SEQ_MNGRG_addSchedulerDebugPrint(tmp);
#endif

   OS_LYRG_unlockMutex(&writerSchedulerDb.mutex);

   return ret;
}


ERRG_codeE SEQ_MNGRG_writerSchedulerReleaseGrant( SEQ_MNGRG_handleT handle, int channelId, SEQ_MNGRG_doneBlockE doneBlock, void **entryP)
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   UINT32 i, grant = 0;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
   UINT32 tryNum = 0;
#endif


   if (!dbP->isMultiGraphInj)
   {
      *entryP =NULL;
      return ret;
   }


#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
      UINT64 time;
      char tmp[256]="";
      OS_LYRG_getTimeNsec(&time);
   sprintf(tmp,"%s: R Grant ch %d %llu\n",dbP->name, channelId,time);
   SEQ_MNGRG_addSchedulerDebugPrint(tmp);
#endif
   *entryP = NULL;
   for(i = 0; i < SEQ_MNGRP_WRITER_SCHED_MAX_NUM_INJECTIONS; i++)
   {
      //find the entry
      if (writerSchedulerDb.channelWritersDb[i].dbP == dbP)
      {
         if (doneBlock == SEQ_MNGRG_DONE_BLOCK_WRITER_E)
         {
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
            printf("SEQ_MNGRG_writerSchedulerReleaseGrant writer %d\n",channelId);
#endif
            writerSchedulerDb.channelWritersDb[i].doneWritersBitmap |= (1 << channelId);
         }
         else if (doneBlock == SEQ_MNGRG_DONE_BLOCK_CHANNEL_E)
         {
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
            printf("SEQ_MNGRG_writerSchedulerReleaseGrant channel %d\n",channelId);
#endif
            writerSchedulerDb.channelWritersDb[i].doneCannels[0] |= (1 << channelId);
         }
         else if (doneBlock == SEQ_MNGRG_DONE_BLOCK_BOTH_E)//forcr release in rollback cases
         {
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
            printf("SEQ_MNGRG_writerSchedulerReleaseGrant BOTH \n");
#endif
            writerSchedulerDb.channelWritersDb[i].doneCannels[0] = writerSchedulerDb.channelWritersDb[i].activeCannels[0];
            writerSchedulerDb.channelWritersDb[i].doneWritersBitmap = 0xFFFFFFFF;
            dbP->configDone = 1;
            dbP->readersBitmap = 0;
         }
          //printf("SEQ_MNGRP_writerSchedulerReleaseGrant done block = %d blockId %d doneWritersBitmap 0x%X  activeWritersBitmap 0x%X doneCh 0x%x activeCh 0x%x\n",doneBlock ,channelId, writerSchedulerDb.channelWritersDb[i].doneWritersBitmap,
           // writerSchedulerDb.channelWritersDb[i].activeWritersBitmap, writerSchedulerDb.channelWritersDb[i].doneCannels[0], writerSchedulerDb.channelWritersDb[i].activeCannels[0]);
         //mutex lock
         OS_LYRG_lockMutex(&writerSchedulerDb.mutex);
         //for release grant active writers and active channel must to be equal to done writers/channel. prevent case of soc channel done before soc writers (LUTs for example-may use only part of thr frame, DSR
         //will inflate the frame, and the soc channel will done before the writers will done. this condition will prevent the release grant

         #ifdef SEQ_MNGRP_WRITER_GRANT_MUTEX_DEBUG
            printf(CYAN("SEQ_MNGRG_writerSchedulerReleaseGrant: activeWritersBitmap = 0x%x, doneWritersBitmap = 0x%x, channelId = %d, index(i) = %d \n"), writerSchedulerDb.channelWritersDb[i].activeWritersBitmap, writerSchedulerDb.channelWritersDb[i].doneWritersBitmap, channelId, i);
            printf(CYAN("if statement: %d %d %d \n"), (writerSchedulerDb.activeDb == dbP), ((writerSchedulerDb.channelWritersDb[i].activeWritersBitmap & writerSchedulerDb.channelWritersDb[i].doneWritersBitmap) == writerSchedulerDb.channelWritersDb[i].activeWritersBitmap), ((writerSchedulerDb.channelWritersDb[i].activeCannels[0] & writerSchedulerDb.channelWritersDb[i].doneCannels[0]) == writerSchedulerDb.channelWritersDb[i].activeCannels[0]));
         #endif

         if ((writerSchedulerDb.activeDb == dbP) &&
            ((writerSchedulerDb.channelWritersDb[i].activeWritersBitmap & writerSchedulerDb.channelWritersDb[i].doneWritersBitmap) == writerSchedulerDb.channelWritersDb[i].activeWritersBitmap) &&
            ((writerSchedulerDb.channelWritersDb[i].activeCannels[0] & writerSchedulerDb.channelWritersDb[i].doneCannels[0]) == writerSchedulerDb.channelWritersDb[i].activeCannels[0]))
         {
            writerSchedulerDb.channelWritersDb[i].doneCannels[0] = 0;
            writerSchedulerDb.channelWritersDb[i].doneWritersBitmap = 0;
            *entryP = &writerSchedulerDb.channelWritersDb[i];

            #ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
               printf("%s: release grant\n",dbP->name);
            #endif
         }
         OS_LYRG_unlockMutex(&writerSchedulerDb.mutex);
         break;
      }
   }

   return ret;
}

static ERRG_codeE SEQ_MNGRP_writerSchedulerRegister( SEQ_MNGP_sequenceDbT *dbP )
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   UINT32 i,j, registered = 0;

   //in case of db with name not equal to "main", it means we are in multi-graph.
   //it under assuming that main exist always, and the other graphs will config after with diffarent name.
   //so mark 'isMultuGraphInj' to be '1' in this dbP and find the main and mark him as well.
   //printf("SEQ_MNGRP_writerSchedulerRegister name %s cmp %d dbP->isMultiGraphInj %d\n",dbP->name,strcmp(dbP->name,"main"),dbP->isMultiGraphInj);
   if (strcmp(dbP->name,"main") != 0)
   {
      dbP->isMultiGraphInj = 1;
      //printf("SEQ_MNGRP_writerSchedulerRegister2 isMultiGraphInj =1\n");
   }

   for(i = 0; (i < SEQ_MNGRP_WRITER_SCHED_MAX_NUM_INJECTIONS) && !registered; i++)
   {
      //check if exists in DB already
      if (writerSchedulerDb.channelWritersDb[i].dbP == dbP)
      {
         registered = 1;
      }
   }

   //entry does not exists, add it
   if (!registered)
   {
      for(i = 0; (i < SEQ_MNGRP_WRITER_SCHED_MAX_NUM_INJECTIONS) && !registered; i++)
      {
         //check if entry is not  taken in DB
         if (!writerSchedulerDb.channelWritersDb[i].dbP)
         {
            writerSchedulerDb.channelWritersDb[i].dbP = dbP;
            (writerSchedulerDb.channelWritersDb[i].dbP)->readersBitmap = 0;
            (writerSchedulerDb.channelWritersDb[i].dbP)->configDone = 0;

             if ((dbP->isMultiGraphInj) && (strcmp(writerSchedulerDb.channelWritersDb[i].dbP->name,"main") == 0))
             {
                //printf("SEQ_MNGRP_writerSchedulerRegister current dbp %s i %d \n", dbP->name,i);
                (writerSchedulerDb.channelWritersDb[i].dbP)->isMultiGraphInj = 1;
             }
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
            printf("%s added new entry at %d\n",dbP->name,i);
#endif
            registered = 1;
            break;
         }
      }
   }

   if (!registered)
   {
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
      printf("no more space for new db! %s\n",dbP->name);
#endif
      assert(0);
      ret = HW_MNGR__ERR_OUT_OF_RSRCS;
   }

   return ret;
}

static ERRG_codeE SEQ_MNGRP_writerSchedulerUnregister( SEQ_MNGP_sequenceDbT *dbP )
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   UINT32 i;

   for(i = 0; i < SEQ_MNGRP_WRITER_SCHED_MAX_NUM_INJECTIONS; i++)
   {
      //check if exists in DB already
      if (writerSchedulerDb.channelWritersDb[i].dbP == dbP)
      {
#ifdef SEQ_MNGRP_WRITER_SCHED_DEBUG
         printf("%s removed from db\n",dbP->name);
#endif
         writerSchedulerDb.channelWritersDb[i].dbP = NULL;
         writerSchedulerDb.channelWritersDb[i].activeWritersBitmap = 0;
         memset(writerSchedulerDb.channelWritersDb[i].activeCannels, 0, sizeof(writerSchedulerDb.channelWritersDb[i].activeCannels));
         writerSchedulerDb.channelWritersDb[i].doneWritersBitmap= 0;
         memset(writerSchedulerDb.channelWritersDb[i].doneCannels, 0, sizeof(writerSchedulerDb.channelWritersDb[i].doneCannels));
      }
   }
   return ret;
}

static ERRG_codeE SEQ_MNGRP_writerSchedulerInit(  )
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;

   memset(&writerSchedulerDb,0,sizeof(writerSchedulerDb));
   OS_LYRG_aquireMutex(&writerSchedulerDb.mutex);

   SEQ_MNGR_initGrantQueues();

   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRG_pauseChannelPaths
*
*  Description: reconfigure a given writer paths. does not recofigure dma
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/

ERRG_codeE SEQ_MNGRG_pauseChannelPaths( SEQ_MNGRG_handleT handle, SEQ_MNGRG_channelH chH)
{
   SEQ_MNGP_sequenceDbT    *dbP = (SEQ_MNGP_sequenceDbT*)handle;
   ERRG_codeE               ret = HW_MNGR__RET_SUCCESS;
   UINT8                    instanceList[4];
   UINT8                    instanceListPosition = 0, blockI,pathI;
   UINT32                   rdList = 0;
   CDE_MNGRG_channnelInfoT *readerH=NULL;
   void                    *arg = NULL, *grantEntry;
   SEQ_MNGP_channelDbT     *channelP = (SEQ_MNGP_channelDbT*)chH;

   memset(instanceList,0xFF,sizeof(instanceList));
   //31.5 - we want to remove the following, and make the for loopbelow cover the closer of the readout also,
   //but since we do a big change - keep this and do it later
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
         char tmp2[256];
        UINT64 time;
        OS_LYRG_getTimeNsec(&time);
        sprintf(tmp2,"%s: p s %llu\n",dbP->name,time);
       SEQ_MNGRG_addSchedulerDebugPrint(tmp2);
#endif

   if (channelP->outputBlock == SEQ_MNGRG_XML_BLOCK_RDOUT_E)
   {
      OS_LYRG_lockMutex(&writerSchedulerDb.mutex);
      rdList = dbP->readersBitmap;
      OS_LYRG_unlockMutex(&writerSchedulerDb.mutex);

      for (int i =0; i < SEQ_MNGRG_MAX_CHANNEL_NUM; i++)
      {
         if (rdList & (1 << i))
         {
            //printf("!!!!!!!!!!!!!!!!SEQ_MNGRG_pauseChannelPaths disable rdout %d dbP->readersBitmap 0x%x\n",i,dbP->readersBitmap);
            instanceList[0] = i;
            instanceList[1] = 0xFF;
            PPE_MNGRG_setDisableAxiRd(dbP->dbH,(NUFLD_blkE)arg,instanceList,arg);
            readerH = dbP->blockVec[channelP->outputBlock].blockInfo[i].blockPrivData;
            if( readerH != NULL )
            {
               ret = CDE_MNGRG_pauseChannel(readerH);
            }
            else
            {
               printf("%s: null dma handle! %s %d\n",__func__,dbP->name,channelP->outputBlockInstance);
            }
         }
      }
   }//else - TBD CVA injection

   //disable all IDVE blocks on paths associated with this channel ID
   //this loop should be after release grant, to prevent disabeling block that writer still wrting  to them. in case of soc channel done before soc writer done (LUTs)
   //the block should be start from IAE and not from DPE
   for(blockI = SEQ_MNGRG_XML_BLOCK_DPE_E ; blockI <= SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E; blockI++)
   {
      for( pathI = 0; pathI < SEQ_MNGRG_MAX_SYSTEM_PATHS; pathI++ )
      {
         if((dbP->path[pathI].status == SEQ_MNGRG_STATUS_TYPE_ENABLED_E) &&
             (dbP->path[pathI].block[blockI] != 0xFF) &&
            (dbP->path[pathI].block[channelP->outputBlock] == channelP->outputBlockInstance))
         {
            //we have case of cropping on ppu, the rd DMA done first, after, writer DMA done, and then we disabling the blocks since release grant process,
            //but pixels probably in paths. it cause to stuck in system.
            if ((blockI>=SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E)&&(blockI<=SEQ_MNGRG_XML_BLOCK_PPU_HYB_E))
            {
               continue;
            }
            //printf("found path %d with my block output, add vsc %d\n",pathI,dbP->path[pathI].block[blockI]);
            instanceList[instanceListPosition++] = dbP->path[pathI].block[blockI];
         }
      }
      if (instanceListPosition)
      {
         HCG_MNGRG_voteUnits(mapSeqToHwUnits[blockI]);
         ret = dbP->blockVec[blockI].blockOperation.disable(dbP, SEQ_MNGRP_xmlblk2nufld(blockI), instanceList, &arg);
         HCG_MNGRG_devoteUnits(mapSeqToHwUnits[blockI]);


      }
      memset(instanceList,0xFF,sizeof(instanceList));
      instanceListPosition = 0;
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: SEQ_MNGRG_reconfigWriterPaths
*
*  Description: reconfigure a given writer paths. does not recofigure dma
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE SEQ_MNGRG_reconfigWriterPaths( SEQ_MNGRG_handleT handle, int channelId, void *writerdmaP,MEM_POOLG_bufDescT *bufP, inu_data *data )
{
//#define DEBUG_TIME
//#define DEBUG_RECONFIG
#ifdef DEBUG_TIME
   char bufTimestamp[1024];
#endif
#ifdef DEBUG_RECONFIG
   char buf[1024];
#endif
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;
   UINT8       blockI=0,blockI2=0,pathI=0,unused2=0,iauInd, active = 0;
   void        *unused = NULL;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;
   CDE_MNGRG_channnelInfoT *readerH;
   UINT8       iauInstanceList[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   UINT8       iauInstanceListPosition = 0;

#ifdef DEBUG_TIME
   UINT32 timestampI = 0;
   UINT64 nsec[20];
   memset(bufTimestamp,0,sizeof(bufTimestamp));
   OS_LYRG_getTimeNsec(&nsec[timestampI++]);
#endif

   UINT32 srcPhyAddress=0;
   MEM_POOLG_getDataPhyAddr(bufP, &srcPhyAddress); 

   //printf("SEQ_MNGRG_reconfigWriterPaths isMultiGraphInj %d\n",dbP->isMultiGraphInj);
   //in multi-graph injection mode, apply reconfig mechanism. in injection without muili-graph, just generate and call injectBuf
   if (dbP->isMultiGraphInj)
   {

      SEQ_MNGRP_writerSchedulerAskGrant(dbP, channelId);

      //check if writer is active
      for (pathI=0;pathI<SEQ_MNGRG_MAX_SYSTEM_PATHS;++pathI)
      {
         if ((dbP->path[pathI].status==SEQ_MNGRG_STATUS_TYPE_ENABLED_E) &&
             (dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] == channelId))
         {
            active = 1;
         }
      }

      if (!active)
      {

      ret = SEQ_MNGRG_writerSchedulerReleaseGrant(handle, 0, SEQ_MNGRG_DONE_BLOCK_BOTH_E, &unused);
      if (ERRG_SUCCEEDED(ret) && (unused))
      {
         ret = SEQ_MNGRG_writerSchedulerReleaseGrantComplete(handle,unused);
      }
      return HW_MNGR__ERR_UNEXPECTED;
   }

   UINT32 writerID = channelId;
   /*Boolean flag to know if we're dealing with the special case of the HELSINKI CVA injector*/
   const bool HELSINKI_SpecialCVAScenario = HELSINKI_checkForSpecialCVAScenario(writerID); 

   if(HELSINKI_SpecialCVAScenario)
   {
      
      UINT32 frameIDValue = 0;
      UINT64 timestampValue = 0;
      /* Invalidate the cache because the metadata came through a DMA
         and offset the frame buffer so that we don't inject metadata into the CVA
      */

      HELSINKI_specialCaseOffsetFrameBufferAndInvalidateCache(bufP,&srcPhyAddress);
      /* This function will take care of the following:
         Decoding the metadata contained within the frame (stored in data)
         Updating frameIDValue with the frame ID to send on the next frame
         Updating timestampValue with the timestamp to send on the next frame
         Updating the metadata for the next frame to say where the image came from
      */
      HELSINKI_prepareCVAMetadataFromWriterOperateContext(writerID,&frameIDValue,&timestampValue,data);
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Updating metadata for writer %lu for frame number %lu, with timestamp %llu \n",frameIDValue,timestampValue);
      METADATA_UPDATER_updateMetadataFromWriterOperate(writerID,timestampValue);
      PPE_DRVG_updateWriterFrameId(writerID, frameIDValue); //--TODO has two issues: the value does not update the CVA index, and HCG does not vote for ppe?
      PPE_DRVG_updateWriterTimestamp(writerID,timestampValue);
   }


#ifdef DEBUG_TIME
      OS_LYRG_getTimeNsec(&nsec[timestampI++]);
#endif

      HCG_MNGRG_voteUnits(HCG_MNGRG_HW_ALL_UNITS);

      UINT32 sensorInd, calibMode;
      CALIB_iauTosensor iauToSensor;

      OS_LYRG_lockMutex(&calUpdateMutex);

      for (pathI = 0; pathI < SEQ_MNGRG_MAX_SYSTEM_PATHS; ++pathI)
      {
         if ((dbP->path[pathI].status == SEQ_MNGRG_STATUS_TYPE_ENABLED_E) &&
             (dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] == channelId))
         {
            for (blockI = SEQ_MNGRG_XML_BLOCK_IAU_E; blockI <= SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E; ++blockI)
            {
               if (dbP->path[pathI].block[blockI] != SEQ_MNGRG_INVALID_ENTRY)
               {
                  iauInd = dbP->path[pathI].block[blockI];
                  iauInd = (blockI == SEQ_MNGRG_XML_BLOCK_IAU_E) ? iauInd : iauInd + 2;
                  if (ERRG_SUCCEEDED(SEQ_MNGRP_getSensorNumAndCalibMode(dbP, iauInd, &iauToSensor)))
                  {
                     //printf("sensor %d calib %d to iau %d\n",iauToSensor.sensorId, iauToSensor.calib,iauInstanceList[pathI]);
                     if (ERRG_FAILED(lut_mngr_gpActivateSensorLut(iauInd, iauToSensor.sensorId, iauToSensor.calib)))
                     {
                        //failed to load some luts, but should not cause a total stop
                        //printf("failed to load some/all luts\n");
                     }
                  }
               }
            }
         }
      }

      OS_LYRG_unlockMutex(&calUpdateMutex);

#ifdef DEBUG_TIME
      OS_LYRG_getTimeNsec(&nsec[timestampI++]);
#endif


#ifdef DEBUG_RECONFIG
      memset(buf,0,sizeof(buf));
      sprintf(buf + strlen(buf), "REGS: \n");
#endif

      //renable output DMA channels
      for (pathI=0; pathI<SEQ_MNGRG_MAX_SYSTEM_PATHS;++pathI)
      {
         readerH = NULL;
         if ((dbP->path[pathI].status == SEQ_MNGRG_STATUS_TYPE_ENABLED_E) &&
             (dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] == channelId))
         {
            if (dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_RDOUT_E] != 0xFF)
            {
               readerH = dbP->blockVec[SEQ_MNGRG_XML_BLOCK_RDOUT_E].blockInfo[dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_RDOUT_E]].blockPrivData;
               //printf("enable soc dma: path %d, channel %d, axird instance %d, reader %p\n",i,channelId,dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_RDOUT_E],readerH);
               if( readerH != NULL )
               {
                  ret = CDE_MNGRG_resumeChannel(readerH);
               }
               else
               {
                  printf("%s: dmaH is null! %s %d\n",__func__,dbP->name,dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_RDOUT_E]);
                  return HW_MNGR__ERR_UNEXPECTED;
               }
            }
            else if ((dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E] != 0xFF) ||
                     (dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_HIST_E] != 0xFF))
            {
               //TBD CVA alternate
               continue;
            }
            else if (dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E] != 0xFF)
            {
               //printf("%s: mipi path! need to add CB %s %d\n",__func__,dbP->name,dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E]);
            }
         }
      }

#ifdef DEBUG_TIME
      OS_LYRG_getTimeNsec(&nsec[timestampI++]);
#endif

      for (pathI=0;pathI<SEQ_MNGRG_MAX_SYSTEM_PATHS;++pathI)
      {
         if ((dbP->path[pathI].status==SEQ_MNGRG_STATUS_TYPE_ENABLED_E) &&
             (dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] == channelId))
         {
            for ( blockI = SEQ_MNGRG_XML_BLOCK_INJECTION_E; blockI < SEQ_MNGRG_XML_BLOCK_DPHY_TX_E; ++blockI) //TBD - need to reorder the blocks enum and prevent write SLU blocks
            {
               if (dbP->path[pathI].block[blockI] != SEQ_MNGRG_INVALID_ENTRY)
               {
#ifdef DEBUG_RECONFIG
                  sprintf(buf + strlen(buf), "block %d instance %d, ",blockI,dbP->path[pathI].block[blockI]);
#endif
                  XMLDB_writeBlockDbToRegs(dbP->dbH, SEQ_MNGRP_xmlblk2nufld(blockI), dbP->path[pathI].block[blockI]);
               }
            }
         }
      }

#ifdef DEBUG_RECONFIG
      sprintf(buf + strlen(buf), "CFG: \n");
#endif

#ifdef DEBUG_TIME
      OS_LYRG_getTimeNsec(&nsec[timestampI++]);
#endif


      for (pathI=0;pathI<SEQ_MNGRG_MAX_SYSTEM_PATHS;++pathI)
      {
         if ((dbP->path[pathI].status==SEQ_MNGRG_STATUS_TYPE_ENABLED_E) &&
             (dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] == channelId))
         {
            for ( blockI = SEQ_MNGRG_XML_BLOCK_IAU_E; blockI < SEQ_MNGRG_XML_BLOCK_CVA_RD_E; ++blockI)
            {
               if (dbP->path[pathI].block[blockI] != SEQ_MNGRG_INVALID_ENTRY)
               {
#ifdef DEBUG_RECONFIG
                  sprintf(buf + strlen(buf), "block %d instance %d, ",blockI,dbP->path[pathI].block[blockI]);
#endif
                  ret = dbP->blockVec[blockI].blockOperation.setCfg(dbP, SEQ_MNGRP_xmlblk2nufld(blockI), dbP->path[pathI].block[blockI], unused);
                  if ((blockI == SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E) || (blockI == SEQ_MNGRG_XML_BLOCK_IAU_E))
                  {
                     iauInd = dbP->path[pathI].block[blockI];
                     iauInd = (blockI == SEQ_MNGRG_XML_BLOCK_IAU_E) ? iauInd : iauInd + 2;
                     iauInstanceList[iauInstanceListPosition++] = iauInd;
                  }
               }
            }
         }
      }

#ifdef DEBUG_TIME
      OS_LYRG_getTimeNsec(&nsec[timestampI++]);
#endif


      UINT8       instanceList[SEQ_MNGRG_MAX_SYSTEM_PATHS];
      UINT8       instanceListPosition = 0;
#ifdef DEBUG_RECONFIG
      sprintf(buf + strlen(buf), "\nENABLE: \n");
#endif

#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
               char tmp[256]="";
               UINT64 time;
               OS_LYRG_getTimeNsec(&time);
               sprintf(tmp,"%s: start reconfig %llu\n",dbP->name,time);
               SEQ_MNGRG_addSchedulerDebugPrint(tmp);
#endif

      for(blockI2 = SEQ_MNGRG_XML_BLOCK_RD_E ; blockI2 >= SEQ_MNGRG_XML_BLOCK_IAU_E; blockI2-- )
      {
         blockI = orderListStart[blockI2];
         memset (instanceList,0xFF,sizeof(instanceList));
         instanceListPosition = 0;
#ifdef DEBUG_RECONFIG
         sprintf(buf + strlen(buf), "block %d ",blockI);
#endif
         for( pathI = 0; pathI < (SEQ_MNGRG_MAX_SYSTEM_PATHS); pathI++ )
         {
            if((dbP->path[pathI].status == SEQ_MNGRG_STATUS_TYPE_ENABLED_E) &&
               (dbP->path[pathI].block[blockI] != 0xFF) &&
               (dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] == channelId))
            {
#ifdef DEBUG_RECONFIG
               sprintf(buf + strlen(buf), "instance %d ",dbP->path[pathI].block[blockI]);
#endif
               unused2=pathI; //for hist
               instanceList[instanceListPosition] = dbP->path[pathI].block[blockI];
               instanceListPosition++;
            }
         }
         if (instanceListPosition)
         {
            ret = dbP->blockVec[blockI].blockOperation.enable(dbP, SEQ_MNGRP_xmlblk2nufld(blockI), instanceList, &unused2, unused);
         }
      }

#ifdef DEBUG_RECONFIG
      sprintf(buf + strlen(buf), "\n");
      printf("%s",buf);
#endif

      //XMLDB_writeFieldToReg(dbP->dbH,NU4100_IAE_OUT_CONTROL_CSC0_MODE_E,2);
      //XMLDB_writeFieldToReg(dbP->dbH,NU4100_IAE_OUT_CONTROL_CSC1_MODE_E,2);
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
                  char tmp2[256]="";
                  //UINT64 time;
                  OS_LYRG_getTimeNsec(&time);
                  sprintf(tmp2,"%s: after reconfig %llu\n",dbP->name,time);
                  SEQ_MNGRG_addSchedulerDebugPrint(tmp2);
#endif



   if (ERRG_SUCCEEDED(ret))
      {
         ret = CDE_MNGRG_generateChannelProgram((CDE_MNGRG_channnelInfoT*)writerdmaP);
         if (ERRG_SUCCEEDED(ret))
         {
            UINT64 nsec;
            OS_LYRG_getTimeNsec(&nsec);
            HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_WRITER_0_E + channelId, nsec, 1);
            ret = CDE_MNGRG_injectBuffer((CDE_MNGRG_channnelInfoT*)writerdmaP,(UINT8*)srcPhyAddress);
         }
      }


      if (ERRG_FAILED(ret))
      {

         ret = SEQ_MNGRG_writerSchedulerReleaseGrant(dbP, 0, SEQ_MNGRG_DONE_BLOCK_BOTH_E, &unused);
         if (ERRG_SUCCEEDED(ret) && (unused))
         {
            ret = SEQ_MNGRG_writerSchedulerReleaseGrantComplete(handle,unused);
         }
      }

      HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_ALL_UNITS);
   }

   else//REGULAR INJECTION(non multi-graph)
   {
      ret = CDE_MNGRG_generateChannelProgram((CDE_MNGRG_channnelInfoT*)writerdmaP);
      if (ERRG_SUCCEEDED(ret))
      {
            UINT64 nsec;
         OS_LYRG_getTimeNsec(&nsec);
         HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_WRITER_0_E + channelId, nsec, 1);
         ret = CDE_MNGRG_injectBuffer((CDE_MNGRG_channnelInfoT*)writerdmaP,(UINT8*)srcPhyAddress);
      }

   }

#ifdef DEBUG_TIME
   UINT32 z;
   OS_LYRG_getTimeNsec(&nsec[timestampI++]);
   sprintf(bufTimestamp + strlen(bufTimestamp), "%llu writer %d: Total = %llu \nDurations:\n",nsec[timestampI-1],channelId,nsec[timestampI-1] - nsec[0]);
   for ( z = 0; z < (timestampI - 1); z++)
   {
      sprintf(bufTimestamp + strlen(bufTimestamp), "%d -> %d = %llu\n",z,z+1,nsec[z+1]-nsec[z]);
   }
   printf("%s",bufTimestamp);
#endif

   //SEQ_MNGRG_showSysConfig(handle);
//   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_ALL_UNITS);
//   IAE_DRVG_dumpRegs();
//   DPE_MNGRG_dumpRegs();
//   PPE_MNGRG_dumpRegs();
//   CVA_MNGRG_dumpRegs();
//   CDE_MNGRG_showConfig();
//   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_ALL_UNITS);

   return ret ;
}



/****************************************************************************
*
*  Function Name:
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
//When performing injection with high fps, the SLU's gets stuck, to
//workaround it, disable the SLU's during configuration. TODO - debug
#define FAST_INJEC_SWITCH
bool IS_CVA_INIT = false;
#include "gme_drv.h"
ERRG_codeE SEQ_MNGRG_startChannel( SEQ_MNGRG_handleT handle, SEQ_MNGRG_xmlBlocksListT startBlock, int channelId, void* channelCB, void* pArgs, SEQ_MNGRG_channelH chH )
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   INT8        i,sensorIndex=-1;
   UINT8       blockI=0,pathI=0,pathJ=0,pathZ=0, iauInd;
   UINT8       instanceList[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   UINT8       iauInstanceList[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   UINT8       pausedPathList[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   UINT8       instanceListPosition = 0, pausedPathListCtr = 0, iauInstanceListPosition = 0;
   UINT8       cdeChId = 0;
   SEQ_MNGRG_xmlBlocksListT activeChStartBlock;
   SEQ_MNGP_channelDbT     *channelP = (SEQ_MNGP_channelDbT*)chH;
   UINT32      enDynVoting = 1;
   inu_nodeH    sensorGroupP;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;

   LOGG_PRINT(LOG_INFO_E, NULL, "-------------SEQ_MNGRG_startChannel %s (%s) %d --\n",channelP->name,dbP->name,channelId);

#ifdef FAST_INJEC_SWITCH
   IAE_DRVG_disableAllSlu();
#endif

   ret = HCG_MNGRG_allocVoter(&channelP->voterHandle, channelP->name, enDynVoting);
   if (ERRG_FAILED(ret))
      assert(0);

   if(channelId > (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER)) // means that we at CVA channel or ISP ch
      {
         if (channelId < (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER + NU4K_NUM_CVA_READERS + NU4K_NUM_DPHY_TX ))//cva
         {
            channelId -= (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER);
         }
         else//isp
         {
            channelId -= (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER + NU4K_NUM_CVA_READERS + NU4K_NUM_DPHY_TX);
         }
      }


   //find all paths with my start block
   for( pathI = 0; pathI < SEQ_MNGRG_MAX_SYSTEM_PATHS; pathI++ )
   {
      if( (dbP->path[pathI].status == SEQ_MNGRG_STATUS_TYPE_CONFIGURED_E) &&
          (dbP->path[pathI].block[startBlock] == channelId))
      {
         //assumption - can't have two paths, that uses the same slu but not the same sensor. so can just pause the slu and sensor instance of my path
         //check if path input is active from a different channel
         if (dbP->blockVec[channelP->inputBlock].blockInfo[dbP->path[pathI].block[channelP->inputBlock]].refCount != 0)
         {
            //The input refCount is not zero, which means there is anther channel that uses it.
            //So now, need to find all the paths for that channel and pause them

            //1. find the startBlock for the active channel
            //printf("found a path that uses the same input as me (I am path %d)\n",pathI);
            for ( pathJ = 0; pathJ < SEQ_MNGRG_MAX_SYSTEM_PATHS; pathJ++ )
            {
               if ( (pathI != pathJ) && (dbP->path[pathJ].status > SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E) &&
                    (dbP->path[pathI].block[channelP->inputBlock] == dbP->path[pathJ].block[channelP->inputBlock]))
               {
                  activeChStartBlock = dbP->path[pathJ].startBlock;
                  //printf("found the start block for the path: %d (activeChStartBlock = %d, instance = %d)\n",pathJ,activeChStartBlock,dbP->path[pathJ].block[activeChStartBlock]);
                  //2. now find all the paths with this start block and add them to the list
                  for ( pathZ = 0; pathZ < SEQ_MNGRG_MAX_SYSTEM_PATHS; pathZ++ )
                  {
                     if (dbP->path[pathZ].block[activeChStartBlock] == dbP->path[pathJ].block[activeChStartBlock])
                     {
                        //printf("add path: %d\n",pathZ);
                        pausedPathList[pausedPathListCtr++] = pathZ;
                     }
                  }
               }
            }
         }
      }
   }
   SEQ_MNGRP_pausePaths(dbP, pausedPathList, pausedPathListCtr);


   memset(iauInstanceList,0xFF,sizeof(iauInstanceList));
   for(i = SEQ_MNGRG_XML_BLOCK_RDOUT_E ; i >= 0; i-- )
   {
      blockI = orderListStart[i];
      memset(instanceList,0xFF,sizeof(instanceList));
      instanceListPosition = 0;
      for( pathI = 0; pathI < (SEQ_MNGRG_MAX_SYSTEM_PATHS); pathI++ )
      {
          if( (dbP->path[pathI].status == SEQ_MNGRG_STATUS_TYPE_CONFIGURED_E) &&
             (dbP->path[pathI].block[blockI] != 0xFF) &&
             (dbP->path[pathI].block[startBlock] == channelId))
         {
            //printf ("SEQ_MNGRG_startChannel: channelId %d pathID =%d blockID=%d instanceID=%d refcount= %d \n",channelId, pathI,blockI, dbP->path[pathI].block[blockI],dbP->blockVec[blockI].blockInfo[dbP->path[pathI].block[blockI]].refCount);
            if (blockI == startBlock)
            {
               cdeChId=pathI;
            }

            if (dbP->blockVec[blockI].blockInfo[dbP->path[pathI].block[blockI]].refCount == 0)
            {
               instanceList[instanceListPosition] = dbP->path[pathI].block[blockI];
               instanceListPosition++;
            }

            HCA_MNGRG_addRequirementToConsumer(channelP->consumerHandle,mapSeqToHcaHwUnits[blockI]);

            if ((blockI == SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E) || (blockI == SEQ_MNGRG_XML_BLOCK_IAU_E))
            {
               iauInd = dbP->path[pathI].block[blockI];
               iauInd = (blockI == SEQ_MNGRG_XML_BLOCK_IAU_E) ? iauInd : iauInd + 2;
               iauInstanceList[iauInstanceListPosition++] = iauInd;
            }
            else if ((blockI == SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E) || (blockI == SEQ_MNGRG_XML_BLOCK_SLU_E))
            {
               UINT8 firstSlu = dbP->path[pathI].block[blockI];
               firstSlu = (blockI == SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E) ? firstSlu : (firstSlu + IAE_DRVG_SLU2_EN_E);
               HCG_MNGRG_registerHwUnitVote(channelP->voterHandle, HCG_MNGRG_IAE);
               HCG_MNGRG_registerStartEvent(channelP->voterHandle, HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_START_ISR_E + firstSlu);
               HCG_MNGRG_registerHwUnitDownEvent(channelP->voterHandle, HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_END_ISR_E + firstSlu, 1,0);
               pausedPathList[pausedPathListCtr++] = pathI;
            }
            else if (blockI == SEQ_MNGRG_XML_BLOCK_INJECTION_E)
            {
               HCG_MNGRG_registerStartEvent(channelP->voterHandle, HCG_MNGRG_HW_EVENT_WRITER_0_E + dbP->path[pathI].block[blockI]);
               HCG_MNGRG_registerHwUnitUpEvent(channelP->voterHandle, HCG_MNGRG_HW_EVENT_WRITER_0_E + dbP->path[pathI].block[blockI]);
               sensorIndex = 0;
               HCA_MNGRG_addInputClockOfConsumer(channelP->consumerHandle,500); //full throughput for injection
            }
            else if (blockI == SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E)
            {
               // Need this while checking active and done channels in GrantRelease.
               dbP->blockVec[blockI].blockInfo[dbP->path[pathI].block[blockI]].channelId = channelId + (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER);
            }

            if( (blockI == SEQ_MNGRG_XML_BLOCK_GEN_E) || (blockI == channelP->inputBlock )) //the last block either generator or sensor
            {
               LOGG_PRINT(LOG_INFO_E, NULL, "Path No.%02d is enabled\n", pathI);
               if(pathI == 3)
               {
                  IS_CVA_INIT = true;
                  printf("set IS_CVA_INIT as true\n");
               }

               dbP->path[pathI].status = SEQ_MNGRG_STATUS_TYPE_ENABLED_E;
               if (blockI != SEQ_MNGRG_XML_BLOCK_INJECTION_E)
               {
                  //activate one timer according to sensor group instance
                  ret = inu_graph__getOrigSensGroup(pArgs, &sensorGroupP);
                  if (ERRG_SUCCEEDED(ret))
                  {
                     sensorIndex = inu_sensors_group__getId(sensorGroupP);
                     HCA_MNGRG_addInputClockOfConsumer(channelP->consumerHandle,inu_sensors_group__getPixelClock(sensorGroupP));
                  }
               }
            }
            dbP->blockVec[blockI].blockInfo[dbP->path[pathI].block[blockI]].refCount++;

            if (blockI == SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E)//in case of mipi, we should register to cb in vsc csi level and not phy level. for example on vc, ew have 2 vsc csi, each should has cb.
            {
               dbP->path[pathI].channelCb = channelCB;
               dbP->path[pathI].cbArgs = pArgs;
            }
         }
      }

      if ((blockI == SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E) && (sensorIndex != -1))
      {
         HCA_MNGRG_calcRequiredCiifClock();
         HCG_MNGRG_startVoterWithTimer(channelP->voterHandle, sensorIndex);
         sensorIndex = -1;
      }
      else if ((blockI == SEQ_MNGRG_XML_BLOCK_INJECTION_E) && (sensorIndex == 0))
      {
         HCG_MNGRG_startVoter(channelP->voterHandle);
         sensorIndex = -1;
      }

      if (instanceListPosition > 0 )
      {
         if (blockI != SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E)//in case of mipi, we should register to cb in vsc csi level and not phy level. for example on vc, ew have 2 vsc csi, each should has cb.
         {
            dbP->path[cdeChId].channelCb = channelCB;
            dbP->path[cdeChId].cbArgs = pArgs;
         }
         //for injection, enable will be done from reconfig path

         if (dbP->isMultiGraphInj)
         {
            if ((channelP->inputBlock == SEQ_MNGRG_XML_BLOCK_INJECTION_E) &&
                ((blockI <= SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E) && (blockI >= SEQ_MNGRG_XML_BLOCK_IAU_E) && (blockI != SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E)))
                continue;
         }
         //printf ("SEQ_MNGRG_startChannel cdeChId = %d, channelCB= %p  pArgs = %p \n",cdeChId,channelCB,pArgs);
         //printf("Enable blocks %s: ", SEQ_MNGRP_blkType2string(blockI));
         //for (int i = 0; i < instanceListPosition; i++)
         //   printf("%d ",instanceList[i]);
         //printf("\n");

         HCG_MNGRG_voteUnits(mapSeqToHwUnits[blockI]);
         if (blockI == SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E)
         {
            ret = dbP->blockVec[blockI].blockOperation.enable(dbP, SEQ_MNGRP_xmlblk2nufld(blockI), instanceList, pArgs, channelP->voterHandle);
         }
         else if ((blockI != SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E) && (blockI != SEQ_MNGRG_XML_BLOCK_SLU_E))
         {
            ret = dbP->blockVec[blockI].blockOperation.enable(dbP, SEQ_MNGRP_xmlblk2nufld(blockI), instanceList, &cdeChId, channelP->voterHandle);
         }
         HCG_MNGRG_devoteUnits(mapSeqToHwUnits[blockI]);
      }
   }

   //Load luts. For injection in multigraph mode, we load on operate, for injection on non multigraph, activate LUTs here
   if ((channelP->inputBlock != SEQ_MNGRG_XML_BLOCK_INJECTION_E) || (!dbP->isMultiGraphInj))
   {
      UINT32 sensorInd, calibMode;
      CALIB_iauTosensor iauToSensor;
      for ( i = 0; i < iauInstanceListPosition; i++)
      {
         if (iauInstanceList[i] != 0xFF)
         {
            memset(&iauToSensor,0xff,sizeof(iauToSensor));
            ret = SEQ_MNGRP_getSensorNumAndCalibMode(dbP, iauInstanceList[i], &iauToSensor);
            if (ERRG_SUCCEEDED(ret))
            {
               //printf("sensor %d calib %d to iau %d\n",iauToSensor.sensorId, iauToSensor.calib,iauInstanceList[i]);
               ret = lut_mngr_gpActivateSensorLut(iauInstanceList[i], iauToSensor.sensorId, iauToSensor.calib);
               if (ERRG_FAILED(ret))
               {
                  //failed to load some luts, but should not cause a total stop
                  //printf("failed to load some/all luts\n");
               }
            }
         }
      }
   }
   else
   {
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
         char tmp[256]="";
         UINT64 time;
         OS_LYRG_getTimeNsec(&time);
      sprintf(tmp,"startCh before R grant ch %d %llu\n",channelId,time);
      SEQ_MNGRG_addSchedulerDebugPrint(tmp);
#endif
      if ((startBlock == SEQ_MNGRG_XML_BLOCK_RDOUT_E) || ( startBlock == SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E)|| ( startBlock == SEQ_MNGRG_XML_BLOCK_DPHY_TX_E))
      {
         void *grantEntry;
         ret = SEQ_MNGRG_writerSchedulerReleaseGrant(handle, 0, SEQ_MNGRG_DONE_BLOCK_BOTH_E,&grantEntry);
         if (grantEntry)
         {
            SEQ_MNGRG_pauseChannelPaths(handle, chH);
            ret = SEQ_MNGRG_writerSchedulerReleaseGrantComplete(handle, grantEntry);
         }
      }
   }

#ifdef FAST_INJEC_SWITCH
   IAE_DRVG_enableAllSlu();
#endif

   SEQ_MNGRP_resumePaths(dbP, pausedPathList, pausedPathListCtr);

   //SEQ_MNGRG_showSysConfig(handle);
//   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_ALL_UNITS);
//   IAE_DRVG_dumpRegs();
//   DPE_MNGRG_dumpRegs();
//   PPE_MNGRG_dumpRegs();
//   CVA_MNGRG_dumpRegs();
//   CDE_MNGRG_showConfig();
//   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_ALL_UNITS);
   return ret ;
}


/****************************************************************************
*
*  Function Name:
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE SEQ_MNGRG_configChannel( SEQ_MNGRG_handleT handle, SEQ_MNGRG_xmlBlocksListT startBlock, int channelId, BOOLEAN configSensor, inu_function *nodeFunction, SEQ_MNGRG_channelH *chH )
{
   ERRG_codeE           ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   int                  i=0, injectGrant = 0;
   int                  blockI=0;
   UINT32               cdeChannelH=0;
   SEQ_MNGP_channelDbT  *channelP;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;

   //HCG_MNGRG_voteUnits(HCG_MNGRG_HW_ALL_UNITS);


   //find free channel
   for (i = 0; i < SEQ_MNGRG_MAX_CHANNEL_NUM; i++)
   {
      if (!channelDb[i].used)
      {
         memset(&channelDb[i],0,sizeof(SEQ_MNGP_channelDbT));
         channelDb[i].used = 1;
         *chH = &channelDb[i];
         channelP = (SEQ_MNGP_channelDbT*)&channelDb[i];

         char str[3] = {0};
         char str2[40] = {0};
         char *temp;
         sprintf(str, "%d", channelId);
         memcpy(str2,SEQ_MNGRP_blkType2string(startBlock),strlen(SEQ_MNGRP_blkType2string(startBlock)));
         temp = strcat(str2,str);
         temp = strcat(str2," ");
         temp = strcat(temp, dbP->name);
         memcpy(channelP->name,temp,strlen(temp));
         break;
      }
   }

   if (i == SEQ_MNGRG_MAX_CHANNEL_NUM)
      assert(0);

   channelP->outputBlock = startBlock;
   channelP->outputBlockInstance = channelId;

   HCA_MNGRG_allocConsumer(&channelP->consumerHandle, channelP->name);
   HCG_MNGRG_startPrimePeriod();

#ifdef FAST_INJEC_SWITCH
   IAE_DRVG_disableAllSlu();
#endif

   LOGG_PRINT(LOG_INFO_E, NULL, "------ Config Channel %s (%s) ----\n",channelP->name,dbP->name);
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
   char tmp[256]="";
   UINT64 time;
   OS_LYRG_getTimeNsec(&time);
   sprintf(tmp,"configCh bef ask grant ch %d %llu\n",channelId,time);
   SEQ_MNGRG_addSchedulerDebugPrint(tmp);
#endif

   if(channelId > (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER)) // means that we at CVA channel or ISP ch
   {
      if (channelId < (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER + NU4K_NUM_CVA_READERS + NU4K_NUM_DPHY_TX ))//cva
      {
         channelId -= (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER);
      }
      else//isp
      {
         channelId -= (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER + NU4K_NUM_CVA_READERS + NU4K_NUM_DPHY_TX);
      }
   }


   for (i=0;i<SEQ_MNGRG_MAX_SYSTEM_PATHS;++i)
   {
      if (dbP->path[i].status==SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E &&
         (   (dbP->path[i].block[startBlock] == channelId)))
      {
         //save the input block type. assumption - can't have a channel with different inputs (sensors + inject)
         if (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E] != SEQ_MNGRG_INVALID_ENTRY)
         {
            channelP->inputBlock = SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E;
         }
         else if (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] != SEQ_MNGRG_INVALID_ENTRY)
         {
            channelP->inputBlock = SEQ_MNGRG_XML_BLOCK_INJECTION_E;
            channelP->inputBlockInstance = dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E];
            if (!injectGrant)
            {
               SEQ_MNGRP_writerSchedulerAskGrant(dbP, channelP->inputBlockInstance);
            }
            injectGrant = 1;
         }

         for (blockI=SEQ_MNGRG_XML_BLOCK_IAU_E;blockI<SEQ_MNGRG_XML_BLOCK_LAST_E;++blockI)
         {
            if (dbP->path[i].block[blockI] != SEQ_MNGRG_INVALID_ENTRY && dbP->blockVec[blockI].blockInfo[dbP->path[i].block[blockI]].refCount == 0)
            {
               //printf ("SEQ_MNGRG_configChannel: pathID =%d channelId %d blockID=%d (%s) instanceID =%d \n",i,channelId,blockI,SEQ_MNGRP_blkType2string(blockI), dbP->path[i].block[blockI]);
               /* We first write registers DB as given from XML. the cfg&en functions per each block might overwrite the registers */
               HCG_MNGRG_voteUnits(mapSeqToHwUnits[blockI]);
               XMLDB_writeBlockDbToRegs(dbP->dbH, SEQ_MNGRP_xmlblk2nufld(blockI), dbP->path[i].block[blockI]);
               HCG_MNGRG_devoteUnits(mapSeqToHwUnits[blockI]);
            }
         }

         if ((!dbP->isMultiGraphInj) && (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] != SEQ_MNGRG_INVALID_ENTRY))
         {
            XMLDB_writeBlockDbToRegs(dbP->dbH, SEQ_MNGRP_xmlblk2nufld(SEQ_MNGRG_XML_BLOCK_INJECTION_E), dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E]);
         }
      }
   }

   for (i=0;i<SEQ_MNGRG_MAX_SYSTEM_PATHS;++i)
   {
      if (dbP->path[i].status==SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E &&
         (   (dbP->path[i].block[startBlock] == channelId)))
      {
         for (blockI=0;blockI<SEQ_MNGRG_XML_BLOCK_LAST_E;++blockI)
         {
            if (dbP->path[i].block[blockI] != SEQ_MNGRG_INVALID_ENTRY && dbP->blockVec[blockI].blockInfo[dbP->path[i].block[blockI]].refCount == 0)
            {
               if (((blockI == SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E) && (configSensor == 1)) || blockI!=SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E)
               {
                  //printf ("SEQ_MNGRG_configChannel: pathID =%d channelId %d blockID=%d (%s) instanceID =%d \n",i,channelId,blockI,SEQ_MNGRP_blkType2string(blockI), dbP->path[i].block[blockI]);
                  HCG_MNGRG_voteUnits(mapSeqToHwUnits[blockI]);
                  if (blockI == SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E )
                  {
                     ret = dbP->blockVec[blockI].blockOperation.setCfg(dbP, SEQ_MNGRP_xmlblk2nufld(blockI), dbP->path[i].block[blockI], nodeFunction);
                  }
                  else
                  {
                     ret = dbP->blockVec[blockI].blockOperation.setCfg(dbP, SEQ_MNGRP_xmlblk2nufld(blockI), dbP->path[i].block[blockI], &cdeChannelH);
                  }
                  HCG_MNGRG_devoteUnits(mapSeqToHwUnits[blockI]);
                  dbP->path[i].status = SEQ_MNGRG_STATUS_TYPE_CONFIGURED_E;
                  dbP->path[i].startBlock = startBlock;
                  if(ERRG_SUCCEEDED(ret))
                  {
                     if( (SEQ_MNGRG_xmlBlocksListT)blockI == SEQ_MNGRG_XML_BLOCK_RDOUT_E ||
                         (SEQ_MNGRG_xmlBlocksListT)blockI == SEQ_MNGRG_XML_BLOCK_INJECTION_E ||
                         (SEQ_MNGRG_xmlBlocksListT)blockI == SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E )
                     {
                        dbP->blockVec[blockI].blockInfo[dbP->path[i].block[blockI]].blockPrivData = (CDE_MNGRG_channnelInfoT *)cdeChannelH;

                        if ((SEQ_MNGRG_xmlBlocksListT)blockI == SEQ_MNGRG_XML_BLOCK_RDOUT_E
						    && SEQ_MNGRG_isExtIntChannel(dbP, dbP->path[i].block[blockI]))
                        {
                           //printf("Config extended interleaving for path%d channel[%d]: dma info at 0x%x\n", i, dbP->path[i].block[blockI], cdeChannelH);
                           CDE_MNGRG_setupExtIntInfo((CDE_MNGRG_channnelInfoT *)cdeChannelH, dbP->path[i].block[blockI]);
                        }
                           //printf("Channel%d instance%d blockPrivData: 0x%x\n", channelId, dbP->path[i].block[blockI], cdeChannelH);
                     }
                     LOGG_PRINT(LOG_DEBUG_E, NULL, "DMA configured for path %d, DMA \n", i);

                     if((SEQ_MNGRG_xmlBlocksListT)blockI == SEQ_MNGRG_XML_BLOCK_INJECTION_E)
                     {
                        //save if injected image is interleave or not
                        //find on channel any writer. ask for input, check if it realy image
                        //count how much output of type image there is to this image.
                        //from here conclude if the source image is interleave or not

                        inu_nodeH writerP, imageP,output;
                        int cnt = 0;
                        ret = inu_graph__getOrigWriters(nodeFunction,&writerP);
                        if(ERRG_SUCCEEDED(ret))
                        {
                           imageP = inu_node__getNextInputNode(writerP, NULL);
                           //check if there is bind between writer to image
                           if (inu_ref__instanceOf(imageP, INU_IMAGE_REF_TYPE))
                           {
                              output = inu_node__getNextOutputNode(imageP,NULL);
                              if (inu_ref__instanceOf(output, INU_SOC_WRITER_REF_TYPE))
                              {
                                 cnt++;
                              }
                              while (output)
                              {
                                 output = inu_node__getNextOutputNode(imageP,output);
                                 if (output)
                                 {
                                    if (inu_ref__instanceOf(output, INU_SOC_WRITER_REF_TYPE))
                                    {
                                       cnt++;
                                    }
                                 }
                              }
                              if (cnt == 2)
                              {
                                 //printf("INTERLEAVE dbP %p\n",dbP);
                                 dbP->injectInterleaveImage = 1;

                              }
                              else
                              {
                                //printf("NON INTERLEAVE ,dbP\n",dbP);
                                 dbP->injectInterleaveImage = 0;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }

#ifdef FAST_INJEC_SWITCH
   IAE_DRVG_enableAllSlu();
#endif

//   SEQ_MNGRG_showSysConfig();
// IAE_DRVG_dumpRegs();
// DPE_MNGRG_dumpRegs();
// PPE_MNGRG_dumpRegs();
// CVA_MNGRG_dumpRegs();
//   CDE_MNGRG_showConfig();
   return ret ;
}


/****************************************************************************
*
*  Function Name:
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE SEQ_MNGRG_stopChannel( SEQ_MNGRG_handleT handle, SEQ_MNGRG_xmlBlocksListT startBlock, int channelId, inu_function *nodeFunction, SEQ_MNGRG_channelH chH)
{
   ERRG_codeE  ret = HW_MNGR__RET_SUCCESS;                           // TODO: build error codes for sequencer
   UINT8 pathI=0, i=0, pathToClose, pathToCloseCntr = 0, inst = 0;
   UINT8 pathList[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   SEQ_MNGRG_xmlBlocksListT blockI=0;
   UINT8                   instanceList[SEQ_MNGRG_MAX_SYSTEM_PATHS];
   UINT8                   instanceListPosition = 0;
   SEQ_MNGP_channelDbT     *channelP = (SEQ_MNGP_channelDbT*)chH;
   SEQ_MNGP_sequenceDbT    *dbP = (SEQ_MNGP_sequenceDbT*)handle;
   void                    *unused;

   LOGG_PRINT(LOG_INFO_E, NULL, "------------SEQ_MNGRG_stopChannel %s (%s) --\n",channelP->name,dbP->name);
   HCG_MNGRG_startPrimePeriod();

   if(channelId > (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER)) // means that we at CVA channel or ISP ch
      {
         if (channelId < (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER + NU4K_NUM_CVA_READERS + NU4K_NUM_DPHY_TX ))//cva
         {
            channelId -= (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER);
         }
         else//isp
         {
            channelId -= (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER + NU4K_NUM_CVA_READERS + NU4K_NUM_DPHY_TX);
         }
      }

   //disable all SLU's, allow pipe to clean up
   IAE_DRVG_disableAllSlu();

   if (channelP->inputBlock == SEQ_MNGRG_XML_BLOCK_INJECTION_E)
   {
      SEQ_MNGRP_writerSchedulerAskGrant( dbP, channelP->inputBlockInstance );
   }

   //first we find the sensor input to this channel, and decrease the ref count.
   //if the ref count is zero, then all the path's from all other channels which
   //start from the sensor can be stopped
   memset(pathList,0xFF,sizeof(pathList));
   for( pathI = 0; pathI < SEQ_MNGRG_MAX_SYSTEM_PATHS; pathI++ )
   {
      //go over the path's for this channel
      if((dbP->path[pathI].status == SEQ_MNGRG_STATUS_TYPE_ENABLED_E) && (dbP->path[pathI].block[startBlock] == channelId))
      {
         //printf("found a path that belongs to this channel (the path is %d)\n",pathI);
         if(dbP->path[pathI].block[channelP->inputBlock] != 0xFF)
         {
            dbP->blockVec[channelP->inputBlock].blockInfo[dbP->path[pathI].block[channelP->inputBlock]].refCount--;
            if (dbP->blockVec[channelP->inputBlock].blockInfo[dbP->path[pathI].block[channelP->inputBlock]].refCount == 0)
            {
               //printf("the sensor for this path is now not used (the path is %d)\n",pathI);
               //all the channels that receive data from this sensor are stopped, its now safe to REALLY stop them all
               //find all paths from this sensor
               for (pathToClose = 0; pathToClose < SEQ_MNGRG_MAX_SYSTEM_PATHS; pathToClose++)
               {
                  if(dbP->path[pathToClose].block[channelP->inputBlock] == dbP->path[pathI].block[channelP->inputBlock])
                  {
                     //found a path to close
                     pathList[pathToCloseCntr] = pathToClose;
                     //printf("preparing path list for closer: adding path %d(at index %d)\n",pathList[pathToCloseCntr], pathToCloseCntr);
                     pathToCloseCntr++;
                  }
               }
            }
            else  // Close blocks in path that reached ref count 0
            {
                for(i = 0 ; i < SEQ_MNGRG_XML_BLOCK_LAST_E; i++)
                {
                   blockI=orderList[i]; //close order decided by orderList array
                   inst = dbP->path[pathI].block[blockI];
                   if( (blockI != channelP->inputBlock) && (inst != 0xFF)) // we already handled SEQ_MNGRG_XML_BLOCK_SENS_E
                   {
                       (dbP->blockVec[blockI].blockInfo[inst].refCount)--;
                       if ( (dbP->blockVec[blockI].blockInfo[inst].refCount == 0) )//
                       {
                          UINT32 arg = pathI;
                          LOGG_PRINT(LOG_DEBUG_E, NULL, "SEQ_MNGRG_stopChannel: blk %d inst %d\n", blockI, inst);
                          memset (instanceList,0xFF,sizeof(instanceList));
                          instanceList[0] = inst;
                          HCG_MNGRG_voteUnits(mapSeqToHwUnits[blockI]);
                          ret = dbP->blockVec[blockI].blockOperation.disable(dbP, SEQ_MNGRP_xmlblk2nufld(blockI), instanceList, &arg);
                          HCG_MNGRG_devoteUnits(mapSeqToHwUnits[blockI]);
                       }
                   }
                }
                dbP->path[pathI].status = SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E;
                dbP->path[pathI].startBlock = 0xFF;
                LOGG_PRINT(LOG_DEBUG_E, NULL, "SEQ_MNGRG_stopChannel: -- 5 -- Stoping channel. Path %d stat %d\n", pathI, dbP->path[pathI].status);
            }
         }
      }
   }

    //now we go over the path's we collected, and stop the blocks in them
   for(i = 0 ; i < SEQ_MNGRG_XML_BLOCK_LAST_E; i++)
   {
      blockI=orderList[i];//close order decided by orderList array
      instanceListPosition = 0;
      memset (instanceList,0xFF,sizeof(instanceList));

      for( pathToClose = 0; pathToClose < (pathToCloseCntr); pathToClose++ )
      {
         //printf("try to close block type %d from path %d (ref %d) %d %d %d\n",blockI,pathList[pathToClose], dbP->blockVec[blockI].blockInfo[dbP->path[pathList[pathToClose]].block[blockI]].refCount,
         //        dbP->path[pathList[pathToClose]].status, dbP->path[pathList[pathToClose]].block[blockI], channelP->inputBlock);
         if((dbP->path[pathList[pathToClose]].status == SEQ_MNGRG_STATUS_TYPE_ENABLED_E) &&
            (dbP->path[pathList[pathToClose]].block[blockI] != 0xFF) &&
            ((dbP->blockVec[blockI].blockInfo[dbP->path[pathList[pathToClose]].block[blockI]].refCount > 0) || (blockI == channelP->inputBlock)))
         {
            if (dbP->blockVec[blockI].blockInfo[dbP->path[pathList[pathToClose]].block[blockI]].refCount)
               dbP->blockVec[blockI].blockInfo[dbP->path[pathList[pathToClose]].block[blockI]].refCount--;
            //printf ("SEQ_MNGRG_stopChannel:2 channelId %d pathID =%d blockID=%d instanceID=%d refcount= %d \n",channelId, pathI,blockI, dbP->path[pathI].block[blockI],dbP->blockVec[blockI].blockInfo[dbP->path[pathI].block[blockI]].refCount);
            if (dbP->blockVec[blockI].blockInfo[dbP->path[pathList[pathToClose]].block[blockI]].refCount == 0)
            {
               //printf("add block %d instance %d\n",blockI,dbP->path[pathList[pathToClose]].block[blockI]);
               instanceList[instanceListPosition] = dbP->path[pathList[pathToClose]].block[blockI];
               instanceListPosition++;
            }
            if( blockI == startBlock )
            {
               LOGG_PRINT(LOG_INFO_E, NULL, "Path No.%02d is disabled\n", pathList[pathToClose]);
               //printf ("----------------------SEQ_MNGRG_stopChannel:disable\n");
               //dbP->path[pathI].status = SEQ_MNGRG_STATUS_TYPE_CONFIGURED_E;
               dbP->path[pathList[pathToClose]].status = SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E;
               dbP->path[pathList[pathToClose]].startBlock = 0xFF;
            }
        }
      }
      if (instanceListPosition>0 )
      {
         //printf("Disable blocks %s: ", SEQ_MNGRP_blkType2string(blockI));
         //for (int i = 0; i < instanceListPosition; i++)
         //   printf("%d ",instanceList[i]);
         //printf("\n");
         HCG_MNGRG_voteUnits(mapSeqToHwUnits[blockI]);
         if (blockI == SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E)
         {
            ret = dbP->blockVec[blockI].blockOperation.disable(dbP, SEQ_MNGRP_xmlblk2nufld(blockI), instanceList, nodeFunction);
         }
         else
         {
            ret = dbP->blockVec[blockI].blockOperation.disable(dbP, SEQ_MNGRP_xmlblk2nufld(blockI), instanceList, NULL);
         }
         HCG_MNGRG_devoteUnits(mapSeqToHwUnits[blockI]);
      }
   }

   HCG_MNGRG_deallocVoter(channelP->voterHandle);
   HCA_MNGRG_removeConsumer(channelP->consumerHandle);

   //reenable all SLU's
   IAE_DRVG_enableAllSlu();

   if (channelP->inputBlock == SEQ_MNGRG_XML_BLOCK_INJECTION_E)
   {
      ret = SEQ_MNGRG_writerSchedulerReleaseGrant(dbP, 0, SEQ_MNGRG_DONE_BLOCK_BOTH_E, &unused);
      if (ERRG_SUCCEEDED(ret) && (unused))
      {
         ret = SEQ_MNGRG_writerSchedulerReleaseGrantComplete(handle,unused);
      }
   }
   //TODO: move to close channel, need to understand why its not called at all
   channelP->used = 0;

  //SEQ_MNGRG_showSysConfig();
   return ret ;
}



/****************************************************************************
*
*  Function Name:
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE SEQ_MNGRG_closeChannel( SEQ_MNGRG_handleT handle, int channelId)
{
   int pathI=0;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;

   for(pathI = 0; pathI< SEQ_MNGRG_MAX_SYSTEM_PATHS; pathI++ )
   {
      if (dbP->path[pathI].status != SEQ_MNGRG_STATUS_TYPE_ENABLED_E &&
          dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_RDOUT_E] == channelId)
      {
         memset(&(dbP->path[pathI]),0xFF,sizeof(SEQ_MNGRG_pathInfoT));
         dbP->path[pathI].status = SEQ_MNGRG_STATUS_TYPE_NOT_IN_USE_E;
         dbP->path[pathI].startBlock = 0xFF;
      }
      else
      {
         return HW_MNGR__ERR_UNEXPECTED;
      }
   }
   return RET_SUCCESS;
}


ERRG_codeE SEQ_MNGRG_getWriterChannel(SEQ_MNGRG_handleT handle, void **writerCHP, unsigned int chId)
{
   int pathI=0;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;

   for( pathI = 0; pathI < (SEQ_MNGRG_MAX_SYSTEM_PATHS); pathI++ )
   {
   //   printf ("SEQ_MNGRG_getWriterChannel pathI %d status %d dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] %d\n",pathI,dbP->path[pathI].status,dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E]);
      if ( (dbP->path[pathI].status == SEQ_MNGRG_STATUS_TYPE_ENABLED_E) && (dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E] == chId) )
      {
         *writerCHP = dbP->blockVec[SEQ_MNGRG_XML_BLOCK_INJECTION_E].blockInfo[dbP->path[pathI].block[SEQ_MNGRG_XML_BLOCK_INJECTION_E]].blockPrivData;
      //   printf ("SEQ_MNGRG_getWriterChannel *writerCHP %p\n",*writerCHP);
         return HW_MNGR__RET_SUCCESS;
      }
   }

   return HW_MNGR__ERR_ILLEGAL_STATE;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRG_getIspDimentions
*
*  Description:   Access to XML DB and get reader configuration
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
ERRG_codeE SEQ_MNGRG_getIspConfig( XMLDB_dbH hwDb, UINT8 ispNum, ISP_MNGRG_streamReadCfgT* cfg )
{
   ERRG_codeE     ret = HW_MNGR__RET_SUCCESS;
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_ISP_RD_E, ispNum, META_ISP_READERS_ISP_RD_0_OUT_RES_HEIGHT_E),   &cfg->height );
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_ISP_RD_E, ispNum, META_ISP_READERS_ISP_RD_0_OUT_RES_WIDTH_E  ),  &cfg->width);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_ISP_RD_E, ispNum, META_ISP_READERS_ISP_RD_0_OUT_RES_BPP_E),       &cfg->bpp);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_META_ISP_RD_E, ispNum, META_ISP_READERS_ISP_RD_0_FORMAT_E),       &cfg->format);

#ifdef TEMP_SOLUTION_BEFORE_STOP_FIXING
   UINT32 ispId = GET_ISP_NUM(ispNum);
   SEQ_MNGRP_getIspNumChannelsActivation(hwDb, ispId, &cfg->numActiveReaders);
   LOGG_PRINT(LOG_INFO_E, NULL, "width %d height %d bpp %d format %d numActiveReaders %d ispId %d ispNum %d\n",cfg->width, cfg->height, cfg->bpp, cfg->format,cfg->numActiveReaders,ispId, ispNum);
#else
   LOGG_PRINT(LOG_INFO_E, NULL, "width %d height %d bpp %d format %d ispNum %d\n",cfg->width, cfg->height, cfg->bpp, cfg->format, ispNum);
#endif


   SEQ_MNGRP_getIspFuncEnables(hwDb, ispNum, cfg);
   SEQ_MNGRP_getIspNumFramesToSkip(hwDb, ispNum, cfg);

   return ret;
}




/****************************************************************************
*
*  Function Name: SEQ_MNGRG_disableAllIdveBlocks
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE SEQ_MNGRG_disableAllIdveBlocks( SEQ_MNGRG_handleT handle )
{
   int i;
   UINT8 instanceList[SEQ_MNGRG_MAX_SYSTEM_PATHS]={0,1,2,3,4,5,6,7,8,9,10,11,0xFF};
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   UINT8 disableAll =1;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;

   for( i = 0 ; i < SEQ_MNGRG_XML_BLOCK_LAST_E; i++ )
   {
       //we don't disable the sensors because we did not enable them when writing the DB
       if ((i != SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E) && (i != SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E) )
       {
         if (mapSeqToHwUnits[i]!=0) //if HW block binded
         {
            HCG_MNGRG_voteUnits(mapSeqToHwUnits[i]);
            dbP->blockVec[i].blockOperation.disable(dbP, SEQ_MNGRP_xmlblk2nufld(i), &instanceList[0], &disableAll);
            HCG_MNGRG_devoteUnits(mapSeqToHwUnits[i]);
         }
       }
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRG_getSluInstBySenInst
*
*  Description: this function is support to auto trigger mode, to find which slu connect to specific sensor
*                     for connecting trigger from his SLU callbacks
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE SEQ_MNGRG_getSluInstBySenInst( UINT32 sensorInst, UINT32 *sluInst)
{
   UINT32     i;
   SEQ_MNGP_sequenceDbT *dbP;
   ERRG_codeE ret;

   ret = SEQ_MNGRG_findDbInAccessTableISP((SEQ_MNGRG_handleT*)&dbP, ISP_MAIN_GRAPH_NAMES);
   if (ERRG_SUCCEEDED(ret))
   {
      for ( i = 0; i < SEQ_MNGRG_MAX_SYSTEM_PATHS; i++)
      {
         if (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SENS_E] == sensorInst)
         {
            if (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SLU_E]!= 0xFF)
            {
               *sluInst = dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SLU_E] + 2;//parllel -> inst 0,1. the regular need factor 2
               LOGG_PRINT(LOG_DEBUG_E, NULL, "found for sensor inst %d slu inst %d\n",sensorInst,*sluInst);
               return HW_MNGR__RET_SUCCESS;
            }
            if (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E]!= 0xFF)
            {
               *sluInst = dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E];
               LOGG_PRINT(LOG_DEBUG_E, NULL, "found for sensor inst %d slu parallel inst %d\n",sensorInst,*sluInst);
               return HW_MNGR__RET_SUCCESS;
            }
         }
      }
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: SEQ_MNGRG_getSenInstBySluInst
*
*  Description: this function is support to auto trigger mode, to find which slu connect to specific sensor
*                     for connecting trigger from his SLU callbacks
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE SEQ_MNGRG_getSenInstBySluInst( UINT32 sluInst, UINT32 *sensorInst)
{
   UINT32     i;
   UINT8      blk;
   ERRG_codeE ret;
   SEQ_MNGP_sequenceDbT *dbP;
   ret = SEQ_MNGRG_findDbInAccessTableISP((SEQ_MNGRG_handleT*)&dbP, ISP_MAIN_GRAPH_NAMES);
   if (ERRG_SUCCEEDED(ret))
   {
      if (sluInst < 2) //slu 0-1
      {
         blk = SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E;
      }
      else // slu 2-5
      {
         blk = SEQ_MNGRG_XML_BLOCK_SLU_E;
         sluInst -= 2;
      }
      for ( i = 0; i < SEQ_MNGRG_MAX_SYSTEM_PATHS; i++)
      {

         if (dbP->path[i].block[blk] == sluInst)
         {
            if (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SENS_E]!= 0xFF)
            {
               *sensorInst = dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SENS_E];
               LOGG_PRINT(LOG_DEBUG_E, NULL, "found for slu inst %d sensor inst %d\n",sluInst,*sensorInst);
               return HW_MNGR__RET_SUCCESS;
            }
         }
      }
   }

   return ret;
}


ERRG_codeE SEQ_MNGRG_updateChannelCrop( SEQ_MNGRG_handleT handle, SEQ_MNGRG_xmlBlocksListT startBlock, int channelId, PPE_MNGRG_cropParamT *crop )
{
   ERRG_codeE           ret = HW_MNGR__RET_SUCCESS;
   INT8                 blockI=0, i;
   UINT8                ppuNum;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "Updating crop on channel %d: x/y (%d/%d), w/h (%d/%d) \n",channelId, crop->xStart, crop->yStart, crop->width, crop->height);
   OS_LYRG_lockMutex(&ppeCropMutex);

    for (i = 0;i < SEQ_MNGRG_MAX_SYSTEM_PATHS; ++i)
    {
       if ( (dbP->path[i].status == SEQ_MNGRG_STATUS_TYPE_ENABLED_E) && (dbP->path[i].block[startBlock] == channelId) )
       {
           for (blockI = 0;blockI < SEQ_MNGRG_XML_BLOCK_LAST_E; ++blockI)
           {
                if ( (blockI >= SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E) && (blockI <= SEQ_MNGRG_XML_BLOCK_PPU_HYB_E) ) // the block is PPU
                {
                    if(dbP->path[i].block[blockI] != 0xFF)
                    {
                        //printf("Found block %d, instance %d\n", blockI, dbP->path[i].block[blockI]);
                        switch(blockI)
                        {
                            case SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E:
                                ppuNum = dbP->path[i].block[blockI];
                            break;
                            case SEQ_MNGRG_XML_BLOCK_PPU_SCL_E:
                                ppuNum = dbP->path[i].block[blockI] + SEQ_MNGRG_NUM_PPUS_NOSCL;
                            break;
                            case SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E:
                                ppuNum = dbP->path[i].block[blockI] + SEQ_MNGRG_NUM_PPUS_NOSCL + SEQ_MNGRG_NUM_PPUS_SCL;
                            break;
                            case SEQ_MNGRG_XML_BLOCK_PPU_HYB_E:
                                ppuNum = dbP->path[i].block[blockI] + SEQ_MNGRG_NUM_PPUS_NOSCL + SEQ_MNGRG_NUM_PPUS_SCL + SEQ_MNGRG_NUM_PPUS_HYBSCL;
                            break;
                            default:
                                OS_LYRG_unlockMutex(&ppeCropMutex);
                                return HW_MNGR__ERR_UNEXPECTED;
                            break;
                        }
                        LOGG_PRINT(LOG_DEBUG_E, NULL, "Found block %d, instance %d. PPU num %d\n", blockI, dbP->path[i].block[blockI], ppuNum);
                        ret = PPE_MNGRG_updatePPUCrop(crop, ppuNum);
                    }
                }
                else if (blockI == SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E)
                {
                   if(dbP->path[i].block[blockI] != 0xFF)
                   {
                      ret = DEPTH_POSTG_updateDPPCrop((DEPTH_POSTG_cropParamT*)crop, dbP->path[i].block[blockI]);
                   }

                }
           }
       }
    }

    PPE_MNGRG_setRegistersReady();
    OS_LYRG_unlockMutex(&ppeCropMutex);

    return ret;
}


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
void SEQ_MNGRG_showSysConfig(SEQ_MNGRG_handleT handle)
{
   UINT8                i = 0, j = 0;
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;

   printf("-------------------\n");
   printf("----Show System configuration:  \n");
   printf("-------------------\n");

   printf( "Path Modules:");

   for( j = 0; j < SEQ_MNGRG_XML_BLOCK_LAST_E; j++ )
   {
      printf( "\t %02d", j);
   }

   printf( "\n" );
   printf("-------------\n");

   for( i = 0; i < SEQ_MNGRG_MAX_SYSTEM_PATHS; i++ )
   {
      printf( "Path No %d:", i );

      for( j = 0; j < SEQ_MNGRG_XML_BLOCK_LAST_E; j++ )
      {
         if (dbP->path[i].block[j]!=0xFF)
         {
            printf( "\t%x,%d", dbP->path[i].block[j], dbP->blockVec[j].blockInfo[dbP->path[i].block[j]].refCount);
         }
         else
         {
            printf( "\t%x,0", dbP->path[i].block[j]);
         }
      }
      printf( "\n" );
   }
   printf("-------------------\n");

}

XMLDB_dbH  SEQ_MNGRG_getXmlDb( SEQ_MNGRG_handleT handle )
{
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;
   return dbP->dbH;
}

char *SEQ_MNGRG_getName( SEQ_MNGRG_handleT handle )
{
   SEQ_MNGP_sequenceDbT *dbP = (SEQ_MNGP_sequenceDbT*)handle;
   return dbP->name;
}



ERRG_codeE SEQ_MNGRG_configIspFe(UINT32 sensor_num, void* node, int* isp_num)
{
    ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
	SEQ_MNGP_sequenceDbT* dbP = NULL;
	UINT32 sluWidth, sluHeight, sluInst, i = 0, j=0;
	SEQ_MNGP_blockOperationT* oper;
	UINT8 inst[NU4K_MAX_DB_META_PATHS] = {0xFF};
	UINT8 pathNum = 0xFF;
	SENSORS_MNGRG_sensorInfoT *senInfo = NULL;

   ret = SEQ_MNGRG_findDbInAccessTableISP((SEQ_MNGRG_handleT*)&dbP, ISP_MAIN_GRAPH_NAMES);
    if(ERRG_FAILED(ret))
    {
      ret = HW_MNGR__ERR_UNEXPECTED;
      LOGG_PRINT(LOG_ERROR_E, ret, "Failed to find Main graph  \n");
    }

    for(i = 0; i < SEQ_MNGRG_MAX_SYSTEM_PATHS; i++)
    {
       if((dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E] != 0xFF) && (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SENS_E] == sensor_num))
       {
         *isp_num = GET_ISP_NUM(dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E]);
         pathNum = i;
         break;
       }
    }
	// TODO: For generalization add other 4 SLUs
	senInfo = SENSORS_MNGRG_getSensorInfo((INU_DEFSG_senSelectE)sensor_num);
	ret = SEQ_MNGRG_getSluInstBySenInst( sensor_num, &sluInst);
	ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_SLU_PARALLEL_E, sluInst ,NU4100_IAE_SLU0_FRAME_SIZE_HORZ_E  ),   &sluWidth);
	ret = XMLDB_getValue( dbP->dbH, NUFLD_calcPath( NUFLD_SLU_PARALLEL_E, sluInst ,NU4100_IAE_SLU0_FRAME_SIZE_VERT_E  ),   &sluHeight);
	senInfo->sensorCfg.senWidth = sluWidth + 1;
	senInfo->sensorCfg.senHeight= sluHeight + 1;

	LOGG_PRINT(LOG_DEBUG_E, ret, "Config blocks on path %d:\n",pathNum);
    for(j = 0; j < SEQ_MNGRG_XML_BLOCK_LAST_E; j++)
    {
    	if( dbP->path[pathNum].block[j] != 0xFF)
    	{
			if(j == SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E)
			{
				dbP->blockVec[j].blockOperation.setCfg(dbP, SEQ_MNGRP_xmlblk2nufld(j), dbP->path[pathNum].block[j], node);
			}
			else if(j == SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E)
			{
				continue;
			}
			else
			{
				dbP->blockVec[j].blockOperation.setCfg(dbP, SEQ_MNGRP_xmlblk2nufld(j), dbP->path[pathNum].block[j], NULL);
			}
    	}
    }

	LOGG_PRINT(LOG_DEBUG_E, ret, "ISP FE config Done\n");

}

ERRG_codeE SEQ_MNGRG_enableIspFe(UINT32 sensor_num, void* node)
{
    ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
	SEQ_MNGP_sequenceDbT* dbP = NULL;
	UINT32 i = 0, j=0;
	SEQ_MNGP_blockOperationT* oper;
	UINT8 inst[NU4K_MAX_DB_META_PATHS] = {0xFF};
	UINT8 pathNum = 0xFF;

   ret = SEQ_MNGRG_findDbInAccessTableISP((SEQ_MNGRG_handleT*)&dbP, ISP_MAIN_GRAPH_NAMES);
    if(ERRG_FAILED(ret))
    {
		ret = HW_MNGR__ERR_UNEXPECTED;
    }

    for(i = 0; i < SEQ_MNGRG_MAX_SYSTEM_PATHS; i++)
    {
       if((dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E] != 0xFF) && (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SENS_E] == sensor_num))
       {
		  pathNum = i;
		  break;
       }
    }

	LOGG_PRINT(LOG_DEBUG_E, ret, "Enable blocks on path %d:\n",pathNum);
    for(j = SEQ_MNGRG_XML_BLOCK_LAST_E ; j > 0 ; j--)
    {
    	if( dbP->path[pathNum].block[j] != 0xFF)
    	{
			inst[0] = dbP->path[pathNum].block[j];
			if(j == SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E)
			{
				dbP->blockVec[j].blockOperation.enable(dbP, SEQ_MNGRP_xmlblk2nufld(j), &inst[0] , node, NULL);
			}
			else if (j == SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E)
			{
				continue;
			}
			else if ((j != SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E) && (j != SEQ_MNGRG_XML_BLOCK_SLU_E))
			{
				dbP->blockVec[j].blockOperation.enable(dbP, SEQ_MNGRP_xmlblk2nufld(j), &inst[0] , NULL, NULL);
			}

        }
    }

    inst[0] = dbP->path[pathNum].block[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E];
    dbP->blockVec[SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E].blockOperation.enable(dbP, SEQ_MNGRP_xmlblk2nufld(SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E), &inst[0], NULL, NULL);

	LOGG_PRINT(LOG_DEBUG_E, ret, "ISP FE enable Done\n");
	return ret;

}

ERRG_codeE SEQ_MNGRG_getIspInstance(UINT32 sensor_num, UINT32 *ispInst, XMLDB_dbH* db)
{
    ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
    SEQ_MNGP_sequenceDbT* dbP = NULL;
    UINT32 i = 0;

   ret = SEQ_MNGRG_findDbInAccessTableISP((SEQ_MNGRG_handleT*)&dbP, ISP_MAIN_GRAPH_NAMES);
    *db = SEQ_MNGRG_getXmlDb(dbP);

    for(i = 0; i < SEQ_MNGRG_MAX_SYSTEM_PATHS; i++)
    {
       if((dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E] != 0xFF) && (dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_SENS_E] == sensor_num))
       {
		  *ispInst = dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E];

		  LOGG_PRINT(LOG_DEBUG_E, ret, "Found ISP path %d:: sensor %d ISP RD %d\n", i, sensor_num, dbP->path[i].block[SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E]);
          break;
       }
    }
	return ret;
}

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
ERRG_codeE SEQ_MNGRG_findDbInAccessTable(SEQ_MNGRG_handleT *handleP, char *name)
{
   UINT32 i;
   for(i = 0; i < SEQ_MNGRG_MAX_DBS_NUM; i++)
   {
      if (!strcmp(SEQ_MNGRP_dbsP[i]->name,name))
      {
         *handleP = SEQ_MNGRP_dbsP[i];
         return HW_MNGR__RET_SUCCESS;
      }
   }
   return HW_MNGR__ERR_OUT_OF_RSRCS;
}

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
static ERRG_codeE SEQ_MNGRG_findDbInAccessTableISP(SEQ_MNGRG_handleT *handleP, char *name_1, char *name_2, char *name_3)
{
   UINT32 i;
   for(i = 0; i < SEQ_MNGRG_MAX_DBS_NUM; i++)
   {
      if (strcmp(SEQ_MNGRP_dbsP[i]->name,name_1) == 0)
      {
         *handleP = SEQ_MNGRP_dbsP[i];
         return HW_MNGR__RET_SUCCESS;
      }
      if (strcmp(SEQ_MNGRP_dbsP[i]->name,name_2) == 0)
      {
         *handleP = SEQ_MNGRP_dbsP[i];
         return HW_MNGR__RET_SUCCESS;
      }
      if (strcmp(SEQ_MNGRP_dbsP[i]->name,name_3) == 0 )
      {
         *handleP = SEQ_MNGRP_dbsP[i];
         return HW_MNGR__RET_SUCCESS;
      }
   }
   return HW_MNGR__ERR_OUT_OF_RSRCS;
}

UINT32 gpStreamNUM = 0;

void SEQ_MNGRG_updateCalibration(inu_deviceH me, inu_device__calibUpdateT *calibUpdate)
{
   OS_LYRG_lockMutex(&calUpdateMutex);

   lut_mngr_updateDblBuffId(me, calibUpdate->dblBuffId);
   gpStreamNUM = calibUpdate->dblBuffId;
   printf("Set gpStreamNUM to %d\n", gpStreamNUM);
   OS_LYRG_unlockMutex(&calUpdateMutex);
}


#ifdef __cplusplus
   }
#endif

