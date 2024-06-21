#include "cde_drv_new.h"
#include "inu_metadata.h"
#include "inu_common.h"
#include "err_defs.h"
#include "mem_map.h"
#include "metadata_target_config.h"
#include "metadata_target_serializer.h"
#include "PL330_dma_misc.h"
#include "os_lyr.h"
#include "inu_sensor.h"
#include "sensors_mngr.h"
#include "queue_lyr.h"
#include "metadata_sensor_updater.h"
#include "hw_regs.h"
#include "sensorsync_updater.h"
#include "helsinki.h"
#include "ppe_mngr.h"
#include "metadata_uart.h"
#include "nuserial.h"
#include "rtc.h"
#include "assert.h"
#include "gme_mngr.h"
#include "helsinki.h"
#include "metadata_isp_updater.h"
#include <mutex>
#include <array>
#include <blocking_queue.hpp>
typedef enum{
    METADATA_SearchUsingSOFFrameID,
    METADATA_SearchUsingEOFFrameID
} FIFO_SEARCH_METHOD_e;

#ifdef CDE_DRVG_METADATA
#define SOF_INTERRUPT_QUEUE_SIZE 40
#define SNSR_UPDATE_QUEUE_SIZE 40
#define ROI_UPDATE_QUEUE_SIZE 40
#define SOF_INTERRUPT_TIMEOUT_MS 2
typedef struct
{
    METADATA_UPDATER_channelHandleT *entry;                             /*Entry pointer*/                                           
    bool ISP_Channel;                                                    /*True if we are an ISP channnel*/
    const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry; /*Map entry pointer*/
    UINT32 INDEX;                                                        /*Buffer index (0->MAX_NUM_BUFFER_LOOPS)*/                                          
    bool overwriteSensorMetadata;                                        /*True if we should overwrite the sensor metadata*/
    UINT32 frameID;                                                      /*Frame ID to use*/
    int bufferCounter;                                                   /*A counter which is used for debugging metadata*/
    UINT32 frameBufferOffset;                                            /*Offset from the start of the metadata buffer to the frame buffer (bytes)*/
    UINT64 currentTs;
}   METADATA_UPDATER_prepareStandardParams;
#define NUMBER_SLU_QUEUES 6
typedef struct
{
    UINT32 previous_slu_frame_ID[NUMBER_SLU_QUEUES];                    /*Used to detect Frame ID overflows*/
    UINT32 frame_id_counter[NUMBER_SLU_QUEUES];                         /*32 bit counter to be used as a replacement for the 16 bit SLU counter*/
    UINT32 eof_frame_id_counter[NUMBER_SLU_QUEUES];                         /*EOF frame ID counter*/
    std::mutex mutex;                                               /*Mutex to lock these variables*/
    OS_LYRG_msgQueT         slu_queues_frame_id[NUMBER_SLU_QUEUES];   /*Queue for SOF interrupt notifications*/
    OS_LYRG_msgQueT         slu_queues_frame_id_eof[NUMBER_SLU_QUEUES];   /*Queue for SOF interrupt notifications*/
} METADATA_UPDATER_frameIDStatus;
/*Here I have added support for delaying the processing of INU_Metadata_T commands.
The goal is to use this queue to achieve frame perfect synchronization of ISP commands or any command if required*/
typedef struct
{
    std::shared_ptr<blocking_queue::queue<INU_Metadata_T>>  delayedCommandQueue;        /*We will push into this queue once we have a command that needs processing on the next SLU SOF interrupt*/            
} METADATA_UPDATER_delayedCommandQueueT;
extern "C"
{
static bool METADATA_UPDATER_checkPendingBufferUpdateForSLU(UINT32 slu);
static void METADATA_UPDATER_clearPendingBufferUpdate(METADATA_UPDATER_channelHandleT *handle, UINT8 bufferIndex);
void METADATA_UPDATER_setPendingBufferUpdatesForAllBuffers(METADATA_UPDATER_channelHandleT *handle);
void METADATA_UPDATER_getFullFrameIDWithOverFlowCounter(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry, UINT32 *frameID);
static void METADATA_UPDATER_getEOFCounter(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry, UINT32 *EofCounter);
static OS_LYRG_threadHandle    METADATA_SENSOR_UPDATER_threadHandle = NULL;
/*The blocking queue has zerocopy push and single copy pop, it also reduces the need for sys calls that the Posix Thread has*/
OS_LYRG_msgQueT sofInterruptQueue;
static METADATA_UPDATER_delayedCommandQueueT      syncedCmdQueue[NUMBER_SLU_QUEUES]   ;   /*A queue of INU_Metadata_T commands that need processing on the next SOF interrupt*/
METADATA_UPDATER_frameIDStatus frameIDStatus;
static ERRG_codeE METADATA_UPDATER_updateISPFrameIDQueue(UINT64 timestamp, UINT32 slu, bool SOFContext);
static int METADATA_UPDATER_updateSensorThread(void *arg);
/**
 * @brief Iniitalizes the Queue and Mutex for a METADATA_UPDATER_channelHandleT entry
 *
 * @param entry Entry to initialize the queue and mutex for
 * @return ERRG_codeE Returns an error code
 */
static ERRG_codeE METADATA_UPDATER_initQueueAndMutex(METADATA_UPDATER_channelHandleT *entry);
static METADATA_UPDATER_channelHandleT metadatachannels[MAX_REGISTERED_METADATA_CHANNELS];
static int registeredDMAChannels = 0;

METADATA_UPDATER_channelHandleT* METADATA_UPDATER_getChannelHandleArray()
{
    return metadatachannels;
}
int METADATA_UPDATER_numberRegisteredDMAChannels()
{
    return registeredDMAChannels;
}
/**
 * @brief For the ISP Streams we have to calculate the frame ID using the timestamps since the SLU frame id register doesn't work.
 * 
 * @param channel Channel Channel handle
 * @param frameID Frame ID result Resultant frame ID
 * @return ERRG_codeE Returns an error code
 */
 ERRG_codeE METADATA_UPDATER_calculateFrameID( METADATA_UPDATER_channelHandleT *channel , UINT32 *frameID)
{
    
    UINT64 timestamp = 0;
    METADATA_UPDATER_getTimestamp(channel->mapentry,&timestamp);
    UINT64 timestamp_ns = RTC_convertRTCTimeToNS(timestamp);
    UINT64 timestamp_us = timestamp_ns/1000UL; /*Convert to us*/
    if(channel->timestamp_0 == 0)
    {   
        /*This is the first frame*/
        channel->timestamp_0 = timestamp_us;
        *frameID = 0 ;
        channel->previousFrameID = 0;
        return INU_METADATA__RET_SUCCESS;
    }
    else
    {
        /*Using the timestamp has its own problems when trying to use it to calculate a frame ID:
        1. What time delta due to use? If its not exactly the right time delta then you get inaccuracies when the time delta reaches large values
        2. The ISP has its own timestamp jitter  of +/- 5 ms caused by SW issues, so the timestamp register might get overwritten with the next frame timestamp 
        */
        #if 0
        /*Perform the calculations in float*/
        if(channel->previousFrameID == 0 )
        {

        }
        double timeDifference = ((double)timestamp_us - (double)channel->timestamp_0);
        double tD = timeDifference / (double)timestamp_delta_us;
        if(tD < 0)
        {
            LOGG_PRINT(LOG_ERROR_E,NULL,"Negative time difference calculated \n");
            assert(0);
        }
        else
        {
            UINT32 calculatedFrameID = (UINT32) tD;
            if((calculatedFrameID >=   (channel->previousFrameID+2))   )
            {
                *frameID  = calculatedFrameID;
                return INU_METADATA__ERR_INVALID_ARGS;
            }
            else
                *frameID = (UINT32) calculatedFrameID;
            LOGG_PRINT(LOG_DEBUG_E,NULL,"Frame ID calculated to be %lu,%llu, td=%.3f \n", *frameID,timestamp,timeDifference);
   
            return INU_METADATA__RET_SUCCESS;
        }
        #else
        if(channel->previousFrameID)
            *frameID  = channel->previousFrameID + 1;
        else
            *frameID = 0;
        return INU_METADATA__RET_SUCCESS;
        #endif
    }
    
}

/* Updates metadata on a SOF interrupt for a DMA channel
   @param channel Channel to deal with interrupt for
   @param timestamp - Timestamp of the interrupt
*/
 ERRG_codeE METADATA_UPDATER_getFrameID(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry, UINT32 *frameID)
{

    /* If the map entry contains the frame ID entry then use it*/
    if (mapEntry->frameIDRegister_phy > 0)
    {
        volatile UINT32  frame_id_temp = 0;
        ERRG_codeE ret = HW_REGSG_readSocRegVolatile(mapEntry->frameIDRegister_phy, &frame_id_temp);
        if (ERRG_SUCCEEDED(ret))
        {
            *frameID = frame_id_temp;
            return INU_METADATA__RET_SUCCESS;
        }
        else
        {
            return ret;
        }
    }
    /* Otherwise we can calculate the frame ID register physical address to use */
    else
    {
        UINT32 frameIDphysicalAddress = 0;
        ERRG_codeE frameID_physical_ret = METADATA_TARGET_CONFIG_getFrameIDPhysicalAddress(&frameIDphysicalAddress, mapEntry);
        if (ERRG_SUCCEEDED(frameID_physical_ret))
        {
            volatile UINT32  frame_id_temp = 0;
            ERRG_codeE ret = HW_REGSG_readSocRegVolatile(frameIDphysicalAddress, &frame_id_temp);
            if (ERRG_SUCCEEDED(ret))
            {
                *frameID = frame_id_temp;
                return INU_METADATA__RET_SUCCESS;
            }
            else
            {
                return ret;
            }
        }
        else
        {
            return frameID_physical_ret;
        }
    }
    return INU_METADATA__ERR_INVALID_ARGS;
}
static ERRG_codeE METADATA_UPDATER_getSLUTimestampFromAddr(const UINT32 timestampPhysicalAddr,UINT64 *timestamp)
{

    ERRG_codeE ret = INU_METADATA__RET_SUCCESS;
    volatile UINT64 timestamp_lower = 0;
    volatile UINT64 timestamp_high = 0;
    volatile UINT64 resultant_timestamp = 0;
    /*We need to do two register reads since this is a 64 bit register*/
    UINT32 *lh = (UINT32 *) (timestampPhysicalAddr); /*Lower half of the 64 bit register*/
    UINT32 *uh = (UINT32 *) &lh[1]; /*Upper half of the 64 bit register*/
    UINT32 *ts_output = (UINT32 *) timestamp;
    ret = HW_REGSG_readSocRegVolatile((UINT32) lh, (volatile UINT32 *) &timestamp_lower);
    if(ERRG_FAILED(ret))
        return ret;
    ret = HW_REGSG_readSocRegVolatile((UINT32) uh, (volatile UINT32 *)  &timestamp_high);
    if(ERRG_FAILED(ret))
        return ret;
    /*Perform this operation on the volatile variable too*/
    resultant_timestamp = ((timestamp_high & 0xFFFFFFFF)  << 32) | (timestamp_lower & 0xFFFFFFFF) ;
    *timestamp = resultant_timestamp;
    return ret;
}
ERRG_codeE METADATA_UPDATER_getTimestamp(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry,UINT64 *timestamp)
{
    UINT32 timestampPhysicalAddr = 0;
    ERRG_codeE ret = INU_METADATA__RET_SUCCESS;
    volatile UINT64 timestamp_lower = 0;
    volatile UINT64 timestamp_high = 0;
    volatile UINT64 resultant_timestamp = 0;
    /* If the map entry contains the timestamp ID entry then use it*/
    if (mapEntry->timestampRegister_phy > 0)
        timestampPhysicalAddr = mapEntry->timestampRegister_phy;
    else
        /* Otherwise we can calculate the frame ID register physical address to use */
        ret = METADATA_TARGET_CONFIG_getTimestampPhysicalAddress(&timestampPhysicalAddr, mapEntry->slu);
   
   METADATA_UPDATER_getSLUTimestampFromAddr(timestampPhysicalAddr,timestamp);
   return ret;
}
/* @brief Calculates the current index for framebuffer/metadata updates
   @param channel - INU_metadata registered channel  */
static UINT32 METADATA_UPDATER_calculateLoopIndex(CDE_DRVG_channelHandleT channel)
{
/*Both metadata channel and normal DMA channel are in sync.
So we can use the normal DMA channel handle to workout
how many loops have been completed */
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
    volatile UINT32 pc = PL330_DMA_getPC(channel->coreNum, channel->hwChannelNum);
#else
    volatile UINT32 pc = PL330_DMA_getPC(channelH->coreNum, channelH->channelNum);
#endif
    UINT8 *temp;

    //calculate current index (round down)
    if(pc < channel->dmaDstOffset)
    {
        return 0;
    }
    else
    {
        UINT32 currentIndex = (pc - channel->dmaDstOffset) / channel->dmaLoopSize;
        return currentIndex;
    }
}
/**
 * @brief Calculates current buffer Index
 * 
 * @param channel Metadata channel
 * @param mapEntry Map entry for this channel
 * @param writerOperateFunction True if we are called from a writer operate context, False if we are called from an SLU SOF/EOF ISR context
 * @return UINT32 Returns the next buffer index
 */
UINT32 METADATA_UPDATER_calculateCurrentBufferIndex(METADATA_UPDATER_channelHandleT *channel, bool writerOperateFunction)
{
    if((channel->mapentry->axiReader!=ISP0_READER) && (channel->mapentry->axiReader!=ISP1_READER))
    {   
        LOGG_PRINT(LOG_ERROR_E,NULL,"This calculation isn't supported \n");
        assert(0);
    }
    else
    {
     #if 1
        UINT32 frameID = 0;
        ERRG_codeE ret = METADATA_UPDATER_getFrameID(channel->mapentry, &frameID);
        /*Previous frame ID will only be stored in the frame done callback*/
        if(channel->metadataUpdates_Counter == 0)
        {
            channel->firstFrameID = frameID; //store the first ever frame ID
        }
        /**/
        const UINT32 currentBuffer = (frameID) % MAX_NUM_BUFFERS_LOOPS;
        return currentBuffer;
        #else
        return 0;
        #endif
    }
}
/**
 * @brief Calculates next metadata buffer Index
 * 
 * @param channel Metadata channel
 * @param mapEntry Map entry for this channel
 * @param writerOperateFunction True if we are called from a writer operate context, False if we are called from an SLU SOF/EOF ISR context
 * @return UINT32 Returns the next buffer index
 */
UINT32 METADATA_UPDATER_calculateMetadataBuffer(METADATA_UPDATER_channelHandleT *channel, bool writerOperateFunction)
{
  return 0;
}

/**
 * @brief Calculates next buffer Index
 * 
 * @param channel Metadata channel
 * @param mapEntry Map entry for this channel
 * @param writerOperateFunction True if we are called from a writer operate context, False if we are called from an SLU SOF/EOF ISR context
 * @return UINT32 Returns the next buffer index
 */
UINT32 METADATA_UPDATER_calculateNextBufferIndex(METADATA_UPDATER_channelHandleT *channel, bool writerOperateFunction)
{
    /*By default we should use METADATA_UPDATER_calculateLoopIndex to calculate the loop index instead of the frame ID
    1. The frame ID may not start from 0
    2. The frame ID may increment by 4 instead of 1 for the Gaze stereo stream
    */

    if((channel->mapentry->axiReader!=ISP0_READER) && (channel->mapentry->axiReader!=ISP1_READER))
    {
        UINT32 currentLI = METADATA_UPDATER_calculateLoopIndex(channel->channel->dmaChannelHandle);
        UINT32 loopIndex = currentLI;
        if(!writerOperateFunction)
            loopIndex = ( loopIndex + 1) % MAX_NUM_BUFFERS_LOOPS;

        if(loopIndex > ( MAX_NUM_BUFFERS_LOOPS - 1))
        {
            LOGG_PRINT(LOG_DEBUG_E,NULL,"Invalid buffer index of %lu \n", loopIndex);
            loopIndex = 0; /*Once in a while, we will read the loop index as 4, this is because its just finished loop 3 and is just about to wrap back around to loop 0 */
        }
        /*We can update the current loop index, it's always safe to do so for readers which are connected to writers*/
        return loopIndex;   
    }
    /*For the ISP streams we will use the SLU frame ID when working out buffer indexes*/
    else
    {
       UINT32 nextBuffer = (METADATA_UPDATER_calculateCurrentBufferIndex(channel,writerOperateFunction) + 1) % MAX_NUM_BUFFERS_LOOPS;
       return nextBuffer;
    }
}
/**
 * @brief Calculates the Number of registered sensors for an entry
 * 
 *
 * @param mapEntry Map entry
 * @return Returns an error code
 */
static UINT16 METADATA_UPDATER_calculateStateCount(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry)
{
    UINT16 count = 0;
    for(int i = 0; i< MAX_NUM_REGISTERED_SENSORS;i++)
    {
        if(mapEntry->sensors[i].valid == true)
        {
            count++;
        }
    }
    return count;
}
static ERRG_codeE METADATA_UPDATER_getUartStatus(INU_metadata_UART_Debug *uartDebug)
{
    nuserial_getUartDebugInformation(&uartDebug->txCounter,&uartDebug->rxCounter,&uartDebug->crcErrors,&uartDebug->txTransmitTime_ns,&uartDebug->lastSyncPacketTimestamp_us);
    return INU_METADATA__RET_SUCCESS;
}
ERRG_codeE METADATA_UPDATER_prepareStandardProtobufPacket( INU_Metadata_T *metadata , METADATA_UPDATER_prepareStandardParams *params)
{
    /*
    metadata[buffer+1 + i] = frameID + i + 1
    If we miss interrupts then the frame ID will still be okay ,
    although the timestamp and serialization_counter won't be
    */
    metadata->protobuf_packet.metadatacounter = params->frameID;
    metadata->protobuf_packet.protocol_version = inu_metadata__getProtocol_Version();
    metadata->protobuf_packet.timestamp_ns.nanoseconds = params->currentTs;
    metadata->protobuf_packet.has_timestamp_ns = true;
    /*Let the host know if we're the master or slave sending this message*/
    if (HELSINKI_getMaster())
    {
        metadata->protobuf_packet.sender = INU_metadata_HardwareType_MASTER;
    }
    else
    {
        metadata->protobuf_packet.sender = INU_metadata_HardwareType_SLAVE;
    }
    if (!params->overwriteSensorMetadata)
    {
        /*State_count is also overwritten to be zero for the special CVA case, */
        metadata->protobuf_packet.state_count = METADATA_UPDATER_calculateStateCount(params->mapEntry);
        /*Prepare sensor metadata using our Helsinki.c map */
        metadata->protobuf_packet.sensors_count = 0;
        if (params->entry->mapentry->sensors[0].valid)
        {
            metadata->protobuf_packet.sensors[metadata->protobuf_packet.sensors_count] = params->entry->mapentry->sensors[0].name;
            metadata->protobuf_packet.sensors_count++;
        }
        if (params->entry->mapentry->sensors[1].valid)
        {
            metadata->protobuf_packet.sensors[metadata->protobuf_packet.sensors_count] = params->entry->mapentry->sensors[1].name;
            metadata->protobuf_packet.sensors_count++;
        }
    }
    /* For the special case of the CVA metadata, we will NOT use the map to calculate the sensor metadata*/
    else
    {
        /*overwriteSensorMetadata is set to be false for the special CVA metadata case where we just want to send
        protobuf_packet.sensors[metadata->protobuf_packet.sensors values and not metadata->protobuf_packet.state  */
        LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Using overwritten state and sensor count: Sensor Count: %lu, Sensor Type: %lu, Protobuf state count: %lu \n", metadata->protobuf_packet.sensors_count, metadata->protobuf_packet.sensors[0].sensorType, metadata->protobuf_packet.state_count);
    }
    /*These two values are overwritten by the DMAC */
    metadata->timestamp = params->entry->metadataUpdates_Counter;
    metadata->frameID = params->frameID;
    metadata->serialization_counter = params->entry->metadataUpdates_Counter;
    metadata->framebuffer_offset = params->frameBufferOffset;
    /*We have debug information on the UART connection*/
    // TODO: Crashes happen here when using GCC 12, we should debug why!!
   // METADATA_UPDATER_getUartStatus(&metadata->protobuf_packet.uartDebug);
    metadata->protobuf_packet.has_uartDebug = true;
    return INU_METADATA__RET_SUCCESS;
}
/*For some sensor registers, updates take more than 1 frame. 
For example the AR2020 exposure  and Analogue gain take 2 frames to take effect
Therefore we need a way of making sure that we keep the metadata update thread running until we have serialized data for the N+2 frame

To do this, I will mark the N+2/N+3/N+4.. buffer as pending

There is a potential optimization that has been skipped:

In general we serialize 3/4 buffers (N+1,N+2,N+3) (skipping N) and this means we don't need to have this system to handle the case where we have to serialize N+1/N+2/N+3 buffers
However, we do actually need to shift what's in the metadata buffers like the following:
N+3 metadata moves to  N+2
And N+2 metadata moves to N+1 

So this is why I have done it like this, it's a bit more complex but it handles making sure that we keep the metadata thread running when its needed and also shifting the metadata buffers when needed

*/

bool METADATA_UPDATER_getFutureUpdatesPending(const METADATA_UPDATER_channelHandleT * entry)
{
    return entry->metadataDelayedUpdateBitFlag > 0;

}
void METADATA_UPDATER_markFutureUpdatePending( METADATA_UPDATER_channelHandleT  * const entry, const METADATA_SENSOR_UPDATER_frameTimingT INDEX)
{
    if(!entry)
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"NULL pointer for entry \n");
        return;
    }
    LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Marking future update pending for %lu \n",INDEX);
    if(INDEX >= MAX_NUM_BUFFERS_LOOPS)
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Index out of bounds \n");
        return;
    }
    entry->metadataDelayedUpdateBitFlag |= (1 << INDEX);
}
/*Once we have finished serializing the metadata update, we will shift each entry in metadataDelayedUpdate to the right and we will also shift the metadata buffers in the same way*/
void METADATA_UPDATER_shiftMetadataBuffers( METADATA_UPDATER_channelHandleT  * const entry)
{
    /*This function is used to shift metadata buffers, it works like this:
    Lets say that you have an N+2 update, you will mark this by setting bit 1 in metadataDelayedUpdateBitFlag to be true
    This function will run and you now have outgoingSensorUpdates[0] being the value that was previously held in outgoingSensorUpdates[1], this means that the next time you serialize metadata, you will serialize the original
    N+2 update
    Then you will skip running anything in this function since metadataDelayedUpdateBitFlag will be set to 1
    */
    if(!entry)
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"NULL pointer for entry \n");
        return;
    }
    LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Metadata delayed update bit flag = %x \n", entry->metadataDelayedUpdateBitFlag);
    if(entry->metadataDelayedUpdateBitFlag > 1) /*We only need to do the shifting if we have updates >  N+1 that are pending*/
    {
        LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Shifting metadata buffers \n");
        // I expect this to be quite expensive to do but its the simplest solution
        for(int i = 0; i < MAX_NUM_BUFFERS_LOOPS - 1; i++)
        {
                LOGG_PRINT(LOG_DEBUG_E,NULL,"Shifting at position %lu \n", i);
                entry->outgoingSensorUpdates[i] = entry->outgoingSensorUpdates[i+1];
        }
        #ifdef ENABLE_METADATA_DEBUG
        printf("SMB \n");
        #endif
    }
    entry->metadataDelayedUpdateBitFlag = (entry->metadataDelayedUpdateBitFlag >> 1); 
    LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Metadata delayed update bit flag = %x \n", entry->metadataDelayedUpdateBitFlag);
   
}

static void METADATA_UPDATER_markReadoutNeeded(METADATA_UPDATER_channelHandleT * entry, UINT32 INDEX)
{
     entry->metadataBufferPendingRead[INDEX] = 1;
}
static void METADATA_UPDATER_markReadoutDone(METADATA_UPDATER_channelHandleT * entry, UINT32 INDEX)
{
     entry->metadataBufferPendingRead[INDEX] = 0;
}
static MEM_POOLG_bufDescT* METADATA_UPDATER_GetNewMetadataFIFOPtr(METADATA_UPDATER_channelHandleT *entry,
    UINT32 frameID, UINT32 eofFrameID)
{
    UINT32 index = entry->metadata_isp_fifo_status.head;
    MEM_POOLG_bufDescT* bufferPtr = entry->metadata_isp_fifo[index].metadata_buffDescListP;
    entry->metadata_isp_fifo[index].startFrameID = frameID;
    entry->metadata_isp_fifo[index].startEOFFrameID = eofFrameID;
    entry->metadata_isp_fifo[index].useCount = 0;
    if(entry->metadata_isp_fifo_status.numEntries < METADATA_FIFO_SIZE )
    {
        entry->metadata_isp_fifo_status.numEntries++;
    }
    entry->metadata_isp_fifo_status.head = (entry->metadata_isp_fifo_status.head + 1) % METADATA_FIFO_SIZE;
    #if 0
    if(entry->mapentry->axiReader == ISP1_READER)
        printf("A:%lu,%lu,%lu \n",index,frameID,frameID );
    #endif
   // printf("Added entry at %lu with frame ID %lu, numEntries %lu \n", index,frameID,entry->metadata_isp_fifo_status.numEntries );
    return bufferPtr;
}
static METADATA_FIFO_Entry* METADATA_UPDATER_GetBufferPointerUsingFrameID(METADATA_UPDATER_channelHandleT *entry,
    UINT32 frameID, FIFO_SEARCH_METHOD_e searchMethod)
{
    UINT32 foundindex = 0;
    INT32 inFrameID = (INT32) frameID;
    INT32 largestFrameID = - 1;
    for(unsigned int i =0; i < entry->metadata_isp_fifo_status.numEntries; i++)
    {   
        INT32 foundFrameID = (INT32)entry->metadata_isp_fifo[i].startFrameID;
        if(searchMethod == METADATA_SearchUsingEOFFrameID )
         foundFrameID = (INT32)entry->metadata_isp_fifo[i].startEOFFrameID;
        //printf("%lu,%lu,%lu \n",i,foundFrameID,frameID);
        if(inFrameID >= foundFrameID && foundFrameID > largestFrameID)
        {
            largestFrameID = foundFrameID;
            foundindex = i;
        }
    }
    #if 0
    if(entry->mapentry->axiReader == ISP1_READER)
        printf("D:%lu,%lu,%lu \n",foundindex,inFrameID,frameID );
    #endif
    //printf("Found entry at %ld with frame ID %d for %lu \n", foundindex,inFrameID,frameID);
    return &entry->metadata_isp_fifo[foundindex];
}

/* @brief Updates metadata on a SOF interrupt or writer operate function for a DMA channel
   @param channel Channel to deal with interrupt for
   @param timestamp - Timestamp of the interrupt
   @param writerOperateFunction - Setting this to true will mean that the AXI Reader current loop index is used to calculate the next buffer instead of the frame ID. 
                                        In general this should be true when this called from an AXI writer operate function where the Frame ID won't be deterministic as it can change depending 
                                        on which reeader is used to inject metadata
   @param overwriteSensorMetadata - Set to true to skip the calculation of the number of sensors and number of state values sent (True is intended for CVA metadata)
   @return Returns an error code
*/
static ERRG_codeE METADATA_UPDATER_updateMetadata(METADATA_UPDATER_channelHandleT *entry, UINT64 timestamp,
                                                     const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry, 
                                                     const bool writerOperateFunction, const bool overwriteSensorMetadata,
                                                     const bool ISP_Channel, UINT32 notification_frame_id)
{
    static UINT32 counter = 0;
    static UINT64 previousTimestamp = 0;
    UINT32 nextBuffer = 0;
    UINT32 current_buffer =0;
    //printf("ISP_Channel %lu \n", ISP_Channel);
    if (1)
    {
        UINT32 frameID = 0;
        ERRG_codeE ret = INU_METADATA__RET_SUCCESS;
        /*Use the AXI Reader frame id register to get the frame ID */
        /*Unused but this can be used as an alternative way of calculating the next buffer index */
        if ((!ISP_Channel) && (entry == NULL || entry->channel->metadataDMAChannelHandle == NULL))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "NULL pointer for channel found %x,%x \n", entry, entry->channel->metadataDMAChannelHandle);
            return INU_METADATA__ERR_INVALID_ARGS;
        }
        UINT32 loopIndex  = 0;
        int numberLoops  = (MAX_NUM_BUFFERS_LOOPS  - 1);
        if(ISP_Channel)
        {
             /*For the ISP we only need to do one loop*/
            numberLoops = 1;
            METADATA_UPDATER_getFullFrameIDWithOverFlowCounter(mapEntry, &frameID);
        }
        else
        {
            /*Its safe to update all metadata buffers, we lock a mutex when serializing*/
            loopIndex = METADATA_UPDATER_calculateLoopIndex(entry->channel->metadataDMAChannelHandle);
            ret = METADATA_UPDATER_getFrameID(mapEntry, &frameID);
        }
        current_buffer = frameID % MAX_NUM_BUFFERS_LOOPS;
        nextBuffer = METADATA_UPDATER_calculateNextBufferIndex(entry,writerOperateFunction);
        if(writerOperateFunction)
        {   
            /*Only update the next buffer for the case of the writer operate call*/
            numberLoops = 1;
            LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Writer_Operate: Start of frame ID is %lu, currentBuffer:%lu, nextBuffer %lu, loopIndex %lu \n", frameID, current_buffer, nextBuffer, loopIndex);
        }
        LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Start of frame ID is %lu, currentBuffer:%lu, nextBuffer %lu, loopIndex %lu \n", frameID, current_buffer, nextBuffer, loopIndex);
        if (ERRG_SUCCEEDED(ret))
        {
            /*By locking this mutex we are now safe to access metadata for all four protobuf metadata structure buffers
                        This is because the Protobuf structures are only accessed by software so there's no HW/SW concurrency issues
                        and just SW<->SW concurrency issues that can be solved by locking a mutex.
                        We should lock it here so that we don't give a chance to the injector node to jump in whilst running these steps
            */
            METADATA_UPDATER_LockMutex(entry); 
            for (int i = 0; i < numberLoops ; i++)
            {
                UINT32 metadatasize = 0;
                MEM_POOLG_bufDescT         *bufferDescriptor = NULL;
                int frameBufferOffset = 0;
                const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
                /*Metadata buffers are always 0 = N+1, 1 = N+2 etc..*/
                const UINT32 METADATA_STRUCT_INDEX = (METADATA_UPDATER_calculateMetadataBuffer(entry,false) + i);
                if(INDEX >= MAX_NUM_BUFFERS_LOOPS)
                {
                    LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Index out of bounds \n");
                    return INU_METADATA__ERR_INVALID_ARGS;
                }
                /*Calculate the metadata size*/  
                if (ISP_Channel)
                {
                    assert(entry->handle);
                    /* We will use the metadata buffers outside of the DMA channel handle for the ISP metadata*/
                    metadatasize = MEM_POOLG_getBufSize(entry->handle);
                    UINT32 currentEOFCounter = 0;
                    METADATA_UPDATER_getEOFCounter(mapEntry,&currentEOFCounter);
                    bufferDescriptor = METADATA_UPDATER_GetNewMetadataFIFOPtr(entry,frameID+1,currentEOFCounter+1);
                    frameBufferOffset = 0;
                }
                else
                {
                    /* We will use the metadata buffers inside the DMAC structure for the standard metadata*/
                    metadatasize = MEM_POOLG_getBufSize(entry->channel->metadataDMAChannelHandle->metadata_memPoolHandle);
                    bufferDescriptor = entry->channel->metadataDMAChannelHandle->metadata_buffDescListP[INDEX];
                    frameBufferOffset = entry->channel->dmaChannelHandle->framebuffer_offset;
                }
                    /*Get the current timestamp */
                UINT64 currentTs;
                OS_LYRG_getTimeNsec(&currentTs);
                INU_Metadata_T *metadata = &entry->outgoingSensorUpdates[METADATA_STRUCT_INDEX];
               LOGG_PRINT(LOG_DEBUG_E,NULL,"(R: %lu F %lu) %lu,%lu,%lu \n",entry->mapentry->axiReader, frameID,INDEX, metadata->protobuf_packet.state[0].sensor_metadata.roi.x_start,metadata->protobuf_packet.state[0].sensor_metadata.roi.y_start );
                METADATA_UPDATER_prepareStandardParams params;
                
                params.bufferCounter = i;
                params.entry = entry;
                params.frameBufferOffset = frameBufferOffset;
                params.frameID  = frameID;
                params.INDEX = INDEX;
                params.ISP_Channel = ISP_Channel;
                params.mapEntry = mapEntry;
                params.overwriteSensorMetadata = overwriteSensorMetadata;
                params.currentTs = currentTs;
                                                            
                /*Prepare the protobuf packet*/
                METADATA_UPDATER_prepareStandardProtobufPacket(metadata, &params);
                /*Its safe to always update N+3 buffers as we have hardware doing the injection*/
                METADATA_target_serialize(bufferDescriptor, metadata, metadatasize);
                /*For the ISP we can mark all buffers as updated as we don't have Four HW buffers.
                    Instead we have a FIFO and we just need to add a new entry to the FIFO
                    Marking all buffers as cleared saves CPU time (Roughly 2% idle time)
                    
                    For the non ISP channels we still need to only update 3/4 buffers to avoid SW writing to a buffer that is being used by the DMAC */
                if(!ISP_Channel)
                    METADATA_UPDATER_clearPendingBufferUpdate(entry,INDEX);
                else
                    for(int i =0; i < MAX_NUM_BUFFERS_LOOPS; i ++)
                    {
                        METADATA_UPDATER_clearPendingBufferUpdate(entry,i);
                    }
                    
                LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Serializing to buffer 0x%x", bufferDescriptor);
                /*Calculate serialization time */
                UINT64 currentTs_post;
                OS_LYRG_getTimeNsec(&currentTs_post);
                UINT64 serialization_time = currentTs_post - currentTs;
                /*Print serialization time */
                LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "loop index %lu, Serialization time :%llu ns\n", i, serialization_time);
            }
            METADATA_UPDATER_shiftMetadataBuffers(entry); /* See comments above this function if you want an explanation of what this code does*/
            METADATA_UPDATER_UnlockMutex(entry);
            OS_LYRG_getTimeNsec(&previousTimestamp);
            entry->metadataUpdates_Counter = entry->metadataUpdates_Counter + 1;
            entry->previousFrameID++;
            return INU_METADATA__RET_SUCCESS;
        }
        else
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Error updating metadata due to invalid AXI reader :%llu\n", timestamp);
        }
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Error updating metadata due to null pointers :%llu\n", timestamp);
    }
    return INU_METADATA__ERR_INVALID_ARGS;
}
ERRG_codeE METADATA_UPDATER_LockMutex(METADATA_UPDATER_channelHandleT *entry)
{
    return (ERRG_codeE)OS_LYRG_lockMutex(&entry->metadataMutex);
}
 ERRG_codeE METADATA_UPDATER_UnlockMutex(METADATA_UPDATER_channelHandleT *entry)
{
    return (ERRG_codeE)OS_LYRG_unlockMutex(&entry->metadataMutex);
}
static ERRG_codeE METADATA_UPDATER_initISPmetadataPool(METADATA_UPDATER_channelHandleT *entry)
{
    MEM_POOLG_cfgT cfg;
    ERRG_codeE ret = INU_METADATA__RET_SUCCESS;
    const INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
    /*leave two buffers spare*/
    UINT32 numBuffers = (UINT32)(MAX_NUM_BUFFERS_LOOPS+2);
    ret = METADATA_TARGET_CONFIG_getMemPoolConfig(&cfg,modelType,numBuffers);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Couldn't allocate mem pool with %lu buffers \n",numBuffers);
        assert(0);
    }
    ret = MEM_POOLG_initPool(&entry->handle,&cfg);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Couldn't allocate mem pool with %lu buffers \n",numBuffers);
        assert(0);
    }
    /*Allocate MAX_NUM+BUFFERS_LOOPS*/
    for(int i = 0; i < METADATA_FIFO_SIZE;i++)
    {
        /*Allocate our buffers for the ISP streams*/
        UINT32 size = MEM_POOLG_getBufSize(entry->handle);
        entry->metadata_isp_fifo[i].startFrameID = 0;
        entry->metadata_isp_fifo[i].useCount = 0;
        ret = MEM_POOLG_alloc(entry->handle, size, &entry->metadata_isp_fifo[i].metadata_buffDescListP);
        if(ERRG_FAILED(ret))
        {
            assert(0);
        }
    }
    entry->metadata_isp_fifo_status.numEntries = 0;
    entry->metadata_isp_fifo_status.head = 0;
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Initialize ISP metadata \n");
    return ret;
}
ERRG_codeE METADATA_UPDATER_initISPCBQueues(METADATA_UPDATER_channelHandleT *entry)
{
  entry->isp_expoQ.maxMsgs = METADATA_ISP_PARAM_QUEUE_SIZE;
  entry->isp_expoQ.msgSize = sizeof(METADATA_ISP_Expo_Queue_Entry);
  snprintf(entry->isp_expoQ.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/ISPExpoQ_%lu", entry->mapentry->channelID);
  ERRG_codeE ret = (ERRG_codeE)OS_LYRG_createMsgQue(&entry->isp_expoQ, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
  if (ERRG_FAILED(ret))
  {
    LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create ISP Expo Queue \n");
  }
  entry->isp_wbQ.maxMsgs = METADATA_ISP_PARAM_QUEUE_SIZE;
  entry->isp_wbQ.msgSize = sizeof(METADATA_ISP_Wb_Queue_Entry);
  snprintf(entry->isp_wbQ.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/ISPWBQ_%lu", entry->mapentry->channelID);
  ret = (ERRG_codeE)OS_LYRG_createMsgQue(&entry->isp_wbQ, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
  if (ERRG_FAILED(ret))
  {
    LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create ISP WB Queue \n");
  }
  return ret;
}
static void METADATA_UPDATER_initAR2020GainQueue(METADATA_UPDATER_channelHandleT *entry)
{
    // Only supported for ISP streams
    if((entry->mapentry->axiReader!=ISP0_READER) && (entry->mapentry->axiReader!=ISP1_READER))
    {
        return;
    }
    // Queue for delayed Gain updates
    entry->AR2020_gainQ.maxMsgs = 5;
    entry->AR2020_gainQ.msgSize = sizeof(METADATA_AR2020Gain_Queue_Entry);
    snprintf(entry->AR2020_gainQ.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/AR2020GainQ");
    ERRG_codeE ret = (ERRG_codeE)OS_LYRG_createMsgQue(&entry->AR2020_gainQ, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
    if (ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create AR2020 Gain Queue \n");
    }
    else
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "Created AR2020 Gain Queue \n");
    }
}
/**
 * @brief 
 * Creates a Queue (for sensor updates) and a mutex that can be used (NOT CURRENTLY USED)
 * 
 *
 * @param entry Pointer to a METADATA_UPDATER channel 
 * @return Returns an error code
 */
static ERRG_codeE METADATA_UPDATER_initQueueAndMutex(METADATA_UPDATER_channelHandleT *entry)
{
    /*Creates a Queue (for sensor updates purposes) and a mutex that can be used for other variables that need thread safety */
    ERRG_codeE ret = (ERRG_codeE) OS_LYRG_aquireMutex(&entry->metadataMutex);
    METADATA_UPDATER_initISPmetadataPool(entry);
    METADATA_UPDATER_initISPCBQueues(entry);
    METADATA_UPDATER_initAR2020GainQueue(entry);
    if (ERRG_SUCCEEDED(ret))
    {
        for (int i = 0; i < MAX_NUM_REGISTERED_SENSORS; i++)
        {
            UINT64 before,after = 0;
            entry->sensors[i].sensorQueue.maxMsgs = SNSR_UPDATE_QUEUE_SIZE;
            entry->sensors[i].sensorQueue.msgSize = sizeof(INU_metadata_State);
            snprintf(entry->sensors[i].sensorQueue.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/SnsrUpdateQueue_%lu_%lu", entry->mapentry->channelID, i);
            
            ret = (ERRG_codeE) OS_LYRG_createMsgQue(&entry->sensors[i].sensorQueue, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
            /*We need to store the first frame ID so that we can accurately workout buffer indexes.
            This is because the GAZE ROI can start with a non-zero frame ID. 
            */
            entry->firstFrameID = 0;
            entry->timestamp_0 = 0;
            entry->previousFrameID = 0;
            entry->detectedDroppedFrameCounter = 0;
            entry->compensatedFrameCounter = 0;
            entry->droppedFrameRecordCounter = 0;
            entry->excessDequeues = 0;
            memset(&entry->droppedFrameRecord,0,sizeof(entry->droppedFrameRecord));
            for(int j = 0; j <MAX_NUM_BUFFERS_LOOPS; j ++ )
            {
                METADATA_UPDATER_markReadoutDone(entry,j);
            }
            METADATA_UPDATER_setPendingBufferUpdatesForAllBuffers(entry); 
            /*For the ISP streams we will serialize our buffers to this memory pool*/
            #ifdef ENABLE_ROI_QUEUE
            if (ret != SUCCESS_E)
            {
                LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create Queue \n");
            }            
            /*ROI updates must take place after the frame done callback */
            entry->sensors[i].ROIQueue.maxMsgs = ROI_UPDATE_QUEUE_SIZE;
            entry->sensors[i].ROIQueue.msgSize = sizeof(INU_metadata_ROI);
            snprintf(entry->sensors[i].ROIQueue.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/ROIUpdateQueue_%lu_%lu", entry->mapentry->channelID, i);
            
            ret = OS_LYRG_createMsgQue(&entry->sensors[i].ROIQueue, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
            if (ret != SUCCESS_E)
            {
                LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create ROI Queue \n");
            }
         
            #endif
        }
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create mutex \n");
    }
    return ret;
}
/**
 * @brief Looks for an existing entry in our list of registered channels
 *
 * @param entry Map Entry
 * @param index This pointer is updated with the index of the metadatachannel if a match is found
 * @return ERRG_codeE
 */
static ERRG_codeE METADATA_UPDATER_checkForExistingEntry(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entry, UINT32 *index)
{
    for (int i = 0; i < registeredDMAChannels; i++)
    {
        if (metadatachannels[i].mapentry == entry)
        {
            *index = i;
            return INU_METADATA__RET_SUCCESS;
        }
    }
    return INU_METADATA__ERR_METADATA_NO_EXISTING_ENTRY;
}
/**
 * @brief Clears the metadata outgoing buffers so that we aren't sending more than we intend
 * 
 * @param entry List of entries to clear the outgoingSensorUpdates for 
 * @return ERRG_codeE 
 */
static ERRG_codeE METADATA_UPDATER_clearMetadataOutgoingBuffers( METADATA_UPDATER_channelHandleT *entry)
{
    for(int i = 0; i < (MAX_NUM_BUFFERS_LOOPS-1); i++)
    {
       memset(&entry->outgoingSensorUpdates[i],0,sizeof(INU_Metadata_T));
       entry->outgoingSensorUpdates[i].framebuffer_offset = 0;
       entry->outgoingSensorUpdates[i].frameID = 0;
       entry->outgoingSensorUpdates[i].serialization_counter = 0;
       entry->outgoingSensorUpdates[i].timestamp = 0;
       entry->outgoingSensorUpdates[i].protobuf_packet.state_count=0; 
       entry->outgoingSensorUpdates[i].protobuf_packet.has_timestamp_ns =false; 
       entry->outgoingSensorUpdates[i].protobuf_packet.metadatacounter  = 0; 
       entry->outgoingSensorUpdates[i].protobuf_packet.protocol_version  = 0;
    }
    return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Initializes the SOF interrupt queue
 * 
 *
 * @return Returns an error code
 */
static ERRG_codeE METADATA_UPDATER_initQueue()
{
    sofInterruptQueue.maxMsgs = SOF_INTERRUPT_QUEUE_SIZE;
    sofInterruptQueue.msgSize = sizeof(METADATA_SENSOR_UPDATER_interruptNotification);
    snprintf(sofInterruptQueue.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/SOfInterruptNotification");
    ERRG_codeE ret = (ERRG_codeE)OS_LYRG_createMsgQue(&sofInterruptQueue, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
    if (ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create SOF interrupt queue \n");
    }
    for (int i = 0; i < NUMBER_SLU_QUEUES; i++)
    {
        frameIDStatus.slu_queues_frame_id[i].maxMsgs = SOF_INTERRUPT_QUEUE_SIZE;
        frameIDStatus.slu_queues_frame_id[i].msgSize = sizeof(METADATA_SENSOR_UPDATER_interruptNotification);
        frameIDStatus.previous_slu_frame_ID[i] = 0;
        frameIDStatus.frame_id_counter[i] = 0;
        snprintf(frameIDStatus.slu_queues_frame_id[i].name, OS_LYRG_MAX_MQUE_NAME_LEN, "/FrameIDQueue");
        ERRG_codeE ret = (ERRG_codeE) OS_LYRG_createMsgQue(&frameIDStatus.slu_queues_frame_id[i], OS_LYRG_MULTUPLE_EVENTS_NUM_E);
        if (ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create Frame ID interrupt queue \n");
        }
        frameIDStatus.slu_queues_frame_id_eof[i].maxMsgs = SOF_INTERRUPT_QUEUE_SIZE;
        frameIDStatus.slu_queues_frame_id_eof[i].msgSize = sizeof(METADATA_SENSOR_UPDATER_interruptNotification);
        snprintf(frameIDStatus.slu_queues_frame_id_eof[i].name, OS_LYRG_MAX_MQUE_NAME_LEN, "/EOFFrameIDQueue");
        ret = (ERRG_codeE)OS_LYRG_createMsgQue(&frameIDStatus.slu_queues_frame_id_eof[i], OS_LYRG_MULTUPLE_EVENTS_NUM_E);
        if (ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create EOF Frame ID interrupt queue \n");
        }
        #ifdef OLD_METHOD
        syncedCmdQueue[i].delayedCommandQueue.maxMsgs = SOF_INTERRUPT_QUEUE_SIZE;
        syncedCmdQueue[i].delayedCommandQueue.msgSize = sizeof(INU_Metadata_T);
        snprintf(syncedCmdQueue[i].delayedCommandQueue.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/SyncedCMD");

        ret =(ERRG_codeE) OS_LYRG_createMsgQue(&syncedCmdQueue[i].delayedCommandQueue, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
        if (ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create EOF Frame ID interrupt queue \n");
        }
        #else
                syncedCmdQueue[i].delayedCommandQueue = std::make_shared<blocking_queue::queue<INU_Metadata_T>>(SOF_INTERRUPT_QUEUE_SIZE);
        #endif
    }
    return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Creates the metadata updater thread
 * 
 *
 * @return Returns an error code
 */
static ERRG_codeE METADATA_UPDATER_createThread()
{
   ERRG_codeE           retCode = INU_METADATA__RET_SUCCESS;
   OS_LYRG_threadParams threadParams;
   /*Init the Queue for metadata updates*/
   METADATA_UPDATER_initQueue();
   /*Thread parameters*/
   threadParams.func = METADATA_UPDATER_updateSensorThread;
   threadParams.id = OS_LYR_SENSOR_UPDATER_THREAD_ID_E;
   threadParams.event = NULL;
   threadParams.param = NULL;

   LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Starting metadata sensor updater thread\n");

   //create thread
   METADATA_SENSOR_UPDATER_threadHandle = OS_LYRG_createThread(&threadParams);
   if (METADATA_SENSOR_UPDATER_threadHandle == NULL)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to create thread\n");
      return INU_METADATA__ERR_ILLEGAL_STATE;
   }
   return retCode;
}

ERRG_codeE  METADATA_getChannelHandleByReaderID(METADATA_UPDATER_channelHandleT **ptr, int READER_ID)
{
    for (int i = 0; i < registeredDMAChannels; i++)
    {
        if((metadatachannels[i].mapentry->axiReader == (UINT32)READER_ID))
        {
            *ptr = &metadatachannels[i];
            return INU_METADATA__RET_SUCCESS;
        }

    } 
    return INU_METADATA__ERR_INVALID_ARGS;
}
static int METADATA_UPDATER_removeFromDelayedQueue(UINT32 slu)
{
    INU_Metadata_T synced_cmd = ZEROED_OUT_METADATA;
    UINT32 length = sizeof(synced_cmd);
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Checking for delayed Commands at %lu \n", slu);
    if(slu > NUMBER_SLU_QUEUES)
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Invalid SLU index \n");
        return -1;
    }
    if(syncedCmdQueue[slu].delayedCommandQueue->size() == 0)
        return 0; /*No messages to handle*/

    /*Process a command at a time*/
    bool status = syncedCmdQueue[slu].delayedCommandQueue->pop_timed(synced_cmd,1);
    if(!status)
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Error reading from synced command queue \n");
        return -1;
    }
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Found delayed command for SLU %lu, state_count %lu, has_ispo %lu, Target HW %lu  \n", slu, synced_cmd.protobuf_packet.state_count, 
        synced_cmd.protobuf_packet.state[1].sensor_metadata.has_isp_expo, synced_cmd.protobuf_packet.state[1].target);
    /*Serialize and send over UART*/
    std::array<char,1024> reserialization_buffer;
    auto ret = inu_metadata__serialize(reserialization_buffer.data(), reserialization_buffer.size(), &synced_cmd);
    const int minimalMetadataSize = inu_metadata__getMetadataSizeWithoutLinePadding(reserialization_buffer.data(), reserialization_buffer.size());
    if (minimalMetadataSize > 0)
    {
        METADATA_UART_repeatSensorUpdateFromMasterToSlave(reserialization_buffer.data(), minimalMetadataSize);
    }
    /*Handle message internally*/
    return METADATA_UPDATER_process_cmd(&synced_cmd);
    
}
static int METADATA_UPDATER_handleNotification(const METADATA_SENSOR_UPDATER_interruptNotification * notification)
{
    UINT64 currentTime = 0;
    OS_LYRG_getUsecTime(&currentTime);
    /*Searches in registered metadata channels for a registered entry with the slu set to slu */
    const UINT32 slu = notification->slu;
    const UINT64 timestamp_us  = notification->timestamp/1000; /*Convert from ns to us*/
    const UINT64 timestamp_userspace_send = notification->timestamp_userspace_send;
    LOGG_PRINT(LOG_DEBUG_E, NULL, "Metadata SOF interrupt timestamp %llu for SLU %lu, %lu registered channels, Delta %llu\n", notification->timestamp, notification->slu, registeredDMAChannels,currentTime-notification->timestamp_userspace_send );
    INT64 timeDiff_us = (currentTime - timestamp_us); /*Calculate the time difference between the SOF interrupt and the currentTime*/
    uint8_t metadataUpdated = 0;     
    if(timeDiff_us > MAXIMUM_TIME_DIFF_USEC)
    {
        LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Interrupt latency too high, ignoring sensor update  %lld \n", timeDiff_us);
        // printf("%lld \n",timeDiff_us );
        return -1;
    }
    if (registeredDMAChannels <= 0)
    {

        LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "No metadata channels enabled \n");
        return -1;
    }
    for (int i = 0; i < registeredDMAChannels; i++)
    {
        if (((metadatachannels[i].inUse == true) 
            || ((metadatachannels[i].mapentry->axiReader==ISP0_READER) || (metadatachannels[i].mapentry->axiReader==ISP1_READER))  )  /* We don't have metadata working for the ISP streams yet but we still wish to use Sensor updates for ISP0 and ISP1 */
            && metadatachannels[i].mapentry != NULL)
        {
            if ((metadatachannels[i].mapentry->slu == (int)slu) && metadatachannels[i].mapentry->slu!= DISABLE_SLU_SOF_ISR_METADATA_PROCESSING)
            {
                LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Found DMA channel to update metadata for SLU:%lu\n", slu);
                if(metadatachannels[i].mapentry->writerID !=  DISABLE_WRITER_OPERATE_METADATA_PROCESSING)
                {
                    /*Updating the metadata in two places will be dangerous*/
                    LOGG_PRINT(LOG_ERROR_E,NULL,"WARNING, Metadata updates within the writer operate function are enabled as well as SOF ISR metadata updates \n");
                }
                /*lock the mutex incase another thread is updating the metadata at the same time */
                bool updatePerformed = false;
                METADATA_SENSOR_UPDATE_handleAR2020DelayedGainUpdate(&metadatachannels[i]);
                for(int j=0; j < MAX_NUM_REGISTERED_SENSORS;j++)
                {
                    ERRG_codeE ret = METADATA_SENSOR_UPDATER_performSensorUpdate(&metadatachannels[i],j,notification);
                    if(ERRG_SUCCEEDED(ret))
                        updatePerformed = true;
                    /*Handle the special case of the downscaled slave ISP*/
                    METADATA_ISP_UPDATER_set_isp_expo_callback(&metadatachannels[i],j);
                    METADATA_ISP_UPDATER_live_WB_Expo_metadata(&metadatachannels[i],j);
                }

                /* For the ISP Streams, we need to serialize our buffers to a different location as we are using a SW node for metadata insertion and not the DMAC
                */
                bool ISP_Channel = ((metadatachannels[i].mapentry->axiReader==ISP0_READER) || (metadatachannels[i].mapentry->axiReader==ISP1_READER));

                LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Updating Metadata. slu - %d\n", slu);
                METADATA_UPDATER_updateMetadata(&metadatachannels[i], notification->timestamp, metadatachannels[i].mapentry,false,false,ISP_Channel,notification->frameID);   /*Update the metadata for the NEXT Buffers only*/

            }
            else
            {
                LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Metadata SLU does not match %lu:%lu \n", metadatachannels[i].mapentry->slu, slu);
            }
        }
        else
        {
            LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Metadata channel %lu not enabled\n", slu);
        }
    }

    /*UART repeat any delayed comamands and then handle the delayed command on the next SOF internally*/
    METADATA_UPDATER_removeFromDelayedQueue(notification->slu);   
    return 0;

}
static int METADATA_UPDATER_updateSensorThread(void *arg)
{
  while(1)
  {
    INT32 status = -1 ;
    UINT32 messageSize = sizeof(METADATA_SENSOR_UPDATER_interruptNotification);
    const METADATA_SENSOR_UPDATER_interruptNotification notification = {0,0,0,0,0,0,0};
    status = OS_LYRG_waitRecvMsg(&sofInterruptQueue,(uint8_t *)&notification,&messageSize);
    if ((status == SUCCESS_E) && (messageSize == sizeof(METADATA_SENSOR_UPDATER_interruptNotification)))
    {
        /*Errors are printed within METADATA_UPDATER_handleNotification */
        METADATA_UPDATER_handleNotification(&notification);
    }
  }
}
ERRG_codeE METADATA_UPDATER_storeDMAChannelPointerWithReaderID(const UINT8 readerID, const INU_DEFSG_moduleTypeE bootID,
                                                   CDE_MNGRG_channnelInfoT *channelHandle)
{
    if (registeredDMAChannels < MAX_REGISTERED_METADATA_CHANNELS)
    {
        const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entry = NULL;
        /*First we have to check that there's a metadata channel enabled */
        ERRG_codeE ret = METADATA_TARGET_CONFIG_getChannelMapEntryByReaderID(bootID, readerID, &entry);
        /*If the map entry exists then enable it */
        if (ERRG_SUCCEEDED(ret))
        {
            UINT32 index = 0;
            ERRG_codeE existingEntryRet = METADATA_UPDATER_checkForExistingEntry(entry, &index);
            /*If we already have an existing entry then we should update it's channel */
            if (existingEntryRet == INU_METADATA__RET_SUCCESS)
            {
                LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Existing entry found for DMA Handle:0x%x, storing at index:%lu  \n", channelHandle,index);
                metadatachannels[index].channel = channelHandle;
                metadatachannels[index].inUse = true;
            }
            else
            {
                /*Register a new entry */
                metadatachannels[registeredDMAChannels].channel = channelHandle;
                metadatachannels[registeredDMAChannels].inUse = true;
                metadatachannels[registeredDMAChannels].mapentry = entry;
                LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Registering entry for DMA Handle:0x%x, storing at index:%lu  \n", channelHandle,registeredDMAChannels);
                /*Clear metadata and create the queue */
                METADATA_UPDATER_clearMetadataOutgoingBuffers(&metadatachannels[registeredDMAChannels]);
                METADATA_UPDATER_initQueueAndMutex(&metadatachannels[registeredDMAChannels]);
                registeredDMAChannels++;
            }
            return INU_METADATA__RET_SUCCESS;
        }
        else
        {
            LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Cannot store DMA channel pointer because there's no configuration found for bootID:%lu,readerID:%lu \n", bootID, readerID);
        }
    }
    return INU_METADATA__ERR_METADATA_NO_EXISTING_ENTRY;
}
ERRG_codeE METADATA_UPDATER_storeSensorExposureAndGains(const INU_DEFSG_moduleTypeE bootID,const SENSORS_MNGRG_sensorInfoT *sensorInfo, UINT16 SensorID)
{

    if (registeredDMAChannels < MAX_REGISTERED_METADATA_CHANNELS)
    {
        #define MAX_ENTRIES 4
        UINT32 sensorIndexes[MAX_ENTRIES] = {0};
        const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entries[MAX_ENTRIES];
        /*First we have to check that there's a metadata channel enabled */
        int numberEntries = METADATA_TARGET_CONFIG_getChannelMapEntriesBySensorID(bootID, SensorID, sensorIndexes, entries, MAX_ENTRIES);
        for(int i =0; i< numberEntries; i ++)
        {
            const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entry = entries[i];
            UINT32 sensorIndex = sensorIndexes[i];
            if(sensorIndex > 1)
            {
                LOGG_PRINT(LOG_ERROR_E,NULL,"Illegal sensor Index %lu \n",sensorIndex);
                return INU_METADATA__ERR_ILLEGAL_STATE;
            }
            if(entry == NULL)
            {
                LOGG_PRINT(LOG_ERROR_E,NULL,"entry==NULL \n");
                return INU_METADATA__ERR_ILLEGAL_STATE;
            }
            /*If the map entry exists then store the exposure and gain */

                UINT32 index = 0;
                ERRG_codeE existingEntryRet = METADATA_UPDATER_checkForExistingEntry(entry, &index);
                if(index > MAX_REGISTERED_METADATA_CHANNELS)
                {
                    LOGG_PRINT(LOG_ERROR_E,NULL,"Index > MAX_REGISTERED_METADATA_CHANNELS \n");
                    return INU_METADATA__ERR_ILLEGAL_STATE;
                }
                if (existingEntryRet == INU_METADATA__RET_SUCCESS)
                {
                    UINT32 startX = 0;
                    UINT32 startY = 0;
                    /*     {0x3C6A, 2, 0x0590},  // X_OUTPUT_OFFSET2
                            {0x3C70, 2, 0x0430},  // Y_OUTPUT_OFFSET2*/
                    if(((sensorInfo->sensorCfg.sensorModel==2020)||(sensorInfo->sensorCfg.sensorModel==2021)))
                    {
                        startX = 0;
                        startY = 0;
                    }

                    /*Initialize each state within each metadata buffer*/
                    for(int buffer =0; buffer <MAX_NUM_BUFFERS_LOOPS;buffer++ )
                    {
                        INU_metadata_State *state = &metadatachannels[index].outgoingSensorUpdates[buffer].protobuf_packet.state[sensorIndex];
                        if(startX && startY)
                        {
                            LOGG_PRINT(LOG_INFO_E,NULL,"Setting AF crop position to be %x,%x \n", startX,startY);
                            state->sensor_metadata.AFCropPositon.StartX = 0;
                            state->sensor_metadata.AFCropPositon.StartY = 0;
                            state->sensor_metadata.has_AFCropPositon = true;
                        }
                        state->has_sensor_metadata = true;
                        state->sensor_metadata.has_exposure = true;
                        state->sensor_metadata.has_gain = true;
                        state->sensor_metadata.exposure.exposure = sensorInfo->sensorCfg.defaultExp;
                        state->sensor_metadata.gain.analogue_gain = sensorInfo->sensorCfg.defaultGain;
                        state->sensor_metadata.gain.digital_gain = sensorInfo->digitalGain[0];
                        state->sensor = entry->sensors[sensorIndex].name;
                        state->has_sensor = true;
                        LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Stored default exposure,%lu and gains,%.3f,%.3f for sensorID:%lu  \n",
                            state->sensor_metadata.exposure.exposure,state->sensor_metadata.gain.analogue_gain,state->sensor_metadata.gain.digital_gain, SensorID );
                    }

                }
        }
        
        
    }
    else
    {
        return INU_METADATA__ERR_INVALID_ARGS;
    }
    return INU_METADATA__RET_SUCCESS;
}
ERRG_codeE METADATA_UPDATER_storeGroupHandle(const INU_DEFSG_moduleTypeE bootID, IO_HANDLE sensorGroupP)
{
    if (registeredDMAChannels < MAX_REGISTERED_METADATA_CHANNELS)
    {
        const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entry = NULL;
        IO_HANDLE groupP = sensorGroupP;
        /*Iterate over group handle and store all of the sensor handles for the sensors*/
        inu_nodeH sensor = inu_node__getNextOutputNode(groupP, NULL);
        int count = 0;  /*Safety measure against an infinite loop*/
        while (sensor)
        {
            SENSORS_MNGRG_sensorInfoT *sensorInfoP = NULL;
            inu_sensor__getSensorHandle((inu_sensor *)sensor, (IO_HANDLE *)&sensorInfoP);
            if (sensorInfoP == NULL)
            {
                LOGG_PRINT(LOG_ERROR_E,NULL,"Null pointer detected for sensorInfoP\n");
                return INU_METADATA__ERR_ILLEGAL_STATE;
            }
            if(count > 12)
            {
                LOGG_PRINT(LOG_ERROR_E,NULL,"Infinite loop detected \n");
                return INU_METADATA__ERR_ILLEGAL_STATE;
            }

            const INU_DEFSG_senSelectE sensorSelect = sensorInfoP->sensorCfg.sensorSelect;
            LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Storing group pointer %p and looking for sensor %lu in our map \n", sensorGroupP,sensorSelect);
            #define MAX_ENTRIES 4
            UINT32 sensorIndexes[MAX_ENTRIES] = {0};
            const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entries[MAX_ENTRIES];
            int numberEntriesFound = METADATA_TARGET_CONFIG_getChannelMapEntriesBySensorID(bootID, sensorSelect, sensorIndexes, entries, MAX_ENTRIES);
            for(int i = 0; i < numberEntriesFound; i ++ )
            {   
                /*Set Entry */
                UINT32 index = 0;
                entry = entries[i];
                UINT32 sensorIndex=sensorIndexes[i];      
                ERRG_codeE existingEntryRet = METADATA_UPDATER_checkForExistingEntry(entry, &index);
                IO_HANDLE GroupHandle = NULL;
                ERRG_codeE ret_gp = inu_sensors_group__getSensorHandle((inu_sensors_group *)groupP,&GroupHandle); /* Store group handle */
                if(ERRG_FAILED(ret_gp))
                {
                    LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to get group handle \n");
                    return INU_METADATA__ERR_ILLEGAL_STATE;
                }
                /*If we already have an existing entry then we should update it's channel */
                if (existingEntryRet == INU_METADATA__RET_SUCCESS)
                {
                    metadatachannels[index].groupHandle = GroupHandle;
                    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Existing entry found for sensorID:%lu, storing sensorInfo=0x%x, groupHandle=0x%x  \n", sensorSelect, sensorInfoP,metadatachannels[index].groupHandle);
                    metadatachannels[index].sensors[sensorIndex].sensorInfo = sensorInfoP;
                    metadatachannels[index].sensors[sensorIndex].registered = true;
                }
                else
                {
                    /*Register a new DMA entry */
                    metadatachannels[registeredDMAChannels].sensors[sensorIndex].sensorInfo = sensorInfoP;
                    metadatachannels[registeredDMAChannels].sensors[sensorIndex].registered = true;
                    metadatachannels[registeredDMAChannels].mapentry = entry;
                    metadatachannels[registeredDMAChannels].groupHandle = GroupHandle;
                    /*Clear metadata and create the queue */
                    METADATA_UPDATER_clearMetadataOutgoingBuffers(&metadatachannels[registeredDMAChannels]);
                    METADATA_UPDATER_initQueueAndMutex(&metadatachannels[registeredDMAChannels]);
                    registeredDMAChannels++;
                }
                count++;
                }
                sensor = inu_node__getNextOutputNode(groupP,sensor);
        }
        return INU_METADATA__RET_SUCCESS;
    }


    return INU_METADATA__ERR_OUT_OF_MEM;
}

INT32 METADATA_UPDATER_findSavedChannelHandle(CDE_DRVG_channelHandleT metadataChannelHandle)
{
    if (registeredDMAChannels > 0)
    {
        for (int i = 0; i < registeredDMAChannels; i++)
        {
            if (metadatachannels[i].inUse == true && metadatachannels[i].mapentry != NULL)
            {
                if (metadatachannels[i].channel->metadataDMAChannelHandle == metadataChannelHandle)
                {
                    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Found matching DMA channel\n");
                    return i;
                }
            }
        }
    }
    return -1;
}
ERRG_codeE METADATA_UPDATER_updateMOVInstructions(CDE_DRVG_channelHandleT metadataChannelHandle, INU_DEFSG_moduleTypeE modelType,
                                                  UINT32 loopIndex, UINT32 phyAddress, UINT32 offset)
{
    INT32 index = METADATA_UPDATER_findSavedChannelHandle(metadataChannelHandle);
    if (index >= 0)
    {
        UINT8 *temp;
        temp = metadataChannelHandle->dstAddressProgramP[loopIndex];
        if (temp != NULL)
        {
            PL330DMAP_DMAMOV(temp, PL330DMA_REG_DAR, (phyAddress + offset)); /*This is used for metadata */
        }
        temp = metadataChannelHandle->regToMemoryParams.dstAddressProgramP_MetaData[loopIndex];
        if (temp != NULL)
        {
            PL330DMAP_DMAMOV(temp, PL330DMA_REG_DAR, (phyAddress + offset + METADATA_FIXED_LOCATION_FRAME_ID_OFFSET)); /*add offset for frame ID */
        }
        return CDE__RET_SUCCESS;
    }
    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "No configuration available for metadata for this model number: %u \n", modelType);
    return (CDE__ERR_NO_METADATA_CONFIG);
}
/**
 * @brief Checks if we need to repeat a sensor update to the slave
 * 
 *
 * @param tempMetadata Sensor update we need to check if we need to send the sensor update to the slave
 * @return Returns true if the we need to repeat the message to the slave
 */
static bool METADATA_UPDATER_checkIfRepeatNeeded(INU_Metadata_T *tempMetadata)
{
    bool repeatToSlave = false;
    bool is_master = HELSINKI_getMaster();
    INU_metadata_HardwareType targetHW = INU_metadata_HardwareType_MASTER;
    if(!is_master)
        targetHW = INU_metadata_HardwareType_SLAVE;

    for (int i = 0; i < tempMetadata->protobuf_packet.state_count; i++)
    {
        /*If the Hardware is the opposite of the current hardware then we should repat the message*/
        if(tempMetadata->protobuf_packet.state[i].target != targetHW)
        {
            repeatToSlave = true;
        }
    }
    return repeatToSlave;
}
/** @brief For synchronized commands we need to wait until the next SOF interrupt before copying the command into the sensor queue
 *  @param toplevel - Toplevel command
 * 
 *  @return Returns true if we need to reserialize the command
*/
static bool METADATA_UPDATER_preProcessSyncedCommand(INU_Metadata_T &toplevel)
{
    /*Currently we are only supporting synced ISP expo commands*/
    std::array<INU_Metadata_T,NUMBER_SLU_QUEUES> delayed_cmd;
    for (auto &val : delayed_cmd)
        val = ZEROED_OUT_METADATA;
    std::array<bool, NUMBER_SLU_QUEUES> delayed_cmd_modified =  {false,false,false,false,false,false};                      /*Flag to say if we should copy the command to a delayed queue*/
    bool command_modified = false;
    int32_t slu_index = 0;
    for (int i = 0; i < toplevel.protobuf_packet.state_count; i++)
    {
        INU_metadata_State &state = toplevel.protobuf_packet.state[i];

        // /*Clear each of the command lists */
        for(auto &temp_state : delayed_cmd)
            temp_state.protobuf_packet.state[i].sensor_metadata = INU_metadata_Sensor_MetaData_init_zero; /*Init the delayed command as zero*/

        const uint32_t slu = state.sensor_metadata.isp_expo.uart_sync.slu_number;
        /*Check if we  have a delayed ISP expo command*/
        if (state.has_sensor_metadata 
                && state.sensor_metadata.has_isp_expo 
                && state.sensor_metadata.isp_expo.has_uart_sync 
                && state.sensor_metadata.isp_expo.uart_sync.wait_for_slu_SOF_Interrupt)
        {
            if(delayed_cmd_modified.size() < slu)
            {
                LOGG_PRINT(LOG_ERROR_E,NULL,"Invalid SLU number %lu \n",state.sensor_metadata.isp_expo.uart_sync.slu_number);
                continue;
            }
            if(!delayed_cmd_modified.at(slu))
            {
                /*An optimization is here (saves 5us/40us), 
                we should only copy the toplevel command if necessary */
                delayed_cmd.at(slu) = toplevel;
            }
            INU_metadata_State &delayed_state = delayed_cmd[slu].protobuf_packet.state[i];
            delayed_cmd_modified.at(slu) = true;
            delayed_state.has_sensor_metadata = true;
            delayed_state.sensor_metadata.has_isp_expo = true;
            delayed_state.sensor_metadata.isp_expo = state.sensor_metadata.isp_expo;
            /*Remove has_uart sync property*/
            delayed_state.sensor_metadata.isp_expo.has_uart_sync =  false;
            /*Make sure that the  isp expo command is ignored in the original command*/
            state.sensor_metadata.has_isp_expo = false;
            /*We will need to reserialize the original command*/
            command_modified = true;
            LOGG_PRINT(LOG_DEBUG_E,NULL,"Synced command found for SLU %lu with %lu states \n", slu, toplevel.protobuf_packet.state_count);
        }
    }
    for(int i =0; i < NUMBER_SLU_QUEUES; i ++)
    {
        if((i < (int)delayed_cmd.size()))
        {
            if(delayed_cmd_modified.at(i))
            {
                LOGG_PRINT(LOG_DEBUG_E,NULL,"Pushed to delayed command queue for SLU %lu with has_expo as %lu \n",i, delayed_cmd.at(i).protobuf_packet.state[1].sensor_metadata.has_isp_expo);
                /*Use std-move as it results in a zerocopy push which is useful as these structs are quite large (hundreds of bytes)*/
                syncedCmdQueue[i].delayedCommandQueue->push(std::move(delayed_cmd.at(i)));
            }
        }
        else
        {
            LOGG_PRINT(LOG_ERROR_E,NULL,"Invalid SLU number index %lu \n",i);
        }
    }
    
    return command_modified;
}
ERRG_codeE METADATA_UPDATER_process_cmd(INU_Metadata_T *tempMetadata)
{
    const INU_metadata_HardwareType hwType = HELSINKI_getHardwareType();
    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Decoded sensor update request, now looking in our map for matching sensors in %lu registered channels,timestamp =%x \n", registeredDMAChannels,tempMetadata->timestamp);
    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "State Count=%lu \n", tempMetadata->protobuf_packet.state_count);

    for (int i = 0; i < tempMetadata->protobuf_packet.state_count; i++)
    {
        for (int j = 0; j < registeredDMAChannels; j++)
        {
            for (int sensorNum = 0; sensorNum < MAX_NUM_REGISTERED_SENSORS; sensorNum++)
            {
                /*Check hardware type matches*/
                if(tempMetadata->protobuf_packet.state[i].target == hwType)
                {
                    /*Search for a matching sensor in our metadata channels, sensorType and sensor Side must match*/
                    if (metadatachannels[j].mapentry->sensors[sensorNum].name.sensorType == tempMetadata->protobuf_packet.state[i].sensor.sensorType)
                    {
                        if (metadatachannels[j].mapentry->sensors[sensorNum].name.side == tempMetadata->protobuf_packet.state[i].sensor.side
                        && metadatachannels[j].mapentry->sensors[sensorNum].name.channelScale ==  tempMetadata->protobuf_packet.state[i].sensor.channelScale)
                        {
                            if(metadatachannels[j].mapentry->sensors[sensorNum].valid == true)
                            {
                                if(metadatachannels[j].sensors[sensorNum].sensorInfo!=NULL)
                                {
                                    UINT64 before,after = 0;
                                    OS_LYRG_getTimeNsec(&before);
                                    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Found matching sensor for index:%lu at sensor %lu with sensorID: %lu \n", j, sensorNum,metadatachannels[j].sensors[sensorNum].sensorInfo->sensorCfg.sensorId);
                                    UINT64 numMessages = OS_LYRG_getCurrNumMsg(&metadatachannels[j].sensors[sensorNum].sensorQueue);
                                    if(numMessages < SOF_INTERRUPT_QUEUE_SIZE-1)
                                    {
                                        /*Store the current Frame ID and timestamp for analysis by the host*/
                                        METADATA_SENSOR_UPATER_updateTimingInformation(&metadatachannels[j],&tempMetadata->protobuf_packet.state[i].snsr_update_timing.sensor_update_rx_timing,&tempMetadata->protobuf_packet.state[i].snsr_update_timing.has_sensor_update_rx_timing);
                                        OS_LYRG_sendMsg(&metadatachannels[j].sensors[sensorNum].sensorQueue, (UINT8 *)&tempMetadata->protobuf_packet.state[i], sizeof(INU_metadata_State));

                                        // Set the pending sensor update Mask for the sensor update to be woken up
                                        METADATA_UPDATER_setPendingBufferUpdatesForAllBuffers(&metadatachannels[j]);
                                    }
                                    else
                                    {
                                        // Set the pending sensor update Mask for the sensor update to be woken up
                                        METADATA_UPDATER_setPendingBufferUpdatesForAllBuffers(&metadatachannels[j]);
                                        LOGG_PRINT(LOG_DEBUG_E,NULL," Dropped messages \n");
                                    }
                                    OS_LYRG_getTimeNsec(&after);
                                    UINT64 timediff = after -before;
                                    LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Sensor update send took %llu ns \n", timediff);
                                    #ifdef ENABLE_ROI_QUEUE
                                    /*This code has now been commented out since we don't need to have a seperate ROI update queue as its safe to update the ROI registers within the sensor update thread
                                    */
                                    if(tempMetadata.protobuf_packet.state[i].sensor_metadata.has_roi)
                                    {
                                        numMessages = OS_LYRG_getCurrNumMsg(&metadatachannels[j].sensors[sensorNum].ROIQueue);
                                        if(numMessages < ROI_UPDATE_QUEUE_SIZE-1)
                                        {
                                            /*ROI updates need to happen during the frame done callback*/
                                            LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Added ROI Update to channel:%lu, sensor: %lu",j,sensorNum);
                                            OS_LYRG_sendMsg(&metadatachannels[j].sensors[sensorNum].ROIQueue, (UINT8 *)&tempMetadata.protobuf_packet.state[i].sensor_metadata.roi, sizeof(INU_metadata_ROI));
                                        }
                                        else
                                        {
                                            LOGG_PRINT(LOG_ERROR_E,NULL,"Dropped ROI updates due to lack of queue space \n");
                                        }
                                    }
                                    #endif
                                }
                                else
                                {
                                    LOGG_PRINT(LOG_ERROR_E, NULL, "Null sensorInfo Pointer found for metadatachannel %lu sensorIndex:%lu\n",j,sensorNum );
                                }
                            }
                            else
                            {
                                LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Invalid entry found\n");
                            }
                        }
                        else
                        {
                            LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Side doesn't match\n");
                        }
                    }
                    else
                    {
                        LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Sensor type doesn't match %lu:%lu for index %lu\n",metadatachannels[j].mapentry->sensors[sensorNum].name.sensorType,tempMetadata->protobuf_packet.state[j].sensor.sensorType,j);
                    }
                }
                else
                {
                    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Hardware type doesn't match %lu:%lu for index %lu\n",metadatachannels[j].mapentry->sensors[sensorNum].name.sensorType,tempMetadata->protobuf_packet.state[j].sensor.sensorType,j);

                    }
                }
            }
        }
    return INU_METADATA__RET_SUCCESS;
}
ERRG_codeE METADATA_UPDATER_processIncomingSensorUpdateRequest(char *buffer, size_t size, const bool repeat,
                                                               const bool checkforSyncedCommands)
{
    static INU_Metadata_T tempMetadata;
    bool synced_command = false;
    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Received sensor update with size %lu bytes \n", size);
    ERRG_codeE ret = inu_metadata__deserialize(buffer, size, &tempMetadata);

    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Decode finished \n");
    if (ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Decode failed (errorcode:%lu) of sensor update with size %lu bytes \n", ret, size);
        return ret;
    }
    /*Strip out Synced commands and check if we need to reserialize the message after doing so*/
    const bool reserialize_needed = METADATA_UPDATER_preProcessSyncedCommand(tempMetadata);
    /*We should only repeat the command if we are receiving from USB or if we are sending a global command*/
    if ((repeat))
    {
        /*An optimization here is that we only need to repeat the message to the slave if there's a sensor update which is for the slave*/
        if ((METADATA_UPDATER_checkIfRepeatNeeded(&tempMetadata)))
        {
            LOGG_PRINT(LOG_DEBUG_E,NULL,"Repeating command to slave \n");
            /*By default we will process the original buffer*/
            char *&process_buffer = buffer;
            size_t &process_size = size;
            std::array<char, 1024> reserialization_buffer;
            /*If reserialization is needed, we will reserialize the message and then repeat that instead*/
            if (reserialize_needed)
            {
                LOGG_PRINT(LOG_DEBUG_E,NULL,"Reser Target %lu, State Count %lu, has_isp_expo[0] %lu, has_isp_expo[1] %lu, has_sensor_metadata %lu \n",
                    tempMetadata.protobuf_packet.state[0].target,tempMetadata.protobuf_packet.state_count, tempMetadata.protobuf_packet.state[0].sensor_metadata.has_isp_expo,
                        tempMetadata.protobuf_packet.state[1].sensor_metadata.has_isp_expo, tempMetadata.protobuf_packet.state[1].has_sensor_metadata);
                ret = inu_metadata__serialize(reserialization_buffer.data(), reserialization_buffer.size(), &tempMetadata);
                if (ERRG_FAILED(ret))
                {
                    LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to reserialize \n");
                    return ret;
                }
                /*Now we will repeat the reserialized buffer instead of the original buffer*/
                process_buffer = reserialization_buffer.data();
                process_size = reserialization_buffer.size();
            }
            /*An optimization has been performed here where we have removed any padding within buffer
            Please see the function inu_metadata__getMetadataSizeWithoutLinePadding for a description of how this works
            */
            const int minimalMetadataSize = inu_metadata__getMetadataSizeWithoutLinePadding(process_buffer, process_size);
            if (minimalMetadataSize > 0)
            {
                METADATA_UART_repeatSensorUpdateFromMasterToSlave(process_buffer, minimalMetadataSize);
            }
            else
            {
                LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to get metadata size retCode=%d \n", minimalMetadataSize);
            }
        }
    }
    return METADATA_UPDATER_process_cmd(&tempMetadata);
}
/**
 * @brief Checks if there's a pending buffer update for an SLU number 
 * @param slu SLU number (0,1,2,3,4,5)
 * @return Returns true if there's a pending metadat buffer update required for an SLU
 */
static bool METADATA_UPDATER_checkPendingBufferUpdateForSLU(UINT32 slu)
{   /*Check if we have a pending metadata update for any of the channels that are using this SLU*/
    bool pendingBufferUpdate = false;
    for (int i = 0; i < registeredDMAChannels; i++)
    {
        if (metadatachannels[i].mapentry->slu == (INT32) slu) 
        {
            METADATA_UPDATER_LockMutex(&metadatachannels[i]);  
            /*We have an update which needs to happen frame N+2 or greater, therefore we must keep serializing metadata until this is cleared*/
            if(METADATA_UPDATER_getFutureUpdatesPending(&metadatachannels[i]))
            {
                pendingBufferUpdate = true;
                LOGG_PRINT(LOG_DEBUG_E,NULL,"Found future metadata update for %lu \n");
            }
            if(METADATA_ISP_UPDATER_getDelayedGainQueueSize(&metadatachannels[i]) > 0)
            {
                pendingBufferUpdate = true;
                LOGG_PRINT(LOG_DEBUG_E,NULL,"Found delayed gain update for %lu \n");
            }
            for(int j =0; j< MAX_NUM_BUFFERS_LOOPS; j++ )
            {
                if(metadatachannels[i].metadataBufferPendingUpdate[j])
                {
                    pendingBufferUpdate = true;
                    LOGG_PRINT(LOG_DEBUG_E,NULL,"Found pending metadata update for %lu \n", j );
                }              
                /*For the slave ISP0 stream, we will have per frame metadata updates so that we get WB and exposure values for each frame into the metadata!*/
                if(HELSINKI_getEnableConstantMetadataUpdates(metadatachannels[i].mapentry->axiReader))
                {
                    const int isp_expo_messages = OS_LYRG_getCurrNumMsg(&metadatachannels[i].isp_expoQ);
                    const int isp_wb_messages = OS_LYRG_getCurrNumMsg(&metadatachannels[i].isp_wbQ);
                    if((isp_expo_messages > 0) || (isp_wb_messages > 0))
                        pendingBufferUpdate = true;
                }
                /*If we have a command in the delayed update queue, then we need to wakeup the thread*/
                UINT32 slu = metadatachannels[i].mapentry->slu;
                if(syncedCmdQueue[slu].delayedCommandQueue->size() > 0)
                    pendingBufferUpdate = true;
        
            }
            for(int sensor =0; sensor < MAX_NUM_REGISTERED_SENSORS; sensor++)
            {
                /*We should always wakeup the thread if we have sensor updates to perform*/
                if(OS_LYRG_getCurrNumMsg(&metadatachannels[i].sensors[sensor].sensorQueue) > 0)
                    pendingBufferUpdate = true;
            }

            METADATA_UPDATER_UnlockMutex(&metadatachannels[i]); 
        }
    }
    return pendingBufferUpdate;
}
/**
 * @brief Clears the pending buffer update for a particular buffer handle
 * @param handle Metadata handle
 * @param bufferIndex Buffer Index (0->MAX_NUM_BUFFERS_LOOPS-1)
 */
static void METADATA_UPDATER_clearPendingBufferUpdate(METADATA_UPDATER_channelHandleT *handle, UINT8 bufferIndex)
{  
    handle->metadataBufferPendingUpdate[bufferIndex] = false;
}
/**
 * @brief Sets there to be a pending metadata buffer update required for all buffers (This function will lock and unlock the mutex for a metadata handle)
 * @param handle Metadata handle
 */
void METADATA_UPDATER_setPendingBufferUpdatesForAllBuffers(METADATA_UPDATER_channelHandleT *handle)
{  
    METADATA_UPDATER_LockMutex(handle);  
    for(int i=0 ; i< MAX_NUM_BUFFERS_LOOPS; i ++)
    {
        handle->metadataBufferPendingUpdate[i] = true;
    }
    METADATA_UPDATER_UnlockMutex(handle);
}
void METADATA_UPDATER_getFullFrameIDWithOverFlowCounter(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry, UINT32 *frameID)
{
    if(mapEntry->slu != 1 && mapEntry->slu !=4)
        return;

    UINT32 frameIDOffset = 0;
    UINT32 tempFrameID = 0;
    /*We will use the overflow counter as well*/
    std::lock_guard<std::mutex> lk(frameIDStatus.mutex);
    METADATA_UPDATER_getFrameID(mapEntry,&tempFrameID);
    /*Top 32 bits are for the overflwo counter*/
    frameIDOffset = frameIDStatus.frame_id_counter[mapEntry->slu];
    /*Bottom 32 bits are from the normal frame ID register*/
    *frameID =  frameIDOffset;
}
static void METADATA_UPDATER_getEOFCounter(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *mapEntry, UINT32 *EofCounter)
{
    if(mapEntry->slu != 1 && mapEntry->slu !=4)
        return;

    UINT32 frameIDOffset = 0;
    UINT32 tempFrameID = 0;
    /*We will use the overflow counter as well*/
    std::lock_guard<std::mutex> lk(frameIDStatus.mutex);
    METADATA_UPDATER_getFrameID(mapEntry,&tempFrameID);
    /*Top 32 bits are for the overflwo counter*/
    frameIDOffset = frameIDStatus.eof_frame_id_counter[mapEntry->slu];
    /*Bottom 32 bits are from the normal frame ID register*/
    *EofCounter =  frameIDOffset;
}
static void METADATA_UPDATER_getISPFrameIDs(METADATA_SENSOR_UPDATER_interruptNotification *notification)
{
    volatile UINT32 isp0,isp1 = 0;
    #define ipe_isp0_ch0_frame_info_frame_id 0x3010100  
    #define ipe_isp1_ch0_frame_info_frame_id 0x3010120
    HW_REGSG_readSocRegVolatile((UINT32) ipe_isp0_ch0_frame_info_frame_id, (volatile UINT32 *) &isp0);
    HW_REGSG_readSocRegVolatile((UINT32) ipe_isp1_ch0_frame_info_frame_id, (volatile UINT32 *) &isp1);
    notification->ISP0FrameID  = isp0;
    notification->ISP1FrameID =  isp1;
}
/**
 * @brief This function is responsible for providing the metadata injection node with frame IDs that increment monotonically per frame and
 *        also handles expanding the frame ID from 16 bits to 32 bits.
 * 
 * @param timestamp Timestamp of the interrupt
 * @param slu SLU number
 * @param SOFContext True if called from SOF context
 * @return ERRG_codeE Returns an error code
 */
static ERRG_codeE METADATA_UPDATER_updateISPFrameIDQueue(UINT64 timestamp, UINT32 slu, bool SOFContext)
{

    /*ISP block takes a while to process so we need to have a queue of frame IDs per SLU*/
    if(slu != 1 && slu !=4)
        return INU_METADATA__ERR_INVALID_ARGS; /*Return early*/
    
    std::lock_guard<std::mutex> lk(frameIDStatus.mutex);
    UINT32 frameTimestampPhyAddr = 0;
    auto interruptNotification = METADATA_SENSOR_UPDATER_interruptNotification();
    interruptNotification.timestamp = timestamp;
    interruptNotification.slu = slu;
    interruptNotification.timestamp_userspace_send = 0;
    interruptNotification.rtcTimestamp = 0;
    interruptNotification.ISP0FrameID = 0;
    interruptNotification.ISP1FrameID = 0;
    /*Store the SLU timestamp*/
    METADATA_TARGET_CONFIG_getTimestampPhysicalAddress(&frameTimestampPhyAddr,slu);
    METADATA_UPDATER_getSLUTimestampFromAddr(frameTimestampPhyAddr,&interruptNotification.rtcTimestamp);
    /*ISP0 and ISP 1 Frame ID regs are used for debugging*/
    METADATA_UPDATER_getISPFrameIDs(&interruptNotification);
    OS_LYRG_getUsecTime(&interruptNotification.timestamp_userspace_send);
    ERRG_codeE ret  = (ERRG_codeE) 0;
    if(ERRG_FAILED(ret))
        return ret;
    /*Frame ID register isn't incrementing monotonically unfortunately so we are using SW counters*/
    if(SOFContext)
        frameIDStatus.frame_id_counter[slu]++;
    else
        frameIDStatus.eof_frame_id_counter[slu]++;
    //printf("increment %lu \n",SOFContext);
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Treating the frame ID register as  0x%x \n", frameIDStatus.frame_id_counter[slu]);
    
    if(SOFContext)
    {
        interruptNotification.frameID = frameIDStatus.frame_id_counter[slu];
        OS_LYRG_sendMsg(&frameIDStatus.slu_queues_frame_id[slu], (UINT8 *)&interruptNotification, sizeof(METADATA_SENSOR_UPDATER_interruptNotification));
    }
    else
    {
        interruptNotification.frameID = frameIDStatus.eof_frame_id_counter[slu];
        OS_LYRG_sendMsg(&frameIDStatus.slu_queues_frame_id_eof[slu], (UINT8 *)&interruptNotification, sizeof(METADATA_SENSOR_UPDATER_interruptNotification));
    }
    return ret;
}
static UINT32 METADATA_UPDATER_getNumberFrameIDMessages(UINT32 slu, bool SOFContext)
{
    if (SOFContext)
        return OS_LYRG_getCurrNumMsg(&frameIDStatus.slu_queues_frame_id[slu]);
    else 
        return OS_LYRG_getCurrNumMsg(&frameIDStatus.slu_queues_frame_id_eof[slu]);
}
static INT32 METADATA_UPDATER_deQueueFrameID(UINT32 slu, METADATA_SENSOR_UPDATER_interruptNotification *notification ,INT32 dqAmount, bool SOFContext, INT32 timeout)
{
    int loopCounter = 0;
    if ((slu != 1) && (slu != 4))
        return 0;
    METADATA_SENSOR_UPDATER_interruptNotification interruptNotification;
    unsigned int size = sizeof(METADATA_SENSOR_UPDATER_interruptNotification);
    INT32 numEntries = 0;
    OS_LYRG_msgQueT *msgQueP = &frameIDStatus.slu_queues_frame_id[slu];
    if (SOFContext)
        msgQueP = &frameIDStatus.slu_queues_frame_id[slu];
    else
        msgQueP = &frameIDStatus.slu_queues_frame_id_eof[slu];
    int status = SUCCESS_E;
    while ((loopCounter < dqAmount)  && (status == SUCCESS_E) && (size == sizeof(METADATA_SENSOR_UPDATER_interruptNotification)))
    {
        //printf("DQ1 %lu \n",OS_LYRG_getCurrNumMsg(msgQueP));
        status = OS_LYRG_recvMsg(msgQueP, (UINT8 *)&interruptNotification, &size, timeout);
        //printf("DQ2 %lu \n",OS_LYRG_getCurrNumMsg(msgQueP));
        if((status == SUCCESS_E) && (size == sizeof(METADATA_SENSOR_UPDATER_interruptNotification)))
        {
          notification[numEntries] = interruptNotification;
          numEntries++;
        }
        /*Break to avoid causing this thread to block if a timeout has been specified*/
        if(OS_LYRG_getCurrNumMsg(msgQueP) == 0)
            break;
        //printf("2 \n");
        loopCounter++;
       // printf("3 \n");
        //printf("DQ3 %lu \n",OS_LYRG_getCurrNumMsg(msgQueP));
    }
    LOGG_PRINT(LOG_DEBUG_E, NULL, "Frame ID dequeued to be %lu \n", notification->frameID);
    return numEntries;
}
static ERRG_codeE METADATA_UPDATER_deQueueEOFFrameID(UINT32 slu, UINT32 *frameID,UINT64 *timestamp, INT32 dqAmount)
{
    METADATA_SENSOR_UPDATER_interruptNotification interruptNotification;
    unsigned int size = sizeof(interruptNotification);
    int status  = 0;
    if((slu==1) || (slu == 4))
        status = OS_LYRG_recvMsg(&frameIDStatus.slu_queues_frame_id_eof[slu], (UINT8 *)&interruptNotification, &size, 0); /*Task 2*/
    if((status == SUCCESS_E) && size == sizeof(METADATA_SENSOR_UPDATER_interruptNotification))
    {

        *frameID = interruptNotification.frameID;
        *timestamp = interruptNotification.rtcTimestamp;
        while(dqAmount && (status == SUCCESS_E) && size == sizeof(METADATA_SENSOR_UPDATER_interruptNotification))
        {
            status = OS_LYRG_recvMsg(&frameIDStatus.slu_queues_frame_id_eof[slu], (UINT8 *)&interruptNotification, &size, 0); /*Task 2*/
            if((status == SUCCESS_E) && size == sizeof(METADATA_SENSOR_UPDATER_interruptNotification))
            {
                *frameID = interruptNotification.frameID;
                *timestamp = interruptNotification.rtcTimestamp;
            }
            dqAmount--;
        }
    
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Frame ID dequeued to be %lu \n", *frameID);
        return INU_METADATA__RET_SUCCESS;
    }
    else
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Frame ID dequeued to be %lu \n", *frameID);
        return INU_METADATA__ERR_ILLEGAL_STATE;
    }
}  

void METADATA_UPDATER_eofInterruptCb(UINT64 timestamp, UINT32 slu, void *argP)
{
    METADATA_UPDATER_updateISPFrameIDQueue(timestamp,slu,false);
}
/* @brief Callback for a SOF interrupt [THIS SHOULD Only be registered once]
    @param Timestamp timestamp of the interrupt
    @param SLU number
    @param argP CDE_MNGRG_channnelInfoT
*/
void METADATA_UPDATER_sofInterruptCb(UINT64 timestamp, UINT32 slu, void *argP)
{
    #ifdef CDE_DRVG_ENABLE_SENSOR_SYNC_AND_UPDATE
    METADATA_UPDATER_updateISPFrameIDQueue(timestamp,slu,true);
    if(METADATA_UPDATER_checkPendingBufferUpdateForSLU(slu) )
    {

        // We have sensor updates to be processed.
        METADATA_SENSOR_UPDATER_interruptNotification interruptNotification;
        memset(&interruptNotification,0,sizeof(interruptNotification)); /*Removes a valgrind warning:  Conditional jump or move depends on uninitialised value(s)*/
        interruptNotification.timestamp = timestamp;
        interruptNotification.slu = slu;
        interruptNotification.timestamp_userspace_send = 0;
        OS_LYRG_getUsecTime(&interruptNotification.timestamp_userspace_send);
        if(&sofInterruptQueue!=NULL && &interruptNotification!=NULL )
        {
            UINT32 numMessages = OS_LYRG_getCurrNumMsg(&sofInterruptQueue);
            if(numMessages < SNSR_UPDATE_QUEUE_SIZE) /*Check that there's space in the queue*/
            {
                OS_LYRG_sendMsg(&sofInterruptQueue, (UINT8 *)&interruptNotification, sizeof(METADATA_SENSOR_UPDATER_interruptNotification));
            }
            else
            {
                /*No space to write message*/
                LOGG_PRINT(LOG_ERROR_E,NULL,"Cannot write SOF interrupt \n");
            }
        }
        else
        {
            LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Nullptr detected \n");
        }
    }

    #endif
}
ERRG_codeE METADATA_UPDATER_init()
{
#ifdef CDE_DRVG_ENABLE_SENSOR_SYNC_AND_UPDATE
    return METADATA_UPDATER_createThread();
#endif
}
#ifdef ENABLE_ROI_QUEUE
ERRG_codeE METADATA_UPDATER_updateCroppingWindow(unsigned int axiReader)
{
    ERRG_codeE ret = INU_METADATA__ERR_METADATA_NO_EXISTING_ENTRY;
    /*Task 1. Search by the AXI reader number to find a metadata channel that has the same AXI reader as the function argument named axiReader
      Task 2. Read out of the ROI queue
      Task 33. Update the PPU registers to apply the cropping window update */
    for (int j = 0; j < registeredDMAChannels; j++)
    {
        if (metadatachannels[j].mapentry->axiReader == axiReader) /* Task 1*/
        {
            for (int i = 0; i < MAX_NUM_REGISTERED_SENSORS; i++)
            {
                if (metadatachannels[j].sensors[i].registered)
                {
                    INU_metadata_ROI roi;
                    unsigned int numMessages = OS_LYRG_getCurrNumMsg(&metadatachannels[j].sensors[i].ROIQueue);
                    if (numMessages > 0)
                    {
                        /*By locking this mutex we are now safe to access metadata for all four buffers
                        This is because the Protobuf structures are only accessed by software so there's no HW/SW concurrency issues
                        and just SW<->SW concurrency issues
                        */
                        METADATA_UPDATER_LockMutex(&metadatachannels[j]);  
                        unsigned int size = sizeof(INU_metadata_ROI);
                        int status = OS_LYRG_recvMsg(&metadatachannels[j].sensors[i].ROIQueue, (UINT8 *)&roi, &size, 0); /*Task 2*/
                        while ((status == SUCCESS_E) && size == sizeof(INU_metadata_ROI))
                        {

                            LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Applying cropping update for reader:%lu, %lu,%lu \n", axiReader, roi.x_start, roi.y_start);
                            const METADATA_TARGET_CONFIG_ppu *ppuPtr = &metadatachannels[j].mapentry->sensors[i].ppu;
                            if (ppuPtr->valid)
                            {
                                PPE_MNGRG_cropParamT cropParam = {
                                    .xStart = roi.x_start,
                                    .yStart = roi.y_start,
                                    .width = 0,
                                    .height = 0, /*Height and width aren't used within PPE_MNGRG_updatePPUCrop as this function only allows you to change the x and y cropping start coordinates*/
                                };
                                PPE_MNGRG_updatePPUCrop(&cropParam, ppuPtr->index);
                                PPE_MNGRG_setRegistersReady();
                                /*Now update the metadata to show that ROI update has taken place*/
                                METADATA_SENSOR_UPDATER_updateROI(&metadatachannels[j], i, METADATA_SENSOR_UPDATER_N_1, roi.x_start, roi.y_start);
                                LOGG_PRINT(LOG_DEBUG_E, NULL, "Applied PPU cropping update for reader :%lu, %lu,%lu, PPU %lu, Sensor %lu \n", axiReader, roi.x_start, roi.y_start, ppuPtr->index, i,j);
                                ret = INU_METADATA__RET_SUCCESS;
                            }
                            else
                            {
                                LOGG_PRINT(LOG_ERROR_E, NULL, "Cannot apply Cropping update to this channel with reader ID %lu \n", axiReader);
                            }
                            size = sizeof(INU_metadata_ROI);
                            status = OS_LYRG_recvMsg(&metadatachannels[j].sensors[i].ROIQueue, (UINT8 *)&roi, &size, 0);
                        }
                        METADATA_UPDATER_UnlockMutex(&metadatachannels[j]);  
                    }
                }
            }
        }
    }
}
#endif
ERRG_codeE METADATA_UPDATER_findEntryByWriterID(const unsigned int writerID,METADATA_UPDATER_channelHandleT **channelHandle )
{
   ERRG_codeE ret = INU_METADATA__ERR_METADATA_NO_EXISTING_ENTRY;    /*No matching writer ID found*/
    for (int i = 0; i < registeredDMAChannels; i++)
    {
        if( metadatachannels[i].inUse == true)
        {
            if(metadatachannels[i].mapentry->writerID == ((int)writerID)) 
            {
                LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Found matching writer ID for channel %d \n", i);
                *channelHandle = &metadatachannels[i];
                return INU_METADATA__RET_SUCCESS;
            }
        }
    }
    return ret;
}
/*@brief This function will update metadata for the CURRENT metadata buffers and should only be called from the writer operate function
  @param writerID - ID of the writer to update metadata for
  @param timestamp - Timestamp to use in the metadata updates
  @return Returns an error code
*/
ERRG_codeE METADATA_UPDATER_updateMetadataFromWriterOperate(const unsigned int writerID,const UINT64 timestamp)
{
    ERRG_codeE ret = INU_METADATA__ERR_INVALID_ARGS;    /*No matching writer ID found*/
    METADATA_UPDATER_channelHandleT *channelHandle = NULL;
    /*Find entry by Writer ID*/
    ret = METADATA_UPDATER_findEntryByWriterID(writerID,&channelHandle);
    if(ERRG_SUCCEEDED(ret))
    {
        const bool updateCurrentBuffer = true;
        const bool overwriteSensorMetadata = true;
        /*We have to update the metadata for the current metadata buffer because of the following reasons:
            1. It's unpredictable which reader the next frame will originate from
            2. The reader isn't active until it has data which will always be predictably sent after this function is called
        */
        ret = METADATA_UPDATER_updateMetadata(channelHandle,timestamp,channelHandle->mapentry,updateCurrentBuffer,overwriteSensorMetadata,false,0);
    }
    else
    {
        LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"No match found Metadata update for writer ID %lu \n", writerID);
    }
    return ret;
}
INT32 METADATA_UPDATER_findMostLikelyFrameIndex(UINT64* sofTimestamps, UINT64 eofTimestamp,INT32 numberSOFTimestamps)
{
    int mostLikelyIndex = 0;
    for(int i =0; i < numberSOFTimestamps; i ++)
    {
        UINT64 ts = sofTimestamps[i];
        if(eofTimestamp > ts)
            mostLikelyIndex = i;
    }
    return mostLikelyIndex;
}

bool METADATA_UPDATER_checkIfFrameDropAccountedFor(METADATA_UPDATER_channelHandleT * channel, UINT32 frameID)
{
    bool accountedFor = false;
    for(int i =0; i < DROPPED_FRAME_RECORD_SIZE; i ++)
    {
        if(channel->droppedFrameRecord[i] == frameID)
        {
            accountedFor = true;
        }
    }
    return accountedFor;
}
void METADATA_UPDATER_recordDroppedFrame(METADATA_UPDATER_channelHandleT * channel, UINT32 frameID)
{
    channel->droppedFrameRecord[channel->droppedFrameRecordCounter] = frameID;
    channel->droppedFrameRecordCounter = (channel->droppedFrameRecordCounter + 1) % DROPPED_FRAME_RECORD_SIZE;
}


static ERRG_codeE  METADATA_UPODATER_correctForISPFrameDrops(METADATA_UPDATER_channelHandleT * channel, UINT32 *frameID, UINT64 *timestamp, UINT32 *eofFrameID, UINT64 *eofTimestamp, UINT32 *isp0FrameID,
    UINT32 *isp1FrameID, UINT32 *droppedFrameCounter, UINT32 *compenstatedFrameCounter, UINT32 *excessDequeues)
{
    #define MAX_DEQUEUES 24
    METADATA_UPDATER_LockMutex(channel);
    UINT64 currentTime =0;
    OS_LYRG_getUsecTime(&currentTime);
    METADATA_SENSOR_UPDATER_interruptNotification sofNotification[MAX_DEQUEUES];
    METADATA_SENSOR_UPDATER_interruptNotification eofNotification[MAX_DEQUEUES];
    memset(sofNotification,0,sizeof(sofNotification));
    memset(eofNotification,0,sizeof(eofNotification));
    UINT32 dQAmount_SOF = 1,dQAmount_EOF  = 1;
    UINT64 rtcTimestamp  =0;
    UINT64 rtcTimestamp_ns = 0;
    INT64 timeDiff_us=0;
 /*Workout how many pending SOF/EOF notifications we have*/
    UINT32 numMessages_sof = METADATA_UPDATER_getNumberFrameIDMessages(channel->mapentry->slu, true);
    UINT32 numMessages_eof = METADATA_UPDATER_getNumberFrameIDMessages(channel->mapentry->slu, false);
    /*At the start we will have dropped Frames for the downscaled stream, we need to remove the excessive frame IDs
    Through experimentation, we should only have two frame IDs in the queue, this if statement will slowly recover from having more than this*/
    if(numMessages_sof > NORMAL_RUNNING_ISP_QUEUE_LIMIT)
    {
        /*Dequeue all messages > 2*/
        /*For example if there were three messages, it would dequeue 2 messages*/
        UINT32 excessElements = numMessages_sof - NORMAL_RUNNING_ISP_QUEUE_LIMIT;
        dQAmount_SOF = excessElements + 1;
        channel->excessDequeues += excessElements;
    }
    INT32 queue_difference = numMessages_sof -numMessages_eof;
    bool use_queue_frame_id = true;
    /*We should try and prevent the Frame IDs building up by using some smart logic for detecting high SOF->ISP EOF latency*/
    /*Dequeue the oldest SOF and EOF frame IDs and use them to workout which metadata buffer to use*/
    METADATA_UPDATER_UnlockMutex(channel);
    if(dQAmount_SOF > MAX_DEQUEUES)
        /*Limit how much dequeue happens - Address sanitizer caught an issue*/
        dQAmount_SOF = MAX_DEQUEUES;
    INT32 dq_entries_sof = METADATA_UPDATER_deQueueFrameID(channel->mapentry->slu,&sofNotification[0],dQAmount_SOF,true,SOF_INTERRUPT_TIMEOUT_MS); /* We need to block to make sure that we always receive a frame ID*/
    METADATA_UPDATER_LockMutex(channel);
    if(dq_entries_sof  == 0)
    {
        *frameID = channel->swInjectionPreviousFrameID + 1 ; /*Assume that the frame ID is the + 1 of the previous Frame ID*/
        *timestamp =  0;
        *eofFrameID = 0;
        *eofTimestamp = 0;
        *isp0FrameID =     channel->previousISPFrameIDs[0]+ 1;
        *isp1FrameID =     channel->previousISPFrameIDs[1] + 1;
        *droppedFrameCounter = channel->detectedDroppedFrameCounter;
        *compenstatedFrameCounter = channel->compensatedFrameCounter;
        *excessDequeues = channel->excessDequeues;
        channel->previousISPFrameIDs[0]++;
        channel->previousISPFrameIDs[1]++;
        channel->swInjectionPreviousFrameID++;
        //printf("NoF %lu\n",*frameID);
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Couldn't read a frame ID \n");
        METADATA_UPDATER_UnlockMutex(channel);
        return INU_METADATA__ERR_ILLEGAL_STATE;
    }
    METADATA_UPDATER_UnlockMutex(channel);
    INT32 dq_entries_eof = METADATA_UPDATER_deQueueFrameID(channel->mapentry->slu,&eofNotification[0],dQAmount_EOF,false,0);
    METADATA_UPDATER_LockMutex(channel);
    UINT32 dq_entry_index = 0, dq_entry_eof_index = 0;
 
    if(dq_entries_sof > 1)
        /*Use the last index*/
        dq_entry_index = dq_entries_sof-1;
    if(dq_entries_eof > 1)
        /*Use the last index*/
        dq_entry_eof_index = dq_entries_sof - 1;
    UINT64 SOFTimestamp_us = sofNotification[dq_entry_index].timestamp_userspace_send;
    UINT64 EOFTimestamp_us = eofNotification[dq_entry_index].timestamp_userspace_send;
    RTC_DRVG_getTimeRaw(&rtcTimestamp);
    rtcTimestamp_ns = RTC_convertRTCTimeToNS(rtcTimestamp);
    /*Calculate the time difference between the current time and the SOF RTC timestamp*/
    timeDiff_us = (INT64)currentTime- (INT64)SOFTimestamp_us;
    /*We should try and prevent the Frame IDs building up by using some smart logic for detecting high SOF->ISP EOF latency and then dequeing frame IDs until we find a healthy looking latency
    */
    /*Another method that I have implemented is by looking to see if the frame ID for the downscaled and the cropped are the same, if they are then we likely have a dropped frame*/
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Reader ID %lu \n", channel->mapentry->axiReader);
    if((channel->mapentry->axiReader == ISP1_READER))
    {
    
        INT32 frameIDDifference = (INT32)sofNotification[dq_entry_index].ISP1FrameID - (INT32)sofNotification[dq_entry_index].ISP0FrameID;
        numMessages_sof = METADATA_UPDATER_getNumberFrameIDMessages(channel->mapentry->slu, true);
        if (frameIDDifference != -1 && HELSINKI_getMaximumISPLatency(timeDiff_us))
        /*We have detected a dropped frame, try to dequeue the next frame ID if available*/
        {
            UINT32 droppedFrameID = sofNotification[dq_entry_index].frameID;     /*Calculate which frame ID was dropped*/
            //printf("A-%d %d %d %d \n",droppedFrameID,numMessages_sof, sofNotification[dq_entry_index].frameID, frameIDDifference);
            channel->detectedDroppedFrameCounter++;
            if(!METADATA_UPDATER_checkIfFrameDropAccountedFor(channel,droppedFrameID))
            {
                LOGG_PRINT(LOG_DEBUG_E,NULL,"Original Time difference %lld \n", timeDiff_us);
                METADATA_UPDATER_UnlockMutex(channel);
                dq_entries_sof = METADATA_UPDATER_deQueueFrameID(channel->mapentry->slu,&sofNotification[0],1,true,20); /*Make sure we dequeue this entry*/
                METADATA_UPDATER_LockMutex(channel);
                numMessages_sof = OS_LYRG_getCurrNumMsg(&frameIDStatus.slu_queues_frame_id[channel->mapentry->slu]);
                /* We have detected a dropped VST cropped frame*/
                if(dq_entries_sof  >= 1)
                {
                    use_queue_frame_id = false;
                    *frameID = droppedFrameID + 1;
                    *timestamp =  sofNotification[dq_entry_index].rtcTimestamp;
                    channel->previousISPFrameIDs[1] = *frameID;
                    //printf("B%d %d %d \n",frameIDDifference,numMessages_sof, sofNotification[dq_entry_index].frameID);
                    channel->compensatedFrameCounter++;  
                    METADATA_UPDATER_recordDroppedFrame(channel,droppedFrameID);
                }
                else
                {
                    printf("Dropped frame detected but we don't have any frames to dequeue \n");
                }


                /* If we have broken early then we couldn't compensate for this dropped frame*/
                SOFTimestamp_us = RTC_convertRTCTimeToNS(sofNotification[dq_entry_index].rtcTimestamp);    
                timeDiff_us = (INT64)currentTime- (INT64)SOFTimestamp_us;
                LOGG_PRINT(LOG_DEBUG_E,NULL,"New Time difference %lld \n", timeDiff_us);
                frameIDDifference = (INT32)sofNotification[dq_entry_index].ISP1FrameID - (INT32)channel->previousISPFrameIDs[1];
            }
            else
            {
                printf("Frame drop has already been taken account of \n");
            }
        }
    }
    if(use_queue_frame_id)
    {
        *frameID = sofNotification[dq_entry_index].frameID;
        *timestamp =  sofNotification[dq_entry_index].rtcTimestamp;
        channel->previousISPFrameIDs[1] = sofNotification[dq_entry_index].ISP1FrameID;
    }
    *eofFrameID = eofNotification[dq_entry_eof_index].frameID;
    *eofTimestamp = eofNotification[dq_entry_eof_index].rtcTimestamp;
    *isp0FrameID = sofNotification[dq_entry_index].ISP0FrameID;
    *isp1FrameID = sofNotification[dq_entry_index].ISP1FrameID;
    *droppedFrameCounter = channel->detectedDroppedFrameCounter;
    *compenstatedFrameCounter = channel->compensatedFrameCounter;
    *excessDequeues = channel->excessDequeues;
    channel->previousISPFrameIDs[0] = sofNotification[dq_entry_index].ISP0FrameID;
    channel->swInjectionPreviousFrameID = *frameID;
    METADATA_UPDATER_UnlockMutex(channel);
    return INU_METADATA__RET_SUCCESS;
}
ERRG_codeE METADATA_UPDATER_injectMetadata(MEM_POOLG_bufDescT *bufDescP,UINT32 readerID, inu_image__hdr_t* hdr)
{
    /*Here's how we are different for channels that use the PL330 DMA controller for metadata injection
        1. We need to copy the metadata manually in software
        2. We aren't preparing metadata for the future, we are in-fact just copying into a buffer that has just been read
        3. We have to do this copying for every image buffer received

        It's intended that this function will be called from a software node by the graph infrastructure. 

        NOTE:
        There's a hardware bug with the SLU Frame ID register, this means we have had to make our own frame ID values up within software
        We are also dealing with an ISP software issue where we have significant jitter in the timing of the frame done callbacks. 

        I have worked around this by doing the following:
        1. This function will update the "previous Frame ID " variable using the image counter within the image header (labelled as X)
        2. The sensor update thread which runs after the SOF ISR will use the previous frame ID to calculate which buffers need updating (Buffers X + 2 to X + 5 are updated)
        3. The sensor update thread will increment the previous frame ID so that if the order is like below, the software will still handle this situation:

            SOF ISR (frame 0)
            SOF ISR (frame 1)
            EOF callback (frame 0)
            EOF callback (frame 1)
        The ideal order is as below
            SOF ISR (frame 0)
            EOF callback (frame 0)
            SOF ISR (frame 1)
            EOF callback (frame 1)
    */
    METADATA_UPDATER_channelHandleT * channel = NULL;
    ERRG_codeE ret =  METADATA_getChannelHandleByReaderID(&channel,readerID);
    /*Convert to ns*/
    UINT64 kernel_time_us = 0;
    OS_LYRG_getUsecTime(&kernel_time_us);
    if(ERRG_FAILED(ret))
        return ret;
    if(!channel)
        return ret;
    /*Copy the metadata into the first line*/
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Serializing metadata \n");
    if(ERRG_SUCCEEDED(ret))
    {
        UINT32 endOfFrameID = 0;
        UINT32 frameID = 0;
        UINT64 ts = 0;
        UINT64 endOfFrameTS = 0;
        UINT32 ispFrameID0 = 0, ispFrameID1 = 0;
        UINT32 droppedFrameCounter = 0, compensatedFrameCounter = 0, excessDequeues = 0;
        /*We have to have implemented a solution for detecting ISP frame drops*/
        METADATA_UPODATER_correctForISPFrameDrops(channel,&frameID,&ts,&endOfFrameID, &endOfFrameTS,&ispFrameID0,&ispFrameID1,&droppedFrameCounter,&compensatedFrameCounter, &excessDequeues);
        /*0 = N+1 frame, 1 = N+2 frame, 2 = N+3 frame, 3 = N+4 frame, 4 = N+5 frame*/
        const UINT32 index = 0;
        const UINT32 metadata_buffer_index = METADATA_UPDATER_calculateMetadataBuffer(channel,false);
        INU_Metadata_T * metadata = &channel->outgoingSensorUpdates[metadata_buffer_index];
        /*Get the size of the metadata buffer*/
        UINT32 metadata_size = MEM_POOLG_getBufSize(channel->handle); 
        /*We need to lock this mutex to make sure that the sensor update thread isn't touching the metadata buffers
            whilst we are injecting the metadata into the first line
        */
        METADATA_UPDATER_LockMutex(channel);
        METADATA_FIFO_Entry* fifoEntry  = METADATA_UPDATER_GetBufferPointerUsingFrameID(channel,frameID,METADATA_SearchUsingSOFFrameID);
        MEM_POOLG_bufDescT   *bufferPtr = fifoEntry->metadata_buffDescListP;
        //printf("SOF frame ID %lu, EOF frame ID %lu \n",fifoEntry->startFrameID,fifoEntry->startEOFFrameID);
        if(ERRG_FAILED(ret))
            return ret;
        #if 1
         /*Modify the frame ID so that frame N for downscaled matches frame N for cropped*/
        if(0)
        {
            if(frameID > 0 )
                frameID = frameID - 1;
        }
        #endif
        metadata->frameID = frameID;
        metadata->timestamp = ts;
        metadata->serialization_counter = channel->metadataUpdates_Counter;
        metadata->framebuffer_offset = 0; /*TODO: Fix this*/
        metadata->eofFrameID = endOfFrameID;
        metadata->software_injection_call_counter = channel->softwareInjectCallCount;
        metadata->isp0FrameID = ispFrameID0;
        metadata->isp1FrameID = ispFrameID1;
        metadata->droppedFrameCounter = droppedFrameCounter;
        metadata->compensatedDroppedFrameCounter = compensatedFrameCounter;
        metadata->excessDequeues = excessDequeues;
        // if(channel->mapentry->axiReader == ISP0_READER)
        //     printf("I%.3f \n",metadata->protobuf_packet.state[0].sensor_metadata.isp_expo.integrationTime);
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Expo being used %.3f \n",metadata->protobuf_packet.state[0].sensor_metadata.isp_expo.integrationTime);
        /*Store the RTC timestamp so that we can analyze when this software node ran */
        RTC_DRVG_getTimeRaw(&metadata->software_injection_timestamp_rtc);
        /*Copy all the metadata first (Fixed location metadata + Protobuf metadata)*/
        memcpy(bufDescP->dataP,bufferPtr->dataP,metadata_size);
        /*Update the fixed location metadata (Frame ID, Timestamp, framebuffer_offset etc)
         This function will also perform a cache write back for the whole buffer! It needs this to be done!*/
        METADATA_target_serializeFixedLocationDataOnly(bufDescP,metadata,metadata_size);         
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Frame ID %lu, TS %llu, Index %lu \n", 
            metadata->frameID,metadata->timestamp, index);
        //printf("%lu,%lu \n", metadata->frameID, metadata->protobuf_packet.state[0].snsr_update_timing.sensor_update_i2cWritesComplete.frameID.frameID);
        /*Update the previous frame ID*/
        channel->previousFrameID  = frameID;
        channel->softwareInjectCallCount++;
        /*Now the sensor update thread can update this buffer since the readout is done*/
        METADATA_UPDATER_markReadoutDone(channel,index);
        // Shift metadata buffers like the following:
        // N+3 is now buffer N+2
        // N+2 is now buffer N+1
        METADATA_UPDATER_UnlockMutex(channel);
    }
    return INU_METADATA__RET_SUCCESS;
}


};


#endif
