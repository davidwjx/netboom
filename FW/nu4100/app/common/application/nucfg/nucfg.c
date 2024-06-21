#include "inu_common.h"
#include "nucfg_priv.h"
#include <assert.h>
#include "nucfg.h"
#include <errno.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUCFG_MAX_HWPATHS (22)
#define NUCFG_MAX_HWPATH_LEN (16)
#define ENCODED_PATH_EMPTY_ENTRY ((UINT32)(~0))

#define NUCFG_MAX_DB_HIST_META_CHANNELS     (2)
#define NUCFG_MAX_DB_WRITERS_META_CHANNELS  (6)
#define NUCFG_MAX_DB_CVA_META_CHANNELS      (7)
#define NUCFG_MAX_DB_MIPI_TX_META_CHANNELS  (2)
#define NUCFG_MAX_DB_META_PATHS             (20)



#define nucfg_init_check  if(!xmldbH) {return NUCFG__ERR_UNEXPECTED;}
//#define nucfg_init_check
#define NUCFG_AXIRD_ID_CHECK(id) (assert(id < NUCFG_NUM_AXI_READERS))
#define NUCFG_INTERLEAVER_ID_CHECK(id) (assert(id < NUCFG_NUM_AXI_INTERLEAVERS))

#define RD_PATH_N_PART0_OFFSET(n)  NU4100_META_RD_0_PATH_##n##_PART_0_E
#define RD_PATH_N_PART1_OFFSET(n)  NU4100_META_RD_0_PATH_##n##_PART_1_E

#define NUCFG_MAX_CHANNELS (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS + NUCFG_MAX_DB_ISP_META_CHANNELS)
#define NUCFG_MAX_GROUPS (NUCFG_MAX_CHANNELS+1)
#define NUCFG_GROUP_NA_INDEX (NUCFG_MAX_GROUPS-1)
#define NUCFG_GROUP_MODE_NA ((INT32)(-1))
#define NUCFG_MAX_LINKED_WRITERS (6)
#define NUCFG_ISP_CHANNELS_START_ID (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS)

#define NUCFG_VGA_WIDTH (640)
#define NUCFG_HD_WIDTH  (1280)

#define NUCFG_DEFAULT_NUM_OF_BUFFERS  (20)

typedef UINT32 NUCFG_setT;
#define NUCFG_SET_MAX_ELEMENTS (32)
#define NUCFG_SET_MSK (NUCFG_SET_MAX_ELEMENTS-1)

#define VALIDATE_DPE_FUNC_VALS(numOfDpeFunc, dpeSetupArr) \
   if ((numOfDpeFunc == MAX_DPE_FUNCT_INST) && (dpeSetupArr[1].enable) && (dpeSetupArr[2].enable))\
   {\
      UINT32 p0_p1_mode_1, p0_p1_mode_2;\
      p0_p1_mode_1 = dpeSetupArr[1].p0_p1_mode;\
      p0_p1_mode_2 = dpeSetupArr[2].p0_p1_mode;\
      if ((p0_p1_mode_1 == p0_p1_mode_2) || (p0_p1_mode_1 == BOTH_P0_P1_ENABLED) || (p0_p1_mode_2 == BOTH_P0_P1_ENABLED))\
         return NUCFG__ERR_ILLEGAL_DPE_FUNC_CALL;\
   }

typedef struct
{
   NUCFG_hwId id;
   nuBlkTypeE type;
   unsigned int inst;
   unsigned int nodeId;
} chInputT;

typedef struct
{
   unsigned int pathLen;
   unsigned int idpath[NUCFG_MAX_HWPATH_LEN];
} hwPathT;

typedef struct
{
   GRAPHG_handleT gh;
   unsigned int root;
   unsigned int maxPaths;
   unsigned int currPath;
   hwPathT *paths;
} chPathBuild;

typedef struct
{
   nusoc_updateSensT sens;
   nusoc_updateSluT slu;
   nusoc_updateIauT iau;
   nusoc_updateWriterT writer;
   nusoc_updateDispT disToDepth;
} socPathUpdateT;

typedef struct
{
   unsigned int width;
   unsigned int height;
}NUCFG_sensorModeParam;

typedef struct
{
   UINT32 model;
   NUCFG_setT supportedModes;
   NUCFG_sensorModeParam modesParams[CALIB_NUM_OP_MODES_E];
}NUCFG_sensorRawParams;

typedef enum
{
   NUCFG_OV7251_SENSOR_E = 0,
   NUCFG_OV9282_SENSOR_E,
   NUCFG_OV5675_SENSOR_E,
   NUCFG_OV8856_SENSOR_E,
   NUCFG_OV2685_SENSOR_E,
   NUCFG_GC2145_SENSOR_E,
   NUCFG_XC9160_SENSOR_E,
   NUCFG_AMS_CGSS130_SENSOR_E,
   NUCFG_OV4689_SENSOR_E,
   NUCFG_AR0234_SENSOR_E,
   NUCFG_AR2020_SENSOR_E,
   NUCFG_AR2021_SENSOR_E, //virtual sensor , dual resolution for sensor 2020
   NUCFG_AR430_SENSOR_E,
   NUCFG_CGS132_SENSOR_E,
   NUCFG_OV9782_SENSOR_E,
   NUCFG_CGS031_SENSOR_E,
   NUCFG_VD56G3_SENSOR_E,
   NUCFG_VD55G0_SENSOR_E,
   NUCFG_OS05A10_SENSOR_E,
   NUCFG_GC2053_SENSOR_E,
   NUCFG_GENERIC_MODEL_E,
   NUCFG_NUM_SENSORS
}NUCFG_supprtedSensors;

typedef struct
{
   unsigned int id; //group id
   NUCFG_setT channels; //channels in the group
   INT32 currMode;
} chGroupT;

typedef struct
{
   GRAPHG_handleT gh;
   unsigned int numPaths;
   hwPathT paths[NUCFG_MAX_HWPATHS]; //TODO - allocate dynamically
   unsigned int numInputs;
   chInputT inputs[NUCFG_MAX_CHAN_INPUTS];
   NUCFG_setT inputSets[NUCFG_NUM_INPUT_TYPES_E];
   NUCFG_setT supportedCalibratedModes; //supported calibrated modes by this channel
   NUCFG_setT supportedSensorModes; //all modes which are supported from the sensors
   chGroupT *group;
} chStreamT;

typedef struct
{
   UINT32 writerInst;
   chInputT dummyInputs[NUCFG_MAX_CHAN_INPUTS];
   UINT32 verticalCropOffset[NUCFG_MAX_CHAN_INPUTS];
   UINT32 totalVerticalSize[NUCFG_MAX_CHAN_INPUTS];
   UINT32 horizontalCropOffset[NUCFG_MAX_CHAN_INPUTS];
   UINT32 totalHorizontalSize[NUCFG_MAX_CHAN_INPUTS];
   UINT32 sensorMode[NUCFG_MAX_CHAN_INPUTS];
   UINT32 sensorRole[NUCFG_MAX_CHAN_INPUTS];
   unsigned int dummyNumInputs;
} linkWriterSensors;

typedef struct
{
    linkWriterSensors linkWriterSensors;
    unsigned int ispRdNum;
} linkWriterSensorsForIsp;

typedef struct
{
   UINT32 group;
   UINT32 mode[NUCFG_MAX_GROUPS];
   UINT32 numOfFuncFiles;
   NUCFG_dpeFuncFileT *dpeFuncFileList;
   NUCFG_resT scaleDim[NUCFG_MAX_CHANNELS];
   NUCFG_resT cropDim[NUCFG_MAX_CHANNELS];
   UINT16 channelChunk[NUCFG_MAX_CHANNELS];
   UINT32 channelForCrop;
   UINT32 channelForScale;
   UINT32 channelForChunk;
   linkWriterSensorsForIsp linkWriterSensorsForIsp[NUCFG_MAX_LINKED_WRITERS];
   linkWriterSensors linkWriterSensors[NUCFG_MAX_LINKED_WRITERS];
   UINT32 numWritersWithCalibData;
   UINT32 numWritersWithCalibDataToIsp;
} nuCfgDbT;

typedef struct nuCfgT
{
   XMLDB_dbH xmldbH;
   NUCFG_socH socH;
   chStreamT streamTbl[NUCFG_MAX_CHANNELS];
   unsigned int numGroups;
   chGroupT groupTbl[NUCFG_MAX_GROUPS];
   NUCFG_calibT calib;
   struct nuCfgT *my_nucfg_orig;
   struct nuCfgT *my_nucfg_mod;
   nuCfgDbT *nuCfgDbP;
}nuCfgT;


typedef struct
{
   nuCfgT   nucfg_orig;
   nuCfgT   nucfg_mod;
   nuCfgDbT nuCfgDb;
   char     name[NUCFG_MAX_CONFIG_NAME];
   char     *xmlPath;
} nuCfgH;


static NUCFG_sensorRawParams sensorsRawParamsTbl[NUCFG_NUM_SENSORS];
static BOOLEAN NUCFG_isIaeBypass(void *xmldbH,UINT32 iaeInd);
static void setPathResolution(nuCfgT *nucfg, chStreamT *streamp, unsigned int p, unsigned int width, unsigned int height);

int NUCFG_getSensorsFromIau(unsigned int *iauToSensor)
{
   //ELAD: find solution to test modules
/*
   unsigned int ind,iauInd,sensorInd;
   for (ind=0;ind<20;ind++)
   {

      XMLDB_getValue(nucfg_mod->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, ind, META_PATHS_PATH_0_IAU_E), &iauInd);
      XMLDB_getValue(nucfg_mod->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, ind, META_PATHS_PATH_0_SENS_E), &sensorInd);
      iauInd = iauInd>>24;
      if (iauInd<4)
      {
         //XMLDB_getValue(nucfg_mod->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, ind, META_PATHS_PATH_0_SENS_E), &sensorInd);
         iauToSensor[iauInd] = sensorInd>>24;
      }
      printf("iau %x sensor %x ---%x %x %x %x\n",iauInd,sensorInd, iauToSensor[0],iauToSensor[1],iauToSensor[2],iauToSensor[3]);
   }
*/
   return 0;
}

static inline void SET_ADD(NUCFG_setT *s,unsigned int i)
{
   *s |= (1 << (i & NUCFG_SET_MSK));
}
static inline void SET_REM(NUCFG_setT *s, unsigned int i)
{
   *s &= ~(1 << (i & NUCFG_SET_MSK));
}
static inline NUCFG_setT SET_CUT(NUCFG_setT *s1, NUCFG_setT *s2)
{
   return (*s1 & *s2);
}

static inline NUCFG_setT SET_UNION(NUCFG_setT *s1,NUCFG_setT *s2)
{
   return (*s1 | *s2);
}
#if 0
static inline NUCFG_setT SET_DIFF(NUCFG_setT *s1,NUCFG_setT *s2)
{
   return (*s1 & (~*s2));//bits set in s1 and not set in s2
}
#endif

static inline void SET_CLEAR(NUCFG_setT *s)
{
   *s = 0;
}

static inline int SET_ISEMPTY(NUCFG_setT *s)
{
   return (*s == 0);
}

static inline int SET_HAS(NUCFG_setT *s, unsigned int i)
{
   return (*s & (1 << (i & NUCFG_SET_MSK)));
}

static inline void SET_ADD_ALL(NUCFG_setT *s,unsigned int max)
{
   unsigned int i;
   for(i = 0; (i < (max & NUCFG_SET_MSK)); i++)
      SET_ADD(s,i);
}

static inline unsigned int SET_REM_NEXT(NUCFG_setT *s)
{
   unsigned int i;
   for(i = 0; i < NUCFG_SET_MAX_ELEMENTS; i++)
   {
      if(SET_HAS(s,i))
      {
         SET_REM(s,i);
         break;
      }
   }
   return i;
}

#if 0
static void calibShow(INU_DEFSG_logLevelE log_level, nuCfgT *nucfg)
{
   unsigned i;
   for(i = 0; i < nucfg->calib.numSections; i++)
   {
      LOGG_PRINT(log_level,NULL,"section %d name=%d opMode= %d\n",i, nucfg->calib.tbl[i].sectionName,nucfg->calib.tbl[i].SensorOperatingMode);
      LOGG_PRINT(log_level,NULL,"  sensorVerticalOffset=%d\n",nucfg->calib.tbl[i].sensorVerticalOffset);
      LOGG_PRINT(log_level,NULL,"  sensorGainOffset=%d\n",nucfg->calib.tbl[i].sensorGainOffset);
      LOGG_PRINT(log_level,NULL,"  gainOffsetPartner=%d\n",nucfg->calib.tbl[i].gainOffsetPartner);
      LOGG_PRINT(log_level,NULL,"  verticalCropOffset=%d\n",nucfg->calib.tbl[i].verticalCropOffset);
      LOGG_PRINT(log_level,NULL,"  horizontalCropOffset=%d\n",nucfg->calib.tbl[i].horizontalCropOffset);
      LOGG_PRINT(log_level,NULL,"  sluOutputVerticalSize=%d\n",nucfg->calib.tbl[i].sluOutputVerticalSize);
      LOGG_PRINT(log_level,NULL,"  sluOutputHorizontalSize=%d\n",nucfg->calib.tbl[i].sluOutputHorizontalSize);
      LOGG_PRINT(log_level,NULL,"  ibVerticalLutOffset=%d\n",nucfg->calib.tbl[i].ibVerticalLutOffset);
      LOGG_PRINT(log_level,NULL,"  ibHorizontalLutOffset=%d\n",nucfg->calib.tbl[i].ibHorizontalLutOffset);
      LOGG_PRINT(log_level,NULL,"  ibOffset=%d\n",nucfg->calib.tbl[i].ibOffset);
      LOGG_PRINT(log_level,NULL,"  dsrVerticalLutOffset=%d\n",nucfg->calib.tbl[i].dsrVerticalLutOffset);
      LOGG_PRINT(log_level,NULL,"  dsrOutputVerticalSize=%d\n",nucfg->calib.tbl[i].dsrOutputVerticalSize);
      LOGG_PRINT(log_level,NULL,"  dsrOutputHorizontalSize=%d\n",nucfg->calib.tbl[i].dsrOutputHorizontalSize);
   }
}
#endif

static const char *calmode2str(INT32 mode)
{
   if(mode == CALIB_MODE_BIN_E)
      return "binning";
   if(mode == CALIB_MODE_FULL_E)
      return "full";
   if (mode == CALIB_MODE_VERTICAL_BINNING_E)
      return "verticalbinning";
   if (mode == CALIB_MODE_UXGA_E)
      return "uxga";
   return "na";
}

static const char *inputType2str(NUCFG_inputTypeE type)
{
   if (type == NUCFG_INPUT_TYPE_SENSOR_E_E)
      return "sensor";
   if (type == NUCFG_INPUT_TYPE_INJECT_E_E)
      return "writer";
   if (type == NUCFG_INPUT_TYPE_GEN_E_E)
      return "generator";
   if (type == NUCFG_INPUT_TYPE_OTHER_E)
      return "unknown input";
   return "unknown input";
}

static unsigned int hwIdLen(NUCFG_hwId hwid)
{
   unsigned int ret = 0;
   if(nusoc_hwId2Str(hwid))
   {
      ret = (unsigned int)strlen(nusoc_hwId2Str(hwid));
      if(ret > 0) ret--; //remove trailing dot
   }
   return ret;
}

static unsigned int copyHwId(NUCFG_hwId hwid, char *buf, unsigned int bufsize)
{
   size_t len = 0;
   size_t tocopy = 0;

   if(!bufsize)
      return 0;

   len = hwIdLen(hwid);
   tocopy = (len+1 <= bufsize) ? len : bufsize-1;
   memcpy(buf, hwid, tocopy);
   buf[tocopy]='\0';

   return (unsigned int) (tocopy+1);
}

static ERRG_codeE getDbField(nuCfgT *nucfg, NUFLD_blkE blk,unsigned int blk_num,XMLDB_pathE field, UINT32 *valP)
{
   return XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(blk, blk_num, field), valP);
}

static ERRG_codeE setDbField(nuCfgT *nucfg, NUFLD_blkE blk,unsigned int blk_num,XMLDB_pathE field, UINT32 val)
{
   return XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(blk, blk_num, field), val);
}

static UINT16 format2Bpp[NUCFG_FORMAT_NUM_FORMATS_E];

static void initFormat2BppTbl(void)
{
   format2Bpp[NUCFG_FORMAT_GREY_16_E] = 16;
   format2Bpp[NUCFG_FORMAT_BAYER_16_E] = 16;
   format2Bpp[NUCFG_FORMAT_RGB888_E] = 24;
   format2Bpp[NUCFG_FORMAT_RGB666_E] = 24;
   format2Bpp[NUCFG_FORMAT_RGB565_E] = 16;
   format2Bpp[NUCFG_FORMAT_RGB555_E] = 16;
   format2Bpp[NUCFG_FORMAT_RGB444_E] = 16;
   format2Bpp[NUCFG_FORMAT_YUV420_SEMI_PLANAR_E] = 12;
   format2Bpp[NUCFG_FORMAT_YUV420_8BIT_E] = 16;
   format2Bpp[NUCFG_FORMAT_YUV420_8BIT_LEGACY_E] = 16;
   format2Bpp[NUCFG_FORMAT_YUV420_10BIT_E] = 24;
   format2Bpp[NUCFG_FORMAT_YUV422_8BIT_E] = 16;
   format2Bpp[NUCFG_FORMAT_YUV422_10BIT_E] = 24;
   format2Bpp[NUCFG_FORMAT_RAW6_E] = 8;
   format2Bpp[NUCFG_FORMAT_RAW7_E] = 8;
   format2Bpp[NUCFG_FORMAT_RAW8_E] = 8;
   format2Bpp[NUCFG_FORMAT_RAW10_E]= 10;
   format2Bpp[NUCFG_FORMAT_RAW12_E]= 12;
   format2Bpp[NUCFG_FORMAT_RAW14_E]= 16;
   format2Bpp[NUCFG_FORMAT_GEN_8_E] = 8;
   format2Bpp[NUCFG_FORMAT_GEN_12_E] = 16;
   format2Bpp[NUCFG_FORMAT_GEN_16_E] = 16;
   format2Bpp[NUCFG_FORMAT_GEN_24_E] = 24;
   format2Bpp[NUCFG_FORMAT_GEN_32_E] = 32;
   format2Bpp[NUCFG_FORMAT_GEN_64_E] = 64;
   format2Bpp[NUCFG_FORMAT_GEN_96_E] = 96;
   format2Bpp[NUCFG_FORMAT_GEN_672_E] = 672;
   format2Bpp[NUCFG_FORMAT_DEPTH_E] = 24;
   format2Bpp[NUCFG_FORMAT_DISPARITY_E] = 24;
   format2Bpp[NUCFG_FORMAT_NA_E] = 16;
};

static  NUCFG_inputTypeE blkE2InputE(nuBlkTypeE blkE)
{
   if(blkE == NUCFG_BLK_SEN_E)
      return NUCFG_INPUT_TYPE_SENSOR_E_E;
   else if(blkE == NUCFG_BLK_AXIWR_E)
      return NUCFG_INPUT_TYPE_INJECT_E_E;
   else if (blkE == NUCFG_BLK_GEN_E)
       return NUCFG_INPUT_TYPE_GEN_E_E;
   else
      return NUCFG_INPUT_TYPE_OTHER_E;
}

static void initChPathBuild(chPathBuild *params)
{
   memset(params->paths[params->currPath].idpath, 0, sizeof(params->paths[params->currPath].idpath));
   params->paths[params->currPath].pathLen = 0;
}

static void chanPathCb(unsigned int len, unsigned int *path, void *arg)
{
   unsigned int i;
   chPathBuild *params = (chPathBuild *)arg;
   hwPathT *currPath;

   if(params->currPath >= params->maxPaths)
      return; //exceeded max paths

   if(len < 2)
      return;//path includes only root - ignore

   //update path len and copy path
   currPath = &params->paths[params->currPath];
   currPath->pathLen= len;
   for(i = 0; i < len; i++)
      currPath->idpath[i] = path[i];

   //prep for next path
   params->currPath++;
   if(params->currPath < params->maxPaths)
      initChPathBuild(params);
}

static void generateChanPaths(chStreamT *stream, unsigned int maxPaths)
{
   chPathBuild params;
   unsigned int v;
   params.gh = stream->gh;
   params.paths = stream->paths; //paths are copied to here
   params.maxPaths = maxPaths;
   params.currPath = 0; //will hold total number of paths on return
   params.root = 0;

   //path extraction for all leaves (i.e. inputs)
   for(v = 0; v < GRAPHG_getNumV(stream->gh); v++)
   {
      if(GRAPHG_degree(stream->gh, v, GRAPHG_DIR_OUT) == 0)
      {
         initChPathBuild(&params);
         GRAPHG_dfsAllPaths(stream->gh,params.root, v, chanPathCb, &params);
      }
   }
   stream->numPaths = params.currPath;
}

static void generatePaths(nuCfgT *nucfg)
{
   unsigned int i;
   for(i = 0; i < nusoc_getNumOutputs(nucfg->socH); i++)
   {
      generateChanPaths(&nucfg->streamTbl[i], NUCFG_MAX_HWPATHS);
   }
}

static ERRG_codeE createGraphs(nuCfgT *nucfg)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   unsigned int i;

   for(i = 0; i < nusoc_getNumOutputs(nucfg->socH); i++)
   {
      ret = nugraph_create(nucfg->socH, i, &nucfg->streamTbl[i].gh);
      if(ERRG_FAILED(ret))
         break;
   }

   return ret;
}

static void deleteGraphs(nuCfgT *nucfg)
{
   unsigned int i;

   if(!nucfg->socH)
      return;

   for(i = 0; i < nusoc_getNumOutputs(nucfg->socH); i++)
   {
      if(nucfg->streamTbl[i].gh)
      {
         nugraph_delete(nucfg->streamTbl[i].gh);
         nucfg->streamTbl[i].gh = NULL;
      }
   }
}


void showGraphs(nuCfgT *nucfg)
{
   unsigned int i;
   LOGG_PRINT(LOG_INFO_E,NULL,"nucfg: graphs\n");
   for(i = 0; i < nusoc_getNumOutputs(nucfg->socH); i++)
   {
      if((nucfg->streamTbl[i].gh) && (GRAPHG_getNumV(nucfg->streamTbl[i].gh) > 1))
         nugraph_show(nucfg->socH, nucfg->streamTbl[i].gh);
   }
}

static void getUserChunkLines(nuCfgT *nucfg, unsigned int chan, UINT16 *numLinesPerChunk)
{
   if ((nucfg->nuCfgDbP->channelForChunk & (1 << chan)) != 0x0)
   {
      *numLinesPerChunk = nucfg->nuCfgDbP->channelChunk[chan];
   }
   else
   {
      *numLinesPerChunk = 0;
   }
}

static void getUserChanDim(nuCfgT *nucfg, unsigned int chan, NUCFG_resT *dim)
{
    ERRG_codeE ret = NUCFG__RET_SUCCESS;

   if (chan < NUCFG_MAX_DB_META_CHANNELS)
   {
      ret = getDbField(nucfg, NUFLD_META_READERS_E, chan, META_READERS_RD_0_OUT_RES_WIDTH_E, &dim->width);
      if (ERRG_FAILED(ret)) { dim->width = 0; }
      ret = getDbField(nucfg, NUFLD_META_READERS_E, chan, META_READERS_RD_0_OUT_RES_HEIGHT_E, &dim->height);
      if (ERRG_FAILED(ret)) { dim->height = 0; }
      ret = getDbField(nucfg, NUFLD_META_READERS_E, chan, META_READERS_RD_0_OUT_RES_BPP_E, &dim->bpp);
      if (ERRG_FAILED(ret)) { dim->bpp = 0; }
      ret = getDbField(nucfg, NUFLD_META_READERS_E, chan, META_READERS_RD_0_OUT_RES_X_START_E, &dim->x);
      if (ERRG_FAILED(ret)) { dim->x = 0; }
      ret = getDbField(nucfg, NUFLD_META_READERS_E, chan, META_READERS_RD_0_OUT_RES_Y_START_E, &dim->y);
      if (ERRG_FAILED(ret)) { dim->y = 0; }
      ret = getDbField(nucfg, NUFLD_META_READERS_E, chan, META_READERS_RD_0_OUT_RES_STRIDE_E, &dim->stride);
      if (ERRG_FAILED(ret)) { dim->stride = 0; }
      ret = getDbField(nucfg, NUFLD_META_READERS_E, chan, META_READERS_RD_0_OUT_RES_BUFFERHEIGHT_E, &dim->bufferHeight);
      if (ERRG_FAILED(ret)) { dim->bufferHeight = 0; }
   }
   else if ((chan >= NUCFG_MAX_DB_META_CHANNELS) && (chan < (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS)))//hist
   {
      ret = getDbField(nucfg, NUFLD_META_HIST_E, (chan - NUCFG_MAX_DB_META_CHANNELS), META_HISTOGRAMS_HIST_0_OUT_RES_WIDTH_E, &dim->width);
      if (ERRG_FAILED(ret)) { dim->width = 0; }
      ret = getDbField(nucfg, NUFLD_META_HIST_E, (chan - NUCFG_MAX_DB_META_CHANNELS), META_HISTOGRAMS_HIST_0_OUT_RES_HEIGHT_E, &dim->height);
      if (ERRG_FAILED(ret)) { dim->height = 0; }
      ret = getDbField(nucfg, NUFLD_META_HIST_E, (chan - NUCFG_MAX_DB_META_CHANNELS), META_HISTOGRAMS_HIST_0_OUT_RES_BPP_E, &dim->bpp);
      if (ERRG_FAILED(ret)) { dim->bpp = 0; }
      ret = getDbField(nucfg, NUFLD_META_HIST_E, (chan - NUCFG_MAX_DB_META_CHANNELS), META_HISTOGRAMS_HIST_0_OUT_RES_X_START_E, &dim->x);
      if (ERRG_FAILED(ret)) { dim->x = 0; }
      ret = getDbField(nucfg, NUFLD_META_HIST_E, (chan - NUCFG_MAX_DB_META_CHANNELS), META_HISTOGRAMS_HIST_0_OUT_RES_Y_START_E, &dim->y);
      if (ERRG_FAILED(ret)) { dim->y = 0; }
      dim->stride = 0;
      dim->bufferHeight = 0;
   }
   else if ((chan >= (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS)) && (chan < (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS)))//cva
   {
      ret = getDbField(nucfg, NUFLD_META_CVA_RD_E, (chan - (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS)), META_CVA_READERS_CVA_RD_0_OUT_RES_WIDTH_E, &dim->width);
      if (ERRG_FAILED(ret)) { dim->width = 0; }
      ret = getDbField(nucfg, NUFLD_META_CVA_RD_E, (chan - (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS)), META_CVA_READERS_CVA_RD_0_OUT_RES_HEIGHT_E, &dim->height);
      if (ERRG_FAILED(ret)) { dim->height = 0; }
      ret = getDbField(nucfg, NUFLD_META_CVA_RD_E, (chan - (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS)), META_CVA_READERS_CVA_RD_0_OUT_RES_BPP_E, &dim->bpp);
      if (ERRG_FAILED(ret)) { dim->bpp = 0; }
      dim->x = 0;
      dim->y = 0;
      dim->stride = 0;
      dim->bufferHeight = 0;
   }
   else if ((chan >= (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS)))//isp
   {
       ret = getDbField(nucfg, NUFLD_META_ISP_RD_E, (chan - (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS)), META_ISP_READERS_ISP_RD_0_OUT_RES_WIDTH_E, &dim->width);
       if (ERRG_FAILED(ret)) { dim->width = 0; }
       ret = getDbField(nucfg, NUFLD_META_ISP_RD_E, (chan - (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS)), META_ISP_READERS_ISP_RD_0_OUT_RES_HEIGHT_E, &dim->height);
       if (ERRG_FAILED(ret)) { dim->height = 0; }
       ret = getDbField(nucfg, NUFLD_META_ISP_RD_E, (chan - (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS)), META_ISP_READERS_ISP_RD_0_OUT_RES_BPP_E, &dim->bpp);
       if (ERRG_FAILED(ret)) { dim->bpp = 0; }
       ret = getDbField(nucfg, NUFLD_META_ISP_RD_E, (chan - (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS)), META_ISP_READERS_ISP_RD_0_OUT_RES_X_START_E, &dim->x);
       if (ERRG_FAILED(ret)) { dim->x = 0; }
       ret = getDbField(nucfg, NUFLD_META_ISP_RD_E, (chan - (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS)), META_ISP_READERS_ISP_RD_0_OUT_RES_Y_START_E, &dim->y);
       if (ERRG_FAILED(ret)) { dim->y = 0; }
       ret = getDbField(nucfg, NUFLD_META_ISP_RD_E, (chan - (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS)), META_ISP_READERS_ISP_RD_0_OUT_RES_STRIDE_E, &dim->stride);
       if (ERRG_FAILED(ret)) { dim->stride = 0; }
       ret = getDbField(nucfg, NUFLD_META_ISP_RD_E, (chan - (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS)), META_ISP_READERS_ISP_RD_0_OUT_RES_BUFFERHEIGHT_E, &dim->bufferHeight);
       if (ERRG_FAILED(ret)) { dim->bufferHeight = 0; }
   }
}

static void getChanRes(nuCfgT *nucfg, unsigned int chan, NUCFG_resT *res)
{
   nublkH blk;
   NUCFG_resT dim = {0};
   GRAPHG_getVertex(nucfg->streamTbl[chan].gh,0, &blk); //root
   nusoc_getBlkOutputRes(nucfg->socH,blk,0,res);
   // Override user defined Res here
   getUserChanDim(nucfg, chan, &dim);


   if(((dim.stride != res->width) || (dim.bufferHeight != res->height)) && ((dim.stride != 0) || (dim.bufferHeight != 0)))
   {
      res->stride = dim.stride;
      res->width = dim.width;
      res->bufferHeight = dim.bufferHeight;
      res->height = dim.height;
      res->x = dim.x;
      res->y = dim.y;

      LOGG_PRINT(LOG_INFO_E,NULL,"nucfg: Channel %d dimentions override. W - %d/H - %d\n", chan, res->width, res->height );
   }
   else
   {
      res->stride = dim.width;
      res->bufferHeight = dim.height;
      res->x = 0;
      res->y = 0;
   }

}

static void getChanFormat(nuCfgT *nucfg, unsigned int chan, NUCFG_formatE *fmt)
{
   nublkH blk;
   GRAPHG_getVertex(nucfg->streamTbl[chan].gh,0, &blk); //root
   *fmt = nusoc_getBlkOutputFormat(nucfg->socH,blk,0);
}

static void getChanFormatDiscriptor(nuCfgT *nucfg, unsigned int chan, NUCFG_formatDiscriptorU *fmtDisc)
{
   nublkH blk;
   GRAPHG_getVertex(nucfg->streamTbl[chan].gh,0, &blk); //root
   nusoc_getBlkOutputFormatDiscriptor(nucfg->socH,blk,0, fmtDisc);
}


static void getChanOutputType(nuCfgT *nucfg, unsigned int chan, NUCFG_outputTypeE *outputType)
{
   nublkH blk;
   nuBlkTypeE type;
   unsigned int inst;

   GRAPHG_getVertex(nucfg->streamTbl[chan].gh,0, &blk); //root
   nusoc_getBlkTypeInst(nucfg->socH, blk, &type, &inst);

   if (type == NUCFG_BLK_DPHY_TX_E)
   {
      *outputType = NUCFG_OUTPUT_TYPE_MIPI_TX_E;
   }
   else if (type == NUCFG_BLK_ISPRD_E)
   {
       *outputType = NUCFG_OUTPUT_TYPE_ISP_RD_E;
   }
   else
   {
      *outputType = NUCFG_OUTPUT_TYPE_AXI_RD_E_E;
   }
}

static void getChanNumLinesPerChunk(nuCfgT *nucfg, unsigned int chan, UINT32 *numLinesPerChunk)
{
   ERRG_codeE ret;

   if (chan < NUCFG_MAX_DB_META_CHANNELS)
   {
       ret = XMLDB_getValue(nucfg->my_nucfg_mod->xmldbH, NUFLD_calcPath(NUFLD_META_READERS_E, chan, META_READERS_RD_0_NUMLINESPERCHUNK_E), numLinesPerChunk);
       if (ERRG_FAILED(ret))
       {
           *numLinesPerChunk = 0;
       }
   }
   else if (chan >= NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS)
   {
       // chunk mode is irrelevant for isp. TODO: move?
       ret = XMLDB_getValue(nucfg->my_nucfg_mod->xmldbH, NUFLD_calcPath(NUFLD_META_ISP_RD_E, chan -(NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS), META_ISP_READERS_ISP_RD_0_NUMLINESPERCHUNK_E), numLinesPerChunk);
       if (ERRG_FAILED(ret))
       {
           *numLinesPerChunk = 0;
       }
   }
   else
   {
       *numLinesPerChunk = 0;
       return;
   }
}


static void getChanHwId(nuCfgT *nucfg, unsigned int chan, NUCFG_hwId *id)
{
   nublkH blk;
   GRAPHG_getVertex(nucfg->streamTbl[chan].gh, 0, &blk);
   *id = nusoc_getBlkHwId(nucfg->socH, blk);
}
static void getInputHwId(nuCfgT *nucfg, unsigned int input, NUCFG_hwId *id)
{
   nublkH blk;
   if(input >= nusoc_getNumInputs(nucfg->socH))
      return;
   blk = nusoc_getInput(nucfg->socH, input);
   *id = nusoc_getBlkHwId(nucfg->socH, blk);
}

static void collectChanInputs(nuCfgT *nucfg, unsigned int chan)
{
   nublkH blk;
   unsigned int i;
   unsigned int cnt = 0;
   chStreamT *st = &nucfg->streamTbl[chan];
   GRAPHG_handleT gh = st->gh;

   cnt = 0;
   for(i = 0; i < GRAPHG_getNumV(gh); i++)
   {
      if(cnt == NUCFG_MAX_CHAN_INPUTS)
         break;

      if(GRAPHG_degree(gh,i, GRAPHG_DIR_OUT) == 0)
      {
         GRAPHG_getVertex(gh, i, &blk);
         if(nusoc_isInputBlk(nucfg->socH, blk))
         {
            st->inputs[cnt].id = nusoc_getBlkHwId(nucfg->socH, blk);
            st->inputs[cnt].nodeId = i;
            nusoc_getBlkTypeInst(nucfg->socH, blk, &st->inputs[cnt].type, &st->inputs[cnt].inst);
            SET_ADD(&(st->inputSets[blkE2InputE(st->inputs[cnt].type)]),st->inputs[cnt].inst);
            cnt++;
         }
      }
   }
   st->numInputs = cnt;
}

static void collectInputs(nuCfgT *nucfg)
{
   unsigned int ch;
   for(ch = 0; ch < NUCFG_MAX_CHANNELS; ch++)
   {
      collectChanInputs(nucfg,ch);
   }
}


static void getInputParams(nuCfgT *nucfg, NUCFG_inputT *input)
{
   ERRG_codeE ret;
   switch(input->type)
   {
      case(NUCFG_INPUT_TYPE_SENSOR_E_E):
      {
         memset(&input->params,0,sizeof(NUCFG_inputParamsU));
         input->params.sensorParams.totalExpTimePerSec = INU_DEFSG_INVALID;
         input->params.sensorParams.expTimePerDutyCycle = INU_DEFSG_INVALID;
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_ROLE_E), &input->params.sensorParams.role);
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_TABLE_TYPE_E), &input->params.sensorParams.tableType);
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_FUNCTION_E), &input->params.sensorParams.function);
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_OP_MODE_E), &input->params.sensorParams.mode);
         ret = XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_FPS_E), &input->params.sensorParams.fps);
         if (ERRG_FAILED(ret))
         {
            input->params.sensorParams.fps = NUCFG_DEFAULT_FPS;
         }
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_POWER_HZ_E), &input->params.sensorParams.power_hz);
//         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_EXP_MODE_E), &input->params.sensorParams.exp_mode);
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_DEFAULT_EXPOSURE_E), &input->params.sensorParams.defaultExp);
         ret = XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_TOTAL_EXP_TIME_PER_SEC_E), &input->params.sensorParams.totalExpTimePerSec);
         if (ERRG_FAILED(ret))
         {
            input->params.sensorParams.totalExpTimePerSec = INU_DEFSG_INVALID;
         }
         ret = XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_EXP_TIME_PER_DUTY_CYCLE_E), &input->params.sensorParams.expTimePerDutyCycle);
         if (ERRG_FAILED(ret))
         {
            input->params.sensorParams.expTimePerDutyCycle = INU_DEFSG_INVALID;
         }
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_ORIENTATION_E), &input->params.sensorParams.orientation);
         //       XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_EXP_MODE_E), &input->params.sensorParams.exp_time);todo
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_DEFAULT_GAIN_E), &input->params.sensorParams.gain);
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_VERT_OFFSET_E), &input->params.sensorParams.vert_offset);
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_TRIGGER_SRC_E), &input->params.sensorParams.trigger_src);
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_TRIGGER_DELAY_E), &input->params.sensorParams.trigger_delay);
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_MODEL_E), &input->params.sensorParams.model);
         XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, input->inst, SENSORS_SENS_0_LENS_TYPE_E), &input->params.sensorParams.lensType);
      }break;
      case(NUCFG_INPUT_TYPE_INJECT_E_E):
      {
         //printf("\tNeed to be handled - NUCFG_INPUT_TYPE_INJECT_E_E\n");
      }break;
      //    case(NUCFG_INPUT_TYPE_GENERATOR_E_E): todo
      default:
      break;
   }
}

static void getChanInputs(nuCfgT *nucfg, unsigned int ch, NUCFG_inputT *inputs, unsigned int *numInputs)
{
   unsigned int i;

   *numInputs = nucfg->streamTbl[ch].numInputs;
   for(i = 0; i < *numInputs; i++)
   {
      inputs[i].type = blkE2InputE(nucfg->streamTbl[ch].inputs[i].type);
      inputs[i].inst = nucfg->streamTbl[ch].inputs[i].inst;
      inputs[i].id = i;
      getInputParams(nucfg, &inputs[i]);
    }
}

/*
   Get the channel interleaving configuration based on the original and modified configurations.
*/
static void getChanSensorsInvolved(nuCfgT *cfg_orig, nuCfgT *cfg_mod, unsigned int chan, outChannelT *info)
{
    chStreamT *chp_mod, *chp_orig;
    unsigned int i, j;
    nublkH blkh;
    unsigned int inst;
    nuBlkTypeE type;

    chp_mod = &cfg_mod->streamTbl[chan];
    chp_orig = &cfg_orig->streamTbl[chan];

    info->inSelect = 0;
    //for each input and path (with that input) in the original configuration check if
    //it is selected in the modified configuration.
    for (i = 0; i < chp_orig->numInputs; i++)
    {
        //info->inputs[i].type = blkE2InputE(chp_orig->inputs[i].type);
        for (j = 0; j < chp_mod->numPaths; j++)
        {
            hwPathT *path = &chp_mod->paths[j];

            //get vertex for input of path in modified cfg
            GRAPHG_getVertex(chp_mod->gh, path->idpath[path->pathLen - 1], &blkh);
            nusoc_getBlkTypeInst(cfg_mod->socH, blkh, &type, &inst);
            //cmp to input type/inst

            //compare type inst in mod
            if ((type == chp_orig->inputs[i].type) && (inst == chp_orig->inputs[i].inst))
            {
                //Check if the path is active in the modified configuration
                info->inSelect |= (1 << i);
                break;
            }
        }
    }
}

static void getChanInterleavingInfoFromDb(nuCfgT *cfg_mod, unsigned int chan, UINT32 *numInter, NUCFG_interModeE *interMode)
{
   nublkH blk;

   GRAPHG_getVertex(cfg_mod->streamTbl[chan].gh, 0, &blk); //root
   nusoc_getInterleaveInfoFromDb(cfg_mod->socH, blk, numInter, (NUCFG_interModeE*)interMode);
}

static void clearMetaPath(nuCfgT *nucfg, unsigned int metaPathNum)
{
   //ENCODED_PATH_EMPTY_ENTRY
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_GEN_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_SENS_GROUP_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_SENS_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_MEDIATOR_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_MIPI_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_PARALLEL_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_INJECTION_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_SLU_PARALLEL_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_SLU_E), ENCODED_PATH_EMPTY_ENTRY);//META_PATHS_PATH_0_IPE_E
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_ISP_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_IAU_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_IAU_COLOR_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_HIST_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_PPU_NOSCL_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_PPU_SCL_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_PPU_HYBSCL_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_PPU_HYB_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_DPE_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_DEPTH_POST_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_RD_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_RDOUT_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_CVA_RD_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_CVA_RDOUT_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_CVA_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_VSC_CSI_TX_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_DPHY_TX_E), ENCODED_PATH_EMPTY_ENTRY);
   XMLDB_setValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, META_PATHS_PATH_0_ISP_RDOUT_E), ENCODED_PATH_EMPTY_ENTRY);
}

static void encodePath(nuCfgT *nucfg, unsigned int metaPathNum, unsigned int outch, unsigned int pathNum)
{
   unsigned int i,inst;
   nublkH blk;
   nuBlkTypeE type;
   hwPathT *path = &nucfg->streamTbl[outch].paths[pathNum];
   GRAPHG_handleT gh = nucfg->streamTbl[outch].gh;
   XMLDB_pathE offset;
   UINT32 val;
   unsigned int pathcnt = 0;



   //blocks not on the path are ff
   clearMetaPath(nucfg, metaPathNum);

   for(i = 0; i < path->pathLen; i++)
   {
      GRAPHG_getVertex(gh,path->idpath[i], &blk);
      nusoc_getBlkTypeInst(nucfg->socH, blk, &type, &inst);

      offset = NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, nusoc_getXmlDbPath(nucfg->socH, type));
      if(offset < XMLDB_NUM_PATHS_E)
      {
         val = (inst << 24) | pathcnt;
         XMLDB_setValue(nucfg->xmldbH, offset, val);
         pathcnt++;
      }
   }

   //add sensor group to meta data, only if the input is sensor.
   GRAPHG_getVertex(gh, path->idpath[path->pathLen-1], &blk);
   nusoc_getBlkTypeInst(nucfg->socH, blk, &type, &inst);
   if (type == NUCFG_BLK_SEN_E)
   {
       if (nucfg->streamTbl[outch].group->id != NUCFG_GROUP_NA_INDEX)
       {
           offset = NUFLD_calcPath(NUFLD_META_PATHS_E, metaPathNum, nusoc_getXmlDbPath(nucfg->socH, NUCFG_BLK_SEN_GROUP_E));
           if (offset < XMLDB_NUM_PATHS_E)
           {
               val = (nucfg->streamTbl[outch].group->id << 24) | pathcnt;
               XMLDB_setValue(nucfg->xmldbH, offset, val);
           }
       }
   }
}

static BOOLEAN iauBypass(nuCfgT *nucfg,unsigned int channel)
{

  unsigned int pathInd,inst,pathNum;
  nublkH blk;
  nuBlkTypeE type;
  hwPathT *path;
  chStreamT *chStream;
  BOOLEAN retVal = TRUE;

  chStream = &nucfg->streamTbl[channel];

  for (pathNum=0;pathNum<chStream->numPaths;pathNum++)
  {

     path = &nucfg->streamTbl[channel].paths[pathNum];
     for(pathInd = 0; pathInd < path->pathLen; pathInd++)
     {
        GRAPHG_getVertex(nucfg->streamTbl[channel].gh,path->idpath[pathInd], &blk);
        nusoc_getBlkTypeInst(nucfg->socH, blk, &type, &inst);
        if ((type == NUCFG_BLK_IAU_E) || (type == NUCFG_BLK_IAU_COLOR_E))
        {
           //iau exists in path
           if (type == NUCFG_BLK_IAU_COLOR_E)
              inst+=2;
           if (!NUCFG_isIaeBypass(nucfg->xmldbH,inst))
              return FALSE;
         }
     }
  }
  return retVal;
}


static void calcPathMap(nuCfgT *nucfg, unsigned int streamNum, UINT32 *pathMap, UINT32 *metaPathNum)
{
   unsigned int j;

   *pathMap = 0;
   for(j = 0; j < nucfg->streamTbl[streamNum].numPaths ; j++)
   {
      if(*metaPathNum >=  NUCFG_MAX_DB_META_PATHS)
         break; //max'd db paths

      if(nucfg->streamTbl[streamNum].paths[j].pathLen)
      {
         encodePath(nucfg, *metaPathNum,streamNum,j);
         *pathMap |= (1 << (*metaPathNum));
         (*metaPathNum)++;
      }
   }
}

static void updateDbMetaPaths(nuCfgT *nucfg)
{
   unsigned int i,metaPathNum = 0;

   UINT32 pathMap = 0;

   for(i = 0; i < NUCFG_MAX_DB_META_CHANNELS; i++)
   {
      calcPathMap(nucfg, i, &pathMap, &metaPathNum);
      setDbField(nucfg, NUFLD_META_READERS_E,i, META_READERS_RD_0_PATH_MAP_E,pathMap);
   }

   for(i = 0; i < NUCFG_MAX_DB_HIST_META_CHANNELS; i++)
   {
      calcPathMap(nucfg, i + NUCFG_MAX_DB_META_CHANNELS, &pathMap, &metaPathNum);
      setDbField(nucfg, NUFLD_META_HIST_E,i, META_HISTOGRAMS_HIST_0_PATH_MAP_E,pathMap);
   }

   for(i = 0; i < NUCFG_MAX_DB_CVA_META_CHANNELS; i++)
   {
      calcPathMap(nucfg, (i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS), &pathMap, &metaPathNum);
      setDbField(nucfg, NUFLD_META_CVA_RD_E,i, META_CVA_READERS_CVA_RD_0_PATH_MAP_E,pathMap);
   }

   for(i = 0; i < NUCFG_MAX_DB_MIPI_TX_META_CHANNELS; i++)
   {
      calcPathMap(nucfg, (i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS), &pathMap, &metaPathNum);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E,i, META_MIPI_TX_MIPI_TX_0_PATH_MAP_E,pathMap);
   }

   for (i = 0; i < NUCFG_MAX_DB_ISP_META_CHANNELS; i++)
   {
       calcPathMap(nucfg, (i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS), &pathMap, &metaPathNum);
       setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_PATH_MAP_E, pathMap);
   }

   for(i = metaPathNum; i < NUCFG_MAX_DB_META_PATHS; i++)
   {
      clearMetaPath(nucfg, i);
   }
}

/*
Update the db meta data - including dma parameters
*/
static void updateDbMetaChannelResolution(nuCfgT *nucfg, UINT32 channel, NUCFG_resT *res )
{
   if( channel < NUCFG_MAX_DB_META_CHANNELS )
   {
      setDbField(nucfg, NUFLD_META_READERS_E, channel, META_READERS_RD_0_OUT_RES_WIDTH_E,  res->width);
      setDbField(nucfg, NUFLD_META_READERS_E, channel, META_READERS_RD_0_OUT_RES_HEIGHT_E, res->height);
      //      setDbField(nucfg, NUFLD_META_READERS_E, channel, META_READERS_RD_0_OUT_RES_BPP_E,     res->bpp);
      setDbField(nucfg, NUFLD_META_READERS_E, channel, META_READERS_RD_0_OUT_RES_X_START_E, res->x);
      setDbField(nucfg, NUFLD_META_READERS_E, channel, META_READERS_RD_0_OUT_RES_Y_START_E, res->y);
      setDbField(nucfg, NUFLD_META_READERS_E, channel, META_READERS_RD_0_OUT_RES_STRIDE_E, res->stride);
      setDbField(nucfg, NUFLD_META_READERS_E, channel, META_READERS_RD_0_OUT_RES_BUFFERHEIGHT_E, res->bufferHeight);
      LOGG_PRINT(LOG_INFO_E,NULL,"nucfg: updated meta resolution for AXI RD channel %d\n", channel);
   }
   else if( channel < NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS)
   {
      UINT32 chIndex = channel -  (NUCFG_MAX_DB_META_CHANNELS)  ;
      LOGG_PRINT(LOG_WARN_E,NULL,"nucfg: not suppoted resolution update for channel %d\n", chIndex);
   }
   else if( channel < NUCFG_MAX_DB_META_CHANNELS +  NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS )
   {
      UINT32 chIndex = channel -  (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS) ;
      LOGG_PRINT(LOG_WARN_E,NULL,"nucfg: not suppoted resolution update for channel %d\n", chIndex);
   }
   else if( channel < NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS +  NUCFG_MAX_DB_MIPI_TX_META_CHANNELS )
   {
      UINT32 chIndex = channel -  (NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS) ;
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, chIndex, META_READERS_RD_0_OUT_RES_WIDTH_E,  res->width);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, chIndex, META_READERS_RD_0_OUT_RES_HEIGHT_E, res->height);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, chIndex, META_READERS_RD_0_OUT_RES_BPP_E,     res->bpp);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, chIndex, META_READERS_RD_0_OUT_RES_X_START_E, res->x);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, chIndex, META_READERS_RD_0_OUT_RES_Y_START_E, res->y);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, chIndex, META_READERS_RD_0_OUT_RES_STRIDE_E, res->stride);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, chIndex, META_READERS_RD_0_OUT_RES_BUFFERHEIGHT_E, res->bufferHeight);
      LOGG_PRINT(LOG_INFO_E,NULL,"nucfg: updated meta resolution for MIPI channel %d\n", chIndex);
   }
   else if (channel < NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS +  NUCFG_MAX_DB_MIPI_TX_META_CHANNELS + NUCFG_MAX_DB_ISP_META_CHANNELS)
   {
       UINT32 chIndex = channel - ( NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS);
       LOGG_PRINT(LOG_INFO_E, NULL, "nucfg: updating meta resolution for ISP channel to be %lu,%lu for ISP channel %lu \n",res->width,res->height, chIndex);
       setDbField(nucfg, NUFLD_META_ISP_RD_E, chIndex, META_ISP_READERS_ISP_RD_0_OUT_RES_WIDTH_E, res->width);
       setDbField(nucfg, NUFLD_META_ISP_RD_E, chIndex, META_ISP_READERS_ISP_RD_0_OUT_RES_HEIGHT_E, res->height);
       setDbField(nucfg, NUFLD_META_ISP_RD_E, chIndex, META_ISP_READERS_ISP_RD_0_OUT_RES_BPP_E, res->bpp);
       setDbField(nucfg, NUFLD_META_ISP_RD_E, chIndex, META_ISP_READERS_ISP_RD_0_OUT_RES_X_START_E, res->x);
       setDbField(nucfg, NUFLD_META_ISP_RD_E, chIndex, META_ISP_READERS_ISP_RD_0_OUT_RES_Y_START_E, res->y);
       setDbField(nucfg, NUFLD_META_ISP_RD_E, chIndex, META_ISP_READERS_ISP_RD_0_OUT_RES_STRIDE_E, res->stride);
       setDbField(nucfg, NUFLD_META_ISP_RD_E, chIndex, META_ISP_READERS_ISP_RD_0_OUT_RES_BUFFERHEIGHT_E, res->bufferHeight);
       LOGG_PRINT(LOG_INFO_E, NULL, "nucfg: updated meta resolution for ISP channel %d\n", chIndex);
   }
   else
   {
      LOGG_PRINT(LOG_WARN_E,NULL,"nucfg: not suppoted resolution update for channel %d\n", channel);
   }
}


/*
Update the db meta data - including dma parameters
*/
static void updateDbMetaResolutions(nuCfgT *nucfg)
{
   unsigned int i;
   UINT16 chunkLines = 0;
   ERRG_codeE ret = NUCFG__RET_SUCCESS;

   for(i = 0; i < NUCFG_MAX_DB_META_CHANNELS; i++)
   {
      NUCFG_resT res;
      NUCFG_resT userDim = {0};
      getChanRes(nucfg,i, &res);
      getUserChanDim(nucfg, i, &userDim);
      getUserChunkLines(nucfg, i, &chunkLines);

      if( (userDim.stride != 0) || (userDim.bufferHeight != 0) )
      {
          setDbField(nucfg, NUFLD_META_READERS_E, i, META_READERS_RD_0_OUT_RES_WIDTH_E, userDim.width);
          setDbField(nucfg, NUFLD_META_READERS_E, i, META_READERS_RD_0_OUT_RES_HEIGHT_E, userDim.height);
          setDbField(nucfg, NUFLD_META_READERS_E, i, META_READERS_RD_0_OUT_RES_BPP_E, res.bpp);
          setDbField(nucfg, NUFLD_META_READERS_E, i, META_READERS_RD_0_OUT_RES_X_START_E, userDim.x);
          setDbField(nucfg, NUFLD_META_READERS_E, i, META_READERS_RD_0_OUT_RES_Y_START_E, userDim.y);
          setDbField(nucfg, NUFLD_META_READERS_E, i, META_READERS_RD_0_OUT_RES_STRIDE_E, userDim.stride);
          setDbField(nucfg, NUFLD_META_READERS_E, i, META_READERS_RD_0_OUT_RES_BUFFERHEIGHT_E, userDim.bufferHeight);
      }
      else
      {
          setDbField(nucfg,NUFLD_META_READERS_E,i, META_READERS_RD_0_OUT_RES_WIDTH_E, res.width);
          setDbField(nucfg,NUFLD_META_READERS_E,i, META_READERS_RD_0_OUT_RES_HEIGHT_E,res.height);
          setDbField(nucfg,NUFLD_META_READERS_E,i, META_READERS_RD_0_OUT_RES_BPP_E,res.bpp);
          setDbField(nucfg,NUFLD_META_READERS_E,i, META_READERS_RD_0_OUT_RES_X_START_E,0);
          setDbField(nucfg,NUFLD_META_READERS_E,i, META_READERS_RD_0_OUT_RES_Y_START_E,0);
      }
      setDbField(nucfg, NUFLD_META_READERS_E, i, META_READERS_RD_0_NUMOFBUFFERS_E, NUCFG_DEFAULT_NUM_OF_BUFFERS);
      if (chunkLines)
      {
         setDbField(nucfg, NUFLD_META_READERS_E, i, META_READERS_RD_0_NUMLINESPERCHUNK_E, chunkLines);
      }
      else
      {
         setDbField(nucfg, NUFLD_META_READERS_E, i, META_READERS_RD_0_NUMLINESPERCHUNK_E, res.height);
      }
   }

   for(i = 0; i < NUCFG_MAX_DB_HIST_META_CHANNELS; i++)
   {
      NUCFG_resT res;
      getChanRes(nucfg,i + NUCFG_MAX_DB_META_CHANNELS, &res);
      setDbField(nucfg,NUFLD_META_HIST_E,i, META_HISTOGRAMS_HIST_0_OUT_RES_WIDTH_E, res.width);
      setDbField(nucfg,NUFLD_META_HIST_E,i, META_HISTOGRAMS_HIST_0_OUT_RES_HEIGHT_E,res.height);
      setDbField(nucfg,NUFLD_META_HIST_E,i, META_HISTOGRAMS_HIST_0_OUT_RES_BPP_E,res.bpp);
      setDbField(nucfg,NUFLD_META_HIST_E,i, META_HISTOGRAMS_HIST_0_OUT_RES_X_START_E,0);
      setDbField(nucfg,NUFLD_META_HIST_E,i, META_HISTOGRAMS_HIST_0_OUT_RES_Y_START_E,0);
   }

   for(i = 0; i < NUCFG_MAX_DB_CVA_META_CHANNELS; i++)
   {
      NUCFG_resT res;
      UINT32 max_kp = 0;
      getChanRes(nucfg,(i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS), &res);
      if(res.height != 0)       // TODO: Temp. Need to implement read according to CVA matrix
      {
        getDbField(nucfg, NUFLD_CVA_E,0, NU4100_CVA_DOG_BUCKET_KEYPOINTS_OUT_NUM_E,&max_kp);
      }
      setDbField(nucfg,NUFLD_META_CVA_RD_E,i, META_CVA_READERS_CVA_RD_0_OUT_RES_WIDTH_E, res.width);
      setDbField(nucfg,NUFLD_META_CVA_RD_E,i, META_CVA_READERS_CVA_RD_0_OUT_RES_HEIGHT_E,res.height);
      setDbField(nucfg,NUFLD_META_CVA_RD_E,i, META_CVA_READERS_CVA_RD_0_OUT_RES_BPP_E,res.bpp);
      setDbField(nucfg,NUFLD_META_CVA_RD_E,i, META_CVA_READERS_CVA_RD_0_OUT_RES_MAX_KP_E,max_kp);  // temp hardcoded for freak
      //printf("updateDbMetaResolutions-CVA:index-%d W-%d, H-%d, B-%d\n", i, res.width, res.height, res.bpp);
   }

   for(i = 0; i < NUCFG_MAX_DB_MIPI_TX_META_CHANNELS; i++)
   {
      NUCFG_resT res;
      getChanRes(nucfg,(i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS), &res);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, i, META_MIPI_TX_MIPI_TX_0_OUT_RES_WIDTH_E, res.width);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, i, META_MIPI_TX_MIPI_TX_0_OUT_RES_HEIGHT_E, res.height);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, i, META_MIPI_TX_MIPI_TX_0_OUT_RES_BPP_E, res.bpp);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, i, META_MIPI_TX_MIPI_TX_0_OUT_RES_X_START_E, res.x);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, i, META_MIPI_TX_MIPI_TX_0_OUT_RES_Y_START_E, res.y);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, i, META_MIPI_TX_MIPI_TX_0_OUT_RES_STRIDE_E, res.stride);
      setDbField(nucfg, NUFLD_META_MIPI_TX_E, i, META_MIPI_TX_MIPI_TX_0_OUT_RES_BUFFERHEIGHT_E, res.bufferHeight);
   }
   for (i = 0; i < NUCFG_MAX_DB_ISP_META_CHANNELS; i++)
   {
       NUCFG_resT res;
       NUCFG_resT userDim = { 0 };
       //getChanRes(nucfg, i, &res);
       getChanRes(nucfg, (i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS), &res);
       getUserChanDim(nucfg, (i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS), &userDim);
       getUserChunkLines(nucfg, (i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS), &chunkLines);

       if ((userDim.stride != 0) || (userDim.bufferHeight != 0))
       {
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_WIDTH_E, userDim.width);
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_HEIGHT_E, userDim.height);
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_BPP_E, res.bpp);
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_X_START_E, userDim.x);
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_Y_START_E, userDim.y);
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_STRIDE_E, userDim.stride);
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_BUFFERHEIGHT_E, userDim.bufferHeight);
       }
       else
       {
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_WIDTH_E, res.width);
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_HEIGHT_E, res.height);
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_BPP_E, res.bpp);
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_X_START_E, 0);
           setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_OUT_RES_Y_START_E, 0);
       }
       setDbField(nucfg, NUFLD_META_READERS_E, i, META_ISP_READERS_ISP_RD_0_NUMLINESPERCHUNK_E, res.height);//TODO: check if chunk mode
   }

   ///////////////////////
   /// the writers section must to be after isp, since we use data from there
   //////////////////////
    UINT32 width, height, sensorId;
    int chId = -1;
   for (i = 0; i < NUCFG_MAX_DB_WRITERS_META_CHANNELS; i++)
   {
       UINT32 j, k, sensors = 0, tmpWidth;

       if (nucfg->nuCfgDbP->numWritersWithCalibDataToIsp)
       {
           for (j = 0; j < nucfg->nuCfgDbP->numWritersWithCalibDataToIsp; j++)
           {
               if (nucfg->nuCfgDbP->linkWriterSensorsForIsp[j].linkWriterSensors.writerInst == i)
               {
                   for (k = 0; k < nucfg->nuCfgDbP->linkWriterSensorsForIsp[j].linkWriterSensors.dummyNumInputs; k++)
                   {
                       sensors |= (1 << nucfg->nuCfgDbP->linkWriterSensorsForIsp[j].linkWriterSensors.dummyInputs[k].inst);
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_INPUT_RES_Y_START_E, 0);
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_INPUT_RES_HEIGHT_E, nucfg->nuCfgDbP->linkWriterSensorsForIsp[j].linkWriterSensors.totalVerticalSize[k]);
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_INPUT_RES_X_START_E, 0);
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_INPUT_RES_WIDTH_E, nucfg->nuCfgDbP->linkWriterSensorsForIsp[j].linkWriterSensors.totalHorizontalSize[k]);
                       //resolution was taken from calib data and not from group mode, because interleaving case. the second channel in the interleaving does not hold data, just the main channel in the interleaving.
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_SENSOR_RESOLUTION_MODE_E, nucfg->nuCfgDbP->linkWriterSensorsForIsp[j].linkWriterSensors.sensorMode[k]);
                       //setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_SENSOR_ROLE_E, nucfg->nuCfgDbP->linkWriterSensors[j].sensorRole[k]);
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_SENSOR_ROLE_E, -1);//to prevent offset on startX in 'SEQ_MNGRP_setEnableWriterDma' function
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_INPUT_RES_STRIDE_E, (nucfg->nuCfgDbP->linkWriterSensorsForIsp[j].linkWriterSensors.totalHorizontalSize[k]) * 2);//to prevent offset on startX in 'SEQ_MNGRP_setEnableWriterDma' function

                       setDbField(nucfg, NUFLD_AXIWR_E, i, NU4100_PPE_AXI_WRITE0_SIZE_HSIZE_E, (nucfg->nuCfgDbP->linkWriterSensorsForIsp[j].linkWriterSensors.totalHorizontalSize[k]) - 1);
                       setDbField(nucfg, NUFLD_AXIWR_E, i, NU4100_PPE_AXI_WRITE0_SIZE_VSIZE_E, (nucfg->nuCfgDbP->linkWriterSensorsForIsp[j].linkWriterSensors.totalVerticalSize[k]) - 1);
                   }
                   setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_SRC_SENSORS_E, sensors);
                   break;
               }
           }
       }
       else
       {

           //update source sensors and crop sizes
           for (j = 0; j < nucfg->nuCfgDbP->numWritersWithCalibData; j++)
           {
               if (nucfg->nuCfgDbP->linkWriterSensors[j].writerInst == i)
               {
                   for (k = 0; k < nucfg->nuCfgDbP->linkWriterSensors[j].dummyNumInputs; k++)
                   {
                       sensors |= (1 << nucfg->nuCfgDbP->linkWriterSensors[j].dummyInputs[k].inst);
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_INPUT_RES_Y_START_E, nucfg->nuCfgDbP->linkWriterSensors[j].verticalCropOffset[k]);
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_INPUT_RES_HEIGHT_E, nucfg->nuCfgDbP->linkWriterSensors[j].totalVerticalSize[k]);
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_INPUT_RES_X_START_E, nucfg->nuCfgDbP->linkWriterSensors[j].horizontalCropOffset[k]);
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_INPUT_RES_WIDTH_E, nucfg->nuCfgDbP->linkWriterSensors[j].totalHorizontalSize[k]);
                       //resolution was taken from calib data and not from group mode, because interleaving case. the second channel in the interleaving does not hold data, just the main channel in the interleaving.
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_SENSOR_RESOLUTION_MODE_E, nucfg->nuCfgDbP->linkWriterSensors[j].sensorMode[k]);
                       setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_SENSOR_ROLE_E, nucfg->nuCfgDbP->linkWriterSensors[j].sensorRole[k]);
                   }
                   setDbField(nucfg, NUFLD_META_WRITERS_E, i, META_WRITERS_WT_0_SRC_SENSORS_E, sensors);
                   break;
               }
           }
       }
   }
}

static void updateDbMetaFormats(nuCfgT *nucfg)
{
   unsigned int i;
   for(i = 0; i < NUCFG_MAX_DB_META_CHANNELS; i++)
   {
      NUCFG_formatE format;
      getChanFormat(nucfg,i, &format);
      format = (format != NUCFG_FORMAT_NA_E) ? format : (NUCFG_formatE)~0;
      setDbField(nucfg,NUFLD_META_READERS_E,i, META_READERS_RD_0_FORMAT_E,format);
   }

   for(i = 0; i < NUCFG_MAX_DB_HIST_META_CHANNELS; i++)
   {
      NUCFG_formatE format;
      getChanFormat(nucfg,i + NUCFG_MAX_DB_META_CHANNELS, &format);
      format = (format != NUCFG_FORMAT_NA_E) ? format : (NUCFG_formatE)~0;
      setDbField(nucfg,NUFLD_META_HIST_E,i, META_HISTOGRAMS_HIST_0_FORMAT_E,format);
   }

   for(i = 0; i < NUCFG_MAX_DB_CVA_META_CHANNELS; i++)
   {
      NUCFG_formatE format;
      getChanFormat(nucfg,(i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS), &format);
      format = (format != NUCFG_FORMAT_NA_E) ? format : (NUCFG_formatE)~0;
      setDbField(nucfg,NUFLD_META_CVA_RD_E,i, META_CVA_READERS_CVA_RD_0_FORMAT_E,format);
   }
   for(i = 0; i < NUCFG_MAX_DB_MIPI_TX_META_CHANNELS; i++)
   {
      NUCFG_formatE format;
      getChanFormat(nucfg,(i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS), &format);
      format = (format != NUCFG_FORMAT_NA_E) ? format : (NUCFG_formatE)~0;
      setDbField(nucfg,NUFLD_META_MIPI_TX_E,i, META_MIPI_TX_MIPI_TX_0_FORMAT_E,format);
   }
   for (i = 0; i < NUCFG_MAX_DB_ISP_META_CHANNELS; i++)
   {
       NUCFG_formatE format;
       getChanFormat(nucfg, (i + NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_MIPI_TX_META_CHANNELS), &format);
       format = (format != NUCFG_FORMAT_NA_E) ? format : (NUCFG_formatE)~0;
       setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_FORMAT_E, format);
   }
}
static void disableDbMetaChannels(nuCfgT *nucfg)
{
   unsigned int i;
   for(i = 0; i < NUCFG_MAX_DB_META_CHANNELS; i++)
   {
      setDbField(nucfg,NUFLD_META_READERS_E,i, META_READERS_RD_0_ENABLE_E,0);
   }

   for(i = 0; i < NUCFG_MAX_DB_HIST_META_CHANNELS; i++)
   {
      setDbField(nucfg,NUFLD_META_HIST_E,i, META_HISTOGRAMS_HIST_0_ENABLE_E,0);
   }

   for(i = 0; i < NUCFG_MAX_DB_CVA_META_CHANNELS; i++)
   {
      setDbField(nucfg,NUFLD_META_CVA_RD_E,i, META_CVA_READERS_CVA_RD_0_ENABLE_E,0);
   }

   for(i = 0; i < NUCFG_MAX_DB_MIPI_TX_META_CHANNELS; i++)
   {
      setDbField(nucfg,NUFLD_META_MIPI_TX_E,i, META_MIPI_TX_MIPI_TX_0_ENABLE_E,0);
   }
   for (i = 0; i < NUCFG_MAX_DB_ISP_META_CHANNELS; i++)
   {
       setDbField(nucfg, NUFLD_META_ISP_RD_E, i, META_ISP_READERS_ISP_RD_0_ENABLE_E, 0);
   }
}

static void updateDbMeta(nuCfgT *nucfg)
{
   updateDbMetaPaths(nucfg);
   updateDbMetaResolutions(nucfg);
   updateDbMetaFormats(nucfg);
}

/*
 clean soc,graphs and paths.
*/
static void nucfgDbClean(nuCfgDbT *nucfgDb)
{
   UINT32   i=0;
   if (nucfgDb->dpeFuncFileList)
   {
      for (i=0;i<nucfgDb->numOfFuncFiles; i++)
      {
         free(nucfgDb->dpeFuncFileList[i].name);
      }
      free(nucfgDb->dpeFuncFileList);
   }
   memset(nucfgDb, 0, sizeof(nuCfgDbT));
}

/*
 clean soc,graphs and paths.
*/
static void nucfgClean(nuCfgT *nucfg)
{
   deleteGraphs(nucfg);
   if(nucfg->socH)
   {
      nusoc_deInit(nucfg->socH);
      nucfg->socH = NULL;
   }
   memset(nucfg->streamTbl, 0, sizeof(nucfg->streamTbl));
}

static void xmldbClose(nuCfgT *nucfg)
{
   if(nucfg->xmldbH)
   {
      XMLDB_close(nucfg->xmldbH);
      nucfg->xmldbH = NULL;
   }
}

static void nucfgCleanAll(inu_nucfgH nucfg)
{
   nuCfgH *nucfgP = (nuCfgH*)nucfg;
   nucfgClean(&nucfgP->nucfg_mod);
   xmldbClose(&nucfgP->nucfg_mod);
   nucfgClean(&nucfgP->nucfg_orig);
   xmldbClose(&nucfgP->nucfg_orig);
   nucfgDbClean(&nucfgP->nuCfgDb);
   free(nucfgP->xmlPath);
   free(nucfgP);
}

static unsigned int index2ModeTbl(CALIB_sensorOperatingModeE **tbl)
{
   static CALIB_sensorOperatingModeE index2Mode[CALIB_NUM_OP_MODES_E];
   unsigned int index = 0;
   index2Mode[index++] = CALIB_MODE_BIN_E;
   index2Mode[index++] = CALIB_MODE_FULL_E;
   index2Mode[index++] = CALIB_MODE_USER_DEFINE_E;
   index2Mode[index++] = CALIB_MODE_VERTICAL_BINNING_E;
   index2Mode[index++] = CALIB_MODE_UXGA_E;
   index2Mode[index++] = CALIB_MODE_UNKNOWN;

   if(tbl)
      *tbl = index2Mode;
   return index;
}

static int chmode2Index(int mode)
{
   unsigned int i,size;
   CALIB_sensorOperatingModeE *tbl;

   size = index2ModeTbl(&tbl);
   for(i = 0; i < size; i++)
   {
      if((CALIB_sensorOperatingModeE)mode == tbl[i])
        break;
   }
   return (i < size) ? (int)i : (-1);
}

static unsigned int chLutModeInd(int mode)
{
   unsigned int calibLutModes[CALIB_NUM_OP_MODES_E]={CALIB_LUT_MODE_BIN_E,CALIB_LUT_MODE_FULL_E,CALIB_NUM_LUT_MODES_E,CALIB_LUT_MODE_VERTICAL_BINNING_E,CALIB_NUM_LUT_MODES_E};

   return calibLutModes[mode%CALIB_NUM_OP_MODES_E];
}

static int index2chmode(unsigned int index)
{
   unsigned int mode = CALIB_MODE_UNKNOWN;
   unsigned int size;
   CALIB_sensorOperatingModeE *tbl;
   size = index2ModeTbl(&tbl);
   if(index < size)
      mode = tbl[index];

   return mode;
}

/*
*/
static void getChanModes(nuCfgT *nucfg, unsigned int ch, INT32 *modeList,unsigned int *numModes)
{
   NUCFG_setT modeSet;
   unsigned int s;
   unsigned int modeCnt;
   //Get supported modes
   modeSet = SET_UNION(&nucfg->streamTbl[ch].supportedCalibratedModes, &nucfg->streamTbl[ch].supportedSensorModes);

   modeCnt = 0;
   while(!SET_ISEMPTY(&modeSet))
   {
      s = SET_REM_NEXT(&modeSet);
      modeList[modeCnt] = index2chmode(s);
      modeCnt++;
      if(modeCnt >= NUCFG_MAX_CHAN_MODES)
         break; //ignore if extra modes are supported
   }
   *numModes = modeCnt;
}

static void getChanGroup(nuCfgT *nucfg, unsigned int ch, unsigned int *group, INT32 *mode)
{
   //Get channel's group and group's current mode

   if((nucfg->streamTbl[ch].group) && (nucfg->streamTbl[ch].group->id != NUCFG_GROUP_NA_INDEX))
      *group = nucfg->streamTbl[ch].group->id;
   else
      *group = NUCFG_CHAN_MODE_NA;

   if((nucfg->streamTbl[ch].group) && (nucfg->streamTbl[ch].group->currMode != NUCFG_GROUP_MODE_NA))
      *mode = nucfg->streamTbl[ch].group->currMode;
   else
      *mode = NUCFG_CHAN_GROUP_NA;
}

/*static int cmpInputs(chInputT *in1, chInputT *in2)
{
   return (((in1->type == in2->type) && (in1->inst == in2->inst)) ? 1 : 0);
}
static UINT32 getCommonInputs(chInputT *inputs1, unsigned int numInputs1, chInputT *inputs2, unsigned int numInputs2)
{
   unsigned int i,j;
   UINT32 intersect = 0;

   for(i = 0; i < numInputs1; i++)
   {
      for(j = 0; j < numInputs2; j++)
      {
         if(cmpInputs(&inputs1[i],&inputs2[j]) && (i<32))
            intersect |= (1<<i);
      }
   }
   return intersect;
}
*/

static void initGroups(nuCfgT *nucfg)
{
   unsigned int i;
   //clear nucfg group related members
   for(i = 0; i < NUCFG_MAX_CHANNELS; i++)
      nucfg->streamTbl[i].group = NULL;
   for(i = 0; i < NUCFG_MAX_GROUPS; i++)
   {
      SET_CLEAR(&nucfg->groupTbl[i].channels);
      nucfg->groupTbl[i].currMode = NUCFG_GROUP_MODE_NA;
      nucfg->groupTbl[i].id = NUCFG_GROUP_NA_INDEX;
   }
   nucfg->numGroups = 0;
}

/*
   check common inputs between 2 input lists. Return bit map of indices to first input list.
*/
static int checkCommonInputs(nuCfgT *nucfg, unsigned int ch0, unsigned int ch1)
{
   unsigned int i;
   NUCFG_setT *sets0 = nucfg->streamTbl[ch0].inputSets;
   NUCFG_setT *sets1 = nucfg->streamTbl[ch1].inputSets;
   NUCFG_setT common_set;
   int common = 0;

   for(i = 0; i < NUCFG_NUM_INPUT_TYPES_E; i++)
   {
      common_set = SET_CUT(&sets0[i], &sets1[i]);
      if(!(SET_ISEMPTY(&common_set)))
      {
         common = 1;
         break;
      }
   }
   return common;
}
/*
   commonInputGroup()
   recursive implementation:
   The common input group for a ch is the union of the common input groups
   for all channels with which it has a (direct) common input.
*/
static void commonInputGroup(nuCfgT *nucfg, NUCFG_setT *setp, unsigned int ch)
{
   unsigned int j;
   UINT32 common;

   SET_ADD(setp,ch);
   for(j = 0; j < NUCFG_MAX_CHANNELS; j++)
   {
      if(SET_HAS(setp, j))
         continue; //already visited j

      //collect common inputs for all channels j with common input to ch
      common = checkCommonInputs(nucfg, ch,j);
      if(common)
         commonInputGroup(nucfg, setp, j);
   }
}

/*
 Assign channels to groups for this configuration.
 A channel group is a group of channels s.t. each channel in the group
 has at least one channel also in the group with a common input.

 The purpose of channel group is that if an operation mode is used for
 one of the channels it must be used for all of them.

 Note the group relation is transitive - if channel C0 has a common input
 with C1; C1 has a (different) common input with C2; then all three
 channels must be in the same group.

 This is a connected component problem which can be solved with a dfs/bfs
 style walk on the connected component.
   void common_input_component(set, c)
      add c to set
      for each ch j
         if j is in set skip
         if j has common input with c
            common_input_compnent(set, j)
*/
static void assignGroups(nuCfgT *nucfg)
{
   unsigned int ch,j;
   NUCFG_setT chSet;
   NUCFG_setT group;
   assert(NUCFG_MAX_CHANNELS < NUCFG_SET_MAX_ELEMENTS);//sanity check

   initGroups(nucfg);

   SET_CLEAR(&chSet);
   SET_ADD_ALL(&chSet,NUCFG_MAX_CHANNELS);

   while(!SET_ISEMPTY(&chSet))
   {
      ch = SET_REM_NEXT(&chSet);
      if(nucfg->streamTbl[ch].numInputs == 0)
      {
         //all channels with no inputs in single group
         SET_ADD(&(nucfg->groupTbl[NUCFG_GROUP_NA_INDEX].channels),ch);
         nucfg->groupTbl[NUCFG_GROUP_NA_INDEX].currMode = NUCFG_GROUP_MODE_NA;
         nucfg->streamTbl[ch].group = &nucfg->groupTbl[NUCFG_GROUP_NA_INDEX];
      }
      else
      {
         SET_CLEAR(&group);
         commonInputGroup(nucfg, &group,ch);
         //add group to group table
         nucfg->groupTbl[nucfg->numGroups].channels = group;
         nucfg->groupTbl[nucfg->numGroups].id = nucfg->numGroups;
         //set the group table entry for each channel in the group
         while(!SET_ISEMPTY(&group))
         {
            j = SET_REM_NEXT(&group);
            nucfg->streamTbl[j].group = &nucfg->groupTbl[nucfg->numGroups];
            SET_REM(&chSet,j); //done with these channels
         }
         nucfg->numGroups++;
      }
   }
}

/*
   Try to match the input (from xmldb) to a calibration section name
   This is done by the sensor role (left/right/color) and possibly other criteria.
*/
static int matchInput2Calib(chInputT *input, int *section)
{// calibration will hold sensor id and not sensor name-"left/right.."
    unsigned int inst = input->inst;

   if (input->type == NUCFG_BLK_SEN_E)
   {
         *section = inst + CALIB_SECTION_SENSOR_0_PARAMETERS_E;
         return 0;
   }
   return -1;
}

/*
   return the n'th  calibration section matching the input in the calib table.
*/
static void getInputCalibN(nuCfgT *nucfg, chInputT *input, unsigned int n, const CALIB_sectionDataT **sectionP)
{
   unsigned int j;
   int calibName = 0;

   if(matchInput2Calib(input, &calibName) < 0)
      return;

   for(j = 0; j < nucfg->calib.numSections ; j++)
   {
      if(nucfg->calib.tbl[j].sectionNum == (CALIB_sectionNumE)calibName)
      {
         if(n ==0)
         {
            *sectionP = &nucfg->calib.tbl[j];
            break;
         }
         n--;
      }
   }
}

/*
 return number of sections in the calibration data which match the input - i.e. the number of modes
 supported in the calibration data.
*/
static unsigned int getInputNumModes(nuCfgT *nucfg, chInputT *input)
{
   unsigned int j,m;
   int calibNum = 0;

   if(matchInput2Calib(input, &calibNum) < 0)
      return 0;

   m = 0;
   for(j = 0; j < nucfg->calib.numSections; j++)
   {
      if(nucfg->calib.tbl[j].sectionNum == (CALIB_sectionNumE)calibNum)
         m++;
   }
   return m;
}

static ERRG_codeE getLinkToSensorList(nuCfgT *nucfg, UINT32 writerInst, linkWriterSensors **link)
{
    UINT32 i;
    for (i = 0; i < nucfg->nuCfgDbP->numWritersWithCalibData; i++)
    {
        if (nucfg->nuCfgDbP->linkWriterSensors[i].writerInst == writerInst)
        {
            *link = &nucfg->nuCfgDbP->linkWriterSensors[i];
            return NUCFG__RET_SUCCESS;
        }
    }

    for (i = 0; i < nucfg->nuCfgDbP->numWritersWithCalibDataToIsp; i++)
    {
        if (nucfg->nuCfgDbP->linkWriterSensorsForIsp[i].linkWriterSensors.writerInst == writerInst)
        {
            *link = &nucfg->nuCfgDbP->linkWriterSensorsForIsp[i].linkWriterSensors;
            return NUCFG__RET_SUCCESS;
        }
    }
    return NUCFG__ERR_INVALID_ARGS;
}

static void markCalibrationMode(nuCfgT *nucfg, chInputT *input, unsigned int ch)
{
    const CALIB_sectionDataT *section;
    unsigned int numModes;
    int modeId;
    unsigned int m;

    numModes = getInputNumModes(nucfg, input);
    for (m = 0; m < numModes; m++)
    {
        section = NULL;
        getInputCalibN(nucfg, input, m, &section);
        if (section)
        {
            modeId = chmode2Index(section->SensorOperatingMode);
            if (modeId >= 0)
                SET_ADD(&(nucfg->streamTbl[ch].supportedCalibratedModes), modeId);
            else
                LOGG_PRINT(LOG_WARN_E, NULL, "nucfg: calib. unknown op mode for section %d - ignored\n", section->sectionNum);
        }
    }
}
/*
 For each channel in the configuration set its supported modes
 according to calibration table
*/
static void assignCalibratedChannelsModes(nuCfgT *nucfg)
{
    unsigned int ch, i, j;
    chStreamT *stp;
    ERRG_codeE ret;

    assert(NUCFG_MAX_CHAN_MODES <= NUCFG_SET_MAX_ELEMENTS); //sanity check

    for (ch = 0; ch < NUCFG_MAX_CHANNELS; ch++)
    {
        SET_CLEAR(&(nucfg->streamTbl[ch].supportedCalibratedModes));

        stp = &nucfg->streamTbl[ch];
        if ((iauBypass(nucfg, ch))  && (ch < NUCFG_ISP_CHANNELS_START_ID))//we have to use calibration data for ISP channels. for example, to take slu crop sizes.
            continue;
        for (i = 0; i < stp->numInputs; i++)
        {
            if (stp->inputs[i].type == NUCFG_BLK_SEN_E)
            {
                markCalibrationMode(nucfg, &stp->inputs[i], ch);
            }
            else if ((stp->inputs[i].type == NUCFG_BLK_AXIWR_E) && ((nucfg->nuCfgDbP->numWritersWithCalibData) || (nucfg->nuCfgDbP->numWritersWithCalibDataToIsp)))
            {
                linkWriterSensors *mySensorList;
                ret = getLinkToSensorList(nucfg, stp->inputs[i].inst, &mySensorList);
                if (ERRG_FAILED(ret))
                {
                    continue;//we dont have all writers data yet, or nobody link between writers and sensors for injection with calibration
                }
                for (j = 0; j < mySensorList->dummyNumInputs; j++)
                {
                    markCalibrationMode(nucfg, &mySensorList->dummyInputs[j], ch);
                }
            }
        }
    }
}

//Match the path's input to the calibration info
static void matchPathToCalib(nuCfgT *nucfg, chStreamT *streamp, unsigned int p, INT32 mode, const CALIB_sectionDataT **out)
{
   chInputT input, myInput;
   nublkH blk;
   unsigned int m, numModes;
   const CALIB_sectionDataT *section = NULL;
   int match = 0;
   ERRG_codeE ret;

   //Get the input to the path
   input.nodeId = streamp->paths[p].idpath[streamp->paths[p].pathLen-1];
   GRAPHG_getVertex(streamp->gh, input.nodeId, &blk);
   nusoc_getBlkTypeInst(nucfg->my_nucfg_orig->socH, blk, &input.type, &input.inst);

   if (input.type == NUCFG_BLK_SEN_E)
   {
       //Try to match the input to a calibration section
       numModes = getInputNumModes(nucfg, &input);
       for (m = 0; m < numModes; m++)
       {
           section = NULL;
           getInputCalibN(nucfg, &input, m, &section);
           if ((section) && (section->SensorOperatingMode == (CALIB_sensorOperatingModeE)mode))
           {
               match = 1;//section exists
               break;
           }
       }
       if (match)
           *out = section;
   }
   else if (input.type == NUCFG_BLK_AXIWR_E)
   {
       linkWriterSensors *mySensorList;
       ret = getLinkToSensorList(nucfg, input.inst, &mySensorList);
       if (ERRG_FAILED(ret))
       {
           return;//we dont have all writers data yet
       }
       myInput.type = NUCFG_BLK_SEN_E;
       if (mySensorList->dummyNumInputs > 2)// in writer case, we have 2 paths with 1 input-writer.
           //in 'linkWriterSensors' we have more information about 2 source sensors of the images that will inject on path
           //we will enter here twice (2 paths) and we need to jump in the second time to the next sensor input on 'linkWriterSensors.dummyInputs'
       {
           LOGG_PRINT(LOG_ERROR_E, NULL, "nucfg support on path only 2 input\n", section->sectionNum);
           assert(0);
       }
       if (mySensorList->dummyNumInputs > 1)//in case of 1 write buffer with 2 src sensor
          myInput.inst = mySensorList->dummyInputs[p].inst;
       else
           myInput.inst = mySensorList->dummyInputs[0].inst;

       numModes = getInputNumModes(nucfg, &myInput);
       for (m = 0; m < numModes; m++)
       {
           section = NULL;
           getInputCalibN(nucfg, &myInput, m, &section);
           if ((section) && (section->SensorOperatingMode == (CALIB_sensorOperatingModeE)mode))
           {
               match = 1;//section exists
               break;
           }
       }
       if (match)
       {
           *out = section;
           //save this date for fill in metadata in writers section
           mySensorList->verticalCropOffset[0] = section->verticalCropOffset;
           mySensorList->totalVerticalSize[0] = section->sluOutputVerticalSize;
           mySensorList->horizontalCropOffset[0] = section->horizontalCropOffset;
           mySensorList->totalHorizontalSize[0] = section->sluOutputHorizontalSize;
           mySensorList->sensorMode[0] = section->SensorOperatingMode;
           mySensorList->sensorRole[0] = section->SensorRole;
       }
   }
}

static void convertCalInfo(const CALIB_sectionDataT *section, CALIB_ymlDataT *ymlDataP,nusoc_updateSensT *sens, nusoc_updateSluT *slu, nusoc_updateIauT *iau,nusoc_updateDispT *dispToDpth, nusoc_updateWriterT *wrt)
{
   wrt->horizontalSize = section->sluOutputHorizontalSize;
   wrt->horizontalSizeValid = 1;
   wrt->verticalSize = section->sluOutputVerticalSize;
   wrt->verticalSizeValid = 1;
   sens->sensorOperatingMode = section->SensorOperatingMode;
   sens->sensorOperatingModeValid = 1;
   sens->sensorVerticalOffset = section->sensorVerticalOffset;
   sens->sensorVerticalOffsetValid = 1;
   sens->sensorGainOffset = section->sensorGainOffset;
   sens->sensorGainOffsetValid = 1;
   slu->verticalCropOffset = section->verticalCropOffset;
   slu->verticalCropOffsetValid = 1;
   slu->horizontalCropOffset = section->horizontalCropOffset;
   slu->horizontalCropOffsetValid = 1;
   slu->sluOutputVerticalSize = section->sluOutputVerticalSize;
   slu->sluOutputVerticalSizeValid = 1;
   slu->sluOutputHorizontalSize = section->sluOutputHorizontalSize;
   slu->sluOutputHorizontalSizeValid = 1;
   iau->ibVerticalLutOffset = section->ibVerticalLutOffset;
   iau->ibVerticalLutOffsetValid = 1;
   iau->ibHorizontalLutOffset = section->ibHorizontalLutOffset;
   iau->ibHorizontalLutOffsetValid = 1;
   iau->ibOffset = section->ibOffset;
   iau->ibOffsetValid = 1;
   iau->ibOffsetR = section->ibOffsetR;
   iau->ibOffsetRValid = 1;
   iau->ibOffsetGR = section->ibOffsetGR;
   iau->ibOffsetGRValid = 1;
   iau->ibOffsetGB = section->ibOffsetGB;
   iau->ibOffsetGBValid = 1;
   iau->ibOffsetB = section->ibOffsetB;
   iau->ibOffsetBValid = 1;
   iau->dsrVerticalLutOffset = section->dsrVerticalLutOffset;
   iau->dsrVerticalLutOffsetValid = 1;
   iau->dsrHorizontalLutOffset = section->dsrHorizontalLutOffset;
   iau->dsrHorizontalLutOffsetValid = 1;
   iau->dsrOutputVerticalSize = section->dsrOutputVerticalSize;
   iau->dsrOutputVerticalSizeValid = 1;
   iau->dsrOutputHorizontalSize = section->dsrOutputHorizontalSize;
   iau->dsrOutputHorizontalSizeValid = 1;

   //those values are optional on the calibration.ini file. If any of the parameters is missing, the original XML value should be used.
   iau->dsrVerticalBlockSizeValid = 0;
   iau->dsrHorizontalBlockSizeValid = 0;
   iau->dsrLutHorizontalSizeValid = 0;
   iau->dsrHorizontalDeltaSizeValid = 0;
   iau->dsrHorizontalDeltaFractionalBitsValid = 0;
   iau->dsrVerticalDeltaSizeValid = 0;
   iau->dsrVerticalDeltaFractionalBitsValid = 0;
   iau->ibVerticalBlockSizeValid = 0;
   iau->ibHorizontalBlockSizeValid = 0;
   iau->ibLutHorizontalSizeValid = 0;
   iau->ibFixedPointFractionalBitsValid = 0;

   if (section->dsrVerticalBlockSize != INU_DEFSG_INVALID)
   {
      iau->dsrVerticalBlockSize = section->dsrVerticalBlockSize;
      iau->dsrVerticalBlockSizeValid = 1;
   }
   if (section->dsrHorizontalBlockSize != INU_DEFSG_INVALID)
   {
      iau->dsrHorizontalBlockSize = section->dsrHorizontalBlockSize;
      iau->dsrHorizontalBlockSizeValid = 1;
   }
   if (section->dsrLutHorizontalSize != INU_DEFSG_INVALID)
   {
      iau->dsrLutHorizontalSize = section->dsrLutHorizontalSize;
      iau->dsrLutHorizontalSizeValid = 1;
   }
   if (section->dsrHorizontalDeltaSize != INU_DEFSG_INVALID)
   {
      iau->dsrHorizontalDeltaSize = section->dsrHorizontalDeltaSize;
      iau->dsrHorizontalDeltaSizeValid = 1;
   }
   if (section->dsrHorizontalDeltaFractionalBits != INU_DEFSG_INVALID)
   {
      iau->dsrHorizontalDeltaFractionalBits = section->dsrHorizontalDeltaFractionalBits;
      iau->dsrHorizontalDeltaFractionalBitsValid = 1;
   }
   if (section->dsrVerticalDeltaSize != INU_DEFSG_INVALID)
   {
      iau->dsrVerticalDeltaSize = section->dsrVerticalDeltaSize;
      iau->dsrVerticalDeltaSizeValid = 1;
   }
   if (section->dsrVerticalDeltaFractionalBits != INU_DEFSG_INVALID)
   {
      iau->dsrVerticalDeltaFractionalBits = section->dsrVerticalDeltaFractionalBits;
      iau->dsrVerticalDeltaFractionalBitsValid = 1;
   }
   if (section->ibVerticalBlockSize != INU_DEFSG_INVALID)
   {
      iau->ibVerticalBlockSize = section->ibVerticalBlockSize;
      iau->ibVerticalBlockSizeValid = 1;
   }
   if (section->ibHorizontalBlockSize != INU_DEFSG_INVALID)
   {
      iau->ibHorizontalBlockSize = section->ibHorizontalBlockSize;
      iau->ibHorizontalBlockSizeValid = 1;
   }
   if (section->ibLutHorizontalSize != INU_DEFSG_INVALID)
   {
      iau->ibLutHorizontalSize = section->ibLutHorizontalSize;
      iau->ibLutHorizontalSizeValid = 1;
   }
   if (section->ibFixedPointFractionalBits != INU_DEFSG_INVALID)
   {
      iau->ibFixedPointFractionalBits = section->ibFixedPointFractionalBits;
      iau->ibFixedPointFractionalBitsValid = 1;
   }
   dispToDpth->d2dFactor = ymlDataP->d2dFactor;
   dispToDpth->fc = ymlDataP->fc;
}

static void initSensorMode(NUCFG_sensorRawParams *entryP, CALIB_sensorOperatingModeE mode, unsigned int width, unsigned int height)
{
   SET_ADD(&entryP->supportedModes, mode);
   entryP->modesParams[mode].height = height;
   entryP->modesParams[mode].width = width;
}

static void initSensorsRawParamsTbl()
{
   //OV7251
   sensorsRawParamsTbl[NUCFG_OV7251_SENSOR_E].model = 7251;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV7251_SENSOR_E], CALIB_MODE_BIN_E, 320, 240);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV7251_SENSOR_E], CALIB_MODE_FULL_E, 640, 480);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV7251_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 640, 240);

   //OV9282
   sensorsRawParamsTbl[NUCFG_OV9282_SENSOR_E].model = 9282;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV9282_SENSOR_E], CALIB_MODE_BIN_E, 640, 400);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV9282_SENSOR_E], CALIB_MODE_FULL_E, 1280, 800);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV9282_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 1280, 400);

   //AR2020
   sensorsRawParamsTbl[NUCFG_AR2020_SENSOR_E].model = 2020;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR2020_SENSOR_E], CALIB_MODE_BIN_E, 1120, 832);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR2020_SENSOR_E], CALIB_MODE_FULL_E, 1920, 2700);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR2020_SENSOR_E], CALIB_MODE_UXGA_E, 800, 736);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR2020_SENSOR_E], CALIB_MODE_USER_DEFINE_E, 608, 448);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR2020_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 832, 608);

   //AR2021 -virtual sensor , dual resolution for sensor 2020
   sensorsRawParamsTbl[NUCFG_AR2021_SENSOR_E].model = 2021;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR2021_SENSOR_E], CALIB_MODE_BIN_E, 544, 384);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR2021_SENSOR_E], CALIB_MODE_FULL_E, 1920, 2700);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR2021_SENSOR_E], CALIB_MODE_UXGA_E, 672, 608);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR2021_SENSOR_E], CALIB_MODE_USER_DEFINE_E, 608, 448);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR2021_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 832, 608);

   //OV5675
   sensorsRawParamsTbl[NUCFG_OV5675_SENSOR_E].model = 5675;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV5675_SENSOR_E], CALIB_MODE_BIN_E, 1920,1080 );
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV5675_SENSOR_E], CALIB_MODE_FULL_E, 1920 ,1080 );
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV5675_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E,1920 ,1080 );

   //OV8856
   sensorsRawParamsTbl[NUCFG_OV8856_SENSOR_E].model = 8856;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV8856_SENSOR_E], CALIB_MODE_BIN_E, 1632 , 1224 );
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV8856_SENSOR_E], CALIB_MODE_FULL_E, 3264 , 2448);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV8856_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E,1920, 1080 );
   //OV4689
   sensorsRawParamsTbl[NUCFG_OV4689_SENSOR_E].model = 4689;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV4689_SENSOR_E], CALIB_MODE_BIN_E, 1344, 760);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV4689_SENSOR_E], CALIB_MODE_FULL_E, 1520, 1520);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV4689_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 1520, 1520);
   //CGS132
   sensorsRawParamsTbl[NUCFG_CGS132_SENSOR_E].model = 132;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_CGS132_SENSOR_E], CALIB_MODE_BIN_E, 544, 640);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_CGS132_SENSOR_E], CALIB_MODE_FULL_E, 1080, 1280);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_CGS132_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 1080, 1280);

   //GC2145
   sensorsRawParamsTbl[NUCFG_GC2145_SENSOR_E].model = 2145;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GC2145_SENSOR_E], CALIB_MODE_BIN_E, 800, 600);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GC2145_SENSOR_E], CALIB_MODE_FULL_E, 1600, 1200);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GC2145_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 1600, 1200);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GC2145_SENSOR_E], CALIB_MODE_USER_DEFINE_E, 1600, 900);

    //AR430
   sensorsRawParamsTbl[NUCFG_AR430_SENSOR_E].model = 430;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR430_SENSOR_E], CALIB_MODE_BIN_E, 1600, 1600);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR430_SENSOR_E], CALIB_MODE_FULL_E, 1600, 1600);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR430_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 1600, 1600);

  //OV2685
   sensorsRawParamsTbl[NUCFG_OV2685_SENSOR_E].model = 2685;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV2685_SENSOR_E], CALIB_MODE_BIN_E, 800, 600);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV2685_SENSOR_E], CALIB_MODE_FULL_E, 1600, 1200);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV2685_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 800, 600);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV2685_SENSOR_E], CALIB_MODE_UXGA_E, 1600, 1200);
   //XC9160
   sensorsRawParamsTbl[NUCFG_XC9160_SENSOR_E].model = 9160;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_XC9160_SENSOR_E], CALIB_MODE_BIN_E, 640, 480);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_XC9160_SENSOR_E], CALIB_MODE_FULL_E, 1280, 960);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_XC9160_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 800, 600);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_XC9160_SENSOR_E], CALIB_MODE_UXGA_E, 1600, 1200);

   //AMS CGSS130
   sensorsRawParamsTbl[NUCFG_AMS_CGSS130_SENSOR_E].model = 130;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AMS_CGSS130_SENSOR_E], CALIB_MODE_BIN_E, 1080, 800);//ELAD TODO sizes
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AMS_CGSS130_SENSOR_E], CALIB_MODE_FULL_E, 1080, 1280);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AMS_CGSS130_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 1280, 800);

   //AR0234
   sensorsRawParamsTbl[NUCFG_AR0234_SENSOR_E].model = 234;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR0234_SENSOR_E], CALIB_MODE_BIN_E, 1280, 800);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_AR0234_SENSOR_E], CALIB_MODE_FULL_E, 1920, 1200);

   //OV9782
   sensorsRawParamsTbl[NUCFG_OV9782_SENSOR_E].model = 9782;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV9782_SENSOR_E], CALIB_MODE_BIN_E, 640, 400);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV9782_SENSOR_E], CALIB_MODE_FULL_E, 1280, 800);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OV9782_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 1280, 800);

   //CGS031
   sensorsRawParamsTbl[NUCFG_CGS031_SENSOR_E].model = 031;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_CGS031_SENSOR_E], CALIB_MODE_BIN_E, 640, 480);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_CGS031_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 1280, 400);

   //VD56G3
   sensorsRawParamsTbl[NUCFG_VD56G3_SENSOR_E].model = 563;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_VD56G3_SENSOR_E], CALIB_MODE_BIN_E, 640, 480);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_VD56G3_SENSOR_E], CALIB_MODE_FULL_E, 800, 800);

   //VD55G0
   sensorsRawParamsTbl[NUCFG_VD55G0_SENSOR_E].model = 550;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_VD55G0_SENSOR_E], CALIB_MODE_BIN_E, 640, 480);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_VD55G0_SENSOR_E], CALIB_MODE_FULL_E, 640, 480);

   //OS05A10
   sensorsRawParamsTbl[NUCFG_OS05A10_SENSOR_E].model = 550;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OS05A10_SENSOR_E], CALIB_MODE_BIN_E, 640, 480);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OS05A10_SENSOR_E], CALIB_MODE_FULL_E, 1280, 960);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_OS05A10_SENSOR_E], CALIB_MODE_USER_DEFINE_E, 2560, 1920);

   //GC2053
   // TODO: Change to real resolutions
   sensorsRawParamsTbl[NUCFG_GC2053_SENSOR_E].model = 2053;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GC2053_SENSOR_E], CALIB_MODE_BIN_E, 800, 600);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GC2053_SENSOR_E], CALIB_MODE_FULL_E, 1600, 1200);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GC2053_SENSOR_E], CALIB_MODE_VERTICAL_BINNING_E, 1600, 1200);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GC2053_SENSOR_E], CALIB_MODE_USER_DEFINE_E, 1600, 900);

   //Gen Model
   sensorsRawParamsTbl[NUCFG_GENERIC_MODEL_E].model = 9998;
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GENERIC_MODEL_E], CALIB_MODE_BIN_E, 640, 480);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GENERIC_MODEL_E], CALIB_MODE_FULL_E, 1280, 960);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GENERIC_MODEL_E], CALIB_MODE_VERTICAL_BINNING_E, 800, 600);
   initSensorMode(&sensorsRawParamsTbl[NUCFG_GENERIC_MODEL_E], CALIB_MODE_UXGA_E, 1600, 1200);
}

static NUCFG_supprtedSensors sensorModelToIndex(unsigned int sensorModel)
{
   if (sensorModel == 130)
      return NUCFG_AMS_CGSS130_SENSOR_E;
   else if (sensorModel == 430)
      return NUCFG_AR430_SENSOR_E;
   if (sensorModel == 7251)
      return NUCFG_OV7251_SENSOR_E;
   else if (sensorModel == 9282)
      return NUCFG_OV9282_SENSOR_E;
   else if (sensorModel == 5675)
      return NUCFG_OV5675_SENSOR_E;
   else if (sensorModel == 8856)
      return NUCFG_OV8856_SENSOR_E;
   else if (sensorModel == 4689)
      return NUCFG_OV4689_SENSOR_E;
   else if (sensorModel == 132)
      return NUCFG_CGS132_SENSOR_E;
   else if (sensorModel == 234)
      return NUCFG_AR0234_SENSOR_E;
   else if (sensorModel == 2020)
      return NUCFG_AR2020_SENSOR_E;
   else if (sensorModel == 2021)
      return NUCFG_AR2021_SENSOR_E;
   else if (sensorModel == 2685)
      return NUCFG_OV2685_SENSOR_E;
   else if (sensorModel == 2145)
      return NUCFG_GC2145_SENSOR_E;
   else if (sensorModel == 2053)
      return NUCFG_GC2053_SENSOR_E;
   else if (sensorModel == 9160)
       return NUCFG_XC9160_SENSOR_E;
   else if (sensorModel == 9782)
       return NUCFG_OV9782_SENSOR_E;
   else if (sensorModel == 031)
       return NUCFG_CGS031_SENSOR_E;
   else if (sensorModel == 563)
       return NUCFG_VD56G3_SENSOR_E;
   else if (sensorModel == 550)
       return NUCFG_VD55G0_SENSOR_E;
   else if (sensorModel == 510)
       return NUCFG_OS05A10_SENSOR_E;
   else if (sensorModel == 9998)
       return NUCFG_GENERIC_MODEL_E;
         
   assert(0);
   return NUCFG_OV7251_SENSOR_E;
}

/*
For each channel in the configuration set its supported modes
according to the sensor type
*/
static NUCFG_setT getSensorNumModes(nuCfgT *nucfg, unsigned int inst)
{
   NUCFG_inputT sensorParams;
   sensorParams.type = NUCFG_INPUT_TYPE_SENSOR_E_E;
   sensorParams.inst = inst;

   getInputParams(nucfg, &sensorParams);
   return sensorsRawParamsTbl[sensorModelToIndex(sensorParams.params.sensorParams.model)].supportedModes;
}

/*
For each channel in the configuration set its supported modes
according to the sensor type
*/
static void assignSensorsChannelsModes(nuCfgT *nucfg)
{
   unsigned int ch, i;
   chStreamT *stp;
   NUCFG_setT sensorModes;

   assert(NUCFG_MAX_CHAN_MODES <= NUCFG_SET_MAX_ELEMENTS); //sanity check

   for (ch = 0; ch < NUCFG_MAX_CHANNELS; ch++)
   {
      SET_CLEAR(&(nucfg->streamTbl[ch].supportedSensorModes));

      stp = &nucfg->streamTbl[ch];
      for (i = 0; i < stp->numInputs; i++)
      {
         if (stp->inputs[i].type == NUCFG_BLK_SEN_E)
         {
            //sanity check - go over all sensors for this channel, and check all has the same modes
            sensorModes = getSensorNumModes(nucfg, stp->inputs[i].inst);
            nucfg->streamTbl[ch].supportedSensorModes = SET_UNION(&(nucfg->streamTbl[ch].supportedSensorModes), &sensorModes);
         }
      }
   }
}

static void updatePath(nuCfgT *nucfg, chStreamT *streamp, unsigned int p, socPathUpdateT *upd,unsigned int updateYml)
{
   int outsel;
   int j;
   nublkH blk, prevblk = NULL;
   nusoc_updateT *update = NULL;
   nuBlkTypeE type;
   unsigned int inst;
   hwPathT *path = &streamp->paths[p];

   if(path->pathLen == 0)
      return; //ignore empty path

   //update the path going from from source to output
   for(j = (int)(path->pathLen-1); j >= 0; j--)
   {
      if(j == (int)(path->pathLen-1))
      {
         GRAPHG_getVertex(streamp->gh, path->idpath[j], &prevblk);
         continue;
      }
      //get update parameters for previous block from calibration info
      nusoc_getBlkTypeInst(nucfg->socH, prevblk, &type, &inst);
      if(type == NUCFG_BLK_SEN_E)
         update = (upd) ? (nusoc_updateT *)&(upd->sens) : NULL;
      else if (type == NUCFG_BLK_SLU_E)
         update = (upd) ? (nusoc_updateT *)&(upd->slu) : NULL;
      else if (type == NUCFG_BLK_SLU_PARALLEL_E)
          update = (upd) ? (nusoc_updateT *)&(upd->slu) : NULL;
      else if (type == NUCFG_BLK_IAU_E || type == NUCFG_BLK_IAU_COLOR_E)
         update = (upd) ? (nusoc_updateT *)&(upd->iau) : NULL;
      else if (type == NUCFG_BLK_AXIWR_E)
         update = (upd) ? (nusoc_updateT *)&(upd->writer) : NULL;
      else if ((type == NUCFG_BLK_DPP_E) && (updateYml == 1))
         update = (upd) ? (nusoc_updateT *)&(upd->disToDepth) : NULL;
      else
         update = NULL;

      GRAPHG_getVertex(streamp->gh, path->idpath[j], &blk);//current block
      //update the previous blk on the path to current block
      outsel = nusoc_getOutSelect(nucfg->socH, prevblk, blk);
      assert(outsel >= 0);
      nusoc_updateBlkOutput(nucfg->socH, prevblk, outsel, update);

      prevblk = blk;
   }
}

static BOOLEAN updateYmldata(nuCfgT *nucfg,unsigned int ch,unsigned int mode,CALIB_ymlDataT *ymlDataP)
{
   CALIB_sensorLutModeE lutMode;
   float fy;
   UINT32 inst0 = 0, inst1 = 0;

   lutMode = (CALIB_sensorLutModeE)chLutModeInd(mode);
   ymlDataP->fc =0;
   ymlDataP->d2dFactor=0;
   if(nucfg->streamTbl[ch].numInputs > 1)
   {
      if (nucfg->streamTbl[ch].inputs[0].type == NUCFG_BLK_SEN_E)
      {
         inst0 = nucfg->streamTbl[ch].inputs[0].inst;
         inst1 = nucfg->streamTbl[ch].inputs[1].inst;
      }
      else if (nucfg->streamTbl[ch].inputs[0].type == NUCFG_BLK_AXIWR_E)
      {
         if (nucfg->nuCfgDbP->numWritersWithCalibData > 1)//we supply in cases: 1 writebuf with 2 linked sensors (split), or 2 writebuf with 1 linked sensprs (without split)
         {
            inst0 = nucfg->nuCfgDbP->linkWriterSensors[0].dummyInputs[0].inst;
            inst1 = nucfg->nuCfgDbP->linkWriterSensors[1].dummyInputs[0].inst;
         }
         else if (nucfg->nuCfgDbP->numWritersWithCalibData == 1)////we supply in cases: 1 writebuf with 2 linked sensors (split), or 2 writebuf with 1 linked sensprs (without split)
         {
            inst0 = nucfg->nuCfgDbP->linkWriterSensors[0].dummyInputs[0].inst;
            inst1 = nucfg->nuCfgDbP->linkWriterSensors[0].dummyInputs[1].inst;
         }
      }
      if (CALIBG_ymlGetSectionsBLValue((CALIB_sensorLutModeE)lutMode, inst0, inst1, &ymlDataP->d2dFactor))
         if (CALIBG_ymlGetSectionFcValue(lutMode,(CALIB_ymlSectionE)inst0, &ymlDataP->fc, &fy))
            return TRUE;
   }
   return FALSE;
}

static void setPathModeFromCalib(nuCfgT *nucfg, unsigned int ch, unsigned int p, const CALIB_sectionDataT *calSection,unsigned int mode)
{
   socPathUpdateT upd;
   CALIB_ymlDataT ymlData;
   chStreamT *streamp;

   streamp = &nucfg->streamTbl[ch];
   if((streamp->numInputs == 0) || (streamp->inputs[0].type != NUCFG_BLK_SEN_E))
       if ((streamp->inputs[0].type != NUCFG_BLK_AXIWR_E) && (nucfg->nuCfgDbP->numWritersWithCalibData == 0))
          return; //support sensor input paths only, or writer with link to src sensors

   updateYmldata(nucfg,ch,mode,&ymlData);

   convertCalInfo(calSection, &ymlData,&upd.sens, &upd.slu, &upd.iau,&upd.disToDepth, &upd.writer);
   updatePath(nucfg, streamp, p, &upd,1);
}

static void setPathModeFromSupportedSensorsTable(nuCfgT *nucfg, chStreamT *streamp, unsigned int p, INT32 mode, const NUCFG_sensorModeParam *sensorModeParams)
{
   socPathUpdateT upd;

   if ((streamp->numInputs == 0) || (streamp->inputs[0].type != NUCFG_BLK_SEN_E))
      return; //support sensor input paths only

   memset(&upd, 0, sizeof(upd));
   upd.sens.sensorOperatingMode = mode;
   upd.sens.sensorOperatingModeValid = 1;
   upd.slu.sluOutputHorizontalSize = sensorModeParams->width;
   upd.slu.sluOutputHorizontalSizeValid = 1;
   upd.slu.sluOutputVerticalSize = sensorModeParams->height;
   upd.slu.sluOutputVerticalSizeValid = 1;
   upd.iau.dsrOutputHorizontalSizeValid = 1;
   upd.iau.dsrOutputHorizontalSize = sensorModeParams->width;
   upd.iau.dsrOutputVerticalSizeValid = 1;
   upd.iau.dsrOutputVerticalSize = sensorModeParams->height;
   //iau params are not updated, assumption is if there is no calibration, then data does not pass through iau's
   updatePath(nucfg, streamp, p, &upd,0);
}

//Match the path's input to the sensor's mode info
static void matchPathToSensorParams(nuCfgT *nucfg, chStreamT *streamp, unsigned int p, INT32 mode, const NUCFG_sensorModeParam **out)
{
   chInputT input;
   nublkH blk;
   NUCFG_inputT sensor;

   //Get the input to the path
   input.nodeId = streamp->paths[p].idpath[streamp->paths[p].pathLen - 1];
   GRAPHG_getVertex(streamp->gh, input.nodeId, &blk);
   nusoc_getBlkTypeInst(nucfg->my_nucfg_orig->socH, blk, &input.type, &input.inst);
   sensor.type = NUCFG_INPUT_TYPE_SENSOR_E_E;
   sensor.inst = input.inst;
   getInputParams(nucfg, &sensor);
   *out = &sensorsRawParamsTbl[sensorModelToIndex(sensor.params.sensorParams.model)].modesParams[mode];
}

static void setGroupMode(nuCfgT *nucfg, unsigned int group, INT32 mode)
{
   unsigned int ch, i;
   chStreamT *streamp;
   const CALIB_sectionDataT *calSection = NULL;
   const NUCFG_sensorModeParam *sensorModeParams = NULL;

   //For each channel in the group, and each path in the channel, update the path according to mode.
   for(ch = 0; ch < NUCFG_MAX_CHANNELS; ch++)
   {
      if(!SET_HAS(&(nucfg->groupTbl[group].channels),ch))
         continue; //channel not in the group

      streamp = &nucfg->streamTbl[ch];

   //generate sizes for all path without calibration data
   /*for (i = 0; i < streamp->numPaths; i++)
   {
      socPathUpdateT upd;
      memset(&upd, 0, sizeof(upd));
      updatePath(nucfg, streamp, i, &upd,0);
   }*/

      //first we check if mode exists in calibration data
      if (SET_HAS(&streamp->supportedCalibratedModes, chmode2Index(mode)) && ((!iauBypass(nucfg, ch)) || (ch >= NUCFG_ISP_CHANNELS_START_ID)))
      {
         nucfg->groupTbl[group].currMode = mode;

         //if no calibration mode exists for the channel in the calibration table this will do nothing
         for (i = 0; i < streamp->numPaths; i++)
         {
            //try to match the path to calibration section and update the path mode accordingly
            calSection = NULL;
            matchPathToCalib(nucfg, streamp, i, mode, &calSection);
            if (calSection)
               setPathModeFromCalib(nucfg, ch, i, calSection, mode);
         }
      }
      //if mode is not supported in calibration data, check if supported sensors table has this mode resolution
      else if (SET_HAS(&streamp->supportedSensorModes, chmode2Index(mode)))
      {
         nucfg->groupTbl[group].currMode = mode;
         for (i = 0; i < streamp->numPaths; i++)
         {
            sensorModeParams = NULL;
            matchPathToSensorParams(nucfg, streamp, i, mode, &sensorModeParams);
            setPathModeFromSupportedSensorsTable(nucfg, streamp, i, mode, sensorModeParams);
         }
      }
      //inject or anything else
      else
      {
            for (i = 0; i < streamp->numPaths; i++)
            {
               socPathUpdateT upd;
               memset(&upd, 0, sizeof(upd));
               updatePath(nucfg, streamp, i, NULL, 0);
            }
      }
   }
}

/*
static INT32 getChDefaultMode(nuCfgT *nucfg, unsigned int inputInst)
{
   INT32 chMode;
   UINT32 temp;
   XMLDB_getValue(nucfg->xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, inputInst, SENSORS_SENS_0_OP_MODE_E), &temp);

   if (temp == INU_DEFSG_RES_VGA_E)
      chMode = CALIB_MODE_BIN_E;
   else if (temp == INU_DEFSG_RES_HD_E)
      chMode = CALIB_MODE_FULL_E;
   else if (temp == INU_DEFSG_RES_VERTICAL_BINNING_E)
      chMode = CALIB_MODE_VERTICAL_BINNING_E;
   else if (temp == INU_DEFSG_RES_FULL_HD_E)
      chMode = CALIB_MODE_UXGA_E;
   else
      chMode = CALIB_MODE_UNKNOWN;

   return chMode;
}
*/
static void setDefaultGroupMode(nuCfgT *nucfg)
{
   unsigned int group, j, i, z;
   NUCFG_setT chSet;
   //INT32 groupMode = 0xFF;
   INT32 groupMode, chMode;
   chStreamT *streamp;

   for (group = 0; group < nucfg->numGroups; group++)
   {
      chSet = nucfg->groupTbl[group].channels;
      groupMode = 0xFF;

      while (!SET_ISEMPTY(&chSet))
      {
         //1. get from xml next ch mode
         j = SET_REM_NEXT(&chSet);

         //2. check if all the inputs of it are the same
         for (i = 0; i < nucfg->streamTbl[j].numInputs; i++)
         {
            chMode = CALIB_MODE_FULL_E;//getChDefaultMode(nucfg, nucfg->streamTbl[j].inputs[i].inst);
            if (groupMode == 0xFF)
               groupMode = chMode;
            else
               if (groupMode != chMode)
                  assert(0);
         }
      }

      //3. update this group mode
      if (groupMode != 0xFF)
         setGroupMode(nucfg /*nucfg_mod*/, group, groupMode);

     //4. set injection default resolution (in case of injection without calicration data)
      if (nucfg->nuCfgDbP->numWritersWithCalibData == 0)
      {
          for (j = 0; j < NUCFG_MAX_CHANNELS; j++)
          {
              streamp = &nucfg->my_nucfg_mod->streamTbl[j];
              for (z = 0; z < streamp->numInputs; z++)
              {
                  if (streamp->inputs[z].type == NUCFG_BLK_AXIWR_E)
                  {
                      //For each path in the channel, set default resolution.
                      for (i = 0; i < streamp->numPaths; i++)
                      {
                          setPathResolution(nucfg->my_nucfg_mod, streamp, i, 640, 480);
                      }
                  }
              }
          }
      }
   }
}

static void initUpdateInfo(unsigned int width, unsigned int height, nusoc_updateSensT *sens, nusoc_updateSluT *slu, nusoc_updateIauT *iau, nusoc_updateWriterT *writer)
{
   //guestimate the operating mode
   sens->sensorOperatingMode = (width > (NUCFG_VGA_WIDTH) + (NUCFG_VGA_WIDTH>>1)) ? CALIB_MODE_FULL_E : CALIB_MODE_BIN_E;
   sens->sensorOperatingModeValid = 1;
   //no cropping in slu
   slu->verticalCropOffset = 0;
   slu->verticalCropOffsetValid = 1;
   slu->horizontalCropOffset = 0;
   slu->horizontalCropOffsetValid = 1;
   //full output size for sly
   slu->sluOutputVerticalSize = height;
   slu->sluOutputVerticalSizeValid = 1;
   slu->sluOutputHorizontalSize = width;
   slu->sluOutputHorizontalSizeValid = 1;
   //set the dsr output size - not needed if dsr not active
   iau->dsrOutputVerticalSize = height;
   iau->dsrOutputVerticalSizeValid = 1;
   iau->dsrOutputHorizontalSize = width;
   iau->dsrOutputHorizontalSizeValid = 1;

   //Invalid values will not be modified in xmldb
   sens->sensorVerticalOffsetValid = 0;
   sens->sensorGainOffsetValid = 0;
   iau->ibVerticalLutOffsetValid = 0;
   iau->ibHorizontalLutOffsetValid = 0;
   iau->ibOffsetValid = 0;
   iau->ibOffsetRValid = 0;
   iau->ibOffsetGRValid = 0;
   iau->ibOffsetGBValid = 0;
   iau->ibOffsetBValid = 0;
   iau->dsrVerticalLutOffsetValid = 0;
   iau->dsrHorizontalLutOffsetValid = 0;
   iau->dsrVerticalBlockSizeValid = 0;
   iau->dsrHorizontalBlockSizeValid = 0;
   iau->dsrLutHorizontalSizeValid = 0;
   iau->dsrHorizontalDeltaSizeValid = 0;
   iau->dsrHorizontalDeltaFractionalBitsValid = 0;
   iau->dsrVerticalDeltaSizeValid = 0;
   iau->dsrVerticalDeltaFractionalBitsValid = 0;
   iau->ibVerticalBlockSizeValid = 0;
   iau->ibHorizontalBlockSizeValid = 0;
   iau->ibLutHorizontalSizeValid = 0;
   iau->ibFixedPointFractionalBitsValid = 0;

   writer->horizontalSizeValid = 1;
   writer->horizontalSize = width;
   writer->verticalSizeValid = 1;
   writer->verticalSize = height;
}

/*
   We want to zero all valid flags, to avoid update this blocks sizes (iau, slu and sensors).
*/
static void resetUpdateInfo(socPathUpdateT *upd)
{
   memset(upd, 0, sizeof(socPathUpdateT));
}

/*
   Set a path resolution based on desired output width/height.
*/
static void setPathResolution(nuCfgT *nucfg, chStreamT *streamp, unsigned int p, unsigned int width, unsigned int height)
{
   socPathUpdateT upd;

   if(streamp->numInputs == 0)
      return;

   initUpdateInfo(width,height,&upd.sens, &upd.slu, &upd.iau, &upd.writer);
   updatePath(nucfg, streamp, p, &upd,0);
}

/*
- create soc handle -> graphs -> paths based on the xml.
*/
static ERRG_codeE nucfgSeq(nuCfgT *nucfg)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;

   ret = nusoc_init(nucfg->xmldbH, &nucfg->socH);
   if(ERRG_SUCCEEDED(ret))
   {
      ret = createGraphs(nucfg);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      generatePaths(nucfg);
      collectInputs(nucfg);
      assignGroups(nucfg);
      assignSensorsChannelsModes(nucfg);
      assignCalibratedChannelsModes(nucfg);
      setDefaultGroupMode(nucfg);
      updateDbMeta(nucfg);
      disableDbMetaChannels(nucfg);
   }

   return ret;
}

/*
 Recreates soc/graphs based on the xml.
 Run update after making changes to the xml that could change the graphs.
*/
static ERRG_codeE nucfgUpdate(nuCfgT *nucfg)
{
   nucfgClean(nucfg);
   return nucfgSeq(nucfg);
}
unsigned int NUCFG_format2Bpp(NUCFG_formatE format)
{
   initFormat2BppTbl();
   return format2Bpp[format];
}
void NUCFG_setBypass(XMLDB_dbH xmldbH)
{
   LOGG_PRINT(LOG_INFO_E,NULL,"*****************************nucfg: calibration directory problem. Start BYPASS MODE ********************\n");
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_IB0_E , 1);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_DSR0_E, 1);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_IB1_E,  1);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_DSR1_E, 1);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_IB2_E , 1);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_DSR2_E, 1);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_IB3_E , 1);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_DSR3_E, 1);
}

void NUCFG_setNormal(XMLDB_dbH xmldbH)
{
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_IB0_E, 0);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_DSR0_E, 0);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_IB1_E, 0);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_DSR1_E, 0);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_IB2_E, 0);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_DSR2_E, 0);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_IB3_E, 0);
   XMLDB_setValue(xmldbH, NU4100_IAE_BYPASS_DSR3_E, 0);
}

void NUCFG_getXmlPathAndName(inu_nucfgH nucfgH, const char **xmlPath, const char **name)
{
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;
   if ((!xmlPath) || (!name))
   {
      assert(0);
   }
   *xmlPath = nucfgP->xmlPath;
   *name = nucfgP->name;
}

BOOLEAN NUCFG_isBypass(void *xmldbH,UINT32 *bypassBitField)
{
   UINT32 ind,bypassVal;
   BOOLEAN retVal=TRUE;
   UINT8 bypassBitMap[]={0,2,3,4,5,6,7,8,
                       10,11,12,13,14,15,16,17,
                       18,22,23,24,25,26,30,31};
   *bypassBitField = 0;
    for(ind=0;ind<sizeof(bypassBitMap);ind++)
    {
      XMLDB_getValue((XMLDB_dbH)xmldbH,(XMLDB_pathE)(NU4100_IAE_BYPASS_YUV0_E + ind) , &bypassVal);
      if (bypassVal != 1)
         retVal=FALSE;
      *bypassBitField |= bypassVal<<bypassBitMap[ind];

    }
    return retVal;
}

static BOOLEAN NUCFG_isIaeBypass(void *xmldbH,UINT32 iaeInd)
{
   UINT32 bypassDsrVal, bypassIbVal;
   BOOLEAN retVal=FALSE;
   UINT32 iauXmlDsrInd[4] = { NU4100_IAE_BYPASS_DSR0_E,NU4100_IAE_BYPASS_DSR1_E, NU4100_IAE_BYPASS_DSR2_E,NU4100_IAE_BYPASS_DSR3_E };
   UINT32 iauXmlIbInd[4] = { NU4100_IAE_BYPASS_IB0_E,NU4100_IAE_BYPASS_IB1_E, NU4100_IAE_BYPASS_IB2_E,NU4100_IAE_BYPASS_IB3_E};
   if (iaeInd < 4)
   {
       XMLDB_getValue((XMLDB_dbH)xmldbH, (XMLDB_pathE)(iauXmlDsrInd[iaeInd]), &bypassDsrVal);
       XMLDB_getValue((XMLDB_dbH)xmldbH,(XMLDB_pathE)(iauXmlIbInd[iaeInd]) , &bypassIbVal);
       if ((bypassDsrVal == 1) && (bypassIbVal == 1))
         retVal= TRUE;
       else retVal=FALSE;

    }
    return retVal;
}

/*
   Parse xml and ini files to get the hw channel info.
   Create xmldb for export.
*/
ERRG_codeE NUCFG_init(NUCFG_calibT *calib, const char *xml, const char *xmlPath, const char *name,inu_nucfgH *nucfg)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;

   nuCfgH *nucfgP = (nuCfgH*)malloc(sizeof(nuCfgH));
   if (!nucfgP)
   {
      return NUCFG__ERR_OUT_OF_MEM;
   }

   memset(nucfgP, 0, sizeof(nuCfgH));
   strncpy(nucfgP->name, name, NUCFG_MAX_CONFIG_NAME);
   nucfgP->xmlPath = strdup(xmlPath);
   nucfgP->nucfg_mod.my_nucfg_orig = &nucfgP->nucfg_orig;
   nucfgP->nucfg_mod.my_nucfg_mod = &nucfgP->nucfg_mod;
   nucfgP->nucfg_orig.my_nucfg_mod = &nucfgP->nucfg_mod;
   nucfgP->nucfg_orig.my_nucfg_orig = &nucfgP->nucfg_orig;
   nucfgP->nucfg_orig.nuCfgDbP = &nucfgP->nuCfgDb;
   nucfgP->nucfg_mod.nuCfgDbP = &nucfgP->nuCfgDb;

   initFormat2BppTbl();
   initSensorsRawParamsTbl();

   if(calib->tbl)
   {
      nucfgP->nucfg_orig.calib.numSections = calib->numSections;
      nucfgP->nucfg_orig.calib.tbl = calib->tbl;
      nucfgP->nucfg_mod.calib.numSections = calib->numSections;
      nucfgP->nucfg_mod.calib.tbl= calib->tbl;
   }

//   calibShow(LOG_INFO_E, nucfg_orig);

   //parse xml,create soc structure, create graphs and update meta data
   ret = XMLDB_open(&nucfgP->nucfg_orig.xmldbH);
   if(ERRG_SUCCEEDED(ret))
   {
      ret = XMLDB_loadFromBuf(nucfgP->nucfg_orig.xmldbH, xml);
     if (calib->tbl == NULL) // bypass when no calibration
     {
        NUCFG_setBypass(nucfgP->nucfg_orig.xmldbH);
     }
     else
     {
        //NUCFG_setNormal(nucfg_orig->xmldbH);
     }
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ret = nucfgSeq(&nucfgP->nucfg_orig);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //repeat for working/modifiable xml
      ret = XMLDB_duplicate(nucfgP->nucfg_orig.xmldbH, &nucfgP->nucfg_mod.xmldbH);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ret = nucfgSeq(&nucfgP->nucfg_mod);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //showGraphs(nucfg_orig);
      *nucfg = (inu_nucfgH)nucfgP;
   }
   else
   {
	// modified to avoid freeing null. david @ 2022.09.24
       if (*nucfg)
        nucfgCleanAll(nucfg);
   }
   return ret;
}

XMLDB_dbH NUCFG_getDbH(inu_nucfgH nucfg)
{
   nuCfgH *nucfgP = (nuCfgH *)nucfg;
   return nucfgP->nucfg_orig.xmldbH;
}

XMLDB_dbH NUCFG_getCurrDbH(inu_nucfgH nucfg)
{
   nuCfgH *nucfgP = (nuCfgH *)nucfg;
   return nucfgP->nucfg_mod.xmldbH;
}

ERRG_codeE NUCFG_revertDb(inu_nucfgH nucfg)
{
   ERRG_codeE ret;
   nuCfgH *nucfgP = (nuCfgH *)nucfg;
   nucfgClean(&nucfgP->nucfg_mod);

   ret = XMLDB_duplicate(nucfgP->nucfg_orig.xmldbH, &nucfgP->nucfg_mod.xmldbH);
   if(ERRG_SUCCEEDED(ret))
      nucfgSeq(&nucfgP->nucfg_mod);
   return ret;
}

void NUCFG_deInit(inu_nucfgH nucfg)
{
   nucfgCleanAll(nucfg);
}

unsigned int NUCFG_getNumInputs(inu_nucfgH nucfg)
{
   nuCfgH *nucfgP = (nuCfgH *)nucfg;
   return nusoc_getNumInputs(nucfgP->nucfg_orig.socH);
}

void NUCFG_getSensorSizes(UINT32 senModel, UINT32 senMode, UINT32 *senWidth, UINT32 *senHeight)
{
    NUCFG_sensorModeParam out;
    CALIB_sensorOperatingModeE mode;

    //tmp: find or implemente convert like index2chmode() or chmode2Index()
    if (senMode == 1)
    {
        mode = CALIB_MODE_BIN_E;
    }
    else if (senMode == 2)
    {
        mode = CALIB_MODE_VERTICAL_BINNING_E;
    }
    else if (senMode == 3)
    {
        mode = CALIB_MODE_FULL_E;
    }
    else
    {
        mode = CALIB_MODE_UNKNOWN;
    }

    out = sensorsRawParamsTbl[sensorModelToIndex(senModel)].modesParams[mode];

    *senWidth = (out).width;
    *senHeight = (out).height;
}

//check if there are senors in the xml, with same trigger source, but different frame rate
static ERRG_codeE validateTriggerSrcFps(inu_nucfgH nucfgH)
{
   unsigned int i, j, triggerSrc1, triggerSrc2;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   nuCfgT *nucfg = &nucfgP->nucfg_mod;
   NUCFG_inputT *input_tbl = (NUCFG_inputT*)malloc(sizeof(NUCFG_inputT) * NUCFG_getNumInputs(nucfgH));

   NUCFG_getInputTable(nucfgH, input_tbl);
   for (i = 0; i < nusoc_getNumInputs(nucfg->socH); i++)
   {
      triggerSrc1 = input_tbl[i].params.sensorParams.trigger_src;
      if (triggerSrc1 != 0)
      {
         for (j = 0; j < nusoc_getNumInputs(nucfg->socH); j++)
         {
            if (i != j)
            {
               triggerSrc2 = input_tbl[j].params.sensorParams.trigger_src;
               if (triggerSrc1 == triggerSrc2)
               {
                  if (input_tbl[j].params.sensorParams.fps != input_tbl[i].params.sensorParams.fps)
                  {
                     LOGG_PRINT(LOG_ERROR_E, NULL, "Wrong data. sensors #%d and #%d are with same trigger source, but different frame rate (%d %d respectively)\n", i, j, input_tbl[i].params.sensorParams.fps, input_tbl[j].params.sensorParams.fps);
                     free(input_tbl);
                     return NUCFG__ERR_INVALID_ARGS;
                  }
               }
            }
         }
      }
   }
   free(input_tbl);

   return NUCFG__RET_SUCCESS;
}

static int compareResolution(UINT32 width0, UINT32 height0, UINT32 width1, UINT32 height1)
{
    int wDiff = ((int)width1 - (int)width0);
    int hDiff = ((int)height1 - (int)height0);

    if( (wDiff >= 0) && (hDiff > 0) )   {printf("case 0\n"); return 1;}
    if( (wDiff > 0) && (hDiff >= 0) )   {printf("case 1\n"); return 1;}

    if( (wDiff <= 0) && (hDiff < 0) )   {printf("case 2\n"); return -1;}
    if( (wDiff < 0) && (hDiff <= 0) )   {printf("case 3\n"); return -1;}

    if( (wDiff == 0) && (hDiff == 0) )  {printf("case 4\n"); return 0;}

    printf("Unexpected values\n");
    return -255;
}

ERRG_codeE NUCFG_checkXmlDataValidation(inu_nucfgH nucfg)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;

   ret = validateTriggerSrcFps(nucfg);

   return ret;
}

ERRG_codeE NUCFG_getInputTable(inu_nucfgH nucfgH, NUCFG_inputT *tbl)
{
   unsigned int i;
   nublkH blk;
   nuBlkTypeE blkType;
   unsigned int inst;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   nuCfgT *nucfg= &nucfgP->nucfg_orig;

   for(i = 0; i < nusoc_getNumInputs(nucfg->socH); i++)
   {
      blk = nusoc_getInput(nucfg->socH, i);
      nusoc_getBlkTypeInst(nucfg->socH, blk,&blkType,&inst);
      tbl[i].type = blkE2InputE(blkType);
      tbl[i].inst = inst;
      tbl[i].id = i;
      getInputParams(nucfg,&tbl[i]);
   }
   return NUCFG__RET_SUCCESS;
}

unsigned int NUCFG_getNumChannels(inu_nucfgH nucfgH)
{
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   return nusoc_getNumOutputs(nucfgP->nucfg_orig.socH);
}

ERRG_codeE NUCFG_getChannelInfo(inu_nucfgH nucfgH, unsigned int index, outChannelT *info)
{
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   getChanInputs(&nucfgP->nucfg_orig, index, info->inputs, &info->numInputs);
   getChanSensorsInvolved(&nucfgP->nucfg_orig, &nucfgP->nucfg_mod, index, info);
   getChanInterleavingInfoFromDb(&nucfgP->nucfg_mod, index, &info->numInterleaved, &info->intreleavedMode);
   getChanModes(&nucfgP->nucfg_mod, index, info->modeList, &info->numModes);
   getChanGroup(&nucfgP->nucfg_mod, index, &info->modeGroup, &info->currentMode);
   getChanRes(&nucfgP->nucfg_mod,index, &info->resolution);
   getChanFormat(&nucfgP->nucfg_mod,index,&info->format);
   getChanFormatDiscriptor(&nucfgP->nucfg_mod,index,&info->formatDiscriptor);
   getChanNumLinesPerChunk(&nucfgP->nucfg_mod,index,&info->numLinesPerChunk);
   getChanOutputType(&nucfgP->nucfg_mod,index,&info->outputType);
   info->id = index;
   return NUCFG__RET_SUCCESS;
}

ERRG_codeE NUCFG_getChannelTable(inu_nucfgH nucfgH, outChannelT *chTbl)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   unsigned int i;

   int k = NUCFG_getNumChannels(nucfgH);
   for(i = 0; i < NUCFG_getNumChannels(nucfgH); i++)
   {
       ret = NUCFG_getChannelInfo(nucfgH, i, &chTbl[i]);
       if(ERRG_FAILED(ret))
          break;
   }
   return ret;
}

unsigned int NUCFG_getNumGroups(inu_nucfgH nucfgH)
{
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   return nucfgP->nucfg_mod.numGroups;
}

ERRG_codeE NUCFG_getChannelGroupTable(inu_nucfgH nucfgH, outChannelT *chTbl)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   unsigned int i;

   for (i = 0; i < NUCFG_getNumGroups(nucfgH); i++)
   {
      ret = NUCFG_getChannelInfo(nucfgH, i, &chTbl[i]);
      if (ERRG_FAILED(ret))
         break;
   }
   return ret;
}

ERRG_codeE NUCFG_setChannelEnable(inu_nucfgH nucfgH, unsigned int index)
{
   //NUCFG_INTERLEAVER_ID_CHECK(id);
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   setDbField(&nucfgP->nucfg_mod, NUFLD_META_READERS_E, index, META_READERS_RD_0_ENABLE_E, 1);
   return NUCFG__RET_SUCCESS;
}

ERRG_codeE NUCFG_setHistEnable(inu_nucfgH nucfgH, unsigned int index)
{
   //NUCFG_INTERLEAVER_ID_CHECK(id);
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   setDbField(&nucfgP->nucfg_mod, NUFLD_META_READERS_E, index, META_HISTOGRAMS_HIST_0_ENABLE_E, 1);
   return NUCFG__RET_SUCCESS;
}

ERRG_codeE NUCFG_setCvaChannelEnable(inu_nucfgH nucfgH, unsigned int index)
{
   //NUCFG_INTERLEAVER_ID_CHECK(id);
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   setDbField(&nucfgP->nucfg_mod, NUFLD_META_READERS_E, index, META_CVA_READERS_CVA_RD_0_ENABLE_E, 1);
   return NUCFG__RET_SUCCESS;
}

ERRG_codeE NUCFG_setChannelDisable(inu_nucfgH nucfgH, unsigned int index)
{
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   //NUCFG_INTERLEAVER_ID_CHECK(id);
   setDbField(&nucfgP->nucfg_mod, NUFLD_META_READERS_E, index, META_READERS_RD_0_ENABLE_E, 0);
   return NUCFG__RET_SUCCESS;
}

ERRG_codeE NUCFG_setChannelInterleave(inu_nucfgH nucfgH, unsigned int index,UINT8 inSelect)
{
   unsigned int i,j;
   chStreamT *chp_orig;
   int k;
   nublkH blk;
   void* arg;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;

   //Interleave based on original channel structure -
   chp_orig = &nucfgP->nucfg_orig.streamTbl[index];
   GRAPHG_handleT gh = nucfgP->nucfg_orig.streamTbl[index].gh;

   //Interleaving assumes at least 2 inputs to the channel and at least one selected input
   //if(chp_orig->numInputs < 2)
   //   return NUCFG__ERR_INVALID_ARGS;

   //for each channel input set the matching interleave setting on the paths
   for(i = 0; i < chp_orig->numInputs; i++)
   {
      //mark start of path
      arg = NULL;
      //Find input i's path(s) to the output interleaver and enable/disable all of them
      for(j = 0; j < chp_orig->numPaths; j++)
      {
         hwPathT *path = &chp_orig->paths[j];
         if(path->idpath[path->pathLen-1] == chp_orig->inputs[i].nodeId)
         {
            for (k = path->pathLen-1; k >= 0; k--)
            {
                GRAPHG_getVertex(gh, path->idpath[k], &blk);
                nusoc_setPathInterleave(nucfgP->nucfg_mod.socH, blk, &arg, ((1 << i) & inSelect) ? 1 : 0);
            }
         }
      }
   }
   //Need to update graphs due to change

   return nucfgUpdate(&nucfgP->nucfg_mod);
}

//configurate registers according to dpe functions configurations
static void NUCFG_updateDpeDB(nuCfgT *nucfg, XMLModesG_dpeFuncT *dpeFuncP)
{
   nusoc_setDpeFuncParam(nucfg->socH, 0, dpeFuncP);
}

/****************************************************************************
*
*  Function Name: NUCFG_setDpeFunc
*
*  Description: configurate groups mode
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: graph finalize
*
****************************************************************************/
ERRG_codeE NUCFG_setDpeFunc(inu_nucfgH nucfgH, NUCFG_dpeFuncFileT *dpeFuncFileList, UINT8 len)
{
   UINT8 i=0;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;

   if (nucfgP->nuCfgDb.dpeFuncFileList==NULL)
   {
      nucfgP->nuCfgDb.dpeFuncFileList=(NUCFG_dpeFuncFileT*)malloc(sizeof(NUCFG_dpeFuncFileT)*len);
   }
   else
   {
      free(nucfgP->nuCfgDb.dpeFuncFileList);
      nucfgP->nuCfgDb.dpeFuncFileList=(NUCFG_dpeFuncFileT*)malloc(sizeof(NUCFG_dpeFuncFileT)*len);
   }

   nucfgP->nuCfgDb.numOfFuncFiles   = len;
   for (i=0;i<nucfgP->nuCfgDb.numOfFuncFiles; i++)
   {
      nucfgP->nuCfgDb.dpeFuncFileList[i].name= (char *)malloc(strlen(dpeFuncFileList[i].name)+1);
      strcpy(nucfgP->nuCfgDb.dpeFuncFileList[i].name, dpeFuncFileList[i].name);
      nucfgP->nuCfgDb.dpeFuncFileList[i].frameNum=dpeFuncFileList[i].frameNum;
   }
   return NUCFG__RET_SUCCESS;
}

static ERRG_codeE verifyP0P1Constraint(XMLModesG_dpeFuncT *dpeFunc)
{
   //from algo doc:
   //The infrastructure should support multiple calls to the same function, one with p0 and the other with p1.
   //In all other cases, the infrastructure will exit with an error if the DPE configuration file contains
   //multiple calls to the same function, as a second call may unintentionally reset default parameters that were set by the first call.

   // ELIG: I left this if as an example to what the macro VALIDATE_DPE_FUNC_VALS expands to:
   //if ((dpeFunc->numOfAggFunc == MAX_DPE_FUNCT_INST) && (dpeFunc->aggregationSetup[1].enable) && (dpeFunc->aggregationSetup[2].enable))// not just on default, and 2 instance of this func
   //   //we will check if p0p1 is same, and if p0p1=2 (same params fr both) and another instance p0/p1
   //{
   //    UINT32 p0_p1_mode_1, p0_p1_mode_2;
   //    p0_p1_mode_1 = dpeFunc->aggregationSetup[1].p0_p1_mode;
   //    p0_p1_mode_2 = dpeFunc->aggregationSetup[2].p0_p1_mode;
   //    if ((p0_p1_mode_1 == p0_p1_mode_2) || (p0_p1_mode_1 == BOTH_P0_P1_ENABLED) || (p0_p1_mode_2 == BOTH_P0_P1_ENABLED))
   //    {
   //       return NUCFG__ERR_ILLEGAL_DPE_FUNC_CALL;
   //    }
   //}

   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfAggFunc, dpeFunc->aggregationSetup)
   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfOptimizationFunc, dpeFunc->optimizationSetup)
   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfUniteFunc, dpeFunc->uniteSetup)
   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfSmallFilterFunc, dpeFunc->smallFilterSetup)
   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfBlobFunc, dpeFunc->blobSetup)
   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfEdgeDetectFunc, dpeFunc->edgeDetectSetup)
   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfInfoMapFunc, dpeFunc->infoMapSetup)
   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfSubpixelFunc, dpeFunc->subpixelSetup)
   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfPlaneFitFunc, dpeFunc->planeFitSetup)
   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfConfidenceFunc, dpeFunc->confidenceSetup)
   VALIDATE_DPE_FUNC_VALS(dpeFunc->numOfConfidenceBinsFunc, dpeFunc->confidenceBinsSetup)

   return NUCFG__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: NUCFG_applyDpeFunc
*
*  Description: configurate group mode DB
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: user api
*
****************************************************************************/
ERRG_codeE NUCFG_applyDpeFunc(inu_nucfgH nucfgH)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   UINT8 i=0;
   XMLModesG_dpeFuncT dpeFuncParams;
   char *xmlParse;
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;

   memset(&dpeFuncParams, 0, sizeof(XMLModesG_dpeFuncT));
   dpeFuncParams.depthScaleFactor.depthScaleFactorCva  = 1;//scale factor by default should be 1
   dpeFuncParams.depthScaleFactor.depthScaleFactorHost = 1;//scale factor by default should be 1

   //TODO: next step
   //structure A getparams;
   //structure B finalparams,defaultParams;
   //1. getFuncData(getparams, buf_default)
   // copy getparams -> all of finalparams
   // copy getparams -> all of defaultParams
   //loop (on all files):
   // getFuncData(getparams, buf_next_file)
   // loop (on all function types and p0p1)
   // if (getparams.enable[type][p0p1] && finalparams.enable[type][p0p1])
   //      return err;
   // finalparams = getparams
   //endloop

   for (i = 0; i < nucfgP->nuCfgDb.numOfFuncFiles; i++)
   {
      //parse config part, parse xml,create soc structure
      xmlParse = (char *)XMLModesG_getParsedXML(nucfgP->nuCfgDb.dpeFuncFileList[i].name);
      if (!xmlParse)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Error trying to open file %s\n", nucfgP->nuCfgDb.dpeFuncFileList[i].name);
         return NUCFG__ERR_INVALID_ARGS;
      }

      XMLModesG_getFuncData(&dpeFuncParams, xmlParse);
      ret = verifyP0P1Constraint(&dpeFuncParams);
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Illegal DPE func calling. check p0 / p1 constraint\n");
         return ret;
      }
      NUCFG_updateDpeDB(&nucfgP->nucfg_mod, &dpeFuncParams);
      XMLModesG_loadFromBuf((XMLDBModes_dbH)nucfgP->nucfg_mod.xmldbH, xmlParse);
      free(xmlParse);
   }
   return NUCFG__RET_SUCCESS;
}

ERRG_codeE NUCFG_setChannelDepthMode(inu_nucfgH nucfgH, unsigned int index,NUCFG_formatE format)
{
   (void)index;
   (void)format;
   //Change the depth mode for the channel
   //Notes: allow only for channels passing through the DPE - so this does not affect the graph
   return (ERRG_codeE)0;
}

static ERRG_codeE nucfgSetGroupMode(unsigned int group, INT32 mode, nuCfgH *nucfg)
{
   if((group >= nucfg->nucfg_mod.numGroups) || (chmode2Index(mode) < 0))
      return NUCFG__ERR_INVALID_ARGS;

   setGroupMode(&nucfg->nucfg_mod, group, mode);
   //group mode does not change channel paths, only their parameters, so we don't need a full nucfg update
   updateDbMeta(&nucfg->nucfg_mod);
   return NUCFG__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: NUCFG_setGroupMode
*
*  Description: configurate group mode DB
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: user api
*
****************************************************************************/
ERRG_codeE NUCFG_setGroupMode(inu_nucfgH nucfgH, unsigned int group, INT32 mode)
{
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;
   nucfgP->nuCfgDb.group      |= 1<<group;
   nucfgP->nuCfgDb.mode[group]=  mode;
   return NUCFG__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: NUCFG_applyGroupsMode
*
*  Description: configurate groups mode
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: graph finalize
*
****************************************************************************/
ERRG_codeE NUCFG_applyGroupsMode(inu_nucfgH nucfgH)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;
   unsigned int modeGroup = 0;

   for (modeGroup = 0; modeGroup < NUCFG_getNumGroups(nucfgH); modeGroup++)
   {
      if ((nucfgP->nuCfgDb.group) & (1<<modeGroup))
         ret = nucfgSetGroupMode(modeGroup, nucfgP->nuCfgDb.mode[modeGroup], nucfgP);
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: NUCFG_setGroupFps
*
*  Description: configurate group mode DB
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: user api
*
****************************************************************************/
ERRG_codeE NUCFG_setGroupFps(inu_nucfgH nucfgH, unsigned int group, UINT32 fps)
{
   unsigned int ch, i;
   chStreamT *streamp;
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;

   if(group >= nucfgP->nucfg_mod.numGroups)
      return NUCFG__ERR_INVALID_ARGS;

   //For each channel in the group, and each path in the channel, update the path according to fps.
   for(ch = 0; ch < NUCFG_MAX_CHANNELS; ch++)
   {
      if(!SET_HAS(&(nucfgP->nucfg_mod.groupTbl[group].channels),ch))
         continue; //channel not in the group

      streamp = &nucfgP->nucfg_mod.streamTbl[ch];
      if((streamp->numInputs == 0) || (streamp->inputs[0].type != NUCFG_BLK_SEN_E))
         return NUCFG__ERR_INVALID_ARGS; //support sensor input paths only

      for (i = 0; i < streamp->numInputs; i++)
      {
         XMLDB_setValue(nucfgP->nucfg_mod.xmldbH, NUFLD_calcPath(NUFLD_SENSOR_E, streamp->inputs[i].inst, SENSORS_SENS_0_FPS_E), fps);
      }
   }
   return NUCFG__RET_SUCCESS;
}

ERRG_codeE NUCFG_setInjectResolution(inu_nucfgH nucfgH, unsigned int ch, unsigned int width, unsigned int height)
{
   unsigned int writerI, writerJ,i,j, writerInst;
   chStreamT *streamp, *streampJ;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;

   if(ch >= NUCFG_MAX_CHANNELS)
      return NUCFG__ERR_INVALID_ARGS;

   streamp = &nucfgP->nucfg_mod.streamTbl[ch];
    for (writerI = 0; writerI < streamp->numInputs; writerI++)
    {
        if (streamp->inputs[writerI].type == NUCFG_BLK_AXIWR_E)
        {
            writerInst = streamp->inputs[writerI].inst;
        }
        else
        {
            //injection channel with none writer input is invalid
            return NUCFG__ERR_INVALID_ARGS;
        }

        //For each channel with the same input writer
        // Channel number 21 has MIPI tx0 as output, the resolution on this 
        // channel is not getting set unless we loop till MAX channels.
        for (j = 0; j < NUCFG_MAX_CHANNELS; j++)//allow different sizes on MIPI_TX
        {
            streampJ = &nucfgP->nucfg_mod.streamTbl[j];
            for (writerJ = 0; writerJ < streampJ->numInputs; writerJ++)
            {
                if ((streampJ->inputs[writerJ].type == NUCFG_BLK_AXIWR_E) && (streampJ->inputs[writerJ].inst == writerInst))
                {
                    //For each path in the channel, update the path according to resolution.
                    for (i = 0; i < streampJ->numPaths; i++)
                    {
                        setPathResolution(&nucfgP->nucfg_mod, streampJ, i, width, height);
                    }
                }
            }
        }
    }
   return NUCFG__RET_SUCCESS;
}

void NUCFG_showInput(INU_DEFSG_logLevelE loglevel, NUCFG_inputT *input)
{
   LOGG_PRINT(loglevel, NULL, "[%d] instance=%d, type=%s\n",input->id, input->inst,inputType2str(input->type));
}

void NUCFG_showOutChannel(inu_nucfgH nucfgH, INU_DEFSG_logLevelE loglevel, outChannelT *outch)
{
   unsigned int i;
   char hwidbuf[NUCFG_MAX_HW_ID_SIZE];
   nublkH blk;
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;
   char str[20] ;

   if (outch->outputType == NUCFG_OUTPUT_TYPE_AXI_RD_E_E)
   {
       strcpy(str, "AXI READER");
   }
   else if (outch->outputType == NUCFG_OUTPUT_TYPE_MIPI_TX_E)
   {
       strcpy(str, "MIPI TX");
   }
   else if (outch->outputType == NUCFG_OUTPUT_TYPE_ISP_RD_E)
   {
       strcpy(str, "ISP_RD");
   }

   LOGG_PRINT(loglevel, NULL,"---------------\n");
   GRAPHG_getVertex(nucfgP->nucfg_orig.streamTbl[outch->id].gh, 0, &blk);
   nusoc_getChHwId(nucfgP->nucfg_mod.socH, blk, hwidbuf);
   LOGG_PRINT(loglevel, NULL,"channel %d hwid=%s\n", outch->id, hwidbuf);

   if(outch->numInputs)
   {
      LOGG_PRINT(loglevel, NULL,"inputs: %d\n",outch->numInputs);
      for(i = 0; i < outch->numInputs; i++)
      {
         NUCFG_showInput(loglevel, &outch->inputs[i]);
         LOGG_PRINT(loglevel, NULL,"  %s\n", ((outch->inSelect & (1<<i)) ? "active":"inactive"));
      }
      LOGG_PRINT(loglevel, NULL,"op_modes: %d\n", outch->numModes);
      for(i = 0; i < outch->numModes; i++)
      {
         INT32 mode = outch->modeList[i];
         LOGG_PRINT(loglevel, NULL,"  %s %s\n", calmode2str(mode), (mode == outch->currentMode) ? "(set)" : "");
      }
      LOGG_PRINT(loglevel, NULL,"group=%d\n", outch->modeGroup);
      LOGG_PRINT(loglevel, NULL,"resolution=%dx%d bpp=%d\n", outch->resolution.width,
         outch->resolution.height,outch->resolution.bpp);
      LOGG_PRINT(loglevel, NULL, "numInterleaved=%d\n", outch->numInterleaved);
      LOGG_PRINT(loglevel, NULL,"format=%d\n", outch->format);
      LOGG_PRINT(loglevel, NULL,"output type=%s\n", str);
      LOGG_PRINT(loglevel, NULL,"Number of lines per chunk=%d\n", outch->numLinesPerChunk);
   }
   else
   {
      LOGG_PRINT(loglevel, NULL,"0 inputs\n");
   }

   LOGG_PRINT(loglevel, NULL,"---------------\n");
}

void NUCFG_showChannelGroup(inu_nucfgH nucfgH, INU_DEFSG_logLevelE loglevel, unsigned int group)
{
   outChannelT outch;
   unsigned int i;
   for(i = 0; i < NUCFG_getNumChannels(nucfgH); i++)
   {
      NUCFG_getChannelInfo(nucfgH, i, &outch);
      if((group == NUCFG_CHAN_GROUP_ANY) || (outch.modeGroup == group))
         NUCFG_showOutChannel(nucfgH, loglevel,&outch);
   }
}

unsigned int NUCFG_getChanHwIdLen(inu_nucfgH nucfgH, unsigned int index)
{
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;

   NUCFG_hwId hwid = NULL;
   getChanHwId(&nucfgP->nucfg_orig, index, &hwid);
   return (unsigned int) ((hwid) ? strlen(hwid) : 0);
}

void NUCFG_getChanHwId(inu_nucfgH nucfgH, unsigned int index, char *buf, unsigned int bufsize)
{
   NUCFG_hwId hwid;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   //returns length of hw id copied to user buf.
   getChanHwId(&nucfgP->nucfg_orig, index, &hwid);
   if(hwid)
      copyHwId(hwid,buf,bufsize);
}

unsigned int NUCFG_getInputHwIdLen(inu_nucfgH nucfgH, unsigned int index)
{
   NUCFG_hwId hwid = NULL;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   getInputHwId(&nucfgP->nucfg_orig, index, &hwid);
   return (unsigned int) ((hwid) ? strlen(hwid) : 0);
}

void NUCFG_getInputHwId(inu_nucfgH nucfgH, unsigned int index, char *buf, unsigned int bufsize)
{
   NUCFG_hwId hwid = NULL;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;
   getInputHwId(&nucfgP->nucfg_orig, index, &hwid);
   if(hwid)
      copyHwId(hwid,buf,bufsize);
}

ERRG_codeE NUCFG_saveDbHAndWriteXml(inu_nucfgH nucfgH, char *xmlbuf, char *xmlPath)
{
   XMLDB_dbH dbh = NULL;
   ERRG_codeE ret;
   unsigned int outSize;
   char *outBuf = NULL;

   dbh = NUCFG_getDbH(nucfgH);
   ret = XMLDB_saveDb(dbh, xmlbuf, &outSize, &outBuf);
   if (ERRG_FAILED(ret))
      return SYSTEM__ERR_UNEXPECTED;
   XMLDB_writeXml(outBuf, xmlPath, outSize - 1);

   if (outBuf)
   {
      free(outBuf);
   }
   return ret;
}

ERRG_codeE NUCFG_saveModDbHAndWriteXml(inu_nucfgH nucfgH, char *xmlbuf, char *xmlPath)
{
   XMLDB_dbH dbh = NULL;
   ERRG_codeE ret;
   unsigned int outSize;
   char *outBuf = NULL;

   dbh = NUCFG_getCurrDbH(nucfgH);
   ret = XMLDB_saveDb(dbh, xmlbuf, &outSize, &outBuf);
   if (ERRG_FAILED(ret))
   {
      return SYSTEM__ERR_UNEXPECTED;
   }
   XMLDB_writeXml(outBuf, xmlPath, outSize - 1);
   if (outBuf)
   {
      free(outBuf);
   }
   return ret;
}

ERRG_codeE NUCFG_setChannelScale(inu_nucfgH nucfgH, UINT32 hSize, UINT32 vSize, UINT32 channel)
{
   int graphRet;
   unsigned int i;
   UINT32 numOfSclBlkOnPath = 0;
   nublkH blk;
   chStreamT *streamp;
   socPathUpdateT upd;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;

   if (channel > NUCFG_MAX_CHANNELS)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Channel %d is illegal\n", channel);
      return NUCFG__ERR_ILLEGAL_CHANNEL;
   }

   if (nucfgP->nucfg_mod.streamTbl[channel].numInputs == 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Channel %d is not configured on HW configuration\n", channel);
      return NUCFG__ERR_CHANNEL_NOT_CONFIGURED;
   }

   graphRet = GRAPHG_getVertex(nucfgP->nucfg_mod.streamTbl[channel].gh, 0, &blk); //root
   if (graphRet < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Graph error\n");
      return NUCFG__ERR_GRAPH_CREATE_FAILED;
   }

   nusoc_updateBlkSclSize(nucfgP->nucfg_mod.socH, blk, hSize - 1, vSize - 1, &numOfSclBlkOnPath);

   if (numOfSclBlkOnPath == 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Channel %d path does not contain block with scaler (ppu or dpp)\n", channel);
      return NUCFG__ERR_CHANNEL_NOT_CONFIGURED;
   }

   if (hSize % 16 != 0)
   {
       LOGG_PRINT(LOG_ERROR_E, NULL, "Scale width is not multipale of 16\n");
       return NUCFG__ERR_ILLEGAL_SCL_WIDTH;
   }
   streamp = &nucfgP->nucfg_mod.streamTbl[channel];
   //channel meta needs to be updated, ppu data has already been updated, use null update for this.
   resetUpdateInfo(&upd);
   for (i=0;i<streamp->numPaths;i++)
      updatePath(&nucfgP->nucfg_mod, streamp, i, &upd,0);
   updateDbMetaResolutions(&nucfgP->nucfg_mod);

   return NUCFG__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: NUCFG_setChannelChunk
*
*  Description: sets a channel chunk on line base size
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: user api
*
****************************************************************************/
ERRG_codeE NUCFG_setChannelChunk(inu_nucfgH nucfgH, UINT32 channel, UINT16 numLinesPerChunk)
{
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;

   if ( (channel > NUCFG_MAX_DB_META_CHANNELS) || (nucfgP->nucfg_mod.streamTbl[channel].numInputs == 0) )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Channel %d is illegal\n", channel);
      return NUCFG__ERR_ILLEGAL_CHANNEL;
   }

   nucfgP->nuCfgDb.channelForChunk       |= 1<<channel;
   nucfgP->nuCfgDb.channelChunk[channel] = numLinesPerChunk;
   return NUCFG__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: NUCFG_setChannelDimentions
*
*  Description: configurate group mode DB
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: user api
*
****************************************************************************/
ERRG_codeE NUCFG_setChannelCrop(inu_nucfgH nucfgH, UINT32 channel, NUCFG_resT *userCrop)
{
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;
   nucfgP->nuCfgDb.channelForCrop    |= 1<<channel;
   memcpy(&(nucfgP->nuCfgDb.cropDim[channel]), userCrop,sizeof(NUCFG_resT));
   return NUCFG__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: NUCFG_setChannelDimentions
*
*  Description: configurate group mode DB
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: user api
*
****************************************************************************/
ERRG_codeE NUCFG_setChannelDimentions(inu_nucfgH nucfgH, UINT32 channel, NUCFG_resT *userDim)
{
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;
   nucfgP->nuCfgDb.channelForScale    |= 1<<channel;
   memcpy(&(nucfgP->nuCfgDb.scaleDim[channel]), userDim,sizeof(NUCFG_resT));
   return NUCFG__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: NUCFG_setChannelFormat
*
*  Description: Sets a channel format
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: user api
*
****************************************************************************/
ERRG_codeE NUCFG_setChannelFormat(inu_nucfgH nucfgH, UINT32 channel, NUCFG_changeFromatE reqFormat)
{
   int graphRet;
   nublkH blk;
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   outChannelT chInfo;
   NUCFG_formatE currentFormat;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;

   NUCFG_getChannelInfo(nucfgH, channel, &chInfo);
   currentFormat = chInfo.format;

   if ( (channel > NUCFG_MAX_CHANNELS) || (nucfgP->nucfg_mod.streamTbl[channel].numInputs == 0) )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Channel %d is illegal\n", channel);
      return NUCFG__ERR_ILLEGAL_CHANNEL;
   }
   if ( (reqFormat > NUCFG_MAX_CHANGE_FORMAT__E) )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "format %d is illegal\n", reqFormat);
      return NUCFG__ERR_INVALID_ARGS;
   }
   graphRet = GRAPHG_getVertex(nucfgP->nucfg_mod.streamTbl[channel].gh, 0, &blk); //root
   if (graphRet < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Graph error\n");
      return NUCFG__ERR_GRAPH_CREATE_FAILED;
   }
   //validate format request
   //check if the format is already equal to request
   if (((reqFormat == NUCFG_CHANGE_FORMAT_TO_YUV_E) && ((currentFormat == NUCFG_FORMAT_YUV422_10BIT_E) || (currentFormat == NUCFG_FORMAT_YUV422_8BIT_E))) ||
      ((reqFormat == NUCFG_CHANGE_FORMAT_TO_RGB_E) && (currentFormat == NUCFG_FORMAT_RGB888_E)) ||
      ((reqFormat == NUCFG_CHANGE_FORMAT_TO_RAW10_E) && (currentFormat == NUCFG_FORMAT_RAW10_E)) ||
      ((reqFormat == NUCFG_CHANGE_FORMAT_TO_GRAY16_E) && (currentFormat == NUCFG_FORMAT_GREY_16_E)))
   {
      LOGG_PRINT(LOG_WARN_E, NULL, "Channel %d format is already %d\n", channel, currentFormat);
      return NUCFG__RET_SUCCESS;
   }
   //validate possible format convert
   if (((reqFormat == NUCFG_CHANGE_FORMAT_TO_YUV_E) && (currentFormat != NUCFG_FORMAT_RGB888_E)) ||
      ((reqFormat == NUCFG_CHANGE_FORMAT_TO_RGB_E) && (currentFormat != NUCFG_FORMAT_YUV422_10BIT_E) && (currentFormat != NUCFG_FORMAT_YUV422_8BIT_E)) || //TODO:check which convert supportd yuv 8 or 10 bit
      ((reqFormat == NUCFG_CHANGE_FORMAT_TO_RAW10_E) && (currentFormat != NUCFG_FORMAT_GREY_16_E)) ||
      ((reqFormat == NUCFG_CHANGE_FORMAT_TO_GRAY16_E) && (currentFormat != NUCFG_FORMAT_RAW10_E)))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Illegal format convert. current format is %d, requst format is %d\n", currentFormat, reqFormat);
      return NUCFG__ERR_INVALID_ARGS;
   }

   ret = nusoc_setChannelFormat(nucfgP->nucfg_mod.socH, blk, channel, reqFormat);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "Error while trying to set channel format (0x%x)\n", ret);
     return ret;
   }

   return nucfgUpdate(&nucfgP->nucfg_mod);
}

/****************************************************************************
*
*  Function Name: NUCFG_setChannelIauBypass
*
*  Description: Sets a channel's IAU's in bypass mode
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: user api
*
****************************************************************************/
ERRG_codeE NUCFG_setChannelIauBypass(inu_nucfgH nucfgH, UINT32 channel, UINT32 *iauNum)
{
   int graphRet;
   nublkH blk;
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   nuCfgH *nucfgP = (nuCfgH *)nucfgH;

   if ( (channel > NUCFG_MAX_CHANNELS) || (nucfgP->nucfg_mod.streamTbl[channel].numInputs == 0) )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Channel %d is illegal\n", channel);
      return NUCFG__ERR_ILLEGAL_CHANNEL;
   }
   graphRet = GRAPHG_getVertex(nucfgP->nucfg_mod.streamTbl[channel].gh, 0, &blk); //root
   if (graphRet < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Graph error\n");
      return NUCFG__ERR_GRAPH_CREATE_FAILED;
   }

   *iauNum = 0;
   ret = nusoc_setIauBypass(nucfgP->nucfg_mod.socH, blk, iauNum);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "Error while trying to set IAU bypass (0x%x)\n", ret);
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "Bypass on channel %d\n", channel);

   return ret;
}
/**
 * @brief Applies channel dimensions for a channel
 * 
 * @param nucfgH nucfgH handle
 * @param i Channel Index
 */
static ERRG_codeE NUCFG_applyChannelDimentionsForChannel(inu_nucfgH nucfgH, unsigned int i)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   UINT32 channel;
   NUCFG_resT *userDim;
   int graphRet;
   nublkH blk;
   NUCFG_resT outRes = {0};
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;
   NUCFG_interModeE mode;
   UINT32 num_interleaved;

   if ((nucfgP->nuCfgDb.channelForCrop) & (1 << i))
   {
      userDim = &(nucfgP->nuCfgDb.cropDim[i]);
      channel = i;

      if ((channel > NUCFG_MAX_CHANNELS) || (nucfgP->nucfg_mod.streamTbl[channel].numInputs == 0))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Channel %d is illegal\n", channel);
         return NUCFG__ERR_ILLEGAL_CHANNEL;
      }

      graphRet = GRAPHG_getVertex(nucfgP->nucfg_mod.streamTbl[channel].gh, 0, &blk); // root
      if (graphRet < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Graph error\n");
         return NUCFG__ERR_GRAPH_CREATE_FAILED;
      }

      getChanRes(&nucfgP->nucfg_mod, channel, &outRes);
      LOGG_PRINT(LOG_INFO_E, NULL, "NUCFG_applyChannelDimentions: (Cropping) Frame (W-%d/H-%d), Buffer (W-%d/H-%d), Start (X-%d/Y-%d)\n",
                 outRes.width, outRes.height, outRes.stride, outRes.bufferHeight, outRes.x, outRes.y);

      nusoc_setBlkCropSize(nucfgP->nucfg_mod.socH, blk, 0, userDim);
      nusoc_getInterleaveInfoFromDb(nucfgP->nucfg_mod.socH, blk, &num_interleaved, &mode);
      if (num_interleaved == 2)
      {
         userDim->stride = userDim->stride * 2;
         userDim->width = userDim->width * 2;
      }
      updateDbMetaChannelResolution(&nucfgP->nucfg_mod, channel, userDim);
   }

   if ((nucfgP->nuCfgDb.channelForScale) & (1 << i))
   {
      userDim = &(nucfgP->nuCfgDb.scaleDim[i]);
      channel = i;

      if ((channel > NUCFG_MAX_CHANNELS) || (nucfgP->nucfg_mod.streamTbl[channel].numInputs == 0))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Channel %d is illegal\n", channel);
         return NUCFG__ERR_ILLEGAL_CHANNEL;
      }

      graphRet = GRAPHG_getVertex(nucfgP->nucfg_mod.streamTbl[channel].gh, 0, &blk); // root
      if (graphRet < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Graph error\n");
         return NUCFG__ERR_GRAPH_CREATE_FAILED;
      }

      getChanRes(&nucfgP->nucfg_mod, channel, &outRes);
      if (compareResolution(userDim->width, userDim->height, outRes.width, outRes.height) != 0)
      {
         ret = NUCFG_setChannelScale(nucfgH, userDim->width, userDim->height, channel);
         if (ERRG_FAILED(ret))
         {
                LOGG_PRINT(LOG_ERROR_E, ret, "Scaling failed on channel %d\n", channel);
                return NUCFG__ERR_INVALID_ARGS;
         }
      }

      getChanRes(&nucfgP->nucfg_mod, channel, &outRes); // Get updated outRes. Should be the same as requestedWidth/requestedHeight
      printf("NUCFG_applyChannelDimentions: (Scaling) Frame (W-%d/H-%d), Buffer (W-%d/H-%d), Start (X-%d/Y-%d)\n",
             outRes.width, outRes.height, outRes.stride, outRes.bufferHeight, outRes.x, outRes.y);

      switch (compareResolution(outRes.width, outRes.height, userDim->stride, userDim->bufferHeight))
      {
      case 1:
         // Do meta data update

         // printf("NUCFG_setChannelDimentions: Frame (W-%d/H-%d), Buffer (W-%d/H-%d), Start (X-%d/Y-%d)\n",
         // userDim->width, userDim->height, userDim->stride, userDim->bufferHeight, userDim->x, userDim->y);

         updateDbMetaChannelResolution(&nucfgP->nucfg_mod, channel, userDim);
         break;

      case 0:
         // Do nothing
         break;

      case -1:
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Requested configuration on channel %d is illegal\n", channel);
         // return NUCFG__ERR_INVALID_ARGS;
         break;
      }
   }

   // now that all the resolutions have been set, we can check if the chunk mode is valid
   if ((nucfgP->nuCfgDb.channelForChunk) & (1 << i))
   {
      getChanRes(&nucfgP->nucfg_mod, i, &outRes);
      if (outRes.bufferHeight % nucfgP->nuCfgDb.channelChunk[i])
      {
         // invalid configuration for chunk mode, residue is not allowed
         LOGG_PRINT(LOG_ERROR_E, NUCFG__ERR_ILLEGAL_CHUNK_SIZE, "Requested for ch %d: %d lines per chunk, bufferHeight %d, res %d\n", i, nucfgP->nuCfgDb.channelChunk[i], outRes.bufferHeight, (outRes.bufferHeight % nucfgP->nuCfgDb.channelChunk[i]));
         return NUCFG__ERR_ILLEGAL_CHUNK_SIZE;
      }
   }
}

/****************************************************************************
*
*  Function Name: NUCFG_applyChannelDimentions
*
*  Description: configurate groups mode
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: graph finalize
*
****************************************************************************/
ERRG_codeE NUCFG_applyChannelDimentions(inu_nucfgH nucfgH)
{

   ERRG_codeE ret = NUCFG__RET_SUCCESS;

   for (int i=0;i<NUCFG_MAX_DB_META_CHANNELS;i++)
   {
      ret = NUCFG_applyChannelDimentionsForChannel(nucfgH,i);
   }
   /*
    * Modified by William.Bright@imd-tec.com so that we can use the SLU for cropping images going to the ISP
   */
   INT32 ispStartChannelIndex = NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS +  NUCFG_MAX_DB_MIPI_TX_META_CHANNELS;
   INT32 ispEndChannelIndex = NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_CVA_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS +  NUCFG_MAX_DB_MIPI_TX_META_CHANNELS + NUCFG_MAX_DB_ISP_META_CHANNELS;
   for (int i =ispStartChannelIndex;i<ispEndChannelIndex;i++)
   {
      ret = NUCFG_applyChannelDimentionsForChannel(nucfgH,i);
   }
   return ret;
}

ERRG_codeE NUCFG_linkChannelWritersToSensors(inu_nucfgH nucfgH, UINT32 ch, NUCFG_channelSrcSensorsList *sensorInst)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   nuCfgH *nucfgP = (nuCfgH*)nucfgH;
   linkWriterSensors *myWriterP;
   unsigned int i, j, foundWriter = 0;;

   if (sensorInst->numOfSensors > NUCFG_MAX_CHAN_INPUTS)
      return NUCFG__ERR_UNEXPECTED;

    if ((nucfgP->nucfg_mod.streamTbl[ch].numInputs == 1) && (nucfgP->nucfg_mod.streamTbl[ch].inputs[0].type == NUCFG_BLK_AXIWR_E))
    {
        foundWriter = 1;
        myWriterP = &(nucfgP->nuCfgDb.linkWriterSensors[nucfgP->nuCfgDb.numWritersWithCalibData]);
        myWriterP->dummyNumInputs = sensorInst->numOfSensors;
        myWriterP->writerInst = nucfgP->nucfg_mod.streamTbl[ch].inputs[0].inst;
        for (j = 0; j < sensorInst->numOfSensors; j++)
        {
            myWriterP->dummyInputs[j].type = NUCFG_BLK_SEN_E;
            myWriterP->dummyInputs[j].inst = sensorInst->sensorList[j];
        }
        nucfgP->nuCfgDb.numWritersWithCalibData++;
    }
    else
    {
        for (i = 0; i < nucfgP->nucfg_mod.streamTbl[ch].numInputs; i++)
        {
            myWriterP = &(nucfgP->nuCfgDb.linkWriterSensors[nucfgP->nuCfgDb.numWritersWithCalibData]);
            if (nucfgP->nucfg_mod.streamTbl[ch].inputs[i].type == NUCFG_BLK_AXIWR_E)
            {
                foundWriter = 1;
                myWriterP->dummyNumInputs = 1;
                myWriterP->writerInst = nucfgP->nucfg_mod.streamTbl[ch].inputs[i].inst;

            myWriterP->dummyInputs[0].type = NUCFG_BLK_SEN_E;
            myWriterP->dummyInputs[0].inst = sensorInst->sensorList[i];

            nucfgP->nuCfgDb.numWritersWithCalibData++;
         }
      }
   }
   if (foundWriter)
   {
      nucfgUpdate(&nucfgP->nucfg_mod);
      return ret;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "can't find on writer on channel %d\n", ch);
      return NUCFG__ERR_UNEXPECTED;
   }
}
ERRG_codeE NUCFG_applyIspLinkToWriterData(inu_nucfgH h)
{
    ERRG_codeE ret = NUCFG__RET_SUCCESS;
    nuCfgH* nucfgP = (nuCfgH*)h;
    const CALIB_sectionDataT* calSection = NULL;
    int chId = -1;
    UINT32 writerId,ch;

    UINT32 width, height, sensorId, mode, ispRdNum;
    chStreamT* streamp;
    //run all writers and find if any isp reader linked to him, get data from there
    for (writerId = 0; writerId < NUCFG_MAX_DB_WRITERS_META_CHANNELS; writerId++)
    {
        ret = nusoc_getIspLinkToWriterData(nucfgP->nucfg_mod.socH, writerId, &width, &height, &sensorId, &ispRdNum);

        if (ERRG_SUCCEEDED(ret))
        {
            nucfgP->nuCfgDb.linkWriterSensorsForIsp[nucfgP->nuCfgDb.numWritersWithCalibDataToIsp].linkWriterSensors.writerInst = writerId;
            nucfgP->nuCfgDb.linkWriterSensorsForIsp[nucfgP->nuCfgDb.numWritersWithCalibDataToIsp].linkWriterSensors.totalHorizontalSize[0] = width;
            nucfgP->nuCfgDb.linkWriterSensorsForIsp[nucfgP->nuCfgDb.numWritersWithCalibDataToIsp].linkWriterSensors.totalVerticalSize[0] = height;
            nucfgP->nuCfgDb.linkWriterSensorsForIsp[nucfgP->nuCfgDb.numWritersWithCalibDataToIsp].ispRdNum = ispRdNum;

            //find isp ch source (sensor), and writer output channel, and send them to link function:
            //sensor id we found at nusoc_getIspLinkToWriterData sunction
            //now find the ch num of this writer
            for (ch = 0; ch < NUCFG_getNumChannels(nucfgP); ch++)
            {
                if ((nucfgP->nucfg_mod.streamTbl[ch].inputs->type == NUCFG_BLK_AXIWR_E) && (nucfgP->nucfg_mod.streamTbl[ch].inputs->inst == writerId))
                {
                    chId = ch;
                    break;
                }
            }
            if (chId == -1)
            {
                printf("can't find channel id to this writer\n");
            }
            else
            {
                NUCFG_inputT sensorParams;
                sensorParams.type = NUCFG_INPUT_TYPE_SENSOR_E_E;
                sensorParams.inst = sensorId;

                getInputParams(&nucfgP->nucfg_orig, &sensorParams);

                nucfgP->nuCfgDb.linkWriterSensorsForIsp[nucfgP->nuCfgDb.numWritersWithCalibDataToIsp].linkWriterSensors.dummyNumInputs = 1;
                nucfgP->nuCfgDb.linkWriterSensorsForIsp[nucfgP->nuCfgDb.numWritersWithCalibDataToIsp].linkWriterSensors.dummyInputs[0].inst = sensorId;
                nucfgP->nuCfgDb.linkWriterSensorsForIsp[nucfgP->nuCfgDb.numWritersWithCalibDataToIsp].linkWriterSensors.dummyInputs[0].type = NUCFG_BLK_SEN_E;
                if (sensorParams.params.sensorParams.mode == 1)
                {
                    mode = CALIB_MODE_BIN_E;
                }
                else if (sensorParams.params.sensorParams.mode == 2)
                {
                    mode = CALIB_MODE_VERTICAL_BINNING_E;
                }
                else if (sensorParams.params.sensorParams.mode == 3)
                {
                    mode = CALIB_MODE_FULL_E;
                }
                else
                {
                    mode = CALIB_MODE_UNKNOWN;
                }
                nucfgP->nuCfgDb.linkWriterSensorsForIsp[nucfgP->nuCfgDb.numWritersWithCalibDataToIsp].linkWriterSensors.sensorMode[0] = mode;

                NUCFG_channelSrcSensorsList sensorInst;
                sensorInst.numOfSensors = 1;
                sensorInst.sensorList[0] = sensorId;
                ret = NUCFG_linkChannelWritersToSensors(nucfgP, chId, &sensorInst);
                if (ERRG_FAILED(ret))
                {
                    return ret;
                }

                //set inject resolution
                ret = NUCFG_setInjectResolution(nucfgP, chId, width, height);
                if (ERRG_FAILED(ret))
                {
                    return ret;
                }
                nucfgP->nuCfgDb.numWritersWithCalibDataToIsp++;
            }
        }
        if (ERRG_SUCCEEDED(ret))
        {
            nucfgUpdate(&nucfgP->nucfg_mod);
        }
    }
    return ret;
}

/////////////////////////////////////////////
#if 0
#define XML_FILE_NAME "config/nu4k.xml"
#define XML_FILE_NAME_OUT_ORIG "config/nu4k_out_orig.xml"
#define XML_FILE_NAME_OUT_MOD "config/nu4k_out_mod.xml"

static int loadNuSocxml(char **xmlbuf, unsigned int *xmlsize)
{
   int ret = 0;
   char *buf = NULL;
   unsigned int size;
   //open and load xml file
   FILE *xmlfile = fopen(XML_FILE_NAME, "r");
   if (!xmlfile)
      return -1;

   fseek(xmlfile, 0L, SEEK_END);
   size = ftell(xmlfile);
   fseek(xmlfile, 0L, SEEK_SET);

   buf = (char *)malloc(size + 1);
   if (buf)
   {
      size_t r = fread(buf, size, 1, xmlfile);
      if ((r != 1) && (!feof(xmlfile)))
      {
         printf("read file error %s\n", strerror(errno));
         ret = -1;
         free(buf);
      }
      buf[size] = '\0';//otherwise sxmlc parser chokes
   }
   else
   {
      ret = -1;
   }

   if (ret >= 0)
   {
      *xmlbuf = buf;
      *xmlsize = size;
   }

   fclose(xmlfile);
   return ret;
}

static void writeXml(const char *xmlbuf, const char *name, size_t size)
{
   FILE *xmlfile = fopen(name, "w");
   if(xmlfile)
   {
      fwrite(xmlbuf, size, 1, xmlfile);
      fflush(xmlfile);
      fclose(xmlfile);
   }
}

static ERRG_codeE saveDb(XMLDB_dbH db, const char *xmlbuf, unsigned int *outSize, char **outBuf)
{
   ERRG_codeE ret;
   char *buf = NULL;
   unsigned int size;

   //first pass - get size of required buffer
   ret = XMLDB_saveToBuf(db, xmlbuf, NULL, &size);
   if (ERRG_SUCCEEDED(ret))
   {
      buf = (char *)malloc(size);
      if (!buf)
         ret = XMLDB__ERR_OUT_OF_RSRCS;
   }

   //second pass - save the buffer as xml to the same buffer
   // nothing is written on second pass because db is
   if (ERRG_SUCCEEDED(ret))
   {
      ret = XMLDB_saveToBuf(db, xmlbuf, buf, NULL);
   }

   if (ERRG_SUCCEEDED(ret))
   {
      *outBuf = buf;
      *outSize = size;
   }
   else
   {
      if (buf)
         free(buf);
   }

   return ret;
}

//nucfg unit test
//  load/open a an xml file.
//  call init function with xml
//  output the "working" xml
//  invoke api get functions to check match with xml
//
//  invoke api set functions:
//     - output xml and get functions
//
int nucfg_unit_test()
{
   ERRG_codeE ret;
   unsigned int xmlsize,outSize;
   char *xmlbuf = NULL;
   char *outBuf = NULL;
   XMLDB_dbH dbh = NULL;
   outChannelT outch;
   unsigned int i;

   if(loadNuSocxml(&xmlbuf, &xmlsize) < 0)
      return (-1);

   ret = NUCFG_init(NULL,xmlbuf);
   if(ERRG_FAILED(ret))
      return (-1);

   for(i = 0; i < NUCFG_MAX_DB_META_CHANNELS + NUCFG_MAX_DB_HIST_META_CHANNELS; i++)
   {
      NUCFG_getChannelInfo(i, &outch);
      if(outch.numInputs >= 1)
      {
         NUCFG_showOutChannel(LOG_INFO_E, &outch);
         NUCFG_setChannelEnable(i);
      }
   }
   dbh = NUCFG_getDbH();
   ret = saveDb(dbh, xmlbuf, &outSize, &outBuf);
   if(ERRG_FAILED(ret))
      return (-1);

   if(outBuf)
   {
      writeXml(outBuf, XML_FILE_NAME_OUT_ORIG, outSize-1);
      free(outBuf);
   }

   dbh = NUCFG_getCurrDbH();
   ret = saveDb(dbh, xmlbuf, &outSize, &outBuf);
   if(ERRG_FAILED(ret))
      return (-1);

   if(outBuf)
   {
      writeXml(outBuf, XML_FILE_NAME_OUT_MOD, outSize-1);
      free(outBuf);
   }

   NUCFG_deInit();
   return 0;
}

//[Dima] - Temp funcs for DB loadinng


static  int loadNu4000Socxml(char **xmlbuf, unsigned int *xmlsize)
{
   int ret = 0;
   char *buf = NULL;
   unsigned int size;
   //open and load xml file
   FILE *xmlfile = fopen("config/nu4k_out_mod.xml", "r");
   if (!xmlfile)
      return -1;

   fseek(xmlfile, 0L, SEEK_END);
   size = ftell(xmlfile);
   fseek(xmlfile, 0L, SEEK_SET);

   buf = (char *)malloc(size + 1);
   if (buf)
   {
      size_t r = fread(buf, size, 1, xmlfile);
      if ((r != 1) && (!feof(xmlfile)))
      {
         printf("read file error %s\n", strerror(errno));
         ret = -1;
         free(buf);
      }
      buf[size] = '\0';//otherwise sxmlc parser chokes
   }
   else
   {
      ret = -1;
   }

   if (ret >= 0)
   {
      *xmlbuf = buf;
      *xmlsize = size;
   }

   fclose(xmlfile);
   return ret;
}

#endif

#ifdef __cplusplus
}
#endif
