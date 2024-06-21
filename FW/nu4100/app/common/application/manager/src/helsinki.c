#include "helsinki.h"
#include "os_lyr.h"
#include "inu_metadata.h"
#if DEFSG_IS_TARGET
#include "gme_mngr.h"
#include "gpio_drv.h"
#include "gme_drv.h"
#include "log.h"
#include "metadata_target_config.h"
#include "inu_sensorsync.h"
#include "pss_drv.h"
#include "os_lyr.h"
#include "metadata_uart.h"
#include "inu_metadata_serializer.h"
#include "cmem.h"
#include "iae_drv.h"
#include "metadata_updater.h"
#include "metadata_sensor_updater.h"
#include "sensorsync_updater.h"
const UINT8 SW_SYNC_0_SRC = 12;
#define EXTERN_SYNC_START_INDEX 16
const UINT8 SLAVE_TRIGGER_SYNC_SOURCE = EXTERN_SYNC_START_INDEX + 11 ; /*GIO 11*/
const UINT8 TE_C2_SYNC_SRC = EXTERN_SYNC_START_INDEX + 14; /*GIO 14*/
const UINT8 TE_C1_SYNC_SRC = EXTERN_SYNC_START_INDEX + 15; /*GIO 15*/
#define ENABLE_EXTERNAL_FSG_TRIGGERING

#define ROI_API_NOT_AVAILABLE  {0,false}    /*Only the Gaze sensors will allow you to update the ROI (Region of interest).
                                            Therefore this macro is used on channels that don't have a PPU in their path
                                            or don't allow you to update the cropping  register values within the PPU  */
#define CVA_SPECIAL_CASE_ID 80                 /* The CVA paths are on the second graph which mean they clash with the IDs in the first graph
                                              , we don't search by the channel ID for the CVA so we can just make up a unique number*/

/*Please note that we no longer use the channel ID as the key due to clashes in the channel ID when you have multiple graphs*/
/*sw_graph_boot10065_SLAVE_UNIFIED_STEREO */
static METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap vchannelMapBoot65Slave[] =
{
    /*Stream_Track_Stereo*/
    {   
    1,                                            /*Channel ID                                                      */
    3,                                            /*SLU 3                                                           */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    0,                                            /*AXI Reader 0                                                    */
    15,                                           /* 15 is the give Event ID                                        */
    16,                                           /* 16 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER,     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    1,                                            /* 1 Row of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_Tracking, true, 0,INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}, /*Left tracking sensor (Sensor ID 0 and Enabled)*/
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_Tracking, true, 1,INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}} /*Right tracking sensor (Sensor ID 1 and Enabled)*/,
    IAE_DRVG_FSG_CNT_0_E,true,                     /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    IAE_DRVG_FSG_CNTSRC_0_E,true,                  /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be updated in SOF ISR*/
    },
    /*VST FF Stereo*/
    {   
    7,                                            /*Channel ID                                                      */
    5,                                            /*SLU 5                                                           */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    9,                                            /*AXI Reader 9                                                    */
    19,                                           /* 19 is the give Event ID                                        */
    20,                                           /* 20 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER,     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    1,                                            /* 1 Row of metadata */
    {{{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_FF_VST, true, 3,INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}, /*Right VST FF sensor (Sensor ID 3 and Enabled)*/
    {{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_FF_VST, true, 2,INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}} /*DISABLED */,
    IAE_DRVG_FSG_CNT_2_E,true,                     /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    IAE_DRVG_FSG_CNTSRC_2_E,true,                  /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be updated in SOF ISR*/
    },
    /*CVA Metadata*/
    {   
    CVA_SPECIAL_CASE_ID,                           /*NOT USED                                                       */
    DISABLE_SLU_SOF_ISR_METADATA_PROCESSING,      /*Disable SLU SOF ISR metadata updates                            */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    16,                                            /*CVA6 READER                                                     */
    21,                                           /* 21 is the give Event ID                                        */
    22,                                           /* 22 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    0X803F2F0,                                    /* Frame ID physical register address  = CVA_FREAK_FRAME_ID       */
    0x8038144,                                    /* Timestamp physical register address = ppe_axi_write1_frame_ts0_lsb_sts (LSB First)  */
    METADATA_CVA_SIZE/CVA_LINE_LENGTH,            /* Bufferwidth=24 so I have set 10 lines of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_CVA, true, 0, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}, /*Left CVA channel (Sensor ID 2 and Enabled)*/
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_CVA, false, 3,INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,false}} /*Right CVA channel (Sensor ID 3 and Enabled) */,
    IAE_DRVG_FSG_CNT_0_E,true,                     /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    IAE_DRVG_FSG_CNTSRC_0_E,true,                    /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    1                                             /*Metadata will be generated in the Writer operate function*/
    },
    /*Tracking mono stream (Reader 2) Left*/
    {   
    CVA_SPECIAL_CASE_ID+1,                        /*NOT USED                                                       */
    2,                                            /*SLU 2 (TODO: Potential optimization here where we disable this) */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    2,                                            /*Reader 2                                                        */
    23,                                           /* 23 is the give Event ID                                        */
    24,                                           /* 24 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER,     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    101,                                            /* Bufferwidth=24 so I have set 10 lines of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_CVA, true, 0, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}, /*Right CVA channel (Sensor ID 3 and disabled)*/
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_CVA, false, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,false}} /*Disabled */,
    IAE_DRVG_FSG_CNT_0_E,true,                     /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    IAE_DRVG_FSG_CNTSRC_0_E,true,                  /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be generated in the Writer operate function*/
    },
    /*Tracking mono stream (Reader 3) Right*/
    {   
    CVA_SPECIAL_CASE_ID+2,                        /*NOT USED                                                       */
    2,                                            /*SLU 3 (TODO: Potential optimization here where we disable this) */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    3,                                            /*Reader 3                                                        */
    25,                                           /* 25 is the give Event ID                                        */
    26,                                           /* 26 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER,     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    101,                                            /* Bufferwidth=24 so I have set 10 lines of metadata */
    {{{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_CVA, true, 1, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}, /*Right CVA channel (Sensor ID 3 and disabled)*/
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_CVA, false, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,false}} /*Disabled */,
    IAE_DRVG_FSG_CNT_0_E,true,                     /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    IAE_DRVG_FSG_CNTSRC_0_E,true,                  /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be generated in the Writer operate function*/
    },
    /*Stream_VST_AF_0 */
    {   
    2,                                            /*Channel ID (Used as the key)                                    */
    1,                                            /*SLU 1                                                           */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    ISP0_READER,/*See comment on the ISP_SPECIAL_CASE define                */
    27,                                           /* 27 is the give Event ID                                        */
    28,                                           /* 28 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    0x3010100,                                    /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    1,                                            /* 1 Row of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_AF_VST, true, 4, INU_metadata_Channel_Scale_Downscaled},ROI_API_NOT_AVAILABLE,true}, /*AF VST Sensor 4*/
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_Tracking, false, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,false}}, /*Right tracking sensor (Sensor ID 3 and Enabled)*/
    IAE_DRVG_FSG_CNT_1_E,true,                     /*FSG_FTRIG2 used and configurable*/
    IAE_DRVG_FSG_CNTSRC_1_E,true,                  /*Use FSG_Counter[2] amd don't let it be changed*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be updated in SOF ISR*/
    },
    /*Stream_VST_AF_1 */
    {   
    3,                                            /*Channel ID (Used as the key)                                    */
    4,                                            /*SLU 4                                                           */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    ISP1_READER,                                  /*See comment on the ISP_SPECIAL_CASE define                      */
    29,                                           /* 29 is the give Event ID                                        */
    30,                                           /* 30 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    0x3010120,                                    /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    1,                                            /* 1 Row of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_AF_VST, true, 5, INU_metadata_Channel_Scale_Cropped},ROI_API_NOT_AVAILABLE,true}, /*AF VST Sensor 5*/
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_Tracking, false, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,false}}, /*Right tracking sensor (Sensor ID 3 and Enabled)*/
    IAE_DRVG_FSG_CNT_1_E,true,                     /*FSG_FTRIG2 used and configurable*/
    IAE_DRVG_FSG_CNTSRC_1_E,true,                  /*Use FSG_Counter[2] amd don't let it be changed*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be updated in SOF ISR*/
    },
    
};   
#define BOOT65_MASTER
#define DISABLED 99
/*sw_graph_boot10065_MASTER_UNIFIED_STEREO */
static METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap vchannelMapBoot65Master[] =
{

    /*Stream_GAZE_Stereo */
    {   
    8,                                            /*Channel ID                                                      */
    5,                                            /*SLU 5                                                           */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    8,                                            /*AXI Reader 8 (Used as the key)                                  */
    16,                                           /* 16 is the give Event ID                                        */
    17,                                           /* 17 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER,     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    1,                                            /* 1 Row of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_Gaze, true, 0, INU_metadata_Channel_Scale_Normal}, ROI_API_NOT_AVAILABLE,true}, /* The gaze sensors have a second stream named Stream_GAZE_ROI 
                                                                                              which make use of the PPU.
                                                                                              Left gaze sensor (Sensor ID 0 and Enabled) -> SLU5->PPU3 */
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_Gaze, true, 1, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}}, /*Right gaze sensor (Sensor ID 1 and Enabled)-> SLU0->PPU2*/     
    IAE_DRVG_FSG_CNT_0_E,true,                    /*FSG_FTRIG0 used and configurable*/
    IAE_DRVG_FSG_CNTSRC_0_E,true,                 /*Use FSG_Counter[0] amd don't let it be changed*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING    /*Metadata will be updated in SOF ISR*/
    },
    #ifdef GAZE_ROI_STREAM_ENABLE
    /*Gaze ROI Left */
    {   
    6,                                            /*Channel ID                                                      */
    0,                                            /*SLU 0                                                           */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    4,                                            /*AXI Reader 4                                                    */
    18,                                           /* 18 is the give Event ID                                        */
    19,                                           /* 19 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER,     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    1,                                            /* 1 Row of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_Gaze_ROI, true, 0, INU_metadata_Channel_Scale_Cropped}, {2,true},true}, /* The gaze sensors have a second stream named Stream_GAZE_ROI 
                                                                                                which make use of the PPU.
                                                                                              Left gaze sensor (Sensor ID 0 and Enabled) -> SLU0->PPU2 */
    
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_Gaze_ROI, false, 1, INU_metadata_Channel_Scale_Cropped},{2,false},false}}, /*Right gaze sensor (Sensor ID 1 and Enabled)-> SLU0->PPU2*/     
    IAE_DRVG_FSG_CNT_0_E,true,                    /*FSG_FTRIG0 used and configurable*/
    IAE_DRVG_FSG_CNTSRC_0_E,true,                 /*Use FSG_Counter[0] amd don't let it be changed*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING    /*Metadata will be updated in SOF ISR*/
    },
    #endif
    /*Stream_Track_Stereo */
    {   
    1,                                            /*Channel ID                                                      */
    2,                                            /*SLU 2                                                           */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    0,                                            /*AXI Reader 0                                                    */
    20,                                           /* 20 is the give Event ID                                        */
    21,                                           /* 21 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER,     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    1,                                            /* 1 Row of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_Tracking, true, 2, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}, /*Left tracking sensor (Sensor ID 2 and Enabled)*/
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_Tracking, true, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}}, /*Right tracking sensor (Sensor ID 3 and Enabled)*/
    IAE_DRVG_FSG_CNT_1_E,true,                     /*FSG_FTRIG1 used and configurable*/
    IAE_DRVG_FSG_CNTSRC_1_E,true,                  /*Use FSG_Counter[1] amd don't let it be changed*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be updated in SOF ISR*/
    },
    /*Stream_VST_AF_0 */
    {   
    2,                                            /*Channel ID (Used as the key)                                    */
    1,                                            /*SLU 1                                                           */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    ISP0_READER,                                  /*This will be ISP0            */
    DISABLED,                                           /* DISABLED                                       */
    DISABLED,                                           /* DISABLED                                        */
    true,                                         /* Metadata enabled                                               */
    0x3010100,                                    /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    1,                                            /* 1 Row of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_AF_VST, true, 4, INU_metadata_Channel_Scale_Downscaled},ROI_API_NOT_AVAILABLE,true}, /*AF VST Sensor 4 */
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_Tracking, false, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,false}}, /*Right tracking sensor (Sensor ID 3 and Enabled)*/
    IAE_DRVG_FSG_CNT_2_E,true,                     /*FSG_FTRIG2 used and configurable*/
    IAE_DRVG_FSG_CNTSRC_2_E,true,                  /*Use FSG_Counter[2] amd don't let it be changed*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be updated in SOF ISR*/
    },
    /*Stream_VST_AF_1 */
    {
    4,                                            /*Channel ID (Used as the key)                                    */
    4,                                            /*SLU 4                                                           */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    ISP1_READER,                                  /*See comment on the ISP1_READER define */
    DISABLED,                                           /* DISABLED                                        */
    DISABLED,                                           /* DISABLED                                    */
    true,                                         /* Metadata enabled                                               */
    0x3010120,                                     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    1,                                            /* 1 Row of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_AF_VST, true, 5, INU_metadata_Channel_Scale_Cropped},ROI_API_NOT_AVAILABLE,true}, /*AF VST Sensor 5*/
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_Tracking, false, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,false}}, /*Right tracking sensor (Sensor ID 3 and Enabled)*/
    IAE_DRVG_FSG_CNT_2_E,true,                     /*FSG_FTRIG2 used and configurable*/
    IAE_DRVG_FSG_CNTSRC_2_E,true,                  /*Use FSG_Counter[2] amd don't let it be changed*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be updated in SOF ISR*/
    },
    /*CVA Metadata*/
    {   
    CVA_SPECIAL_CASE_ID,                           /*NOT USED                                                       */
    DISABLE_SLU_SOF_ISR_METADATA_PROCESSING,      /*Disable SLU SOF ISR metadata updates                            */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    16,                                            /*CVA6 READER                                                     */
    22,                                           /* 22 is the give Event ID                                        */
    23,                                           /* 23 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    0X803F2F0,                                    /* Frame ID physical register address  = CVA_FREAK_FRAME_ID       */
    0x8038144,                                    /* Timestamp physical register address = ppe_axi_write1_frame_ts0_lsb_sts (LSB First) */
    METADATA_CVA_SIZE/CVA_LINE_LENGTH,                         /* Bufferwidth=24 so I have set 10 lines of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_CVA, true, 2, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}, /*Left CVA channel (Sensor ID 2 and Enabled)*/
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_CVA, false, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,false}} /*Right CVA channel (Sensor ID 3 and Enabled) */,
    IAE_DRVG_FSG_CNT_1_E,true,                     /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    IAE_DRVG_FSG_CNTSRC_1_E,true,                  /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    1                                             /*Metadata will be generated in the Writer operate function*/
    },
    /*Tracking mono stream (Reader 2) Left*/
    {   
    CVA_SPECIAL_CASE_ID+1,                        /*NOT USED                                                       */
    2,                                            /*SLU 3 (TODO: Potential optimization here where we disable this) */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    2,                                            /*Reader 2                                                        */
    24,                                           /* 24 is the give Event ID                                        */
    25,                                           /* 25 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER,     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    101,                                            /* Bufferwidth=24 so I have set 10 lines of metadata */
    {{{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_CVA, true, 2, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}, /*Right CVA channel (Sensor ID 3 and disabled)*/
    {{INU_metadata_Channel_Side_Left,INU_metadata_SensorType_CVA, false, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,false}} /*Disabled */,
    IAE_DRVG_FSG_CNT_1_E,true,                     /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    IAE_DRVG_FSG_CNTSRC_1_E,true,                  /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be generated in the Writer operate function*/
    },
    /*Tracking mono stream (Reader 3) Right*/
    {   
    CVA_SPECIAL_CASE_ID+2,                        /*NOT USED                                                       */
    2,                                            /*SLU 3 (TODO: Potential optimization here where we disable this) */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    3,                                            /*Reader 3                                                        */
    26,                                           /* 26 is the give Event ID                                        */
    27,                                           /* 27 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER,     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    101,                                            /* Bufferwidth=24 so I have set 10 lines of metadata */
    {{{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_CVA, true, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,true}, /*Right CVA channel (Sensor ID 3 and disabled)*/
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_CVA, false, 3, INU_metadata_Channel_Scale_Normal},ROI_API_NOT_AVAILABLE,false}} /*Disabled */,
    IAE_DRVG_FSG_CNT_1_E,true,                     /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    IAE_DRVG_FSG_CNTSRC_1_E,true,                  /*TODO: FSG Configs will not be  configurable on the slave but I have enabled this functionality for testing*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING     /*Metadata will be generated in the Writer operate function*/
    },
    #ifdef GAZE_ROI_STREAM_ENABLE
    /*GAZE ROI RIght */
    {   
    6,                                            /*Channel ID                                                      */
    5,                                            /*SLU 5                                                           */
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA core                             */ 
    AUTOMATIC_DMA_MAPPING,                        /*Allow automatic mapping of DMA vchannel                         */
    5,                                            /*AXI Reader 5                                                    */
    28,                                           /* 28 is the give Event ID                                        */
    29,                                           /* 29 is the wait Event ID                                        */
    true,                                         /* Metadata enabled                                               */
    AUTOCALCULATE_FRAME_ID_PHYSICAL_REGISTER,     /* Frame ID physical register address  = Automatically calculated */
    AUTOCALCULATE_TIMESTAMP_PHYSICAL_REGISTER,    /* Timestamp physical register address = Automatically calculated */
    1,                                            /* 1 Row of metadata */
    {{{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_Gaze_ROI, true, 1, INU_metadata_Channel_Scale_Cropped}, {3,true},true}, /* The gaze sensors have a second stream named Stream_GAZE_ROI 
                                                                                                which make use of the PPU.
                                                                                              Left gaze sensor (Sensor ID 1 and Enabled) -> SLU5->PPU3 */
    
    {{INU_metadata_Channel_Side_Right,INU_metadata_SensorType_Gaze_ROI, false, 1, INU_metadata_Channel_Scale_Cropped},{2,false},false}}, /*Right gaze sensor (Sensor ID 1 and Enabled)-> SLU0->PPU2*/     
    IAE_DRVG_FSG_CNT_0_E,true,                    /*FSG_FTRIG0 used and configurable*/
    IAE_DRVG_FSG_CNTSRC_0_E,true,                 /*Use FSG_Counter[0] amd don't let it be changed*/
    DISABLE_WRITER_OPERATE_METADATA_PROCESSING    /*Metadata will be updated in SOF ISR*/
    },
    #endif
}; 
static HELSINKI_masterOrSlaveE masterorslave;
/**
 * @brief Resets the hardware type pin (GPIO 12 (IO_21) - UART0 SIN) back to UART functionality
 * 
 */
static void HELSINKI_resetMasterOrSlavePin()
{
  /*Restore the GME registers for UART 0 back to how they are originally */
  GME_DRVG_setIoMux(GME_DRVG_HW_UNIT_UART0_E);
}

static ERRG_codeE HELSINKI_configureHWTypePinsToRead()
{
  GPIO_DRVG_gpioSetDirParamsT directionParams;
  GME_DRVG_switchUART0SinToGPIOIn(); /*The GME registers contain the MUX to decide if we are UART or GPIO functionality for PIN 21*/
  GME_DRVG_switchUART0SoutToGPIOIn(); /*The GME registers contain the MUX to decide if we are UART or GPIO functionality for PIN 22*/

  /*GPIO 32 should be configured as a GPIO pin*/
  GME_DRVG_switchWAKEUPToGPIOIn();
  
  directionParams.direction = GPIO_DRVG_IN_DIRECTION_E;
  directionParams.gpioNum = GPIO_DRVG_GPIO_12_E;
  IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &directionParams);
  LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Set GPIO %d direction to %d\n", directionParams.gpioNum, directionParams.direction);

  directionParams.direction = GPIO_DRVG_IN_DIRECTION_E;
  directionParams.gpioNum = GPIO_DRVG_GPIO_13_E;
  IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &directionParams);
  LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Set GPIO %d direction to %d\n", directionParams.gpioNum, directionParams.direction);

  directionParams.direction = GPIO_DRVG_IN_DIRECTION_E;
  directionParams.gpioNum = GPIO_DRVG_GPIO_32_E;
  IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &directionParams);
  LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Set GPIO %d direction to %d\n", 32, directionParams.direction);
  return RET_SUCCESS;
}
static ERRG_codeE HELSINKI_getHWTypeFromPins(HELSINKI_masterOrSlaveE *hwType)
{
  GPIO_DRVG_gpioGetValParamsT getParams_GIO12,getParams_GIO13,getParams_GIO32;
  getParams_GIO12.gpioNum = GPIO_DRVG_GPIO_12_E; 
  ERRG_codeE ret_1 = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_GET_GPIO_VAL_CMD_E, &getParams_GIO12);
  LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Read GPIO HW pin %d as %d\n", getParams_GIO12.gpioNum,getParams_GIO12.val);

  getParams_GIO13.gpioNum = GPIO_DRVG_GPIO_13_E; 
  ERRG_codeE ret_2 = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_GET_GPIO_VAL_CMD_E, &getParams_GIO13);
  LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Read GPIO HW pin %d as %d\n", getParams_GIO13.gpioNum,getParams_GIO13.val);

  getParams_GIO32.gpioNum = GPIO_DRVG_GPIO_32_E; 
  ERRG_codeE ret_3 = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_GET_GPIO_VAL_CMD_E, &getParams_GIO32);
  LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Read GPIO HW pin %d as %d\n", 32,getParams_GIO32.val);

  if (hwType != NULL && ERRG_SUCCEEDED(ret_1) && ERRG_SUCCEEDED(ret_2) && ERRG_SUCCEEDED(ret_3))
  {
    /*Refactored this code so it looks the same as the kernelspace code*/
    const int GPIO_12_Value_UART0_SIN   =  (getParams_GIO12.val == GPIO_DRVG_GPIO_STATE_SET_E);
    const int GPIO_13_Value_UART0_SOUT  =  (getParams_GIO13.val == GPIO_DRVG_GPIO_STATE_SET_E);
    const int GPIO_32_Value_Wake_Up_2   =  (getParams_GIO32.val == GPIO_DRVG_GPIO_STATE_SET_E);

    #ifdef CONFIG_ARCH_HELSINKI_ADAPTER_HARDWARE
    {
        /*This device must be old hardware*/
        if(GPIO_12_Value_UART0_SIN && !GPIO_13_Value_UART0_SOUT)
        {
            LOGG_PRINT(LOG_INFO_E,NULL,"Old master hardware detected \n");
            *hwType = HELSINKI_MASTER_E;  
            return RET_SUCCESS;
        }
        else if(!GPIO_12_Value_UART0_SIN && GPIO_13_Value_UART0_SOUT)
        {
            LOGG_PRINT(LOG_INFO_E,NULL,"Old slave hardware detected \n");
            *hwType = HELSINKI_SLAVE_E;
            return RET_SUCCESS;
        }
        else
        {
            /*Unknown state, fall back to being a master*/
            LOGG_PRINT(LOG_ERROR_E,NULL,"Cannot determine if the hardware is a master or slave, defaulting to being a master \n");
            *hwType = HELSINKI_MASTER_E;
            return ERR_IO_ERROR;
        }
    }
    #else /*Use -d (or --adapter) argument for FW/nu4100/app/build/gp/build.sh to build for old adapter hardware */
    //TODO: This if statement is broken on S1 hardware IMD have. 
    //if(GPIO_12_Value_UART0_SIN && GPIO_13_Value_UART0_SOUT)
    {
        /* This device must be S1 hardware*/
        if(GPIO_32_Value_Wake_Up_2)
        {
            LOGG_PRINT(LOG_INFO_E,NULL,"S1 Master hardware detected \n");
            *hwType = HELSINKI_MASTER_E;  
            return RET_SUCCESS;
        }
        else
        {
            LOGG_PRINT(LOG_INFO_E,NULL,"S1 Slave hardware detected \n");
            *hwType = HELSINKI_SLAVE_E;
            return RET_SUCCESS;
        }
    }
    #endif
  }
  else
  {
    LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Failed to read GPIO Hardware Pin");
    return ERR_IO_ERROR;
  }

}
static ERRG_codeE HELSINKI_determineIfMasterOrSlave()
{
  HELSINKI_configureHWTypePinsToRead(); /*Set the GME mux to be GIOs 12, 13 and 32 */
  HELSINKI_masterOrSlaveE pin_state;
  ERRG_codeE ret = HELSINKI_getHWTypeFromPins(&pin_state); /*Uses GPIO 12, 13 and 32 to determine what type of hardware we are */
  HELSINKI_resetMasterOrSlavePin(); /* Only UART0 pins need reconfiguring as GPIO 32 can remain as a GPIO pin like it was before HELSINKI_determineIfMasterOrSlave was called */
  if (ERRG_SUCCEEDED(ret))
  {
    if (pin_state == HELSINKI_MASTER_E)
    {
      masterorslave = HELSINKI_MASTER_E;
    }
    else
    {
      masterorslave = HELSINKI_SLAVE_E;
    }
  }
  else
  {
    LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to read hardware type \n");
  }
  return ret;
}
static void HELSINKI_TofTriggerInit()
{
  /*Varjo have a requirement where they need the ToF trigger to be high until the ToF is enabled*/
  GPIO_DRVG_gpioSetDirParamsT directionParams;
  GPIO_DRVG_gpioSetValParamsT         gpioParams;
  GME_DRVG_modifyFTRIGMode(IAE_DRVG_FSG_CNT_3_E,TRIGGER_MANAGER); /*Switch to GPIO mode*/
  LOGG_PRINT(LOG_DEBUG_E,NULL,"Setting ToF trigger to be an output and high \n");
  directionParams.direction = GPIO_DRVG_OUT_DIRECTION_E;
  directionParams.gpioNum = GPIO_DRVG_GPIO_25_E;
  IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &directionParams);
  LOGG_PRINT(LOG_DEBUG_E, NULL, "GPIO: Set GPIO %d direction to %d\n", directionParams.gpioNum, directionParams.direction);
  /*Set GPIO 25 to be high */
  gpioParams.val = GPIO_DRVG_GPIO_STATE_SET_E;
  gpioParams.gpioNum = GPIO_DRVG_GPIO_25_E;
  IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
}
/**
 * @brief Initializes the GPIO for the master
 * 
 *
 */
static void HELSINKI_gpioMasterInit()
{
  GME_DRVG_modifyFTRIGMode(IAE_DRVG_FSG_CNT_3_E,FSG_BLOCK);
  GME_DRVG_enableAllFTrigIOOutputEnables();

  #ifdef ENABLE_EXTERNAL_FSG_TRIGGERING 
  /*
 "Following changes have been done to Luxor S2 design:
  Master NU4100
    TE_C1 moved to GIO15
    TE_C2 moved to GIO14 "
  */
  GME_DRVG_switchToGPIOForExternalEvents(GME_DRVG_EXTERNAL_EVENTS_14);
  GME_DRVG_switchToGPIOForExternalEvents(GME_DRVG_EXTERNAL_EVENTS_15);
 #endif
}
/**
 * @brief Initializes the GPIO for the slave
 * 
 *
 */
static void HELSINKI_gpioSlaveInit()
{
  GME_DRVG_enableAllFTrigIOOutputEnables();
  /*Varjo require the ToF to be high until the ToF trigger is enabled*/
  HELSINKI_TofTriggerInit();
#ifdef ENABLE_EXTERNAL_FSG_TRIGGERING 
  /*TODO: Waiting for new hardware to test
  Following changes have been done to Luxor S2 design:
  Slave NU4100:
    TE_C1 moved to GIO15
    TE_C2 moved to GIO14
    TRACKING_SYNC moved to GIO11
  */
  GME_DRVG_switchToGPIOForExternalEvents(GME_DRVG_EXTERNAL_EVENTS_11);
  GME_DRVG_switchToGPIOForExternalEvents(GME_DRVG_EXTERNAL_EVENTS_14);
  GME_DRVG_switchToGPIOForExternalEvents(GME_DRVG_EXTERNAL_EVENTS_15);
 #endif

}
ERRG_codeE HELSINKI_init()
{                                      
  /*This function uses IO_21 to read if the board is a master or slave type */
  ERRG_codeE ret = HELSINKI_determineIfMasterOrSlave();
  /*We must disable the PPS host pin until the RTC registers have been written otherwise the RTC will glitch and misbehave 
    I have done this by setting the PPS host pin to be in GPIO muxing until the RTC has been configured
  */
  GME_SetPPSHostPinMuxing(1);
  /*We then need to run the correct pin muxing function depending on if we're a slave or master*/
  if(HELSINKI_getMaster())
  {
    HELSINKI_gpioMasterInit();
  }
  else
  {
    HELSINKI_gpioSlaveInit();
  }
  /*The PSS has to be enabled so that we can use external events to trigger sensors using the FSG*/
  PSS_DRVG_enableWithoutDebouncing();
  METADATA_UART_init();
  return ret;
}

bool HELSINKI_getMaster()
{
  return masterorslave==HELSINKI_MASTER_E;
}
ERRG_codeE HELSINKI_getChannelMap(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap **map, UINT32 *mapEntries)
{
  const bool master = HELSINKI_getMaster();
  /*Updates **map to point to the appropriate map depending on if we're a slave or master */
  if(master)
  {
    *map = vchannelMapBoot65Master;
    *mapEntries = sizeof(vchannelMapBoot65Master) / sizeof(METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap);
  }
  else
  {
    *map = vchannelMapBoot65Slave;
    *mapEntries = sizeof(vchannelMapBoot65Slave) / sizeof(METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap);
  }
  return INU_METADATA__RET_SUCCESS;
}

INU_metadata_HardwareType HELSINKI_getHardwareType()
{
    if(HELSINKI_getMaster())
    {
        return INU_metadata_HardwareType_MASTER;
    }
    else
    {
        return INU_metadata_HardwareType_SLAVE;
    }
}





/**
 * @brief This deals with the special case of the VST AF sensors for the helsinki project.
 *
 *   1. The fps of VST AF sensors 832x608 is 75
 *
 */
void HELSINKI_changeVSTAFFsgCnt(int fps)
{
  LOGG_PRINT(LOG_INFO_E,NULL,"[HELSINKI] Changing VST AF sensors FSG cnt\n");
  if(HELSINKI_getMaster()){
      IAE_DRVG_setFSGCounterPeriod(IAE_DRVG_FSG_CNT_2_E,fps);
      IAE_DRVG_enableFTRIGOutput(IAE_DRVG_FSG_CNT_2_E);
  } else {
      IAE_DRVG_setFSGCounterPeriod(IAE_DRVG_FSG_CNT_1_E,fps);
      IAE_DRVG_enableFTRIGOutput(IAE_DRVG_FSG_CNT_1_E);
  }
}
/*This function is not delcared in inu_data_api.h due to a C++ template being in pb.h, 
inu_Data_api.h gets included with many ifdef c++ include guards which causes the pb.h to give a build time error
*/
ERRG_codeE inu_data__getMetadata(inu_dataH dataH, INU_Metadata_T* output);
ERRG_codeE HELSINKI_prepareCVAMetadataFromWriterOperateContext(UINT32 writerID,UINT32 * frameIDValue,UINT64 *timestampValue, inu_data* data)
{
  INU_Metadata_T metadata;
  UINT64 previousTime = 0;
  UINT64 currentTime = 0;
  ERRG_codeE ret = INU_METADATA__ERR_METADATA_BUFFER_FAIL;
  OS_LYRG_getTimeNsec(&previousTime);
  MEM_POOLG_bufDescT* bufDescP;
  inu_data__bufDescPtrGet(data, &bufDescP);
  /*DataP is offstet for the EV72 so I am instead using StartP for metadata*/
  CMEM_cacheWbInv(bufDescP->startP,METADATA_CVA_SIZE);
  if (bufDescP->startP != NULL)
  {
    ret = inu_metadata__deserialize((char*)bufDescP->startP, METADATA_CVA_SIZE, &metadata); /*Measured to take 50-70 microseconds - IMDT 1st March 2023*/
  }
  
  OS_LYRG_getTimeNsec(&currentTime);
  UINT64 timeDelta = currentTime-previousTime;
  if(ERRG_SUCCEEDED(ret))
  {
      METADATA_UPDATER_channelHandleT *entry = NULL;
      ERRG_codeE ret = METADATA_UPDATER_findEntryByWriterID(writerID,&entry);
      if(ERRG_SUCCEEDED(ret))
      {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Helsinki: Writer Operate: Decode Time: %llu ns, frameID %lu, Timestamp %llu  \n",timeDelta, metadata.frameID, metadata.timestamp );
        *frameIDValue = metadata.frameID;
        *timestampValue = metadata.timestamp;
        INU_metadata_HardwareType hwType = INU_metadata_HardwareType_MASTER;
        /*Let the host know if we're the master or slave sending this message*/
        if(HELSINKI_getMaster())
        {
          hwType = INU_metadata_HardwareType_MASTER;
        }
        else
        {
          hwType = INU_metadata_HardwareType_SLAVE;
        }
        INU_metadata_Sensor sensor = metadata.protobuf_packet.sensors[0];
        METADATA_SENSOR_UPDATER_updateSensorInfoAndHWType(entry,0,METADATA_SENSOR_UPDATER_N_1,sensor,hwType,true);
        if(sensor.side ==  INU_metadata_Channel_Side_Left)
        {
          LOGG_PRINT(LOG_DEBUG_E,NULL,"Helsinki: Writer Operate: Left side stream \n");
        }
        else
        {
          LOGG_PRINT(LOG_DEBUG_E,NULL,"Helsinki:  Writer Operate: Right side stream \n");
        }
      }
      return INU_METADATA__RET_SUCCESS;
  }
  else
  {
    LOGG_PRINT(LOG_INFO_E,NULL,"Decoding failed for the writer operate with retcode %lu", ret);
  }

  return ret;
}
bool HELSINKI_checkForSpecialCVAScenario(UINT32 writerID)
{
  METADATA_UPDATER_channelHandleT *channelHandle = NULL;
  ERRG_codeE ret = METADATA_UPDATER_findEntryByWriterID(writerID,&channelHandle);
  if(ERRG_SUCCEEDED(ret))
  {
    return true;
  }
  return false;
}
ERRG_codeE HELSINKI_specialCaseOffsetFrameBufferAndInvalidateCache(MEM_POOLG_bufDescT * buf, UINT32 *physicalAddress)
{
  UINT32 srcPhyAddress = 0;
  /*Invalidate the cache so that we can decode the metadata*/
  CMEM_cacheInv((void *)(buf->dataP), METADATA_CVA_SIZE);
  /*We need to use startP for the CVA becausse we offset dataP for the EV72*/
  ERRG_codeE ret = MEM_POOLG_getDataPhyAddrUsingStartP(buf,&srcPhyAddress);
  srcPhyAddress += 800;
  *physicalAddress = srcPhyAddress;
  return ret;
}
void HELSINKI_enableToFTrigger(void)
{
  if(!HELSINKI_getMaster())
  {
    GME_DRVG_modifyFTRIGMode(IAE_DRVG_FSG_CNT_3_E,FSG_BLOCK); /*Enable the ToF trigger*/

  }
}
/**
 * @brief For the tracking and gaze sensors we need to make sure that we perform both the left and right exposure updates at the same time
 * @param sensorType Sensor type
 * @return Return true
 */
bool HELSINKI_getGroupedExposureUpdatesRequired(INU_metadata_SensorType sensorType)
{
   if(sensorType == INU_metadata_SensorType_Tracking || sensorType == INU_metadata_SensorType_Gaze )
   {
    return true;
   }
   return false;
}
/**
 * @brief For the Tracking and Gaze we will ignore the right exposure updates and only process left exposure updates
 * @param sensorType Sensor type
 * @param side Sensor side
 * @return Returns true if we should ignore the sensor update
 */
bool HELSINKI_ignoreExposureUpdate(INU_metadata_SensorType sensorType, INU_metadata_Channel_Side side )
{
   if(sensorType == INU_metadata_SensorType_Tracking || sensorType == INU_metadata_SensorType_Gaze )
   {
    if(side == INU_metadata_Channel_Side_Right)
    {
      return true;
    }
   }
   return false;
}

bool HELSINKI_getMaximumISPLatency(UINT32 latency_us)
{
   return latency_us > HELSINKI_MAXIMUM_ISP_LATENCY;
}
bool HELSINKI_getEnableConstantMetadataUpdates(int READER)
{
  /*For the slave ISP0 stream, we will have per frame metadata updates so that we get WB and exposure values for each frame into the metadata!*/
  return (READER == ISP0_READER) && (!HELSINKI_getMaster());
}
#endif

/*For the helsinki project we have high frame rate sensors and a high sample rate IMU
We noticed that small transactions have a high cost on the USB bus, the effective throughput of 1024 sized transfers is only 30-50MBit/s
So we have reduced the number of USB transfers by up to three times by sending the header and the frame together plus also making sure that we only
send packets that are a multiple of 1024 bytes so that we don't get a short packet too.
*/
#define HEADER_SIZE                 (172)
#define CVA_Stream_Size             (82*801)
#ifdef ISP_BUFFER_ALIGN_SIZE
#define ISP_Stream_Downscaled_Size ((1120*832*1.5)+METADATA_SIZE_ALIGN(1120*2))
#define ISP_Stream_Cropped_Size   ((544*384*1.5)+METADATA_SIZE_ALIGN(544*2))//(544*384*1.5)
#else
#define ISP_Stream_Downscaled_Size (1120*832*1.5)
#define ISP_Stream_Cropped_Size    (544*384*1.5)
#endif

#define HELSINKI_MULTI_AR2020 1

#define GAZE_ROI_Size           (512*305) /*Includes 1 line of metadata*/
#define Track_Stereo_Stream (1600*801)/*Includes 1 line of metadata*/
#define GAZE_STEREO (1280*481)
#define IMU_SIZE    (4*16)+12
#ifdef HELSINKI_MULTI_AR2020
#define HELSINKI_USB_IF_1_FIXED_SIZE 1*(IMU_SIZE+HEADER_SIZE)
#define HELSINKI_USB_IF_2_FIXED_SIZE 0
#define HELSINKI_USB_IF_3_FIXED_SIZE 1*(GAZE_ROI_Size+HEADER_SIZE)
#define HELSINKI_USB_IF_4_FIXED_SIZE 0
#define HELSINKI_USB_IF_5_FIXED_SIZE ((68956))
#else
#define HELSINKI_USB_IF_1_FIXED_SIZE 1*(IMU_SIZE+HEADER_SIZE)
#define HELSINKI_USB_IF_2_FIXED_SIZE 1*(ISP_Stream_Downscaled_Size+HEADER_SIZE)
#define HELSINKI_USB_IF_3_FIXED_SIZE 1*(GAZE_STEREO+HEADER_SIZE)
#define HELSINKI_USB_IF_4_FIXED_SIZE 1*((ISP_Stream_Cropped_Size+HEADER_SIZE))
#define HELSINKI_USB_IF_5_FIXED_SIZE ((68956))
#endif

UINT32 HELSINKI_getURBSize(UINT8 endpointID)
{
/*For endpoints 1->5 we have enabled URB optimization*/
#ifdef HELSINKI_USB_OPTIMIZATION
  switch(endpointID)
  {
    case 0:
    return 0; /*Single URB optimization disabled for this endpoint*/
    case 1: /*USB_IF1*/
    return HELSINKI_USB_IF_1_FIXED_SIZE;
    case 2: /*USB_IF2*/
    return HELSINKI_USB_IF_2_FIXED_SIZE;
    case 3: /*USB_IF3*/
    return HELSINKI_USB_IF_3_FIXED_SIZE;
    case 4: /*USB_IF4*/
    return HELSINKI_USB_IF_4_FIXED_SIZE;
    case 5: /*USB_IF5*/
    return HELSINKI_USB_IF_5_FIXED_SIZE;
    default:
    return 0;
  }
#endif

  return 0;
}