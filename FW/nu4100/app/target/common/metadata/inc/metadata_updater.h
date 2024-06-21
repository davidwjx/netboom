#pragma once
#include "cde_mngr_new.h"
#include "inu_sensor.h"
#include "sensors_mngr.h"
#include "metadata_target_config.h"
#include <stdint.h>
#include "cde_drv_new.h"
#include "vsi3a/vsi3a_types.h"
#define METADATA_FIFO_SIZE (6)
#define MAXIMUM_TIME_DIFF_USEC 2000 /*Only allow the interrupt latency to be 2000 microseconds*/
#define NORMAL_RUNNING_ISP_QUEUE_LIMIT 2 /*Found through experimentation */
#define METADATA_ISP_PARAM_QUEUE_SIZE (10)
#define MAX_NUMBER_FUTURE_UPDATES_SUPPORTED MAX_NUM_BUFFERS_LOOPS
#define METADATA_NEXT_FRAME_INDEX 0 
#ifdef __cplusplus
extern "C" {
#endif
/*Strucutres used for updating metadata every SOF */
typedef struct
{
    SENSORS_MNGRG_sensorInfoT *sensorInfo;
    bool registered;                                                                /*True if sensorInfo has been registered*/
    OS_LYRG_msgQueT sensorQueue;                                                    /*Queue of sensor updates to deal with on each SOF (Added to by connection layer, read by the sensor update manager) */
    OS_LYRG_msgQueT ROIQueue;                                                     /*Queue of ROI updates to process*/
    INU_metadata_State                  incomingSensorUpdate;                       /*Incoming sesnor update to perform*/
} METADATA_UPDATE_registeredSensor;
typedef struct
{
    UINT32 startFrameID;                                                /*Use this entry if Frame ID > startFrame ID*/
    UINT32 startEOFFrameID;                                             /*EOF frame counter when this entered in*/
    MEM_POOLG_bufDescT         *metadata_buffDescListP;                 /*ISP metadata buffers*/
    UINT32 useCount;                                                    /*How many times this buffer is used*/
} METADATA_FIFO_Entry;
typedef struct
{
    UINT32 numEntries;                                                  /*How many entries are valid*/
    UINT32 head;
} METADATA_FIFO_Status;
typedef struct
{
    INU_metadata_ISP_ExpoParams expoParams;
    uint32_t sensor_index;
} METADATA_AR2020Gain_Queue_Entry;
#define DROPPED_FRAME_RECORD_SIZE 50
typedef struct
{
    CDE_MNGRG_channnelInfoT *channel;                                               /* CDE_MNGR DMA channel (metadata buffers are pointed to in here)*/
    bool inUse;                                                                     /* False = Not in use, True = in Use */
    uint64_t firstFrameID;
    UINT64 metadataUpdates_Counter;                                                 /* Number of metadata updates */
    const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapentry;           /*Contains the SLU->Core,Vchannel,AXI RD, giveEvent,WaitEvent Map */
    METADATA_UPDATE_registeredSensor sensors[MAX_NUM_REGISTERED_SENSORS];           /*Contains the IO handle for the sensor*/
    IO_HANDLE                       groupHandle;                                    /*Group handle of the sensor group*/
    INU_Metadata_T                  outgoingSensorUpdates[MAX_NUM_BUFFERS_LOOPS];    /*WARNING: This has been changed so that we can support N+2 metadata updates
                                                                                    Buffer 0 = N +1 metadata
                                                                                    Buffer 1 = N+2 metadata
                                                                                    Buffer 2 = N+3 metadata
                                                                                    Buffer 3 = N+4 metadata*/

    OS_LYRG_mutexT metadataMutex;                                                    /*Mutex for updating metadata [Locked by SOF interrupt and Sensor updater] */
    bool                        metadataBufferPendingUpdate[MAX_NUM_BUFFERS_LOOPS];  /*True = Pending update, false = No pending update*/
    uint8_t                        metadataDelayedUpdateBitFlag;                            /*Bit 0 = Pending update for the N+1 frame, Bit 1 = Pending update for N+2 frame etc...*/
    /*For the ISP streams, we will store out buffers here*/
    METADATA_FIFO_Entry         metadata_isp_fifo[METADATA_FIFO_SIZE];
    METADATA_FIFO_Status        metadata_isp_fifo_status;
    MEM_POOLG_bufDescT         *metadata_buffDescListP[MAX_NUM_BUFFERS_LOOPS];       /*ISP metadata buffers*/
    MEM_POOLG_handleT           handle;                                              /*ISP metadata buffer pool*/
    UINT64 timestamp_0;                                                              /*Timestamp 0 (ms)*/
    UINT32 previousFrameID;                                                            /*Frame ID offset*/
    double timestamp_delta_us;
    bool                        metadataBufferPendingRead[MAX_NUM_BUFFERS_LOOPS];     /*0 = Safe to write to, 1 = Pending read*/
    bool firstFrameIDFound;
    UINT32 softwareInjectCallCount;                                                  /*How many times software injection has happened*/
    UINT32 compensatedFrameCounter;                                                  /*How many frames have been dropped and compensated for*/
    UINT32 detectedDroppedFrameCounter;                                              /*How many frames have been dropped */
    UINT32 previousISPFrameIDs[2];
    UINT32 droppedFrameRecord[DROPPED_FRAME_RECORD_SIZE];                           /*Stores if a dropped frame has happened*/
    UINT32 droppedFrameRecordCounter;                                               /*How many dropped frames have been recorded*/
    UINT32 swInjectionPreviousFrameID;                                              /*Previous frame ID when using SW injection*/
    UINT32 excessDequeues;                                                          /*How many extra frames we have dequeued*/
    bool   has_set_isp_callbacks;                                                   /*If the ISP callbacks have been set or not*/
    OS_LYRG_msgQueT                     isp_expoQ;                                  /*ISP Exposure parameter queue*/
    OS_LYRG_msgQueT                     isp_wbQ;                                    /*ISP wb parameter queue*/
    OS_LYRG_msgQueT                     AR2020_gainQ;                               /*Gain commands for the AR2020 so that we can have gain/expo updates take effect at the same frame*/
    uint32_t    exp_counter;
} METADATA_UPDATER_channelHandleT;

typedef struct
{
  UINT64 timestamp;                                                         /*Timestamp of when the interrupt happened */
  UINT32 slu;                                                              /*SLU of the interrupt */
  UINT64 timestamp_userspace_send;                                         /* Userspace timestamp of when the interrupt happened*/
  UINT32 frameID;                                                          /*Frame ID of this register*/
  UINT64 rtcTimestamp;                                                     /*Frame RTC timestamp*/
  UINT32 ISP0FrameID;                                                      /*Hardware frame ID register for ISP0*/
  UINT32 ISP1FrameID;                                                      /*Hardware frame ID register for ISP1*/
} METADATA_SENSOR_UPDATER_interruptNotification;
#define ZEROED_INTERRUPT_NOTIFICATION {0,0,0,0,0,0,0}
typedef struct
{
    struct Vsi3AMetaExp  value;                                            
    METADATA_SENSOR_UPDATER_interruptNotification notificationInfo;
} METADATA_ISP_Expo_Queue_Entry;
typedef struct
{
    struct Vsi3AMetaWb  value;                                            
    METADATA_SENSOR_UPDATER_interruptNotification notificationInfo;
} METADATA_ISP_Wb_Queue_Entry;
typedef int METADATA_SENSOR_UPDATER_frameTimingT;
/* This module is for handling the Following:
    1. Registering Sensor IDs into our Channel ID+Sensor ID Map
    2. Registering Channel IDs into our Channel ID+Sensor ID Map
    3. Handling SOF interrupts for an SLU (Via calling the metadata_sensor_updater and then serializing a new metadata message)
*/
/* @brief Callback for a SOF interrupt 
    @param Timestamp Timestamp of the interrupt
    @param SLU number
    @param argP Callback argument (NOT USED)  */
void METADATA_UPDATER_sofInterruptCb(UINT64 timestamp, UINT32 slu, void *argP);
/* @brief Callback for a SOF interrupt 
    @param Timestamp Timestamp of the interrupt
    @param SLU number
    @param argP Callback argument (NOT USED)  */
void METADATA_UPDATER_eofInterruptCb(UINT64 timestamp, UINT32 slu, void *argP);
/* @brief 
    Stores the readerID to our LUT which is used in the SOF interrupt handler
  @param channelID     - Unique reader ID
  @param bootID        - Boot ID
  @param channelHandle - channelHandle which is stored in our LUT and used to update the metadata buffers when the SOF interrupt 
                        happens for the SLU
*/
ERRG_codeE  METADATA_UPDATER_storeDMAChannelPointerWithReaderID(const UINT8 readerID,const INU_DEFSG_moduleTypeE bootID, 
    CDE_MNGRG_channnelInfoT* channelHandle);
/* @brief Updates the MOV instructions in the metadata thread
   @param channelHandle Metadata DMA channel
   @param modelType Model type of the device
   @param loopIndex Loop index to update the MOV instruction for
   @param phyAddress Physical address of the new memory buffer
   @param offset Offset to apply for the metadata
*/
ERRG_codeE METADATA_UPDATER_updateMOVInstructions(CDE_DRVG_channelHandleT channelHandle,INU_DEFSG_moduleTypeE modelType, 
    UINT32 loopIndex, UINT32 phyAddress, UINT32 offset);
/**
 * @brief Stores a group handle and the sensor handles for this group in our LUT which is used in the SOF interrupt handler 
 *        and also for setting up the GME I/O muxing for the FSG
 * 
 * @param bootID Model type of the device
 * @param sensorGroupP Handle of the group
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_UPDATER_storeGroupHandle(const INU_DEFSG_moduleTypeE bootID,IO_HANDLE sensorGroupP);
/**
 * @brief Processes an incoming sensor update request by deserializing the buffer, 
 *  looking for matching sensors in our LUT and then adds the sensor update to the Queue for that paricular sensor 
 * 
 * @param buffer INU_Metadata_T serialized buffer
 * @param size Size of the serialized buffer
 * @param Repeat Forces UART repeating
 * @param checkForSyncedCommands If true, we will checked for synced commands and delay processing of the sensor update request until the next SOF interrupt
 * @return ERRG_codeE Returns an errror code
 */
ERRG_codeE METADATA_UPDATER_processIncomingSensorUpdateRequest(char *buffer, size_t size, const bool repeat, 
    const bool checkforSyncedCommands);
/**
 * @brief Function to get the frame ID for a map entry
 * 
 * @param mapEntry Pointer to map entry
 * 
 * @param frameID 
 * @return ERRG_codeE 
 */
 ERRG_codeE METADATA_UPDATER_getFrameID(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry, UINT32 *frameID);
/**
 * @brief Function to calculate the next metadata buffer index
 * 
*/
 UINT32 METADATA_UPDATER_calculateMetadataBuffer(METADATA_UPDATER_channelHandleT *channel, bool writerOperateFunction);
/**
 * @brief Calculates next buffer Index
 * 
 * @param channel Metadata channel
 * @param mapEntry Map entry for this channel
 * @param writerOperateFunction True if we are called from a writer operate context, false if we are from an SLU SOF/EOF context
 * @return UINT32 Returns the next buffer index
 */
UINT32 METADATA_UPDATER_calculateNextBufferIndex(METADATA_UPDATER_channelHandleT *channel, bool writerOperateFunction);
/**
 * @brief Initializes the metadata updater (and creates a thread)
 * 
 *
 * @param deviceH Device
 * @return Returns an error code
 */
ERRG_codeE METADATA_UPDATER_init();
/**
 * @brief Returns a pointer containing the registered Channels
 * 
 *
 * @return Returns pointer to list of registered channels
 */
METADATA_UPDATER_channelHandleT* METADATA_UPDATER_getChannelHandleArray();
/**
 * @brief Returns the number of registered channels
 * 
 *
 * @return Number of registered channels
 */
int METADATA_UPDATER_numberRegisteredDMAChannels();

/**
 * @brief Stores the Sensors exposure and gains for a sensor
 * 
 *
 * @param bootID Boot ID
 * @param sensorInfo Sensor Info Pointer
 * @param SensorID Sensor ID
 * @return Returns an errror code
 */
ERRG_codeE METADATA_UPDATER_storeSensorExposureAndGains(const INU_DEFSG_moduleTypeE bootID,const SENSORS_MNGRG_sensorInfoT *sensorInfo, UINT16 SensorID);
/**
 * @brief Updates the cropping window for a sensor using the PPU if there's been a cropping window request sent from the host
 * 
 * @param axiReader AXI reader number
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_UPDATER_updateCroppingWindow(unsigned int axiReader);
/*@brief This function will update metadata for the CURRENT metadata buffers and should only be called from the writer operate function
  @param writerID - ID of the writer to update metadata for
  @param timestamp - Timestamp to use in the metadata updates
  @return Returns an error code
*/
ERRG_codeE METADATA_UPDATER_updateMetadataFromWriterOperate(const unsigned int writerID,const UINT64 timestamp);
/**
 * @brief Finds a metadata entry by using the writer ID
 * 
 *
 * @param writerID Writer ID
 * @param channelHandle Double pointer to a metadata channel handle
 * @return returns an error code
 */
ERRG_codeE METADATA_UPDATER_findEntryByWriterID(const unsigned int writerID,METADATA_UPDATER_channelHandleT **channelHandle );
/**
 * @brief Locks a mutex for a specific metadata entry
 * 
 *
 * @param entry Metadata entry to lock the mutex for
 * @return Returns an error code
 */
ERRG_codeE METADATA_UPDATER_LockMutex(METADATA_UPDATER_channelHandleT *entry);
/**
 * @brief Unlocks a mutex for a specific metadata entry
 * 
 *
 * @param entry Metadata entry to unlock the mutex for
 * @return Returns an error code
 */
ERRG_codeE METADATA_UPDATER_UnlockMutex(METADATA_UPDATER_channelHandleT *entry);
/**
 * @brief Injects metadata into bufDescP for a reader
 * @param bufDescP - Buffer to injec metadata into
 * @param readerID - Reader ID
 * @returns Returns an error code
 * 
 */
ERRG_codeE METADATA_UPDATER_injectMetadata(MEM_POOLG_bufDescT *bufDescP,UINT32 readerID, inu_image__hdr_t* hdr);
/**
 * @brief Gets the timestamp being used for a metadata channel
 * 
 * @param mapEntry Map entry
 * @param timestamp Output pointer for the timestamp
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_UPDATER_getTimestamp(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry, UINT64 *timestamp);
/**
 * @brief Gets a metadata channel from searching via reader ID
 * 
 * @param ptr 
 * @param READER_ID 
 * @return ERRG_codeE 
 */
ERRG_codeE  METADATA_getChannelHandleByReaderID(METADATA_UPDATER_channelHandleT **ptr, int READER_ID);

 ERRG_codeE METADATA_UPDATER_calculateFrameID( METADATA_UPDATER_channelHandleT *channel , UINT32 *frameID);

 void METADATA_UPDATER_setPendingBufferUpdatesForAllBuffers(METADATA_UPDATER_channelHandleT *handle);
 void METADATA_UPDATER_getFullFrameIDWithOverFlowCounter(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry, UINT32 *frameID);
 ERRG_codeE METADATA_UPDATER_process_cmd(INU_Metadata_T *tempMetadata);

 bool METADATA_UPDATER_getFutureUpdatesPending(const METADATA_UPDATER_channelHandleT * entry);
void METADATA_UPDATER_markFutureUpdatePending( METADATA_UPDATER_channelHandleT  * const entry, METADATA_SENSOR_UPDATER_frameTimingT index);
/*Once we have finished serializing the metadata update, we will shift each entry in metadataDelayedUpdate to the right and we will also shift the metadata buffers in the same way*/
void METADATA_UPDATER_shiftMetadataBuffers( METADATA_UPDATER_channelHandleT  *const entry);

#ifdef __cplusplus
}
#endif