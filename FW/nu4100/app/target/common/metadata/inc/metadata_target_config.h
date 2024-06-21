

#include "inu_metadata.h"
#include "inu_common.h"
#include "err_defs.h"
#include "iae_drv.h"
#include "metadata_cde.h"
#include "cde_mngr_new.h"
#include "cde_drv_new.h"

#ifdef __cplusplus
      extern "C" {
#endif

/* Defines */
#define AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER 0
#define AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER 0
#define AUTOMATIC_DMA_MAPPING -1
#define DISABLE_SLU_SOF_ISR_METADATA_PROCESSING -1      /*Disables metadata update within the SOF ISR */
#define DISABLE_WRITER_OPERATE_METADATA_PROCESSING -1   /*Disables metadata update within the writer operate function*/
#define USE_WRITER_ID 80 
#define ISP0_READER 23
#define ISP1_READER 26
/* The ISP DMAC doesn't support metadata updates but for now we wish to have sensor updates (Gain,Exposure,Cropping window) working for the ISP streams.
So I have created this special "Reader ID" which will switch off metadata updates for the ISP channels
*/
#define MAX_REGISTERED_METADATA_CHANNELS 16 
/*  @brief Gets the metadata configuration for a certain boot ID
    @param configParams - Output to store configuration parameters
    @param bootID -    Boot ID of the board
    @param readerID - Reader ID (Used to find map entry)
    @param reader - Reader channel (Used to read the core number and virtual channel number)
    @return returns an error code
*/ 
ERRG_codeE METADATA_TARGET_CONFIG_getConfigParamsByReaderID(CDE_DRVG_regToMemoryCopyConfigParams *configParams, const INU_DEFSG_moduleTypeE bootID,
    const UINT8 readerID, CDE_MNGRG_channnelInfoT*  reader);
/* @brief Gets the memory pool configuration for metadata
   @param metadata_pool_cfg  Pointer to where to store mem pool config
   @param modelType Model type of current device
   @param numBuffers Number of buffers to create
*/
ERRG_codeE METADATA_TARGET_CONFIG_getMemPoolConfig(MEM_POOLG_cfgT *metadata_pool_cfg, INU_DEFSG_moduleTypeE modelType,
                                                   UINT32 numBuffers);
/* @brief Looks for an entry in our channel map (using the boot ID to find the Map and readerID as the key in this map) 
    and sets entry to point to this entry if one is found
   @param bootID - Boot ID used to work out what map to use
   @param core   - readerID used to find an entry in the map
   @param entry  - Double pointer, updated to point to a found map entry if one has been found
   @return Returns an error code
*/
ERRG_codeE METADATA_TARGET_CONFIG_getChannelMapEntryByReaderID(const INU_DEFSG_moduleTypeE bootID,const UINT8 readerID, const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap **entry);
/*  @brief Updates the Readers GiveEvent and WaitIDs which wakeup the second metadata thread
    @param bootID - Boot ID used for lookup using METADATA_TARGET_CONFIG_getChannelMapEntry()
    @param readerID - Reader ID 
    @param reader - Reader channel configuration which is modified to store the give and wait Event IDs in
*/
ERRG_codeE METADATA_TARGET_CONFIG_updateReaderGiveEventAndWaitIDsByReaderID(const INU_DEFSG_moduleTypeE bootID,const UINT8 readerID,
    CDE_MNGRG_channnelInfoT*  reader);
/*  @brief There's an option not to hardcode the frame ID physical address and instead calculate it based off the Reader number 
    described in const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap
    @param frameIDPhyAddr -  Pointer to store the frameID physical address into
    @param entryt - Map entry used to lookup reader number in
*/
ERRG_codeE METADATA_TARGET_CONFIG_getFrameIDPhysicalAddress( UINT32 *frameIDPhyAddr,  const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap *entryt);
/*  @brief There's an option not to hardcode the timestamp physical address and instead calculate it based off the SLU Number. 
          This function can be used when this is being used
    @param timestampPhyAddr -  Pointer to store the timestamp address into
    @param entryt - Map entry used to lookup the SLU number in
*/ 
ERRG_codeE METADATA_TARGET_CONFIG_getTimestampPhysicalAddress( UINT32 *timestampPhyAddr, const UINT32 slu);
/* @brief Stores the number of metadata lines that are configured for this target within configuration
    @param bootID - Boot ID used for lookup using METADATA_TARGET_CONFIG_getChannelMapEntry()
    @param channelID - Used for lookup using METADATA_TARGET_CONFIG_getChannelMapEntry()
    @param  configuration - number of metadata lines stored within this variable which is then used to calculate frame buffer offset 
*/
ERRG_codeE METADATA_TARGET_CONFIG_updateNumberMetadataLinesByReaderID(const INU_DEFSG_moduleTypeE bootID,const UINT8 readerID, CDE_DRVG_channelCfgT*  configuration);
/**
 * @brief Finds a channel map entry by looking for an entry with the SensorID==SensorID
 * 
 * @param bootID    - Boot ID used for lookup using METADATA_TARGET_CONFIG_getChannelMapEntry()
 * @param SensorID  - Used for lookup using METADATA_TARGET_CONFIG_getChannelMapEntry()
 * @param sensorIndex This pointer will be updated with 0 or 1 depending on which SensorID entry out of the two SensorID entries it matches 
 * @param entry       double pointer, updated to point to a found map entry if one has been found
 * @return ERRG_codeE 
 */
ERRG_codeE METADATA_TARGET_CONFIG_getChannelMapEntryBySensorID(const INU_DEFSG_moduleTypeE bootID,const UINT16 SensorID,UINT32 *sensorIndex, const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap **entry);
/**
 * @brief Finds channel map entries by looking for an entry with the SensorID==SensorID
 * 
 *
 * @param bootID     - Boot ID used for lookup using METADATA_TARGET_CONFIG_getChannelMapEntry()
 * @param SensorID   - Used for lookup using METADATA_TARGET_CONFIG_getChannelMapEntry()
 * @param sensorIndex This double pointer will be updated with 0 or 1 depending on which SensorID entry out of the two SensorID entries it matches 
 * @param entry       double pointer, updated to point to a found map entry if one has been found
 * @param listSize Size of the sensorIndex and Entry arrays
 * @return Returns number of entries found
 */
int METADATA_TARGET_CONFIG_getChannelMapEntriesBySensorID(const INU_DEFSG_moduleTypeE bootID,const UINT16 SensorID,UINT32 *sensorIndex,  METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap **entry, size_t listSize);

#ifdef __cplusplus
}
#endif
