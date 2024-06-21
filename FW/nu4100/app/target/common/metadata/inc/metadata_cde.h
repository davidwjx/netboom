#pragma once
#include "os_lyr.h"
#include "inu_metadata.h"
#include "iae_drv.h"
#define MAX_NUM_REGISTERED_SENSORS 2

typedef struct 
{
    int index;
    bool valid;
} METADATA_TARGET_CONFIG_ppu;
typedef struct 
{
    INU_metadata_Sensor name; 
    METADATA_TARGET_CONFIG_ppu ppu;   
    bool valid;
} METADATA_TARGET_CONFIG_registeredSensorIDs;
/*channelID to
  {
    SLU
    Core,
    VChannel,
    AxiReader,
    giveEventID,
    waitEventID
  }
  Map which has to be manually created so that the SOF updates can read out the frame ID from the SLU and from the
  AXI Reader readers.
*/
typedef struct 
{
    UINT32  channelID;                   /*Channel ID (Key)                                                                      */   
    INT32   slu;                         /*SLU Number                                                                            */             
    INT32   core;                         /*DMA core number (DMA core number)                                                     */
    INT32   vchannel;                     /*Vchannel number  (Virtual channel number)                                             */
    UINT32  axiReader;                   /*AXI reader number                                                                     */
    UINT32  giveEventID;                 /*Give Event ID                                                                         */
    UINT32  waitEventID;                 /*Wait Event ID                                                                         */
    bool    enabled;                     /*True = Enable metadata update, False = Disable metadata update                        */
    UINT32  frameIDRegister_phy;         /*Physical address of the frame ID register (0 == Automatically from axiReader number)  */
    UINT32  timestampRegister_phy;       /*Physical address of the timestamp register (0 == Automatically from SLU number)       */
    UINT32  metadataLines;               /*Number of metadata lines to send for this channel                                     */             
    METADATA_TARGET_CONFIG_registeredSensorIDs  sensors[MAX_NUM_REGISTERED_SENSORS];  /*Sensor IDs for this channel */
    IAE_DRVG_fsgCounterNumE fsgIndex;          /*FSG_FTRIG Index*/
    bool                    fsgUpdatesEnabled; /*True == Allow updates to the FSG pulse registers*/
    IAE_DRVG_fsgCounterSrc  counterSrc;        /*FSG Counter Source                                                              */
    bool                    fixedCounterSrc;    /*True == Always use the FSG Counter source described in counterSrc, 
                                                False = Can be changed via the API                                               */
    INT32   writerID;                      /*Writer ID, whne used SLU must be set to be DISABLE_SLU_SOF_ISR_METADATA_PROCESSING */
} const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap;