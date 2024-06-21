#include "SandBoxFWLib/SandBoxLib.h"
#include "Sandbox.h"
#include "post_process.h"
#include "runtime_commands.h"
#include "inu2.h"
#include "assert.h"
#include <string.h>

#if defined _OPENCV_
#include "opencv2/core/persistence.hpp"
#endif

#if (defined _WIN32) || (defined _WIN64)
#include "getopt.h"
#include "windows.h"
#include <sys/types.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <unistd.h>
#endif

//#define DEBUG_THREAD_ANALYSIS

#ifdef DEBUG_THREAD_ANALYSIS
#include "cvmarkersobj.h"
using namespace Concurrency::diagnostic;
#endif

#ifdef _DEBUG
#define SANDBOX_ASSERT(x) assert(0);
#else
#define SANDBOX_ASSERT(x) return x;
#endif

#if (MAX_STREAM_OUT_NAME_LEN > MAX_NAME_LEN)
#error "MAX_STREAM_OUT_NAME_LEN can't be larger than MAX_NAME_LEN!"
#endif

#define SANDBOX_MAX_NUM_CONFIGS             (12)
#define SANDBOX_MAX_NUM_HW_TESTS            (4)
#define SANDBOX_MAX_NUM_NETWORK_ID          (5)

#define SANDBOX_DURATION_INFINITE           (-1)

#define SANDBOX_PARAMS_FOLDER               ".\\SandBoxParams"

typedef struct SANDBOX_cdnnNetworkIdItem
{
   char cdnnFunctionName[MAX_NAME_LEN];
   UINT32 networkId;
} SANDBOX_cdnnNetworkIdItemT;

typedef struct SANDBOX_cdnnNetworkIdList
{
   SANDBOX_cdnnNetworkIdItemT cdnnNetworkIdList[SANDBOX_MAX_NUM_NETWORK_ID];
   UINT32 numOfNetworks;
} SANDBOX_cdnnNetworkIdListT;

typedef struct
{
   char config_name1[NUCFG_MAX_CONFIG_NAME];
   char node_name1[MAX_NAME_LEN];
   char config_name2[NUCFG_MAX_CONFIG_NAME];
   char node_name2[MAX_NAME_LEN];
}bindNodesConfigT;

typedef struct
{
   char                 hw_functions_name[MAX_NAME_LEN];
   FWLib_chH            chH;
}SANDBOX_hwFunctionTestParamsT;

typedef struct
{
   SANDBOX_hwFunctionTestParamsT hwFuncTestsParams[SANDBOX_MAX_NUM_HW_TESTS];
   UINT32                        numHwFunctionTests;
   FWLib_configParams            fwlibConfigParams;
   SANDBOX_channelParamNodeT     *channelParamNodes;
   SANDBOX_cdnnNetworkIdListT    cdnnNetworkIdList;
}SANDBOX_configParams;

static SANDBOX_initParams   initParams;
static SANDBOX_configParams configParams[SANDBOX_MAX_NUM_CONFIGS];
static bindNodesConfigT     bindNodesParams[SANDBOX_MAX_NUM_CONFIGS];
static UINT32 numBinds = 0;
static int duration = SANDBOX_DURATION_INFINITE;
static int iterations = 1;
UINT8 testResult = 0;
static int cmdListener = 0;
static bool sandBoxUseLongerTimeout = false;
static bool extInterleaveMode = false;
static UINT32 extInterleaveModeIdx = 0;
static UINT32 extInterleaveChannels[NUCFG_MAX_DB_META_CHANNELS] = { 0 };

//UINT32 streamNum;
extern UINT32 streamNum;

int millisleep(unsigned ms)
{
#if (defined _WIN32) || (defined _WIN64)
   SetLastError(0);
   Sleep(ms);
   return GetLastError() ? -1 : 0;
#elif defined(__linux__)
   usleep(1000 * ms);
   return 0;
#else
#error ("no milli sleep available for platform")
   return -1;
#endif
}

ERRG_codeE findConfigInTable(const char *name, void **configH)
{
   int i;
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;

   for (i = 0; i < SANDBOX_MAX_NUM_CONFIGS; i++)
   {
      if ((configParams[i].fwlibConfigParams.name) && (!strcmp(configParams[i].fwlibConfigParams.name, name)))
      {
         *configH = configParams[i].fwlibConfigParams.configH;
         ret = (ERRG_codeE)RET_SUCCESS;
         break;
      }
   }

   return ret;
}

ERRG_codeE setStreamRecordOutputFrames(const char* stream_name, UINT32 nFrames)
{
   int i;
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
   SANDBOX_channelParamNodeT* channel_paramP;

   for (i = 0; i < SANDBOX_MAX_NUM_CONFIGS; i++)
   {
       if (configParams[i].fwlibConfigParams.name)
       {
           channel_paramP = configParams[i].channelParamNodes;
           while (channel_paramP)
           {
               if ((channel_paramP->channelParam->stream_out_name) && (!strcmp(channel_paramP->channelParam->stream_out_name, stream_name)))
               {
                   channel_paramP->channelParam->recordOutputFrames = nFrames;
                   return (ERRG_codeE)RET_SUCCESS;
               }
               channel_paramP = channel_paramP->next;
           }
       }
   }
   return ret;
}

static SANDBOX_configParams *findOrAllocConfigInTable(const char *name)
{
   int i;
   for (i = 0; i < SANDBOX_MAX_NUM_CONFIGS; i++)
   {
      if ((configParams[i].fwlibConfigParams.name) && (!strcmp(configParams[i].fwlibConfigParams.name, name)))
      {
         return &configParams[i];
      }
      if (!configParams[i].fwlibConfigParams.name)
      {
         configParams[i].fwlibConfigParams.name = strdup(name);
         return &configParams[i];
      }
   }

   printf("Could not find  or assign a place holder for config %s, exit\n",name);
   exit(-1);
}

static void display_channels(inu_nucfgH cfgH)
{
   unsigned int groupCtr;
   outChannelT *chTbl;
   ERRG_codeE ret;

   //view\query channels and input table
   chTbl = (outChannelT*)malloc(sizeof(outChannelT) * NUCFG_getNumChannels(cfgH));

   ret = NUCFG_getChannelTable(cfgH, chTbl);
   if (ERRG_SUCCEEDED(ret))
   {
      for (groupCtr = 0; groupCtr < NUCFG_getNumGroups(cfgH); groupCtr++)
      {
         NUCFG_showChannelGroup(cfgH, LOG_INFO_E, groupCtr);
      }
   }
   free(chTbl);
}

static void channel_params_add_new_node(SANDBOX_channelParamNodeT **listHead, SANDBOX_channelParamsT *node)
{
   SANDBOX_channelParamNodeT  *newNode, *runner;
   newNode = (SANDBOX_channelParamNodeT*)malloc(sizeof(SANDBOX_channelParamNodeT));
   if (!newNode)
      assert(0);
   newNode->channelParam = node;
   newNode->next = NULL;
   //add inputs at the end, to maintain the same order of creation
   //check if head is empty
   if (!(*listHead))
   {
      *listHead = newNode;
   }
   else
   {
      runner = *listHead;
      //find last in list
      while (runner->next)
      {
         runner = runner->next;
      }
      runner->next = newNode;
   }
}

static void parse_channel_params(SANDBOX_configParams *configP, char *string)
{
   char *str = NULL;
   str = strtok(string, ",");

   if (str) {
      SANDBOX_channelParamsT *channelParam;

      channelParam = (SANDBOX_channelParamsT*)malloc(sizeof(SANDBOX_channelParamsT));
      if (channelParam == NULL)
         assert(0);
      memset(channelParam, 0, sizeof(SANDBOX_channelParamsT));
      strncpy(channelParam->stream_out_name, str, MAX_STREAM_OUT_NAME_LEN);

      str = strtok(NULL, ",");
      while (str)
      {
          unsigned int tmp = 0;
         if ((strcmp(str, "stats") == 0) || (strcmp(str, "q") == 0))
         {
            channelParam->statsEnable = 1;
         }
         else if ((strcmp(str, "record") == 0) || (strcmp(str, "y") == 0))
         {
            channelParam->recordOutput = 1;
         }
         else if ((strcmp(str, "dumppng") == 0) || (strcmp(str, "yp") == 0))
         {
            channelParam->recordPNG = 1;
         }
         else if ((strcmp(str, "bayer_2_rgb_convert") == 0) || (strcmp(str, "c") == 0))
         {
             channelParam->bayer2RgbConvert = 1;
         }
         else if ((sscanf(str, "bayer_pattern %d ", &tmp) == 1) || (sscanf(str, "bp_%d", &tmp) == 1))
         {
             channelParam->bayerPattern = tmp;
         }
#if defined _OPENCV_
         else if ((strcmp(str, "display") == 0) || (strcmp(str, "s") == 0))
         {
            channelParam->displayOutput = 1;
         }
#endif
         else if ((strcmp(str, "metadata") == 0) || (strcmp(str, "m") == 0))
         {
             channelParam->printMetadata = 1;
         }
         else if ((strcmp(str, "snsrupdate") == 0) || (strcmp(str, "z") == 0))
         {
             channelParam->sendSensorUpdates = 1;
         }
         else if ((strcmp(str, "roi") == 0) || (strcmp(str, "r") == 0))
         {
             channelParam->sendROIUpdate = 1;
         }
         else if ((strcmp(str, "syncupdate") == 0) || (strcmp(str, "x") == 0))
         {
             channelParam->sendSampleSensorSyncUpdate = 1;
         }
         else if ((strcmp(str, "trackingsync") == 0) || (strcmp(str, "u") == 0))
         {
            channelParam->sendSampleSwitchToInternalTrackingSyncUpdate = 0;
            channelParam->sendSampleSwitchToTrackingSyncUpdate = 1;
         }
         else if ((strcmp(str, "intsync") == 0) || (strcmp(str, "i") == 0))
         {
            channelParam->sendSampleSwitchToInternalTrackingSyncUpdate = 1;
            channelParam->sendSampleSwitchToTrackingSyncUpdate = 0;
         }
         else if ((strcmp(str, "afVST") == 0) || (strcmp(str, "f") == 0))
         {
            channelParam->sendSampleAFVSTCropWindowUpdate = 1;
         }
         else if ((strcmp(str, "metrec") == 0) || (strcmp(str, "w") == 0))
         {
            channelParam->recordMetadata = 1;
         }
         else if ((strcmp(str, "vstfoc") == 0) || (strcmp(str, "e") == 0))
         {
            channelParam->sendSampleAFVSTFocusPosition = 1;
         }
         else if ((strcmp(str, "fsg") == 0) )
         {
            channelParam->startAllStreams = 1;
         }
         else if ((strcmp(str, "wb") == 0) || (strcmp(str, "b") == 0))
         {
            channelParam->sendSampleAFVSTWBGain = 1;
         }
         else if ((strcmp(str, "synct") == 0))
         {
            channelParam->sendSampleAFVSTExpoDelayedUpdate = 1;
         }
         else if ((strcmp(str, "mcsv") == 0))
         {
            channelParam->recordCSVMetadata = 1;
         }
         else
         {
            assert(0);
         }
         str = strtok(NULL, ",");
      }
      channel_params_add_new_node(&configP->channelParamNodes, channelParam);
   }
   else
   {
      printf("Empty command for -T is invalid.");
   }
}

static void parse_load_network_params(char *string)
{
   char *str = NULL;
   str = strtok(string, ",");

   initParams.loadNetworkEnabled = true;

   if (str)
   {
      inu_load_network_paramsT params;
      params.networkId = INU_LOAD_NETWORK__CDNN_2_E;
      params.channelSwap = CDNNG_NETWORK_CHANNEL_SWAP;
      params.pipeDepth = 1;
      params.ProcesssingFlags = 0;

      strncpy(params.bin_name, str, SANDBOX_MAX_LEN_FILE_NAME);
      params.is_active = true;
      str = strtok(NULL, ",");
      while (str)
      {
         unsigned int tmp = 0;
         if ((sscanf(str, "pipe_depth_%d", &tmp) == 1) || (sscanf(str, "pd_%d", &tmp) == 1))
         {
            params.pipeDepth = tmp;
         }
         else if ((sscanf(str, "process_flags_%d", &tmp) == 1) || (sscanf(str, "pf_%d", &tmp) == 1))
         {
            params.ProcesssingFlags = tmp;
         }
         else if ((sscanf(str, "network_id_%d", &tmp) == 1) || (sscanf(str, "ni_%d", &tmp) == 1))
         {
            params.networkId = (inu_load_network_cdnnIdE)tmp;
         }
         else if ((sscanf(str, "channel_swap_%d", &tmp) == 1) || (sscanf(str, "cs_%d", &tmp) == 1))
         {
            params.channelSwap = (CDNNG_channelSwapE)tmp;
         }
         else
         {
            assert(0);
         }
         str = strtok(NULL, ",");
      }

      if (params.networkId >= INU_LOAD_NETWORK__CDNN_LAST_E || params.networkId < INU_LOAD_NETWORK__CDNN_0_E)
      {
         assert(0);
      }
      memcpy(&initParams.inu_load_network_params[params.networkId], &params, sizeof(inu_load_network_paramsT));
   }
   else
   {
      printf("Empty command for -L is invalid.");
   }
}

static void display_help(void)
{
   char helpStringBuffer[BUFFER_SIZE];

   sprintf(helpStringBuffer,
      "Usage: SandBox [OPTION]\n"
#if defined _OPENCV_
      "\n"
      "  <loadparams/lp> paramsFile.yaml    Load SandBox arguments from ./SandBoxParams/paramsFile.yaml or from full path paramsFile.yaml\n"
#endif
      "\n"
      "  -N cfgname hwxmlpath swxmlpath     Create new configuration\n"
      "  -E cfgname -t 0 -v 0 1 ....        Edit a configuration\n"
      "  -S filepath                        Display HW channels for a given XML (offline, does not invoke boot)\n"
      "  -C                                 Open a command listener for invoking runtime commands\n"
      " Parameters per Streamer in a configuration:\n"
      "  -T streamerName,<display/s>,<stats/q>,<record/y>,<metadata/m>     Add streamer. Additional parameters that may be added are:\n"
#if defined _OPENCV_
      "                                                                     display or s - Display output\n"
#endif
      "                                                                     stats or q - Collect and print statistics for a test\n"
      "                                                                     record or y - Record output of the test to the disk (./record folder)\n"
      "                                                                     metadata or m - Print metadata for each frame\n"
      "                                                                     snsrupdate or z - Send example sensorUpdate for each frame\n"
      "                                                                     syncupdate or x - Send example sensor sync updates for each frame\n"
      "                                                                     roi or r - Send an example ROI update every second\n"
      "                                                                     u or tracksync - Send an example command to switch to triggering the Tracking sensor from the Track Sync signal\n"
      "                                                                     i or intsync - Send an example command to switch to triggering the Tracking sensor internally instead of from the Track Sync Signal \n"
      "                                                                     w or metrec - Record metadata to .inudata files \n"
      "                                                                     e or vstfoc - A sample function where the AF VST focus is incremented between 0 and 800\n"
      "                                                                     bayer2RgbConvert or c -to covert from bayer to RGB\n"
      "                                                                     bayer_pattern_<num> or bp_<num> for bayer pattern 1 - RG , 2 - GR ,3 - GB , 4 - BG \n "
      );

   snprintf(helpStringBuffer + strlen(helpStringBuffer), BUFFER_SIZE - strlen(helpStringBuffer),

      " Parameters for configuration:\n"
      "  -v channel bitmap                  set interleaving off for a channel. bitmap specifies which streams to keep\n"
      "  -c channel lines                   Set chunk mode on channel, each chunk with given number of lines\n"
      "  -w channel W H                     Set HW scaling on channel, (width and height)\n"
      "  -e channel W H X Y                 Set crop on channel (width, height, location xy)\n"
      "  -u channel S H W H X Y             Set DMA stride on channel (stride, buffer height,width, height, location x y)\n"
      "  -b channel                         Set bypass in a channel\n"
      "  -m channel format                  Set channel format. format  0-RGB  1-YUV  2-RAW10  3-GRAY16 \n\t\t\t  (YUV-->RGB(by PPU CSC), RGB-->YUV(just set bypass to csc if enabled), GRAY16-->RAW10(by CSC manipulation) RAW10-->GRAY16(bypass to CSC if enabled) \n"
      "  -n sensor fps                      Set sensor (or group) fps\n"
      "  -r sensor mode                     Set sensor (or group) resolution mode (0 - bin, 1 - full, 2 - user defined, 3 - vertical binning, 4 - UXVGA\n"
      "  -i channel sensor                  Add sensor info to injection\n"
      "  -J channel W H                     define injection channel resolution\n"
      "  -I mode                            injection (1 - IR Raw 640x400, 2 - IR Raw 1280x800, 3 - FE Raw 640x480, 4 - RGB Raw 400x400)\n"
      "  -H HW_name                         activate hardware by name\n"
      "  -L bin_name,                       Load network bin, to a certain network id. # is the value that one desires to set.\n"
      "     <pipe_depth_#/pd_#>,            Default pipeDepth value = 1\n"
      "     <process_flags_#/pf_#>,         Default ProcesssingFlags value = 0\n"
      "     <network_id_#/ni_#>,            Default networkId value = 2\n"
      "     <channel_swap_#/cs_#>           Default channelSwap value = 1\n"
      "  -G CDNN_func_name network_id       Set network ID to CDNN Function Name\n"
      "\n"
      " Paremeters in general:\n"
      "  -B cfgname1 name1 cfgname2 name2   Set sensor (or group) resolution mode (0 - bin, 1 - full, 2 - user defined, 3 - vertical binning, 4 - UXVGA\n"
      "  -l                                 Alternate, each nibble is number of frames for the modes. example 23: 3 mode 0, 2 mode 1..\n"
      "  -g                                 Standalone mode\n"
      "  -x                                 HW XML debug mode\n"
      "  -d seconds                         Duration for the tests in seconds\n"
      "  -o iterations                      Number of iterations\n"
      "  -p                                 Disable power optimizations\n"
      "  -Z                                 Allow power down and USB suspend\n"
      "  -a seconds                         Set application watchdog duration. 0 will disable application watchdog\n"
      "  -j path                            Provide a specific path for XML configuration files\n"
      "  -D name                            Provide a specific name of DPE XML configuration files\n"
      "  -k bootid                          Force different bootId\n"
      "  -z calibPath                       Use external calibration folder (C:\\Program Files\\Inuitive\\InuDev\\config\\InuSensors\\XXXXXXXXX)\n"
      "  -A filepath dstfile                Write files to the linux filesystem. Filepath is the path to the file on the local machine, dstfile is the filename to be saved on the linux. (/media/inuitive/)\n"
      "  -U uartPortName                    Activates Time Sync via UART, with specific Port Number or Port Device Name.\n"
      "  -O Interface_select                Which interface to use for deviceInitParams.interface_0.select - 0-USB, 1-UART, 2-TCP 3-CDC, 4-stub.\n"
      "  -t                                 Use longer timeout.\n"
      "  -F                                (Helsinki Specific) 1= Force Master XMLs, 2 = Force Slave XMLs, 3 = Auto detect XMLs based off hardware type.\n"
      " -X channel                         Use extended DMA interleave mode on the specified channel\n"
      );

   printf(helpStringBuffer);
   exit(EXIT_SUCCESS);
}

void process_options(int argc, char* argv[])
{
   SANDBOX_configParams *configP = findOrAllocConfigInTable("main");
   int error = 0;
   int opt_num = 0;
   FWLib_channelSrcSensorsListT *injectSensorInfo = NULL;
   FWLib_resT *crop = NULL;
   FWLib_resT *scale = NULL;
   FWLib_resT *injectRes = NULL;

   initParams.tuningServer = (char)-1;

   for (;;) {
      static char const *short_options = "a:b:c:d:e:f:r:w:i:o:j:v:n:m:l:k:t:z:u:A:N:E:D:B:F:J:I:H:S:T:L:G:pghxCU:ZV:O:X:";

      int c = getopt(argc, argv, (char*)short_options);
      if (c == EOF)
         break;

     if (optarg)
        opt_num = atoi(optarg);

     switch (c) {
      case 't':
         sandBoxUseLongerTimeout = true;
         break;      
      case 'h':
         display_help();
         exit(0);
         break;
      case 'p':
         initParams.disablePowerOptimization = true;
         break;
      case 'Z':
         initParams.allowPowerDown = true;
         break;
      case 'C':
         cmdListener = true;
         break;
      case 'k':
         initParams.bootid = opt_num;
         break;
      case 'z':
         strncpy(initParams.calibPath, optarg, 256);
         break;
      case 'i':
        injectSensorInfo = &configP->fwlibConfigParams.injectSensorInfo[opt_num];

        injectSensorInfo->sensorList[injectSensorInfo->numOfSensors] = atoi(argv[optind]);
        injectSensorInfo->numOfSensors++;
         break;
      case 'A':
         strncpy(initParams.filesSendList[initParams.numberOfFilesToSend], optarg, 256);
         strncpy(initParams.filesDstNameSendList[initParams.numberOfFilesToSend], argv[optind], 256);
         initParams.numberOfFilesToSend++;
         break;
      case 'E':
         configP = findOrAllocConfigInTable(optarg);
         break;
      case 'F':
         /*-F x allows us to force us to use the HW/SW XML strings to be for the master or for the slave
         x can be the following values:
         0 = Disabled
         1 = Use Master XMLs
         2 = Use Slave XMLs
         3 = Autodetect XMLs
         */
         configP->fwlibConfigParams.forceXML_Type = opt_num;
         break;
      case 'N':
         configP = findOrAllocConfigInTable(optarg);
         configP->fwlibConfigParams.hwXmlFile = strdup(argv[optind]);
         configP->fwlibConfigParams.swXmlFile = strdup(argv[optind + 1]);
         break;
      case 'S':
         inu_nucfgH cfgH;
         strncpy(initParams.configFilesPath, optarg, 256);
         FwLib_offline_get_config(initParams.configFilesPath, &cfgH);
         display_channels(cfgH);
         exit(0);
         break;
      case 'B':
         strncpy(bindNodesParams[numBinds].config_name1, optarg, SANDBOX_MAX_LEN_FILE_NAME);
         strncpy(bindNodesParams[numBinds].node_name1,   argv[optind], SANDBOX_MAX_LEN_FILE_NAME);
         strncpy(bindNodesParams[numBinds].config_name2, argv[optind + 1], SANDBOX_MAX_LEN_FILE_NAME);
         strncpy(bindNodesParams[numBinds].node_name2,   argv[optind + 2], SANDBOX_MAX_LEN_FILE_NAME);
         numBinds++;
         break;
      case 'D':
         strcpy(configP->fwlibConfigParams.dpeConfigFiles[configP->fwlibConfigParams.dpeConfigNumFiles], optarg);
         configP->fwlibConfigParams.dpeConfigNumFiles++;
         break;
      case 'x':
         initParams.hwXmlDebugMode = true;
         break;
      case 'j':
         strncpy(initParams.configFilesPath, optarg, 256);
         break;
      case 'g':
         initParams.standalone = true;
         initParams.interfaceSelect = INU_DEVICE_INTERFACE_0_STUB;
         break;
      case 'a':
         initParams.watchdogTimeout = opt_num;
         break;
      case 'n':
         configP->fwlibConfigParams.fps[opt_num] = atoi(argv[optind]);
         break;
      case 'I':
         configP->fwlibConfigParams.injectionMode = opt_num;
         break;
      case 'T':
         streamNum++;
         parse_channel_params(configP, optarg);
         break;
      case 'd':
         duration = opt_num;
         break;
      case 'o':
         iterations = opt_num;
         break;
      case 'l':
         initParams.alternate = opt_num;
         break;
      case 'b':
         configP->fwlibConfigParams.iaeBypass[opt_num] = 1;
         break;
      case 'm':
         configP->fwlibConfigParams.cscMode[opt_num] = atoi(argv[optind]);
         break;
      case 'r':
         configP->fwlibConfigParams.mode[opt_num] = atoi(argv[optind]);
         break;
      case 'c':
         configP->fwlibConfigParams.chunkMode[opt_num] = atoi(argv[optind]);
         break;
      case 'u':
         crop = &configP->fwlibConfigParams.crop[opt_num];

         crop->stride =  atoi(argv[optind]);
         crop->bufferHeight =  atoi(argv[optind + 1]);
         crop->width = atoi(argv[optind + 2]);
         crop->height = atoi(argv[optind + 3]);
         crop->x = atoi(argv[optind + 4]);
         crop->y = atoi(argv[optind + 5]);
         break;
      case 'e':
         crop = &configP->fwlibConfigParams.crop[opt_num];

         crop->width = atoi(argv[optind]);
         crop->height = atoi(argv[optind + 1]);
         crop->x = atoi(argv[optind + 2]);
         crop->y = atoi(argv[optind + 3]);
         break;
      case 'w':
       scale = &configP->fwlibConfigParams.scale[opt_num];

       scale->width = atoi(argv[optind]);
       scale->stride = atoi(argv[optind]);
       scale->height = atoi(argv[optind + 1]);
       scale->bufferHeight = atoi(argv[optind + 1]);
         break;
      case 'J':
       injectRes = &configP->fwlibConfigParams.injectRes[opt_num];

       injectRes->width = atoi(argv[optind]);
       injectRes->stride = atoi(argv[optind]);
       injectRes->height = atoi(argv[optind + 1]);
       injectRes->bufferHeight = atoi(argv[optind + 1]);
         break;
      case 'v':
         configP->fwlibConfigParams.interleave[opt_num] = atoi(argv[optind]);
         break;
      case 'H':
         if (configP->numHwFunctionTests < SANDBOX_MAX_NUM_HW_TESTS)
         {
            strncpy(configP->hwFuncTestsParams[configP->numHwFunctionTests++].hw_functions_name, optarg, MAX_NAME_LEN);
         }
         else
         {
            assert(0);
         }
         break;
      case 'L':
         parse_load_network_params(optarg);
         break;
      case 'G':
         if (configP->cdnnNetworkIdList.numOfNetworks < SANDBOX_MAX_NUM_NETWORK_ID)
         {
            strncpy(configP->cdnnNetworkIdList.cdnnNetworkIdList[configP->cdnnNetworkIdList.numOfNetworks].cdnnFunctionName, optarg, MAX_NAME_LEN);
            configP->cdnnNetworkIdList.cdnnNetworkIdList[configP->cdnnNetworkIdList.numOfNetworks].networkId = atoi(argv[optind]);
            configP->cdnnNetworkIdList.numOfNetworks++;
         }
         else
         {
            assert(0);
         }
         break;
      case 'U':
          initParams.useUartSync = 1;
          strncpy(initParams.uartPortName, optarg, MAX_UART_PORT_NAME_LEN);
          break;
      case 'O':
          initParams.interfaceSelect = opt_num;

          if (initParams.interfaceSelect == INU_DEVICE_INTERFACE_0_UART)
          {
             initParams.uartIfParams.portNumber = atoi(argv[optind]);
             initParams.uartIfParams.baudrate   = atoi(argv[optind + 1]);
          }
          else if (initParams.interfaceSelect == INU_DEVICE_INTERFACE_0_CDC)
          {
             initParams.cdcIfParams.portNumber = atoi(argv[optind]);
             initParams.cdcIfParams.baudrate   = atoi(argv[optind + 1]);
          }
          else if (initParams.interfaceSelect == INU_DEVICE_INTERFACE_0_TCP)
          {
              strcpy(initParams.tcpIfParams.targetIp, "192.168.10.5");
              strcpy(initParams.tcpIfParams.hostIp, "192.168.10.6");
              initParams.tcpIfParams.port = atoi(argv[optind]);
          }
          break;
      case 'y':
          initParams.tuningServer = opt_num;
          break;
#if 0
      case 'X':
          extInterleaveMode = true;
          if (extInterleaveModeIdx >= NUCFG_MAX_DB_META_CHANNELS)
          {
              error++;
          }
          else
          {
              extInterleaveChannels[extInterleaveModeIdx++] = opt_num;
          }
          break;
#endif
      case '?':
         error++;
         break;
      }
   }

   argc -= optind;
   argv += optind;

}

#if defined _OPENCV_
int process_file_networks(cv::FileNode* networks, SANDBOX_configParams* configP, SANDBOX_initParams* initParams)
{
   if (networks->size() <= SANDBOX_MAX_NUM_NETWORK_ID)
   {
      for (int i = 0; i < networks->size(); i++)
      {
         inu_load_network_paramsT params;

         params.networkId = INU_LOAD_NETWORK__CDNN_2_E;
         params.channelSwap = CDNNG_NETWORK_CHANNEL_SWAP;
         params.pipeDepth = 1;
         params.ProcesssingFlags = 0;
         params.is_active = true;

         cv::FileNode networkParam = (*networks)[i]["networkId"];
         if (networkParam.empty())
         {
            printf("Missing Netork ID in the network list\n");
            assert(0);
         }

         params.networkId = (inu_load_network_cdnnIdE)(int)networkParam;
         if (params.networkId < 0)
         {
            continue;
         }
         else if (params.networkId >= INU_LOAD_NETWORK__CDNN_LAST_E)
         {
            printf("Network ID %d is illegal\n", params.networkId);
            assert(0);
         }

         networkParam = (*networks)[i]["binName"];
         if (!networkParam.empty())
            strncpy(params.bin_name, networkParam.string().c_str(), SANDBOX_MAX_LEN_FILE_NAME);

         networkParam = (*networks)[i]["pipeDepth"];
         if (!networkParam.empty())
            params.pipeDepth = (int)networkParam;

         networkParam = (*networks)[i]["processsingFlags"];
         if (!networkParam.empty())
            params.ProcesssingFlags = (int)networkParam;

         networkParam = (*networks)[i]["channelSwap"];
         if (!networkParam.empty())
            params.channelSwap = (CDNNG_channelSwapE)(int)networkParam;
         memcpy(&initParams->inu_load_network_params[params.networkId], &params, sizeof(inu_load_network_paramsT));
         initParams->loadNetworkEnabled = true;

         networkParam = (*networks)[i]["CDNN_funcName"];
         if (!networkParam.empty())
         {
            if (configP->cdnnNetworkIdList.numOfNetworks < SANDBOX_MAX_NUM_NETWORK_ID)
            {
               strncpy(configP->cdnnNetworkIdList.cdnnNetworkIdList[configP->cdnnNetworkIdList.numOfNetworks].cdnnFunctionName,
                  networkParam.string().c_str(), MAX_NAME_LEN);
               configP->cdnnNetworkIdList.cdnnNetworkIdList[configP->cdnnNetworkIdList.numOfNetworks].networkId = (UINT32)params.networkId;
               configP->cdnnNetworkIdList.numOfNetworks++;
            }
            else
            {
               assert(0);
            }
         }
      }
   }
   return 0;
}

int process_file_backgrounds(cv::FileNode* backgrounds, SANDBOX_configParams* configP, SANDBOX_initParams* initParams)
{
    for (int i = 0; i < backgrounds->size(); i++)
    {
        inu_load_background_paramsT params;

        params.backgroundId = 2;

        cv::FileNode backgroundParam = (*backgrounds)[i]["backgroundId"];
        if (backgroundParam.empty())
        {
            printf("Missing background ID in the background list\n");
            assert(0);
        }

        params.backgroundId = (int)backgroundParam;
        if (params.backgroundId < 0)
        {
            continue;
        }

        backgroundParam = (*backgrounds)[i]["binName"];
        if (!backgroundParam.empty())
            strncpy(params.bin_name, backgroundParam.string().c_str(), SANDBOX_MAX_LEN_FILE_NAME);

        memcpy(&initParams->inu_load_background_params, &params, sizeof(inu_load_background_paramsT));
        initParams->loadBackgroundEnabled = true;
    }

    return 0;
}

int process_file_channels(cv::FileNode* channels, SANDBOX_configParams* configP)
{
   if (channels->size() <= SANDBOX_NUM_HW_CHANNELS)
   {
      for (int i = 0; i < channels->size(); i++)
      {
         cv::FileNode channelParam = (*channels)[i]["channel"];
         if (channelParam.empty())
         {
            printf("Missing channel number in the channel list\n");
            assert(0);
         }

         int channelID = (int)channelParam;
         if (channelID < 0)
            continue;

         channelParam = (*channels)[i]["interleave"];
         if (!channelParam.empty())
            configP->fwlibConfigParams.interleave[channelID] = (int)channelParam;

         channelParam = (*channels)[i]["chunkMode"];
         if (!channelParam.empty())
            configP->fwlibConfigParams.chunkMode[channelID] = (int)channelParam;

         channelParam = (*channels)[i]["scale"];
         if (!channelParam.empty())
         {
            FWLib_resT* scale = &configP->fwlibConfigParams.scale[channelID];
            cv::FileNode scaleParam;

            scaleParam = channelParam["width"];
            if (!scaleParam.empty())
            {
               scale->width = (int)scaleParam;
               scale->stride = (int)scaleParam;
            }

            scaleParam = channelParam["height"];
            if (!scaleParam.empty())
            {
               scale->height = (int)scaleParam;
               scale->bufferHeight = (int)scaleParam;
            }
         }

         channelParam = (*channels)[i]["cscMode"];
         if (!channelParam.empty())
            configP->fwlibConfigParams.cscMode[channelID] = channelParam;

         channelParam = (*channels)[i]["iauBypass"];
         if (!channelParam.empty())
            configP->fwlibConfigParams.iaeBypass[channelID] = (bool)(int)channelParam;

         channelParam = (*channels)[i]["crop"];
         if (!channelParam.empty())
         {
            FWLib_resT* crop = &configP->fwlibConfigParams.crop[channelID];
            cv::FileNode cropParam;

            cropParam = channelParam["stride"];
            if (!cropParam.empty())
               crop->stride = (int)cropParam;

            cropParam = channelParam["bufferHeight"];
            if (!cropParam.empty())
               crop->bufferHeight = (int)cropParam;

            cropParam = channelParam["width"];
            if (!cropParam.empty())
               crop->width = (int)cropParam;

            cropParam = channelParam["height"];
            if (!cropParam.empty())
               crop->height = (int)cropParam;

            cropParam = channelParam["x"];
            if (!cropParam.empty())
               crop->x = (int)cropParam;

            cropParam = channelParam["y"];
            if (!cropParam.empty())
               crop->y = (int)cropParam;
         }

         channelParam = (*channels)[i]["setInjectionRes"];
         if (!channelParam.empty())
         {
            cv::FileNode width = channelParam["width"];
            cv::FileNode height = channelParam["height"];
            FWLib_resT* injectRes = &configP->fwlibConfigParams.injectRes[channelID];

            injectRes->width = (int)width;
            injectRes->stride = (int)width;
            injectRes->height = (int)height;
            injectRes->bufferHeight = (int)height;
         }

         channelParam = (*channels)[i]["linkSensorsToWriter"];
         if (!channelParam.empty())
         {
            FWLib_channelSrcSensorsListT* injectSensorInfo = &configP->fwlibConfigParams.injectSensorInfo[channelID];

            if (channelParam.size() <= SANDBOX_NUM_CHANNELS_SRC_SENSORS)
            {
               for (int i = 0; i < channelParam.size(); i++)
               {
                  injectSensorInfo->sensorList[injectSensorInfo->numOfSensors] = (int)channelParam[i];
                  injectSensorInfo->numOfSensors++;
               }
            }
            else
            {
               assert(0);
            }
         }
      }
   }
   else
   {
      assert(0);
   }
   return  0;
}

int process_file_sensors(cv::FileNode* sensors, SANDBOX_configParams* configP)
{
   if (sensors->size() < SANDBOX_NUM_SENSORS)
   {
      for (int i = 0; i < sensors->size(); i++)
      {
         cv::FileNode sensorParam = (*sensors)[i]["sensor"];
         if (sensorParam.empty())
         {
            assert(0);
         }

         int sensorID = (int)sensorParam;

         if (sensorID < 0)
            continue;

         sensorParam = (*sensors)[i]["frameRate"];
         if (!sensorParam.empty())
            configP->fwlibConfigParams.fps[sensorID] = (int)sensorParam;

         sensorParam = (*sensors)[i]["resMode"];
         if (!sensorParam.empty())
            configP->fwlibConfigParams.mode[sensorID] = (int)sensorParam;
      }
   }
   else
   {
      assert(0);
   }
   return 0;
}

int process_file_streams(cv::FileNode* streams, SANDBOX_configParams* configP)
{
   for (int i = 0; i < streams->size(); i++)
   {
      SANDBOX_channelParamsT* channelParam = (SANDBOX_channelParamsT*)malloc(sizeof(SANDBOX_channelParamsT));
      if (channelParam == NULL)
         assert(0);

      memset(channelParam, 0, sizeof(SANDBOX_channelParamsT));

      cv::FileNode streamParam = (*streams)[i]["stream"];
      if (streamParam.empty())
      {
         printf("Missing stream name in the stream list\n");
         assert(0);
      }

      strncpy(channelParam->stream_out_name, streamParam.string().c_str(), MAX_STREAM_OUT_NAME_LEN);

      streamParam = (*streams)[i]["statsEnable"];
      if (!streamParam.empty())
         channelParam->statsEnable = (int)streamParam;

      streamParam = (*streams)[i]["recordOutput"];
      if (!streamParam.empty())
         channelParam->recordOutput = (int)streamParam;

      streamParam = (*streams)[i]["bayer2RgbConvert"];
      if (!streamParam.empty())
          channelParam->bayer2RgbConvert = (int)streamParam;

      streamParam = (*streams)[i]["bayerPattern"];
      if (!streamParam.empty())
          channelParam->bayerPattern = (int)streamParam;

      streamParam = (*streams)[i]["displayOutput"];
      if (!streamParam.empty())
         channelParam->displayOutput = (int)streamParam;

      channel_params_add_new_node(&configP->channelParamNodes, channelParam);
   }
   return 0;
}

int process_file(const char* filename, SANDBOX_initParams* initParams)
{
   cv::FileStorage fs;
   char fullFileName[256];
   cv::FileNode node;

   sprintf(fullFileName, "%s\\%s", SANDBOX_PARAMS_FOLDER, filename);
   if (!fs.open(fullFileName, cv::FileStorage::READ))
   {
      if (!fs.open(filename, cv::FileStorage::READ))
      {
         fprintf(stderr, "File %s not found\n", filename);
         return -1;
      }
   }

   std::string tag = fs["tag"].string();
   double version = (double)fs["version"];
   cv::FileNode params = fs["params"];

   if (tag != "sandbox-parameters")
   {
      printf("Wrong YAML file tag\n");
      assert(0);
   }

   printf("Parsing %s file arguments (tag %s v%.1lf)\n", fullFileName, tag.c_str(), version);

   node = params["offlineHwXmlParse"];
   if (!node.empty())
   {
      cv::FileNode offlineEnable = node["offlineEnable"];

      if (!offlineEnable.empty() && (int)offlineEnable == 1)
      {
         cv::FileNode hwXmlPath = node["hwXmlPath"];
         inu_nucfgH cfgH;

         if (!hwXmlPath.empty())
            strncpy(initParams->configFilesPath, hwXmlPath.string().c_str(), 256);

         FwLib_offline_get_config(initParams->configFilesPath, &cfgH);
         display_channels(cfgH);
         exit(0);
      }
   }

   node = params["disablePowerOptimization"];
   if (!node.empty())
      initParams->disablePowerOptimization = (bool)(int)node;

   node = params["duration"];
   if (!node.empty())
      duration = (int)node;

   node = params["iterations"];
   if (!node.empty())
      iterations = (int)node;

   node = params["cmdListener"];
   if (!node.empty())
      cmdListener = (bool)(int)node;

   node = params["standaloneMode"];
   if (!node.empty())
   {
      initParams->standalone = (int)node;
      initParams->interfaceSelect = INU_DEVICE_INTERFACE_0_STUB;
   }

   node = params["watchdogTimeout"];
   if (!node.empty())
      initParams->watchdogTimeout = (int)node;

   node = params["configFilesPath"];
   if (!node.empty() && !node.string().empty())
      strcpy(initParams->configFilesPath, node.string().c_str());

   node = params["calibPath"];
   if (!node.empty() && !node.string().empty())
      strcpy(initParams->calibPath, node.string().c_str());

   node = params["linuxFiles"];
   if (!node.empty())
   {
      if (node.size() <= SANDBOX_MAX_FILES_TO_SEND)
      {
         for (int i = 0; i < node.size(); i++)
         {
            cv::FileNode path = node[i]["filePath"];
            cv::FileNode filename = node[i]["fileName"];
            if (!path.empty() && !filename.empty() && !filename.string().empty())
            {
               strncpy(initParams->filesSendList[initParams->numberOfFilesToSend], path.string().c_str(), SANDBOX_MAX_LEN_FILE_NAME);
               strncpy(initParams->filesDstNameSendList[initParams->numberOfFilesToSend], filename.string().c_str(), SANDBOX_MAX_LEN_FILE_NAME);
               initParams->numberOfFilesToSend++;
            }
         }
      }
      else
      {
         assert(0);
      }
   }

   node = params["bootID"];
   if (!node.empty())
      initParams->bootid = (int)node;

   node = params["hwXmlDebugMode"];
   if (!node.empty())
      initParams->hwXmlDebugMode = (bool)(int)node;

    node = params["tuningServer"];
    if (!node.empty())
        initParams->tuningServer = (char)(int)node;
    else
        initParams->tuningServer = -1;

   node = params["alternate"];
   if (!node.empty())
   {
      cv::FileNode mode = node["mode"];
      cv::FileNode frames = node["frames"];
      if (!mode.empty() && !frames.empty())
         initParams->alternate = ((int)mode << 4) | (int)frames;
   }

   node = params["uartPortName"];
   if (!node.empty() && !node.string().empty())
   {
      initParams->useUartSync = 1;
      strncpy(initParams->uartPortName, node.string().c_str(), MAX_UART_PORT_NAME_LEN);
   }

   node = params["interfaceParams"];
   if (!node.empty())
   {
      cv::FileNode interfaceSelect = node["interfaceSelect"];

      if (!interfaceSelect.empty())
      {
         cv::FileNode portNumberNode = node["portNumber"];
         cv::FileNode baudrateNode = node["baudrate"];
         UINT32 portNumber = 5;
         UINT32 baudrate = 921600;

         initParams->interfaceSelect = (int)interfaceSelect;

         if (!portNumberNode.empty())
         {
            portNumber = (int)portNumberNode;
         }

         if (!baudrateNode.empty())
         {
            baudrate = (int)baudrateNode;
         }

         if (initParams->interfaceSelect == INU_DEVICE_INTERFACE_0_UART)
         {
            initParams->uartIfParams.portNumber = (int)portNumber;
            initParams->uartIfParams.baudrate = (int)baudrate;
         }
         else if (initParams->interfaceSelect == INU_DEVICE_INTERFACE_0_CDC)
         {
            initParams->cdcIfParams.portNumber = (int)portNumber;
            initParams->cdcIfParams.baudrate = (int)baudrate;
         }
         else if (initParams->interfaceSelect == INU_DEVICE_INTERFACE_0_TCP)
         {
            strcpy(initParams->tcpIfParams.targetIp, "192.168.10.5");
            strcpy(initParams->tcpIfParams.hostIp, "192.168.10.6");
            initParams->tcpIfParams.port = (int)portNumber;
         }

      }
   }

   // Always Create the main configuration "main" as the first configuration
   findOrAllocConfigInTable("main");

   node = params["configurations"];
   if (!node.empty())
   {
      for (int i = 0; i < node.size() && i < SANDBOX_MAX_NUM_CONFIGS; i++)
      {
         SANDBOX_configParams* configP;
         cv::FileNode configParam;
         std::string name;

         configParam = node[i]["name"];
         if (!configParam.empty() && !configParam.string().empty())
            name = std::string(configParam.string());
         else
            name = std::string("main");

         configP = findOrAllocConfigInTable(name.c_str());
         if (!configP)
         {
            assert(0);
         }

         configParam = node[i]["hwXmlFile"];
         if (!configParam.empty() && !configParam.string().empty())
            configP->fwlibConfigParams.hwXmlFile = strdup(configParam.string().c_str());

         configParam = node[i]["swXmlFile"];
         if (!configParam.empty() && !configParam.string().empty())
            configP->fwlibConfigParams.swXmlFile = strdup(configParam.string().c_str());

         configParam = node[i]["channels"];
         if (!configParam.empty())
            process_file_channels(&configParam, configP);

         configParam = node[i]["sensorsFrameRate"];
         if (!configParam.empty())
         {
            for (int i = 0; i < SANDBOX_NUM_SENSORS; i++)
            {
               configP->fwlibConfigParams.fps[i] = (int)configParam;
            }
         }

         configParam = node[i]["sensorsResMode"];
         if (!configParam.empty())
         {
            for (int i = 0; i < SANDBOX_NUM_SENSORS; i++)
            {
               configP->fwlibConfigParams.mode[i] = (int)configParam;
            }
         }

         configParam = node[i]["sensors"];
         if (!configParam.empty())
            process_file_sensors(&configParam, configP);

         configParam = node[i]["streams"];
         if (!configParam.empty())
            process_file_streams(&configParam, configP);

         configParam = node[i]["networks"];
         if (!configParam.empty())
             process_file_networks(&configParam, configP, initParams);

         configParam = node[i]["backgrounds"];
         if (!configParam.empty())
             process_file_backgrounds(&configParam, configP, initParams);

         configParam = node[i]["injectionTest"];
         if (!configParam.empty())
         {
            if (configParam.string() == "IR1")
            {
               configP->fwlibConfigParams.injectionMode = (int)1;
            }
            else if (configParam.string() == "IR2")
            {
               configP->fwlibConfigParams.injectionMode = (int)2;
            }
            else if (configParam.string() == "FE1")
            {
               configP->fwlibConfigParams.injectionMode = (int)3;
            }
            else if (configParam.string() == "RGB1")
            {
                configP->fwlibConfigParams.injectionMode = (int)4;
            }
            else if (configParam.string() == "DPEPP")
            {
                configP->fwlibConfigParams.injectionMode = (int)6;
            }
         }

         configParam = node[i]["HwFunctions"];
         if (!configParam.empty())
         {
            if (configParam.size() <= SANDBOX_MAX_NUM_HW_TESTS)
            {
               for (int i = 0; i < configParam.size(); i++)
               {
                  if (configParam[i].string() != "")
                  {
                     strncpy(configP->hwFuncTestsParams[configP->numHwFunctionTests++].hw_functions_name,
                        configParam[i].string().c_str(),
                        MAX_NAME_LEN);
                  }
               }
            }
            else
            {
               assert(0);
            }
         }

         configParam = node[i]["dpeConfigFiles"];
         if (!configParam.empty())
         {
            if (configParam.size() <= SANDBOX_MAX_DPE_PRESET_FILE)
            {
               for (int i = 0; i < configParam.size(); i++)
               {
                  strcpy(configP->fwlibConfigParams.dpeConfigFiles[configP->fwlibConfigParams.dpeConfigNumFiles], configParam[i].string().c_str());
                  configP->fwlibConfigParams.dpeConfigNumFiles++;
               }
            }
            else
            {
               assert(0);
            }
         }
      }
   }

   node = params["bindNodes"];
   if (!node.empty())
   {
      for (int i = 0; i < node.size() && i < SANDBOX_MAX_NUM_CONFIGS; i++)
      {
         cv::FileNode configName[] = { node[i]["configName1"], node[i]["configName2"] };
         cv::FileNode nodeName[] =   { node[i]["nodeName1"],   node[i]["nodeName2"] };
         if (configName[0].empty() || configName[1].empty() || nodeName[0].empty() || nodeName[1].empty())
            assert(0);

         strncpy(bindNodesParams[numBinds].config_name1, configName[0].string().c_str(), NUCFG_MAX_CONFIG_NAME);
         strncpy(bindNodesParams[numBinds].node_name1, nodeName[0].string().c_str(), SANDBOX_MAX_LEN_FILE_NAME);
         strncpy(bindNodesParams[numBinds].config_name2, configName[1].string().c_str(), NUCFG_MAX_CONFIG_NAME);
         strncpy(bindNodesParams[numBinds].node_name2, nodeName[1].string().c_str(), SANDBOX_MAX_LEN_FILE_NAME);
         numBinds++;
      }
   }
   return 0;
}
#endif

#if (defined _WIN32) || (defined _WIN64)
#define PORT 12345
#define BUFFER_SIZE (16 * 1024 * 1024)

#define FRAME_MAGIC (0x22aa)

struct domFrameHeader {
    uint16_t    frame_magic;
    uint16_t    frame_type;
    uint16_t    frame_width;
    uint16_t    frame_height;
    uint32_t    frame_size;
    uint8_t     buffer[0];
};

int receive_sized_bytes(int sock, char* buffer, int len)
{
    int recved = 0;
    int ret;

    while (recved < len) {
        ret = recv(sock, &buffer[recved], len - recved, 0);
        if (ret <= 0) {
            break;
        }
        recved += ret;
    }

    return recved;
}

#define SAFE_RECV(sock, buf, size, res, e)      \
do {                                            \
    res = receive_sized_bytes(sock, buf, size); \
    if (res < size)                             \
        return (e);                             \
} while (0)

int client_receive_full_frame(int sock, char* buffer, int len)
{
    int recved = 0;
    int ret;
    int headersize = sizeof(domFrameHeader);
    char b[16];
    domFrameHeader* pframe = (domFrameHeader*)buffer;

    //get frame header
    while (true) {
        SAFE_RECV(sock, b, 2, ret, 0);
        //printf("get %d byte: %02x %02x to local %04x\n", ret, b[0], b[1], ntohs(*(uint16_t *)b));
        if (ntohs(*(uint16_t*)b) == FRAME_MAGIC) {
            //printf("frame sycned\n");
            SAFE_RECV(sock, &b[2], 10, ret, 0);
            pframe->frame_magic = FRAME_MAGIC;
            pframe->frame_type = ntohs(*(uint16_t*)&b[2]);
            pframe->frame_width = ntohs(*(uint16_t*)&b[4]);
            pframe->frame_height = ntohs(*(uint16_t*)&b[6]);
            pframe->frame_size = ntohl(*(uint32_t*)&b[8]);

            //printf("get frame header: type %d, res %dx%d, size %d\n",
            //        pframe->frame_type, pframe->frame_width, pframe->frame_height, pframe->frame_size);
            break;
        }
        else {
            //printf("first 2 bytes %04x\n", ntohs(*(uint16_t*)b));
            return -1;
        }
    }
    //get the frame data
    recved = headersize;
    if (len < headersize + pframe->frame_size)
        return recved;

    len = headersize + pframe->frame_size;
    while (recved < len) {
        SAFE_RECV(sock, &buffer[recved], len - recved, ret, 0);
        recved += ret;
    }
    //printf("!!!!!!  get frame : type %d, res %dx%d, size %d received bytes %d\n",
    //    pframe->frame_type, pframe->frame_width, pframe->frame_height, pframe->frame_size, recved);
    return recved;
}

int client_connect_to_server(char* ip, int port, uint32_t timeout_ms)
{
    int clientSocket, retval;
    struct sockaddr_in serverAddr;
    uint32_t time = 0;

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0) {
        int erroroor = WSAGetLastError();
        printf("[-]failed to create Client Socket - %d.\n", erroroor);
        return -1;
    }
    printf("[+]Client Socket Created Sucessfully.\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    int retry = 100 /** 1000*/;
    while (time < timeout_ms) {
        if (0 == connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) {
            printf("[+]Connected to Server.\n");
            return clientSocket;
        }
        Sleep(retry);
        time += retry;
    }

    printf("[-]Connected to Server timeout.\n");
    closesocket(clientSocket);

    return -1;
}


void startTuningImageClient(void)
{
    FILE* fp = NULL;
    int clientSocket;
    char serverIP[] = { "192.168.10.5" };
    char* buffer;
    int index = 0;
    char fname[128];
    int printres = 0;
    int recved;

    clientSocket = client_connect_to_server(serverIP, PORT, 1000000000);
    if (clientSocket < 0) {
        printf("service %s:%d is not available\n", serverIP, PORT);
        return;
    }

    printf("[+]Connected to Server.\n");

    buffer = (char*)malloc(BUFFER_SIZE);

    domFrameHeader* pframe = (domFrameHeader*)buffer;
    while (true) {
        recved = client_receive_full_frame(clientSocket, buffer, BUFFER_SIZE);
        if (recved < 0)
            printf("error occured\n");
        if ((uint32_t)recved == (pframe->frame_size + sizeof(domFrameHeader)))
        {
            show_tuning_server_stream(pframe->buffer, pframe->frame_width, pframe->frame_height, pframe->frame_type);
        }
        else
        {
            printf("broken packet\n");
        }
    }

    return;
}
#endif

void set_defaults()
{
   UINT32     sensorIndex,ind,j;
   memset(&initParams,   0, sizeof(initParams));
   memset(&configParams, 0, sizeof(configParams));
   for (ind = 0; ind < SANDBOX_MAX_NUM_CONFIGS; ind++)
   {
      for (j = 0; j < SANDBOX_NUM_HW_CHANNELS; j++)
      {
         configParams[ind].fwlibConfigParams.cscMode[j] = -1;
      }
      for (sensorIndex = 0; sensorIndex < SANDBOX_NUM_SENSORS; sensorIndex++)
      {
         configParams[ind].fwlibConfigParams.mode[sensorIndex] = FWLIB_SENSOR_MODE_FULL_E;
      }
      configParams[ind].fwlibConfigParams.dpeConfigNumFiles = 0;
   }
   initParams.watchdogTimeout = 3;
   initParams.bootid = -1;
   initParams.useUartSync = 0;
   initParams.interfaceSelect = INU_DEVICE_INTERFACE_0_USB;

   post_process_init();
}

ERRG_codeE activateChannels(inu_deviceH deviceH, SANDBOX_configParams *paramsP)
{
   ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;
   UINT32 testIndex;
   SANDBOX_channelParamNodeT *channelParamNode;

   channelParamNode = paramsP->channelParamNodes;

   while (channelParamNode)
   {
      FWLib_cfgChannelCbsT chCfgCbs;
      get_callbacks(&paramsP->fwlibConfigParams, &chCfgCbs, channelParamNode->channelParam->stream_out_name);
      chCfgCbs.cookie = channelParamNode->channelParam;
      status = FwLib_activate_channel(deviceH, &paramsP->fwlibConfigParams, channelParamNode->channelParam->stream_out_name, &chCfgCbs, &channelParamNode->channelParam->chH);
      channelParamNode = channelParamNode->next;
   }

   for (testIndex = 0; testIndex < paramsP->numHwFunctionTests; testIndex++)
   {
      status = FwLib_activate_hw(deviceH, &paramsP->fwlibConfigParams, paramsP->hwFuncTestsParams[testIndex].hw_functions_name, &paramsP->hwFuncTestsParams[testIndex].chH);
      if (ERRG_FAILED(status))
         SANDBOX_ASSERT(status);
   }

   return status;
}

ERRG_codeE stopChannels(inu_deviceH deviceH, SANDBOX_configParams *paramsP)
{
   ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;
   UINT32 testIndex;
   SANDBOX_channelParamNodeT *channelParamNode;

   for (testIndex = 0; testIndex < paramsP->numHwFunctionTests; testIndex++)
   {
      status = FwLib_stop_hw(deviceH, paramsP->hwFuncTestsParams[testIndex].chH);
      if (ERRG_FAILED(status))
         SANDBOX_ASSERT(status);
   }

   channelParamNode = paramsP->channelParamNodes;
   while (channelParamNode)
   {
      status = FwLib_stop_channel(deviceH, channelParamNode->channelParam->chH);
      channelParamNode = channelParamNode->next;
   }

   return status;
}

void freeChannels(inu_deviceH deviceH, SANDBOX_configParams *paramsP)
{
   SANDBOX_channelParamNodeT *channelParamNode;
   (void)(deviceH); //To fix unused params warning.

   channelParamNode = paramsP->channelParamNodes;
   while (channelParamNode)
   {
      free(channelParamNode->channelParam);
      channelParamNode->channelParam = NULL;
      channelParamNode = channelParamNode->next;
   }
}

void sandboxDeinit(inu_deviceH deviceH)
{
   int i;

   for (i = 0; i < SANDBOX_MAX_NUM_CONFIGS; i++)
   {
      if (configParams[i].fwlibConfigParams.name)
      {
         freeChannels(deviceH, &configParams[i]);
      }
   }

}

int main(int argc, char* argv[])
{
   inu_deviceH deviceH = NULL;
   const inu_device__version *verionP;
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   UINT32 i, j;
#ifdef DEBUG_THREAD_ANALYSIS
    marker_series series;
    span* Span = new span(series, 1, _T("Boot"));
#endif

   set_defaults();

#if defined _OPENCV_
   if (argc == 3 && (!strcmp(argv[1], "loadparams") || !strcmp(argv[1], "lp")))
       process_file(argv[2], &initParams);
   else
#endif
       process_options(argc, argv);
   inu_useLongerTimeout(sandBoxUseLongerTimeout);

   ret = FwLib_lib_init(&initParams);
   if (ERRG_FAILED(ret))
      assert(0);

#ifdef DEBUG_THREAD_ANALYSIS
    series.write_flag(_T("FwLib_lib_init done"));
#endif

   if (cmdListener)
   {
      createRuntimeCmdsListener(&deviceH);
   }

   if (!initParams.standalone)
   {
      // currently entering suspend/power down is supported only in standalone mode.
      // In case we are not in stadndalone mode, it will not be allowed even if the user specified that.
      initParams.allowPowerDown = FALSE;
   }

   /**************************************************************************************************
     cycles of sandbox can be in regular mode (user sets number of iterations and duration time),
     or in power managment mode (infinite cycles of power downs and wake-ups):

                          regular mode ('iterations' times)   /   power down mode (infinite loop)
                          ---------------------------------       -------------------------------
     init + config:                   full                       partial (full at first time only)
     operation time:             sleep ('duration' sec)              wait for power down
     deinit + deconfig:               full                                 partial
     next iteration:                imediate                          wait for wake up
   **************************************************************************************************/
   while (iterations || initParams.allowPowerDown)
   {
      ret = FwLib_boot(initParams.bootid);
      if (ERRG_FAILED(ret))
         SANDBOX_ASSERT(ret);

#ifdef DEBUG_THREAD_ANALYSIS
        series.write_flag(_T("FwLib_boot done"));
#endif
      ret = FwLib_init(&deviceH, &verionP);
      if (ERRG_FAILED(ret))
         SANDBOX_ASSERT(ret);
#ifdef DEBUG_THREAD_ANALYSIS
        series.write_flag(_T("FwLib_init done"));
        delete Span;
        Span = new span(series, 1, _T("Config"));
#endif
      for (i = 0; i < SANDBOX_MAX_NUM_CONFIGS; i++)
      {
         if (configParams[i].fwlibConfigParams.name)
         {
            configParams[i].fwlibConfigParams.cfgId = i;
#if 0
            if (extInterleaveMode)
            {
                configParams[i].fwlibConfigParams.extInterleaveIdx = extInterleaveModeIdx;
                printf("*************fwlibConfigParams.extInterleaveIdx: %d**************\n");
                for (int j = 0; j < extInterleaveModeIdx; j++)
                {
                    configParams[i].fwlibConfigParams.extInterleaveChannels[j] = extInterleaveChannels[j];
                    printf("EXT DMA Interleave channel: %d\n", configParams[i].fwlibConfigParams.extInterleaveChannels[j]);
                }
            }
#endif
            ret = FwLib_config(deviceH, &configParams[i].fwlibConfigParams);
            if (ERRG_FAILED(ret))
            {
               SANDBOX_ASSERT(ret);
            }

            for (j = 0; j < configParams[i].cdnnNetworkIdList.numOfNetworks; j++)
            {
               FwLib_set_network_to_node(deviceH, configParams[i].fwlibConfigParams.configH, configParams[i].cdnnNetworkIdList.cdnnNetworkIdList[j].cdnnFunctionName, configParams[i].cdnnNetworkIdList.cdnnNetworkIdList[j].networkId);
            }
         }
      }

      if(initParams.tuningServer != -1)
      {
          ret = FwLib_enableTuningServer(deviceH,initParams.tuningServer);

#if (defined _WIN32) || (defined _WIN64)
          DWORD threadID;
          CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&startTuningImageClient, NULL, 0, &threadID);
#endif
      }
      else
      {
          for (i = 0; i < numBinds; i++)
          {
             ret = FwLib_bind_nodes_configs(&findOrAllocConfigInTable(bindNodesParams[i].config_name1)->fwlibConfigParams, bindNodesParams[i].node_name1,
                                            &findOrAllocConfigInTable(bindNodesParams[i].config_name2)->fwlibConfigParams, bindNodesParams[i].node_name2);
             if (ERRG_FAILED(ret))
                SANDBOX_ASSERT(ret);
          }
#ifdef DEBUG_THREAD_ANALYSIS
        series.write_flag(_T("config done"));
        delete Span;
        Span = new span(series, 1, _T("Stream"));
#endif

          for (i = 0; i < SANDBOX_MAX_NUM_CONFIGS; i++)
          {
             if (configParams[i].fwlibConfigParams.name)
             {
                ret = activateChannels(deviceH, &configParams[i]);
                if (ERRG_FAILED(ret))
                   SANDBOX_ASSERT(ret);
             }
          }
      }

      if (initParams.allowPowerDown)
      {
         FwLib_waitForPdState(FWLIB_PD_WAIT_FOR_PD_E);
      }
      else
      {
         if (duration == SANDBOX_DURATION_INFINITE)
         {
            while (TRUE)
            {
               millisleep(100000);
            }
         }
         else
         {
            millisleep(duration * 1000);
         }
      }

#ifdef DEBUG_THREAD_ANALYSIS
        series.write_flag(_T("Stream Stop"));
        delete Span;
#endif

      for (i = 0; i < SANDBOX_MAX_NUM_CONFIGS; i++)
      {
         if (configParams[i].fwlibConfigParams.name)
         {
            ret = stopChannels(deviceH, &configParams[i]);
            if (ERRG_FAILED(ret))
               SANDBOX_ASSERT(ret);
         }
      }

      for (i = 0; i < SANDBOX_MAX_NUM_CONFIGS; i++)
      {
         if (configParams[i].fwlibConfigParams.name)
         {
            ret = FwLib_close_config(deviceH, &configParams[i].fwlibConfigParams);
            if (ERRG_FAILED(ret))
               SANDBOX_ASSERT(ret);
         }
      }

      ret = FwLib_deinit(deviceH);
      if (ERRG_FAILED(ret))
         SANDBOX_ASSERT(ret);

      ret = FwLib_boot_deinit();
      if (ERRG_FAILED(ret))
         SANDBOX_ASSERT(ret);

      if (initParams.allowPowerDown)
      {
         FwLib_waitForPdState(FWLIB_PD_WAIT_FOR_WU_E);
      }
      else
      {
         millisleep(2000);
         iterations--;
      }
   }

   sandboxDeinit(deviceH);

   return testResult;
}
