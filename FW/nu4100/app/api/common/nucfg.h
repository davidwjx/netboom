#ifndef __NUCFG_H__
#define __NUCFG_H__

#include "calibration.h"
#include "xml_db_modes.h"
#include "inu2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NUCFG_MAX_CONFIG_NAME  (128)
#define NUCFG_MAX_INPUTS      (12)
#define NUCFG_MAX_CHAN_INPUTS (4)
#define NUCFG_MAX_DB_META_CHANNELS (12)
#define NUCFG_MAX_CHAN_MODES  (4)
#define NUCFG_MAX_HW_ID_SIZE  (128)
#define NUCFG_MAX_DB_ISP_META_CHANNELS      (6)

#define NUCFG_CHAN_GROUP_NA ((unsigned)(~0))
#define NUCFG_CHAN_MODE_NA ((unsigned)(~0))

#define NUCFG_CHAN_GROUP_ANY ((unsigned int)0xaaaaaaaa)

#define NUCFG_DEFAULT_FPS (30)

typedef enum
{
   NUCFG_CHANGE_FORMAT_TO_RGB_E = 0,
   NUCFG_CHANGE_FORMAT_TO_YUV_E,
   NUCFG_CHANGE_FORMAT_TO_RAW10_E,
   NUCFG_CHANGE_FORMAT_TO_GRAY16_E,
   NUCFG_MAX_CHANGE_FORMAT__E,
}NUCFG_changeFromatE;

typedef enum
{
   NUCFG_FORMAT_GREY_16_E = 0,
   NUCFG_FORMAT_BAYER_16_E,
   NUCFG_FORMAT_RGB888_E,
   NUCFG_FORMAT_RGB666_E,
   NUCFG_FORMAT_RGB565_E,
   NUCFG_FORMAT_RGB555_E,
   NUCFG_FORMAT_RGB444_E,
   NUCFG_FORMAT_YUV420_8BIT_E,
   NUCFG_FORMAT_YUV420_8BIT_LEGACY_E,
   NUCFG_FORMAT_YUV420_10BIT_E,
   NUCFG_FORMAT_YUV422_8BIT_E,
   NUCFG_FORMAT_YUV422_10BIT_E,
   NUCFG_FORMAT_RAW6_E,
   NUCFG_FORMAT_RAW7_E,
   NUCFG_FORMAT_RAW8_E,
   NUCFG_FORMAT_RAW10_E,
   NUCFG_FORMAT_RAW12_E,
   NUCFG_FORMAT_RAW14_E,
   NUCFG_FORMAT_GEN_8_E,
   NUCFG_FORMAT_GEN_12_E,
   NUCFG_FORMAT_GEN_16_E,
   NUCFG_FORMAT_GEN_24_E,
   NUCFG_FORMAT_GEN_32_E,
   NUCFG_FORMAT_GEN_64_E,
   NUCFG_FORMAT_GEN_96_E,
   NUCFG_FORMAT_GEN_672_E,
   NUCFG_FORMAT_DEPTH_E,
   NUCFG_FORMAT_DISPARITY_E,
   NUCFG_FORMAT_DISPARITY_DEBUG_E,
   NUCFG_FORMAT_HISTOGRAM_E,
   NUCFG_FORMAT_NA_E,
   NUCFG_FORMAT_YUV420_SEMI_PLANAR_E,
   NUCFG_FORMAT_NUM_FORMATS_E,
} NUCFG_formatE;

typedef enum
{
   NUCFG_INPUT_TYPE_SENSOR_E_E = 0,
   NUCFG_INPUT_TYPE_INJECT_E_E,
   NUCFG_INPUT_TYPE_GEN_E_E,
   NUCFG_INPUT_TYPE_OTHER_E,
   NUCFG_NUM_INPUT_TYPES_E,
} NUCFG_inputTypeE;

typedef enum
{
   NUCFG_OUTPUT_TYPE_AXI_RD_E_E = 0,
   NUCFG_OUTPUT_TYPE_MIPI_TX_E,
   NUCFG_OUTPUT_TYPE_ISP_RD_E,
   NUCFG_NUM_OUTPUT_TYPES_E,
} NUCFG_outputTypeE;

typedef struct
{
	UINT32 sensorList[2];
	UINT32 numOfSensors;
}NUCFG_channelSrcSensorsList;

typedef struct
{
   UINT32 disparitySubPixleNumBits : 3;
   UINT32 disparityOrDepthNumBits  : 5;
   UINT32 depthPos                 : 3; 
   UINT32 minDisp                  : 11;
   UINT32 depthScaleFactor         : 10;
} NUCFG_depthParamsT;

typedef union
{
   UINT32             val;
   NUCFG_depthParamsT depthFields;
} NUCFG_formatDiscriptorU;

typedef enum
{
	NUCFG_INTER_MODE_LINE_BY_LINE_E = 0,
	NUCFG_INTER_MODE_PIXEL_BY_PIXEL_E = 1,
	NUCFG_INTER_MODE_FRAME_BY_FRAME_E = 2,
	NUCFG_INTER_MODE_UNKNOWN_E = 3,
	NUCFG_NUM_OF_INT_MODE_E
} NUCFG_interModeE;

typedef struct NUCFG_res
{
   UINT32 width;
   UINT32 height;
   UINT32 bpp;
   UINT32 x;
   UINT32 y;
   UINT32 stride;
   UINT32 bufferHeight;
} NUCFG_resT;

typedef struct
{
   UINT16 frameStartX;
   UINT16 frameStartY;
   UINT16 frameEndX;
   UINT16 frameEndY;
   UINT16 bufferWidth;
   UINT16 bufferHeight;
} NUCFG_dimentionsT;

typedef struct
{
   char *name;
   int frameNum;
} NUCFG_dpeFuncFileT;

typedef struct
{
   const CALIB_sectionDataT *tbl;
   unsigned int numSections;
} NUCFG_calibT;

typedef struct
{
   unsigned int role;
   unsigned int function;
   unsigned int mode;
   unsigned int tableType;
   unsigned int fps;
   unsigned int power_hz;
   unsigned int defaultExp;
   unsigned int totalExpTimePerSec;
   unsigned int expTimePerDutyCycle;
   unsigned int orientation;
   unsigned int exp_time; //us
   unsigned int gain;
   unsigned int vert_offset;
   unsigned int trigger_src;
   unsigned int trigger_delay;
   unsigned int model;
   unsigned int lensType;
   unsigned int projector_inst;
   unsigned int sensor_width;
   unsigned int sensor_height;
}NUCFG_sensorParamsT;

typedef struct
{
   //tbd
}NUCFG_injectParamsT;

typedef struct
{
   //tbd
}NUCFG_generatorParamsT;

typedef union
{
   NUCFG_sensorParamsT    sensorParams;
   NUCFG_injectParamsT    injectParams;
   NUCFG_generatorParamsT generatorParams;
}NUCFG_inputParamsU;

typedef struct
{
   unsigned int id;
   NUCFG_inputTypeE type;
   unsigned int inst;
   NUCFG_inputParamsU params;
} NUCFG_inputT;

typedef struct
{
   unsigned int id;
   unsigned int numInputs;
   NUCFG_inputT inputs[NUCFG_MAX_CHAN_INPUTS];
   UINT8 inSelect;
   unsigned int numModes;
   INT32 modeList[NUCFG_MAX_CHAN_MODES];
   unsigned int modeGroup;
   INT32 currentMode;
   NUCFG_resT resolution;
   NUCFG_formatE format;
   NUCFG_formatDiscriptorU formatDiscriptor;
   UINT32 numInterleaved;
   NUCFG_interModeE intreleavedMode;
   NUCFG_dimentionsT *userDim;
   NUCFG_outputTypeE outputType;
   UINT32 numLinesPerChunk;
} outChannelT;

ERRG_codeE NUCFG_init(NUCFG_calibT *calib, const char *xml, const char *xmlPath, const char *name,inu_nucfgH *nucfg);
void NUCFG_deInit(inu_nucfgH nucfg);
//query functions
unsigned int NUCFG_getNumInputs(inu_nucfgH nucfg);
ERRG_codeE NUCFG_getInputTable(inu_nucfgH nucfgH, NUCFG_inputT *);
ERRG_codeE NUCFG_checkXmlDataValidation(inu_nucfgH nucfg);
unsigned int NUCFG_getNumChannels(inu_nucfgH nucfg);
ERRG_codeE NUCFG_getChannelInfo(inu_nucfgH nucfgH, unsigned int index, outChannelT *);
ERRG_codeE NUCFG_getChannelTable(inu_nucfgH nucfgH, outChannelT *);
unsigned int NUCFG_getNumGroups(inu_nucfgH nucfgH);
NUCFG_depthParamsT* NUCFG_getDpeFuncParams();
//channel configuration functions
ERRG_codeE NUCFG_setChannelEnable(inu_nucfgH nucfgH, unsigned int index);
ERRG_codeE NUCFG_setHistEnable(inu_nucfgH nucfgH, unsigned int index);
ERRG_codeE NUCFG_setChannelDisable(inu_nucfgH nucfgH, unsigned int index);
ERRG_codeE NUCFG_setCvaChannelEnable(inu_nucfgH nucfgH, unsigned int index);
ERRG_codeE NUCFG_setChannelInterleave(inu_nucfgH nucfgH, unsigned int index, UINT8 inSelect);
ERRG_codeE NUCFG_setInjectResolution(inu_nucfgH nucfgH, unsigned int ch, unsigned int width, unsigned int height);
ERRG_codeE NUCFG_setDpeFunc(inu_nucfgH nucfgH, NUCFG_dpeFuncFileT dpeFuncFileList[], UINT8 len);
ERRG_codeE NUCFG_applyDpeFunc(inu_nucfgH nucfgH);
ERRG_codeE NUCFG_applyChannelDimentions(inu_nucfgH nucfgH);
ERRG_codeE NUCFG_setChannelDepthMode(inu_nucfgH nucfgH, unsigned int index, NUCFG_formatE);
//group (of channels) mode configuration
ERRG_codeE NUCFG_setGroupMode(inu_nucfgH nucfgH, unsigned int group, INT32 mode);
ERRG_codeE NUCFG_setGroupFps(inu_nucfgH nucfgH, unsigned int group, UINT32 fps);
ERRG_codeE NUCFG_applyGroupsMode(inu_nucfgH nucfgH);

//Display functions
void NUCFG_showInput(INU_DEFSG_logLevelE loglevel, NUCFG_inputT *input);
void NUCFG_showOutChannel(inu_nucfgH nucfgH, INU_DEFSG_logLevelE loglevel, outChannelT *outch);
void NUCFG_showChannelGroup(inu_nucfgH nucfgH, INU_DEFSG_logLevelE loglevel, unsigned int group);

//hw ids
unsigned int NUCFG_getChanHwIdLen(inu_nucfgH nucfgH, unsigned int index);
void NUCFG_getChanHwId(inu_nucfgH nucfgH, unsigned int index, char *buf, unsigned int bufsize);
unsigned int NUCFG_getInputHwIdLen(inu_nucfgH nucfgH, unsigned int index);
void NUCFG_getInputHwId(inu_nucfgH nucfgH, unsigned int index, char *buf, unsigned int bufsize);
void NUCFG_getXmlPathAndName(inu_nucfgH nucfgH, const char **xmlPath, const char **name);

ERRG_codeE NUCFG_saveDbHAndWriteXml(inu_nucfgH nucfgH, char *xmlbuf, char *xmlPath);
ERRG_codeE NUCFG_saveModDbHAndWriteXml(inu_nucfgH nucfgH, char *xmlbuf, char *xmlPath);
/* 
   Allows setting a PPU channel's scaling. Assumption is that the channel has PPU scaling ability 
   hSize must be multipile of 16
*/
ERRG_codeE NUCFG_setChannelScale(inu_nucfgH nucfgH, UINT32 hSize, UINT32 vSize, UINT32 channel);
ERRG_codeE NUCFG_setChannelDimentions(inu_nucfgH nucfgH, UINT32 channel, NUCFG_resT *userDim);
ERRG_codeE NUCFG_setChannelCrop(inu_nucfgH nucfgH, UINT32 channel, NUCFG_resT *userCrop);
ERRG_codeE NUCFG_setChannelIauBypass(inu_nucfgH nucfgH, UINT32 channel, UINT32 *iauNum);
ERRG_codeE NUCFG_setChannelChunk(inu_nucfgH nucfgH, UINT32 channel, UINT16 numLinesPerChunk);
ERRG_codeE NUCFG_setChannelFormat(inu_nucfgH nucfgH, UINT32 channel, NUCFG_changeFromatE reqFormat);
void NUCFG_getSensorSizes(UINT32 senModel, UINT32 senMode, UINT32 *senWidth, UINT32 *senHeight);
ERRG_codeE NUCFG_applyIspLinkToWriterData(inu_nucfgH h);

//utility functions
unsigned int NUCFG_format2Bpp(NUCFG_formatE);
BOOLEAN NUCFG_isBypass(void *xmldbH,UINT32 *bypassBitField);

ERRG_codeE NUCFG_linkChannelWritersToSensors(inu_nucfgH nucfgH, UINT32 ch, NUCFG_channelSrcSensorsList *sensorInst);
#ifdef __cplusplus
}
#endif

#endif //__NUCFG_H__

