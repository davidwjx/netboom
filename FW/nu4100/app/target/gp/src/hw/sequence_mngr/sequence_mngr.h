
/****************************************************************************
 *
 *   FileName: sequence_mngr.h
 *
 *   Author:  Dima S.
 *
 *   Date:
 *
 *   Description: Control HW sequences
 *
 ****************************************************************************/


#ifndef __SEQUENCE_MNGR_H__

#define __SEQUENCE_MNGR_H__


#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_defs.h"
#include "nu4k_defs.h"
#include "xml_db.h"
#include "inu_function.h"
#include "isp_mngr.hpp"

#include "ppe_mngr.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define SEQ_MNGRG_MAX_SEQ_PATH_LENGTH		8
#define SEQ_MNGRG_MAX_PATHES_PER_READER	4
//#define SCHEDULER_DEBUG_PRINT_TO_BUF
//NOTE: xml block list must be aligned with the blocks in XML file
typedef enum
{
   SEQ_MNGRG_XML_BLOCK_GEN_E,                   // 0
   SEQ_MNGRG_XML_BLOCK_SENS_GROUP_E,            // 1
   SEQ_MNGRG_XML_BLOCK_SENS_E,                  // 2
   SEQ_MNGRG_XML_BLOCK_MEDIATOR_E,              // 3
   SEQ_MNGRG_XML_BLOCK_MIPI_RX_E,               // 4
   SEQ_MNGRG_XML_BLOCK_PAR_RX_E,                // 5
   SEQ_MNGRG_XML_BLOCK_INJECTION_E,             // 6
   SEQ_MNGRG_XML_BLOCK_SLU_PARALLEL_E,          // 7
   SEQ_MNGRG_XML_BLOCK_SLU_E,                   // 8
   SEQ_MNGRG_XML_BLOCK_ISP_E,                   // 9
   SEQ_MNGRG_XML_BLOCK_IAU_E,                   // 10
   SEQ_MNGRG_XML_BLOCK_IAU_COLOR_E,             // 11
   SEQ_MNGRG_XML_BLOCK_HIST_E,                  // 12
   SEQ_MNGRG_XML_BLOCK_DPE_E,                   // 13
   SEQ_MNGRG_XML_BLOCK_DEPTH_POST_E,            // 14
   SEQ_MNGRG_XML_BLOCK_PPU_NOSCL_E,             // 15
   SEQ_MNGRG_XML_BLOCK_PPU_SCL_E,               // 16
   SEQ_MNGRG_XML_BLOCK_PPU_HYBSCL_E,            // 17
   SEQ_MNGRG_XML_BLOCK_PPU_HYB_E,               // 18
   SEQ_MNGRG_XML_BLOCK_CVA_E,                   // 19
   SEQ_MNGRG_XML_BLOCK_RD_E,                    // 20
   SEQ_MNGRG_XML_BLOCK_CVA_RD_E,                // 21
   SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E,             // 22
   SEQ_MNGRG_XML_BLOCK_VSC_CSI_TX_E,            // 23
   SEQ_MNGRG_XML_BLOCK_DPHY_TX_E,               // 24
   SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E,             // 25
   SEQ_MNGRG_XML_BLOCK_RDOUT_E,                 // 26
   SEQ_MNGRG_XML_BLOCK_LAST_E,                  // 27
}SEQ_MNGRG_xmlBlocksListT;

typedef enum
{
   SEQ_MNGRG_DONE_BLOCK_WRITER_E,
   SEQ_MNGRG_DONE_BLOCK_CHANNEL_E,
   SEQ_MNGRG_DONE_BLOCK_BOTH_E,
}SEQ_MNGRG_doneBlockE;



/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef void* SEQ_MNGRG_channelH;
typedef void* SEQ_MNGRG_handleT;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/


ERRG_codeE SEQ_MNGRG_init( void );
ERRG_codeE SEQ_MNGRG_deinit( void );
ERRG_codeE SEQ_MNGRG_initDb( SEQ_MNGRG_handleT *handleP, XMLDB_dbH hwDb, const char *name );
ERRG_codeE SEQ_MNGRG_deinitDb( SEQ_MNGRG_handleT handle );
XMLDB_dbH  SEQ_MNGRG_getXmlDb( SEQ_MNGRG_handleT handle );
char      *SEQ_MNGRG_getName( SEQ_MNGRG_handleT handle );
ERRG_codeE SEQ_MNGRG_configChannel( SEQ_MNGRG_handleT handle, SEQ_MNGRG_xmlBlocksListT startBlock, int channelId, BOOLEAN configSensor,  inu_function *nodeFunction, SEQ_MNGRG_channelH *chH );
ERRG_codeE SEQ_MNGRG_startChannel( SEQ_MNGRG_handleT handle, SEQ_MNGRG_xmlBlocksListT startBlock, int channelId,void* channelCB, void* pArgs, SEQ_MNGRG_channelH chH );
ERRG_codeE SEQ_MNGRG_stopChannel( SEQ_MNGRG_handleT handle, SEQ_MNGRG_xmlBlocksListT startBlock, int channelId, inu_function *nodeFunction, SEQ_MNGRG_channelH chH);
ERRG_codeE SEQ_MNGRG_closeChannel( SEQ_MNGRG_handleT handle, int channelId );
ERRG_codeE SEQ_MNGRG_getWriterChannel( SEQ_MNGRG_handleT handle, void **writerCHP, unsigned int chId);
ERRG_codeE SEQ_MNGRG_reconfigWriterPaths( SEQ_MNGRG_handleT handle, int channelId, void *writerdmaP, MEM_POOLG_bufDescT *bufP,inu_data *data );
ERRG_codeE SEQ_MNGRG_disableAllIdveBlocks( SEQ_MNGRG_handleT handle );
ERRG_codeE SEQ_MNGRG_updateChannelCrop( SEQ_MNGRG_handleT handle, SEQ_MNGRG_xmlBlocksListT startBlock, int channelId, PPE_MNGRG_cropParamT *crop );
ERRG_codeE SEQ_MNGRG_findDbInAccessTable(SEQ_MNGRG_handleT *handleP, char *name);
void       SEQ_MNGRG_showSysConfig(SEQ_MNGRG_handleT handle);
//TODO - the following APIs are being called from trigger manager. Need to be accessed via handle
ERRG_codeE SEQ_MNGRG_getSluInstBySenInst( UINT32 sensorInst, UINT32 *sluInst);
ERRG_codeE SEQ_MNGRG_getSenInstBySluInst( UINT32 sluInst, UINT32 *sensorInst);
ERRG_codeE SEQ_MNGRG_writerSchedulerReleaseGrant( SEQ_MNGRG_handleT handle, int channelId, SEQ_MNGRG_doneBlockE doneBlock, void **entryP);
ERRG_codeE SEQ_MNGRG_writerSchedulerReleaseGrantComplete( SEQ_MNGRG_handleT handle, void *entryP );
ERRG_codeE SEQ_MNGRG_pauseChannelPaths( SEQ_MNGRG_handleT handle, SEQ_MNGRG_channelH chH);
void       SEQ_MNGRG_updateCalibration(inu_deviceH me, inu_device__calibUpdateT *calibUpdate);
ERRG_codeE SEQ_MNGRG_getIspInstance(UINT32 sensor_num, UINT32 *ispInst, XMLDB_dbH *db);
ERRG_codeE SEQ_MNGRG_configIspFe(UINT32 sensor_num, void* node, int* isp_num);
ERRG_codeE SEQ_MNGRG_enableIspFe(UINT32 sensor_num, void* node);
ERRG_codeE SEQ_MNGRG_getIspConfig( XMLDB_dbH hwDb, UINT8 ispNum, ISP_MNGRG_streamReadCfgT* cfg );
void SEQ_MNGRG_markExtIntChannel(void *handle, UINT32 chId);
BOOLEAN SEQ_MNGRG_isExtIntChannel(void *handle, UINT32 chId);

#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
void SEQ_MNGRG_addSchedulerDebugPrint(char *string);
void SEQ_MNGRG_printSchedulerDebugBuf();
#endif
#ifdef __cplusplus
}
#endif


#endif //__SEQUENCE_MNGR_H__
