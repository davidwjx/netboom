

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
#ifndef __CDE_MNGR_H__
#define __CDE_MNGR_H__

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/

#include "cde_drv_new.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define CDE_MNGRG_NUM_CHANNEL_CB                (3)   //  Number of callbacks supported for dma channel	
#define CDE_MNGRG_MAX_NUM_INT_CHANNELS          (4)   // 4 channels interleaving at most
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
#define CDE_MNGRG_MAX_AVAILABLE_CHAN            (CDE_DRVG_MAX_VIRTUAL_CHANNELS_PER_CORE * CDE_DRVG_CORE_NUM_MAX_E) //  Number of maximum dma channels in system.
#else
#define CDE_MNGRG_MAX_AVAILABLE_CHAN            (CDE_DRVG_MAX_CHANNELS_PER_CORE * CDE_DRVG_CORE_NUM_MAX_E) //  Number of maximum dma virtual channels in system.
#endif

/****************************************************************************
 ***************      G L O B A L        T Y P E D E F S      ***************
 ****************************************************************************/
typedef struct _CDE_MNGRG_channnelInfoT CDE_MNGRG_channnelInfoT;						// Forward declaration to resolve CDE_MNGRG_userCbParamsT

typedef struct
{
   UINT32               dmaChanId;
   UINT32               systemFrameCntr;
   UINT64               timeStamp;
   MEM_POOLG_bufDescT   *buffDescriptorP;
   UINT8                *currVirtPtr;
   CDE_MNGRG_channnelInfoT* channelInfo;
   UINT32               chunkNum;
} CDE_MNGRG_userCbParamsT;
 
typedef void (*CDE_MNGRG_chCbT) 				(CDE_MNGRG_userCbParamsT *cbParam, void *arg);

typedef enum
{
	CDE_MNGRG_PORT_SOURCE_E,
	CDE_MNGRG_PORT_DESTINATION_E,	
}CDE_MNGRG_portDirE;


typedef enum
{
	CDE_MNGRG_DRV_STATUS_CLOSED_E,
	CDE_MNGRG_DRV_STATUS_OPENED_E,	
	CDE_MNGRG_DRV_STATUS_INVALID_E,
}CDE_MNGRG_drvStatusE;


typedef enum
{
	CDE_MNGRG_CHAN_STATUS_CLOSED_E = 0,
	CDE_MNGRG_CHAN_STATUS_OPENED_E,	
	CDE_MNGRG_CHAN_STATUS_CONFIGURED_E,		
	CDE_MNGRG_CHAN_STATUS_RUNNING_E,	
	CDE_MNGRG_CHAN_STATUS_INVALID_E,
}CDE_MNGRG_channelStatusE;

typedef enum
{
	CDE_MNGRG_CCF_CB_NOT_CONFIGURED_E,
	CDE_MNGRG_CCF_CB_NON_CCF_CB_E,	
	CDE_MNGRG_CCF_CB_CCF_CB_E,		
	CDE_MNGRG_CCF_CB_INVALID_E,	
}CDE_MNGRG_ccfCbE;

typedef struct
{	
	CDE_MNGRG_chCbT				cb;
	void*						arg;
}CDE_MNGRG_cbTypeT;


typedef struct
{
	CDE_MNGRG_cbTypeT			frameDoneCb;
	CDE_MNGRG_cbTypeT			channelDoneCb;
}CDE_MNGRG_chanCbInfoT;

typedef struct
{
    bool useExtIntMode;
    UINT32 mainBlkInstance; // Record the first AXI_RD instance
    UINT32 subChannels;
    CDE_MNGRG_channnelInfoT *subChannelsInfo[CDE_MNGRG_MAX_NUM_INT_CHANNELS]; // 1 extra pointer, main + sub channels will have 4 channels at most
}CDE_MNGRG_extIntInfoT;

typedef struct _CDE_MNGRG_channnelInfoT
{
	INU_DEFSG_sysChannelIdE		systemChanId;		// key value in DB
#ifdef CDE_DRVG_METADATA
	CDE_DRVG_channelHandleT		metadataDMAChannelHandle;	// access channel parametersx
	CDE_MNGRG_channelStatusE    metadataChannelStatus;
	CDE_MNGRG_chanCbInfoT 		metadataCallBacksInfo[CDE_MNGRG_NUM_CHANNEL_CB];
	bool						metadataEnabled;
#endif
	CDE_DRVG_channelHandleT		dmaChannelHandle;			// access channel parametersx
	CDE_MNGRG_channelStatusE	channelStatus;
	CDE_MNGRG_chanCbInfoT      callBacksInfo[CDE_MNGRG_NUM_CHANNEL_CB];
	MEM_POOLG_handleT	         memPoolHandle;
    /*Extended DMA Interleaving Mode*/
    CDE_MNGRG_extIntInfoT       extInfo;
}CDE_MNGRG_channnelInfoT;


typedef struct
{
	CDE_MNGRG_drvStatusE		drvStatus;
	CDE_MNGRG_channnelInfoT		chanInfo[CDE_MNGRG_MAX_AVAILABLE_CHAN];	// TODO: relay on new substitute of INU_DEFSG_NUM_OF_SYS_CHANNELS_E
	
}CDE_MNGRG_dmaInfoT;	
/****************************************************************************
 ***************	  G L O B A L		 D A T A			  ***************
 ****************************************************************************/
extern CDE_DRVG_PeriphClientIdE ppeAxiBuffers[];
extern CDE_DRVG_PeriphClientIdE ppeAxiWriterBuffers[];

/****************************************************************************
 ***************	 G L O B A L		 F U N C T I O N S	  ***************
 ****************************************************************************/
	
ERRG_codeE CDE_MNGRG_init( void );
ERRG_codeE CDE_MNGRG_open( void );
ERRG_codeE CDE_MNGRG_close( void );
ERRG_codeE CDE_MNGRG_openChannel(CDE_MNGRG_channnelInfoT** channelInfo, CDE_DRVG_PeriphClientIdE source, CDE_DRVG_PeriphClientIdE destination);
ERRG_codeE CDE_MNGRG_closeChannel(CDE_MNGRG_channnelInfoT* channelInfo);
ERRG_codeE CDE_MNGRG_startChannel(CDE_MNGRG_channnelInfoT* channelInfo);
ERRG_codeE CDE_MNGRG_stopChannel(CDE_MNGRG_channnelInfoT* channelInfo);
ERRG_codeE CDE_MNGRG_resumeChannel(CDE_MNGRG_channnelInfoT* channelInfo);
ERRG_codeE CDE_MNGRG_pauseChannel(CDE_MNGRG_channnelInfoT* channelInfo);
ERRG_codeE CDE_MNGRG_setupChannel(CDE_MNGRG_channnelInfoT* channelInfo, CDE_DRVG_channelCfgT* configuration, CDE_MNGRG_chanCbInfoT * cbInfo );
ERRG_codeE CDE_MNGRG_invokeFrameDoneUserCallbacks( void *managerInfo,	UINT64 timeStamp ,UINT32 frameDoneCounter, void *bufDescP);
ERRG_codeE CDE_MNGRG_invokeChannelDoneUserCallbacks( void *managerInfo, UINT64 timeStamp );
ERRG_codeE CDE_MNGRG_registerUserCallBacks( CDE_MNGRG_channnelInfoT* channelInfo, CDE_MNGRG_chanCbInfoT* cbInfo );
ERRG_codeE CDE_MNGRG_setSysId(CDE_MNGRG_channnelInfoT* channelInfo, INU_DEFSG_sysChannelIdE sysId );
ERRG_codeE CDE_MNGRG_showConfig(void);
ERRG_codeE CDE_MNGRG_isPeriphInUse(CDE_MNGRG_channnelInfoT** channelInfo, CDE_DRVG_PeriphClientIdE periph, CDE_MNGRG_portDirE portDir  );
ERRG_codeE CDE_MNGRG_memcpyOpenChan( CDE_MNGRG_channnelInfoT **channelInfo, UINT32 size, CDE_MNGRG_chCbT doneCb, void *cbArgP);
ERRG_codeE CDE_MNGRG_memcpyCloseChan( CDE_MNGRG_channnelInfoT *channelInfo );
ERRG_codeE CDE_MNGRG_memcpyBlock( CDE_MNGRG_channnelInfoT *channelInfo, UINT32 dst, UINT32 src, UINT32 size );
ERRG_codeE CDE_MNGRG_memcpy( CDE_MNGRG_channnelInfoT *channelInfo, UINT32 dst, UINT32 src, void *buffP );
ERRG_codeE CDE_MNGRG_stopChannelMemcpy(CDE_MNGRG_channnelInfoT* channelInfo);


ERRG_codeE CDE_MNGRG_injectBuffer( CDE_MNGRG_channnelInfoT* channelH, UINT8 *injectBuff);
ERRG_codeE CDE_MNGRG_generateChannelProgram( CDE_MNGRG_channnelInfoT* channelInfo );
ERRG_codeE CDE_MNGRG_colorDataBuffers( CDE_MNGRG_channnelInfoT *channelInfo, UINT8 value );
#ifdef CDE_DRVG_METADATA
ERRG_codeE CDE_MNGRG_metadataOpenChan( CDE_MNGRG_channnelInfoT **channelInfo,CDE_DRVG_regToMemoryCopyConfigParams *configParams);
ERRG_codeE CDE_MNGRG_metadataCloseChan( CDE_MNGRG_channnelInfoT *channelInfo);
ERRG_codeE CDE_MNGRG_registerUserMetadataCallBacks( CDE_MNGRG_channnelInfoT* channelInfo, CDE_MNGRG_chanCbInfoT* cbInfo );
#endif
//Extended DMA Interleaving
ERRG_codeE CDE_MNGRG_setupExtIntInfo(CDE_MNGRG_channnelInfoT *mainChannelInfo, UINT32 mainBlkInstance);
ERRG_codeE CDE_MNGRG_fixExtIntAxiRdCfg(CDE_MNGRG_channnelInfoT *mainChannelInfo, CDE_DRVG_channelCfgT *configuration);
ERRG_codeE CDE_MNGRG_allocExtIntSubChannels(CDE_MNGRG_channnelInfoT *mainChannelInfo);

#ifdef __cplusplus
}
#endif

#endif //__CDE_MNGR_H__

