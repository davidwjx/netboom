#include "cde_drv_new.h"
#include "inu_metadata.h"
#include "inu_common.h"
#include "err_defs.h"
#include "mem_map.h"
#include "metadata_target_config.h"
#include "metadata_target_serializer.h"
#include "helsinki.h"
#ifdef CDE_DRVG_METADATA
#define PPE_BASE 0x08030000
#define IAE_BASE 0x08000000

#define SENSOR_ID_VST_0 4
#define SENSOR_ID_VST_1 5

ERRG_codeE METADATA_TARGET_CONFIG_getConfigParamsByReaderID(CDE_DRVG_regToMemoryCopyConfigParams *configParams, const INU_DEFSG_moduleTypeE bootID,
    const UINT8 readerID, CDE_MNGRG_channnelInfoT*  reader)
{
    const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entry = NULL;
    ERRG_codeE ret = METADATA_TARGET_CONFIG_getChannelMapEntryByReaderID(bootID,readerID,&entry);
    //TODO Deal with multiple dma channels
   if(ERRG_SUCCEEDED(ret))                                     
    {
        if(entry->enabled && entry->metadataLines > 0) /* Set enable = False or Metadata lines = 0 will disable metadata*/
        {
            /*This configuration enables copying of the frame ID and the timestamp automatically in sync with frame buffer copies*/
            UINT8 INDEX = 0;
            configParams->dstIncrement[INDEX] = Address_Increment;
            configParams->srcIncrement[INDEX] = Address_Increment;
            configParams->giveEvent = GIVE;
            configParams->waitforEvent = WAIT;
            configParams->waitEventID = entry->waitEventID;                             /*Wait for this give ID to happen */
            configParams->giveEventID = entry->giveEventID;                             /*Gives this event ID */
            configParams->entry = entry;
            if(entry->frameIDRegister_phy!=0)
            {
                /*Use the frame ID described in the entry if it exists */
                configParams->registerAddresses[INDEX] = entry->frameIDRegister_phy;        
            }
            else
            {
                /*Calculate the entry automatically */
                METADATA_TARGET_CONFIG_getFrameIDPhysicalAddress( &configParams->registerAddresses[INDEX],entry);
            }
            /*Frame ID is 4 bytes however we have to copy 8 bytes to reduce the size of the DMA program */
            configParams->transferSizes[INDEX] = 8;
            configParams->numLoops[INDEX] = 0;
            for (int i = 0; i < reader->dmaChannelHandle->numLoops; i++)
            {
                UINT32 phyAddress = 0;
                MEM_POOLG_getDataPhyAddr(reader->dmaChannelHandle->buffDescListP[i], &phyAddress);
                configParams->memoryAddresses[i][INDEX] = phyAddress +METADATA_FIXED_LOCATION_FRAME_ID_OFFSET; /*Physical address plus METADATA_FIXED_LOCATION_FRAME_ID_OFFSET for the frame ID */
                LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Configuring metadata for register:%lu with registerAddress:0x%X,  \
                physical address:0x%X, giveEventID: %u, waitEventID %u \n",
                    0, configParams->registerAddresses[INDEX], configParams->memoryAddresses[i][INDEX],
                    configParams->giveEventID, configParams->waitEventID);
                configParams->protobuf_metadata_dstAddress[i] = phyAddress;
            }
            /*I have performed an optimization so that the DMAC program is < 255 bytes, 
            by making the CCR  the same as the first register copy. This modification removes a single MOV per buffer  */ 
            INDEX = 1;
            configParams->dstIncrement[INDEX] = Address_Increment;
            configParams->srcIncrement[INDEX] = Address_Increment;
            configParams->giveEvent = GIVE;
            configParams->waitforEvent = WAIT;
            configParams->waitEventID = entry->waitEventID;            /*Wait for this give ID to happen */
            configParams->giveEventID = entry->giveEventID;            /*Gives this event ID */
            configParams->entry = entry;
            if(entry->timestampRegister_phy!=0)
            {
                /*Use the rimestamp described in the entry if it exists */
                configParams->registerAddresses[INDEX] = entry->timestampRegister_phy;        
            }
            else
            {
                /*Calculate the entry automatically */
                METADATA_TARGET_CONFIG_getTimestampPhysicalAddress( &configParams->registerAddresses[INDEX],entry->slu);
            }
            /*Timestamp register is 8 bytes */
            configParams->transferSizes[INDEX] = 8;                        
            configParams->numLoops[INDEX] = 0;
            for (int i = 0; i < reader->dmaChannelHandle->numLoops; i++)
            {
                configParams->memoryAddresses[i][INDEX] = 0; 
            }
            configParams->registerAddressCopiesToDo = 2;
            /*Use configured metadata lines */
            configParams->numberMetadataLines = entry->metadataLines;    
            /* The number of buffers must match the number of loops in the dmaChannelHandle */
            configParams->numBuffers = reader->dmaChannelHandle->numLoops;  
            /*Copy the number of bytes for the frame buffer offset, this is serialized and sent with the frame buffer 
                so the host knows how many bytes to offset for the frame buffer */
            configParams->frameBufferOffsetBytes = reader->dmaChannelHandle->framebuffer_offset;   
            return CDE__RET_SUCCESS;
        }
    }
    
    configParams->registerAddressCopiesToDo = 0;
    LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"No configuration available for metadata for this model number: %u \n", bootID);
    return (CDE__ERR_NO_METADATA_CONFIG); /*No configuration available */
    
}

ERRG_codeE METADATA_TARGET_CONFIG_getMemPoolConfig(MEM_POOLG_cfgT *metadata_pool_cfg, INU_DEFSG_moduleTypeE modelType,
                                                   UINT32 numBuffers)
{
    if (modelType == INU_DEFSG_BOOT65_E)
    {
        /* This function will configure the memory pool to use the same number of numbuffers 
            as the main thread wit ha size equal to the framebuffer_offset (which is derived from the number of rows * number of metadata rows) */
        metadata_pool_cfg->numBuffers = numBuffers; //+1 for drain
        metadata_pool_cfg->bufferSize = 32*20; /*480 bytes of metadata which is small enough for the GAZE ROI and large enough to hold the Gaze ROI metadata*/
        metadata_pool_cfg->freeCb = NULL;
        metadata_pool_cfg->type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
        metadata_pool_cfg->memP = NULL;
        metadata_pool_cfg->freeArg = NULL;
        metadata_pool_cfg->resetBufPtrInAlloc = 0;
        LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"%lu metadata buffers being created with size %lu \n", metadata_pool_cfg->numBuffers, metadata_pool_cfg->bufferSize);
        return CDE__RET_SUCCESS;
    }
    return (CDE__ERR_NO_METADATA_CONFIG);
}

ERRG_codeE METADATA_TARGET_CONFIG_getChannelMapEntryByReaderID(const INU_DEFSG_moduleTypeE bootID,const UINT8 readerID, const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap **entry)
{
    /* Scans the registered metadata channels for a match with the readerID being set to readerID */
    UINT32 n = 0;
    const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *map = NULL;
    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Searching for configuration for boot ID: %lu,channelID: %lu\n",bootID,readerID);
    switch(bootID)
    {
        case INU_DEFSG_BOOT65_E:
            /*Get the channel map based off if we're a master or a slave */
            HELSINKI_getChannelMap(&map,&n);
            break;
        default:
            return INU_METADATA__ERR_NOT_SUPPORTED;
    }
    /*Scan the channelID Map */
    if(map !=NULL)
    {
        LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Number map entries:%lu \n", n);
        /*Scan the map for a matching core and vChannel pair */
        for(UINT32 i = 0; i < n; i ++)
        {
            if(map[i].axiReader  == readerID)
            {
                /*We have found the map entry */
                *entry = &map[i];
                LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Found SLU mapping channel %d for SLU:%d for core,channel %d.%d at index %d for reader ID %d\n"
                    ,map[i].channelID, map[i].slu,map[i].core,map[i].vchannel,i, readerID);
                return INU_METADATA__RET_SUCCESS;
            }
        }
        return INU_METADATA__ERR_NOT_SUPPORTED;
    }
    else
    {
        LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "No map entry found \n");
        return INU_METADATA__ERR_NOT_SUPPORTED;
    }
}
int METADATA_TARGET_CONFIG_getChannelMapEntriesBySensorID(const INU_DEFSG_moduleTypeE bootID,const UINT16 SensorID,UINT32 *sensorIndex, const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap **entry, size_t listSize)
{
    /* Scans the registered metadata channels for a match with the channelID being set to channelID */
    int n = 0;
    int numberEntries = 0;
    const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *map = NULL;
    UINT16 SensorIndex = SensorID;
    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Searching for configuration for boot ID: %lu,sensor: %lu\n",bootID,SensorIndex);
    switch(bootID)
    {
        case INU_DEFSG_BOOT65_E:
            /*Get the channel map based off if we're a master or a slave */
            HELSINKI_getChannelMap(&map,&n);
            break;
        default:
            return INU_METADATA__ERR_NOT_SUPPORTED;
    }
    /*Scan the channelID Map */
    if(map !=NULL)
    {
        /*Scan the map for a matching core and vChannel pair */
        for(int i = 0; i < n; i ++)
        {
            for(int j =0; j < MAX_NUM_REGISTERED_SENSORS; j ++)
            {
                /* Sensor ID is used as the key*/
                if((map[i].sensors[j].name.sensorID.sensorID == SensorIndex) && (map[i].sensors[j].valid) )
                {
                    /*We have found the map entry */
                    entry[numberEntries] = &map[i];
                    sensorIndex[numberEntries] = j;
                    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Found SLU mapping channel %dfor SLU:%d for core,channel %d.%d at index %d for sensor %d\n"
                        ,map[i].channelID, map[i].slu,map[i].core,map[i].vchannel,i,SensorIndex);
                    if(numberEntries < (int)listSize)
                    {
                        numberEntries++;
                        
                    }
                    else
                    {
                    return numberEntries;
                    }

                }
                else
                {
                    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Match not found %lu!=%lu\n"
                        ,map[i].sensors[j].name.sensorID.sensorID,SensorIndex);
                }
            }
        }
    }
    return numberEntries;
   
}

ERRG_codeE METADATA_TARGET_CONFIG_getChannelMapEntryBySensorID(const INU_DEFSG_moduleTypeE bootID,const UINT16 SensorID,UINT32 *sensorIndex, const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap **entry)
{
    /* Scans the registered metadata channels for a match with the channelID being set to channelID */
    UINT32 n = 0;
    const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *map = NULL;
    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Searching for configuration for boot ID: %lu,sensor: %lu\n",bootID,SensorID);
    switch(bootID)
    {
        case INU_DEFSG_BOOT65_E:
            /*Get the channel map based off if we're a master or a slave */
            HELSINKI_getChannelMap(&map,&n);
            break;
        default:
            return INU_METADATA__ERR_NOT_SUPPORTED;
    }
    /*Scan the channelID Map */
    if(map !=NULL)
    {
        /*Scan the map for a matching core and vChannel pair */
        for(UINT32 i = 0; i < n; i ++)
        {
            for(UINT32 j =0; j < MAX_NUM_REGISTERED_SENSORS; j ++)
            {
                /* Sensor ID is used as the key*/
                if((map[i].sensors[j].name.sensorID.sensorID == SensorID) && (map[i].sensors[j].valid) )
                {
                    /*We have found the map entry */
                    *entry = &map[i];
                    *sensorIndex = j;
                    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Found SLU mapping channel %lu for SLU:%lu for core,channel %lu.%lu at index %lu for sensor %lu\n"
                        ,map[i].channelID, map[i].slu,map[i].core,map[i].vchannel,i,SensorID);
                    return INU_METADATA__RET_SUCCESS;
                }
                else
                {
                    LOGG_PRINT(METADATA_DEBUG_LEVEL, NULL, "Match not found %lu!=%lu\n"
                        ,map[i].sensors[j].name.sensorID.sensorID,SensorID);
                }
            }
 
        }
        return INU_METADATA__ERR_NOT_SUPPORTED;
    }
    else
    {
        LOGG_PRINT(LOG_DEBUG_E, NULL, "No map entry found \n");
        return INU_METADATA__ERR_NOT_SUPPORTED;
    }
}

/*  @brief Updates the Readers GiveEvent and WaitIDs which wakeup the second metadata thread
    @param bootID - Boot ID used for lookup using METADATA_TARGET_CONFIG_getChannelMapEntry()
    @param readerID - Reader ID 
    @param reader - Reader channel configuration which is modified to store the give and wait Event IDs in
*/
ERRG_codeE METADATA_TARGET_CONFIG_updateReaderGiveEventAndWaitIDsByReaderID(const INU_DEFSG_moduleTypeE bootID,const UINT8 readerID,
    CDE_MNGRG_channnelInfoT*  reader)
{
    const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entry = NULL;
    ERRG_codeE ret = METADATA_TARGET_CONFIG_getChannelMapEntryByReaderID(bootID,readerID,&entry);
    if(ERRG_SUCCEEDED(ret))
    {
        /*The main thread (DmaChannelHandle) will wait for giveEventID and give waitEventID
          This is the inverse of what the metadata channel handle will be doing */
        reader->dmaChannelHandle->regToMemoryParams.waitEventID     = entry->giveEventID;
        reader->dmaChannelHandle->regToMemoryParams.waitforEvent    = WAIT;
        reader->dmaChannelHandle->regToMemoryParams.giveEventID     = entry->waitEventID;
        reader->dmaChannelHandle->regToMemoryParams.giveEvent       = GIVE;
        LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"WaitEventID:%lu, GiveEventID:%lu Set for mainchannel thread bootID:%lu,readerID:%lu \n",
        reader->dmaChannelHandle->regToMemoryParams.waitEventID, reader->dmaChannelHandle->regToMemoryParams.giveEventID , 
        bootID, readerID);
        return INU_METADATA__RET_SUCCESS;
    }
    else
    {
        /*Disable the wait and give event, this should mean that the mainDMA channel thread runs as normal */
        reader->dmaChannelHandle->regToMemoryParams.waitEventID     = 0;
        reader->dmaChannelHandle->regToMemoryParams.waitforEvent    = NO_WAIT;
        reader->dmaChannelHandle->regToMemoryParams.giveEventID     = 0;
        reader->dmaChannelHandle->regToMemoryParams.giveEvent       = NO_GIVE;
        LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"No metadata enabled for channel ID %lu \n",readerID);
        return ret;
    }
}

ERRG_codeE METADATA_TARGET_CONFIG_updateNumberMetadataLinesByReaderID(const INU_DEFSG_moduleTypeE bootID,const UINT8 readerID,
    CDE_DRVG_channelCfgT*  configuration)
{
    const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entry = NULL;
    ERRG_codeE ret = METADATA_TARGET_CONFIG_getChannelMapEntryByReaderID(bootID,readerID,&entry);
    if(ERRG_SUCCEEDED(ret))
    {
        /*Store this setting in regToMemoryCopyParams instead of adjusting the old structure */
        configuration->regToMemoryCopyParams.numberMetadataLines = entry->metadataLines;
        return INU_METADATA__RET_SUCCESS;
    }
    return INU_METADATA__ERR_INVALID_ARGS;
}
ERRG_codeE METADATA_TARGET_CONFIG_getFrameIDPhysicalAddress( UINT32 *frameIDPhyAddr, const const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entryt)
{
    if(entryt!=NULL && entryt->axiReader<=9)
    {
        UINT32 physicalAddress = PPE_BASE + (0x9004) + (0x100*entryt->axiReader); /*Register = PPE_BASE + 0x9004 + axiReader*0x100 */
        LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Calculated frame ID register to be 0x%X for channel %lu \n", physicalAddress,entryt->channelID);
        *frameIDPhyAddr = physicalAddress;
        return INU_METADATA__RET_SUCCESS;
    }
    else
    {
        return INU_METADATA__ERR_INVALID_ARGS;
    }
}
ERRG_codeE METADATA_TARGET_CONFIG_getTimestampPhysicalAddress( UINT32 *frameIDPhyAddr, const UINT32 slu)
{
    if(slu <=5 )
    {
        UINT32 physicalAddress = IAE_BASE + (0x2010 /*iae_snsr_ctrl_fsg_ts_slu0_lsb*/ )  
                                + (0x8*slu); /*Register = IAE_BASE + 0x2010 + slu*0x10 */
        LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Calculated Timestamp register to be 0x%X for channel %lu \n", physicalAddress,slu);
        *frameIDPhyAddr = physicalAddress;
        return INU_METADATA__RET_SUCCESS;
    }
    else
    {
        return INU_METADATA__ERR_INVALID_ARGS;
    }
}

#endif
