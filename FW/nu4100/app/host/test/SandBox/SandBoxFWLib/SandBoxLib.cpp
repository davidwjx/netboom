
#include "SandBoxLib.h"

#include "inu2.h"

#if (defined _WIN32) || (defined _WIN64)
#include "windows.h"
#endif

#include <time.h>

#if (defined _WIN32) || (defined _WIN64)
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#else
#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>
#endif

#include "inu2.h"
#include "requirements.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <thread>        
#include <chrono>         
#include <ctime>
#include "inu_metadata_serializer.h"

#if !((defined _WIN32) || (defined _WIN64))
   #include <sys/time.h>
#endif

#ifdef DEBUG_THREAD_ANALYSIS
#include "cvmarkersobj.h"
    using namespace Concurrency::diagnostic;
#endif
/****************************************************************************
***************         L O C A L       D E F N I T I O N S  ***************
****************************************************************************/
//#define CHIP_VERSION_FROM_USB
//#define STREAM_CROPPED
//#define FDK_CNN_CLIENT_DEMO_STREAM
//#define FDK_CNN_DEMO_STREAM
//#define FDK_BASIC_DEMO_STREAM
//#define FDK_MATCHER_DEMO_STREAM
//#define FDK_GAUSSIAN_DEMO_INPUT_FROM_FILE
//#define FDK_GAUSSIAN_DEMO
//#define STREAM_IN_OUT
//#define FAST_ORB
//#define CDNN
//#define CDNN_INPUT_FROM_FILE
//#define DPE_PP
//#define SLAM
//#define SLAM_INPUT_FROM_FILE
//#define FAST_ORB_INPUT_FROM_FILE
//#define READ_WRITE_REGISTER
//#define READ_WRITE_BUFFER
#define INJECTION
//#define INJECTION_BUFFER
//#define INJECTION_FROM_FILES
//#define INJECTION_FROM_RAW_FILES_BIN
//#define INJECTION_FROM_RAW_FILES_FULL_FE
#define INJECTION_FROM_RAW_FILES_FULL
//#define SENSOR_CONTROL_CHANGE_ROI
//#define SENSOR_CONTROL_PARAMS_UPDATE
//#define PROJECTOR
//#define SW_RESET
//#define PWM_TRIGGER
//#define SHARED_MEM_PRINT
#if (defined _WIN32) || (defined _WIN64)
//#define ENABLE_DISPLAY
#endif
//#define ENABLE_SAVE_IMG_TO_DISK
//#define ENABLE_STREAM_PRINTS
//#define FAST_STOP_START_CHECK
//#define ENABLE_OUTPUT_FILE
//#define SECOND_NETWORK
#define CDNN_OUT_NUM 30
//#define TCP_COMMUNICATION


#ifdef READ_WRITE_BUFFER
#define READ_WRITE_BUF_SIZE 500000
#define READ_WRITE_MEM_ADDRESS 0xA0000000
#define NUM_OF_TEST_LOOP 10
#endif

#define SCL_WIDTH      (304)
#define SCL_HEIGHT     (300)
#define SCL_WIDTH_PAD  (SCL_WIDTH + 48)
#define SCL_HEIGHT_PAD (SCL_HEIGHT + 20)


#ifdef ENABLE_DISPLAY
#include <freeglut.h>
#include <glut.h>
#endif

#if (defined _WIN32) || (defined _WIN64)
typedef HANDLE THREAD_HANDLE;
#else
typedef void* THREAD_HANDLE;
#endif

typedef void (threadFunc)(void *paramsP);

typedef struct
{
   THREAD_HANDLE threadH;
   inu_deviceH   deviceH;
   inu_functionH sourceH;
   inu_streamerH streamerH;
   inu_dataH     dataH;
   int           active;
   threadFunc    *func;
   frameCallbackFunc *frameCallback;
   startCallbackFunc *startCallback;
   stopCallbackFunc  *stopCallback;
   failedFrameCallBackFunc *failedCallback;
   failedFrameCallBackFunc *recursiveStartDone;
   void *cookie;
}thread_params;

typedef struct
{
   UINT32  algType;
   UINT32  opcode;
   UINT32  networkId;
} CNN_CLIENT_DEMO_hostGpMsgStructT;

#define FAILED_PRINTF(status,ref) printf("FAILURE: 0x%x for %s(%d)\n",status,inu_ref__getUserName((inu_refH)ref),inu_ref__getRefId((inu_refH)ref));

#define ONET_TAIL_XY       (2)
#define OBJECT_MAX_NUMBER  (10)
#define FACE_LABEL_ID      (31)

inu_deviceH devG;
FWLib_chH logchH;
static SANDBOX_initParams    initParams;

// the following variables are global since they hold data that are
// saved between power cycles if used
static inu_device__version   deviceVersion;
static inu_graph__initParams graphInitParams;
static inu_nucfgH            cfgG[NUCFG_MAX_INPUTS];
static int                   powerCycleMode = 0;

/****************************************************************************
***************       L O C A L         D A T A              ***************
****************************************************************************/

typedef struct SANDBOX_channelListParamNodes
{
   FWLib_chH chH;
   struct SANDBOX_channelListParamNodes *next;
}SANDBOX_channelListParamNodesT;

SANDBOX_channelListParamNodesT *channelListParamsHead;

int slamMatches;
int slamQ;
int slamX[800];
int slamY[800];
#define SLAM_ROUTE_WINDOW_WIDTH (640)
#define SLAM_ROUTE_WINDOW_HEIGHT (480)
unsigned char slamRouteFrame[3* SLAM_ROUTE_WINDOW_WIDTH * SLAM_ROUTE_WINDOW_HEIGHT];
int face_box_width, face_box_height, face_box_min_x, face_box_min_y;

UINT32 Gl_num_of_faces_in_image = 1;
float onet_x_points[OBJECT_MAX_NUMBER][5], onet_y_points[OBJECT_MAX_NUMBER][5];
float onet_x_image_points[OBJECT_MAX_NUMBER][5], onet_y_image_points[OBJECT_MAX_NUMBER][5];

const int WIDTH = 1280;
const int HEIGHT = 960;
const float ASPECT = float(WIDTH) / HEIGHT;   // desired aspect ratio


INT32 bootid = 0;
INT32 devid = 0;
inu_function__startParamsT startParam;
inu_function__startParamsT startParam_1;
inu_function__stopParamsT stopParam;
inu_function__stopParamsT stopParam_1;

static char *copy_buf = NULL;

int streamInI = 0;
int counter = 0;
#ifdef ENABLE_OUTPUT_FILE
static FILE* fp_output = NULL;
#endif
static   int injectionActive = 0;

char* open_bmp_file(char* filename);

volatile bool g_device_operational = false;
volatile bool g_device_boot = false;
volatile bool g_device_chipVersion = false;
volatile inu_device_interface_0_e g_if_select;
//extern UINT32 streamNum;
UINT32 streamNum = 0;

/****************************************************************************
***************            L O C A L    T Y P E D E F S      ***************
****************************************************************************/
#ifdef _DEBUG
#define SANDBOX_TEST_RET(ret) SANDBOX_TEST_RET_ASSERT(ret);
#else
#define SANDBOX_TEST_RET(ret) SANDBOX_TEST_RET_RETURN(ret);
#endif

#define SANDBOX_TEST_RET_ASSERT(ret) if (ERRG_FAILED(ret)) assert(0);
#define SANDBOX_TEST_RET_RETURN(ret) if (ERRG_FAILED(ret)) return ret;


/****************************************************************************
***************      L O C A L       F U N C T I O N S       ***************
****************************************************************************/
static void *sharedMemMallocFunc(unsigned int size, void *refPtr)
{
   unsigned int *ptr;
   ptr = (unsigned int*)malloc(size);
#ifdef SHARED_MEM_PRINT
   inu_streamer_poolCfg poolCfg;
   inu_streamer__getPoolCfg(refPtr, &poolCfg);
   printf("++++++++++++++++++malloc shared mem ptr %llx refPtr %llx refId %d buffers %d hdrSize %d buffSize %d\n", ptr, refPtr,
      poolCfg.refId,
      poolCfg.numBuffers,
      poolCfg.hdrSize,
      poolCfg.bufferSize);
#endif
  return ptr;
}

static void sharedMmemFreeFunc(void *ptr,void *refPtr)
{
#ifdef SHARED_MEM_PRINT
   inu_streamer_poolCfg poolCfg;
   inu_streamer__getPoolCfg(refPtr,&poolCfg);
   printf("-------------------free shared mem shared ptr %llx ref %llx refId %d\n",ptr, refPtr, poolCfg.refId);
#endif
   return free(ptr);
}

ERRG_codeE read_cdnn_network_from_files(inu_load_networkH loadNetworkH);

int millisleep_host(unsigned ms)
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

char* open_bmp_file(char* filename)
{
    int i;
    FILE* bmp = fopen(filename, "rb");
    unsigned char info[54 + 1024];
    unsigned char tmp_line[1280];
    fread(info, sizeof(unsigned char), (54 + 1024), bmp); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];
    int bpp = ((*(int*)&info[26]) >> 16);
    int size = (bpp >> 3) * width * height;
    //if first frame, allocate buffer
    if (copy_buf == NULL)
    {
        copy_buf = (char*)malloc(size);
    }
    printf("width %d, height %d, bpp(bits) %d --> buff %d\n", width, height, bpp, size);

    fread((void*)copy_buf, sizeof(unsigned char), size, bmp); // read the rest of the data at once
    fclose(bmp);

    for (i = 0; i < height / 2; i++)
    {

        memcpy(tmp_line, copy_buf + i * width, width);
        memcpy(copy_buf + i * width, copy_buf + (height - i - 1) * width, width);
        memcpy(copy_buf + (height - i - 1) * width, tmp_line, width);
    }


    return copy_buf;
}

char *open_raw_file(char* filename, char *bufP, int size)
{
    FILE* raw = fopen(filename, "rb");
    fread((void*)(bufP), sizeof(unsigned char), size, raw);
    fclose(raw);
    return bufP;
}

#if defined FAST_ORB || defined (FAST_ORB_INPUT_FROM_FILE)
static void inu_fast_orb__printFeaturesOnScreen(UINT16* p_keypoint_x, UINT16* p_keypoint_y, UINT16* pFrame, UINT32 frameStride, UINT32 count)
{
   UINT32 i;
   INT16 x0;
   INT16 y0;

   for (i = 0; i < count; i++)
   {
      x0 = p_keypoint_x[i];
      y0 = p_keypoint_y[i];
      pFrame[y0*(INT16)frameStride + x0] = 0xFFFF;
      pFrame[y0*(INT16)frameStride + x0 + 1] = 0xFFFF;
      pFrame[(y0 + 1)*(INT16)frameStride + x0] = 0xFFFF;
      pFrame[(y0 + 1)*(INT16)frameStride + x0 + 1] = 0xFFFF;
   }
}

void fast_orb(inu_imageH imageFastOrbH)
{
   ERRG_codeE status;
   inu_fast_orb_dataH data;
   void* dataP;

   //read from data object
   status = inu_streamer__read(fastOrbStreamerH, &data);
   if (ERRG_SUCCEEDED(status))
   {
      dataP = inu_data__getMemPtr(data);
      inu_fast_orb_data__hdr_t *dataHdr = (inu_fast_orb_data__hdr_t *)inu_data__getHdr(data);

      UINT64 virtAddressX = (UINT64)dataP;
      UINT32 yOffset = dataHdr->yOffset;
      UINT32 scoreOffset = dataHdr->scoreOffset;
      UINT32 descOffset = dataHdr->descOffset;
      UINT16 *resultsXP = (UINT16*)dataP;
      UINT16 *resultsYP = (UINT16*)(virtAddressX + yOffset);
      UINT16 *resultsScoreP = (UINT16*)(virtAddressX + scoreOffset);
      UINT32 *desc = (UINT32*)(virtAddressX + descOffset);

#ifdef ENABLE_OUTPUT_FILE
      //fprintf(fp_output, "Frame: %d\n", frameNum + 1);
      fprintf(fp_output, "Number of points: %u\n", dataHdr->numKeyPoints);
      for (int i = 0; i < dataHdr->numKeyPoints; i++)
      {
         UINT32 *d = &desc[8 * i];
         fprintf(fp_output, "[%4u, %4u, %4u]    ", resultsXP[i], resultsYP[i], resultsScoreP[i]);
         fprintf(fp_output, "[%12x, %12x, %12x, %12x, %12x, %12x, %12x, %12x]\n", d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
      }
#endif // ENABLE_OUTPUT_FILE

#ifdef ENABLE_DISPLAY
      inu_image__hdr_t* imgHdrP = (inu_image__hdr_t*)inu_data__getHdr(imageFastOrbH);
      unsigned short* imgDataP = (unsigned short*)inu_data__getMemPtr(imageFastOrbH);
      UINT32 width;

      inu_fast_orb__printFeaturesOnScreen(resultsXP, resultsYP, imgDataP, 640, dataHdr->numKeyPoints);

      if (imgHdrP->imgDescriptor.format != NUCFG_FORMAT_RAW8_E)
      {
         width = imgHdrP->imgDescriptor.width;
      }
      else
      {
         width = imgHdrP->imgDescriptor.width / 2;
      }
      StreamDisplay(width, imgHdrP->imgDescriptor.height, imgDataP, GL_LUMINANCE, GL_UNSIGNED_SHORT, 0); //display as BGRA
#endif
      inu_streamer__free(fastOrbStreamerH, data);
   }
   //inu_function__stop(fastOrbStreamerH, &stopParam);
}
#endif


static ERRG_codeE recursiveStopFunctions(inu_functionH functionH)
{
   inu_nodeH inputNode;
   ERRG_codeE status;

   if (INU_REF__IS_FUNCTION_TYPE(inu_ref__getRefType(functionH)))
   {
      status = inu_function__stop(functionH, &stopParam);
      SANDBOX_TEST_RET(status);
   }

   inputNode = inu_node__getNextInputNode(functionH, NULL);
   while (inputNode)
   {
      status = recursiveStopFunctions(inputNode);
      inputNode = inu_node__getNextInputNode(functionH, inputNode);
   }
   return status;
}

static ERRG_codeE recursiveStartFunctions(inu_functionH functionH)
{
   inu_nodeH inputNode;
   ERRG_codeE status;

   if (INU_REF__IS_FUNCTION_TYPE(inu_ref__getRefType(functionH)))
   {
      status = inu_function__start(functionH, &startParam);
      SANDBOX_TEST_RET(status);
   }

   inputNode = inu_node__getNextInputNode(functionH, NULL);
   while (inputNode)
   {
      status = recursiveStartFunctions(inputNode);
      inputNode = inu_node__getNextInputNode(functionH, inputNode);
   }
   return status;
}

/****************************************************************************
inu_load_network_cdnnIdE network_id;            //  network id
inu_load_network_cdnnEngineTypeE engineType;    //  engineType: 0-ceva 1-ev61
CDNNG_inputImageRawScaleE rawScale;             //  raw scale: 0-pixel range 0->1, 1-pixel range 0->255
CDNNG_channelSwapE channelSwap;                 //  rgb channel order: 0-rgb order, 1-bgr order
CDNNG_resultsFormatE resultsFormat;             //  results format: 0-fixed point, 1-floating point
CDNNG_netType netType;                          //  is yolo or other net
CDNNG_bitAccuracyE bitAccuracy;                 //  results format: 0-fixed point, 1-floating point
UINT32 ddrWorkspaceSize;                        //  the required external memory space for current net
UINT32 internalworkspaceSize;                   //  the required internal memory space for current net
CDNNG_version_E version;                        //  cdnn version
****************************************************************************/
static inline ERRG_codeE loadNetwork(   inu_load_networkH loadNetworkH,
                                        char filebin[],
                                        inu_load_network_cdnnIdE networkId,
                                        inu_load_network_cdnnEngineTypeE engineType,
                                        CDNNG_inputImageRawScaleE rawScale,
                                        CDNNG_channelSwapE channelSwap,
                                        CDNNG_resultsFormatE resultsFormat,
                                        CDNNG_netType netType,
                                        CDNNG_bitAccuracyE bitAccuracy,
                                        UINT32 ddrWorkspaceSize,
                                        UINT32 internalworkspaceSize,
                                        UINT32 pipeDepth,
                                        UINT32 ProcesssingFlags,
                                        CDNNG_version_E version)
{
   ERRG_codeE ret;
   inu_load_network_memory_t networkfile;
   inu_load_network_hdr_t networkHdr;

   {
      if (readBinFile(filebin, (inu_device_memory_t *)&networkfile) < 0)
      {
         return (ERRG_codeE)ERR_UNEXPECTED;
      }
       //{
          // UINT32 i;
          // UINT64 checksum = 0;
          // UINT8 *qdata_ddr_addr;

          // qdata_ddr_addr = (UINT8*)networkfile.bufP;
          // for (i = 0; i < networkfile.bufsize; i++) {
             //  checksum += qdata_ddr_addr[i];
          // }
          // printf("CDNN_LOAD_NETWORK: checksum %llu\n", checksum);
       //}
   }

   networkfile.path = (char*)malloc(256);
   strcpy(networkfile.path, filebin);

   networkHdr.networkId = networkId;
   networkHdr.engineType = engineType;
   networkHdr.rawScale = rawScale;
   networkHdr.channelSwap = channelSwap;
   networkHdr.resultsFormat = resultsFormat;
   networkHdr.netType = netType;
   networkHdr.bitAccuracy = bitAccuracy;
   networkHdr.ddrWorkspaceSize = ddrWorkspaceSize;
   networkHdr.internalworkspaceSize = internalworkspaceSize;
   networkHdr.version = version;
   networkHdr.pipeDepth = pipeDepth;
   networkHdr.ProcesssingFlags = ProcesssingFlags;
   ret = inu_load_network__loadNetwork(loadNetworkH, &networkfile, &networkHdr);
   free(networkfile.bufP);
   free(networkfile.path);
   return ret;
}

ERRG_codeE read_cdnn_network_from_files(inu_load_networkH loadNetworkH)
{
   ERRG_codeE ret;
   char filebin[500];
   int netword_id;

   for (netword_id = 0; netword_id < INU_LOAD_NETWORK__IAE_TOTAL_CDNN_E; netword_id++)
   {
      if (initParams.inu_load_network_params[netword_id].is_active)
      {
         memset(filebin, 0, sizeof(filebin));
         strcpy(filebin, get_installation_path());
         strcat(filebin, PATH_SEPARATOR"config");
         strcat(filebin, PATH_SEPARATOR"AI4100");
         strcat(filebin, PATH_SEPARATOR"cnn_bins");
         strcat(filebin, PATH_SEPARATOR);
         strcat(filebin, initParams.inu_load_network_params[netword_id].bin_name);
         ret = loadNetwork(loadNetworkH,
                           filebin,
                           initParams.inu_load_network_params[netword_id].networkId,
                           INU_LOAD_NETWORK__EV61_E,
                           CDNNG_INPUT_IMAGE_RAW_SCALE_256,
                           initParams.inu_load_network_params[netword_id].channelSwap,
                           CDNNG_FLOATING_POINT_QUERY,
                           CDNNG_OTHER_CNN,
                           CDNNG_BIT_ACCURACY_16BIT,
                           70000000,
                           100000,
                           initParams.inu_load_network_params[netword_id].pipeDepth,
                           initParams.inu_load_network_params[netword_id].ProcesssingFlags,
                           CDNNG_VERSION_2018_3);
      }
   }
   SANDBOX_TEST_RET(ret);
   return ret;
}

static inline ERRG_codeE loadBackground(inu_load_networkH loadBackgroundH,
    char filebin[],
    int backgroundId)
{
    ERRG_codeE ret;
    inu_load_background_memory_t backgroundfile;
    inu_load_background_hdr_t backgroundHdr;

    if (readBinFile(filebin, (inu_device_memory_t*)&backgroundfile) < 0)
    {
        return (ERRG_codeE)ERR_UNEXPECTED;
    }

    backgroundHdr.backgroundId = backgroundId;
    ret = inu_load_background__loadBackground(loadBackgroundH, &backgroundfile, &backgroundHdr);
    free(backgroundfile.bufP);
    return ret;
}

ERRG_codeE read_vision_proc_background_from_files(inu_load_backgroundH loadBackgroundH)
{
    ERRG_codeE ret;
    char filebin[500];

    memset(filebin, 0, sizeof(filebin));
    strcpy(filebin, get_installation_path());
    strcat(filebin, PATH_SEPARATOR"config");
    strcat(filebin, PATH_SEPARATOR"background");
    strcat(filebin, PATH_SEPARATOR);
    strcat(filebin, initParams.inu_load_background_params.bin_name);
    ret = loadBackground(loadBackgroundH, filebin, initParams.inu_load_background_params.backgroundId);

    SANDBOX_TEST_RET(ret);
    return ret;
}

/* handle_onet_result */
void handle_onet_result(inu_cdnn_data__hdr_t *cdnn_msg, short *blob_p)
{
   unsigned int blob_ind;
   UINT32 face_ind;
   float scale;
   static int order_of_tails[3] = { 0, 1, 2 };
   static bool first_parse = true;
   int index, tail_index, next_tail_index;

   int onet_tail_xy;

   /* On first time, Determine tails order - They are in incrementing order in size */
   if (first_parse) {
      first_parse = false;
      index = 0;
      while (index < 2) {
         tail_index = order_of_tails[index];
         next_tail_index = order_of_tails[index + 1];
         if (cdnn_msg->tailHeader[next_tail_index].elementCount < cdnn_msg->tailHeader[tail_index].elementCount) {
            int tmp = order_of_tails[index];
            order_of_tails[index] = order_of_tails[index + 1];
            order_of_tails[index + 1] = tmp;
            index = 0;
         }
         else {
            index++;
         }
      }
   }

   //onet_tail_score = order_of_tails[0];
   //onet_tail_mv = order_of_tails[1];
   onet_tail_xy = order_of_tails[2];

   //printf("handle_onet_result for Gl_num_of_faces_in_image:%d\n", Gl_num_of_faces_in_image);

   if (cdnn_msg->numOfTails != Gl_num_of_faces_in_image * 3)
   {
      printf("handle_onet_result: Illegal number of tail: %d  totSizeFromCeva:%d\n", cdnn_msg->numOfTails, cdnn_msg->totOutputSize);
      return;
   }

   for (face_ind = 0; face_ind < Gl_num_of_faces_in_image; face_ind++)
   {

      blob_ind = cdnn_msg->offsetToBlob[onet_tail_xy] / sizeof(short) + face_ind * 16;

      scale = (float)cdnn_msg->tailHeader[onet_tail_xy + face_ind * 3].cnnFields.tailSynopsysFields.scale;

      for (int i = 0; i < 5; i++)
      {
         onet_x_points[face_ind][i] = (float)blob_p[blob_ind + i] / scale;
         onet_y_points[face_ind][i] = (float)blob_p[blob_ind + i + 5] / scale;
         //printf("X: %f %d scale:%f\n", onet_x_points[face_ind][i], blob_p[blob_ind + i], scale);
         //printf("Y: %f %d scale:%f\n", onet_y_points[face_ind][i], blob_p[blob_ind + i + 5], scale);
      }

      /* Convert the face points to the image scale */
      for (int j = 0; j < 5; j++)
      {
         onet_x_image_points[face_ind][j] = onet_x_points[face_ind][j] * face_box_width + face_box_min_x;
         onet_y_image_points[face_ind][j] = onet_y_points[face_ind][j] * face_box_height + face_box_min_y;
         //printf("X:%d Y:%d\n", (int)onet_x_image_points[face_ind][j], (int)onet_y_image_points[face_ind][j]);
      }

   }
   if (Gl_num_of_faces_in_image == 0)
   {
      printf("*** handle_onet_result - Got unexpected Onet output ***\n");
   }
}

inu_load_networkH loadNetworkH = NULL;
ERRG_codeE load_networks(inu_deviceH deviceH, UINT32 load)
{
   ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;
   inu_load_network__initParams loadNetworkInitParams;
   if (load)
   {
      if (!loadNetworkH)
      {
         loadNetworkInitParams.deviceH = deviceH;
         loadNetworkInitParams.initWorkSpace = 1;
         status = inu_load_network__new(&loadNetworkH, &loadNetworkInitParams);
      }
      SANDBOX_TEST_RET(status);
      status = read_cdnn_network_from_files(loadNetworkH);
   }
   else
   {
      if (loadNetworkH)
      {
         inu_load_network__releaseAll(loadNetworkH);
         inu_load_network__delete(loadNetworkH);
         loadNetworkH = NULL;
      }
   }
   return status;
}

inu_load_backgroundH loadBackgroundH = NULL;
ERRG_codeE load_background(inu_deviceH deviceH, UINT32 load)
{
    ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;
    inu_load_background__initParams loadBackgroundInitParams;
    if (load)
    {
        if (!loadBackgroundH)
        {
            loadBackgroundInitParams.deviceH = deviceH;
            status = inu_load_background__new(&loadBackgroundH, &loadBackgroundInitParams);
        }
        SANDBOX_TEST_RET(status);
        status = read_vision_proc_background_from_files(loadBackgroundH);
    }
    else
    {
        if (loadBackgroundH)
        {
            inu_load_background__release(loadBackgroundH);
            inu_load_background__delete(loadBackgroundH);
            loadBackgroundH = NULL;
        }
    }
    return status;
}

ERRG_codeE FwLib_set_network_to_node(inu_deviceH deviceH, void *configH, char *cdnn_func_name, UINT32 network_id)
{
   ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;
   inu_cdnn__networkIdSetT networkIdParamP;
   inu_graphH graphH = (inu_graphH)configH;
   inu_cdnnH cdnnH;

   status = inu_graph__findNode(graphH, cdnn_func_name, &cdnnH);
   SANDBOX_TEST_RET(status);

   networkIdParamP.network_id = (inu_load_network_cdnnIdE)network_id;
   status = inu_cdnn__setNetworkId(cdnnH, &networkIdParamP);
   return status;
}

void FwLib_reload_lut_files(inu_deviceH deviceH, char *calib_path)
{
   inu_device__reloadLutFiles(deviceH, calib_path);
}

#ifdef SLAM
#include "types.hpp"
#include "opencv2/imgproc.hpp"
#include <vector>
#include <array>
using namespace std;
#define DRAW_CUBE
void DrawCube(cv::Mat &image);
#endif

int frame_cnt = 0;

#if defined FDK_CNN_DEMO_STREAM ||defined FDK_CNN_CLIENT_DEMO_STREAM
void use_case_fdk_cnn_demo(inu_deviceH deviceH, inu_graphH graphH)
{
   ERRG_codeE status;
   inu_fdkH fdkH;
   status = inu_graph__findNode(graphH, "FDK_1", &fdkH);
   SANDBOX_TEST_RET(status);

   status = inu_function__start(fdkH, &startParam);
   SANDBOX_TEST_RET(status);
}
#endif

#ifdef CDNN_INPUT_FROM_FILE
void load_networks_from_file(inu_deviceH deviceH, inu_graphH graphH)
{
   ERRG_codeE status;
   inu_streamerH streamerInCdnnH;
   status = inu_graph__findNode(graphH, "cnn_inject_stream_in_func", &streamerInCdnnH);
   SANDBOX_TEST_RET(status);

   inu_imageH imageCdnnH;
   status = inu_graph__findNode(graphH, "cnn_inject_data_in", &imageCdnnH);
   SANDBOX_TEST_RET(status);

   inu_cdnnH cdnnH;
   status = inu_graph__findNode(graphH, "cnn_inject_func", &cdnnH);
   SANDBOX_TEST_RET(status);

   inu_cdnn_dataH cdnnDataH;
   status = inu_graph__findNode(graphH, "cnn_inject_data", &cdnnDataH);
   SANDBOX_TEST_RET(status);

   status = inu_function__start(streamerInCdnnH, &startParam);
   SANDBOX_TEST_RET(status);

   inu_image__hdr_t imgHdr;
   UINT32 size;
   //FILE* fp_input = fopen("network//rgb_image//dog_rgb888_96X96_abc.raw", "rb");
   //FILE* fp_input = fopen("network//rgb_image//bedroom_RGB888_224x224.raw", "rb");
   //FILE* fp_input = fopen("network//rgb_image//Jay_RGB888_500x358.raw", "rb");
   //FILE* fp_input = fopen("network//rgb_image//vulture_RGB888_500x358.raw", "rb");
   //FILE* fp_input = fopen("network//rgb_image//eagle_RGB888_227x227.raw", "rb");
   //FILE* fp_input = fopen("network//rgb_image//image_jay.yuv", "rb");
   //FILE* fp_input = fopen("network//rgb_image//5_raw8_40x40.raw", "rb");
   //FILE* fp_input = fopen("network//rgb_image//dog_rgb888_48x48_2.raw", "rb");
   //FILE* fp_input = fopen("network//rgb_image//face_rot_rgb888_48x48.raw", "rb");
   FILE* fp_input = fopen("network//rgb_image//face_rgb888_500x500.raw", "rb");

   if (fp_input == NULL)
   {
      assert(0);
   }

   memset(&imgHdr, 0, sizeof(imgHdr));
   imgHdr.imgDescriptor.bitsPerPixel = 24; //8;//
   imgHdr.imgDescriptor.format = NUCFG_FORMAT_RGB888_E;//NUCFG_FORMAT_RAW8_E;// NUCFG_FORMAT_YUV422_8BIT_E;//
   imgHdr.imgDescriptor.height = 500;
   imgHdr.imgDescriptor.width = 500;
   imgHdr.imgDescriptor.x = 0;
   imgHdr.imgDescriptor.y = 0;
   imgHdr.imgDescriptor.stride = 500;
   imgHdr.imgDescriptor.realBitsMask = 0;
   imgHdr.imgDescriptor.bufferHeight = 500;
   imgHdr.imgDescriptor.numInterleaveImages = 1;
   imgHdr.imgDescriptor.interMode = NUCFG_INTER_MODE_FRAME_BY_FRAME_E;
   size = imgHdr.imgDescriptor.numInterleaveImages*((imgHdr.imgDescriptor.bitsPerPixel + 7) / 8)*imgHdr.imgDescriptor.height*imgHdr.imgDescriptor.width;

   copy_buf = (char*)malloc(size);
   size_t sizeLine;
   int comp_idx;
   int i;
   int num_comp = 1;
   unsigned char* p;

   //read image
   for (comp_idx = 0; comp_idx < num_comp; comp_idx++)
   {
      p = (unsigned char*)copy_buf;
      for (i = 0; i < imgHdr.imgDescriptor.numInterleaveImages*imgHdr.imgDescriptor.height; i++)
      {
         sizeLine = fread(p, 1, ((imgHdr.imgDescriptor.bitsPerPixel + 7) / 8)*imgHdr.imgDescriptor.width, fp_input);
         p += (((imgHdr.imgDescriptor.bitsPerPixel + 7) / 8)*imgHdr.imgDescriptor.stride);
      }
   }

   fclose(fp_input);
#ifdef ENABLE_DISPLAY
   unsigned short* imgDataP = (unsigned short*)copy_buf;
   CreateGLWindow(imgHdr.imgDescriptor.width, imgHdr.imgDescriptor.numInterleaveImages*imgHdr.imgDescriptor.height, "1");
   StreamDisplay(imgHdr.imgDescriptor.width, imgHdr.imgDescriptor.numInterleaveImages*imgHdr.imgDescriptor.height, imgDataP, GL_RGB, GL_UNSIGNED_BYTE, 0); //display as BGRA
#endif
   inu_streamer__setBufWriteCB(streamerInCdnnH, NULL);

   for (i = 0; i < 8; i++)
   {

      inu_streamer__write(streamerInCdnnH,
           copy_buf,
           size,
           (inu_data__hdr_t*)&imgHdr);
       millisleep_host(1000);


   }

}
#endif

#ifdef FDK_GAUSSIAN_DEMO_INPUT_FROM_FILE
//gaussian blur 3x3 with integer coefficients
void GaussianBlur3x3(UINT8* src, UINT8* dst, int width, int height, int stride)
{
   for (int j = 1; j < height - 1; j++)
   {
      for (int i = 1; i < width - 1; i++)
      {
         UINT32 acc;
         acc = (src[(j - 1)*stride + i - 1]);
         acc += (src[(j - 1)*stride + i]) << 1; // same as ()*2
         acc += (src[(j - 1)*stride + i + 1]);
         acc += (src[(j)*stride + i - 1]) << 1; // same as ()*2
         acc += (src[(j)*stride + i]) << 2;   // same as ()*4
         acc += (src[(j)*stride + i + 1]) << 1;  // same as ()*2
         acc += (src[(j + 1)*stride + i - 1]);
         acc += (src[(j + 1)*stride + i]) << 1; // same as ()*2
         acc += (src[(j + 1)*stride + i + 1]);
         dst[j*stride + i] = (UINT8)(acc >> 4); // normalize energy
      }
   }
}

void use_case_gaussian_from_file(inu_deviceH deviceH, inu_graphH graphH)
{
   ERRG_codeE status;
   inu_streamerH streamerInGaussianH;
   status = inu_graph__findNode(graphH, "Stream_In_Gaussian", &streamerInGaussianH);
   SANDBOX_TEST_RET(status);

   status = inu_function__start(streamerInGaussianH, &startParam);
   SANDBOX_TEST_RET(status);

   inu_image__hdr_t imgHdr;
   UINT32 size;
   FILE* fp_input = fopen("network//rgb_image//barbara_raw8_512x512.raw", "rb");
   FILE* fp_output = fopen("network//rgb_image//barbara_raw8_512x512_ref.raw", "wb");

   if (fp_input == NULL)
   {
      assert(0);
   }

   imgHdr.imgDescriptor.bitsPerPixel = 8;//
   imgHdr.imgDescriptor.format = NUCFG_FORMAT_RAW8_E;
   imgHdr.imgDescriptor.height = 512;
   imgHdr.imgDescriptor.width = 512;
   imgHdr.imgDescriptor.x = 0;
   imgHdr.imgDescriptor.y = 0;
   imgHdr.imgDescriptor.stride = 512;
   imgHdr.imgDescriptor.realBitsMask = 0;
   imgHdr.imgDescriptor.bufferHeight = 512;
   imgHdr.imgDescriptor.numInterleaveImages = 1;
   imgHdr.imgDescriptor.interMode = NUCFG_INTER_MODE_FRAME_BY_FRAME_E;

   size = imgHdr.imgDescriptor.numInterleaveImages*((imgHdr.imgDescriptor.bitsPerPixel + 7) / 8)*imgHdr.imgDescriptor.height*imgHdr.imgDescriptor.width;

   copy_buf = (char*)malloc(size);
   size_t sizeLine;
   int comp_idx;
   int i;
   int num_comp = 1;
   unsigned char* p;

   //read image
   for (comp_idx = 0; comp_idx < num_comp; comp_idx++)
   {
      p = (unsigned char*)copy_buf;
      for (i = 0; i < imgHdr.imgDescriptor.numInterleaveImages*imgHdr.imgDescriptor.height; i++)
      {
         sizeLine = fread(p, 1, ((imgHdr.imgDescriptor.bitsPerPixel + 7) / 8)*imgHdr.imgDescriptor.width, fp_input);
         p += (((imgHdr.imgDescriptor.bitsPerPixel + 7) / 8)*imgHdr.imgDescriptor.stride);
      }
   }
   fclose(fp_input);

   UINT8 *ref_buf = (UINT8*)malloc(size);
   UINT8 *in_buf = (UINT8*)copy_buf;
   memcpy(ref_buf, in_buf, size);
   GaussianBlur3x3(in_buf, ref_buf, imgHdr.imgDescriptor.width, imgHdr.imgDescriptor.height, imgHdr.imgDescriptor.stride);

#ifdef ENABLE_DISPLAY
   CreateGLWindow(imgHdr.imgDescriptor.width, imgHdr.imgDescriptor.numInterleaveImages*imgHdr.imgDescriptor.height, "in");
   StreamDisplay(imgHdr.imgDescriptor.width, imgHdr.imgDescriptor.numInterleaveImages*imgHdr.imgDescriptor.height, in_buf, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0); //display as BGRA
   CreateGLWindow(imgHdr.imgDescriptor.width, imgHdr.imgDescriptor.numInterleaveImages*imgHdr.imgDescriptor.height, "ref");
   StreamDisplay(imgHdr.imgDescriptor.width, imgHdr.imgDescriptor.numInterleaveImages*imgHdr.imgDescriptor.height, ref_buf, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0); //display as BGRA
#endif
   fwrite(ref_buf, 1, size, fp_output);
   fclose(fp_output);
   inu_streamer__setBufWriteCB(streamerInGaussianH, NULL);

   for (i = 0; i < 8; i++)
   {
      inu_streamer__write(streamerInGaussianH,
         copy_buf,
         size,
         (inu_data__hdr_t*)&imgHdr);
      millisleep_host(1000);
   }

}
#endif

#ifdef SLAM_INPUT_FROM_FILE
void use_case_slam_from_file(inu_deviceH deviceH, inu_graphH graphH)
{
   ERRG_codeE status;
   inu_streamerH streamerInSlamH;
   status = inu_graph__findNode(graphH, "Stream_In_SLAM", &streamerInSlamH);
   SANDBOX_TEST_RET(status);

   inu_cva_dataH cvaDataH;
   status = inu_graph__findNode(graphH, "CVA_in_1", &cvaDataH);
   SANDBOX_TEST_RET(status);

   inu_slamH slamH;
   status = inu_graph__findNode(graphH, "slam_1", &slamH);
   SANDBOX_TEST_RET(status);

   inu_slam_dataH slamDataH;
   status = inu_graph__findNode(graphH, "slam_data_1", &slamDataH);
   SANDBOX_TEST_RET(status);

   status = inu_function__start(streamerInSlamH, &startParam);
   SANDBOX_TEST_RET(status);

   inu_cva_data__hdr_t cvaDataHdr;
   UINT32 size;
   int i;
   FILE* fp_input = fopen("slam_input//injection.bin", "rb");


   if (fp_input == NULL)
   {
      assert(0);
   }

   inu_streamer__setBufWriteCB(streamerInSlamH, NULL);
#define MAX_KP_TO_RECORD (1024)
#define DESC_SIZE_BYTES (32)
   for (i = 0; i < 100; i++)
   {
      int injectionSize = 4 + 4 + MAX_KP_TO_RECORD*(5 * 4 + 2 * DESC_SIZE_BYTES) + 4 + 4 + 4 + 4 + 4;
      cvaDataHdr.cvaDescriptor.kpNum = i;
      cvaDataHdr.cvaDescriptor.height = 0;
      cvaDataHdr.cvaDescriptor.width = 0;

      size = injectionSize;

      copy_buf = (char*)malloc(size);
      size_t sizeLine;
      unsigned char* p;

      //read dog freak data
      p = (unsigned char*)copy_buf;
      sizeLine = fread(p, 1, size, fp_input);

      inu_streamer__write(streamerInSlamH,
         copy_buf,
         size,
         (inu_data__hdr_t*)&cvaDataHdr);
      millisleep_host(1000);
   }
   fclose(fp_input);

}
#endif

#ifdef DPE_PP
void use_case_dpe_pp(inu_deviceH deviceH, inu_graphH graphH)
{
   ERRG_codeE status;
   inu_dpe_ppH dpeH;
   inu_dpe_pp_hostGpMsgStructT hostGPmsg;

   status = inu_graph__findNode(graphH, "dpe_pp_0", &dpeH);
   SANDBOX_TEST_RET(status);

   hostGPmsg.ConfidenceThresholdRegion1 = 0;
   hostGPmsg.BlobDispDiffSameBlob = 16;
   hostGPmsg.BlobMaxHeight = 16;
   hostGPmsg.BlobMaxSize = 500;

   status = inu_dpe_pp__send_data_async(dpeH, (char*)&hostGPmsg, sizeof(hostGPmsg));
   SANDBOX_TEST_RET(status);

   status = inu_function__start(dpeH, &startParam);
   SANDBOX_TEST_RET(status);

}
#endif

#ifdef FAST_ORB_INPUT_FROM_FILE
ERRG_codeE use_case_fast_orb_input_from_file(inu_deviceH deviceH, inu_graphH graphH)
{
   ERRG_codeE status;
   inu_streamerH  Forb_streamerInH;
   inu_imageH     Forb_imageH;
   inu_streamerH    Forb_outStreamerH;
   inu_fast_orbH    Forb_fastOrbH;
   inu_fast_orb_dataH  Forb_fast_orb_dataH;


   status = inu_graph__findNode(graphH, "stream_in2", &Forb_streamerInH);
   if (ERRG_SUCCEEDED(status))
   {
      //inu_streamer__setBufWriteCB(Forb_streamerInH, buffWriteCompleteCB);
      inu_streamer__setBufWriteCB(Forb_streamerInH, NULL);
      status = inu_graph__findNode(graphH, "stream_image2", &Forb_imageH);
   }
   if (ERRG_SUCCEEDED(status))
   {
      status = inu_graph__findNode(graphH, "fast_orb1", &Forb_fastOrbH);
   }
   if (ERRG_SUCCEEDED(status))
   {
      status = inu_graph__findNode(graphH, "fast_orb_data1", &Forb_fast_orb_dataH);
   }
   if (ERRG_FAILED(status))
   {
      return status;
   }

   inu_image__hdr_t *imgHdrP;
   imgHdrP = (inu_image__hdr_t*)inu_data__getHdr(Forb_imageH);

   inu_graph__findNode(graphH, "Sout_orb", &Forb_outStreamerH);

   inu_function__start(Forb_outStreamerH, &startParam);
   inu_function__start(Forb_fastOrbH, &startParam);
   inu_function__start(Forb_streamerInH, &startParam);

   //new user buffer to copy to
   imageInitParams.imageDescriptor = imgHdrP->imgDescriptor;
   copy_buf = (char*)malloc(((imgHdrP->imgDescriptor.bitsPerPixel + 7) / 8)*imgHdrP->imgDescriptor.height*imgHdrP->imgDescriptor.width);

   FILE* fp_input = fopen("input_640x480_10bpp.yuv", "rb");
   if (fp_input == NULL)
   {
      assert(0);
   }


   for (int frameNum = 0; frameNum < 1; frameNum++)
   {
      size_t size;
      int comp_idx;
      int i;
      int num_comp = 1;
      unsigned char* p;

      //read image
      for (comp_idx = 0; comp_idx < num_comp; comp_idx++)
      {
         p = (unsigned char*)copy_buf;
         for (i = 0; i < imageInitParams.imageDescriptor.height; i++)
         {
            size = fread(p, 1, ((imageInitParams.imageDescriptor.bitsPerPixel + 7) / 8)*imageInitParams.imageDescriptor.width, fp_input);
            p += (((imageInitParams.imageDescriptor.bitsPerPixel + 7) / 8)*imageInitParams.imageDescriptor.stride);
         }
      }

      //send image
      inu_streamer__write(Forb_streamerInH, copy_buf, ((imgHdrP->imgDescriptor.bitsPerPixel + 7) / 8)*imgHdrP->imgDescriptor.height*imgHdrP->imgDescriptor.width, NULL);
   }
   fclose(fp_input);

   CreateGLWindow(imageInitParams.imageDescriptor.width, imageInitParams.imageDescriptor.height, "1");

   //read from data object
   inu_fast_orb_dataH data;
   void* dataP;

   status = inu_streamer__read(Forb_outStreamerH, &data);


   inu_fast_orb_data__hdr_t *dataHdr;
   if (ERRG_SUCCEEDED(status))
   {
      dataP = inu_data__getMemPtr(data);
      dataHdr = (inu_fast_orb_data__hdr_t *)inu_data__getHdr(data);
      UINT64 virtAddressX = (UINT64)dataP;
      UINT32 yOffset = dataHdr->yOffset;
      UINT32 scoreOffset = dataHdr->scoreOffset;
      UINT32 descOffset = dataHdr->descOffset;
      UINT16 *resultsXP = (UINT16*)dataP;
      UINT16 *resultsYP = (UINT16*)(virtAddressX + yOffset);
      UINT16 *resultsScoreP = (UINT16*)(virtAddressX + scoreOffset);
      UINT32 *desc = (UINT32*)(virtAddressX + descOffset);



   #ifdef ENABLE_DISPLAY
      inu_image__hdr_t* imgHdrP = (inu_image__hdr_t*)inu_data__getHdr(Forb_imageH);
      unsigned short* imgDataP = (unsigned short*)copy_buf;
      inu_fast_orb__printFeaturesOnScreen(resultsXP, resultsYP, imgDataP, 640, dataHdr->numKeyPoints);
      for (i = 0; i < imgHdrP->imgDescriptor.width*imgHdrP->imgDescriptor.height; i++)
      {
         (imgDataP)[i] = (imgDataP)[i] << 6;
      }
      StreamDisplay(imgHdrP->imgDescriptor.width, imgHdrP->imgDescriptor.height, imgDataP, GL_LUMINANCE, GL_UNSIGNED_SHORT, 0); //display as BGRA
   #endif
      inu_streamer__free(Forb_outStreamerH, data);
   }
}
#endif


#ifdef READ_WRITE_REGISTER
static void use_case_read_write_register(inu_deviceH deviceH, inu_graphH graphH)
{
   millisleep_host(10000);
   inu_device__writeRegT writeReg;
   inu_device__readRegT readReg;
   writeReg.regType = INU_DEVICE__REG_SOC_E;
   writeReg.phase = INU_DEVICE__DB_PHASE_0_E;
   writeReg.socReg.addr = 0x8020104; //use GME general register for the test
   writeReg.socReg.val = 0xCAFA;
   status = inu_device__writeRegister(deviceH, &writeReg);
   SANDBOX_TEST_RET(status);

   readReg.regType = INU_DEVICE__REG_SOC_E;
   readReg.socReg.addr = 0x8020104;
   status = inu_device__readRegister(deviceH, &readReg);
   SANDBOX_TEST_RET(status);

   if (readReg.socReg.val != writeReg.socReg.val)
      SANDBOX_TEST_RET(status);
}
#endif

#ifdef READ_WRITE_BUFFER
static void use_case_read_write_buffer(inu_deviceH deviceH, inu_graphH graphH)
{
   inu_device__buff_info_t writeBufInfo, readBufInfo;
   char *orgBuf;
   char *recBuf;
   bool testPass = true;
   int numLoop;

   orgBuf = (char*)malloc(READ_WRITE_BUF_SIZE);
   recBuf = (char*)malloc(READ_WRITE_BUF_SIZE);
   for (numLoop = 0; numLoop < NUM_OF_TEST_LOOP; numLoop++)
   {
      for (i = 0; i < READ_WRITE_BUF_SIZE; i++)
      {
         orgBuf[i] = rand();
      }
      memset(recBuf, 0, READ_WRITE_BUF_SIZE);

      writeBufInfo.address = READ_WRITE_MEM_ADDRESS;
      writeBufInfo.buf = orgBuf;
      writeBufInfo.len = READ_WRITE_BUF_SIZE;

      readBufInfo.address = READ_WRITE_MEM_ADDRESS;
      readBufInfo.buf = recBuf;
      readBufInfo.len = READ_WRITE_BUF_SIZE;

      printf("Start Writing buffer to memory, loop number %d\n", numLoop);
      status = inu_device__writeBuff(deviceH, &writeBufInfo);
      SANDBOX_TEST_RET(status);
      printf("Writing buffer size %d to memory address 0x%x done, loop number %d\n", READ_WRITE_BUF_SIZE, READ_WRITE_MEM_ADDRESS, numLoop);

      printf("Start reading buffer from memory, loop number %d\n", numLoop);
      status = inu_device__readBuff(deviceH, &readBufInfo);
      SANDBOX_TEST_RET(status);
      printf("Reading buffer size %d from memory address 0x%x done, loop number %d\n", READ_WRITE_BUF_SIZE, READ_WRITE_MEM_ADDRESS, numLoop);

      if (memcmp(readBufInfo.buf, writeBufInfo.buf, sizeof(char) * READ_WRITE_BUF_SIZE))
      {
         testPass = false;
         break;
      }
   }
   if (testPass)
   {
      printf("Write and read buffer test success (num of loops %d)\n", NUM_OF_TEST_LOOP);
   }
   else
   {
      printf("Write and read buffer test failed at loop %d\n", numLoop);
   }
   free(recBuf);
   free(orgBuf);
}
#endif

#ifdef STREAM_IN_OUT
static void use_case_stream_in_out(inu_deviceH deviceH, inu_graphH graphH)
{
   ERRG_codeE status;
   inu_streamerH streamerInH;
   inu_imageH imageH;
   inu_streamerH outStreamerH;
   status = inu_graph__findNode(graphH, "Stream_in0", &streamerInH);
   SANDBOX_TEST_RET(status);
   inu_streamer__setBufWriteCB(streamerInH, buffWriteCompleteCB);
   status = inu_graph__findNode(graphH, "IMAGE", &imageH);
   SANDBOX_TEST_RET(status);
   inu_image__hdr_t *imgHdrP;
   imgHdrP = (inu_image__hdr_t*)inu_data__getHdr(imageH);

   status = inu_graph__findNode(graphH, "Stream_out0", &outStreamerH);
   SANDBOX_TEST_RET(status);
   status = inu_function__start(outStreamerH, &startParam);
   SANDBOX_TEST_RET(status);
   status = inu_function__start(streamerInH, &startParam);
   SANDBOX_TEST_RET(status);

   //new user buffer to copy to
   imageInitParams.imageDescriptor = imgHdrP->imgDescriptor;
   UINT32 imgSize = ((imgHdrP->imgDescriptor.bitsPerPixel + 7) / 8)*imgHdrP->imgDescriptor.height*imgHdrP->imgDescriptor.width*imgHdrP->imgDescriptor.numInterleaveImages;
   copy_buf = (char*)malloc(imgSize);
   status = inu_streamer__write(streamerInH, copy_buf, imgSize, NULL);
   SANDBOX_TEST_RET(status);
}
#endif

static void streamLoggerThread(void *argP)
{
   volatile thread_params *paramP = (thread_params*)argP;
   ERRG_codeE status;
   inu_logH logH = (inu_logH)paramP->streamerH;
   logDataT logData;

   paramP->active = 1;
   while (paramP->active)
   {
      //read from data object
      status = inu_logger__read(logH, &logData);
      //check if there was a disconnection
      if (g_device_operational)
      {
         if (ERRG_SUCCEEDED(status))
         {
            printf("%s\n",logData.dataP);
            inu_logger__free(&logData);
         }
      }
      else
      {
         return;
      }
   }
}


#if (defined _WIN32) || (defined _WIN64)
   static LARGE_INTEGER frequency = { 0 };
   static void OS_LYRG_getUsecTime(UINT64 *usecP)
   {
      LARGE_INTEGER time;
      if (!frequency.QuadPart)
         QueryPerformanceFrequency(&frequency);
      if (!QueryPerformanceCounter(&time))
         printf("QueryPerformanceFrequency failed\n");
      *usecP = (UINT64)(time.QuadPart / (frequency.QuadPart / 1000000));
   }
/*
   static void OS_LYRG_getUsecTimeAdjust(UINT64* usecP)
   {

           OS_LYRG_getUsecTime(usecP);
           *usecP += 1000000;
   }
*/
#else
   static void OS_LYRG_getUsecTime(UINT64 *usecP)
   {
      struct timeval time;
       if (gettimeofday(&time, NULL) == -1)
         {
               printf("gettimeofday failed\n");
         }
      *usecP = (time.tv_sec*1000000) + time.tv_usec;
   }
#endif

#ifdef STREAM_CROPPED
   int cropCounter = 0;
#endif
static void streamThread(void *argP)
{
   volatile thread_params *paramP = (thread_params*)argP;
   ERRG_codeE status;
   inu_data__hdr_t *hdrP = (inu_data__hdr_t*)inu_data__getHdr(inu_node__getNextInputNode(paramP->streamerH, NULL));

   if (!hdrP)
      assert(0);

   if (paramP->startCallback != NULL)
   {
      paramP->startCallback(paramP->streamerH, hdrP, paramP->cookie);
   }
   if (inu_streamer__getStreamType(paramP->streamerH) == INU_STREAMER__REGULAR)
       recursiveStartFunctions(paramP->streamerH);
   else
   {
       while(1)
           millisleep_host(10000);
   }
   if(paramP->recursiveStartDone)
      paramP->recursiveStartDone(paramP->streamerH,paramP->cookie, (inu_deviceH*)&paramP->deviceH);
   paramP->active = 1;
   while (paramP->active && (g_device_operational))
   {
      inu_dataH data = NULL;

      //read from data object
      status = inu_streamer__read(paramP->streamerH, &data);
      //check if there was a disconnection
      if (g_device_operational)
      {
         if (ERRG_SUCCEEDED(status))
         {
            hdrP = (inu_data__hdr_t*)inu_data__getHdr(data);
#ifdef STREAM_CROPPED
            inu_soc_channel__cropParamsT crop;
            inu_image__hdr_t *imgHdrP = (inu_image__hdr_t*)hdrP;

            if ((imgHdrP->imgDescriptor.width == 200) && (imgHdrP->dataHdr.dataIndex % 20 == 0))
            {
               cropCounter++;
               if ((cropCounter) % 2 == 0)
               {
                  crop.xStart = 0;
                  crop.yStart = 0;
               }
               else if ((cropCounter) % 3 == 0)
               {
                  crop.xStart = 100;
                  crop.yStart = 100;
               }
               else
               {
                  crop.xStart = 300;
                  crop.yStart = 300;
               }
               printf("%s change crop xy to %d,%d\n", inu_ref__getUserName(paramP->streamerH), crop.xStart, crop.yStart);
               inu_soc_channel__updateCropWindow(paramP->sourceH, &crop);
            }
#endif

#ifdef ENABLE_STREAM_PRINTS
            printf("read success %s. cntr = %d, ts = %llu, mode %d\n",inu_ref__getUserName(paramP->streamerH), hdrP->dataIndex, hdrP->timestamp, inu_data__getMode(data));
#endif
            if (paramP->frameCallback != NULL)
            {
                static float gain = 1;
                INU_Metadata_T metadata;
                ERRG_codeE ret = inu_data__getMetadata(data, &metadata);
                if (ERRG_SUCCEEDED(ret))
                {
                    paramP->frameCallback(paramP->streamerH, hdrP, (char*)inu_data__getMemPtr(data), inu_data__chunkSizeGet(data), paramP->cookie, &metadata, (inu_deviceH *) &paramP->deviceH);
                }
                else
                {
                    paramP->frameCallback(paramP->streamerH, hdrP, (char*)inu_data__getMemPtr(data), inu_data__chunkSizeGet(data), paramP->cookie,NULL, (inu_deviceH*) &paramP->deviceH);
                }
            }
            //release data object memory
            inu_streamer__free(paramP->streamerH, data);

#ifdef FAST_STOP_START_CHECK
            if ((paramP->numToRead%100)==0)
            {
               recursiveStopFunctions(paramP->streamerH);
               recursiveStartFunctions(paramP->streamerH);
            }
#endif
         }
         else
         {
            if(paramP->failedCallback)
            {
               paramP->failedCallback(paramP->streamerH,paramP->cookie, (inu_deviceH*)&paramP->deviceH);
            }
            FAILED_PRINTF(status, paramP->streamerH);
            //printf("failed: %x \n",status);
         }
      }
   }
   recursiveStopFunctions(paramP->streamerH);

   if (paramP->stopCallback != NULL)
   {
      paramP->stopCallback(paramP->streamerH, paramP->cookie);
   }
}

static void streamImuThread(void *argP)
{
   volatile thread_params *paramP = (thread_params*)argP;
   ERRG_codeE status;
   static unsigned int ImuCount = 0;
#if 1
   inu_imu__temperatureParams imuTempParams;
#endif
   inu_imu_data__hdr_t *hdrP = (inu_imu_data__hdr_t*)inu_data__getHdr(inu_node__getNextInputNode(paramP->streamerH, NULL));
   inu_imu_data__readableData readableData[128];

   if (!hdrP)
      assert(0);

   if (paramP->startCallback != NULL)
   {
      paramP->startCallback(paramP->streamerH, (inu_data__hdr_t*)hdrP, paramP->cookie);
   }
   recursiveStartFunctions(paramP->streamerH);

   paramP->active = 1;
   while ((paramP->active) && (g_device_operational))
   {
      inu_dataH data = NULL;
#if 1
      ImuCount++;
      if ((ImuCount % 10000) == 0)
      {
         status = inu_imu__getTemperature(paramP->sourceH, &imuTempParams);
         printf("IMU temperature value: %f\n", imuTempParams.temperature);
         ImuCount = 0;
      }
#endif

      //read from data object
      status = inu_streamer__read(paramP->streamerH, &data);

      //check if there was a disconnection
      if (g_device_operational)
      {
         if (ERRG_SUCCEEDED(status))
         {
            hdrP = (inu_imu_data__hdr_t*)inu_data__getHdr(data);
            /*The Helsinki batch optimization is explained below:
               Instead of sending a single set of X,Y,Z IMU readings we instead
               send batchCount number of X,Y,Z,Timestamp samples all in one USB message. 
               Where each X,Y,Z,Timestamp sample is of size batchSizeBytes.

               This means that in memory, the data looks like this:
               X[0],Y[0],Z[0],Timestamp[0],  X[0],Y[0],Z[0],Timestamp[0]... X[batchCount-1],Y[batchCount-1],Z[batchCount-1],Timestamp[batchCount-1]

               And the total size of the IMU payload is now batchCount*batchSizeBytes
               The batch size is defined by the target within helsinki.h, see HELSINKI_IMU_BATCH_SIZE
            */
            INU_Metadata_T temporaryMetadata = ZEROED_OUT_METADATA;
            ERRG_codeE metadata_Ret =    inu_metadata__deserialize(hdrP->metadata.buffer,sizeof(hdrP->metadata.buffer),&temporaryMetadata);
            ERRG_codeE ret_status = INU_METADATA__RET_SUCCESS;
            if(ERRG_SUCCEEDED(metadata_Ret) && temporaryMetadata.protobuf_packet.has_imuTimestamp && temporaryMetadata.protobuf_packet.imuTimestamp.start_imu_timestamp_ns && temporaryMetadata.protobuf_packet.has_imuBatchInfo)
            {
               if(temporaryMetadata.protobuf_packet.imuBatchInfo.batchingMethod == INU_metadata_IMU_Batching_Method_Individual_Channel_Batching)
               {
                  /*Magnetometer will still use individual channel batching */
                  UINT64 timestamp_offset = temporaryMetadata.protobuf_packet.imuTimestamp.start_imu_timestamp_ns;
                  //printf("Timestamp offset %llu \n", timestamp_offset);
                  ret_status = inu_imu_data__convertDataMultipleWithTimestampConversion(data,readableData,sizeof(readableData)/sizeof(inu_imu_data__readableData),hdrP);
               }
               else if(temporaryMetadata.protobuf_packet.imuBatchInfo.batchingMethod == INU_metadata_IMU_Batching_Method_Paired_Gyro_Accelerometer_Channel_Batching)
               {
                  ret_status = inu_imu_data__convertDataMultipleWithPairedBatchingAndTimestampConversion(data,readableData,sizeof(readableData)/sizeof(inu_imu_data__readableData),hdrP);
               }
            }
            else
            {
               /*Timestamp conversion is not being used */
               ret_status = inu_imu_data__convertDataMultiple(data,readableData,sizeof(readableData)/sizeof(inu_imu_data__readableData),hdrP);
            }
            
         
            if(ret_status == INU_IMU_DATA__RET_SUCCESS)
            {
               for (int batchIndex = 0; batchIndex < hdrP->batchCount ; batchIndex++)
               {
                  // printf("Processing batch %lu/%lu with size %lu \n",batchIndex,hdrP->batchCount,hdrP->batchSizeBytes);
                  if (paramP->frameCallback != NULL)
                  {
                     paramP->frameCallback(paramP->streamerH, &hdrP->dataHdr, (char*)&readableData[batchIndex], sizeof(inu_imu_data__readableData), paramP->cookie, &temporaryMetadata, NULL);
                  }
               }
            }
            inu_streamer__free(paramP->streamerH, data);
         }
      }
   }
   recursiveStopFunctions(paramP->streamerH);
}


//for manual testing of sensor control, we find all the sensors and sensor groups
//repeat for each group:
//  increase FPS of the group
//  go over all sensors, increase exposure and gain and verify the values
//  reset group fps, exposure and gain
static void manualSensorControlThread(void *argP)
{
   volatile thread_params *paramP = (thread_params*)argP;
   ERRG_codeE status;
   inu_sensors_groupH sensorGroupsH;
   inu_sensorH sensor;
   inu_sensor__runtimeCfg_t sensorGroupOrigCfg, modCfg, test;

   sensorGroupOrigCfg.context = INU_SENSOR__CONTEXT_A;
   modCfg.context = INU_SENSOR__CONTEXT_A;
   test.context = INU_SENSOR__CONTEXT_A;
   millisleep_host(1000);

   sensorGroupsH = paramP->streamerH;

   paramP->active = 1;
   while (paramP->active)
   {
      if (sensorGroupsH)
      {
         //get start configuration
         status = inu_sensors_group__getRuntimeCfg(sensorGroupsH, &sensorGroupOrigCfg);
       SANDBOX_TEST_RET_ASSERT(status);

         modCfg = sensorGroupOrigCfg;
         modCfg.fps += 20;
         //set new fps for group
         status = inu_sensors_group__setRuntimeCfg(sensorGroupsH, &modCfg);
       SANDBOX_TEST_RET_ASSERT(status);

         millisleep_host(1000);
         sensor = inu_node__getNextOutputNode(sensorGroupsH, NULL);
         //go over all sensors in the group and set new exposure and gain for each
         while (sensor)
         {
            modCfg.exposure += 5000;
            modCfg.gain.analog += 10;
            status = inu_sensor__setRuntimeCfg(sensor, &modCfg);
         SANDBOX_TEST_RET_ASSERT(status);

            millisleep_host(1000);
            status = inu_sensor__getRuntimeCfg(sensor, &test);
         SANDBOX_TEST_RET_ASSERT(status);

            if (memcmp(&modCfg, &test, sizeof(inu_sensor__runtimeCfg_t)))
            {
               assert(0);
            }
            sensor = inu_node__getNextOutputNode(sensorGroupsH, sensor);
         }
         //set start configuration
         status = inu_sensors_group__setRuntimeCfg(sensorGroupsH, &sensorGroupOrigCfg);
       SANDBOX_TEST_RET_ASSERT(status);
         millisleep_host(1000);
         //go over all sensors, check if they have the same configuration as the sensors_group config
         sensor = inu_node__getNextOutputNode(sensorGroupsH, NULL);
         //go over all sensors in the group and set new exposure and gain for each
         while (sensor)
         {
            status = inu_sensor__getRuntimeCfg(sensor, &test);
         SANDBOX_TEST_RET_ASSERT(status);

            if (memcmp(&sensorGroupOrigCfg, &test, sizeof(inu_sensor__runtimeCfg_t)))
            {
               assert(0);
            }
            sensor = inu_node__getNextOutputNode(sensorGroupsH, sensor);
         }
      }
   }
}


//simple test, activate the algo and display the current exposure/gain
static void autoSensorControlThread(void *argP)
{
   volatile thread_params *paramP = (thread_params*)argP;
   ERRG_codeE status;
   inu_sensors_groupH sensorGroupsH;
   inu_sensor__runtimeCfg_t sensorGroupCfg;
   inu_functionH sensor_controlH;
#ifdef SENSOR_CONTROL_CHANGE_ROI
   inu_histogram__roiCfgT roiCfg = { 0 };
   inu_histogram_dataH histData;
   inu_histogramH histFunc;
#endif
   UINT32 operateModes, mode = 0;
   inu_sensor_control__operation_mode_e operationMode;

   sensor_controlH = paramP->streamerH;
   operateModes = inu_function__getOperateModes(sensor_controlH);
   while (operateModes)
   {
      if (operateModes & 0x1)
         break;
      mode++;
      operateModes = operateModes >> 1;
   }
   sensorGroupCfg.context = (inu_sensor__sensorContext_e)mode;
   //set context according to operating modes. if more then one, then just use the first
#ifdef SENSOR_CONTROL_PARAMS_UPDATE
   int counter = 5; //update every 5 seconds
   inu_sensor_control__parameter_list_t params, origParams;
   status = inu_sensor_control__getParams(sensor_controlH, &origParams);
   SANDBOX_TEST_RET(status);
#endif
   recursiveStartFunctions(paramP->streamerH);
   status = inu_graph__getOrigSensGroup(sensor_controlH, &sensorGroupsH);
   SANDBOX_TEST_RET_ASSERT(status);
#ifdef ENABLE_SENSOR_CONTROL
   inu_sensor_control__getOperationMode(sensor_controlH, &operationMode);
#endif
   paramP->active = 1;
   while (paramP->active && (g_device_operational))
   {
      if (sensorGroupsH)
      {
         //get start configuration
         status = inu_sensors_group__getRuntimeCfg(sensorGroupsH, &sensorGroupCfg);
         SANDBOX_TEST_RET_ASSERT(status);
//#ifdef ENABLE_DISPLAY
          if (operationMode == INU_SENSOR_CONTROL__SW_MODE)
          {
             printf("%s (context %d): exposure %d, gain (%f,%f)\n", inu_ref__getUserName(paramP->streamerH), sensorGroupCfg.context, sensorGroupCfg.exposure, sensorGroupCfg.gain.analog, sensorGroupCfg.gain.digital);
          }
//#endif
         millisleep_host(1000);
#ifdef SENSOR_CONTROL_CHANGE_ROI
         roiCfg.roi[0].x0 = 100;
         roiCfg.roi[0].x1 = 200;
         roiCfg.roi[0].y0 = 100;
         roiCfg.roi[0].y1 = 200;
         //find all the histogram functions for this sensor contorl, and change their ROI
         histData = inu_node__getNextInputNode(sensor_controlH, NULL);
         while (histData)
         {
            if (inu_ref__instanceOf(histData, INU_HISTOGRAM_DATA_REF_TYPE))
            {
               histFunc = inu_node__getNextInputNode(histData, NULL);
               status = inu_histogram__cfgRoi(histFunc, &roiCfg);
               FAILED_PRINTF(status,histFunc);
            }
            histData = inu_node__getNextInputNode(sensor_controlH, histData);
         }
#endif
#ifdef SENSOR_CONTROL_PARAMS_UPDATE
         counter--;
         if (counter == 0)
         {
            status = inu_sensor_control__getParams(sensor_controlH, &params);
            SANDBOX_TEST_RET(status);

            if (params.exposureMax == origParams.exposureMax)
            {
               params.exposureMax = 255 * 4;
               params.exposureMin = 200 * 4;
               params.nSatMax = 30;
               params.debug = 1;
            }
            else
            {
               params = origParams;
               params.debug = 0;
            }

            status = inu_sensor_control__updateParams(sensor_controlH, &params);
            SANDBOX_TEST_RET(status);
            counter = 5;
         }
#endif
      }
   }
   recursiveStopFunctions(paramP->streamerH);

}

// static void awbThread(void *argP)
// {
//    volatile thread_params *paramP = (thread_params*)argP;

//    recursiveStartFunctions(paramP->streamerH);
//    paramP->active = 1;
//    while (paramP->active && (g_device_operational))
//    {
//       printf("%s \n", inu_ref__getUserName(paramP->streamerH));
//       millisleep_host(1000);
//    }
//    recursiveStopFunctions(paramP->streamerH);
// }

static thread_params *invokeStreamThread(inu_deviceH deviceH, inu_functionH sourceH, inu_streamerH streamerH, inu_dataH dataH, threadFunc func, FWLib_cfgChannelCbsT *chCfgCbsP)
{
   thread_params *paramsP;
   paramsP = (thread_params*)malloc(sizeof(thread_params));
   memset(paramsP, 0, sizeof(thread_params));
   paramsP->sourceH = sourceH;
   paramsP->streamerH = streamerH;
   paramsP->dataH = dataH;
   paramsP->active = 0;
   paramsP->func = func;
   if (chCfgCbsP)
   {
      paramsP->startCallback = chCfgCbsP->startCallback;
      paramsP->stopCallback = chCfgCbsP->stopCallback;
      paramsP->frameCallback = chCfgCbsP->frameCallback;
      paramsP->failedCallback = chCfgCbsP->failCallback;
      paramsP->recursiveStartDone = chCfgCbsP->recursiveStartDone;
      paramsP->cookie = chCfgCbsP->cookie;
   }
   paramsP->deviceH = deviceH;
#if (defined _WIN32) || (defined _WIN64)
   DWORD threadID;
   paramsP->threadH = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, paramsP, 0, &threadID);
#else
   typedef void *(*PTHREAD_ROUTINE) (void *);
   if (pthread_create((pthread_t *)&paramsP->threadH, NULL, (PTHREAD_ROUTINE)func, paramsP))
   {
      assert(0);
   }
#endif
   //wait until the thread is active
   while (!paramsP->active)
   {
      millisleep_host(100);
   }

   return paramsP;
}

static void closeStreamThreads(thread_params *threadParamsP)
{
   threadParamsP->active = 0;
#if (defined _WIN32) || (defined _WIN64)
   WaitForSingleObject(threadParamsP->threadH, INFINITE);
#else
   pthread_join((pthread_t)threadParamsP->threadH, NULL);
#endif
   free(threadParamsP);
}

void FwLib_sw_reset(inu_deviceH meH)
{
   ERRG_codeE status;
   inu_device__watchdog_timeout_t param;
   SANDBOX_channelListParamNodesT *pHead = channelListParamsHead;
   param.sec = 0xFFFFFFFF;
   status = inu_device__setWatchdog(meH, &param);
   SANDBOX_TEST_RET_ASSERT(status);
   while(pHead)
   {
      closeStreamThreads((thread_params *)pHead->chH);
      pHead = pHead->next;
   }
}

void IRbuffWriteCompleteCB(inu_streamerH meH, UINT8* data, void *cookie)
{
   ERRG_codeE status;
   FWLib_configParams *paramsP = (FWLib_configParams*)cookie;
   inu_function__stateParam state;
   inu_function__getState(meH, &state);
   static char injFilePath[100];
   static char* imgBuff = NULL;

   if ((g_device_operational) && (state.state == ENABLE) && (injectionActive == 1))
   {
      inu_image__hdr_t *hdrP;
      inu_imageH imageH;

      imageH = inu_node__getNextOutputNode(meH, NULL);
      if (imageH == NULL)
         assert(0);
      hdrP = (inu_image__hdr_t*)inu_data__getHdr(imageH);
      millisleep_host(100);
      int bytesPerPixel = ((hdrP->imgDescriptor.bitsPerPixel + 7) / 8);
      int size = bytesPerPixel*hdrP->imgDescriptor.height*hdrP->imgDescriptor.width*hdrP->imgDescriptor.numInterleaveImages;
      if (imgBuff == NULL)
      {
         imgBuff = (char*)malloc(size);
      }
      hdrP->dataHdr.dataIndex = counter;

      if (paramsP->injectionMode == 1)
      {
         sprintf((char*)(&injFilePath), "Bin_640_400_inter\\image15_%d.raw", (counter % 5));
         status = inu_streamer__write(meH, open_raw_file(injFilePath, imgBuff, size), size, (inu_data__hdr_t*)hdrP, cookie);
      }
      else if (paramsP->injectionMode == 2)
      {
         sprintf((char*)(&injFilePath), "Full_1280_800_inter\\image15_%d.raw", (counter % 5));
         status = inu_streamer__write(meH, open_raw_file(injFilePath, imgBuff, size), size, (inu_data__hdr_t*)hdrP, cookie);
      }

#if defined INJECTION_BUFFER
      char *bufP;
      static char temp = 0;
      temp++;
      //memset(copy_buf, 255, size);
      for (int i = 0; i < hdrP->imgDescriptor.height; i++)
      {
         bufP = copy_buf + (i * hdrP->imgDescriptor.width * hdrP->imgDescriptor.numInterleaveImages * bytesPerPixel);
         memset(bufP, temp, hdrP->imgDescriptor.width * bytesPerPixel);
         bufP = bufP + (hdrP->imgDescriptor.width * bytesPerPixel);
         memset(bufP, 0xFF - temp, hdrP->imgDescriptor.width * bytesPerPixel);
      }

      status = inu_streamer__write(meH, imgBuff, size, NULL);
#endif
     SANDBOX_TEST_RET_ASSERT(status);
      counter++;

   }
}


void FEbuffWriteCompleteCB(inu_streamerH meH, UINT8* data, void *cookie)
{
   ERRG_codeE status;
   FWLib_configParams *paramsP = (FWLib_configParams*)cookie;
   inu_function__stateParam state;
   inu_function__getState(meH, &state);
   static char injFilePath[100];
   static char* imgBuff = NULL;

   if ((g_device_operational) && (state.state == ENABLE) && (injectionActive == 1))
   {
      inu_image__hdr_t *hdrP;
      inu_imageH imageH;

      imageH = inu_node__getNextOutputNode(meH, NULL);
      if (imageH == NULL)
         assert(0);
      hdrP = (inu_image__hdr_t*)inu_data__getHdr(imageH);
      millisleep_host(100);
      int bytesPerPixel = ((hdrP->imgDescriptor.bitsPerPixel + 7) / 8);
      int size = bytesPerPixel*hdrP->imgDescriptor.height*hdrP->imgDescriptor.width*hdrP->imgDescriptor.numInterleaveImages;
      if (imgBuff == NULL)
      {
         imgBuff = (char*)malloc(size);
      }
      hdrP->dataHdr.dataIndex = counter;

      if (paramsP->injectionMode == 3)
      {
         sprintf((char*)(&injFilePath), "Bin_640_480_inter\\image15_%d.raw", (counter % 5));
         status = inu_streamer__write(meH, open_raw_file(injFilePath, imgBuff, size), size, (inu_data__hdr_t*)hdrP, cookie);
      }

     SANDBOX_TEST_RET_ASSERT(status);
      counter++;
   }
}

void RGBbuffWriteCompleteCB(inu_streamerH meH, UINT8* data, void* cookie)
{
    ERRG_codeE status;
    FWLib_configParams* paramsP = (FWLib_configParams*)cookie;
    inu_function__stateParam state;
    inu_function__getState(meH, &state);
    static char injFilePath[100];
    static char* imgBuff = NULL;

    if ((g_device_operational) && (state.state == ENABLE) && (injectionActive == 1))
    {
        inu_image__hdr_t* hdrP;
        inu_imageH imageH;

        imageH = inu_node__getNextOutputNode(meH, NULL);
        if (imageH == NULL)
            assert(0);
        hdrP = (inu_image__hdr_t*)inu_data__getHdr(imageH);
        millisleep_host(100);
        int bytesPerPixel = ((hdrP->imgDescriptor.bitsPerPixel + 7) / 8);
        int size = bytesPerPixel * hdrP->imgDescriptor.height * hdrP->imgDescriptor.width * hdrP->imgDescriptor.numInterleaveImages;
        if (imgBuff == NULL)
        {
            imgBuff = (char*)malloc(size);
        }
        hdrP->dataHdr.dataIndex = counter;

        if (paramsP->injectionMode == 4)
        {
            sprintf((char*)(&injFilePath), "Bin_400_400_inter\\image11_0.raw");
            status = inu_streamer__write(meH, open_raw_file(injFilePath, imgBuff, size), size, (inu_data__hdr_t*)hdrP, cookie);
        }

#if defined INJECTION_BUFFER
        char* bufP;
        static char temp = 0;
        temp++;
        //memset(copy_buf, 255, size);
        for (int i = 0; i < hdrP->imgDescriptor.height; i++)
        {
            bufP = copy_buf + (i * hdrP->imgDescriptor.width * hdrP->imgDescriptor.numInterleaveImages * bytesPerPixel);
            memset(bufP, temp, hdrP->imgDescriptor.width * bytesPerPixel);
            bufP = bufP + (hdrP->imgDescriptor.width * bytesPerPixel);
            memset(bufP, 0xFF - temp, hdrP->imgDescriptor.width * bytesPerPixel);
        }

        status = inu_streamer__write(meH, imgBuff, size, NULL);
#endif
        SANDBOX_TEST_RET_ASSERT(status);

   }
}

void DPEPPbuffWriteCompleteCB(inu_streamerH meH, UINT8* data, void* cookie)
{
    ERRG_codeE status;
    FWLib_configParams* paramsP = (FWLib_configParams*)cookie;
    inu_function__stateParam state;
    inu_function__getState(meH, &state);
    static char* imgBuff = NULL;

    if ((g_device_operational) && (state.state == ENABLE) && (injectionActive == 1))
    {
        inu_image__hdr_t* hdrP;
        inu_imageH imageH;

        imageH = inu_node__getNextOutputNode(meH, NULL);
        if (imageH == NULL)
            assert(0);
        hdrP = (inu_image__hdr_t*)inu_data__getHdr(imageH);
        millisleep_host(100);
        int bytesPerPixel = ((hdrP->imgDescriptor.bitsPerPixel + 7) / 8);
        int size = bytesPerPixel * hdrP->imgDescriptor.height * hdrP->imgDescriptor.width * hdrP->imgDescriptor.numInterleaveImages;
        if (imgBuff == NULL)
        {
            imgBuff = (char*)malloc(size);
        }
        hdrP->dataHdr.dataIndex = counter;

        if (paramsP->injectionMode == 6)
        {
            size_t sizeLine;
            unsigned int i;
            unsigned char* p;
            FILE* fp_input = fopen("dpe_pp_input//Disparity_00000214.tif", "rb");

            if (fp_input == NULL)
            {
                assert(0);
            }

            p = (unsigned char*)imgBuff;
            for (i = 0; i < hdrP->imgDescriptor.numInterleaveImages * hdrP->imgDescriptor.height; i++)
            {
                sizeLine = fread(p, 1, bytesPerPixel * hdrP->imgDescriptor.width, fp_input);
                p += (bytesPerPixel * hdrP->imgDescriptor.stride);
            }
            fclose(fp_input);
            status = inu_streamer__write(meH, imgBuff, size, (inu_data__hdr_t*)hdrP, cookie);
            SANDBOX_TEST_RET_ASSERT(status);
        }
    }
}

void activateInjector(inu_graphH graphH, FWLib_configParams *paramsP, const char* streamerName, const char* writerName, inu_streamer__buffWriteCompleteCB cb)
{
   ERRG_codeE status;
   int size = 0;
   inu_soc_writerH  soc_writerH = NULL;
   inu_functionH    streamerInH;
   inu_imageH       imageH;
   inu_image__hdr_t *hdrP;
   static char injFilePath[100];

   status = inu_graph__findNode(graphH, streamerName, &streamerInH); //"Stream_In_1"
   if (ERRG_FAILED(status))
      assert(0);
   inu_streamer__setBufWriteCB(streamerInH, cb);//buffWriteCompleteCB

   status = inu_graph__findNode(graphH, writerName, &soc_writerH); //"WRITER_2"
   if (ERRG_FAILED(status))
      assert(0);

   imageH = inu_node__getNextOutputNode(streamerInH, NULL);
   if (imageH == NULL)
      assert(0);
   hdrP = (inu_image__hdr_t*)inu_data__getHdr(imageH);

   //first frame inject junk
   if (paramsP->injectionMode == 1)
   {
#if defined(INJECTION_BUFFER) || defined(INJECTION_FROM_BMP_FILES)
      size = ((hdrP->imgDescriptor.bitsPerPixel + 7) / 8)*hdrP->imgDescriptor.height*hdrP->imgDescriptor.width*hdrP->imgDescriptor.numInterleaveImages;
#else
      size = 640 * 400 * 2 * 2;
#endif
      sprintf((char*)(&injFilePath), "Bin_640_400_inter\\image15_0.raw");
   }
   else if (paramsP->injectionMode == 2)
   {
      size = 1280 * 800 * 2 * 2;
      sprintf((char*)(&injFilePath), "Full_1280_800_inter\\image15_0.raw");
   }
   else if (paramsP->injectionMode == 3)
   {
      size = 640 * 480 * 2 * 2;
      sprintf((char*)(&injFilePath), "Bin_640_480_inter\\image15_0.raw");
   }
   else if (paramsP->injectionMode == 4)
   {
       size = 400 * 400 * 3;
       sprintf((char*)(&injFilePath), "Bin_400_400_inter\\image11_0.raw");
   }
   copy_buf = (char*)malloc(size);
   hdrP->dataHdr.dataIndex = 0;
   status = inu_streamer__write(streamerInH, open_raw_file(injFilePath, copy_buf, size), size, (inu_data__hdr_t*)hdrP, paramsP);
}

void activateInjectorDpePp(inu_graphH graphH, FWLib_configParams* paramsP, const char* streamerName, const char* writerName, inu_streamer__buffWriteCompleteCB cb)
{
    ERRG_codeE status;
    int size = 0;
    inu_functionH    streamerInH;
    inu_imageH       imageH;
    inu_image__hdr_t* imghdrP;

    status = inu_graph__findNode(graphH, streamerName, &streamerInH);
    if (ERRG_FAILED(status))
        assert(0);
    inu_streamer__setBufWriteCB(streamerInH, cb);

    imageH = inu_node__getNextOutputNode(streamerInH, NULL);
    if (imageH == NULL)
        assert(0);
    imghdrP = (inu_image__hdr_t*)inu_data__getHdr(imageH);

    //first frame inject junk
    if (paramsP->injectionMode == 6)
    {
        size_t sizeLine;
        unsigned int i;
        unsigned char* p;
        FILE* fp_input = fopen("dpe_pp_input//Disparity_00000214.tif", "rb");

        if (fp_input == NULL)
        {
            assert(0);
        }

        imghdrP->imgDescriptor.bitsPerPixel = 16;
        imghdrP->imgDescriptor.format = NUCFG_FORMAT_DISPARITY_E;
        imghdrP->imgDescriptor.height = 1024;
        imghdrP->imgDescriptor.width = 1024;
        imghdrP->imgDescriptor.x = 0;
        imghdrP->imgDescriptor.y = 0;
        imghdrP->imgDescriptor.stride = 1024;
        imghdrP->imgDescriptor.realBitsMask = 0;
        imghdrP->imgDescriptor.bufferHeight = 1024;
        imghdrP->imgDescriptor.numInterleaveImages = 1;
        imghdrP->imgDescriptor.interMode = NUCFG_INTER_MODE_FRAME_BY_FRAME_E;

        size = imghdrP->imgDescriptor.numInterleaveImages * ((imghdrP->imgDescriptor.bitsPerPixel + 7) / 8) * imghdrP->imgDescriptor.height * imghdrP->imgDescriptor.width;
        copy_buf = (char*)malloc(size);
        p = (unsigned char*)copy_buf;
        for (i = 0; i < imghdrP->imgDescriptor.numInterleaveImages * imghdrP->imgDescriptor.height; i++)
        {
            sizeLine = fread(p, 1, ((imghdrP->imgDescriptor.bitsPerPixel + 7) / 8) * imghdrP->imgDescriptor.width, fp_input);
            p += (((imghdrP->imgDescriptor.bitsPerPixel + 7) / 8) * imghdrP->imgDescriptor.stride);
        }
        fclose(fp_input);
        imghdrP->dataHdr.dataIndex = 0;
        status = inu_streamer__write(streamerInH, copy_buf, size, (inu_data__hdr_t*)imghdrP, paramsP);
    }
}

ERRG_codeE FwLib_activate_hw(inu_deviceH deviceH, FWLib_configParams *paramsP, char *function_name, FWLib_chH *chH)
{
   ERRG_codeE status;
   inu_nodeH channelH = NULL;

   status = inu_graph__findNode(paramsP->configH, function_name, &channelH);
   if (ERRG_SUCCEEDED(status) && channelH)
   {
      if (INU_REF__IS_FUNCTION_TYPE(inu_ref__getRefType(channelH)))
      {
         status = recursiveStartFunctions(channelH);
         if (ERRG_SUCCEEDED(status))
         {
            *chH = channelH;
         }
      }
      else
      {
         status = (ERRG_codeE)ERR_INVALID_ARGS;
      }
   }

   return status;
}

ERRG_codeE FwLib_stop_hw(inu_deviceH deviceH, FWLib_chH chH)
{
   ERRG_codeE status;
   inu_functionH channelH = (inu_functionH)chH;

   if (channelH)
   {
      status = recursiveStopFunctions(channelH);
   }
   else
   {
      status = (ERRG_codeE)ERR_NULL_PTR;
   }

   return status;
}

static void channel_params_add_new_node(FWLib_chH node)
{
   SANDBOX_channelListParamNodesT  *newNode, *runner;
   newNode = (SANDBOX_channelListParamNodesT*)malloc(sizeof(SANDBOX_channelListParamNodesT));
   if (!newNode)
      assert(0);
   newNode->chH = node;
   newNode->next = NULL;
   //add inputs at the end, to maintain the same order of creation
   //check if head is empty
   if (!(channelListParamsHead))
   {
      channelListParamsHead = newNode;
   }
   else
   {
      runner = channelListParamsHead;
      //find last in list
      while (runner->next)
      {
         runner = runner->next;
      }
      runner->next = newNode;
   }
}

ERRG_codeE FwLib_activate_channel(inu_deviceH deviceH, FWLib_configParams *paramsP, char *stream_out_name, FWLib_cfgChannelCbsT *chCfgCbsP, FWLib_chH *chH)
{
   ERRG_codeE status;
   inu_nodeH streamerH;
   inu_nodeH channelH;
   inu_nodeH dataH;

   status = inu_graph__findNode(paramsP->configH, stream_out_name, &streamerH);

   if (ERRG_SUCCEEDED(status))
   {
      inu_ref__types streamer_ref_type = inu_ref__getRefType((inu_refH*)streamerH);

      //get data
      dataH = inu_node__getNextInputNode(streamerH, NULL);
      if (dataH)
      {
         //get channel
         channelH = inu_node__getNextInputNode(dataH, NULL);
         if (channelH)
         {
            threadFunc *func = NULL;
            if (streamer_ref_type == INU_STREAMER_REF_TYPE)
            {
               inu_ref__types data_ref_type = inu_ref__getRefType((inu_refH*)dataH);

               switch (data_ref_type)
               {
                  case(INU_IMAGE_REF_TYPE) :
                  case(INU_CVA_DATA_REF_TYPE) :
                  case(INU_SLAM_DATA_REF_TYPE) :
                  case(INU_HISTOGRAM_DATA_REF_TYPE) :
                  case(INU_TEMPERATURE_DATA_REF_TYPE) :
                  case(INU_CDNN_DATA_REF_TYPE) :
                  case(INU_POINT_CLOUD_DATA_REF_TYPE) :
                  case(INU_RAW_DATA_REF_TYPE):
                  {
                     func = streamThread;
                     break;
                  }
                  case(INU_IMU_DATA_REF_TYPE) :
                  {
                     func = streamImuThread;
                     break;
                  }
                  default:
                  {
                     func = NULL;
                     break;
                  }
               }
            }
            else if ((streamer_ref_type == INU_SENSOR_CONTROL_REF_TYPE) || (streamer_ref_type == INU_CDNN_REF_TYPE))
            {
               func = autoSensorControlThread;
            }
            else if (streamer_ref_type == INU_SENSORS_GROUP_REF_TYPE)
            {
               func = manualSensorControlThread;
            }
            else
            {
               printf("Invalid streamer type = %d\n", streamer_ref_type);
               assert(0);
            }

            *chH = invokeStreamThread(deviceH, channelH, streamerH, dataH, func, chCfgCbsP);
            channel_params_add_new_node(*chH);
         }
      }
   }
   else
   {
      printf("Can't find Streamer Name [%s] in SW Graph.\n\n", stream_out_name);
      printf("Please choose one of the following Streamers\n");
      inu_graph__printNodes(paramsP->configH, &streamerH);
      printf("Press Enter to exit application\n");
      getc(stdin);
      exit(0);
   }

#ifdef INJECTION
   if (!injectionActive)
   {
      injectionActive = 1;
      if ((paramsP->injectionMode == 1) || (paramsP->injectionMode == 2))
      {
         activateInjector(paramsP->configH, paramsP, "Stream_In_0", "WRITER_1", IRbuffWriteCompleteCB);
      }
      else if (paramsP->injectionMode == 3)
      {
         activateInjector(paramsP->configH, paramsP, "Stream_In_1", "WRITER_2", FEbuffWriteCompleteCB);
      }
      else if (paramsP->injectionMode == 4)
      {
          activateInjector(paramsP->configH, paramsP, "Stream_In_3", "WRITER_0", RGBbuffWriteCompleteCB);
      }
      else if (paramsP->injectionMode == 6) //  DPE PP injection
      {
          activateInjectorDpePp(paramsP->configH, paramsP, "Stream_In_disparity", "WRITER_2", DPEPPbuffWriteCompleteCB);
      }
   }
#endif

   return status;
}

ERRG_codeE FwLib_stop_channel(inu_deviceH deviceH, FWLib_chH chH)
{
   thread_params *paramsP = (thread_params*)chH;
#ifdef INJECTION
   injectionActive = 0;
#endif

#ifdef SW_RESET
   resetTarget(deviceH);
#else
   closeStreamThreads(paramsP);
#endif

#if defined (FAST_ORB)
   fclose(fp_output);
#endif

   return (ERRG_codeE)RET_SUCCESS;
}

ERRG_codeE FwLib_reset_channel_frame_count(inu_refH ref)
{
   inu_nodeH  currentH = NULL;
   currentH = inu_node__getNextInputNode(ref, NULL);
   while (currentH)
   {
       if (inu_ref__instanceOf(currentH, INU_SOC_CHANNEL_REF_TYPE))
       {
           break;
       }
       currentH = inu_node__getNextInputNode(currentH, NULL);
   }
   if (!currentH)
      SANDBOX_TEST_RET((ERRG_codeE)ERR_UNEXPECTED);

   return inu_soc_channel__resetFrameCnt(currentH);
}

ERRG_codeE FwLib_set_channel_user_info(inu_refH ref, UINT32 param_1, UINT32 param_2)
{
   inu_nodeH  currentH = NULL;
   inu_soc_channel__userParamsT u;
   currentH = inu_node__getNextInputNode(ref, NULL);
   while (currentH)
   {
       if (inu_ref__instanceOf(currentH, INU_SOC_CHANNEL_REF_TYPE))
       {
           break;
       }
       currentH = inu_node__getNextInputNode(currentH, NULL);
   }
   if (!currentH)
      SANDBOX_TEST_RET((ERRG_codeE)ERR_UNEXPECTED);

   u.param1 = param_1;
   u.param2 = param_2;

   return inu_soc_channel__updateUserParams(currentH, &u);
}

inu_logH logH;

void inu_device_event_handler(inu_deviceH deviceH, inu_device_connect_state_e connect_state)
{
   printf("DEVICE CALLBACK: connect_state = %d\n", connect_state);
   if (connect_state == INU_DEVICE__CONNECT)
   {
      g_device_operational = true;
      if ((g_if_select == INU_DEVICE_INTERFACE_0_UART) || (g_if_select == INU_DEVICE_INTERFACE_0_CDC))
      {
         inu_device__boot_deinit();
      }
   }
   else
   {
      //flush and read/ioctls... and notify all threads of disconnection.
      g_device_operational = false;
   }
}

int usbMonitorCallbackWrapper(INU_DEFSG_usbStatusE event, inu_device__hwVersionIdU serviceLinkEvent)
{
   printf("MONITOR CALLBACK: event = %s\n", usbEventToString(event));
   if (event == INU_DEFSG_USB_EVT_INSERTION)
   {
      printf("Chip version 0x%x\n", serviceLinkEvent.val);
      g_device_chipVersion = true;
   }

   return 0;
}

#ifdef TCP_COMMUNICATION
int usbEventCallbackWrapper(INU_DEFSG_usbStatusE event, inu_device__hwVersionIdU serviceLinkEvent)
{
   printf("USB BOOT callback!: event = %s\n", usbEventToString(event));
#if (defined _WIN32) || (defined _WIN64)
   if ((event == INU_DEFSG_USB_EVT_BOOT_COMPLETE)  || (event == INU_DEFSG_USB_EVT_INSERTION) )
#else
   if ((event == INU_DEFSG_USB_EVT_BOOT_COMPLETE) || (event == INU_DEFSG_USB_EVT_INSERTION) || (event == INU_DEFSG_USB_EVT_REMOVAL))
#endif
   {
      g_device_boot = true;
   }
   else
   {
      g_device_boot = false;
   }
   return 0;
}
#else
int usbEventCallbackWrapper(INU_DEFSG_usbStatusE event, inu_device__hwVersionIdU serviceLinkEvent)
{
   printf("USB BOOT callback!: event = %s\n", usbEventToString(event));
   if (event == INU_DEFSG_USB_EVT_BOOT_COMPLETE || ((event == INU_DEFSG_USB_EVT_REMOVAL) && ((g_if_select == INU_DEVICE_INTERFACE_0_UART) || (g_if_select == INU_DEVICE_INTERFACE_0_CDC) ||  (g_if_select == INU_DEVICE_INTERFACE_0_TCP))))
   {
      g_device_boot = true;
   }
   else
   {
      g_device_boot = false;
   }
   return 0;
}
#endif
/*-- boot device */
ERRG_codeE FwLib_boot(int bootId)
{
   ERRG_codeE ret;
   int filterId = -1;
   #ifdef CHIP_VERSION_FROM_USB
   //read chip id first
   ret = inu_device__usb_monitor(usbMonitorCallbackWrapper, get_usb_id(), get_external_hp(),-1, 20);
   if (ERRG_SUCCEEDED(ret))
   {
      while (!g_device_chipVersion)
         os_sleep(10);

      ret = inu_device__usb_monitor_deinit();
     SANDBOX_TEST_RET(ret);
   }
#endif

   //boot via usb
   ret = inu_device__usb_boot(usbEventCallbackWrapper, get_boot_path(), get_usb_id(), get_external_hp(), bootId, filterId, get_thread_sleep_time_us());

    if (ERRG_SUCCEEDED(ret))
   {
      //wait for boot to complete
      while (!g_device_boot)
      {
#if (defined _WIN32) || (defined _WIN64)
         Sleep(10);
#else
         sleep(10);
#endif
      }

      if ((g_if_select == INU_DEVICE_INTERFACE_0_UART) || (g_if_select == INU_DEVICE_INTERFACE_0_CDC))
      {
#if (defined _WIN32) || (defined _WIN64)
         Sleep(100);
#else
         sleep(100);
#endif
      }
   }
   return ret;
}

ERRG_codeE FwLib_boot_deinit()
{
   ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;

   status = inu_device__boot_deinit();
    g_device_boot = false;

   return status;
}

ERRG_codeE FwLib_bind_nodes_configs(FWLib_configParams *params1P, char *name1, FWLib_configParams *params2P, char *name2)
{
   ERRG_codeE status;
   inu_nodeH node1H;
   inu_nodeH node2H;

   status = inu_graph__findNode(params1P->configH, name1, &node1H);
   SANDBOX_TEST_RET(status);
   status = inu_graph__findNode(params2P->configH, name2, &node2H);
   SANDBOX_TEST_RET(status);
   status = inu_node__bindNodes(node1H, node2H);
   SANDBOX_TEST_RET(status);

   return status;
}

ERRG_codeE applyChannelsCfg(inu_nucfgH cfgH, FWLib_configParams *paramsP)
{
   ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;;

   for (int i = 0; i < SANDBOX_NUM_HW_CHANNELS; i++)
   {
      if (paramsP->cscMode[i] != -1)
      {
         status = NUCFG_setChannelFormat(cfgH, i, (NUCFG_changeFromatE)paramsP->cscMode[i]);
         SANDBOX_TEST_RET(status);
      }

      if (paramsP->iaeBypass[i])
      {
         UINT32 iauNum;
         status = NUCFG_setChannelIauBypass(cfgH, i, &iauNum);
         SANDBOX_TEST_RET(status);
      }

      if ((paramsP->crop[i].height) || (paramsP->crop[i].bufferHeight))
      {
         status = NUCFG_setChannelCrop(cfgH, i, (NUCFG_resT*)&paramsP->crop[i]);
         SANDBOX_TEST_RET(status);
      }

      if (paramsP->scale[i].stride && paramsP->scale[i].bufferHeight)
      {
         status = NUCFG_setChannelDimentions(cfgH, i, (NUCFG_resT*)&paramsP->scale[i]);
         SANDBOX_TEST_RET(status);
      }

      if (paramsP->chunkMode[i])
      {
         status = NUCFG_setChannelChunk(cfgH, i, paramsP->chunkMode[i]);
         SANDBOX_TEST_RET(status);
      }

      if (paramsP->interleave[i])
      {
         status = NUCFG_setChannelInterleave(cfgH, i, paramsP->interleave[i]);
         SANDBOX_TEST_RET(status);
      }

      if (paramsP->injectSensorInfo[i].numOfSensors)
      {
         //copy struct, do a sanity
         NUCFG_channelSrcSensorsList sensorList;
         if (sizeof(NUCFG_channelSrcSensorsList) != sizeof(FWLib_channelSrcSensorsListT))
         {
            assert(0);
         }
         memcpy(&sensorList, &paramsP->injectSensorInfo[i], sizeof(NUCFG_channelSrcSensorsList));
         status = NUCFG_linkChannelWritersToSensors(cfgH, i, &sensorList);
         SANDBOX_TEST_RET(status);
      }

      if ((paramsP->injectRes[i].height) || (paramsP->injectRes[i].bufferHeight))
      {
         status = NUCFG_setInjectResolution(cfgH, i, paramsP->injectRes[i].width, paramsP->injectRes[i].height);
         SANDBOX_TEST_RET(status);
      }
   }
   return status;
}

ERRG_codeE FwLib_waitForPdState(FWLib_pdStateE pdState)
{
   ERRG_codeE                 status = (ERRG_codeE)RET_SUCCESS;
   inu_device__powerDownCmdE  state;

   if (pdState == FWLIB_PD_WAIT_FOR_PD_E)
   {
      inu_device__updatePowerDownStatus(INU_DEVICE_POWER_DOWN_WAIT_FOR_EVENT_E);

      // run until suspend/power down
      do
      {
         inu_device__getPowerDownStatus(&state);
         os_sleep(10);

      } while (state != INU_DEVICE_POWER_DOWN_PREPARE_E);

      // from this point (entering pd), any deinit/init process will be in suspend/resume mode (i.e. partial)
      powerCycleMode = 1;
   }
   else if (pdState == FWLIB_PD_WAIT_FOR_WU_E)
   {
      inu_device__updatePowerDownStatus(INU_DEVICE_POWER_DOWN_ENTER_E);

      // wait for resume/wake-up
      do
      {
         inu_device__getPowerDownStatus(&state);
         os_sleep(10);

      } while (state != INU_DEVICE_POWER_DOWN_EXIT_E);
   }
   else
   {
      os_sleep(10000);
   }
   return status;
}

ERRG_codeE FwLib_config(inu_deviceH deviceH, FWLib_configParams *paramsP)
{
   ERRG_codeE status;
   inu_graphH graphH;
   unsigned int groupCtr;
#ifdef DEBUG_THREAD_ANALYSIS
   marker_series series_SandboxLib;
   span* Span = new span(series_SandboxLib, 1, _T("FwLib_config"));
#endif

   // when this confiuration occures when resuming from suspend/pd, it opens a graph with
   // already exsisting configuration, and then returns (no need to set again the configuration)
   if (powerCycleMode)
   {
      graphInitParams.deviceH = deviceH;

      status = inu_graph__renew(&graphH, &graphInitParams, &cfgG[paramsP->cfgId]);
      SANDBOX_TEST_RET(status);

      paramsP->configH = graphH;
      return status;
   }

   //initialize graph from soc xml based on bootid
   graphInitParams.deviceH = deviceH;
   graphInitParams.debugMode = initParams.hwXmlDebugMode;
   /*forceXML Type allows us to force us to use the HW/SW XML strings to be for the master or for the slave
    ForceXML Type can be the following values:
    0 = Disabled
    1 = Use Master XMLs
    2 = Use Slave XMLs
    3 = Autodetect XMLs
    */
   if(paramsP->forceXML_Type !=0)
   {
      const inu_device__version *versionP;
      //get device ID (nu3k/4k a0,b0 etc.)
      inu_device__getDeviceInfo(deviceH, &versionP);
      bool isMaster = (paramsP->forceXML_Type == 1) || ((paramsP->forceXML_Type)  == 3 && (versionP->isMaster));
      if(isMaster)
      {
         printf("Master detected \n");
         paramsP->hwXmlFile = strdup("nu4100_boot10065_Master_CVA.xml");
         paramsP->swXmlFile = strdup("sw_graph_boot10065_Master_CVA.xml");
      }
      else
      {
         printf("Slave detected \n");
         paramsP->hwXmlFile = strdup("nu4100_boot10065_Slave_CVA.xml");
         paramsP->swXmlFile = strdup("sw_graph_boot10065_Slave_CVA.xml");
      }     
   }
    if (!paramsP->injectionMode)
   {
         graphInitParams.socxml_path = get_socxml_path(&deviceVersion, initParams.configFilesPath, paramsP->hwXmlFile);
         graphInitParams.graphXMLpath = get_swxml_path(&deviceVersion, initParams.configFilesPath, paramsP->swXmlFile);
   }
   else
   {
         graphInitParams.socxml_path = get_inject_socxml_path(&deviceVersion, initParams.configFilesPath);
         graphInitParams.graphXMLpath = get_inject_swxml_path(&deviceVersion, initParams.configFilesPath);
   }
   graphInitParams.calibPathsP = get_calib_path((const char*)deviceVersion.serialNumber, initParams.calibPath);
   graphInitParams.saveXmlMod = true;
   strcpy((char*)graphInitParams.name, paramsP->name);
#ifdef DEBUG_THREAD_ANALYSIS
   series_SandboxLib.write_flag(_T("inu_graph__new start"));
#endif
   status = inu_graph__new(&graphH, &graphInitParams, &cfgG[paramsP->cfgId]);
#ifdef DEBUG_THREAD_ANALYSIS
   series_SandboxLib.write_flag(_T("inu_graph__new finish"));
#endif
   SANDBOX_TEST_RET(status);

   if (paramsP->injectionMode == 1)
   {
      NUCFG_setInjectResolution(cfgG[paramsP->cfgId], 0, 640, 400);
   }
   else if (paramsP->injectionMode == 2)
   {
      NUCFG_setInjectResolution(cfgG[paramsP->cfgId], 0, 1280, 800);
   }
   else if (paramsP->injectionMode == 3)
   {
      NUCFG_setInjectResolution(cfgG[paramsP->cfgId], 2, 640, 480);
   }
   else if (paramsP->injectionMode == 4)
   {
      NUCFG_setInjectResolution(cfgG[paramsP->cfgId], 3, 400, 400);
   }
   else if (paramsP->injectionMode == 6)
   {
       NUCFG_setInjectResolution(cfgG[paramsP->cfgId], 0, 1024, 1024);
   }

   applyChannelsCfg(cfgG[paramsP->cfgId], paramsP);

   for (unsigned int modeGroup = 0; modeGroup < NUCFG_getNumGroups(cfgG[paramsP->cfgId]); modeGroup++)
   {
      status = NUCFG_setGroupMode(cfgG[paramsP->cfgId], modeGroup, (CALIB_sensorOperatingModeE)paramsP->mode[modeGroup]);
      SANDBOX_TEST_RET(status);
      if (paramsP->fps[modeGroup] != 0)
      {
         status = NUCFG_setGroupFps(cfgG[paramsP->cfgId], modeGroup, paramsP->fps[modeGroup]);
         SANDBOX_TEST_RET(status);
      }
   }

   UINT32 numOfFiles = paramsP->dpeConfigNumFiles + 1;
   int i;
   //create list of dpe configuration files
   NUCFG_dpeFuncFileT dpeFuncFileList[SANDBOX_MAX_DPE_PRESET_FILE + 1];
   dpeFuncFileList[0].name = get_dpe_mode_full_path(&deviceVersion, "DPE_Default.xml", initParams.configFilesPath);
   dpeFuncFileList[0].frameNum = 0;
   for (i = 0; i < paramsP->dpeConfigNumFiles; i++)
   {
      dpeFuncFileList[i + 1].name = get_dpe_mode_full_path(&deviceVersion, paramsP->dpeConfigFiles[i], initParams.configFilesPath);
      dpeFuncFileList[i + 1].frameNum = 0;
   }
   NUCFG_setDpeFunc(cfgG[paramsP->cfgId], dpeFuncFileList, numOfFiles);
#ifdef DEBUG_THREAD_ANALYSIS
   series_SandboxLib.write_flag(_T("inu_graph__finalize start"));
#endif

   status = inu_graph__finalize(graphH, cfgG[paramsP->cfgId]);
#ifdef DEBUG_THREAD_ANALYSIS
   series_SandboxLib.write_flag(_T("inu_graph__finalize finish"));
#endif
   SANDBOX_TEST_RET(status);

   for (groupCtr = 0; groupCtr < NUCFG_getNumGroups(cfgG[paramsP->cfgId]); groupCtr++)
   {
      NUCFG_showChannelGroup(cfgG[paramsP->cfgId], LOG_INFO_E, groupCtr);
   }

   if (initParams.alternate)
   {
      inu_device__alternateProjCfgT cfg;
      cfg.alternateModeEnable = 1;
      cfg.numFramesFlood = initParams.alternate / 10;
      cfg.numFramesPattern = initParams.alternate % 10;
      status = inu_device__alternateProjector(deviceH, &cfg);
      SANDBOX_TEST_RET(status);
   }

#ifdef FDK_CNN_DEMO_STREAM
   use_case_fdk_cnn_demo(deviceH, graphH);
#endif

#ifdef CDNN_INPUT_FROM_FILE
   load_networks_from_file(deviceH, graphH);
#endif

#ifdef FDK_GAUSSIAN_DEMO_INPUT_FROM_FILE
   use_case_gaussian_from_file(deviceH, graphH);
#endif

#ifdef SLAM_INPUT_FROM_FILE
   use_case_slam_from_file(deviceH, graphH);
#endif

#ifdef DPE_PP
   use_case_dpe_pp(deviceH, graphH);
#endif

#ifdef STREAM_IN_OUT
   use_case_stream_in_out(deviceH, graphH);
#endif

#ifdef FAST_ORB_INPUT_FROM_FILE
   use_case_fast_orb_input_from_file(deviceH, graphH);
#endif

#ifdef READ_WRITE_REGISTER
   use_case_read_write_register(deviceH, graphH);
#endif

#ifdef READ_WRITE_BUFFER
   use_case_read_write_buffer(deviceH, graphH);
#endif

#ifdef PWM_TRIGGER
   inu_device__pwmConfigT pwmConfig;
   pwmConfig.fps = 20;
   pwmConfig.pwmNum = INU_DEVICE_PWM_1_E;
   pwmConfig.pwmCmd = INU_DEVICE_PWM_STOP_E;
   pwmConfig.widthInUsec = 200;
   inu_device__pwmConfig(deviceH, &pwmConfig);
#endif

#ifdef DEBUG_THREAD_ANALYSIS
   delete Span;
#endif

   paramsP->configH = graphH;

   return status;
}

ERRG_codeE FwLib_offline_get_config(const char *socxml_path, inu_nucfgH *cfgH)
{
   const char *path;

   if (socxml_path)
   {
      path = socxml_path;
   }
   else
   {
      path = get_socxml_path(&deviceVersion, initParams.configFilesPath, NULL);
   }
   return inu_graph__offlineGetConfig(path, cfgH);
}

ERRG_codeE FwLib_close_config(inu_deviceH deviceH, FWLib_configParams *paramsP)
{
   ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;

   // in regular configuration closing, we delete the graph and the configuration. this confiuration occures when resuming from suspend/pd, it opens a graph with
   // already exsisting configuration, and then returns (no need to set again the configuration)
   if (!powerCycleMode)
   {
      status = inu_graph__delete(paramsP->configH);
   }
   else
   {
      status = inu_graph__close(paramsP->configH);
   }
   return status;
}

/*
-- initialize library
*/
ERRG_codeE FwLib_lib_init(SANDBOX_initParams *paramsP)
{
   ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;

   initParams = *paramsP;

   g_if_select = (inu_device_interface_0_e)initParams.interfaceSelect;

#if (defined _WIN32) || (defined _WIN64)
   //set the OS resolution to 1ms
   timeBeginPeriod(1);
#endif

   return status;
}
static void Sleep_Until_Next_Second()
{
   /*Sleep until the start of the next second so we can get the coarse time correction sent just after the start of a second*/
   std::time_t tt = std::chrono::system_clock::to_time_t (std::chrono::system_clock::system_clock::now());
   struct std::tm * ptm = std::localtime(&tt); /*Get current time*/
   ptm->tm_sec = ((ptm->tm_sec+ 1) % 60); /*Sleep until next of start second*/
   std::this_thread::sleep_until (std::chrono::system_clock::system_clock::from_time_t (mktime(ptm)));
}
/*
-- read boot ID from device
-- copy calibration data from device and set calibration path
-- return context
*/
ERRG_codeE FwLib_init(inu_deviceH *deviceH_ptr, const inu_device__version **deviceVersionP)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   int     i;
#ifdef FW_UPGRADE
   inu_device__fwUpdateHdrT fwUpdt;
#endif

   ret = inu_init(powerCycleMode);
   SANDBOX_TEST_RET(ret);

   //create device
   inu_deviceH deviceH;
   inu_device__initParams deviceInitParams;
#ifdef TCP_COMMUNICATION
   inu_device_tcp_setup_t tcpSetup;
#endif
   //setup interface 0 - this is the interface used for reading bootId
   //and downloading target sw.
   get_interface_0_setup(&deviceInitParams.interface_0);
   //set bootid folder
   deviceInitParams.bootPath = get_boot_path();
   //register handler on device to monitor device connection
   deviceInitParams.eventCb = inu_device_event_handler;
   deviceInitParams.interface_0.select = (inu_device_interface_0_e)initParams.interfaceSelect;
   deviceInitParams.interface_0.select = (initParams.standalone == 1) ? INU_DEVICE_INTERFACE_0_STUB : (inu_device_interface_0_e)initParams.interfaceSelect;
   deviceInitParams.sharedMallocFunc = sharedMemMallocFunc;
   deviceInitParams.sharedFreeFunc = sharedMmemFreeFunc;
   deviceInitParams.useSharedMemory = 1;
   deviceInitParams.standAloneMode = (initParams.standalone == 1) ? INU_DEVICE__STANDALONE_MODE : INU_DEVICE__NORMAL_MODE;

   if (g_if_select == INU_DEVICE_INTERFACE_0_UART)
   {
      deviceInitParams.interface_0.interfaceParams.uart = initParams.uartIfParams;
   }
   else if (g_if_select == INU_DEVICE_INTERFACE_0_CDC)
   {
      deviceInitParams.interface_0.interfaceParams.cdc = initParams.cdcIfParams;
   }
   else if (g_if_select == INU_DEVICE_INTERFACE_0_TCP)
   {
       deviceInitParams.interface_0.interfaceParams.tcp = initParams.tcpIfParams;
   }
   //initialize device - and boot using interface_0 according to bootpath/bootid
   ret = inu_device__new(&deviceH, &deviceInitParams);
   SANDBOX_TEST_RET(ret);

   //wait for device to become operational
   while (!g_device_operational)
      os_sleep(10);

   inu_device__watchdog_timeout_t timeout;
   timeout.sec = initParams.watchdogTimeout;
#ifdef _DEBUG
   //cancel watchdog in debug mode
   timeout.sec = 0;
#endif
   ret   = inu_device__setWatchdog(deviceH, &timeout);
   SANDBOX_TEST_RET(ret);

   //create logger
   //Initialize logger
   inu_log__initParams  logInitParams;
   memset((void*)&logInitParams, 0, sizeof(inu_log__initParams));
   logInitParams.configParams.log_levels[HOST_ID] = LOG_INFO_E;
   logInitParams.configParams.log_levels[GP_ID] = LOG_INFO_E;
   logInitParams.configParams.gpPrintLogs = TRUE;
   logInitParams.configParams.gpSendLogs = FALSE;
   logInitParams.configParams.gpSendCdcLogs = FALSE;
   logInitParams.configParams.hostPrintGpLogs = TRUE;
   logInitParams.configParams.hostSendGpLogs = FALSE;
   logInitParams.configParams.hostPrintLogs = TRUE;
   logInitParams.configParams.hostSendLogs = FALSE;
   logInitParams.deviceH = deviceH;

   ret = inu_logger__new(&logH, &logInitParams);
   SANDBOX_TEST_RET(ret);
   inu_logger__start(logH);
   logchH = invokeStreamThread(deviceH, NULL, logH, NULL, streamLoggerThread, NULL);

   if (initParams.disablePowerOptimization)
   {
      inu_device__powerModeT powerMode;
      //disable all units power mode
      powerMode.disableUnits = 0xFFFFFFFF;
      inu_device__setPowerMode(deviceH, &powerMode);
   }

   inu_device__calibUpdateT num;
   num.dblBuffId = 0;   /*Temp fix by IMDT, this was originally streamNum*/
   inu_device__calibUpdate(deviceH, &num);
   
   // after power cycle, no need to retrieve device info and calibration from the flash,
   // as we already have it from the initial boot in global variables (device info) or in
   // the file system (calibration folder)
   if (!powerCycleMode)
   {
      const inu_device__version *versionP;

      //get device ID (nu3k/4k a0,b0 etc.)
      ret = inu_device__getDeviceInfo(deviceH, &versionP);
      SANDBOX_TEST_RET(ret);

      memcpy(&deviceVersion, versionP, sizeof(deviceVersion));
      *deviceVersionP = versionP;
      bootid = versionP->bootId;

      //copy calibration data from device and store in system
      inu_device_calibration_header_t calib_header;
      calib_header.calib_data_size = 0;
      ret = inu_device__getCalibrationHeader(deviceH, &calib_header);
      if (ERRG_SUCCEEDED(ret))
      {
         inu_device_calibration_data_t calib_data;
         calib_data.data = (char*)malloc(calib_header.calib_data_size);
         calib_data.len = calib_header.calib_data_size;
         ret = inu_device__getCalibrationData(deviceH, &calib_data);
         if (ERRG_SUCCEEDED(ret))
         {
            store_calib_data(calib_data.data, calib_header.calib_data_size);
            //free calib data struct
            free(calib_data.data);
         }
      }
   }

   //set device time
   inu_device__set_time_t settime;
   inu_device__getUsecTimeFuncT* getUsecTimeFunc = NULL;// OS_LYRG_getUsecTimeAdjust;//

   memset(&settime, 0, sizeof(inu_device__set_time_t));

   if (initParams.useUartSync)
   {
      settime.method = INU_DEVICE__SET_TIME_PLL_LOCK_UART_E;
      settime.uartBaudRate = 115200; //921600;
      settime.secondHostResolution = 1;
      strncpy(settime.uartPortName, initParams.uartPortName, MAX_UART_PORT_NAME_LEN);
      settime.startTick = 10000; //10312;
      settime.debugEnable = 1;
   }
   else
   {
      settime.method = INU_DEVICE__SET_TIME_ONE_SHOT_E;
   }
   inu_device_setTimeMethodE originalMethod = settime.method ;
   /*VERY IMPORTANT - Please do not remove!!!
   We must sleep until the next start of second to prevent race conditions where the PPS could happen just after sending a coarse time correction. 
   The RTC would then apply the new coarse timestamp and you would lose a second within the TS_HIGH value.
   Doing a sleep until the next second before sending the coarse time is also sensible in that it can allow you to get millisecond accurate time without a PPS
   */
   Sleep_Until_Next_Second();
   UINT64 usec;
   OS_LYRG_getUsecTime(&usec);
   /*Set RTC time first as its the most accurate*/
   settime.method = INU_DEVICE__SET_TIME_PLL_LOCK_RTC_E;
   settime.seconds = (INT32)(usec / 1000000);
   settime.microSeconds = (INT32)(usec % 1000000);
   ret = inu_device__setTime(deviceH, &settime, getUsecTimeFunc);
   SANDBOX_TEST_RET(ret);
   /*Send RTC lock set time request too */
   settime.method = originalMethod;
   ret = inu_device__setTime(deviceH, &settime,getUsecTimeFunc);
   settime.method = originalMethod; /*Set the get time method back to the original mechanism*/
   inu_device__get_time_t gettime;
   ret = inu_device__getTime(deviceH, &gettime);
   SANDBOX_TEST_RET(ret);
//   OS_LYRG_getUsecTime(&usec);
//   printf("time diff us = %llu\n", usec- gettime.usec);

   //output context and device
   *deviceH_ptr = deviceH;
#ifdef FW_UPGRADE
   fwUpdt.element[0].fileName = "C:\\Program Files\\Inuitive\\InuDev\\bin\\NU4000b0\\boot0\\zImage";
   fwUpdt.element[0].type = INU_DEVICE_PRODUCT_TYPE_LXKERNEL_E;
   fwUpdt.element[1].fileName = "C:\\Program Files\\Inuitive\\InuDev\\bin\\NU4000b0\\boot0\\nu4000b0-boot51.dtb";
   fwUpdt.element[1].type = INU_DEVICE_PRODUCT_TYPE_LXDTB_E;
   fwUpdt.element[2].fileName = "C:\\Program Files\\Inuitive\\InuDev\\bin\\NU4000b0\\boot0\\initramfs_boot51.gz";
   fwUpdt.element[2].type = INU_DEVICE_PRODUCT_TYPE_INITRAMFS_E;
   fwUpdt.element[3].fileName = "C:\\Program Files\\Inuitive\\InuDev\\bin\\NU4000b0\\boot0\\nu4000b0.zip";
   fwUpdt.element[3].type = INU_DEVICE_PRODUCT_TYPE_APPLICATION_E;
   fwUpdt.numProductsToUpdate = 4;
   ret = inu_device__fwUpdate(deviceH, &fwUpdt);
#endif //FW_UPGRADE

#ifdef PROJECTOR
   devG = deviceH;
   //get proj level
   inu_device__projectorGetModeT getProjMode;
   getProjMode.projSelect = INU_DEVICE_PROJ_0_E;
   inu_device__getProjectorMode(deviceH, &getProjMode);
   //set proj level
   inu_device__projectorSetModeT setProjMode;
   setProjMode.projSelect = INU_DEVICE_PROJ_0_E;
   if (getProjMode.projGetMode == INU_DEVICE_POJECTOR_DISABLE_E)
   {
      setProjMode.projSetMode = INU_DEVICE_POJECTOR_PATTERN_LOW_E;
   }
   else if (getProjMode.projGetMode == INU_DEVICE_POJECTOR_PATTERN_LOW_E)
   {
      setProjMode.projSetMode = INU_DEVICE_POJECTOR_PATTERN_HIGH_E;
   }
   else if (getProjMode.projGetMode == INU_DEVICE_POJECTOR_PATTERN_HIGH_E)
   {
      setProjMode.projSetMode = INU_DEVICE_POJECTOR_FLOOD_LOW_E;
   }
   else if (getProjMode.projGetMode == INU_DEVICE_POJECTOR_FLOOD_LOW_E)
   {
      setProjMode.projSetMode = INU_DEVICE_POJECTOR_FLOOD_HIGH_E;
   }
   else
   {
      setProjMode.projSetMode = INU_DEVICE_POJECTOR_DISABLE_E;
   }

   status = inu_device__setProjectorMode(deviceH, &setProjMode);
   SANDBOX_TEST_RET(status);
#endif

   for (i = 0; i < initParams.numberOfFilesToSend; i++)
   {
      ret = inu_device__writeFileToGp(deviceH, initParams.filesSendList[i], initParams.filesDstNameSendList[i]);
      SANDBOX_TEST_RET(ret);
   }

   // inu_device__powerDown initiate the cyclic power management thread, so it should be called
   // only when we are working with power and we are not in power cycle (i.e initial boot)
   if (initParams.allowPowerDown && !powerCycleMode)
   {
      inu_device__powerDownT powerDownParams;

      powerDownParams.cmd = INU_DEVICE_POWER_DOWN_WAIT_FOR_EVENT_E;
      inu_device__powerDown(deviceH, &powerDownParams);
   }

   if (initParams.loadNetworkEnabled)
   {
      ret = load_networks(deviceH, 1);
      SANDBOX_TEST_RET_ASSERT(ret);
   }

   if (initParams.loadBackgroundEnabled)
   {
       ret = load_background(deviceH, 1);
       SANDBOX_TEST_RET_ASSERT(ret);
   }

   return ret;
}

ERRG_codeE FwLib_deinit(inu_deviceH deviceH)
{
   ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;

   if (!g_device_operational)
      return (ERRG_codeE)ERR_UNEXPECTED;

   if (initParams.alternate)
   {
      inu_device__alternateProjCfgT cfg;
      cfg.alternateModeEnable = 0;
      status = inu_device__alternateProjector(deviceH, &cfg);
      SANDBOX_TEST_RET(status);
   }

   if (initParams.useUartSync)
   {
      ERRG_codeE ret;
      //set device time
      inu_device__set_time_t settime;
      settime.method = INU_DEVICE__SET_TIME_PLL_LOCK_DISABLE_E;
      ret = inu_device__setTime(deviceH, &settime, NULL);
      SANDBOX_TEST_RET(ret);
   }

   closeStreamThreads((thread_params*)logchH);

   if (initParams.loadNetworkEnabled)
   {
      status = load_networks(deviceH, 0);
      SANDBOX_TEST_RET_ASSERT(status);
   }

   if (initParams.loadBackgroundEnabled)
   {
       status = load_background(deviceH, 0);
       SANDBOX_TEST_RET_ASSERT(status);
   }

   printf("use_case_deinit\n");
   inu_device__delete(deviceH); //releases everything
   inu_deinit(powerCycleMode);  //close library

#ifdef _MSC_VER
    // Reset the clock resolution in order to save battery life time
   timeEndPeriod(1);
#endif

   printf("deinit done\n");

   return status;
}

ERRG_codeE FwLib_read_register(inu_deviceH deviceH, FWLib_accessRegisterT *accessRegP)
{
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
   inu_device__readRegT readReg;
   memset(&readReg,0,sizeof(readReg));
   readReg.regType = (inu_device__regTypeE)accessRegP->regType;
   switch (readReg.regType)
   {
      case INU_DEVICE__REG_SOC_E:
      {
         memcpy(&readReg.socReg, &accessRegP->socReg, sizeof(readReg.socReg));
      }break;
      case INU_DEVICE__REG_I2C_E:
      {
         memcpy(&readReg.i2cReg, &accessRegP->i2cReg, sizeof(readReg.i2cReg));
      }break;
      case INU_DEVICE__REG_ATT_E:
      {
         memcpy(&readReg.attReg, &accessRegP->attReg, sizeof(readReg.attReg));
      }break;
      case INU_DEVICE__REG_DPHY_E:
      {
         memcpy(&readReg.dphyReg, &accessRegP->dphyReg, sizeof(readReg.dphyReg));
      }break;
      case INU_DEVICE__REG_TEMP_E:
      {
         memcpy(&readReg.tempReg, &accessRegP->tempReg, sizeof(readReg.tempReg));
      }break;
      default:
         return ret;
         break;
   }
   ret = inu_device__readRegister(deviceH, &readReg);
   switch (readReg.regType)
   {
      case INU_DEVICE__REG_SOC_E:
      {
         accessRegP->socReg.val = readReg.socReg.val;
      }break;
      case INU_DEVICE__REG_I2C_E:
      {
         accessRegP->i2cReg.i2cData = readReg.i2cReg.i2cData;
      }break;
      case INU_DEVICE__REG_ATT_E:
      {
         accessRegP->attReg.val = readReg.attReg.val;
      }break;
      case INU_DEVICE__REG_DPHY_E:
      {
          accessRegP->dphyReg.val = readReg.dphyReg.val;
      }break;
      case INU_DEVICE__REG_TEMP_E:
      {
         accessRegP->tempReg.vali = readReg.tempReg.vali;
         accessRegP->tempReg.valf = readReg.tempReg.valf;
      }break;
      default:
         return ret;
         break;
   }
   return ret;
}

ERRG_codeE FwLib_write_register(inu_deviceH deviceH, FWLib_accessRegisterT *accessRegP)
{
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
   inu_device__writeRegT writeReg;
   memset(&writeReg, 0, sizeof(writeReg));
   writeReg.regType = (inu_device__regTypeE)accessRegP->regType;
   writeReg.phase = INU_DEVICE__DB_PHASE_0_E;
   switch (writeReg.regType)
   {
      case INU_DEVICE__REG_SOC_E:
      {
         memcpy(&writeReg.socReg, &accessRegP->socReg, sizeof(writeReg.socReg));
      }break;
      case INU_DEVICE__REG_I2C_E:
      {
         memcpy(&writeReg.i2cReg, &accessRegP->i2cReg, sizeof(writeReg.i2cReg));
      }break;
      case INU_DEVICE__REG_ATT_E:
      {
         memcpy(&writeReg.attReg, &accessRegP->attReg, sizeof(writeReg.attReg));
      }break;
      default:
         return ret;
         break;
   }
   return inu_device__writeRegister(deviceH, &writeReg);
}

ERRG_codeE FwLib_get_sensor_params(inu_deviceH deviceH, UINT32 sensorId, UINT32 *exposureUsP, float *analogGainP, float *digitalGainP, UINT32 *fps, INT32 *temperature)
{
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
   inu_sensor__runtimeCfg_t cfg;
   inu_sensorH sensorH= NULL;
   char sensorString[128];


   memset(sensorString, 0, sizeof(sensorString));
   sprintf(sensorString, "SENSOR_%d", sensorId);
   ret = inu_device__findNode(deviceH, sensorString, &sensorH);
   if (ERRG_SUCCEEDED(ret))
   {
      cfg.context = INU_SENSOR__CONTEXT_A;
      ret = inu_sensor__getRuntimeCfg(sensorH, &cfg);
      if (ERRG_SUCCEEDED(ret))
      {
         *exposureUsP = cfg.exposure;
         *analogGainP = cfg.gain.analog;
         *digitalGainP = cfg.gain.digital;
         *fps = cfg.fps;
	 *temperature = cfg.temperature;
      }
   }
   return ret;
}

ERRG_codeE FwLib_get_sensorGroup_params(inu_deviceH deviceH, UINT32 sensorGroupId, UINT32 *exposureUsP, float *analogGainP, float *digitalGainP, UINT32 *fps)
{
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
   inu_sensor__runtimeCfg_t cfg;
   inu_sensors_groupH sensors_groupH = NULL;
   char sensorGroupString[128];
   memset(sensorGroupString, 0, sizeof(sensorGroupString));
   sprintf(sensorGroupString, "SENSOR_GROUP_%d", sensorGroupId);
   ret = inu_device__findNode(deviceH, sensorGroupString, &sensors_groupH);
   if (ERRG_SUCCEEDED(ret))
   {
      cfg.context = INU_SENSOR__CONTEXT_A;
      ret = inu_sensors_group__getRuntimeCfg(sensors_groupH, &cfg);
      if (ERRG_SUCCEEDED(ret))
      {
         *exposureUsP = cfg.exposure;
         *analogGainP = cfg.gain.analog;
         *digitalGainP = cfg.gain.digital;
         *fps = cfg.fps;
      }
   }
   return ret;
}

ERRG_codeE FwLib_set_sensorGroup_params(inu_deviceH deviceH, UINT32 sensorGroupId, UINT32 exposureUs, float analogGain, float digitalGain, UINT32 fps)
{
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
   inu_sensor__runtimeCfg_t cfg;
   inu_sensors_groupH sensors_groupH = NULL;
   char sensorGroupString[128];
   memset(sensorGroupString, 0, sizeof(sensorGroupString));
   sprintf(sensorGroupString, "SENSOR_GROUP_%d", sensorGroupId);
   ret = inu_device__findNode(deviceH, sensorGroupString, &sensors_groupH);
   if (ERRG_SUCCEEDED(ret))
   {
      cfg.context = INU_SENSOR__CONTEXT_A;
      ret = inu_sensors_group__getRuntimeCfg(sensors_groupH, &cfg);
      if (ERRG_SUCCEEDED(ret))
      {
         cfg.exposure = exposureUs;
         cfg.gain.analog = analogGain;
         cfg.gain.digital = digitalGain;
         cfg.fps = fps;
         ret = inu_sensors_group__setRuntimeCfg(sensors_groupH, &cfg);
      }
   }
   return ret;
}

ERRG_codeE FwLib_getSensorFocusParams(inu_deviceH deviceH, UINT32 sensorId, INT32 *dac, INT32 *min, INT32 *max, UINT32* chipId)
{
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
   inu_sensor__getFocusParams_t params;
   inu_sensorH sensorsH = NULL;
   char sensorString[128];
   memset(sensorString, 0, sizeof(sensorString));
   sprintf(sensorString, "SENSOR_%d", sensorId);
   ret = inu_device__findNode(deviceH, sensorString, &sensorsH);
   if (ERRG_SUCCEEDED(ret))
   {
      params.context = INU_SENSOR__CONTEXT_A;
      ret = inu_sensor__getFocusParams(sensorsH, &params);
      if (ERRG_SUCCEEDED(ret))
      {
         *dac = params.dac;
         *min = params.min;
         *max = params.max;
         *chipId = params.chipId;
      }
   }
   return ret;
}

ERRG_codeE FwLib_setSensorFocusParams(inu_deviceH deviceH, UINT32 sensorId, INT32 dac, int mode)
{
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
   inu_sensor__setFocusParams_t params;
   inu_sensorH sensorH = NULL;
   char sensorString[128];
   memset(sensorString, 0, sizeof(sensorString));
   sprintf(sensorString, "SENSOR_%d", sensorId);
   ret = inu_device__findNode(deviceH, sensorString, &sensorH);
   if (ERRG_SUCCEEDED(ret))
   {
      params.dac = dac;
      params.mode = (inu_sensor__focusMode_e)mode;
      params.context = INU_SENSOR__CONTEXT_A;
      ret = inu_sensor__setFocusParams(sensorH, &params);
   }
   return ret;
}

ERRG_codeE FwLib_setSensorCropParams(inu_deviceH deviceH, UINT32 sensorId, UINT32 startX, UINT32 startY)
{
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
   inu_sensor__setCropParams_t params;
   inu_sensorH sensorH = NULL;
   char sensorString[128];
   memset(sensorString, 0, sizeof(sensorString));
   sprintf(sensorString, "SENSOR_%d", sensorId);
   ret = inu_device__findNode(deviceH, sensorString, &sensorH);
   if (ERRG_SUCCEEDED(ret))
   {
      params.startX = startX;
      params.startY = startY;
      ret = inu_sensor__setCropParams(sensorH, &params);
   }
   return ret;
}

ERRG_codeE FwLib_ispCommand(inu_refH ref, UINT32 cmd, ...)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   inu_isp_channel__commandE command = (inu_isp_channel__commandE)cmd;
   inu_isp_channel__ispCmdParamU *p, ispCmdParam;
   unsigned int size = 0;
   va_list args;

   switch(cmd)
   {
      case INU_ISP_COMMAND_SET_EXPOSURE_E:
         va_start(args, cmd);

         p = &ispCmdParam;
         p->exposureParam.integrationTime = (float)va_arg(args, double);
         p->exposureParam.analogGain      = (float)va_arg(args, double);
         p->exposureParam.ispGain         = (float)va_arg(args, double);
         va_end(args);

         printf("%s :ISP_COMMAND_SET_EXPOSURE: intTime %f anGain %f ispGain %f\n",
               __func__, p->exposureParam.integrationTime, p->exposureParam.analogGain, p->exposureParam.ispGain);
         ret = inu_isp_channel__sendIspCommand(ref, command, p);
   break;

   case INU_ISP_COMMAND_GET_EXPOSURE_E:
         p = &ispCmdParam;
         ret = inu_isp_channel__sendIspCommand(ref, command, p);
         printf("%s :ISP_COMMAND_GET_EXPOSURE: intTime %f anGain %f ispGain %f\n",
                  __func__, p->exposureParam.integrationTime, p->exposureParam.analogGain, p->exposureParam.ispGain);
      break;

      case INU_ISP_COMMAND_SET_WB_E:
         va_start(args, cmd);

         p = &ispCmdParam;
         p->wbParam.wbGain.gainB    = (float)va_arg(args, double);
         p->wbParam.wbGain.gainGb   = (float)va_arg(args, double);
         p->wbParam.wbGain.gainGr   = (float)va_arg(args, double);
         p->wbParam.wbGain.gainR    = (float)va_arg(args, double);

         p->wbParam.ccOffset.blue   = (int)va_arg(args, int);
         p->wbParam.ccOffset.green  = (int)va_arg(args, int);
         p->wbParam.ccOffset.red    = (int)va_arg(args, int);

         size = sizeof(p->wbParam.ccMatrix.coeff)/sizeof(p->wbParam.ccMatrix.coeff[0]);
         for (uint32_t i = 0; i< size; i++) {
               p->wbParam.ccMatrix.coeff[i] = (float)va_arg(args, double);
         }
         va_end(args);

         printf("ISPCORE_MODULE_WB_CFG_SET ::: Setting ->>>>>>>\n");
         printf("ISPCORE_MODULE_WB_CFG_GET: wbGains.Blue:[%.3f], wbGains.GreenB:[%.3f], wbGains.GreenR:[%.3f], wbGains.Red:[%.3f]\n",
         p->wbParam.wbGain.gainB, p->wbParam.wbGain.gainGb, p->wbParam.wbGain.gainGr,p->wbParam.wbGain.gainR);
         printf("ISPCORE_MODULE_WB_CFG_GET: ccOffset.Blue:[%d], ccOffset.Green:[%d], ccOffset.Red:[%d]\n",
         p->wbParam.ccOffset.blue, p->wbParam.ccOffset.green, p->wbParam.ccOffset.red);
         for (uint32_t i = 0; i< size; i++) {
            printf("ISPCORE_MODULE_WB_CFG_GET cc_coeff[%d]: %.3f\n",i,p->wbParam.ccMatrix.coeff[i]);
         }

         ret = inu_isp_channel__sendIspCommand(ref, command, p);
      break;

      case INU_ISP_COMMAND_GET_WB_E:
         p = &ispCmdParam;
         ret = inu_isp_channel__sendIspCommand(ref, command, p);

         size = sizeof(p->wbParam.ccMatrix.coeff)/sizeof(p->wbParam.ccMatrix.coeff[0]);

         printf("ISPCORE_MODULE_WB_CFG_SET ::: Getting <<<<<<<-\n");
         printf("ISPCORE_MODULE_WB_CFG_GET: wbGains.Blue:[%.3f], wbGains.GreenB:[%.3f], wbGains.GreenR:[%.3f], wbGains.Red:[%.3f]\n",
         p->wbParam.wbGain.gainB, p->wbParam.wbGain.gainGb, p->wbParam.wbGain.gainGr,p->wbParam.wbGain.gainR);
         printf("ISPCORE_MODULE_WB_CFG_GET: ccOffset.Blue:[%d], ccOffset.Green:[%d], ccOffset.Red:[%d]\n",
         p->wbParam.ccOffset.blue, p->wbParam.ccOffset.green, p->wbParam.ccOffset.red);
         for (uint32_t i = 0; i< size; i++) {
            printf("ISPCORE_MODULE_WB_CFG_GET cc_coeff[%d]: %.3f\n",i,p->wbParam.ccMatrix.coeff[i]);
         }
      break;

      case INU_ISP_COMMAND_SET_LSC_E:
         va_start(args, cmd);
         p = &ispCmdParam;
         p->lscParam.set_by_file    = (int32_t)va_arg(args, int32_t);
         strcpy(p->lscParam.file_name, (const char *)va_arg(args, char *));
         va_end(args);
         //printf("ISPCORE_MODULE_LSC_CFG_SET ::: Setting ->>>>>>> set_by_file:[%d], file name =%s\n",p->lscParam.set_by_file,p->lscParam.file_name);
         ret = inu_isp_channel__sendIspCommand(ref, command, p);
      break;


      case INU_ISP_COMMAND_GET_LSC_E:
         va_start(args, cmd);
         p = &ispCmdParam;
         p->lscParam.set_by_file    = (int32_t)va_arg(args, int32_t);
         va_end(args);
         //printf("ISPCORE_MODULE_LSC_CFG_GET: set_by_file:[%d]\n",p->lscParam.set_by_file);
         ret = inu_isp_channel__sendIspCommand(ref, command, p);
      break;

      default:
         printf("!!!!!!!!!! %s:: command %d is not valid\n", __func__, command);
      break;
   }

   return ret;
}

ERRG_codeE FwLib_set_manual_exposure_params(inu_deviceH deviceH, UINT32 sensorGroupId, UINT32 exposureUs, float analogGain, float digitalGain)
{
   ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
   inu_sensor_control__manualExposure_t manualExposure;
   inu_sensor_controlH sensor_controlH = NULL;
   char sensorControlString[128];

   memset(sensorControlString, 0, sizeof(sensorControlString));
   sprintf(sensorControlString, "SENSOR_CONTROL_%d", sensorGroupId);
   ret = inu_device__findNode(deviceH, sensorControlString, &sensor_controlH);

   if (ERRG_SUCCEEDED(ret))
   {
      manualExposure.exposureTimeUsec = exposureUs;
      manualExposure.gain.analog = analogGain;
      manualExposure.gain.digital = digitalGain;
      manualExposure.context = INU_SENSOR__CONTEXT_A;
#ifdef ENABLE_SENOSR_CONTROl
      ret = inu_sensor_control__setManualExp(sensor_controlH, &manualExposure);
#endif
   }
   else
   {
      printf("%s:: SENSOR_CONTROL Node was not found 0x%x\n", __func__, ret);
   }
   return ret;
}
ERRG_codeE FWLIB_getWBGains(inu_refH ref, inu_isp_channel__ispCmdParamU *ispCommand )
{
   inu_isp_channel__commandE command = (inu_isp_channel__commandE)INU_ISP_COMMAND_GET_WB_E;
   ERRG_codeE ret = inu_isp_channel__sendIspCommand(ref, command, ispCommand);
   return ret;
}
ERRG_codeE FWLIB_getExpoParams(inu_refH ref, inu_isp_channel__ispCmdParamU *ispCommand )
{
   inu_isp_channel__commandE command = (inu_isp_channel__commandE)INU_ISP_COMMAND_GET_EXPOSURE_E;
   ERRG_codeE ret = inu_isp_channel__sendIspCommand(ref, command, ispCommand);
   return ret;
}
ERRG_codeE FwLib_enableTuningServer(inu_deviceH deviceH,UINT8 sensorNum)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   inu_device__tuningParamsT tsParams;
   tsParams.sensorNum= sensorNum;

   ret = inu_device__enableTuningServer(deviceH, &tsParams);

   return ret;
}
/**
 * @brief Serializes INU_Metadata_T into a buffer
 *
 *
 * @param metadata INU_Metadata_T pointer
 * @param buffer Buffer to serialize into
 * @param size Size of the buffer
 * @return Returns true if successful and false if there's an error
 */
 bool FWLib_serializeMetadata(INU_Metadata_T* metadata, char* buffer, UINT32 size)
{
    return inu_metadata__serialize(buffer, size, metadata);
}
 bool FWLib_serializeMetadataNoDelimit(INU_Metadata_T* metadata, char* buffer, UINT32 size, UINT32 *outputSize)
{
    return inu_metadata__serializeWithoutDelimiter(buffer, size, metadata, outputSize);
}
/* @brief Gets the size of metadata without padding 
 */
 int FWLib_GetMetadataSizeWithoutLinePadding(char *buffer, size_t size)
{
    return inu_metadata__getMetadataSizeWithoutLinePadding(buffer, size);
}
#ifdef DRAW_CUBE
#define vec3 std::array<float, 3>
#define vec4 std::array<float, 4>
#define pointMZ std::array<int, 2>
void MultiplyPoseByVec_nativeC(vec3 vec, float*quaternionP, float *translationP, vec3 &out)
{
   float uv[3];
   float vecP[3] = { vec[0], vec[1], vec[2] };
   float resP[3];
   uv[0] = 2 * (quaternionP[1] * vecP[2] - quaternionP[2] * vecP[1]);
   uv[1] = 2 * (quaternionP[2] * vecP[0] - quaternionP[0] * vecP[2]);
   uv[2] = 2 * (quaternionP[0] * vecP[1] - quaternionP[1] * vecP[0]);
   resP[0] = vecP[0] + quaternionP[3] * uv[0] + (quaternionP[1] * uv[2] - quaternionP[2] * uv[1]);
   resP[1] = vecP[1] + quaternionP[3] * uv[1] + (quaternionP[2] * uv[0] - quaternionP[0] * uv[2]);
   resP[2] = vecP[2] + quaternionP[3] * uv[2] + (quaternionP[0] * uv[1] - quaternionP[1] * uv[0]);
   resP[0] += translationP[0];
   resP[1] += translationP[1];
   resP[2] += translationP[2];
   out[0] = resP[0];
   out[1] = resP[1];
   out[2] = resP[2];
}
void DrawCube(cv::Mat &image)
{
   int i, w;
   vector<vec3> worldPoints;
   float quaternion[4] = { slamPos[0],slamPos[1],slamPos[2],slamPos[3] };
   float translation[3] = { slamPos[4],slamPos[5],slamPos[6] };
   float front = 0.5f;
   float back = 0.7f;
   printf("%f %f %f %f      %f %f %f\n", slamPos[0], slamPos[1], slamPos[2], slamPos[3], slamPos[4], slamPos[5], slamPos[6]);
   worldPoints.push_back({ -0.1f, -0.1f, front });
   worldPoints.push_back({ 0.1f, -0.1f, front });
   worldPoints.push_back({ 0.1f, 0.1f, front });
   worldPoints.push_back({ -0.1f, 0.1f, front });
   worldPoints.push_back({ -0.1f, -0.1f, back });
   worldPoints.push_back({ 0.1f, -0.1f, back });
   worldPoints.push_back({ 0.1f, 0.1f, back });
   worldPoints.push_back({ -0.1f, 0.1f, back });
   vector<cv::Point> drawPoints;
   vector<vec3> pcPoints;
   for (w = 0; w < worldPoints.size(); w++)
   {
      vec3 pc;
      MultiplyPoseByVec_nativeC(worldPoints[w], quaternion, translation, pc);
      pcPoints.push_back(pc);
      cv::Point point;
      point.x = (pc[0] * slamHostGPmsg.fx / pc[2] + slamHostGPmsg.cx + 0.5);
      point.y = (pc[1] * slamHostGPmsg.fy / pc[2] + slamHostGPmsg.cy + 0.5);
      drawPoints.push_back(point);
   }
   vector<pair<int, int>> lines = {
      { 0, 1 },{ 1,2 },{ 2,3 },{ 3,0 },
      { 0, 4 },{ 1,5 },{ 2,6 },{ 3,7 },
      { 4, 5 },{ 5,6 },{ 6,7 },{ 7,4 }
   };
   float minZ = 0.005f;
   for (i = 0; i < lines.size(); i++)
   {
      int thickness = (i < 4) ? 3 : ((i < 8) ? 2 : 1);
      cv::Point p1 = drawPoints[lines[i].first];
      cv::Point p2 = drawPoints[lines[i].second];
      if (((p1.x >= 0 && p1.x < 640 && p1.y >= 0 && p1.y < 480) ||
         (p2.x >= 0 && p2.x < 640 && p2.y >= 0 && p2.y < 480)) &&
         (pcPoints[lines[i].first][2] > minZ || pcPoints[lines[i].second][2] > minZ))
      {
         if (pcPoints[lines[i].first][2] <= minZ)
         {
            vec3 diff;
            for (int d = 0; d < 3; d++)
               diff[d] = pcPoints[lines[i].first][d] - pcPoints[lines[i].second][d];
            float t = (minZ - pcPoints[lines[i].second][2]) / diff[2];
            vec3 newPoint;
            for (int d = 0; d < 3; d++)
               newPoint[d] = pcPoints[lines[i].second][d] + t * diff[d];
            p1.x = (newPoint[0] * slamHostGPmsg.fx / newPoint[2] + slamHostGPmsg.cx + 0.5);
            p1.y = (newPoint[1] * slamHostGPmsg.fy / newPoint[2] + slamHostGPmsg.cy + 0.5);
         }
         else if (pcPoints[lines[i].second][2] <= minZ)
         {
            vec3 diff;
            for (int d = 0; d < 3; d++)
               diff[d] = pcPoints[lines[i].second][d] - pcPoints[lines[i].first][d];
            float t = (minZ - pcPoints[lines[i].first][2]) / diff[2];
            vec3 newPoint;
            for (int d = 0; d < 3; d++)
               newPoint[d] = pcPoints[lines[i].first][d] + t * diff[d];
            p2.x = (newPoint[0] * slamHostGPmsg.fx / newPoint[2] + slamHostGPmsg.cx + 0.5);
            p2.y = (newPoint[1] * slamHostGPmsg.fy / newPoint[2] + slamHostGPmsg.cy + 0.5);
         }
         cv::line(image, p1, p2, cv::Scalar(255), thickness);
      }
   }
}

#endif   // DRAW_CUBE
