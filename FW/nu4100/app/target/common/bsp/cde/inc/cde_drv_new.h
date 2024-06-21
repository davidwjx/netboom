
/****************************************************************************
 *
 *   FileName: CDE_DRV_new.h
 *
 *   Author: Dima S.
 *
 *   Date: 
 *
 *   Description: DMA driver
 *   
 ****************************************************************************/
#ifndef __CDE_DRV_H__
#define __CDE_DRV_H__

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "err_defs.h"
#include "PL330_dma_misc.h"
#include "PL330_dma_instruction.h"
#include "mem_pool.h"
#include <metadata.pb.h>
#include "metadata_cde.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define  CDE_DRVG_CH_MAX_NUM_OF_BUFFERS      15
//#define   CDE_DRVG_CH_PROGRAMM_MAX_SIZE    0x180 // need to revert it before B0
#define  CDE_DRVG_CH_PROGRAMM_MAX_SIZE    0x200

#define  CDE_DRVG_MAX_EVENTS_PER_CHANNEL     2
#define  CDE_DRVG_MAX_CHANNELS_PER_CORE   8
#define  CDE_DRVG_MAX_VIRTUAL_CHANNELS_PER_CORE   16
#define  CDE_DRVG_PERIPH_NUM_MEMORY       0x13
#define  CDE_DRVG_PERIPH_NUM_INVALID      0xFF
#define INJECTION_BURST_SIZE              (4)

#define  CDE_DRVG_VIRTUAL_CHANNELS
#define  CDE_DRVG_IMPROVED_ISR_HANDLING
#define CDE_DRVG_METADATA
#define CDE_DRVG_CVA_METADATA
#define CDE_DRVG_ENABLE_SENSOR_SYNC_AND_UPDATE
#define CDE_DRVG_METADATA_AFTER_LOOP
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
//Testing by William.Bright@imd-tec.com has shown that we cannot use than 4 buffers before we get the an overflow in backwards_jump
#define MAX_NUM_BUFFERS_LOOPS (4)
#define MAX_NUM_REGISTER_ADDRESS (4)
#endif
//Used for Extended DMA Interleaving Mode, every buffer loop allows 3 wait events(3 subChannels at most)
#define CDE_DRVG_MAX_WAIT_EVENTS   (3)
/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef enum 
{
   CDE_DRVG_CORE_NUM_0_E        = 0,
   CDE_DRVG_CORE_NUM_1_E        = 1,
   CDE_DRVG_CORE_NUM_2_E        = 2,
   CDE_DRVG_CORE_NUM_MAX_E,
   CDE_DRVG_CORE_NUM_ANY_E,
   CDE_DRVG_CORE_NUM_INVALID_E
} CDE_DRVG_coreNumE;

typedef enum 
{
   CDE_DRVG_PERIPH_DIR_INVALID_E   = 0,
   CDE_DRVG_PERIPH_DIR_RX_E        = 1,
   CDE_DRVG_PERIPH_DIR_TX_E        = 2,
} CDE_DRVG_periphDirE;

typedef enum
{
   CDE_DRVG_PERIPH_CLIENT_I2SS_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 0),
   CDE_DRVG_PERIPH_CLIENT_I2SS_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 1),
   CDE_DRVG_PERIPH_CLIENT_I2C0_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 2),
   CDE_DRVG_PERIPH_CLIENT_I2C0_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 3),
   CDE_DRVG_PERIPH_CLIENT_I2C3_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 4),
   CDE_DRVG_PERIPH_CLIENT_I2C3_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 5),
   CDE_DRVG_PERIPH_CLIENT_SPI1_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 6),
   CDE_DRVG_PERIPH_CLIENT_SPI1_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 7),
   CDE_DRVG_PERIPH_CLIENT_UART0_TX_E         =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 8),
   CDE_DRVG_PERIPH_CLIENT_UART0_RX_E         =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 9),
   CDE_DRVG_PERIPH_CLIENT_AXI_RD0_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 10),
   CDE_DRVG_PERIPH_CLIENT_AXI_RD1_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 11),
   CDE_DRVG_PERIPH_CLIENT_AXI_RD2_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 12),
   CDE_DRVG_PERIPH_CLIENT_AXI_RD3_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 13),
   CDE_DRVG_PERIPH_CLIENT_CVA0_IMG_TX_E      =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 14),
   CDE_DRVG_PERIPH_CLIENT_CVA1_IMG_TX_E      =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 15),
   CDE_DRVG_PERIPH_CLIENT_AXI_WB0_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 17),
   CDE_DRVG_PERIPH_CLIENT_CVJ0_HD_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 18),
   CDE_DRVG_PERIPH_CLIENT_IAE_HIST0_TX_E     =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 26),
   CDE_DRVG_PERIPH_CLIENT_IAE_HIST1_TX_E     =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_0_E << 8) | 27),
   CDE_DRVG_PERIPH_CLIENT_I2SM_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 0),
   CDE_DRVG_PERIPH_CLIENT_I2SM_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 1),
   CDE_DRVG_PERIPH_CLIENT_I2C1_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 2),
   CDE_DRVG_PERIPH_CLIENT_I2C1_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 3),
   CDE_DRVG_PERIPH_CLIENT_I2C4_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 4),
   CDE_DRVG_PERIPH_CLIENT_I2C4_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 5),
   CDE_DRVG_PERIPH_CLIENT_0106_E          =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 6),
   CDE_DRVG_PERIPH_CLIENT_0107_E          =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 7),
   CDE_DRVG_PERIPH_CLIENT_UART1_TX_E         =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 8),
   CDE_DRVG_PERIPH_CLIENT_UART1_RX_E         =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 9),
   CDE_DRVG_PERIPH_CLIENT_AXI_RD4_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 10),
   CDE_DRVG_PERIPH_CLIENT_AXI_RD5_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 11),
   CDE_DRVG_PERIPH_CLIENT_CVA2_IMG_TX_E      =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 14),
   CDE_DRVG_PERIPH_CLIENT_CVA3_KP_DESC_TX_E  =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 15) ,
   CDE_DRVG_PERIPH_CLIENT_AXI_WB1_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 17) ,
   CDE_DRVG_PERIPH_CLIENT_CVJ1_HD_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_1_E << 8) | 18)  ,
   CDE_DRVG_PERIPH_CLIENT_ELU_RX_E           =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 0)   ,
   CDE_DRVG_PERIPH_CLIENT_0201_E          =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 1)   ,
   CDE_DRVG_PERIPH_CLIENT_I2C2_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 2)   ,
   CDE_DRVG_PERIPH_CLIENT_I2C2_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 3)   ,
   CDE_DRVG_PERIPH_CLIENT_I2C5_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 4)   ,
   CDE_DRVG_PERIPH_CLIENT_I2C5_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 5)   ,
   CDE_DRVG_PERIPH_CLIENT_SPI0_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 6)   ,
   CDE_DRVG_PERIPH_CLIENT_SPI0_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 7)   ,
   CDE_DRVG_PERIPH_CLIENT_UART2_TX_E         =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 8)   ,
   CDE_DRVG_PERIPH_CLIENT_UART2_RX_E         =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 9)   ,
   CDE_DRVG_PERIPH_CLIENT_AXI_RD8_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 10)  ,
   CDE_DRVG_PERIPH_CLIENT_AXI_RD9_TX_E       =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 11)  ,
   CDE_DRVG_PERIPH_CLIENT_CVA4_IMG_TX_E      =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 14)	,
   CDE_DRVG_PERIPH_CLIENT_CVA5_KP_DESC_TX_E  =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 15)	,
   CDE_DRVG_PERIPH_CLIENT_CVA6_CTRL_TX_E     =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 16)	,
   CDE_DRVG_PERIPH_CLIENT_AXI_WB2_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 17)  ,
   CDE_DRVG_PERIPH_CLIENT_CVJ2_KP_RX_E       =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_2_E << 8) | 18)  ,
   CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E        =  ((CDE_DRVG_PERIPH_DIR_RX_E << 12) | (CDE_DRVG_CORE_NUM_ANY_E << 8) | CDE_DRVG_PERIPH_NUM_MEMORY),
   CDE_DRVG_PERIPH_CLIENT_MEMORY_TX_E        =  ((CDE_DRVG_PERIPH_DIR_TX_E << 12) | (CDE_DRVG_CORE_NUM_ANY_E << 8) | CDE_DRVG_PERIPH_NUM_MEMORY),
   CDE_DRVG_PERIPH_INVALID_E
}CDE_DRVG_PeriphClientIdE;

typedef enum
{
   CDE_DRVG_STATUS_STOPPED_E                 =  0  ,
   CDE_DRVG_STATUS_EXECUTING_E               =  1  ,
   CDE_DRVG_STATUS_CACHE_MISS_E              =  2  ,
   CDE_DRVG_STATUS_UPDATING_PC_E             =  3  ,
   CDE_DRVG_STATUS_WAITING_FOR_EVENT_E       =  4  ,
   CDE_DRVG_STATUS_AT_BARRIER_E              =  5  ,
   CDE_DRVG_STATUS_RESERVED0_E               =  6  ,
   CDE_DRVG_STATUS_WAITING_FOR_PERIPHERAL_E  =  7  ,
   CDE_DRVG_STATUS_KILLING_E                 =  8  ,
   CDE_DRVG_STATUS_COMPLETING_E              =  9  ,
   CDE_DRVG_STATUS_RESERVED1_E               =  10 ,
   CDE_DRVG_STATUS_RESERVED2_E               =  11 ,
   CDE_DRVG_STATUS_RESERVED3_E               =  12 ,
   CDE_DRVG_STATUS_RESERVED4_E               =  13 ,
   CDE_DRVG_STATUS_FAULTING_COMPLETING_E     =  14 ,
   CDE_DRVG_STATUS_FAULTING_E                =  15 ,
}CDE_DRVG_channelStatusE;

typedef union
{
   CDE_DRVG_PeriphClientIdE periphId;
   struct 
   {
      UINT8 periphPort;
      UINT8 core:4;
      UINT8 direction:4;
   }field;
}CDE_DRVG_PeriphClientIdU;


typedef enum
{
   CDE_DRVG_SCENARIO_TYPE_INVALID_E,
   CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_E,               // video/depth streaming with known frame size
   CDE_DRVG_SCENARIO_TYPE_STREAM_NON_FIXEDSIZE_E,           // algo streaming with unknown size
   CDE_DRVG_SCENARIO_TYPE_INJECTION_FIXEDSIZE_E, 
   CDE_DRVG_SCENARIO_TYPE_INJECTION_NON_FIXEDSIZE_E,     
   CDE_DRVG_SCENARIO_TYPE_MEMCOPY_PLAIN2PLAIN_E,      
   CDE_DRVG_SCENARIO_TYPE_MEMCOPY_CYCLIC2PLAIN_E,     
   CDE_DRVG_SCENARIO_TYPE_STREAM_CYCLIC_E,     
   CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS,
   CDE_DRVG_SCENARIO_REGISTER_TO_MEMORY_E,
   CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_EXT_E,          //Extended DMA Interleaving with know frame size
   CDE_DRVG_SCENARIO_TYPE_LAST_E
}CDE_DRVG_scenarioTypeE;



typedef  enum
{
   CDE_DRVG_INVALID_OP_MODE_E = 0,  
   CDE_DRVG_OME_SHOT_OP_MODE_E,                       // oneshot copy
   CDE_DRVG_CONTINIOUS_OP_MODE_E,                        // cyclic transfer
   CDE_DRVG_OP_MODE_LAST_E
} CDE_DRVG_chOpModeE;

typedef  enum
{
   CDE_DRVG_1D_TRANSFER_MODE_E = 0,  // none-block mode (without stride)
   CDE_DRVG_2D_TRANSFER_MODE_E = 1,  // block mode (stride is not equal to width)
   CDE_DRVG_TRANSFER_MODE_LAST_E
} CDE_DRVG_transferModeE;

typedef enum
{
   CDE_DRVG_RESOLUTION_TYPE_BINNING_E  = 0,
   CDE_DRVG_RESOLUTION_TYPE_FULL_E   = 1,
   CDE_DRVG_NUM_OF_RESOLUTION_TYPES_E
}  CDE_DRVG_resolutionTypeE;

typedef enum
{
   CDE_DRVG_FRAME_MODE_DUMMY_E           = 0,
   CDE_DRVG_FRAME_MODE_NONE_INTERLEAVE_E = 1,
   CDE_DRVG_FRAME_MODE_INTERLEAVE_E      = 2
} CDE_DRVG_frameModeE;

typedef struct
{
   CDE_DRVG_scenarioTypeE     scenarioType;
   CDE_DRVG_chOpModeE         opMode;
   CDE_DRVG_transferModeE     transferMode;
   CDE_DRVG_resolutionTypeE   resolutionType;
   CDE_DRVG_frameModeE        frameMode;  
   UINT32                  numOfBuffs;
}CDE_DRVG_streamCfgT;




typedef struct
{
   UINT32                  pixelSize;
   UINT32                  imageWidth;
   UINT32                  imageHeight;
} CDE_DRVG_imgDimensionsT;

typedef struct
{
   CDE_DRVG_imgDimensionsT       imageDim;
   UINT32                        frameStartX;
   UINT32                        frameStartY;
   UINT32                        bufferWidth;
   UINT32                        bufferHeight;
   UINT32                        bufferSize; // For ddrless scenario
   UINT32                        numLinesPerChunk;
   UINT32                        numLoopsPerLine;
   UINT32                        stride;
   UINT32                        metadata_size;
} CDE_DRVG_dimensionsCfgT;


typedef struct
{
   CDE_DRVG_PeriphClientIdE   peripheral;
   CDE_DRVG_dimensionsCfgT    dimensionCfg;
}CDE_DRVG_portCfgT;

typedef void (*CDE_DRVG_mngrEofCbT)(void*, void* );
typedef void (*CDE_DRVG_mngrChannelDoneCbT)(void*, void* );

typedef struct
{
   CDE_DRVG_mngrEofCbT        cdeMngrFrameDoneCb;
   CDE_DRVG_mngrChannelDoneCbT   cdeMngrChannelDoneCb;
}CDE_DRVG_callbackCfgT;

typedef struct
{
   UINT32                        srcAddress;
   UINT32                        dstAddress;
   UINT32                        buffSize;
}CDE_DRVG_memcpyCfgT;
typedef enum 
{
   NO_WAIT = 0,               /* Don't wait for the event */
   WAIT = 1                   /*Wait for the waitEventID */
} CDE_W_EventT;
typedef enum 
{
   NO_GIVE = 0,               /* Don't give an event */
   GIVE = 1                   /* Give the giveEventID */
} CDE_G_EventT; 
typedef enum 
{
   No_Address_Increment = 0,           
   Address_Increment = 1               
} CDE_Addr_IncrementT; 
typedef struct 
{
   UINT32                      registerAddresses[MAX_NUM_REGISTER_ADDRESS];             /*Register addresses to copy from for register->memory copies by the DMAC */ 
   UINT32                      memoryAddresses[MAX_NUM_BUFFERS_LOOPS][MAX_NUM_REGISTER_ADDRESS];      /*This stores the PC for where the MOV to the dar happens */
   UINT8                       *dstAddressProgramP_MetaData[MAX_NUM_BUFFERS_LOOPS];
   UINT8                       transferSizes[MAX_NUM_REGISTER_ADDRESS];                 /*transfer sizes to use */
   CDE_Addr_IncrementT         srcIncrement[MAX_NUM_REGISTER_ADDRESS];                  /*Src increment can be enabled or disabled per register copy  */
   CDE_Addr_IncrementT         dstIncrement[MAX_NUM_REGISTER_ADDRESS];                  /*Dest increment can be enabled or disabled per register copy */
   UINT8                       waitEventID;                                             /*We will wait for this event ID before starting the register copies*/ 
   CDE_W_EventT                waitforEvent;                                            /*You can change this to wait for an event or not*/
   UINT8                       giveEventID;                                             /*We will send this event ID after the register copies have completed */ 
   CDE_G_EventT                giveEvent;                                               /*You can change this to give an event or not */
   UINT8                       registerAddressCopiesToDo;                               /*Number of register address copies to do (5 bits) */
   UINT8                       numLoops[MAX_NUM_REGISTER_ADDRESS];                      /*Number of transfer loops to do */
   UINT8                       numBuffers;
   UINT32                      protobuf_metadata_dstAddress[MAX_NUM_BUFFERS_LOOPS];
   UINT32                      numberMetadataLines;                                     /*Number of metadata lines for this channel */
   UINT32                      frameBufferOffsetBytes;                                  /* We are sending where the frame buffer starts, so the host knows how to offset the start of frame buffer for showing images */
   const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entry;                   /*Metadata entry*/
} CDE_DRVG_regToMemoryCopyConfigParams;

typedef struct 
{
   UINT32                      registerAddresses[MAX_NUM_REGISTER_ADDRESS];                           /*Register addresses to copy from for register->memory copies by the DMAC */ 
   UINT32                      memoryAddresses[MAX_NUM_BUFFERS_LOOPS][MAX_NUM_REGISTER_ADDRESS];      /*This stores the PC for where the MOV to the dar happens */
   UINT8                       *dstAddressProgramP_MetaData[MAX_NUM_BUFFERS_LOOPS];
   UINT8                       waitEventID;                                                           /*We will wait for this event ID before starting the register copies*/ 
   UINT8                       giveEventID;                                                           /*We will send this event ID after the register copies have completed */ 
   CDE_W_EventT                waitforEvent;                                                          /*You can change this to wait for an event or not*/
   CDE_G_EventT                giveEvent;                                                             /*You can change this to give an event or not */
   UINT8                       registerAddressCopiesToDo;                                             /*Number of register address copies to do (5 bits) */
   UINT8                       numLoops[MAX_NUM_REGISTER_ADDRESS];                                    /*Number of transfer loops to do */
   PL330DMA_CcrU               ccrValuesToUse[MAX_NUM_REGISTER_ADDRESS];                              /*CCR values to use */
   UINT8                       numBuffers;                                                   
   UINT32                      protobuf_metadata_dstAddress[MAX_NUM_BUFFERS_LOOPS];
   UINT32                      frameBufferOffsetBytes;                                                /* We are sending where the frame buffer starts,
                                                                                                       so the host knows how to offset the start of frame buffer for showing images */
   const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entry;                                 /*Metadata entry*/
} CDE_DRVG_regToMemoryCopyParams;
//------------------------------------------
//------------------------------------------
// main configuration structure
// used by manager to configure channel
typedef struct
{
   CDE_DRVG_streamCfgT        streamCfg;
   CDE_DRVG_portCfgT       srcCfg;
   CDE_DRVG_portCfgT       dstCfg;
   CDE_DRVG_callbackCfgT      cbCfg;
   CDE_DRVG_memcpyCfgT        memcpyCfg;
   CDE_DRVG_regToMemoryCopyConfigParams regToMemoryCopyParams;
}CDE_DRVG_channelCfgT;

//------------------------------------------
//------------------------------------------

#define MAX_SCENARIO_LOOPS 2

typedef struct
{
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   UINT32                  address[1];
#else
   UINT32                  address[CDE_DRVG_CH_MAX_NUM_OF_BUFFERS];
#endif
   UINT32                  numberOfBuffers;
   UINT32                  loopProgLocation;
   UINT32                  addressOffset;                                  //write in the middle of buffer
   UINT32                  buffSize;
   UINT32                  buffSizeOffset;
   UINT32                  imgWidth;
   UINT32                  imgHeight;
   UINT32                  imgStartX;
   UINT32                  imgStartY;
   UINT32                  imgBpp;
   UINT32                  stride;
   UINT32                  drainCtr;
   UINT32                  offset;
   UINT32                  buffWidth;
   UINT32                  numLoopsPerLine;
   UINT8                   periphNum;
   UINT8                   loopCount;
}CDE_DRVG_portParamsT;


typedef enum
{
   CDE_DRVG_EVENT_TYPE_NOT_SET_E,
   CDE_DRVG_EVENT_TYPE_DIR_FROM_CHANNEL_E,   
   CDE_DRVG_EVENT_TYPE_DIR_TO_CHANNEL_E,  
   CDE_DRVG_EVENT_TYPE_INVALID_E,
}CDE_DRVG_eventTypeE;


typedef struct _CDE_DRVG_channelParamsT CDE_DRVG_channelParamsT;                 // Forward declaration to resolve CDE_DRVG_ScenarioT

typedef ERRG_codeE (*CDE_DRVG_ScenarioT)( CDE_DRVG_channelParamsT *param, void *arg);

typedef enum
{
   CDE_DRVG_INTERRUPT_TYPE_INVALID_E,
   CDE_DRVG_INTERRUPT_TYPE_FRAME_DONE_E, 
   CDE_DRVG_INTERRUPT_TYPE_CHANNEL_DONE_E,      
   CDE_DRVG_INT_GROUP_GENERAL_ERR_E,
   CDE_DRVG_INTERRUPT_TYPE_LAST_E,
}CDE_DRVG_InterruptTypeE;

#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING  
typedef void (*CDE_DRVG_driverIntCbT)( void *managerInfo, CDE_DRVG_InterruptTypeE intType,  UINT64 timeStamp);
#else
typedef ERRG_codeE (*CDE_DRVG_driverIntCbT)( void *managerInfo, CDE_DRVG_InterruptTypeE intType,  UINT64 timeStamp);
#endif
//------------------------------------------
//------------------------------------------
// Entity of driver database
typedef struct _CDE_DRVG_channelParamsT
{
   UINT8                       coreNum;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   UINT8                       hwChannelNum;
#else
   UINT8                       channelNum;
#endif
   void*                       managerInfo;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   UINT8                      *program;
   UINT32                      phyProgAddress;
#else
   UINT8                       program[CDE_DRVG_CH_PROGRAMM_MAX_SIZE];
#endif
   UINT8                      *programPtr;
   UINT8                       dmaLoopSize;
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   UINT32                      dmaDstOffset; /* Physical address of beggining of loops */
#else
   UINT8                       dmaDstOffset;
#endif
   CDE_DRVG_ScenarioT          scenario;
   CDE_DRVG_scenarioTypeE      scenarioType;
   PL330DMA_CcrU               ccr;
   CDE_DRVG_portParamsT        srcParams;
   CDE_DRVG_portParamsT        dstParams;
   CDE_DRVG_eventTypeE         event[CDE_DRVG_MAX_EVENTS_PER_CHANNEL];
   UINT8                       frameDoneIntEnabled;
   UINT8                       channelDoneIntEnabled;
   UINT32                      frameDoneCounter;
   UINT32                      chunksNumPerFrame;
   UINT32                      chunkNum;
   MEM_POOLG_handleT           memPoolHandle;
   MEM_POOLG_bufDescT         *drainingBuffDesc;
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   MEM_POOLG_bufDescT         *buffDescListP[MAX_NUM_BUFFERS_LOOPS];       /* List to hold the bufferDescriptors allocated for the DMA which are in the program        */
   UINT8                      *dstAddressProgramP[MAX_NUM_BUFFERS_LOOPS];  /* List to hold the locations of all the MOV TO DAR ADDRESS instructions in the program */
   UINT8                       oldIndex;                                    /* The value will hold the "oldest" entry, when handling interrupt we need to start from   */
   UINT8                       numLoops;
   CDE_DRVG_driverIntCbT       intCb;
#else
   MEM_POOLG_bufDescT         *curBuffDesc;
   MEM_POOLG_bufDescT         *doneBuffDesc;
   MEM_POOLG_bufDescT         *nextBuffDesc;
#endif
   UINT32                      drainingBuffAddr;
   UINT32                      drainCtr;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   UINT8                      *setEventFrameDoneProgramP[MAX_NUM_BUFFERS_LOOPS];
   UINT8                       numSetEventFrameDone;
   UINT8                      *setEventChannelDoneProgramP;
   UINT8                       virtualChannelNum;
#endif
#ifdef CDE_DRVG_METADATA
   MEM_POOLG_bufDescT         *metadata_buffDescListP[MAX_NUM_BUFFERS_LOOPS];          /* List to hold the bufferDescriptors allocated for the DMA which are in the program (For metadata part)       */
   UINT8                      *meatadata_srcAddressProgramP[MAX_NUM_BUFFERS_LOOPS];    /* List to hold the locations of all the MOV TO SAR ADDRESS instructions for metadata (For metadata src) */
   MEM_POOLG_handleT           metadata_memPoolHandle;                                 /* Mempool handle for metadata */
   UINT8                       number_metadata_loops ;                                 /*Number of metadata loops*/
   CDE_DRVG_regToMemoryCopyParams regToMemoryParams;
   UINT32                      framebuffer_offset;                                     /*Offset for where to store frame buffer */
   UINT32                      numberMetadataLines;                                    /*Number of metadata lines for this channel */
#endif
   /* Used for main channel of  Extended DMA Interleaving Mode */
   UINT8                       *waitEventProgramP[MAX_NUM_BUFFERS_LOOPS][CDE_DRVG_MAX_WAIT_EVENTS];
   UINT8                       numWaitEvents;
}CDE_DRVG_channelParamsT;

typedef CDE_DRVG_channelParamsT* CDE_DRVG_channelHandleT;

/* Extended DMA Interleaving Mode Params */
typedef struct
{
   UINT32 numSubChannels;
   CDE_DRVG_channelHandleT subChannelHandles[4];
}CDE_DRVG_extIntParams;

/****************************************************************************
 ***************    G L O B A L      D A T A         ***************
 ****************************************************************************/

/****************************************************************************
 ***************   G L O B A L       F U N C T I O N S     ***************
 ****************************************************************************/

 
ERRG_codeE CDE_DRVG_init(UINT32 dmaRegistersVirtualAddr, UINT32 dmaDriverDBVirtualAddr, UINT32 size);
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING  
ERRG_codeE CDE_DRVG_open( );
#else
ERRG_codeE CDE_DRVG_open(CDE_DRVG_driverIntCbT intCb);
#endif
ERRG_codeE CDE_DRVG_close(void);

ERRG_codeE CDE_DRVG_openChannel( CDE_DRVG_channelHandleT* channelH, CDE_DRVG_PeriphClientIdE source, CDE_DRVG_PeriphClientIdE destination );
ERRG_codeE CDE_DRVG_openChannelFixedCoreNum( CDE_DRVG_channelHandleT* channelH,CDE_DRVG_coreNumE requestedCore, CDE_DRVG_PeriphClientIdE source,  CDE_DRVG_PeriphClientIdE destination);
ERRG_codeE CDE_DRVG_closeChannel( CDE_DRVG_channelHandleT channelH );
ERRG_codeE CDE_DRVG_startChannel( CDE_DRVG_channelHandleT channelH );
ERRG_codeE CDE_DRVG_startChannelExt( CDE_DRVG_channelHandleT channelH, CDE_DRVG_extIntParams *extIntParams);
ERRG_codeE CDE_DRVG_stopChannel( CDE_DRVG_channelHandleT channelH );

void       CDE_DRVG_updateMemToPeriphsrc( CDE_DRVG_channelHandleT h, UINT8 *injectBuff );
void       CDE_DRVG_updateMemToMemAddress( CDE_DRVG_channelHandleT h, UINT32 dst, UINT32 src );
ERRG_codeE CDE_DRVG_updateMemToMem( CDE_DRVG_channelHandleT h, UINT32 dst, UINT32 src, UINT32 size );
ERRG_codeE CDE_DRVG_configureScenario( CDE_DRVG_channelHandleT    channelHandle, CDE_DRVG_channelCfgT* channelCfg );
ERRG_codeE CDE_DRVG_setPeripheralParams( CDE_DRVG_portParamsT* portParams, UINT8 periphNumber, UINT32 periphAddress );
ERRG_codeE CDE_DRVG_generateChannelProgram(  CDE_DRVG_channelHandleT    channelHandle, void* arg );
ERRG_codeE CDE_DRVG_getFrameCounter(  CDE_DRVG_channelHandleT channelHandle, UINT32* counter );
CDE_DRVG_channelStatusE CDE_DRVG_getChannelStatus( CDE_DRVG_channelHandleT channelHandle );
UINT32     CDE_DRVG_getChannelDestAddress( CDE_DRVG_channelHandleT channelHandle );
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
ERRG_codeE CDE_DRVG_assignPhysicalChannel( CDE_DRVG_channelHandleT channelH );
#endif
ERRG_codeE CDE_DRVG_calculateCcrForTransferSize(PL330DMA_CcrU * ccr, UINT8 transferSize, CDE_Addr_IncrementT SrcIncrement, CDE_Addr_IncrementT DestIncrement);
ERRG_codeE CDE_DRVG_configureregToMemoryCopyParams(CDE_DRVG_channelHandleT  channelHandle, CDE_DRVG_regToMemoryCopyConfigParams configParams);
ERRG_codeE CDE_SCENARIOSG_registerToMemorywithExtraMetadataInfinite( CDE_DRVG_channelParamsT* params, void* arg );
#ifdef __cplusplus
}
#endif

#endif   //__CDE_DRV_H__


