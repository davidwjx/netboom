#include "inu2.h"
#include <time.h>
#include "SandBoxFWLib/SandBoxLib.h"
#include "Sandbox.h"
#include <string.h>
#include <assert.h>
#include "utils.h"
#include "inttypes.h"
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iostream>
#define MAX_AF_VST_CROP_WINDOW_X 2815 /*Based off the register table we are using, this is the current AF VST cropWindow X Limit*/
#define MAX_AF_VST_CROP_WINDOW_Y 2111 /*Based off the register table we are using, this is the current AF VST cropWindow Y Limit*/
#define MAX_AF_VST_FOCUS 800
#define MIN_AF_VST_FOCUS 0
#define AF_VST_CROP_WIDTH 544
#define AF_VST_CROP_HEIGHT 384
#define AF_VST_DOWNSCALED_WIDTH 1120
#define AF_VST_DOWNSCALED_HEIGHT 832
#define AF_VST_NATIVE_WIDTH 3360
#define AF_VST_NATIVE_HEIGHT 2496
//#define DEBUG_CROP_OVERLAY
UINT32 cropStartX = 0;
typedef struct 
{
	UINT32 cropStartX  = 0;
	UINT32 cropStartY = 0;
	UINT32 frameID = 0;
	UINT32 EOF_SOF_Latency = 0;
	UINT32 EOFFrameID = 0;
	UINT32 ISP0FrameID = 0;
	UINT32 ISP1FrameID= 0;
	UINT32 softwareInjectionCounter = 0;
} FrameStats;


using namespace std;

#if defined _OPENCV_
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#endif


#define RECORD_FOLDER_NAME "record"
#if (defined _WIN32) || (defined _WIN64)
#define CREATE_RECORD_FOLDER "\"\"mkdir " RECORD_FOLDER_NAME
#define BACK_SLASH "\\"
#else
#define CREATE_RECORD_FOLDER "mkdir " RECORD_FOLDER_NAME
#define BACK_SLASH "/"
#endif

#define SANDBOX_TEST_RET_ASSERT(ret) if (ERRG_FAILED(ret)) assert(0);


#define GET_WIN_NAME(winName, streamName, width, height)   sprintf(winName, "%s  %d X %d", streamName, width, height);

#define STREAM_STATS_PERIOD (30)
#define STREAM_STATS_DROPPED_THRESHOLD (0.1)//10% dropped frame

/* basic demo definitions */
#define CORE_XM4                             0
#define CORE_EV62                            1
#define CORE_GP                              2
#define CORES_NUM                            3

#define LOC_MAXLEN (12*1024)

//cv
#define Y_CONST (1.1640625f)
#define BU_CONST (2.015625f)
#define GU_CONST (-0.390625f)
#define GV_CONST (-0.8125f)
#define RV_CONST (1.59765625f)

#define CONST_16 (16.0f)
#define CONST_128 (128.0f)
#define SHIFT (10)
#define C1192 ((int)(Y_CONST * (1<<SHIFT)))
#define C2066 ((int)(BU_CONST * (1<<SHIFT)))
#define C832 ((int)(-GV_CONST * (1<<SHIFT)))
#define C400 ((int)(-GU_CONST * (1<<SHIFT)))
#define C1634 ((int)(RV_CONST * (1<<SHIFT)))
#define C16 ((int)CONST_16)
#define C128 ((int)CONST_128)
#define ROUND (1<<(SHIFT - 1))

#define min(a,b) (a<b ? a : b)
#define max(a,b) (a>b ? a : b)

typedef struct
{
	UINT32 xm4Cycles;
	UINT32 ev62Cycles;
	UINT32 gpUsec;
	UINT32 counter;
	// you may add more parameters here
} BASIC_DEMO_gpHostMsgStructT;

typedef struct
{
	UINT32 enable;
	UINT32 startX;
	UINT32 startY;
	UINT32 picWidth;
	UINT32 picHeight;
	// you may add more parameters here
} BASIC_DEMO_algInfoStructT;

typedef struct
{
	BASIC_DEMO_algInfoStructT algInfo[CORES_NUM];
	// you may add more parameters here
} BASIC_DEMO_hostGpMsgStructT;

typedef struct
{
	UINT64 counter;
	UINT32 cycles;
	INT16 matchedIdx[800];
} MATCHER_DEMO_gpHostMsgStructT;

void startAllStreamsExample(inu_deviceH* device);
typedef struct {
	int debug;
	int label;
	float confidence;
	float x_min, y_min, x_max, y_max;
} BBOX;
float bx[20];
float by[20];
float bxe[20];
float bye[20];
float object_confidence[20];
int num_box = 0;
int label[20] = { 0 };
char classes[20][30];
char object_names[20][30];
float slamPos[7];
BASIC_DEMO_hostGpMsgStructT hostGPmsg;
int aiDraw = 0;
int num_of_objects = 0;
int first_frame = 1;
#if 0
static char *copy_buf = NULL;
#endif
#define MAX_NUM_CLASSES 20 //mobilenet_ssd labels

#if defined _OPENCV_
static cv::Mat depthLut;
typedef struct 
{
	FrameStats stats;
	cv::Mat Image;
	INU_Metadata_T metadata;
} Image_With_Metadata;
#include <map>
std::map<uint32_t, Image_With_Metadata> croppedMap;
std::map<uint32_t, Image_With_Metadata> downscaledMap;
#define MAX_COLORS 6
static cv::Scalar class_colors[MAX_COLORS]{
	 { 255, 0, 128 }, { 255, 0, 255 }, { 0, 255, 0 }, { 0, 0, 255 }, { 0, 255, 255 }, { 255, 255, 192 }
};
#endif

static void printDepth(char *data_tempP, inu_image__hdr_t *imgHdrP)
{
	UINT16 *pixelPtr, *linePtr;
	UINT32 startX, startY, picWidth = 5, picHeight = 5;
	UINT16 i = 0, j = 0;

	//Window should be in the middle, depending on resolution
	startX = (imgHdrP->imgDescriptor.width)/2;
	startY = (imgHdrP->imgDescriptor.height)/2;

	linePtr = ((UINT16 *)data_tempP + (imgHdrP->imgDescriptor.width*startY) + startX);

	for(i=0; i < picHeight; i++)
	{
		pixelPtr = linePtr;
		for (j=0; j<picWidth; j++)
		{
			printf("%d ,", *pixelPtr);
			pixelPtr++;
		}
		printf("\n");
		linePtr += imgHdrP->imgDescriptor.width;
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
#else
#include <sys/time.h>
static void OS_LYRG_getUsecTime(UINT64 *usecP)
{
	struct timeval time;
	if (gettimeofday(&time, NULL) == -1)
	{
		printf("gettimeofday failed\n");
}
	*usecP = (time.tv_sec * 1000000) + time.tv_usec;
	}
#endif
static  void startAllTriggersExample(inu_deviceH* device);
static void streamStatsInit(SANDBOX_channelParamsT *paramP)
{
	paramP->totalFramesReceived = 0;
	paramP->prevCtr = 0;
	paramP->printPeriod = 0;
	paramP->periodReceived = 0;
	paramP->bytesReceived = 0;
	paramP->latencyMax = 0;
	paramP->latencyMin = 0xFFFFFFFF;
	paramP->latencySum = 0;
	paramP->periodNumDropped = 0;
	paramP->totalNumDropped = 0;
	paramP->minFPS = 999;
	paramP->maxFPS = -999;
	paramP->metadata_time_value = std::vector < UINT64 >();
	paramP->metadata_time_CVA_Left_value = std::vector < UINT64 >();
	paramP->metadata_time_CVA_Right_value = std::vector < UINT64 >();
}
static double convertMetadataTimeToDouble(UINT64 metadata_time)
{
	UINT32 second = (metadata_time >> 32); 
	UINT32 frac_part = (metadata_time & 0xFFFFFFFF);
	float frac_s = (double) frac_part/((double)0xFFFFFFFF);	/* Scale frac_part to be from 0->1*/
	return (double) second + (double) frac_s;
}
/**
 * @brief Converts RTC time into nanoseconds
 * @param metadata_time RTC based time reading
 * @return Returns time in nanoseconds
 */
static UINT64 convertRTCTimeToNS(UINT64 rtc_time)
{
	UINT64 second = (rtc_time >> 32);
	UINT64 frac_part = (rtc_time & 0xFFFFFFFF);
	double nanoseconds = (double)1E9* (double)frac_part/((double)0xFFFFFFFF);	/* Scale frac_part to be from 0->1*/
	return 1000000000*second + (UINT64) nanoseconds;
}
std::vector<double> convertTimestampsIntoTimeStampDifferences(std::vector<UINT64> timestampVec)
{
	std::vector<double> timeDiffs;
	for (int i =1; i< timestampVec.size(); i++)
	{
		const double t_now = convertMetadataTimeToDouble(timestampVec.at(i));
		const double t_previous = convertMetadataTimeToDouble(timestampVec.at(i-1));
		timeDiffs.push_back(t_now-t_previous);
	}
	return timeDiffs;
}
/**
 * @brief Calculates the mean on a vector
 * 
 *
 * @param input A vector containing samples to calculate the mean on
 * @return Returns the mean
 */
static double calculateMean(std::vector<double> &input)
{
	double sum = 0.0;
	for(auto &x : input) 
	/*Iterate over each element in the  vector*/
	{
		sum += x;	/*Calculate the sum */
	}
	/*Calculate the mean */
	return sum/input.size();
}
/**
 * @brief Calculates the Standard deviation for a vector named input
 * 
 *
 * @param timeValues Input to calculate the standard deviation on
 * @return Returns standard deviation as double
 */
static double calculateStandardDeviation(std::vector<double> input, double mean)
{
	double standardDeviation = 0;
	for(auto &diff : input)
	{
		standardDeviation += pow(diff - mean,2);
	}
	return sqrt(standardDeviation/input.size());	
 
}
static void streamStatsProcess(SANDBOX_channelParamsT *paramP, inu_data__hdr_t *dataHdrP, inu_refH refH, int size, INU_Metadata_T *metadata)
{
	float fps;
	float throughput;
	float total;
	UINT64 latencyUs, currentUs;
	time_t current_time;
	struct tm* time_info;
	char timeString[20];  // plenty of space for "HH:MM:SS\0"

	OS_LYRG_getUsecTime(&currentUs);
	latencyUs = currentUs - (dataHdrP->timestamp / 1000);
	if (paramP->prevCtr)
	{
		UINT64 diff = (dataHdrP->dataIndex - paramP->prevCtr);
		if (diff > 1)
		{
			paramP->periodNumDropped += (diff -1);
			//printf("dropped %d frames %s. (current = %d, prev = %d).\n", diff - 1, inu_ref__getUserName(refH), dataHdrP->dataIndex,paramP->prevCtr);
		 paramP->totalNumDropped += paramP->periodNumDropped;
		}

		if (paramP->printPeriod++ > STREAM_STATS_PERIOD)
		{
			OS_LYRG_getUsecTime(&paramP->endUs);
			total = (float)(paramP->endUs - paramP->startUs);
			fps = (paramP->periodReceived / (total / 1000000));
			throughput = (paramP->bytesReceived / (total / 1000000));
			time(&current_time);
			time_info = localtime(&current_time);
			strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
			printf("%s:", timeString);

			// NB - we ignore the first sample period as the stream starts
			if ((paramP->totalFramesReceived > 50) && (fps > paramP->maxFPS))
			{
				paramP->maxFPS = fps;
			}

			// NB - we ignore the first sample period as the stream starts
			if ((paramP->totalFramesReceived > 50) && (fps < paramP->minFPS))
			{
				paramP->minFPS = fps;
			}
			if(paramP->metadata_time_value.size() > 0 && paramP->metadata_time_CVA_Left_value.size() == 0 && paramP->metadata_time_CVA_Right_value.size() == 0) 
			{	/* If we have metadata then we can also print metadata timestamp stats*/
				std::vector<double> MetadataTimeDifferences = convertTimestampsIntoTimeStampDifferences(paramP->metadata_time_value); 					/*Convert timestamps into timestamp differences (seconds)*/
				if(MetadataTimeDifferences.size() > 0)
				{
					const double metadataTimestampDiffMean_seconds = calculateMean(MetadataTimeDifferences); 												/*Calculate the mean timestamp difference in seconds*/
					const double metadataTimestampDiffSD_seconds  = calculateStandardDeviation(MetadataTimeDifferences,metadataTimestampDiffMean_seconds);  /*Calculate the standard deviation of the timestamp difference in seconds*/
		
					double metadatTimestampDiffMax_seconds = 0;
					double metadatTimestampDiffMin_seconds = 0;

					if(MetadataTimeDifferences.size() > 0)
					{
						metadatTimestampDiffMax_seconds = *std::max_element(std::begin(MetadataTimeDifferences), std::end(MetadataTimeDifferences)); 
						metadatTimestampDiffMin_seconds = *std::min_element(std::begin(MetadataTimeDifferences), std::end(MetadataTimeDifferences));
					}
				
					printf("%s ref %d: period %f, frames %.4" PRId64 ", fps: %.2f (%.2f : %.2f), throughput: %.10d, latency[us]: min %" PRId64 ", max %" PRId64 ", ave %" PRId64 ", dropped frames %" PRId64 ", Metadata Timestamp difference mean %.3f ms,"
						"Metadata timestamp difference standard deviation: %.6f μs,Metadata timestamp difference maximum: %.6f ms, Metadata timestamp difference minimum: %.6f ms  \n",
						inu_ref__getUserName(refH), inu_ref__getRefId(refH), total / 1000000, paramP->totalFramesReceived, fps, paramP->minFPS, paramP->maxFPS, (UINT32)throughput, paramP->latencyMin, paramP->latencyMax, paramP->latencySum / paramP->periodReceived, paramP->periodNumDropped,
						metadataTimestampDiffMean_seconds * 1000.0, metadataTimestampDiffSD_seconds * 1.0E6, metadatTimestampDiffMax_seconds * 1.0E3, metadatTimestampDiffMin_seconds * 1.0E3);
				}
			}
			else if((paramP->metadata_time_CVA_Left_value.size() > 0) && (paramP->metadata_time_CVA_Right_value.size() > 0 ))
			{
				std::vector<double> CVA_LEFT_MetadataTimeDifferences = convertTimestampsIntoTimeStampDifferences(paramP->metadata_time_CVA_Left_value); 					/*Convert timestamps into timestamp differences (seconds)*/
				const double CVA_LEFT_metadataTimestampDiffMean_seconds = calculateMean(CVA_LEFT_MetadataTimeDifferences); 												/*Calculate the mean timestamp difference in seconds*/
				const double CVA_LEFT_metadataTimestampDiffSD_seconds  = calculateStandardDeviation(CVA_LEFT_MetadataTimeDifferences,CVA_LEFT_metadataTimestampDiffMean_seconds);  /*Calculate the standard deviation of the timestamp difference in seconds*/
				const double CVA_LEFT_metadatTimestampDiffMax_seconds = *std::max_element(std::begin(CVA_LEFT_MetadataTimeDifferences), std::end(CVA_LEFT_MetadataTimeDifferences)); 
				const double CVA_LEFT_metadatTimestampDiffMin_seconds = *std::min_element(std::begin(CVA_LEFT_MetadataTimeDifferences), std::end(CVA_LEFT_MetadataTimeDifferences)); 
				std::vector<double> CVA_RIGHT_MetadataTimeDifferences = convertTimestampsIntoTimeStampDifferences(paramP->metadata_time_CVA_Right_value); 					/*Convert timestamps into timestamp differences (seconds)*/
				const double CVA_RIGHT_metadataTimestampDiffMean_seconds = calculateMean(CVA_RIGHT_MetadataTimeDifferences); 												/*Calculate the mean timestamp difference in seconds*/
				const double CVA_RIGHT_metadataTimestampDiffSD_seconds  = calculateStandardDeviation(CVA_RIGHT_MetadataTimeDifferences,CVA_RIGHT_metadataTimestampDiffMean_seconds);  /*Calculate the standard deviation of the timestamp difference in seconds*/
				const double CVA_RIGHT_metadatTimestampDiffMax_seconds = *std::max_element(std::begin(CVA_RIGHT_MetadataTimeDifferences), std::end(CVA_RIGHT_MetadataTimeDifferences)); 
				const double CVA_RIGHT_metadatTimestampDiffMin_seconds = *std::min_element(std::begin(CVA_RIGHT_MetadataTimeDifferences), std::end(CVA_RIGHT_MetadataTimeDifferences));
				printf("%s ref %d: period %f, frames %.4" PRId64 ", fps: %.2f (%.2f : %.2f), throughput: %.10d, latency[us]: min %" PRId64 ", max %" PRId64 ", ave %" PRId64 ", dropped frames %" PRId64 ", CVA Metadata Left Timestamp difference mean %.3f ms,"
					   "CVA Metadata Left timestamp difference standard deviation: %.6f μs, CVA Metadata Right Timestamp difference mean %.3f ms"
					   "CVA Metadata Right timestamp difference standard deviation: %.6f μs \n",
					   inu_ref__getUserName(refH), inu_ref__getRefId(refH), total / 1000000, paramP->totalFramesReceived, fps, paramP->minFPS, paramP->maxFPS, (UINT32)throughput, paramP->latencyMin, paramP->latencyMax, paramP->latencySum / paramP->periodReceived, paramP->periodNumDropped,
					   CVA_LEFT_metadataTimestampDiffMean_seconds * 1000.0, CVA_LEFT_metadataTimestampDiffSD_seconds * 1.0E6, CVA_RIGHT_metadataTimestampDiffMean_seconds * 1000.0, CVA_RIGHT_metadataTimestampDiffSD_seconds * 1.0E6);
			}
		
			else
			{
				printf("%s ref %d: period %f, frames %.4" PRId64 ", fps: %.2f (%.2f : %.2f), throughput: %.10d, latency[us]: min %" PRId64 ", max %" PRId64 ", ave %" PRId64 ", dropped frames %" PRId64 ", Metadata Timestamp difference mean: N/A, Metadata timestamp difference standard deviation: N/A \n", 
				inu_ref__getUserName(refH), inu_ref__getRefId(refH), total / 1000000, paramP->totalFramesReceived, fps, paramP->minFPS, paramP->maxFPS, (UINT32)throughput, paramP->latencyMin, paramP->latencyMax, paramP->latencySum / paramP->periodReceived, paramP->periodNumDropped);
			}
			paramP->printPeriod = 0;
			paramP->periodReceived = 0;
			paramP->bytesReceived = 0;
			paramP->latencySum = 0;
			paramP->latencyMax = 0;
			paramP->latencyMin = 0xFFFFFFFF;
			paramP->periodNumDropped = 0;
			paramP->metadata_time_value.clear();
			paramP->metadata_time_CVA_Left_value.clear();
			paramP->metadata_time_CVA_Right_value.clear();
			OS_LYRG_getUsecTime(&paramP->startUs);
		}
	}
	else
	{
		OS_LYRG_getUsecTime(&paramP->startUs);
	}
	paramP->prevCtr = dataHdrP->dataIndex;
	paramP->totalFramesReceived++;
	paramP->periodReceived++;
	paramP->bytesReceived += size;
	paramP->latencySum += latencyUs;

	if(metadata)
	{
		/*Deal with CVA case where we have one stream having two different outputs (one for the left side, one for the right side)*/
		if(metadata->protobuf_packet.sensors_count == 1 && metadata->protobuf_packet.sensors[0].sensorType == INU_metadata_SensorType_CVA)
		{	
			if(metadata->protobuf_packet.sensors[0].side == INU_metadata_Channel_Side_Left)
			{
				paramP->metadata_time_CVA_Left_value.push_back(metadata->timestamp);	
			}
			else
			{
				paramP->metadata_time_CVA_Right_value.push_back(metadata->timestamp);	
			}
		}
		else
		{
			paramP->metadata_time_value.push_back(metadata->timestamp);	/*Store all timstamps to make it easy to do analysis on the time stamp differences*/
		}
		
	}
	
	(latencyUs > paramP->latencyMax) ? paramP->latencyMax = latencyUs : paramP->latencyMax;
	(latencyUs < paramP->latencyMin) ? paramP->latencyMin = latencyUs : paramP->latencyMin;
}

static void saveDataToDisk(char *ptr, int size, char *filename)
{
	FILE* outFile = fopen(filename, "wb");
	fwrite(ptr, 1, size, outFile);
	fflush(outFile);
	fclose(outFile);
}
#if 0
static void saveImageAsCiifToDisk(char *dataP, int size, char *filename, UINT32 width, UINT32 height)
{
	UINT8  wString[20] = { 0 };
	UINT8  wString1[18] = { 0 };
	FILE* imgCiifFile = fopen(filename, "wb");
	(void)size;

	//printf("image arrived. buffsize 0x%08x w- %d, h -%d bpp - %d \n", imgHdrP->imgDescriptor.bufferHeight, width, imgHdrP->imgDescriptor.height, imgHdrP->imgDescriptor.bitsPerPixel);
	sprintf((char*)(&wString1), "//Frame_width=%d", width);
	fwrite(wString1, 1, sizeof(wString1), imgCiifFile);
	fwrite("\n", sizeof(char), 1, imgCiifFile); memset(&wString1, 0x00, sizeof(wString1));// printf("%s", wString1);
	sprintf((char*)(&wString1), "//Frame_height=%d", height);
	fwrite(wString1, 1, sizeof(wString1), imgCiifFile);
	fwrite("\n", sizeof(char), 1, imgCiifFile); memset(&wString1, 0x00, sizeof(wString1));// printf("%s", wString1);

	fwrite("//Number_disparities=1\n", 1, (sizeof("//Number_disparities=1") + 0), imgCiifFile);
	fwrite("//Number_frames=1\n", 1, (sizeof("//Number_frames=1") + 0), imgCiifFile);
	fwrite("//Pixel_format=8\n", 1, (sizeof("//Pixel_format=8") + 0), imgCiifFile);
	fwrite("//Line_direction=LeftToRight\n", 1, (sizeof("//Line_direction=LeftToRight") + 0), imgCiifFile);
	fwrite("//Data_format=Hex\n", 1, (sizeof("//Data_format=Hex") + 0), imgCiifFile);

	for (UINT32 y = 0; y < height; y++)
	{
		for (UINT32 x = 0; x < width; x++)
		{
			sprintf((char*)(&wString), "(0, %03d, %04d) %02x\r\n", y, x, ((*(dataP + (y * width) + x)) >> 2/* & 0xFF*/));
			fwrite(wString, 1, sizeof(wString), imgCiifFile);
			//                  fwrite("\n", sizeof(char), 1, imgCiifFile);
		}
	}

	fflush(imgCiifFile);
	fclose(imgCiifFile);
}
#endif

static void saveBufferAsTextToDisk(inu_refH ref, char *ptr, inu_data__hdr_t *hdrP)
{
	char wFileName[64];
	sprintf((char*)(&wFileName), "%s%s%s_frame_%" PRId64 ".txt", RECORD_FOLDER_NAME,BACK_SLASH, inu_ref__getUserName(ref), hdrP->dataIndex);
	FILE* outFile = fopen(wFileName, "wb");
	fwrite(ptr, 1, strlen(ptr), outFile);
	fflush(outFile);
	fclose(outFile);
}

#if defined _OPENCV_
void CreateCVWindow(const char *title, UINT32 width, UINT32 height)
{
	char str[100];

   GET_WIN_NAME(str, title, width, height)
   cv::namedWindow(str, cv::WINDOW_NORMAL);
   cv::resizeWindow(str, width, height);
}

static const unsigned char ALPHA_TRANSPARENT = 0xff;
static inline void Insert(unsigned char*& buffer, const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A)
{
	*buffer++ = R;
	*buffer++ = G;
	*buffer++ = B;
	*buffer++ = A;
}
void YUV420SemiPlanarToRGB(unsigned char *_pOutData, unsigned short *_pInData, unsigned int width, unsigned int height)
{
    cv::Mat YUV420_NV12(height + height/2, width, CV_8UC1, _pInData); 
    cv::Mat RGBA888(height, width, CV_8UC4, _pOutData);
    cv::cvtColor(YUV420_NV12, RGBA888, cv::COLOR_YUV2BGRA_NV12); 
    return;
}


void YUV422ToRGB(unsigned char *_pOutData, unsigned short *_pInData, unsigned int imageSize, bool swapOutput, bool swapInput)
{
	unsigned char *pOutData = (unsigned char *)_pOutData;
	const unsigned char *pInData = (unsigned char *)_pInData;

	int uOffset = swapInput ? 3 : 2;
	int vOffset = swapInput ? 1 : 0;
	int yOffset0 = swapInput ? 0 : 3;
	int yOffset1 = swapInput ? 2 : 1;

	if (swapOutput)
	{
		for (int i = 0; i < int(sizeof(_pInData[0])*imageSize / sizeof(pInData[0])); i += 4 * sizeof(pInData[0]))
		{
			unsigned char y0 = pInData[i + yOffset0];
			unsigned char u0 = pInData[i + uOffset];
			unsigned char v0 = pInData[i + vOffset];

			unsigned char y1 = pInData[i + yOffset1];
			unsigned char u1 = u0;
			unsigned char v1 = v0;

			int c0 = y0 - C16;
			int d0 = u0 - C128;
			int e0 = v0 - C128;

			int c1 = y1 - C16;
			int d1 = u1 - C128;
			int e1 = v1 - C128;

			int b = (C1192 * c0 + C2066 * d0 + ROUND) >> SHIFT;               // B
			b = max(min(b, 255), 0);

			int g = (C1192 * c0 - C400 * d0 - C832 * e0 + ROUND) >> SHIFT;    // G
			g = max(min(g, 255), 0);

			int r = (C1192 * c0 + C1634 * e0 + ROUND) >> SHIFT;               // R
			r = max(min(r, 255), 0);

			Insert(pOutData, r, g, b, ALPHA_TRANSPARENT);

			b = (C1192 * c1 + C2066 * d1 + ROUND) >> SHIFT;                 // B
			b = max(min(b, 255), 0);

			g = (C1192 * c1 - C400 * d1 - C832 * e1 + ROUND) >> SHIFT;      // G
			g = max(min(g, 255), 0);

			r = (C1192 * c1 + C1634 * e1 + ROUND) >> SHIFT;                // R
			r = max(min(r, 255), 0);

			Insert(pOutData, r, g, b, ALPHA_TRANSPARENT);
		}
	}
	else
	{
		for (int i = 0; i < int(sizeof(_pInData[0])*imageSize / sizeof(pInData[0])); i += 4 * sizeof(pInData[0]))
		{
			unsigned char y0 = pInData[i + yOffset0];
			unsigned char u0 = pInData[i + uOffset];
			unsigned char v0 = pInData[i + vOffset];

			unsigned char y1 = pInData[i + yOffset1];
			unsigned char u1 = u0;
			unsigned char v1 = v0;

			int c0 = y0 - C16;
			int d0 = u0 - C128;
			int e0 = v0 - C128;

			int c1 = y1 - C16;
			int d1 = u1 - C128;
			int e1 = v1 - C128;

			int b = (C1192 * c0 + C2066 * d0 + ROUND) >> SHIFT;               // B
			b = max(min(b, 255), 0);

			int g = (C1192 * c0 - C400 * d0 - C832 * e0 + ROUND) >> SHIFT;    // G
			g = max(min(g, 255), 0);

			int r = (C1192 * c0 + C1634 * e0 + ROUND) >> SHIFT;               // R
			r = max(min(r, 255), 0);

			Insert(pOutData, b, g, r, ALPHA_TRANSPARENT);

			b = (C1192 * c1 + C2066 * d1 + ROUND) >> SHIFT;                   // B
			b = max(min(b, 255), 0);

			g = (C1192 * c1 - C400 * d1 - C832 * e1 + ROUND) >> SHIFT;        // G
			g = max(min(g, 255), 0);

			r = (C1192 * c1 + C1634 * e1 + ROUND) >> SHIFT;                   // R
			r = max(min(r, 255), 0);

			Insert(pOutData, b, g, r, ALPHA_TRANSPARENT);
		}
	}
}

unsigned int StreamDisplay(cv::Mat *img, const char *winName)
{
   cv::imshow(winName, *img);
  //cv::waitKey(1);
   int k = cv::waitKey(2);
   if (k == 'y' || k == 'Y') //snapshot the frame
       return 1;
   else
       return 0;

}

static void paintDepthMap(cv::Mat *depthImg)
{
	cv::Mat map = *depthImg;
	cv::Mat adjMap;
	cv::Mat coloredMap;

	map.convertTo(adjMap, CV_8U, 255.0 / 8192.0, 0);
	cv::cvtColor(adjMap.clone(), adjMap, cv::COLOR_GRAY2BGR);
	LUT(adjMap, depthLut, coloredMap);

	*depthImg = coloredMap;
}

#if defined _OPENCV_
/* handle_classification_image */
void handle_classification_image(cv::Mat& webcam_mat, int image_height, int image_width)
{
	 int obj_ind, x_min, x_max;
	 char name_line[100];
	 cv::Point pt1, pt2, text_pos;
	 int thickness = 1;
	 int text_thickness = 1;
	 float text_scale = 1;
	 int line_type = 8;
	 int shift = 0;
	 int base_line = 0;
	 cv::Size textSize;
	 cv::Scalar text_color = { 0, 0, 0 };

	 int flipped = 0; //TODO check

	 for (obj_ind = 0; obj_ind < num_of_objects; obj_ind++)
	 {
		  int bx_obj = (int)(bx[obj_ind] * image_width);
		  int bxe_obj = (int)(bxe[obj_ind] * image_width);
		  int by_obj = (int)(by[obj_ind] * image_height);
		  int bye_obj = (int)(bye[obj_ind] * image_height);

		  /* Add the name and the confidence */
		  sprintf(name_line, "%s (%2.3f)", object_names[obj_ind], object_confidence[obj_ind]);

		  /* If the frame is flipped, we need to adjust the X coordinates */
		  if (flipped)
		  {
				x_max = webcam_mat.cols - bx_obj;
				x_min = webcam_mat.cols - bxe_obj;
		  }
		  else
		  {
				x_min = bx_obj;
				x_max = bxe_obj;
		  }

		  /* If face too high, put the text below the box, else put it above */
		  if (by_obj <= 50) {
				if (bye_obj > webcam_mat.rows - 50) {
					 text_pos = cv::Point(x_min, bye_obj - 10); // Frame too big. Put the text inside the box
				}
				else {
					 text_pos = cv::Point(x_min, bye_obj + 20); // was 10
				}
		  }
		  else {
				text_pos = cv::Point(x_min, bye_obj - 10); // was 5
		  }

		  textSize = cv::getTextSize(name_line, cv::FONT_HERSHEY_PLAIN, text_scale, text_thickness, &base_line);
		  textSize.height += 5;
		  pt1 = text_pos + cv::Point(0, -textSize.height);
		  pt2 = text_pos + cv::Point(textSize.width, base_line);
		  cv::rectangle(webcam_mat, pt1, pt2, class_colors[label[obj_ind] % MAX_COLORS], cv::FILLED, line_type, shift);


		  cv::putText(webcam_mat,
				name_line,
				text_pos,
				cv::FONT_HERSHEY_PLAIN, // Font
				text_scale, // Scale. 2.0 = 2x bigger... was 0.5
				text_color, //obj_class_colors[obj_ind], // Color
				text_thickness); // Thickness

		  /* Put the frame around the objects */
		  pt1 = cv::Point(x_min, by_obj);
		  pt2 = cv::Point(x_max, bye_obj);
		  cv::rectangle(webcam_mat, pt1, pt2, class_colors[label[obj_ind] % MAX_COLORS], thickness, line_type, shift);
	 }

}
#endif
#ifdef DEBUG_CROP_OVERLAY
std::mutex map_mutex;
#include <iostream>
/*Taken from https://stackoverflow.com/questions/17232059/detecting-difference-between-2-images/17237076#17237076*/
double getMSE(const cv::Mat& I1, const cv::Mat& I2)
{
    cv::Mat s1;
    cv::absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2

    cv::Scalar s = cv::sum(s1);         // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double  mse =sse /(double)(I1.channels() * I1.total());
        return mse;

    }
}
float mse(cv::Mat &cropped, cv::Mat &downscaled, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	cv::Mat roi(downscaled, cv::Rect(x ,y, cropped.cols, cropped.rows));
	return getMSE(roi,cropped);

}
#define MSE_THRESHOLD 400.0
void displayOverlayStream(const char *winName,bool recordPNG, const char *name)
{
	std::vector<uint32_t> dsdeletedValues;
	std::vector<uint32_t> crdeletedValues;
	if(downscaledMap.size() == 0)
		return;
	for ( auto& [key, downscaledVal] : downscaledMap) {

		UINT32 croppedMapKey = key;
		if(croppedMap.count(croppedMapKey) > 0 )
		{
			//std::cout << "Looking for entry at " << key << std::endl;
			Image_With_Metadata &croppedImageVal = croppedMap.at(croppedMapKey);
            using std::chrono::high_resolution_clock;
            using std::chrono::duration_cast;
            using std::chrono::duration;
            using std::chrono::milliseconds;
            auto t1 = high_resolution_clock::now();
			cv::Mat &downscaledImage = downscaledVal.Image;
			cv::Mat &croppedImage = croppedImageVal.Image;
			cv::Mat dst = downscaledImage;
			cv::Mat downscaled_original = downscaledImage.clone();
			cv::Mat croppedImageScaled;

			/*Resize the cropped image to be 1/3 */
			cv::resize(croppedImage, croppedImageScaled, cv::Size(AF_VST_CROP_WIDTH/3, AF_VST_CROP_HEIGHT/3));
			UINT32 x_coordinate = croppedImageVal.metadata.protobuf_packet.state[0].sensor_metadata.roi.x_start/3;
			UINT32 y_coordinate = croppedImageVal.metadata.protobuf_packet.state[0].sensor_metadata.roi.y_start/3;
			char frameBuffer[128] = {0};
			char frameBufferCropped[128] = {0};
			char frameIDCounter[128] = {0};
			snprintf(frameIDCounter,sizeof(frameBuffer),"%lu",croppedImageVal.metadata.frameID );
			cv::putText(croppedImage, //target image
			frameIDCounter, //text
			cv::Point(80, 80), //top-left position
			cv::FONT_HERSHEY_DUPLEX,
			2,
			CV_RGB(118, 185, 0), //font color
			2);
			int64_t  ts_ds = convertRTCTimeToNS(downscaledVal.metadata.timestamp)/1000;
			int64_t  ts_cr = convertRTCTimeToNS(croppedImageVal.metadata.timestamp)/1000;
			int64_t ts_diff = ts_ds - ts_cr;

			INU_Metadata_T * meta = &croppedImageVal.metadata;
			if (1)
			{
				std::ofstream outfile;
				// writeTime can be used to look at the I2C write times
				uint64_t writeTime = meta->protobuf_packet.state[0].snsr_update_timing.sensor_update_i2cWritesComplete.timestamp.nanoseconds - meta->protobuf_packet.state[0].snsr_update_timing.sensor_update_dequed_timing.timestamp.nanoseconds;
				outfile.open("Debug_Log.csv", std::ios_base::app); // append instead of overwrite
				uint64_t start_time = (croppedImageVal.metadata.protobuf_packet.state[0].snsr_update_timing.sensor_update_i2cWritesComplete.timestamp.nanoseconds/1000);
				uint64_t end_time = (croppedImageVal.metadata.protobuf_packet.state[0].snsr_update_timing.sensor_update_dequed_timing.timestamp.nanoseconds/1000);

				uint64_t eof = convertRTCTimeToNS(croppedImageVal.metadata.software_injection_timestamp_rtc)/1000 ;
				uint64_t sof =  convertRTCTimeToNS(croppedImageVal.metadata.timestamp)/1000;
				uint64_t eof_latency = eof - sof;

				outfile << meta->frameID << "," << convertRTCTimeToNS(meta->timestamp)/1000 << "," << meta->serialization_counter << "," << meta->protobuf_packet.state[0].sensor_metadata.roi.x_start<< "," << meta->protobuf_packet.state[0].sensor_metadata.roi.y_start << "," << meta->protobuf_packet.state[0].snsr_update_timing.sensor_update_i2cWritesComplete.frameID.frameID << ","  <<  "," << eof << std::endl;
				
				outfile.close();
			}
			snprintf(frameBuffer,sizeof(frameBuffer),"Dscl Frame ID %lu, Dropped %lu, SW CNT %lu,ISP0:%lu,ISP1:%lu,TD:%lld,DQ:%lu", downscaledVal.metadata.frameID, downscaledVal.metadata.compensatedDroppedFrameCounter ,downscaledVal.metadata.software_injection_call_counter,downscaledVal.metadata.isp0FrameID, downscaledVal.metadata.isp1FrameID,ts_diff,downscaledVal.metadata.excessDequeues);
			snprintf(frameBufferCropped,sizeof(frameBufferCropped),"Crop Frame ID %lu, Dropped %lu, SW CNT %lu,ISP0:%lu,ISP1:%lu,TD:%lld,DQ:%lu", croppedImageVal.metadata.frameID, croppedImageVal.metadata.compensatedDroppedFrameCounter ,croppedImageVal.metadata.software_injection_call_counter,croppedImageVal.metadata.isp0FrameID, croppedImageVal.metadata.isp1FrameID,ts_diff,croppedImageVal.metadata.excessDequeues);
			/*Construct an image with a ROI*/
			cv::Mat insetImage(dst, cv::Rect(x_coordinate ,y_coordinate, croppedImageScaled.cols, croppedImageScaled.rows));
			/*Copy into the ROI*/
			croppedImageScaled.copyTo(insetImage);
			cv::Mat shrinked;

			cv::putText(dst, //target image
				frameBuffer, //text
				cv::Point(40, 40), //top-left position
				cv::FONT_HERSHEY_DUPLEX,
				0.75,
				CV_RGB(118, 185, 0), //font color
				2);
			cv::putText(dst, //target image
				frameBufferCropped, //text
				cv::Point(40, 80), //top-left position
				cv::FONT_HERSHEY_DUPLEX,
				0.75,
				CV_RGB(118, 185, 0), //font color
				2);
			/* Add a rectangle for showing where the crop window is */
            cv::Point p1(x_coordinate, y_coordinate);
            cv::Point p2(x_coordinate + croppedImageScaled.cols, y_coordinate+croppedImageScaled.rows); 
            int thickness = 25; 
            cv::rectangle(dst, p1, p2,cv::Scalar(0, 0, 255),cv::LINE_8); 
			StreamDisplay(&dst, winName);
			if(1)
			{
				double result = mse(croppedImageScaled,downscaled_original,x_coordinate,y_coordinate,AF_VST_CROP_WIDTH/3,AF_VST_CROP_HEIGHT/3);
				if(result > MSE_THRESHOLD)
				{
					std::cout << "Fail found at frame id " << downscaledVal.metadata.frameID << "MSE is " << result << std::endl;
					StreamDisplay(&dst, "Error");
					char buffer[128];
					snprintf(buffer,sizeof(buffer),"Record/Error_VST_Composition_%lu_MSE%.3f.jpeg", downscaledVal.metadata.frameID,result);
					/*Records the image to a PNG file for easy offline analysis*/
					cv::imwrite(buffer,dst);
				}
			}
			auto t2 = high_resolution_clock::now();
            duration<double, std::milli> ms_double = t2 - t1;
            //std::cout << "Scaling time was"  << ms_double.count() << "ms\n";
			if(0)
			{
				char buffer[128];
				snprintf(buffer,sizeof(buffer),"Record/VST_Composition_%lu.png", downscaledVal.metadata.frameID);
				/*Records the image to a PNG file for easy offline analysis*/
				cv::imwrite(buffer,dst);
			}
			dsdeletedValues.push_back(key);
			crdeletedValues.push_back(croppedMapKey);
	

		}
	}
	for(auto &x : dsdeletedValues)
	{
		downscaledMap.erase(x);
	}
	for(auto &x : crdeletedValues)
	{
		croppedMap.erase(x);
	}
}
#endif
static void displayImage(SANDBOX_channelParamsT *channelParamsP, char *data_tempP, int size, inu_image__hdr_t *imgHdrP, 
const char *streameName, bool recordPNG, char * name, INU_Metadata_T *meta)
{
	UINT16 *dataP = (UINT16*)data_tempP;
	char winName[100];
	int type;
	void *buffer = nullptr;

   GET_WIN_NAME(winName, streameName, imgHdrP->imgDescriptor.stride, imgHdrP->imgDescriptor.bufferHeight);
   type = CV_16UC1;
   if (channelParamsP->bayer2RgbConvert)
   {
      if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_RAW10_E)
      {
         type = CV_8UC3;
         buffer = dataP;
      }
      else 
      {
         printf("image format is not valid for DMS!\n");
      }
  
   }
   
   else if ((imgHdrP->imgDescriptor.format == NUCFG_FORMAT_RAW8_E) || (imgHdrP->imgDescriptor.bitsPerPixel == 8))
   {
      type = CV_8UC1;
      buffer = dataP;
   }
   else if ((imgHdrP->imgDescriptor.format == NUCFG_FORMAT_RAW10_E) || (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_YUV422_10BIT_E))
   {
      buffer = dataP;
      for (unsigned int i = 0; i < (size / IMAGE_BYTES_PER_PIXEL(imgHdrP)); i++)
         (dataP)[i] = (dataP)[i] << 6;
   }
   else if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_BAYER_16_E)
   {
      type = CV_8UC3;
      buffer = dataP;
   }
   else if ((imgHdrP->imgDescriptor.format == NUCFG_FORMAT_DISPARITY_E))
   {
      buffer = dataP;
      for (unsigned int i = 0; i < (size / IMAGE_BYTES_PER_PIXEL(imgHdrP)); i++)
         (dataP)[i] = (dataP)[i] << 8;
   }
   else if ((imgHdrP->imgDescriptor.format == NUCFG_FORMAT_DEPTH_E))
   {
      buffer = dataP;

		/* In order to show depth in Inuitive format (Hot - short, Cold - long), we flip the values.
			Currenly we choose color map named "TURBU" provided by OpenCV, since it is the closest
			to Inuitive's format, but we need to change it, since it doesn't show invalid pixels (zeros)
			in black */
		for (unsigned int i = 0; i < (size / IMAGE_BYTES_PER_PIXEL(imgHdrP)); i++)
		{
			 if ((dataP)[i] != 0)  // Keep zeros invalid
			 {
				  (dataP)[i] = (dataP)[i] > 8191 ? 8191 : (dataP)[i];   // max value is 8191
				  (dataP)[i] = 8192 - (dataP)[i];
			 }
		}
	}
	else if ((imgHdrP->imgDescriptor.format == NUCFG_FORMAT_DISPARITY_DEBUG_E))
	{
		buffer = dataP;
		/* The following is used when displaying AFTER the DPE scaler
		UINT8 *tempP = (UINT8*)dataP;
		UINT32 *temp32P;
		UINT32 pixelSwap, pixelToSwap, numPixelsPerLine, numLines, dstPixelNum;;

		type = CV_8UC1;

		numPixelsPerLine = imgHdrP->imgDescriptor.width;
		numLines = imgHdrP->imgDescriptor.height;

		for (unsigned int i = 0; i < numLines; i++)
		{
			for (unsigned int j = 0; j < numPixelsPerLine; j++)
			{
				dstPixelNum = ((i * numPixelsPerLine) + j);
				temp32P = (UINT32*)&tempP[((i * numPixelsPerLine) + j) * 3];//the source buffer is 1280x400x3
				pixelSwap = *temp32P;
				pixelSwap = pixelSwap & 0xFFFFFF;
				channelParamsP->bufferP[(i * 2*numPixelsPerLine) + j] = (pixelSwap >> 2) & 0xFF; //dst buffer is 2x640x400x1
				channelParamsP->bufferP[(i * 2*numPixelsPerLine) + numPixelsPerLine + j] = (pixelSwap >> 12) & 0xFF;
			}
		}
		imgHdrP->imgDescriptor.stride = imgHdrP->imgDescriptor.stride * 2;
		GET_WIN_NAME(winName, streameName, imgHdrP->imgDescriptor.stride, imgHdrP->imgDescriptor.height);
		*/
	}
	else if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_YUV422_8BIT_E)
	{
		type = CV_8UC4;

		/*if (buffer == nullptr)
		{
			buffer = (unsigned char*)malloc(4 * imgHdrP->imgDescriptor.height * imgHdrP->imgDescriptor.width);
		}*/

      YUV422ToRGB((unsigned char*)channelParamsP->bufferP, (unsigned short*)dataP, imgHdrP->imgDescriptor.height * imgHdrP->imgDescriptor.width, true, true);
      size = imgHdrP->imgDescriptor.height * imgHdrP->imgDescriptor.width * 4;
   }
   else if ((imgHdrP->imgDescriptor.format == NUCFG_FORMAT_RGB888_E))
   {
      buffer = dataP;
      type = CV_8UC3;
   }
   else if((imgHdrP->imgDescriptor.format == NUCFG_FORMAT_YUV420_SEMI_PLANAR_E))
   {
      type = CV_8UC4;
      YUV420SemiPlanarToRGB((unsigned char*)channelParamsP->bufferP, (unsigned short*)dataP, imgHdrP->imgDescriptor.width,imgHdrP->imgDescriptor.height);
      size = imgHdrP->imgDescriptor.height * imgHdrP->imgDescriptor.width * 4;
   }
   else
   {
      buffer = dataP;
   }
#if (defined CDNN || defined FDK_CNN_DEMO_STREAM || defined FDK_CNN_CLIENT_DEMO_STREAM)
	if (imgHdrP->imgDescriptor.width == SCL_WIDTH_PAD)
		ai = 1;
#endif
#ifdef DRAW_CUBE
	if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_RAW10_E)
	{
		int height = imgHdrP->imgDescriptor.bufferHeight;
		short line[1280];
		short *dstImageP = (short*)dataP;
		short *srcImageP = (short*)dataP;
		for (int row = 0; row < height; row++)
		{
			memcpy(&dstImageP[row * width], &srcImageP[row * imgHdrP->imgDescriptor.stride], width * sizeof(*dstImageP));
		}
		cv::Mat imageMat = cv::Mat(imgHdrP->imgDescriptor.bufferHeight, width, CV_16UC1, dstImageP);
		DrawCube(imageMat);
		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				line[col] = srcImageP[row * width + width - 1 - col];  // swap
			}
			memcpy(&srcImageP[row * width], line, width * sizeof(*dstImageP));
		}
	}
	StreamDisplay(width, imgHdrP->imgDescriptor.bufferHeight, dataP, fmt, typ, ai);
#else
   if ((imgHdrP->imgDescriptor.format != NUCFG_FORMAT_YUV422_8BIT_E) && (imgHdrP->imgDescriptor.format != NUCFG_FORMAT_GEN_16_E) && 
   (imgHdrP->imgDescriptor.format != NUCFG_FORMAT_DISPARITY_DEBUG_E) && (imgHdrP->imgDescriptor.format != NUCFG_FORMAT_YUV420_SEMI_PLANAR_E))
   {
      if (imgHdrP->dataHdr.chunkNum == 0)
      {
         memset(channelParamsP->bufferP, 0, IMAGE_SIZE_BYTES(imgHdrP));
      }
      channelParamsP->yLoc = (imgHdrP->dataHdr.chunkNum * size);
#if 0//BOKEH
		if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_RGB888_E)
		{
			 int h = 1080;
			 int w = 1920;
			 for (int plane = 0; plane < 3; plane++)
			 {
				  for (int i = 0; i < h * w; i++)
				  {
						int input_index = plane * h * w + i;
						int output_index = i * 3 + plane;
						channelParamsP->bufferP[output_index] = ((unsigned char*)dataP)[input_index];
				  }
			 }
		}
		else
#endif
      {
          memcpy(&channelParamsP->bufferP[channelParamsP->yLoc], (unsigned char*)buffer, size);
      }
   }

   if (((imgHdrP->imgDescriptor.format == NUCFG_FORMAT_BAYER_16_E) || (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_RAW10_E)) && (channelParamsP->bayerPattern))
   {
      type = CV_16U;
      cv::Mat img(imgHdrP->imgDescriptor.bufferHeight, imgHdrP->imgDescriptor.stride, type, channelParamsP->bufferP);
      int cvtCode;
      switch (channelParamsP->bayerPattern)
      {
      case 1:
         cvtCode = cv::COLOR_BayerGR2RGB;
         break;
      case 2:
         cvtCode = cv::COLOR_BayerRG2RGB;
         break;
      case 3:
         cvtCode = cv::COLOR_BayerGB2RGB;
         break;
      case 4:
         cvtCode = cv::COLOR_BayerBG2RGB;
         break;
      default:
         printf("ERROR!! Invalid Bayer Pattern! Allowd values are [1-4]\n");
         assert(0);
      }

        cv::cvtColor(img, img, cvtCode);
		if(recordPNG)
		{
		/*Records the image to a PNG file for easy offline analysis*/
			//cv::imwrite(name,img);
		}

        StreamDisplay(&img, winName);
   }
   else
   {
      cv::Mat img(imgHdrP->imgDescriptor.bufferHeight, imgHdrP->imgDescriptor.stride, type, channelParamsP->bufferP);
      if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_DEPTH_E)//after we made the img from 'channelParamsP->bufferP' we can paint the depth map, and send it to display
      {
         paintDepthMap(&img);
      }
      if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_RGB888_E)//after we made the img from 'channelParamsP->bufferP' we canvert BGR to RGB
      {
         cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
      }
      if (aiDraw)
      {
         handle_classification_image(img, imgHdrP->imgDescriptor.bufferHeight, imgHdrP->imgDescriptor.stride);
      }
	#ifdef DEBUG_CROP_OVERLAY
	
		if( imgHdrP->imgDescriptor.stride == AF_VST_CROP_WIDTH)
		{
			map_mutex.lock();
			Image_With_Metadata imageWithM;
			imageWithM.Image = img.clone();
			imageWithM.metadata = *meta;
			croppedMap[meta->frameID] = imageWithM;
			displayOverlayStream(winName,recordPNG,name);
			map_mutex.unlock();
		}
		else if(imgHdrP->imgDescriptor.stride == AF_VST_DOWNSCALED_WIDTH)
		{	
			map_mutex.lock();
			Image_With_Metadata imageWithM;
			imageWithM.Image = img.clone();
			imageWithM.metadata = *meta;
			downscaledMap[meta->frameID] = imageWithM;
			map_mutex.unlock();
		}
	#else
	StreamDisplay(&img, winName);
	#endif

      
   }
#endif
}
#endif

/* load_labels */
bool load_labels()
{
	 FILE* fd;
	 bool status = true;
	 char line[30], * line_p;
	 char classes_file[500] = "C:\\program files\\Inuitive\\InuDev\\config\\AI\\ObjectDetectionSsd\\labels.txt";

	 fd = fopen(classes_file, "r");
	 if (fd)
	 {
		  printf("Reading class information from %s\n", classes_file);
		  line_p = fgets(line, sizeof(line), fd);
		  int number_of_classes = 0;
		  while (line_p)
		  {
				if (line[0] != '#')
				{
					 if (number_of_classes > MAX_NUM_CLASSES)
					 {
						  printf("classification_load_classes: too many classes in %s\n", classes_file);
						  fclose(fd);
						  return true;
					 }

					 line[strlen(line) - 1] = '\0'; // Remove the CR

					 strcpy(classes[number_of_classes], line);
					 //printf("Class %d: %s len:%d\n", number_of_classes[net_id], classes[net_id][number_of_classes[net_id]], (int)strlen(line));
					 number_of_classes++;
				}
				line_p = fgets(line, sizeof(line), fd);
		  }
		  fclose(fd);
		  printf("Loaded %d class names\n", number_of_classes);
	 }
	 else
	 {
		  printf("Failed to load %s\n", classes_file);
		  status = false;
	 }

	 return (status);
}

static void printCdnnResults(void *dataP, inu_cdnn_data__hdr_t *header)
{
	switch (header->network_id)
	{
		case(INU_LOAD_NETWORK__CDNN_0_E) : //google image net
		{
			unsigned int *p;
			unsigned int max_val = 0;
			int max_index = 0;
			int N0 = 1000;
			UINT64 virtAddressX = (UINT64)dataP;
			p = (unsigned int *)virtAddressX;
			for (int i = 0; i < N0; i++)
			{
				if (p[i] > max_val)
				{
					max_val = p[i];
					max_index = i;
				}
			}
			printf("cdnn google image net: max imagenet = %d\n", header->frameId);
			break;
		}

		case(INU_LOAD_NETWORK__CDNN_2_E) : //Mobile Net SSD - Object Detection
		{
			unsigned char *blobp = (unsigned char *)dataP;
			int valid_count = 0;
			num_of_objects = 0;

			aiDraw = 1;
			if (first_frame)
			{
				 load_labels();
				 first_frame = 0;
			}

			if ((header->totOutputSize % sizeof(BBOX)) == 4) // New API (Synopsys version 2019_3)
			{
				valid_count = ((int *)blobp)[0];
				blobp += 4;
			}
			else
			{
				if (header->totOutputSize)
					valid_count = 15;
			}

			BBOX *boxes = (BBOX *)blobp;

			for (int i = 0; i < valid_count; i++)
			{
				label[i] = 0;
				if (boxes[i].confidence > 0.5)
				{
					bx[i] = boxes[i].x_min;
					by[i] = boxes[i].y_min;
					bxe[i] = boxes[i].x_max;
					bye[i] = boxes[i].y_max;
					label[i] = boxes[i].label;
					num_box = i;
					printf("%d BOX %f %f %f %f conf %f\n", boxes[i].label, boxes[i].x_min, boxes[i].y_min, boxes[i].x_max, boxes[i].y_max, boxes[i].confidence);
					int bx_int, by_int, bxe_int, bye_int, conf_int;
					bx_int = (int)(bx[i] * 1e6);
					by_int = (int)(by[i] * 1e6);
					bxe_int = (int)(bxe[i] * 1e6);
					bye_int = (int)(bye[i] * 1e6);
					conf_int = (int)(boxes[i].confidence * 1e6);
					if (label[i] == 15 &&
						 bx_int == 26079 &&
						 by_int == -1409 &&
						 bxe_int == 922643 &&
						 bye_int == 998062 &&
						 conf_int == 993652)

						// 0.026079 - 0.001410 0.922644 0.998062 conf 0.993652
					{
						 printf("\n\n\n ============================ PASS TEST =============================\n\n\n");
					}
					if (boxes[i].label >= MAX_NUM_CLASSES || boxes[i].label < 0)
					{
						 printf("ssd_parser: Invalid label: %d at index %d\n", boxes[i].label, i);
						 break;
					}

					label[num_of_objects] = boxes[i].label;
					bx[num_of_objects] = boxes[i].x_min;
					bxe[num_of_objects] = boxes[i].x_max;
					by[num_of_objects] = boxes[i].y_min;
					bye[num_of_objects] = boxes[i].y_max;
					object_confidence[num_of_objects] = boxes[i].confidence;
					strcpy(object_names[num_of_objects], classes[boxes[i].label]);
					num_of_objects++;
				}
				else
				{
					bx[i] = 0;
					by[i] = 0;
					bxe[i] = 0;
					bye[i] = 0;
					num_box = i;
					label[i] = 0;
				}
			}
			break;
		}

		default:
			printf("unknown network id %d\n", header->network_id);
			break;
	}
}

void startCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	(void)ref;
	(void)hdrP;
	std::string fileName = std::string(channelParamsP->stream_out_name);
	if (channelParamsP->statsEnable)
	{
		streamStatsInit(channelParamsP);
	}
	if(channelParamsP->recordCSVMetadata)
	{
		fileName += ".csv";
		std::ofstream newFile(fileName, std::ios::trunc);
		newFile << "Frame ID, RTC Timestamp(ns), Host Timestamp (ns), ISP Exposure Time (us), ISP Command ID" << std::endl;
	}

}

void frameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie, INU_Metadata_T * metadata)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	(void)dataP;
	if (channelParamsP->statsEnable)
	{
		streamStatsProcess(channelParamsP, hdrP, ref, size,metadata);
	}
}

void stopCb(inu_refH ref, void *cookie)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	(void)ref;
	if (channelParamsP->statsEnable)
	{
		if (((channelParamsP->totalFramesReceived + channelParamsP->totalNumDropped) * STREAM_STATS_DROPPED_THRESHOLD) < (channelParamsP->totalNumDropped) )
		{
			testResult = 1;
			printf("Failure: dropped frame reached to threshold\n");
		}
	}
}

void imageStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	inu_vision_proc__parameter_list_t visionProcParams;
#if defined _OPENCV_
	inu_image__hdr_t *imgHdrP = (inu_image__hdr_t*)hdrP;
#endif
	ERRG_codeE status;
	inu_nodeH  currentH;

   //search for the pp function node
   currentH = inu_node__getNextInputNode(ref, NULL);
   while (currentH)
   {
       if (inu_ref__instanceOf(currentH, INU_VISION_PROC_REF_TYPE))
       {
           break;
       }
       currentH = inu_node__getNextInputNode(currentH, NULL);
   }
   if (currentH)
   {
       visionProcParams.algType = VISION_PROC_BOKEH;
       visionProcParams.bokehParams.blurLevel = 0.8f;
       visionProcParams.bokehParams.visionProcResolution = VISION_PROC_1080P;
       visionProcParams.bokehParams.depthOutput = false;
       //visionProcParams.algType = VISION_PROC_BACKGROUND_REMOVAL;
       //visionProcParams.backgroundRemovalParams.visionProcResolution = VISION_PROC_1080P;
       //visionProcParams.backgroundRemovalParams.backgroundImage = 1;//enable 0 disable
       //visionProcParams.algType = VISION_PROC_LOW_LIGHT_ISP;
       //visionProcParams.lowLightIspParams.visionProcResolution = VISION_PROC_1080P;
       //visionProcParams.lowLightIspParams.stride = 4;

       //send parameters to gp vision proc
#ifdef ENABLE_VISION_EFFECT
       status = inu_vision_proc__updateParams(currentH, &visionProcParams);
   #endif
       SANDBOX_TEST_RET_ASSERT(status);
   }

   //search for the dpe pp function node
   currentH = inu_node__getNextInputNode(ref, NULL);
   while (currentH)
   {
       if (inu_ref__instanceOf(currentH, INU_DPE_PP_REF_TYPE))
       {
           break;
       }
       currentH = inu_node__getNextInputNode(currentH, NULL);
   }
   if (currentH)
   {
       inu_dpe_ppH dpeH = currentH;
       inu_dpe_pp_hostGpMsgStructT hostGPmsg;

       hostGPmsg.ConfidenceThresholdRegion1 = 0;
       hostGPmsg.BlobDispDiffSameBlob = 16;// 1;
       hostGPmsg.BlobMaxHeight = 16;// 300;
       hostGPmsg.BlobMaxSize = 500;// 1500;
       hostGPmsg.TemporolRapidTh = 6 * 16;
       hostGPmsg.TemporolStableTh = 1 * 16;
       hostGPmsg.algType = (1 << INU_DPE_PP_BLOB_OUTLIERS_REMOVAL);
       status = inu_dpe_pp__send_data_async(dpeH, (char*)&hostGPmsg, sizeof(hostGPmsg));
       SANDBOX_TEST_RET_ASSERT(status);
   }


	startCb(ref, hdrP, cookie);

	if (channelParamsP->displayOutput)
	{
#if defined _OPENCV_
		char tmpName[100], titleName[100];
		// These are values of 'TURBO' colormap + mapping '0' to Black for painting depth display
		static const unsigned char depthLut_RGB[3][256] = {
			{ 0, 49, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 67, 68, 68, 69, 69, 69, 69, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 69, 69, 68, 68, 67, 66, 65, 63, 62, 61, 59, 58, 56, 54, 53, 51, 49, 47, 45, 44, 42, 40, 38, 37, 35, 33, 32, 31, 29, 28, 27, 26, 25, 24, 24, 24, 23, 23, 23, 24, 24, 25, 26, 27, 29, 30, 32, 34, 36, 39, 41, 44, 47, 50, 53, 56, 60, 63, 67, 70, 74, 78, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 128, 132, 136, 139, 143, 146, 150, 153, 156, 159, 162, 164, 167, 170, 172, 175, 177, 180, 183, 185, 188, 191, 193, 196, 198, 201, 203, 206, 208, 210, 213, 215, 217, 219, 221, 224, 226, 228, 230, 231, 233, 235, 237, 238, 240, 241, 243, 244, 245, 247, 248, 249, 250, 250, 251, 252, 252, 253, 253, 254, 254, 254, 254, 255, 255, 255, 255, 254, 254, 254, 254, 253, 253, 252, 252, 251, 251, 250, 249, 248, 248, 247, 246, 245, 244, 243, 242, 240, 239, 238, 237, 235, 234, 233, 231, 230, 228, 227, 225, 223, 222, 220, 218, 216, 214, 212, 210, 208, 206, 204, 202, 200, 198, 195, 193, 191, 188, 186, 183, 180, 178, 175, 172, 170, 167, 164, 161, 158, 155, 152, 149, 146, 142, 139, 136, 133, 129, 126, 122 },
			{ 0, 21, 24, 27, 30, 33, 36, 38, 41, 44, 47, 50, 53, 56, 58, 61, 64, 67, 70, 72, 75, 78, 81, 83, 86, 89, 91, 94, 97, 99, 102, 105, 107, 110, 113, 115, 118, 120, 123, 125, 128, 130, 133, 135, 138, 140, 143, 145, 148, 150, 153, 155, 158, 161, 163, 166, 168, 171, 173, 176, 178, 181, 183, 185, 188, 190, 192, 195, 197, 199, 201, 204, 206, 208, 210, 212, 214, 216, 218, 219, 221, 223, 224, 226, 227, 229, 230, 232, 233, 234, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 250, 251, 252, 252, 253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 253, 252, 252, 251, 250, 249, 248, 248, 246, 245, 244, 243, 242, 240, 239, 238, 236, 235, 233, 232, 230, 228, 226, 225, 223, 221, 219, 217, 215, 213, 211, 209, 207, 205, 203, 201, 199, 197, 195, 193, 191, 189, 186, 184, 182, 180, 177, 175, 172, 169, 167, 164, 161, 158, 156, 153, 150, 147, 144, 141, 138, 135, 132, 129, 126, 123, 120, 117, 114, 111, 108, 105, 102, 99, 96, 93, 91, 88, 85, 83, 80, 78, 75, 73, 71, 69, 66, 64, 62, 60, 58, 56, 54, 52, 50, 49, 47, 45, 43, 41, 40, 38, 36, 35, 33, 31, 30, 28, 27, 25, 24, 22, 21, 20, 18, 17, 16, 15, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4 },
			{ 0, 66, 74, 81, 88, 95, 102, 108, 115, 121, 128, 134, 139, 145, 151, 156, 162, 167, 172, 177, 182, 186, 191, 195, 199, 203, 207, 211, 215, 218, 221, 225, 228, 231, 233, 236, 238, 241, 243, 245, 247, 249, 250, 252, 253, 254, 254, 255, 255, 255, 255, 255, 254, 253, 253, 251, 250, 249, 247, 246, 244, 242, 240, 238, 236, 234, 231, 229, 226, 224, 221, 219, 216, 213, 211, 208, 205, 203, 200, 197, 195, 192, 190, 187, 185, 183, 180, 178, 175, 173, 170, 167, 164, 161, 158, 155, 152, 148, 145, 142, 139, 135, 132, 128, 125, 121, 118, 115, 111, 108, 105, 101, 98, 95, 92, 89, 86, 83, 80, 78, 75, 73, 70, 68, 66, 64, 62, 61, 59, 58, 57, 56, 55, 54, 53, 53, 52, 52, 52, 52, 51, 51, 52, 52, 52, 52, 52, 53, 53, 53, 54, 54, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 58, 58, 58, 58, 58, 57, 57, 56, 56, 55, 54, 53, 53, 52, 51, 50, 49, 47, 46, 45, 44, 43, 42, 40, 39, 38, 36, 35, 34, 32, 31, 30, 28, 27, 26, 25, 23, 22, 21, 20, 19, 17, 16, 15, 14, 14, 13, 12, 11, 10, 10, 9, 9, 8, 7, 7, 6, 6, 5, 5, 5, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2 },
		};
		cv::Mat channels[] = { cv::Mat(256,1, CV_8U, (unsigned char*)depthLut_RGB[2]), cv::Mat(256,1, CV_8U, (unsigned char*)depthLut_RGB[1]), cv::Mat(256,1, CV_8U, (unsigned char*)depthLut_RGB[0]) };

		// Merge 3 channels of the RGB values into one MAT that will be used as the look uptable depth colors
		merge(channels, 3, depthLut);

      if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_DISPARITY_DEBUG_E)
      {
         //output into new interleaved image
         imgHdrP->imgDescriptor.stride = imgHdrP->imgDescriptor.stride * 2;
      }
      memset(titleName, 0, sizeof(titleName));
      sprintf(titleName, "%s(%d)", inu_ref__getUserName(ref), inu_ref__getRefId(ref));
      CreateCVWindow(titleName, imgHdrP->imgDescriptor.stride, imgHdrP->imgDescriptor.bufferHeight);
      //save the win name on test params, for destroy win while closing
      GET_WIN_NAME(tmpName, titleName, imgHdrP->imgDescriptor.stride, imgHdrP->imgDescriptor.bufferHeight);
      strcpy(channelParamsP->winName, tmpName);
      if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_YUV420_SEMI_PLANAR_E)
	  {
			channelParamsP->bufferP = (char*)malloc(IMAGE_SIZE_PIXELS(imgHdrP)*4);
			memset(channelParamsP->bufferP, 0, IMAGE_SIZE_PIXELS(imgHdrP) * 4);
      }
      else if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_YUV422_8BIT_E || imgHdrP->imgDescriptor.format == NUCFG_FORMAT_GEN_16_E)//enough for RGB camera recognation?
      {
         channelParamsP->bufferP = (char*)malloc(IMAGE_SIZE_PIXELS(imgHdrP) * 4);
         memset(channelParamsP->bufferP, 0, IMAGE_SIZE_PIXELS(imgHdrP) * 4);
      }
      else if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_BAYER_16_E)
      {
         channelParamsP->bufferP = (char*)malloc(IMAGE_SIZE_BYTES(imgHdrP));
         memset(channelParamsP->bufferP, 0, IMAGE_SIZE_BYTES(imgHdrP));
      }
      else
      {
         channelParamsP->bufferP = (char*)malloc(IMAGE_SIZE_BYTES(imgHdrP));
         memset(channelParamsP->bufferP, 0, IMAGE_SIZE_BYTES(imgHdrP));
      }
      channelParamsP->yLoc = 0;
#endif
	}
}

void imageStopCb(inu_refH ref, void *cookie)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	stopCb(ref, cookie);
	if (channelParamsP->displayOutput)
	{
		free(channelParamsP->bufferP);
#if defined _OPENCV_
		cv::destroyWindow(channelParamsP->winName);
#endif
	}
}
void imagefailCallback(inu_refH ref,void *cookie,inu_deviceH* device)
{
	/*Can be used to start triggers if needed*/
}
void imageRecursiveStartDone(inu_refH ref,void *cookie,inu_deviceH* device)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	if(channelParamsP)
	{
		if(channelParamsP->startAllStreams)
		{
			startAllTriggersExample(device);
		}	
	}
}
void printSensor(INU_metadata_Sensor *sensorState)
{
	switch (sensorState->sensorType)
	{
	case INU_metadata_SensorType_AF_VST:
		printf("\t Sensor: AF VST \n");
		break;
	case INU_metadata_SensorType_Gaze:
		printf("\t Sensor: Gaze \n");
		break;
	case INU_metadata_SensorType_Tracking:
		printf("\t Sensor: Tracking \n");
		break;
	case INU_metadata_SensorType_FF_VST:
		printf("\t Sensor: FF VST \n");
		break;
	case INU_metadata_SensorType_CVA:
		printf("\t Sensor: CVA \n");
		break;
	case INU_metadata_SensorType_Gaze_ROI:
		printf("\t Sensor: Gaze ROI \n");
		break;
	}
	switch (sensorState->side)
	{
	case INU_metadata_Channel_Side_Left:
		printf("\t Sensor side: Left \n");
		break;
	case INU_metadata_Channel_Side_Right:
		printf("\t Sensor side: Right \n");
	}
	if(sensorState->has_sensorID)
	{
		printf("\t Sensor ID: %lu \n", sensorState->sensorID);
	}
	switch(sensorState->channelScale)
	{
		case INU_metadata_Channel_Scale_Normal:
			printf("\t Image scale: Normal \n");
			break;
		case INU_metadata_Channel_Scale_Cropped:
			printf("\t Image scale: Cropped \n");
			break;
		case INU_metadata_Channel_Scale_Downscaled:
			printf("\t Image scale: Downscaled \n");
			break;
		break;
	}
}
void   printState(INU_metadata_State *inuState)
{
	if (inuState->has_sensor_metadata)
	{
		if (inuState->sender == INU_metadata_HardwareType_MASTER)
		{
			printf("\t Sender: Master \n");
		}
		else if (inuState->sender == INU_metadata_HardwareType_SLAVE)
		{
			printf("\t Sender: Slave \n");
		}
		else if (inuState->sender == INU_metadata_HardwareType_HOST)
		{
			printf("\t Sender: Host \n");
		}
		
		if (inuState->sensor_metadata.has_exposure)
		{
			printf("\t Exposure: %lu \n", inuState->sensor_metadata.exposure.exposure);
		}
		if (inuState->sensor_metadata.has_gain)
		{
			printf("\t Analogue Gain: %.3f, Digital Gain: %.3f \n", inuState->sensor_metadata.gain.analogue_gain, inuState->sensor_metadata.gain.digital_gain);
		}
		if (inuState->sensor_metadata.has_roi)
		{
			printf("\t ROI Start X,%lu ROI Start Y, %lu \n", inuState->sensor_metadata.roi.x_start, inuState->sensor_metadata.roi.y_start);
		}
	}
	if (inuState->has_snsr_update_timing && inuState->snsr_update_timing.has_sensor_update_dequed_timing && inuState->snsr_update_timing.has_sensor_update_i2cWritesComplete)
	{
		// writeTime can be used to look at the I2C write times
		UINT64 writeTime = inuState->snsr_update_timing.sensor_update_i2cWritesComplete.timestamp.nanoseconds - inuState->snsr_update_timing.sensor_update_dequed_timing.timestamp.nanoseconds;
		printf("\t I2C Write time: %llu us \n", writeTime / 1000);
	}
	if (inuState->has_snsr_update_timing && inuState->snsr_update_timing.has_sensor_update_i2cWritesComplete && inuState->snsr_update_timing.has_sensor_update_rx_timing)
	{
		// Time difference between receiving the sensor update and the I2C writes being complete
		UINT64 writeTime = inuState->snsr_update_timing.sensor_update_i2cWritesComplete.timestamp.nanoseconds - inuState->snsr_update_timing.sensor_update_rx_timing.timestamp.nanoseconds;
		printf("\t RX->I2C Write Time: %llu us \n", writeTime / 1000);
	}
	if(inuState->sensor_metadata.has_isp_expo && inuState->sensor_metadata.isp_expo.has_uart_sync )
		printf("\t ISP Expo command ID %lu \n",inuState->sensor_metadata.isp_expo.uart_sync.cmd_id );
	if(inuState->has_sensor)
	{
		printSensor(&inuState->sensor);
	}
}

void printMetadata(INU_Metadata_T* metadata)
{
	if (metadata != NULL)
	{
		/* Top 32 bits are the second value since Linux Epoch
		  Bottom 32 bits are the fractional part of the second, 
		  least significant bit = 1/(1<<32 - 1) seconds
		*/
		UINT32 second = (metadata->timestamp >> 32); 
		UINT32 frac_part = (metadata->timestamp & 0xFFFFFFFF);
		float frac_s = (double) frac_part/((double)0xFFFFFFFF);	/* Scale frac_part to be from 0->1*/
		/*Print out the fractional part of frac_s */
		char fractional_print[20];
		snprintf(fractional_print,sizeof(fractional_print),"%.7f \n", frac_s);
		/*Convert the time since epoch into the standard date and time structure (tm) */
		time_t rawtime = second;	
		struct tm *info;
		info = gmtime(&rawtime );
		/*Print out the date structure*/
		char buffer[100];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", info);
		/*Get the current host time*/
		UINT64 time_usec = 0;
		OS_LYRG_getUsecTime(&time_usec);
		/*Convert host time to double*/
		double host_time = (double)time_usec / 1E6;
		/*Convert target time to double*/
		double target_time = second*1.0 + frac_s;
		/*Calculate time difference in seconds between host and target*/
		double time_difference_seconds = host_time - target_time;
		/*Print out the date and time plus other metadata */																							
		printf("Fixed data Metadata, Timestamp: %s.%s Frame ID: %lu, Protobuf metadata, Timestamp:%llu, Frame ID:%llu, Serialization counter:%lu, Host_Target_Time_Difference: %.6fs\n ", buffer,&fractional_print[2], metadata->frameID,
			metadata->protobuf_packet.timestamp_ns.nanoseconds, metadata->protobuf_packet.metadatacounter, metadata->serialization_counter, time_difference_seconds);
		if (metadata->protobuf_packet.state_count > 0)
		{
			for (int i = 0; i < metadata->protobuf_packet.state_count; i++)
			{
				printState(&metadata->protobuf_packet.state[i]);
			}
		}
		if (metadata->protobuf_packet.sensors_count > 0)
		{
			for (int i = 0; i < metadata->protobuf_packet.sensors_count; i++)
			{
				printf("\t Printing Sensor table \n");
				printSensor(&metadata->protobuf_packet.sensors[i]);
			}
		}
	}
}
#define CIIF_PERIOD (1000*1000*1000)/3
void sendSensorSyncExample(inu_deviceH* device)
{
	
	static UINT64 previous_sensorSync = 0;
	static int counter = 0;
	static bool pulseEnable = true;
	UINT64 time_ns, time_us = 0;
	OS_LYRG_getUsecTime(&time_us);
	UINT64 timeDiff = time_us - previous_sensorSync;
	if (timeDiff > 200E3) /*Limit updates to 1Hz due to Synchronous message crash issue*/
	{
		pulseEnable = true;

		counter++;
		inu_sensorSync__updateT sensorSync;
		sensorSync.protobuf_packet = SENSOR_SYNC_topLevel_init_zero;
		int index =0;
		sensorSync.protobuf_packet.configs_count = 1;
		/*Counters configuration*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.counter = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_0;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.countMode.countMode = SENSOR_SYNC_FSG_Count_mode_FSG_CNTMODE_SWMODE;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_countMode = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.triggerSource.triggerSource = 12;	/*SW_Sync 0*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_triggerSource  = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.value.counter_value = CIIF_PERIOD/75UL;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_value = true;
		sensorSync.protobuf_packet.configs[index].has_counterConfiguration = true;
		/*Channel configuration */
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseChannel = SENSOR_SYNC_FSG_Pulse_Channel_FSG_PULSE_0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseEnable = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulsePolarity = SENSOR_SYNC_Pulse_Polarity_Active_High;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseWidth = 255;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseOffset = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseRepeatPeriod = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.has_counterSource = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.counterSource.counter_source = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_0;
		sensorSync.protobuf_packet.configs[index].has_channelConfiguration  = true;
		inu_device__sensorSyncCfg(*device, &sensorSync);
		previous_sensorSync = time_us;
		
	}

}

void demoFSGConfiguration(inu_deviceH* device)
{
	
	static UINT64 previous_sensorSync = 0;
	static int counter = 0;
	static bool pulseEnable = true;
	UINT64 time_ns, time_us = 0;
	OS_LYRG_getUsecTime(&time_us);
	UINT64 timeDiff = time_us - previous_sensorSync;
	if (timeDiff > 200E3) /*Limit updates to 1Hz due to Synchronous message crash issue*/
	{
		pulseEnable = true;

		counter++;
		inu_sensorSync__updateT sensorSync;
		sensorSync.protobuf_packet = SENSOR_SYNC_topLevel_init_zero;
		sensorSync.protobuf_packet.configs_count = 3;
		int index = 0;

		/*Counters configuration*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.counter = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_0;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.countMode.countMode = SENSOR_SYNC_FSG_Count_mode_FSG_CNTMODE_SWMODE;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_countMode = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.triggerSource.triggerSource = 12;	/*SW_Sync 0*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_triggerSource  = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.value.counter_value = CIIF_PERIOD/90UL;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_value = true;
		sensorSync.protobuf_packet.configs[index].has_counterConfiguration = true;
		/*Channel configuration */
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseChannel = SENSOR_SYNC_FSG_Pulse_Channel_FSG_PULSE_0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseEnable = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulsePolarity = SENSOR_SYNC_Pulse_Polarity_Active_High;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseWidth = 255;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseOffset = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseRepeatPeriod = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.has_counterSource = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.counterSource.counter_source = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_0;
		sensorSync.protobuf_packet.configs[index].has_channelConfiguration  = true;
		index++;

		/*Counters configuration*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.counter = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_1;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.countMode.countMode = SENSOR_SYNC_FSG_Count_mode_FSG_CNTMODE_SWMODE;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_countMode = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.triggerSource.triggerSource = 13;	/*SW_Sync 0*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_triggerSource  = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.value.counter_value = CIIF_PERIOD/90UL;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_value = true;
		sensorSync.protobuf_packet.configs[index].has_counterConfiguration = true;
		/*Channel configuration */
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseChannel = SENSOR_SYNC_FSG_Pulse_Channel_FSG_PULSE_1;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseEnable = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulsePolarity = SENSOR_SYNC_Pulse_Polarity_Active_High;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseWidth = 255;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseOffset = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseRepeatPeriod = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.has_counterSource = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.counterSource.counter_source = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_1;
		sensorSync.protobuf_packet.configs[index].has_channelConfiguration  = true;

		index++;

		/*Counters configuration*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.counter = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_2;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.countMode.countMode = SENSOR_SYNC_FSG_Count_mode_FSG_CNTMODE_SWMODE;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_countMode = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.triggerSource.triggerSource = 13;	/*SW_Sync 0*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_triggerSource  = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.value.counter_value = CIIF_PERIOD/90UL;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_value = true;
		sensorSync.protobuf_packet.configs[index].has_counterConfiguration = true;
		/*Channel configuration */
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseChannel = SENSOR_SYNC_FSG_Pulse_Channel_FSG_PULSE_1;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseEnable = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulsePolarity = SENSOR_SYNC_Pulse_Polarity_Active_High;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseWidth = 255;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseOffset = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseRepeatPeriod = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.has_counterSource = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.counterSource.counter_source = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_2;
		sensorSync.protobuf_packet.configs[index].has_channelConfiguration  = true;

		
		inu_device__sensorSyncCfg(*device, &sensorSync);
		previous_sensorSync = time_us;
		std::cout << "Starting all streams " << std::endl;
		
	}

}
void startAllTriggersExample(inu_deviceH* device)
{
	
	static UINT64 previous_sensorSync = 0;
	static int counter = 0;
	static bool pulseEnable = true;
	UINT64 time_ns, time_us = 0;
	OS_LYRG_getUsecTime(&time_us);
	UINT64 timeDiff = time_us - previous_sensorSync;
	if (timeDiff > 200E3) /*Limit updates to 1Hz due to Synchronous message crash issue*/
	{
		pulseEnable = true;

		counter++;
		inu_sensorSync__updateT sensorSync;
		sensorSync.protobuf_packet = SENSOR_SYNC_topLevel_init_zero;
		sensorSync.protobuf_packet.configs_count = 3;
		int index = 0;
		/*Starts all FSG counters at the same time*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.counter = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_0;
		sensorSync.protobuf_packet.configs[index].has_counterConfiguration = true;
		index++;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.counter = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_1;
		sensorSync.protobuf_packet.configs[index].has_counterConfiguration  = true;
		index++;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.counter = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_2;
		sensorSync.protobuf_packet.configs[index].has_counterConfiguration  = true;

		
		inu_device__sensorSyncCfg(*device, &sensorSync);
		previous_sensorSync = time_us;
		std::cout << "Starting all triggers " << std::endl;
		
	}

}


/**
 * @brief This should only be sent to the slave, this function will switch the trigger source for the Slave tracking sensors to be the trigger_sync signal from the master. 
 *											 
 *
 * @param device Device handle
 */
void sendSensorSyncSwitchToTriggeringFromTheMaster(inu_deviceH* device)
{
	static UINT64 previous_sensorSync = 0;
	static int counter = 0;
	static bool pulseEnable = true;
	UINT64 time_ns, time_us = 0;
	OS_LYRG_getUsecTime(&time_us);
	UINT64 timeDiff = time_us - previous_sensorSync;
	if (timeDiff > 200E3) /*Limit updates to 1Hz due to Synchronous message crash issue*/
	{
		pulseEnable = true;

		counter++;
		inu_sensorSync__updateT sensorSync;
		sensorSync.protobuf_packet = SENSOR_SYNC_topLevel_init_zero;
		sensorSync.protobuf_packet.configs_count = 1;
		int index = 0;

		/*Counters configuration*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.counter = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_0;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.countMode.countMode = SENSOR_SYNC_FSG_Count_mode_FSG_CNTMODE_HWMODE;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_countMode = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.triggerSource.triggerSource = 11;	/*External triggering*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_triggerSource  = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.value.counter_value = 0xFFFFFFFF;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_value = true;
		sensorSync.protobuf_packet.configs[index].has_counterConfiguration = true;
		/*Channel configuration */
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseChannel = SENSOR_SYNC_FSG_Pulse_Channel_FSG_PULSE_0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseEnable = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulsePolarity = SENSOR_SYNC_Pulse_Polarity_Active_High;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseWidth = 255;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseOffset = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseRepeatPeriod = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.has_counterSource = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.counterSource.counter_source = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_0;
		sensorSync.protobuf_packet.configs[index].has_channelConfiguration  = true;
		index++;
		inu_device__sensorSyncCfg(*device, &sensorSync);
		previous_sensorSync = time_us;
		
	}
}
/**
 * @brief This should only be sent to the slave, this function will switch the trigger source for the Slave tracking sensors to be the internal counter on the slave.
 *											 
 *
 * @param device Device handle
 */
void sendSensorSyncSwitchToInteralTriggering(inu_deviceH* device)
{
	static UINT64 previous_sensorSync = 0;
	static int counter = 0;
	static bool pulseEnable = true;
	UINT64 time_ns, time_us = 0;
	OS_LYRG_getUsecTime(&time_us);
	UINT64 timeDiff = time_us - previous_sensorSync;
	if (timeDiff > 200E3) /*Limit updates to 1Hz due to Synchronous message crash issue*/
	{
		pulseEnable = true;

		counter++;
		int index =0;
		inu_sensorSync__updateT sensorSync;
		sensorSync.protobuf_packet = SENSOR_SYNC_topLevel_init_zero;
		/*Counters configuration*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.counter = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_0;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.countMode.countMode = SENSOR_SYNC_FSG_Count_mode_FSG_CNTMODE_SWMODE;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_countMode = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.triggerSource.triggerSource = 12;	/*External triggering*/
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_triggerSource  = true;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.value.counter_value = CIIF_PERIOD/90;
		sensorSync.protobuf_packet.configs[index].counterConfiguration.has_value = true;
		sensorSync.protobuf_packet.configs[index].has_counterConfiguration = true;
		/*Channel configuration */
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseChannel = SENSOR_SYNC_FSG_Pulse_Channel_FSG_PULSE_0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseEnable = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulsePolarity = SENSOR_SYNC_Pulse_Polarity_Active_High;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseWidth = 255;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseOffset = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.pulseRepeatPeriod = 0;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.has_counterSource = true;
		sensorSync.protobuf_packet.configs[index].channelConfiguration.counterSource.counter_source = SENSOR_SYNC_FSG_Counter_FSG_COUNTER_0;
		sensorSync.protobuf_packet.configs[index].has_channelConfiguration  = true;
		index++;
		inu_device__sensorSyncCfg(*device, &sensorSync);
		previous_sensorSync = time_us;
		
	}
}
void sendROIUpdateExample(inu_deviceH* device)
{
	#define MAX_X_POSITION ((640) - (512))
	#define MAX_Y_POSITION ((480) - (304))
	static UINT64 previous_sensorUpdateTS = 0;
	static int startX = 0;
	static int startY = 0;
	INU_Metadata_T snsrUpdate = ZEROED_OUT_METADATA;
	UINT64 time_ns, time_us = 0;
	OS_LYRG_getUsecTime(&time_us);
	UINT64 timeDiff = time_us - previous_sensorUpdateTS;
	if (timeDiff > 1000E3) /*Limit updates to 1Hz due to Synchronous message crash issue*/
	{
		if(startX < MAX_X_POSITION)
		{
			startX+= 256;
		}
		else{
			startX =0;
		}
		if(startY < MAX_Y_POSITION)
		{
			startY++;
		}
		else
		{
			startY = 0;
		}

		time_ns = time_us * 1000; /*Convert to ns*/
		/*Unused by the target*/
		snsrUpdate.framebuffer_offset = 0;
		snsrUpdate.frameID = 0;
		snsrUpdate.serialization_counter = 1;
		snsrUpdate.timestamp = time_ns;
		/*Used by the target*/
		snsrUpdate.protobuf_packet.state[0].timestamp_ns.nanoseconds = time_ns;
		snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
		snsrUpdate.protobuf_packet.state[0].sender = INU_metadata_HardwareType_HOST;
		snsrUpdate.protobuf_packet.state[0].target = INU_metadata_HardwareType_MASTER;
		snsrUpdate.protobuf_packet.state[0].sensor.sensorType = INU_metadata_SensorType_Gaze_ROI;
		snsrUpdate.protobuf_packet.state[0].sensor.side = INU_metadata_Channel_Side_Left;
		snsrUpdate.protobuf_packet.state[0].sensor.channelScale = INU_metadata_Channel_Scale_Cropped;
		snsrUpdate.protobuf_packet.state[0].has_sensor = true;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.exposure.exposure = 0;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_exposure = false;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_gain = false;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.gain.analogue_gain = 0;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.gain.digital_gain = (float)0;
		snsrUpdate.protobuf_packet.state[0].has_sensor_metadata = true;
		snsrUpdate.protobuf_packet.state[0].has_timestamp_ns = true;
		snsrUpdate.protobuf_packet.state[0].context = INU_metadata_Context_Context_A;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_roi = true;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.roi.x_start = startX;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.roi.y_start = startY;

		snsrUpdate.protobuf_packet.state[1].timestamp_ns.nanoseconds = time_ns;
		snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
		snsrUpdate.protobuf_packet.state[1].sender = INU_metadata_HardwareType_HOST;
		snsrUpdate.protobuf_packet.state[1].target = INU_metadata_HardwareType_MASTER;
		snsrUpdate.protobuf_packet.state[1].sensor.sensorType = INU_metadata_SensorType_Gaze_ROI;
		snsrUpdate.protobuf_packet.state[1].sensor.side = INU_metadata_Channel_Side_Right;
		snsrUpdate.protobuf_packet.state[1].sensor.channelScale = INU_metadata_Channel_Scale_Cropped;
		snsrUpdate.protobuf_packet.state[1].has_sensor = true;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.exposure.exposure = 0;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.has_exposure = false;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.has_gain = false;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.gain.analogue_gain = 0;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.gain.digital_gain = (float)0;
		snsrUpdate.protobuf_packet.state[1].has_sensor_metadata = true;
		snsrUpdate.protobuf_packet.state[1].has_timestamp_ns = true;
		snsrUpdate.protobuf_packet.state[1].context = INU_metadata_Context_Context_A;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.has_roi = true;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.roi.x_start = startX;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.roi.y_start = startY;
		snsrUpdate.protobuf_packet.state_count = 2;

		inu_device__sensorUpdate(*device, &snsrUpdate);
		previous_sensorUpdateTS = time_us;

	}
}
static int currentMode_ = 0;
static int currentX_ = 0;
static int currentY_ = 0;
static int randomModeCount_ = 0;
std::pair<float, float> ROI_newValue(int maxX_, int maxY_, int minX_,int minY_)
{
	/*Taken from the test tool*/


        //std::cout << " mode: " << currentMode_ << std::endl;
        if (currentMode_ == 0)
        {
            if (randomModeCount_ < 100)
            {
                currentX_ = rand() % (int)maxX_;
                currentY_ = rand() % (int)maxY_;
                randomModeCount_++;
            }
            else
            {
                randomModeCount_ = 0;

                currentX_ = minX_;
                currentY_ = minY_;
                currentMode_ = 1;
                //std::cout << "change mode to 0" << std::endl;
            }
        }
        if (currentMode_ == 1)
        {
            currentX_ += (maxX_ - minX_) / 10.0f;
            if (currentX_ > maxX_)
            {
                currentX_ = minX_;
                currentY_ += (maxY_ - minY_) / 10.0f;
                if (currentY_ > maxY_)
                {
                    currentY_ = minY_;
                    currentMode_ = 2;
                    //std::cout << "change mode to 1" << std::endl;
                }
            }
        }
        else if (currentMode_ == 2)
        {
            currentY_ += (maxY_ - minY_) / 10.0f;
            if (currentY_ > maxY_)
            {
                currentY_ = minY_;
                currentX_ += (maxX_ - minX_) / 10.0f;
                if (currentX_ > maxX_)
                {
                    currentX_ = maxX_;
                    currentY_ = maxY_;
                    currentMode_ = 3;
                    //std::cout << "change mode to 2" << std::endl;
                }
            }
        }
        else if (currentMode_ == 3)
        {
            currentX_ -= (maxX_ - minX_) / 10.0f;
            if (currentX_ < minX_)
            {
                currentX_ = maxX_;
                currentY_ -= (maxY_ - minY_) / 10.0f;
                if (currentY_ < minY_)
                {
                    currentX_ = maxX_;
                    currentY_ = maxY_;
                    currentMode_ = 4;
                    //std::cout << "change mode to 3" << std::endl;
                }
            }
        }
        else if (currentMode_ == 4)
        {
            currentY_ -= (maxY_ - minY_) / 10.0f;
            if (currentY_ < minY_)
            {
                currentY_ = maxY_;
                currentX_ -= (maxX_ - minX_) / 10.0f;
                if (currentX_ < minX_)
                {
                    currentX_ = minX_;
                    currentY_ = minY_;
                    currentMode_ = 0;
                    //std::cout << "change mode to 0" << std::endl;
                }
            }
}
        return std::make_pair(currentX_, currentY_);
}

int roundUp(int numToRound, int multiple) 
{
    assert(multiple);
    return ((numToRound + multiple - 1) / multiple) * multiple;
}

/**
 * @brief Sends an example message which changes the AF VST crop window
 * 
 *
 * @param device INU Device handle
 */
void sendAFVSTCropWindowUpdateExample(inu_deviceH* device)
{
	static UINT64 previous_sensorUpdateTS = 0;
	static int cropwindow_x = 0 ;
	static int cropwindow_y = 0;
	
	INU_Metadata_T snsrUpdate = ZEROED_OUT_METADATA;
	UINT64 time_ns, time_us = 0;
	OS_LYRG_getUsecTime(&time_us);
	UINT64 timeDiff = time_us - previous_sensorUpdateTS;
	if (timeDiff > 20E3) /*Limit updates to 45Hz due to high CPU usage when sending i2c commmands for every frame*/
	{	
		//printf("Time difference %llu \n", timeDiff);
		time_ns = time_us * 1000; /*Convert to ns*/
		/*Unused by the target*/
		snsrUpdate.framebuffer_offset = 0;
		snsrUpdate.frameID = 0;
		snsrUpdate.serialization_counter = 1;
		snsrUpdate.timestamp = time_ns;
		/*Used by the target*/
		auto position = ROI_newValue(MAX_AF_VST_CROP_WINDOW_X,MAX_AF_VST_CROP_WINDOW_Y,0,0);

		cropwindow_x = ((int)position.first/4) * 4;
		cropwindow_y = ((int)position.second/4)*4;

		snsrUpdate.protobuf_packet.state[0].timestamp_ns.nanoseconds = time_ns;
		snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
		snsrUpdate.protobuf_packet.state[0].sender = INU_metadata_HardwareType_HOST;
		snsrUpdate.protobuf_packet.state[0].target = INU_metadata_HardwareType_MASTER;
		snsrUpdate.protobuf_packet.state[0].sensor.sensorType = INU_metadata_SensorType_AF_VST;
		snsrUpdate.protobuf_packet.state[0].sensor.side = INU_metadata_Channel_Side_Left;
		snsrUpdate.protobuf_packet.state[0].sensor.channelScale = INU_metadata_Channel_Scale_Cropped;
		snsrUpdate.protobuf_packet.state[0].has_sensor = true;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.exposure.exposure = 0;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_exposure = false;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_gain = false;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.gain.analogue_gain = 0;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.gain.digital_gain = (float)0;
		snsrUpdate.protobuf_packet.state[0].has_sensor_metadata = true;
		snsrUpdate.protobuf_packet.state[0].has_timestamp_ns = true;
		snsrUpdate.protobuf_packet.state[0].context = INU_metadata_Context_Context_A;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_AFCropPositon = true;
		/*This is where the crop window is set*/
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.AFCropPositon.StartX = cropwindow_x;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.AFCropPositon.StartY = cropwindow_y;
		snsrUpdate.protobuf_packet.state_count = 1;
	
		inu_device__sensorUpdate(*device, &snsrUpdate);
		previous_sensorUpdateTS = time_us;
		
	}
}
/**
 * @brief Sends an example message which changes the AF VST focus between MIN_AF_VST_FOCUS and MAX_AF_VST_FOCUS
 * 
 *
 * @param device INU Device handle
 */
void sendAFVSTFocus(inu_deviceH* device)
{
	static UINT64 previous_sensorUpdateTS = 0;
	static int focus = 0;
	if(focus > MAX_AF_VST_FOCUS)
	{
		focus = MIN_AF_VST_FOCUS;
	}
	INU_Metadata_T snsrUpdate = ZEROED_OUT_METADATA;
	UINT64 time_ns, time_us = 0;
	OS_LYRG_getUsecTime(&time_us);
	UINT64 timeDiff = time_us - previous_sensorUpdateTS;
	if (timeDiff > 0) 
	{
		time_ns = time_us * 1000; /*Convert to ns*/
		/*Unused by the target*/
		snsrUpdate.framebuffer_offset = 0;
		snsrUpdate.frameID = 0;
		snsrUpdate.serialization_counter = 1;
		snsrUpdate.timestamp = time_ns;
		/*Used by the target*/
		snsrUpdate.protobuf_packet.state[0].timestamp_ns.nanoseconds = time_ns;
		snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
		snsrUpdate.protobuf_packet.state[0].sender = INU_metadata_HardwareType_HOST;
		snsrUpdate.protobuf_packet.state[0].target = INU_metadata_HardwareType_MASTER;
		snsrUpdate.protobuf_packet.state[0].sensor.sensorType = INU_metadata_SensorType_AF_VST;
		snsrUpdate.protobuf_packet.state[0].sensor.side = INU_metadata_Channel_Side_Left;
		snsrUpdate.protobuf_packet.state[0].sensor.channelScale = INU_metadata_Channel_Scale_Downscaled;
		snsrUpdate.protobuf_packet.state[0].has_sensor = true;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.exposure.exposure = 0;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_exposure = false;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_gain = false;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.gain.analogue_gain = 0;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.gain.digital_gain = (float)0;
		snsrUpdate.protobuf_packet.state[0].has_sensor_metadata = true;
		snsrUpdate.protobuf_packet.state[0].has_timestamp_ns = true;
		snsrUpdate.protobuf_packet.state[0].context = INU_metadata_Context_Context_A;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_AFFocusPosition = true;


		snsrUpdate.protobuf_packet.state[1].timestamp_ns.nanoseconds = time_ns;
		snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
		snsrUpdate.protobuf_packet.state[1].sender = INU_metadata_HardwareType_HOST;
		snsrUpdate.protobuf_packet.state[1].target = INU_metadata_HardwareType_SLAVE;
		snsrUpdate.protobuf_packet.state[1].sensor.sensorType = INU_metadata_SensorType_AF_VST;
		snsrUpdate.protobuf_packet.state[1].sensor.side = INU_metadata_Channel_Side_Left;
		snsrUpdate.protobuf_packet.state[1].sensor.channelScale = INU_metadata_Channel_Scale_Downscaled;
		snsrUpdate.protobuf_packet.state[1].has_sensor = true;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.exposure.exposure = 0;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.has_exposure = false;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.has_gain = false;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.gain.analogue_gain = 0;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.gain.digital_gain = (float)0;
		snsrUpdate.protobuf_packet.state[1].has_sensor_metadata = true;
		snsrUpdate.protobuf_packet.state[1].has_timestamp_ns = true;
		snsrUpdate.protobuf_packet.state[1].context = INU_metadata_Context_Context_A;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.has_AFFocusPosition = true;
		/*This is where the focus is set*/
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.AFFocusPosition.AFFocusPosition = focus;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.AFFocusPosition.AFFocusPosition = focus;
		focus++;
		snsrUpdate.protobuf_packet.state_count = 2;
	
		inu_device__sensorUpdate(*device, &snsrUpdate);
		previous_sensorUpdateTS = time_us;
	}
}
/**
 * @brief Sends an example message which changes the AF VST WB gains on the cropped streams by reading 
 * 		   the AF VST downscaled stream wb gains and then sends them to the cropped stream ISPs
 * 
 *
 * @param device INU Device handle
 */
void sendISPGains(inu_deviceH* device)
{
	static UINT64 previous_sensorUpdateTS = 0;
	static int counter = 0;
	inu_isp_channel__ispCmdParamU ispCmdParam,expoParams;
	counter++;
	if(previous_sensorUpdateTS == 0)
		OS_LYRG_getUsecTime(&previous_sensorUpdateTS);
	INU_Metadata_T snsrUpdate = ZEROED_OUT_METADATA;
	/*Wait until we have 100 frames*/
	if(counter > 200)
	{
		return;
	}
	{
		ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
		inu_nodeH streamerH = NULL;
		/*Read the white balance from the master ISP0 (downscaled stream) */
		ret = inu_device__findNode(*device, "INU_ISP_CHANNEL_23", &streamerH);
		if (ERRG_SUCCEEDED(ret))
		{
			ret = FWLIB_getWBGains(streamerH,&ispCmdParam);
			if (ERRG_FAILED(ret))
			{
				printf("Coudln't get the ISP gains \n");
				return;
			}
			ret = FWLIB_getExpoParams(streamerH,&expoParams);
			if (ERRG_FAILED(ret))
			{
				printf("Coudln't get the ISP expo params \n");
				return;
			}

			inu_isp_channel__ispCmdParamU *p = &ispCmdParam;
			printf("ISPCORE_MODULE_WB_CFG_GET: wbGains.Blue:[%.3f], wbGains.GreenB:[%.3f], wbGains.GreenR:[%.3f], wbGains.Red:[%.3f]\n",
				p->wbParam.wbGain.gainB, p->wbParam.wbGain.gainGb, p->wbParam.wbGain.gainGr,p->wbParam.wbGain.gainR);
          	printf("ISPCORE_MODULE_WB_CFG_GET: ccOffset.Blue:[%d], ccOffset.Green:[%d], ccOffset.Red:[%d]\n",
			            p->wbParam.ccOffset.blue, p->wbParam.ccOffset.green, p->wbParam.ccOffset.red);
			/*Print out the exposure parameters*/
			printf("ISP_COMMAND_GET_EXPOSURE: intTime %f anGain %f ispGain %f\n", expoParams.exposureParam.integrationTime, expoParams.exposureParam.analogGain, expoParams.exposureParam.ispGain);   

			/*We have now read the WB gains */
			snsrUpdate.framebuffer_offset = 0;
			snsrUpdate.frameID = 0;
			snsrUpdate.serialization_counter = 1;
			snsrUpdate.timestamp = 0;
			/*Used by the target*/
			snsrUpdate.protobuf_packet.state[0].timestamp_ns.nanoseconds = 0;
			snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
			snsrUpdate.protobuf_packet.state[0].sender = INU_metadata_HardwareType_HOST;
			snsrUpdate.protobuf_packet.state[0].target = INU_metadata_HardwareType_MASTER;
			snsrUpdate.protobuf_packet.state[0].sensor.sensorType = INU_metadata_SensorType_AF_VST;
			snsrUpdate.protobuf_packet.state[0].sensor.side = INU_metadata_Channel_Side_Left;
			snsrUpdate.protobuf_packet.state[0].sensor.channelScale = INU_metadata_Channel_Scale_Downscaled;
			snsrUpdate.protobuf_packet.state[0].has_sensor = true;
			snsrUpdate.protobuf_packet.state[0].has_sensor_metadata = true;
			snsrUpdate.protobuf_packet.state[0].has_timestamp_ns = true;
			snsrUpdate.protobuf_packet.state[0].context = INU_metadata_Context_Context_A;
			/*Send to the master and the slave*/
			snsrUpdate.protobuf_packet.state[1].timestamp_ns.nanoseconds = 0;
			snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
			snsrUpdate.protobuf_packet.state[1].sender = INU_metadata_HardwareType_HOST;
			snsrUpdate.protobuf_packet.state[1].target = INU_metadata_HardwareType_MASTER;
			snsrUpdate.protobuf_packet.state[1].sensor.sensorType = INU_metadata_SensorType_AF_VST;
			snsrUpdate.protobuf_packet.state[1].sensor.side = INU_metadata_Channel_Side_Left;
			snsrUpdate.protobuf_packet.state[1].sensor.channelScale = INU_metadata_Channel_Scale_Cropped;
			snsrUpdate.protobuf_packet.state[1].has_sensor = true;
			snsrUpdate.protobuf_packet.state[1].has_sensor_metadata = true;
			snsrUpdate.protobuf_packet.state[1].has_timestamp_ns = true;
			snsrUpdate.protobuf_packet.state[1].context = INU_metadata_Context_Context_A;
			/*This is where the colour correction coeffcients are copied from */
			for(int i=0; i < 2; i ++)
			{
				for(int j=0; j< 9; j++)
				{	/*Set the colour correction matrix*/
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.colourcorrectinCoefficients.coeff[j] = p->wbParam.ccMatrix.coeff[j];
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.colourcorrectinCoefficients.coeff_count++;
				}
				/*Set the colour correction offsets to be the same as downscaled*/
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.colourcorrection.blue = p->wbParam.ccOffset.blue;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.colourcorrection.green = p->wbParam.ccOffset.green;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.colourcorrection.red = p->wbParam.ccOffset.red;
				/*Set the gains to be gain*/
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.wbGains.gainR = p->wbParam.wbGain.gainR;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.wbGains.gainGr = p->wbParam.wbGain.gainGr;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.wbGains.gainGb = p->wbParam.wbGain.gainGb;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.wbGains.gainB = p->wbParam.wbGain.gainB;
				/*Copy the exposure parameters*/
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.analogGain = expoParams.exposureParam.analogGain;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.integrationTime = expoParams.exposureParam.integrationTime;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.ispGain = expoParams.exposureParam.ispGain;
				/*Set the has fields so that this data is serialzied*/
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.has_wbGains = true;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.has_colourcorrection = true;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.has_colourcorrectinCoefficients = true;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.has_wb = true;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.has_isp_expo = true;
				/*False = AE and AWB enabled*/
				if(i == 0)
				{
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.ispAWB.metadataEnableAWB = counter > 100; /*AWB enabled*/
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.ispAE.metadataEnableAE = counter > 100; /*AE enabled*/
				}
				else
				{
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.ispAWB.metadataEnableAWB = true;
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.ispAE.metadataEnableAE = true;
				}

				snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.has_ispAWB = true; /*AWB enabled*/
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.has_ispAE  = true; /*AE enabled*/
			}
			snsrUpdate.protobuf_packet.state_count = 2;
			/*Send the sensor update*/
			inu_device__sensorUpdate(*device, &snsrUpdate);
		}
	}
}
/**
 * @brief Sends an example message which changes the AF VST WB gains on the cropped streams by reading 
 * 		   the AF VST downscaled stream wb gains and then sends them to the cropped stream ISPs
 * 
 * 	This test makes use of the Delayed update feature to delay sensor updates
 * 
 * Please enable DEBUG_CROP_OVERLAY when running the test
 *
 * @param device INU Device handle
 */
void ISP_Delayed_Update_Test(inu_deviceH* device)
{
	static UINT64 previous_sensorUpdateTS = 0;
	static int counter = 0;
	static inu_isp_channel__ispCmdParamU ispCmdParam,expoParams;
	counter++;
	if(previous_sensorUpdateTS == 0)
		OS_LYRG_getUsecTime(&previous_sensorUpdateTS);
	INU_Metadata_T snsrUpdate = ZEROED_OUT_METADATA;
	/*Wait until we have 100 frames*/
	if(counter < 100)
	{
		inu_nodeH streamerH = NULL;
		auto ret = inu_device__findNode(*device, "INU_ISP_CHANNEL_23", &streamerH);
		 ret = FWLIB_getWBGains(streamerH,&ispCmdParam);
		if (ERRG_FAILED(ret))
		{
			printf("Coudln't get the ISP gains \n");
			return;
		}
		ret = FWLIB_getExpoParams(streamerH,&expoParams);
		if (ERRG_FAILED(ret))
		{
			printf("Coudln't get the ISP expo params \n");
			return;
		}
		return;
	}

	else if((counter % 4) == 0)
	{
		ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
	
		/*Read the white balance from the master ISP0 (downscaled stream) */

		if (ERRG_SUCCEEDED(ret))
		{
		
			inu_isp_channel__ispCmdParamU *p = &ispCmdParam;
			/*We have now read the WB gains */
			snsrUpdate.framebuffer_offset = 0;
			snsrUpdate.frameID = 0;
			snsrUpdate.serialization_counter = 1;
			snsrUpdate.timestamp = 0;
			/*Used by the target*/
			snsrUpdate.protobuf_packet.state[0].timestamp_ns.nanoseconds = 0;
			snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
			snsrUpdate.protobuf_packet.state[0].sender = INU_metadata_HardwareType_HOST;
			snsrUpdate.protobuf_packet.state[0].target = INU_metadata_HardwareType_MASTER;
			snsrUpdate.protobuf_packet.state[0].sensor.sensorType = INU_metadata_SensorType_AF_VST;
			snsrUpdate.protobuf_packet.state[0].sensor.side = INU_metadata_Channel_Side_Left;
			snsrUpdate.protobuf_packet.state[0].sensor.channelScale = INU_metadata_Channel_Scale_Downscaled;
			snsrUpdate.protobuf_packet.state[0].has_sensor = true;
			snsrUpdate.protobuf_packet.state[0].has_sensor_metadata = true;
			snsrUpdate.protobuf_packet.state[0].has_timestamp_ns = true;
			snsrUpdate.protobuf_packet.state[0].context = INU_metadata_Context_Context_A;
			/*Send to the master and the slave*/
			snsrUpdate.protobuf_packet.state[1].timestamp_ns.nanoseconds = 0;
			snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
			snsrUpdate.protobuf_packet.state[1].sender = INU_metadata_HardwareType_HOST;
			snsrUpdate.protobuf_packet.state[1].target = INU_metadata_HardwareType_MASTER;
			snsrUpdate.protobuf_packet.state[1].sensor.sensorType = INU_metadata_SensorType_AF_VST;
			snsrUpdate.protobuf_packet.state[1].sensor.side = INU_metadata_Channel_Side_Left;
			snsrUpdate.protobuf_packet.state[1].sensor.channelScale = INU_metadata_Channel_Scale_Cropped;
			snsrUpdate.protobuf_packet.state[1].has_sensor = true;
			snsrUpdate.protobuf_packet.state[1].has_sensor_metadata = true;
			snsrUpdate.protobuf_packet.state[1].has_timestamp_ns = true;
			snsrUpdate.protobuf_packet.state[1].context = INU_metadata_Context_Context_A;
			/*This is where the colour correction coeffcients are copied from */
			for(int i=0; i < 2; i ++)
			{

				/*Copy the exposure parameters*/
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.analogGain = expoParams.exposureParam.analogGain;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.integrationTime = expoParams.exposureParam.integrationTime;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.ispGain = expoParams.exposureParam.ispGain;

				snsrUpdate.protobuf_packet.state[i].sensor_metadata.has_isp_expo = true;
				/*Open loop ISP is being used */
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.has_ispAEclosedLoop = true;

			
				/*Downscaled will have AWB and AE enabled*/
				if(i == 0)
				{
				/*False = AE and AWB enabled*/
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.ispAE.metadataEnableAE = true; /*AE enabled*/
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.has_ispAE  = true; /*AE enabled*/
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.ispAWBclosedLoop.awb_closedLoop = true;
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.ispAEclosedLoop.ae_closedLoop = true;
				}
				/*Cropped will have AWB and AE disabled*/
				if(i == 1)
				{
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.ispAE.metadataEnableAE = true; /*AE disabled*/
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.has_ispAE  = true; /*AE enabled*/
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.ispAWBclosedLoop.awb_closedLoop = false;
					snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.ispAEclosedLoop.ae_closedLoop = false;

				}
				/*Enable UART Sync*/
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.uart_sync.cmd_id = counter;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.uart_sync.wait_for_slu_SOF_Interrupt = true;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.uart_sync.slu_number = INU_metadata_SLU_SLU_1;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.uart_sync.frameID = 0;
				snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.has_uart_sync = true;
			}
			snsrUpdate.protobuf_packet.state_count = 2;
			/*Send the sensor update*/
			inu_device__sensorUpdate(*device, &snsrUpdate);
		}
	}
}
void recordMetadataToCSV(char * streamName, INU_Metadata_T * metadata)
{
	std::string fileName = std::string(streamName);
	fileName += ".csv";
	uint64_t nanoseconds_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if(metadata)
	{
		std::ofstream newFile(fileName, std::ios::app);
		newFile << metadata->frameID << ", ";
		newFile << convertRTCTimeToNS(metadata->timestamp) << ", ";
		newFile << nanoseconds_since_epoch << ", ";
		newFile << metadata->protobuf_packet.state[0].sensor_metadata.isp_expo.integrationTime * 1E6 << ", ";
		newFile << metadata->protobuf_packet.state[0].sensor_metadata.isp_expo.uart_sync.cmd_id << std::endl;
		
	}
}
void sendSensorUpdateExample(inu_deviceH* device)
{
	static UINT64 previous_sensorUpdateTS = 0;
	static int gain = 0;
	/*Toggle gain value*/
	if (gain != 8)
	{
		gain = 8;
	}
	else
	{
		gain = 0;
	}
	int cropwindow = rand() % 50;
	INU_Metadata_T snsrUpdate = ZEROED_OUT_METADATA;
	UINT64 time_ns, time_us = 0;
	OS_LYRG_getUsecTime(&time_us);
	UINT64 timeDiff = time_us - previous_sensorUpdateTS;
	if (timeDiff > 1000E3 ) /*Limit updates to 1Hz due to Synchronous message crash issue*/
	{
		time_ns = time_us * 1000; /*Convert to ns*/
		/*Unused by the target*/
		snsrUpdate.framebuffer_offset = 0;
		snsrUpdate.frameID = 0;
		snsrUpdate.serialization_counter = 1;
		snsrUpdate.timestamp = time_ns;
		/*Used by the target*/
		snsrUpdate.protobuf_packet.state[0].timestamp_ns.nanoseconds = time_ns;
		snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
		snsrUpdate.protobuf_packet.state[0].sender = INU_metadata_HardwareType_HOST;
		snsrUpdate.protobuf_packet.state[0].target = INU_metadata_HardwareType_MASTER;
		snsrUpdate.protobuf_packet.state[0].sensor.sensorType = INU_metadata_SensorType_Tracking;
		snsrUpdate.protobuf_packet.state[0].sensor.side = INU_metadata_Channel_Side_Right;
		snsrUpdate.protobuf_packet.state[0].sensor.channelScale = INU_metadata_Channel_Scale_Normal;
		snsrUpdate.protobuf_packet.state[0].has_sensor = true;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.exposure.exposure = 4;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_exposure = false;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_gain = true;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.gain.analogue_gain = gain;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.gain.digital_gain = (float)2;
		snsrUpdate.protobuf_packet.state[0].has_sensor_metadata = true;
		snsrUpdate.protobuf_packet.state[0].has_timestamp_ns = true;
		snsrUpdate.protobuf_packet.state[0].sensor_metadata.has_roi = false;
		snsrUpdate.protobuf_packet.state[0].context = INU_metadata_Context_Context_A;

		snsrUpdate.protobuf_packet.state[1].timestamp_ns.nanoseconds = time_ns;
		snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
		snsrUpdate.protobuf_packet.state[1].sender = INU_metadata_HardwareType_HOST;
		snsrUpdate.protobuf_packet.state[1].target = INU_metadata_HardwareType_MASTER;
		snsrUpdate.protobuf_packet.state[1].sensor.sensorType = INU_metadata_SensorType_Tracking;
		snsrUpdate.protobuf_packet.state[1].sensor.side = INU_metadata_Channel_Side_Left;
		snsrUpdate.protobuf_packet.state[1].sensor.channelScale = INU_metadata_Channel_Scale_Normal;
		snsrUpdate.protobuf_packet.state[1].has_sensor = true;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.exposure.exposure = 4;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.has_exposure = false;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.has_gain = true;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.gain.analogue_gain = gain;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.gain.digital_gain = (float)2;
		snsrUpdate.protobuf_packet.state[1].has_sensor_metadata = true;
		snsrUpdate.protobuf_packet.state[1].has_timestamp_ns = true;
		snsrUpdate.protobuf_packet.state[1].sensor_metadata.has_roi = false;
		snsrUpdate.protobuf_packet.state[1].context = INU_metadata_Context_Context_A;

		snsrUpdate.protobuf_packet.state[2].timestamp_ns.nanoseconds = time_ns;
		snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
		snsrUpdate.protobuf_packet.state[2].sender = INU_metadata_HardwareType_HOST;
		snsrUpdate.protobuf_packet.state[2].target = INU_metadata_HardwareType_SLAVE;
		snsrUpdate.protobuf_packet.state[2].sensor.sensorType = INU_metadata_SensorType_Tracking;
		snsrUpdate.protobuf_packet.state[2].sensor.side = INU_metadata_Channel_Side_Left;
		snsrUpdate.protobuf_packet.state[2].sensor.channelScale = INU_metadata_Channel_Scale_Cropped;
		snsrUpdate.protobuf_packet.state[2].has_sensor = true;
		snsrUpdate.protobuf_packet.state[2].sensor_metadata.exposure.exposure = 4;
		snsrUpdate.protobuf_packet.state[2].sensor_metadata.has_exposure = false;
		snsrUpdate.protobuf_packet.state[2].sensor_metadata.has_gain = true;
		snsrUpdate.protobuf_packet.state[2].sensor_metadata.gain.analogue_gain = gain;
		snsrUpdate.protobuf_packet.state[2].sensor_metadata.gain.digital_gain = (float)2;
		snsrUpdate.protobuf_packet.state[2].has_sensor_metadata = true;
		snsrUpdate.protobuf_packet.state[2].has_timestamp_ns = true;
		snsrUpdate.protobuf_packet.state[2].sensor_metadata.has_roi = false;
		snsrUpdate.protobuf_packet.state[2].context = INU_metadata_Context_Context_A;

		snsrUpdate.protobuf_packet.state[3].timestamp_ns.nanoseconds = time_ns;
		snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
		snsrUpdate.protobuf_packet.state[3].sender = INU_metadata_HardwareType_HOST;
		snsrUpdate.protobuf_packet.state[3].target = INU_metadata_HardwareType_SLAVE;
		snsrUpdate.protobuf_packet.state[3].sensor.sensorType = INU_metadata_SensorType_Tracking;
		snsrUpdate.protobuf_packet.state[3].sensor.side = INU_metadata_Channel_Side_Right;
		snsrUpdate.protobuf_packet.state[3].sensor.channelScale = INU_metadata_Channel_Scale_Normal;
		snsrUpdate.protobuf_packet.state[3].has_sensor = true;
		snsrUpdate.protobuf_packet.state[3].sensor_metadata.exposure.exposure = 4;
		snsrUpdate.protobuf_packet.state[3].sensor_metadata.has_exposure = false;
		snsrUpdate.protobuf_packet.state[3].sensor_metadata.has_gain = true;
		snsrUpdate.protobuf_packet.state[3].sensor_metadata.gain.analogue_gain = gain;
		snsrUpdate.protobuf_packet.state[3].sensor_metadata.gain.digital_gain = (float)2;
		snsrUpdate.protobuf_packet.state[3].has_sensor_metadata = true;
		snsrUpdate.protobuf_packet.state[3].has_timestamp_ns = true;
		snsrUpdate.protobuf_packet.state[3].sensor_metadata.has_roi = false;
		snsrUpdate.protobuf_packet.state[3].context = INU_metadata_Context_Context_A;
		snsrUpdate.protobuf_packet.state_count = 4;

		inu_device__sensorUpdate(*device, &snsrUpdate);
		previous_sensorUpdateTS = time_us;
	}
}
#include <thread>
void imageFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie, INU_Metadata_T *metadata,inu_deviceH *device)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	inu_image__hdr_t *imgHdrP = (inu_image__hdr_t*)hdrP;
	inu_vision_proc__parameter_list_t visionProcParams;
	ERRG_codeE status;
	inu_nodeH  currentH;

	//search for the pp function node
	currentH = inu_node__getNextInputNode(ref, NULL);
	while (currentH)
	{
		 if (inu_ref__instanceOf(currentH, INU_VISION_PROC_REF_TYPE))
		 {
			  break;
		 }
		 currentH = inu_node__getNextInputNode(currentH, NULL);
	}
	//if (currentH)
	//{
	//    visionProcParams.algType = VISION_PROC_BOKEH;
	//    visionProcParams.bokehParams.blurLevel = 0.66f;
	//    visionProcParams.bokehParams.visionProcResolution = VISION_PROC_1080P;
	//    visionProcParams.bokehParams.depthOutput = false;
	//    visionProcParams.bokehParams.faceBoxDraw = false;
	//    visionProcParams.bokehParams.faceSmoothFactor = 0.1f;
	//    visionProcParams.bokehParams.noFaceFramesToKeepBlur = 1000;
	//    visionProcParams.bokehParams.multiFaceMode = false;
	//    //send parameters to gp vision proc
	//    status = inu_vision_proc__updateParams(currentH, &visionProcParams);
	//    SANDBOX_TEST_RET_ASSERT(status);
	//}


	frameCb(ref, hdrP, dataP, size, cookie, metadata);

//   if (imgHdrP->imgDescriptor.format == NUCFG_FORMAT_DEPTH_E || imgHdrP->imgDescriptor.format == NUCFG_FORMAT_DISPARITY_E)
//   {
//      printDepth(dataP, imgHdrP);
//   }

	if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
	{
		char name[512];
#if 1
		sprintf(name, "%s%simage%d_%" PRId64 "_%" PRId64 "_%" PRId64 "_%s_refid%d.raw", RECORD_FOLDER_NAME, BACK_SLASH, imgHdrP->imgDescriptor.format, imgHdrP->dataHdr.dataIndex,imgHdrP->dataHdr.chunkNum, imgHdrP->dataHdr.timestamp, inu_ref__getUserName(ref), inu_ref__getRefId(ref));
		saveDataToDisk(dataP, IMAGE_SIZE_BYTES(imgHdrP), name);
#else
		sprintf((char*)(&name), "record\\img_frame_%" PRId64 "_%" PRId64 ".ciif", imgHdrP->dataHdr.dataIndex, imgHdrP->dataHdr.timestamp);
		saveImageAsCiifToDisk(dataP, IMAGE_SIZE_BYTES(imgHdrP), name, imgHdrP->imgDescriptor.stride, imgHdrP->imgDescriptor.height);
#endif
      //decrease number of remaining frames to record
      if (channelParamsP->recordOutputFrames)
          channelParamsP->recordOutputFrames--;
	}
	if(channelParamsP->recordMetadata)
	{
		if(metadata!=NULL)
		{
			char buffer[1024] = {0};
			char name_metadata[512] = {0} ;
			/*Name should be the same as the iamge but with .inumetadata as the extension*/
			sprintf(name_metadata, "%s%simage%d_%" PRId64 "_%" PRId64 "_%" PRId64 "_%s_refid%d.inumetadata", RECORD_FOLDER_NAME, BACK_SLASH, imgHdrP->imgDescriptor.format, imgHdrP->dataHdr.dataIndex,imgHdrP->dataHdr.chunkNum, imgHdrP->dataHdr.timestamp, inu_ref__getUserName(ref), inu_ref__getRefId(ref));
			/*Reserialize the metadata*/
			UINT32 outputSize = 0;
			if(FWLib_serializeMetadataNoDelimit(metadata,buffer,sizeof(buffer), &outputSize) == true)
			{	
				/*Calculate minimum required buffer size (without line padding that the buffer was sent with) used for metadata*/
				
				
					saveDataToDisk(buffer, outputSize, name_metadata);
				
			}
		}
	}

	if (channelParamsP->displayOutput)
	{
#if defined _OPENCV_
		char titleName[100];
		static int i = 0;
		#ifdef DEBUG_CROP_OVERLAY
		if(metadata)
		{
			if(metadata->protobuf_packet.state[0].sensor_metadata.has_roi)
			{
				if(metadata->protobuf_packet.state[0].sensor.channelScale  == INU_metadata_Channel_Scale_Cropped )
				{
					uint64_t eof = convertRTCTimeToNS(metadata->software_injection_timestamp_rtc);
					uint64_t sof = convertRTCTimeToNS(metadata->timestamp);

				}
			}
			else  if(metadata->protobuf_packet.state[0].sensor.channelScale  == INU_metadata_Channel_Scale_Downscaled )
			{
				uint64_t eof = convertRTCTimeToNS(metadata->software_injection_timestamp_rtc);
				uint64_t sof = convertRTCTimeToNS(metadata->timestamp);
			}
		}
		#endif
		memset(titleName, 0, sizeof(titleName));
		sprintf(titleName, "%s(%d)", inu_ref__getUserName(ref), inu_ref__getRefId(ref));
		char name[512];

		sprintf(name, "%s%simage%d_%" PRId64 "_%" PRId64 "_%" PRId64 "_%s_refid%d.png", RECORD_FOLDER_NAME, BACK_SLASH, imgHdrP->imgDescriptor.format, imgHdrP->dataHdr.dataIndex,imgHdrP->dataHdr.chunkNum, imgHdrP->dataHdr.timestamp, inu_ref__getUserName(ref), inu_ref__getRefId(ref));
		displayImage(channelParamsP, dataP, size, imgHdrP, titleName, channelParamsP->recordPNG, name,metadata);
#endif
	
	}
	if (channelParamsP->printMetadata)
	{
		printMetadata(metadata);
	}
	if (channelParamsP->sendSensorUpdates)
	{
		sendSensorUpdateExample(device);
	}
	if (channelParamsP->sendROIUpdate)
	{
		sendROIUpdateExample(device);
	}
	if (channelParamsP->sendSampleSensorSyncUpdate)
	{
		sendSensorSyncExample(device);
	}
	if (channelParamsP->sendSampleSwitchToTrackingSyncUpdate)
	{
		sendSensorSyncSwitchToTriggeringFromTheMaster(device);
	}
	if (channelParamsP->sendSampleSwitchToInternalTrackingSyncUpdate)
	{
		sendSensorSyncSwitchToInteralTriggering(device);
	}
	if(channelParamsP->sendSampleAFVSTCropWindowUpdate)
	{
		sendAFVSTCropWindowUpdateExample(device);
	}
	if(channelParamsP->sendSampleAFVSTFocusPosition)
	{
		sendAFVSTFocus(device);
	}
	if(channelParamsP->sendSampleAFVSTWBGain)
	{
		sendISPGains(device);
	}
	if(channelParamsP->sendSampleAFVSTExpoDelayedUpdate)
	{
		ISP_Delayed_Update_Test( device);
	}
	if(channelParamsP->recordCSVMetadata)
	{
		recordMetadataToCSV(channelParamsP->stream_out_name, metadata);
	}
	
}


void cdnnFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie, INU_Metadata_T* metadata, inu_deviceH *deviceH)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	inu_cdnn_data__hdr_t *cdnnHdrP = (inu_cdnn_data__hdr_t*)hdrP;
	frameCb(ref, hdrP, dataP, size, cookie, metadata);

	if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
	{
		char name[512];
		sprintf(name, "%s%scdnn_%" PRId64 "_%" PRId64 "_%s.raw", RECORD_FOLDER_NAME, BACK_SLASH, cdnnHdrP->dataHdr.dataIndex, cdnnHdrP->dataHdr.timestamp, inu_ref__getUserName(ref));
		saveDataToDisk(dataP, size, name);
      //decrease number of remaining frames to record
      if (channelParamsP->recordOutputFrames)
          channelParamsP->recordOutputFrames--;
	}

	if (channelParamsP->displayOutput)
	{
		printCdnnResults(dataP, cdnnHdrP);
	}
}

void temperatureFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie, INU_Metadata_T* metadata, inu_deviceH *device)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	inu_temperature_data__hdr_t *tempHdrP = (inu_temperature_data__hdr_t*)hdrP;
	char outputBuffer[LOC_MAXLEN];
	frameCb(ref, hdrP, dataP, size, cookie, metadata);

	sprintf(outputBuffer, "chipProcess: (%f , %f, %f), chipTemp: %f, chipVoltage: %f, imuTemp: %f, sensorTempCvl: %d, sensorTempCvr: %d\n",
		tempHdrP->chipProcess[0], tempHdrP->chipProcess[1], tempHdrP->chipProcess[2], tempHdrP->chipTemp, tempHdrP->chipVoltage, tempHdrP->imuTemp, tempHdrP->sensorTempCvl, tempHdrP->sensorTempCvr);

	if (channelParamsP->displayOutput)
	{
		printf(outputBuffer);
	}

	if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
	{
		saveBufferAsTextToDisk(ref, outputBuffer, hdrP);
      //decrease number of remaining frames to record
      if (channelParamsP->recordOutputFrames)
          channelParamsP->recordOutputFrames--;
	}
}

UINT64 ts; // for debug
#define MEASURE_IMU_JITTER
#ifdef MEASURE_IMU_JITTER
UINT64 previous_ts = 0;
#define AVERAGE_PERIOD_JITTER 1000
void imuMeasureIMUJitter(UINT64 timestamp_ns, UINT64 metadata_timestamp_ns)
{
	static UINT64 previous_ts = 0, previous_metadata_timestamp_ns=0;
	static UINT64 counter = 0;
	static UINT64 max_ts_d = 0;
	static UINT64 min_ts_d = 1000000000000000;
	static INT64 accumulated_timestamp_delta_ns =0, accumulated_metadata_timestamp_delta_ns =0;
	static double averaged_timestamp_delta = 0;

	INT64 timestamp_delta_ns = (INT64) timestamp_ns - previous_ts;
	INT64 timestamp_delta_ns_metadata = (INT64) metadata_timestamp_ns - previous_metadata_timestamp_ns;

	previous_ts = timestamp_ns;
	previous_metadata_timestamp_ns = metadata_timestamp_ns;

	if(timestamp_delta_ns > max_ts_d )
	{
		max_ts_d = timestamp_delta_ns;
	}
	if(timestamp_delta_ns < min_ts_d)
	{
		min_ts_d = timestamp_delta_ns;
	}
	
	if(counter < AVERAGE_PERIOD_JITTER)
	{
		accumulated_timestamp_delta_ns +=  timestamp_delta_ns;
		accumulated_metadata_timestamp_delta_ns += timestamp_delta_ns_metadata;
		counter ++;
	}
	else
	{
		averaged_timestamp_delta = accumulated_timestamp_delta_ns/AVERAGE_PERIOD_JITTER;
		double metadata_averaged_timestamp_delta = accumulated_metadata_timestamp_delta_ns/AVERAGE_PERIOD_JITTER;
		printf("IMU Timestamp Stats, Average timestamp difference ,%.0fns, Average Metadata RTC timestamp difference , %.0fns Max timestamp difference ,%lu ns,Min Timestamp difference, %lu ns,Max - Min,%lu ns \n" ,averaged_timestamp_delta,metadata_averaged_timestamp_delta,max_ts_d,min_ts_d,max_ts_d-min_ts_d  );
	 	max_ts_d = 0;
		min_ts_d = 1000000000000000;
		counter = 0;
		accumulated_timestamp_delta_ns = 0;
		accumulated_metadata_timestamp_delta_ns =0;
	}
}
#endif
void imuFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie, INU_Metadata_T* metadata, inu_deviceH *device)
{
	inu_imu_data__readableData *readableDataP = (inu_imu_data__readableData*)dataP;
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	char outputBuffer[LOC_MAXLEN];
	int i;
	frameCb(ref, hdrP, dataP, size, cookie, metadata);

	for (i = 0; i < INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E - 2; i++)
	{
		if (readableDataP->imuData[i].name){
			if ( i==INU_IMU_DATA__SENSOR_ACCELAROMETER_X_E || i == INU_IMU_DATA__SENSOR_GYROSCOPE_X_E){
				ts = readableDataP->timeStamp;
				#ifdef MEASURE_IMU_JITTER
				if(i==INU_IMU_DATA__SENSOR_ACCELAROMETER_X_E && strcmp(readableDataP->imuData[i].name,"in_accel_x") == 0)
				{
					UINT64 metadata_ts = convertRTCTimeToNS(metadata->timestamp);
					imuMeasureIMUJitter(ts, metadata_ts);
				}
				#endif
				sprintf(outputBuffer, "%" PRId64 ": ", readableDataP->timeStamp);
			} else if (i==INU_IMU_DATA__SENSOR_MAGNETOMETER_X_E){
				sprintf(outputBuffer, "%" PRId64 ": ", ts);
			}
			snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "%s = %f ", readableDataP->imuData[i].name, readableDataP->imuData[i].imuVal);
		}
	}
	snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "\n");

	if (channelParamsP->displayOutput)
	{
		printf(outputBuffer);
	}
	if (channelParamsP->printMetadata)
	{
		printMetadata(metadata);
	}

	if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
	{
		saveBufferAsTextToDisk(ref, outputBuffer, hdrP);
      //decrease number of remaining frames to record
      if (channelParamsP->recordOutputFrames)
          channelParamsP->recordOutputFrames--;
	}
}


void cvaFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie, INU_Metadata_T* metadata, inu_deviceH *device)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	inu_cva_data__hdr_t *cvaHdrP = (inu_cva_data__hdr_t*)hdrP;
	inu_cva_data__freak_descriptor_t *frkDescP = (inu_cva_data__freak_descriptor_t*)dataP;
	UINT32 xs, ys;
	frameCb(ref, hdrP, dataP, size, cookie, metadata);

	if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
	{
		char wFileName[300];
		char wString[187];
		char tmp2;
		char sideString[5];

		if ((channelParamsP->printMetadata) && (metadata->protobuf_packet.sensors_count > 0))
		{
			// only look at the first sensor (there will probably only be one anyway)
			switch (metadata->protobuf_packet.sensors[0].side)
			{
				case INU_metadata_Channel_Side_Left:
					strcpy(sideString, "_L");
					break;
				case INU_metadata_Channel_Side_Right:
					strcpy(sideString, "_R");
					break;
				default:
					strcpy(sideString, "_U");
			}
		}
		else
		{
			strcpy(sideString, "");
		}

		sprintf((char*)(&wFileName), "%s%sfreak_kp_frame%s_%06" PRId64 "_%" PRId64 ".txt", RECORD_FOLDER_NAME, BACK_SLASH, sideString, cvaHdrP->dataHdr.dataIndex, cvaHdrP->dataHdr.timestamp);
		FILE* frkOutFile = fopen(wFileName, "wb");

		for (UINT32 i = 0; i < cvaHdrP->cvaDescriptor.kpNum; i++)
		{
			xs = ((frkDescP + i)->xs) >> 14;
			ys = ((frkDescP + i)->ys) >> 14;
			memset(wString, 0, sizeof(wString));
			tmp2 = sprintf((char*)(&wString), "(%4d,%4d,%4d) %01x_%02x_%08f_%01x_%07x_%07x_%04x_%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x", 0, 0, i,
				(frkDescP + i)->imgIsOdd, (frkDescP + i)->jobId, (frkDescP + i)->angle, (frkDescP + i)->notImg1, (frkDescP + i)->xs, (frkDescP + i)->ys, (frkDescP + i)->patternSize,
				(frkDescP + i)->kp_descriptor[15], (frkDescP + i)->kp_descriptor[14], (frkDescP + i)->kp_descriptor[13], (frkDescP + i)->kp_descriptor[12],
				(frkDescP + i)->kp_descriptor[11], (frkDescP + i)->kp_descriptor[10], (frkDescP + i)->kp_descriptor[9], (frkDescP + i)->kp_descriptor[8],
				(frkDescP + i)->kp_descriptor[7], (frkDescP + i)->kp_descriptor[6], (frkDescP + i)->kp_descriptor[5], (frkDescP + i)->kp_descriptor[4],
				(frkDescP + i)->kp_descriptor[3], (frkDescP + i)->kp_descriptor[2], (frkDescP + i)->kp_descriptor[1], (frkDescP + i)->kp_descriptor[0]);
			strcat(wString, "\n");
			tmp2 += 2;
			fputs(wString, frkOutFile);
		}
		fflush(frkOutFile);
		fclose(frkOutFile);
      //decrease number of remaining frames to record
      if (channelParamsP->recordOutputFrames)
          channelParamsP->recordOutputFrames--;
	}

	//sanity testing
	for (UINT32 i = 0; i < cvaHdrP->cvaDescriptor.kpNum; i++)
	{
		xs = ((frkDescP + i)->xs) >> 14;
		ys = ((frkDescP + i)->ys) >> 14;
		if ((xs > 1280) || (ys > 800))
		{
			printf("Found KP out of boundaries on index %d kp at X-%d/Y-%d (ts %" PRId64 ", index %" PRId64 ")\n", i, xs, ys, cvaHdrP->dataHdr.timestamp, cvaHdrP->dataHdr.dataIndex);
			break;
		}
	}
	if (channelParamsP->printMetadata)
	{
		printMetadata(metadata);
	}
	#if defined _OPENCV_
	/*VARJO SPECIFIC CHANGE */
	if (metadata != NULL)
	{
		if (channelParamsP->displayOutput)
		{
			cv::Mat mat = cv::Mat::zeros(cv::Size(800, 800), CV_8UC1);
			for (UINT32 i = 0; i < cvaHdrP->cvaDescriptor.kpNum - 1; i++)
			{
				xs = ((frkDescP + i)->xs) >> 14;
				ys = ((frkDescP + i)->ys) >> 14;
				mat.at<uchar>(xs, ys) = 255;
			}

			if (metadata->protobuf_packet.sensors[0].side == INU_metadata_Channel_Side_Left)
			{
				cv::imshow("left", mat);
			}
			else
			{
				cv::imshow("right", mat);
			}
			cv::waitKey(1);
		}
	}
	#endif
}



void matcherFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie)
{
	MATCHER_DEMO_gpHostMsgStructT *tmpP = (MATCHER_DEMO_gpHostMsgStructT*)dataP;
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	frameCb(ref, hdrP, dataP, size, cookie, NULL);

	if (channelParamsP->displayOutput)
	{
		printf("ts %" PRId64 ": counter %" PRId64 ", cycle %d\n", hdrP->timestamp, tmpP->counter, tmpP->cycles);
	}
}


void gaussianFromFileFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	frameCb(ref, hdrP, dataP, size, cookie,NULL);

	if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
	{
		FILE *fp_output;
		fp_output = fopen("gaussian_image_out.raw", "wb");
		if (fp_output == NULL)
		{
			assert(0);
		}
		fwrite(dataP, 1, size, fp_output);
		fflush(fp_output);
		fclose(fp_output);
      //decrease number of remaining frames to record
      if (channelParamsP->recordOutputFrames)
          channelParamsP->recordOutputFrames--;
	}
}

void iicFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie)
{
	inu_image__hdr_t* iicDataHdr = (inu_image__hdr_t*)hdrP;
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	frameCb(ref, hdrP, dataP, size, cookie,NULL);

	if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
	{
		char wFileName[40];
#if 1
		UINT8  wString1[65] = { 0 };
#else
		UINT32  wString[13] = { 0 };
#endif
		sprintf((char*)(&wFileName), "%s%siic_frame_%" PRId64 ".txt", RECORD_FOLDER_NAME, BACK_SLASH, iicDataHdr->dataHdr.dataIndex);
		FILE* iicOutFile = fopen(wFileName, "wb");

		//printf("\t\tIIC arrived. buffsize 0x%08x, frameNum %d \n", iicDataHdr->imgDescriptor.bufferHeight, iicDataHdr->dataHdr.dataIndex);
		for (UINT32 i = 0; i < iicDataHdr->imgDescriptor.bufferHeight / 16; i++)
		{
#if 1
			sprintf((char*)(&wString1), "%08x%08x%08x%08x", (*(dataP + 3)), (*(dataP + 2)), (*(dataP + 1)), (*(dataP + 0)));
			fwrite(wString1, 1, sizeof(wString1), iicOutFile);
			fwrite("\n", sizeof(char), 1, iicOutFile);
#else
			sprintf((char*)(&wString), "(%4d,%4d,%6d) %032x ", 0, 0, ((i * 4) + 0), (*(dataP + 0)));
			fwrite(wString, 1, sizeof(wString), iicOutFile);
			fwrite("\n", sizeof(char), 1, iicOutFile);
			sprintf((char*)(&wString), "(%4d,%4d,%6d) %032x ", 0, 0, ((i * 4) + 1), (*(dataP + 1)));
			fwrite(wString, 1, sizeof(wString), iicOutFile);
			fwrite("\n", sizeof(char), 1, iicOutFile);
			sprintf((char*)(&wString), "(%4d,%4d,%6d) %032x ", 0, 0, ((i * 4) + 2), (*(dataP + 2)));
			fwrite(wString, 1, sizeof(wString), iicOutFile);
			fwrite("\n", sizeof(char), 1, iicOutFile);
			sprintf((char*)(&wString), "(%4d,%4d,%6d) %032x ", 0, 0, ((i * 4) + 3), (*(dataP + 3)));
			fwrite(wString, 1, sizeof(wString), iicOutFile);
			fwrite("\n", sizeof(char), 1, iicOutFile);
#endif
			dataP += 4;
		}

		fflush(iicOutFile);
		fclose(iicOutFile);
      //decrease number of remaining frames to record
      if (channelParamsP->recordOutputFrames)
          channelParamsP->recordOutputFrames--;
	}
}


void fdkBasicDemoStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie)
{
	ERRG_codeE status;
	inu_nodeH dataH = NULL, channelH = NULL;

	startCb(ref, hdrP, cookie);
	hostGPmsg.algInfo[CORE_GP].enable = 1;
	hostGPmsg.algInfo[CORE_GP].startX = 50;
	hostGPmsg.algInfo[CORE_GP].startY = 50;
	hostGPmsg.algInfo[CORE_GP].picWidth = 128;
	hostGPmsg.algInfo[CORE_GP].picHeight = 128;

	hostGPmsg.algInfo[CORE_XM4].enable = 1;
	hostGPmsg.algInfo[CORE_XM4].startX = 200;
	hostGPmsg.algInfo[CORE_XM4].startY = 200;
	hostGPmsg.algInfo[CORE_XM4].picWidth = 100;
	hostGPmsg.algInfo[CORE_XM4].picHeight = 100;

	hostGPmsg.algInfo[CORE_EV62].enable = 1;
	hostGPmsg.algInfo[CORE_EV62].startX = 400;
	hostGPmsg.algInfo[CORE_EV62].startY = 400;
	hostGPmsg.algInfo[CORE_EV62].picWidth = 128;
	hostGPmsg.algInfo[CORE_EV62].picHeight = 100;

	//get data
	dataH = (inu_imageH)inu_node__getNextInputNode(ref, NULL);
	if (dataH)
	{
		//get channel
		channelH = (inu_imageH)inu_node__getNextInputNode(dataH, NULL);
	}

	status = inu_fdk__send_data_async(channelH, (char*)&hostGPmsg, sizeof(hostGPmsg));
	if (ERRG_FAILED(status)) assert(0);
}

void fdkBasicDemoFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	BASIC_DEMO_gpHostMsgStructT *msgP = (BASIC_DEMO_gpHostMsgStructT*)dataP;
	char outputBuffer[LOC_MAXLEN];

	frameCb(ref, hdrP, dataP, size, cookie,NULL);

	//printf("raw data: ctr = %d, ts = %" PRId64 ", size = %d\n", hdrP->dataIndex, hdrP->timestamp, inu_data__getMemSize(data));
	//printf("FDK raw data: gp ctr = %d, gp usec %d XM4 num of cycles = %d\n", dataP->counter, dataP->gpUsec, dataP->xm4Cycles);
	sprintf(outputBuffer, "FDK raw data: ");
	if (hostGPmsg.algInfo[CORE_XM4].enable)
	{
		snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "XM4 num of cycles = %d ", msgP->xm4Cycles);
	}
	if (hostGPmsg.algInfo[CORE_EV62].enable)
	{
		snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "EV62 num of cycles = %d ", msgP->ev62Cycles);
	}
	if (hostGPmsg.algInfo[CORE_GP].enable)
	{
		snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "GP counter = %d GP process time (usec) = %d ", msgP->counter, msgP->gpUsec);
	}

	if (channelParamsP->displayOutput)
	{
		printf("%s\n", outputBuffer);
	}

	if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
	{
		saveBufferAsTextToDisk(ref, outputBuffer, hdrP);
      //decrease number of remaining frames to record
      if (channelParamsP->recordOutputFrames)
          channelParamsP->recordOutputFrames--;
	}
}

static void SENSORS_CTRLP_getImgMedian(char *outputBuffer, int side, UINT32 picSize, UINT32 *histP)
{
	INT16  i;
	UINT32  sum;
	UINT32  median;

	/* calc statistics */
	median = (picSize >> 1);

	/* calc median value */
	sum = 0; i = 0;
	while ((sum < median) && i < 256)
	{
		sum += histP[i++];
	}

	median = i - 1;

	snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "Median: side = %d, median = %d, picSize = %d\n", side, median, picSize);
}

/****************************************************************************
*
*  Function Name: SENSORS_CTRLP_getImgGl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Service manager
*
****************************************************************************/
static void SENSORS_CTRLP_getImgGl(char *outputBuffer, int side, UINT32 picSize, UINT32 *histP)
{
	INT16  i;
	UINT32  sum;
	UINT32  nSatMax = 2;
	UINT32 histEdge;

	/*** Calculate the gray level on the current Histogram of nSatMax precent ***/
	sum = 0; i = (256 - 1);

	/* Running from end until precent is nSatMax */
	while (((sum * 100)  < (nSatMax * picSize)) && (i >= 0))
	{
		sum += histP[i--];
	}

	histEdge = i + 1;

	snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "Img Gl: side = %d, nSatMax = %d, histEdge = %d,picSize = %d\n", side, nSatMax, histEdge, picSize);
}

/****************************************************************************
*
*  Function Name: SENSORS_CTRLP_getImgSaturation
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Service manager
*
****************************************************************************/
static void SENSORS_CTRLP_getImgSaturation(char *outputBuffer, int side, UINT32 picSize, UINT32 *histP)
{
	INT16  i;
	UINT32 sum;
	UINT32 sat;

	/*** Calculate the precent from the current Histogram from gray level glSat ***/
	sum = 0; i = (980 / 4);

	/* Running from glSat to end, and save the precentage */
	while (i < 256)
	{
		sum += histP[i++];
	}

	sat = (sum * 100) / picSize;

	snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "Saturation: side = %d, satPrecent = %d, picSize = %d\n", side, sat, picSize);
}

static void print_hist(char *outputBuffer, UINT8 *dataP)
{
	int i, roi;
	UINT32 *temp_32 = (UINT32*)dataP;
	UINT32 bins[256];
	UINT32 accumulator, testAccumulator;

	for (roi = 0; roi < 3; roi++)
	{
		snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "roi %d:\n", roi);
		testAccumulator = 0;

		for (i = 0; i < (0x100); i++)
		{
			bins[i] = temp_32[i + (roi*(0xc00 / 4))];
			snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "%d ", bins[i]);
		}

		for (i = 0; i < (0x100); i++)
		{
			testAccumulator += i * bins[i];
		}

		accumulator = temp_32[(0x2400 / 4) + (roi*(0xc / 4))];
		snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "\ntestAccumulator = %d accumulator %d\n", testAccumulator, accumulator);
	}
	snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "\n\n\n");
}

void histogramFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie, INU_Metadata_T* metadata, inu_deviceH *device)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	inu_histogram_data__hdr_t *histHdrP = (inu_histogram_data__hdr_t*)hdrP;
	char outputBuffer[LOC_MAXLEN];
	UINT32 picSize = (histHdrP->roi[0].x1 - histHdrP->roi[0].x0 + 1) * (histHdrP->roi[0].y1 - histHdrP->roi[0].y0 + 1);

	frameCb(ref, hdrP, dataP, size, cookie,NULL);

	sprintf(outputBuffer, "index %" PRId64 ", from: %s, (nw %d,%d) (se %d,%d)\n", histHdrP->dataHdr.dataIndex, inu_ref__getUserName(ref),
		histHdrP->roi[0].x0, histHdrP->roi[0].y0, histHdrP->roi[0].x1, histHdrP->roi[0].y1);

	SENSORS_CTRLP_getImgMedian(outputBuffer, 0, picSize, (UINT32*)dataP);
	SENSORS_CTRLP_getImgGl(outputBuffer, 0, picSize, (UINT32*)dataP);
	SENSORS_CTRLP_getImgSaturation(outputBuffer, 0, picSize, (UINT32*)dataP);
	print_hist(outputBuffer, (UINT8*)dataP);

	if (channelParamsP->displayOutput)
	{
		printf(outputBuffer);
	}

	if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
	{
		saveBufferAsTextToDisk(ref, outputBuffer, hdrP);
      //decrease number of remaining frames to record
      if (channelParamsP->recordOutputFrames)
          channelParamsP->recordOutputFrames--;
	}
}

void dppStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie)
{
	//NOAM fix this
	startCb(ref,hdrP,cookie);
#if 0
	ERRG_codeE status;
	inu_dpe_ppH dpeH;
	inu_dpe_pp_hostGpMsgStructT hostGPmsg;

	status = inu_graph__findNode(graphH, "dpe_pp_0", &dpeH);
	SANDBOX_TEST_RET_ASSERT(status);

	hostGPmsg.ConfidenceThresholdRegion1 = 0;
	hostGPmsg.BlobDispDiffSameBlob = 16;
	hostGPmsg.BlobMaxHeight = 16;
	hostGPmsg.BlobMaxSize = 500;

	status = inu_dpe_pp__send_data_async(dpeH, (char*)&hostGPmsg, sizeof(hostGPmsg));
	SANDBOX_TEST_RET_ASSERT(status);
#endif
}

void cdnnStartCb(inu_refH ref, inu_data__hdr_t* hdrP, void* cookie)
{
	 inu_vision_proc__parameter_list_t visionProcParams;
	 ERRG_codeE status;
	 inu_nodeH  currentH;

	 //search for the pp function node
	 currentH = inu_node__getNextInputNode(ref, NULL);
	 while (currentH)
	 {
		  if (inu_ref__instanceOf(currentH, INU_VISION_PROC_REF_TYPE))
		  {
				break;
		  }
		  currentH = inu_node__getNextInputNode(currentH, NULL);
	 }
	 //if (currentH)
	 //{
	 //    visionProcParams.algType = VISION_PROC_BACKGROUND_REMOVAL;
	 //    visionProcParams.bokehParams.blurLevel = 0.33f;
	 //    visionProcParams.bokehParams.visionProcResolution = VISION_PROC_1080P;
	 //    visionProcParams.bokehParams.depthOutput = false;
	 //    //send parameters to gp vision proc
	 //    status = inu_vision_proc__updateParams(currentH, &visionProcParams);
	 //    SANDBOX_TEST_RET_ASSERT(status);
	 //}
	 startCb(ref, hdrP, cookie);
}

void plyStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie)
{
    inu_pp__parameter_list_t pointCloudParams;
    inu_sensors_groupH sensorGroupsH;
    ERRG_codeE status;
    inu_nodeH  currentH;

    //search for the pp function node
    currentH = inu_node__getNextInputNode(ref, NULL);
    while (currentH)
    {
        if (inu_ref__instanceOf(currentH, INU_PP_REF_TYPE))
        {
            break;
        }
        currentH = inu_node__getNextInputNode(currentH, NULL);
    }
    if (!currentH)
        assert(0); //error building the graph

    //find sensor group
    status = inu_graph__getOrigSensGroup(currentH, &sensorGroupsH);
    SANDBOX_TEST_RET_ASSERT(status);
    pointCloudParams.sensorGroup = inu_sensors_group__getId(sensorGroupsH);
    printf("sensorGroup: %d\n", pointCloudParams.sensorGroup);

    //find optical parameters from yml
    CALIBG_ymlGetSectionFcValue(CALIB_LUT_MODE_FULL_E, CALIB_YML_SECTION_SENSOR_0_E, &(pointCloudParams.fx), &(pointCloudParams.fy));
    CALIBG_ymlGetSectionCcValue(CALIB_LUT_MODE_FULL_E, CALIB_YML_SECTION_SENSOR_0_E, &(pointCloudParams.cxL), &(pointCloudParams.cy));
    CALIBG_ymlGetSectionCcValue(CALIB_LUT_MODE_FULL_E, CALIB_YML_SECTION_SENSOR_1_E, &(pointCloudParams.cxR), &(pointCloudParams.cy));
    CALIBG_ymlGetSectionsBLValue(CALIB_LUT_MODE_FULL_E, 0, 1, &(pointCloudParams.baseline));
    pointCloudParams.maxDepthMm = 10000;
    pointCloudParams.voxel_leaf_x_size = 20;
    pointCloudParams.voxel_leaf_y_size = 20;
    pointCloudParams.voxel_leaf_z_size = 20;
    pointCloudParams.vgf_flag = 1;
    pointCloudParams.flip_x = 0;
    pointCloudParams.flip_y = 1;
    pointCloudParams.flip_z = 0;
    pointCloudParams.model = INU_PP_EQUIDISTANCE;
    Calib_YmlSectionParamsT *sectionStart;
    CALIBG_ymlGetSectionsDB(CALIB_LUT_MODE_FULL_E, CALIB_YML_SECTION_SENSOR_0_E, &sectionStart);
    int model = (int)((float *)sectionStart->sensorParams.calibYmlParams[CALIB_YML_VIRTUAL_E][CALIB_YML_MODEL_E].paramList)[0];
    pointCloudParams.model = (INU_PP__modelE)model;

    //send parameters to gp
    status = inu_pp__updateParams(currentH, &pointCloudParams);
    SANDBOX_TEST_RET_ASSERT(status);

    startCb(ref, hdrP, cookie);
}

void plyFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie, INU_Metadata_T* metadata, inu_deviceH *deviceH)
{
    SANDBOX_channelParamsT* channelParamsP = (SANDBOX_channelParamsT*)cookie;
    inu_point_cloud_data__hdr_t* plyHdrP = (inu_point_cloud_data__hdr_t*)hdrP;

    //TO DO add PLY parsing
	frameCb(ref, hdrP, dataP, size, cookie,NULL);
    printf("ply size %d\n", plyHdrP->pointCloudListSize);

    if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
    {
        char name[512];
        sprintf(name, "%s%sply_%" PRId64 "_%s.raw", RECORD_FOLDER_NAME, BACK_SLASH, hdrP->dataIndex, inu_ref__getUserName(ref));
        saveDataToDisk(dataP, plyHdrP->pointCloudListSize, name);
        //decrease number of remaining frames to record
        if (channelParamsP->recordOutputFrames)
            channelParamsP->recordOutputFrames--;
    }
}

#define SLAM_Q_DEPTH       14
#define SLAM_Q_KEYPOINT    14
#define SLAM_Q_SCALE_VAL   8
#define SLAM_Q_RHO         6

void slamStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie)
{
	 ERRG_codeE status;
	 inu_nodeH  currentH;
	 inu_slam__parameter_list_t slamCalibParams;

	 //search for the slam function node
	 currentH = inu_node__getNextInputNode(ref, NULL);
	 while (currentH)
	 {
		  if (inu_ref__instanceOf(currentH, INU_SLAM_REF_TYPE))
		  {
				break;
		  }
		  currentH = inu_node__getNextInputNode(currentH, NULL);
	 }
	 if (!currentH)
		  assert(0); //error building the graph


	 CALIBG_ymlGetSectionFcValue(CALIB_LUT_MODE_FULL_E, CALIB_YML_SECTION_SENSOR_4_E, &(slamCalibParams.fx), &(slamCalibParams.fy));
	 CALIBG_ymlGetSectionCcValue(CALIB_LUT_MODE_FULL_E, CALIB_YML_SECTION_SENSOR_4_E, &(slamCalibParams.cx), &(slamCalibParams.cy));
	 CALIBG_ymlGetSectionsBLValue(CALIB_LUT_MODE_FULL_E, 3, 4, &(slamCalibParams.baseline));

	 //IMU noise
	 slamCalibParams.NoiseGyro = 1.7e-04;
	 slamCalibParams.NoiseAcc = 2.0e-03;
	 slamCalibParams.GyroWalk = 1.9393e-05;
	 slamCalibParams.AccWalk = 3.e-03;
	 slamCalibParams.IMU_freq = 200;

	 //config
	 float rho_threshold_mono   = 2.447651936;  // = sqrt(5.991);
	 float rho_threshold_stereo = 2.7955321497; // = sqrt(7.815);

	 slamCalibParams.TrackingSt.rhoThresholdMono = (int)(rho_threshold_mono * (float)(1 << SLAM_Q_RHO));
	 slamCalibParams.TrackingSt.rhoThresholdStereo = (int)(rho_threshold_stereo * (float)(1 << SLAM_Q_RHO));
	 slamCalibParams.TrackingSt.MaximumDepthThreshold = (int)(35 * (float)(1 << SLAM_Q_DEPTH));
	 slamCalibParams.TrackingSt.minimumDepthThreshold = (int)((slamCalibParams.baseline / 1000.0) * (float)(1 << SLAM_Q_DEPTH));
	 slamCalibParams.TrackingSt.ClosePointsDepthThreshold = 50;

    slamCalibParams.TrackingSt.StereoMatcherSt.secondBestTh_100 = 80;   // ratio in percentage
    slamCalibParams.TrackingSt.StereoMatcherSt.radiusThreshold = (int)(2.0f * (float)(1 << SLAM_Q_KEYPOINT));
    slamCalibParams.TrackingSt.StereoMatcherSt.HammingDistanceThreshold = 150;
    slamCalibParams.TrackingSt.StereoMatcherSt.scaleDifferenceThreshold = (int)(0.5f * (float)(1 << SLAM_Q_SCALE_VAL));

    slamCalibParams.TrackingSt.FrameMatcherSt.ratioThreshold_100 = 70;  // ratio in percentage
    slamCalibParams.TrackingSt.FrameMatcherSt.radiusThreshold = 15;
    slamCalibParams.TrackingSt.FrameMatcherSt.HammingDistanceThreshold = 150;
    slamCalibParams.TrackingSt.FrameMatcherSt.scaleDifferenceThreshold = (int)(0.5f * (float)(1 << SLAM_Q_SCALE_VAL));

	 slamCalibParams.TrackingSt.KeyframesSt.UseKeyframes = 1;
	 slamCalibParams.TrackingSt.KeyframesSt.numberOfKeyframesForMatching = 10;
	 slamCalibParams.TrackingSt.KeyframesSt.connectedKeyframesThreshold = 15;
	 slamCalibParams.TrackingSt.KeyframesSt.numTrackedClosePointsThreshold = 100;
	 slamCalibParams.TrackingSt.KeyframesSt.numNonTrackedClosePointsThreshold = 70;

    slamCalibParams.TrackingSt.KeyframeMatcherSt.ratioThreshold_100 = (int)(0.7 * (float)100);  // ratio in percentage
    slamCalibParams.TrackingSt.KeyframeMatcherSt.radiusThreshold = 3;
    slamCalibParams.TrackingSt.KeyframeMatcherSt.HammingDistanceThreshold = 150;
    slamCalibParams.TrackingSt.KeyframeMatcherSt.scaleDifferenceThreshold = (int)(0.5 * (float)(1 << SLAM_Q_SCALE_VAL));

	 slamCalibParams.TrackingSt.MapPointCullingSt.mapPointCullRatioThr = 0.25;
	 slamCalibParams.TrackingSt.MapPointCullingSt.observingKeyframesThr = 3;
	 slamCalibParams.TrackingSt.MapPointCullingSt.keyframeIndexDiffThr = 2;
	 slamCalibParams.TrackingSt.MapPointCullingSt.keyframeIndexDiffThr2 = 3;

    slamCalibParams.TrackingSt.KeyframeCullingSt.observationThreshold = 3;
    slamCalibParams.TrackingSt.KeyframeCullingSt.redundantMapPointThreshold_100 = (int)(0.9f * (float)100); // ratio in percentage

	 slamCalibParams.LocalBundleAdjustmentSt.RunLocalBundleAdjustment = 0;
	 slamCalibParams.LocalBundleAdjustmentSt.numberOfKeyframesForLBA = 10;

	 slamCalibParams.LocalizationSt.RunRelocalization = 0;

	 slamCalibParams.fps = 25;


   //send parameters to gp
   status = inu_slam__updateParams(currentH, &slamCalibParams);
   SANDBOX_TEST_RET_ASSERT(status);

	 startCb(ref, hdrP, cookie);
}

void slamFromFileStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie)
{
#if 0
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	inu_function__startParamsT startParam;
	ERRG_codeE status;
	inu_streamerH streamerInSlamH;
#endif
	startCb(ref, hdrP, cookie);
#if 0
	CALIBG_ymlGetSectionFcValue(CALIB_LUT_MODE_FULL_E, CALIB_YML_SECTION_SENSOR_4_E, &(slamCalibParams.fx), &(slamCalibParams.fy));
	CALIBG_ymlGetSectionCcValue(CALIB_LUT_MODE_FULL_E, CALIB_YML_SECTION_SENSOR_4_E, &(slamCalibParams.cx), &(slamCalibParams.cy));
	CALIBG_ymlGetSectionsBLValue(CALIB_LUT_MODE_FULL_E, 3, 4, &(slamCalibParams.baseline));

	status = inu_graph__findNode(graphH, "Stream_In_SLAM", &streamerInSlamH);
	SANDBOX_TEST_RET_ASSERT(status);

	inu_cva_dataH cvaDataH;
	status = inu_graph__findNode(graphH, "CVA_in_1", &cvaDataH);
	SANDBOX_TEST_RET_ASSERT(status);

	inu_slamH slamH;
	status = inu_graph__findNode(graphH, "slam_1", &slamH);
	SANDBOX_TEST_RET_ASSERT(status);

	inu_slam_dataH slamDataH;
	status = inu_graph__findNode(graphH, "slam_data_1", &slamDataH);
	SANDBOX_TEST_RET_ASSERT(status);

	status = inu_function__start(streamerInSlamH, &startParam);
	SANDBOX_TEST_RET_ASSERT(status);

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
#endif
}
#ifdef DUMP_SLAM_RESULTS
static void slamAddIntValueToBuffer(char *outputBuffer, int numKp, int *ptr)
{
	for (int i = 0; i < numKp; i++)
	{
		snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "%d ", *ptr);
		ptr++;
	}
}
#endif
static void slamAddFloatValueToBuffer(char *outputBuffer, int numKp, float *ptr)
{
	for (int i = 0; i < numKp; i++)
	{
		snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "%f ", *ptr);
		ptr++;
	}
}

void InuPrinVec(char* pMessage, float* mat, int len, FILE* fP)
{
	 fprintf(fP, "%s\n", pMessage);
	 for (int r = 0; r < len; r++)
	 {
		  fprintf(fP, "%*.16f ", 10, mat[r]);
		  fprintf(fP, "\n");
	 }
	 fprintf(fP, "\n");
}

void InuPrintMat(char* pMessage, float* mat, int mat_rows, int mat_cols, FILE* fP)
{
	 fprintf(fP, "%s\n", pMessage);
	 for (int r = 0; r < mat_rows; r++)
	 {
		  for (int c = 0; c < mat_cols; c++)
		  {
				fprintf(fP, "%*.16f ", 10, mat[r * mat_rows + c]);
		  }
		  fprintf(fP, "\n");
	 }
	 fprintf(fP, "\n");
}

void slamFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie, INU_Metadata_T* metadata, inu_deviceH *deviceH)
{
	SANDBOX_channelParamsT *channelParamsP = (SANDBOX_channelParamsT*)cookie;
	float *floatP;
	UINT64 virtAddressX = (UINT64)dataP;
#ifdef DUMP_SLAM_RESULTS
	int numKp, idx, offsef = 0, i = 0;
#endif
	char outputBuffer[LOC_MAXLEN] = { 0 };
	frameCb(ref, hdrP, dataP, size, cookie,metadata);

	floatP = (float *)virtAddressX;

#ifdef DUMP_PREINTEGRATION_RESULTS
	FILE* fp_output = fopen("C:\\Work\\PreIntegration\\MH01_out.txt", "a");
	int i, k;
	float Inu_dT;
	float Inu_dR[9];
	float Inu_dV[3];
	float Inu_dP[3];
	float Inu_JRg[9];
	float Inu_JVg[9];
	float Inu_JVa[9];
	float Inu_JPg[9];
	float Inu_JPa[9];
	float Inu_C[15 * 15];
	float Inu_avgA[3];
	float Inu_avgW[3];

	k = 70;
	Inu_dT = floatP[k];
	k ++;
	for (i = 0; i < 9; i++)
	{
		 Inu_dR[i] = floatP[k + i];
	}
	k += i;
	for (i = 0; i < 3; i++)
	{
		 Inu_dV[i] = floatP[k + i];
	}
	k += i;
	for (i = 0; i < 3; i++)
	{
		 Inu_dP[i] = floatP[k + i];
	}
	k += i;
	for (i = 0; i < 9; i++)
	{
		 Inu_JRg[i] = floatP[k + i];
	}
	k += i;
	for (i = 0; i < 9; i++)
	{
		 Inu_JVg[i] = floatP[k + i];
	}
	k += i;
	for (i = 0; i < 9; i++)
	{
		 Inu_JVa[i] = floatP[k + i];
	}
	k += i;
	for (i = 0; i < 9; i++)
	{
		 Inu_JPg[i] = floatP[k + i];
	}
	k += i;
	for (i = 0; i < 9; i++)
	{
		 Inu_JPa[i] = floatP[k + i];
	}
	k += i;
	for (i = 0; i < 15*15; i++)
	{
		 Inu_C[i] = floatP[k + i];
	}
	k += i;
	for (i = 0; i < 3; i++)
	{
		 Inu_avgA[i] = floatP[k + i];
	}
	k += i;
	for (i = 0; i < 3; i++)
	{
		 Inu_avgW[i] = floatP[k + i];
	}

	fprintf(fp_output, "dT %f\n", Inu_dT);
	InuPrinVec("avgA", Inu_avgA, 3, fp_output);
	InuPrinVec("avgW", Inu_avgW, 3, fp_output);
	InuPrinVec("dP", Inu_dP, 3, fp_output);
	InuPrinVec("dV", Inu_dV, 3, fp_output);
	InuPrintMat("JPa", Inu_JPa, 3, 3, fp_output);
	InuPrintMat("JPg", Inu_JPg, 3, 3, fp_output);
	InuPrintMat("JVa", Inu_JVa, 3, 3, fp_output);
	InuPrintMat("JVg", Inu_JVg, 3, 3, fp_output);
	InuPrintMat("dR", Inu_dR, 3, 3, fp_output);
	InuPrintMat("C", Inu_C, 15, 15, fp_output);
	InuPrintMat("JRg", Inu_JRg, 3, 3, fp_output);

	fclose(fp_output);
#endif

#ifdef DUMP_SLAM_RESULTS
	slamAddFloatValueToBuffer(outputBuffer, 4, floatP);
	//fwrite(floatP, 4, 16, fp_output);

	offsef += 64;  // file is written after 256 bytes
	numKp = intP[offsef];
	slamAddIntValueToBuffer(outputBuffer, 1, &intP[offsef]);
	//fwrite(&intP[offsef], 4, 1, fp_output);

	offsef += 1;
	slamAddIntValueToBuffer(outputBuffer, numKp, &intP[offsef]);
	//fwrite(&intP[offsef], 4, numKp, fp_output);

	offsef += numKp;
	slamAddIntValueToBuffer(outputBuffer, numKp, &intP[offsef]);
	//fwrite(&intP[offsef], 4, numKp, fp_output);

	offsef += numKp;
	slamAddIntValueToBuffer(outputBuffer, numKp * 16, &intP[offsef]);
	//fwrite(&intP[offsef], 4, numKp * 16, fp_output);

	offsef += numKp * 16;
	slamAddIntValueToBuffer(outputBuffer, numKp, &intP[offsef]);
	//fwrite(&intP[offsef], 4, numKp, fp_output);

	offsef += numKp;
	numKp = intP[offsef];
	slamAddIntValueToBuffer(outputBuffer, 1, &intP[offsef]);
	//fwrite(&intP[offsef], 4, 1, fp_output);

	offsef += 1;
	slamAddIntValueToBuffer(outputBuffer, numKp, &intP[offsef]);
	//fwrite(&intP[offsef], 4, numKp, fp_output);

	offsef += numKp;
	slamAddIntValueToBuffer(outputBuffer, numKp, &intP[offsef]);
	//fwrite(&intP[offsef], 4, numKp, fp_output);

	offsef += numKp;
	slamAddIntValueToBuffer(outputBuffer, numKp * 16, &intP[offsef]);
	//fwrite(&intP[offsef], 4, numKp * 16, fp_output);

	offsef += numKp * 16;
	slamAddFloatValueToBuffer(outputBuffer, 5, &floatP[offsef]);
	//fwrite(&floatP[offsef], 4, 5, fp_output);
#endif
	slamAddFloatValueToBuffer(outputBuffer, 16, floatP);
	snprintf(outputBuffer + strlen(outputBuffer), LOC_MAXLEN - strlen(outputBuffer), "\n");
	slamPos[0] = floatP[0];
	slamPos[1] = floatP[1];
	slamPos[2] = floatP[2];
	slamPos[3] = floatP[3];
	slamPos[4] = floatP[4];
	slamPos[5] = floatP[5];
	slamPos[6] = floatP[6];

#if 0
	if (0)
	{
		GLenum typ;
		GLenum fmt;
		int width = SLAM_ROUTE_WINDOW_WIDTH;
		int height = SLAM_ROUTE_WINDOW_HEIGHT;
		static int frameIdx = 0;
		static int yStartValue;
		if (frameIdx == 0)
		{
			memset(slamRouteFrame, 0, sizeof(slamRouteFrame));
			CreateGLWindow(width, height, "SLAM_ROUTE");
			yStartValue = (int)(slamPos[5] * 10);
		}
		typ = GL_UNSIGNED_BYTE;
		fmt = GL_RGB;
		int x = (int)(slamPos[4] * 10);
		int y = (int)(slamPos[5] * 10);
		int z = (int)(slamPos[6] * 10);
		x += width / 2;
		z += height / 2;
		if ((x > 0) && (x < width) && (z > 0) && (z < height))
		{
			unsigned char depthRed, depthGreen, depthBlue;
			int yDiff = y - yStartValue + 128;
			if (yDiff < 0)
				yDiff = 0;
			if (yDiff > 255)
				yDiff = 255;
			if (yDiff < 43) {
				depthRed = yDiff * 6;
				depthGreen = 0;
				depthBlue = yDiff * 6;
			}
			if (yDiff > 42 && yDiff < 85) {
				depthRed = 255 - (yDiff - 43) * 6;
				depthGreen = 0;
				depthBlue = 255;
			}
			if (yDiff > 84 && yDiff < 128) {
				depthRed = 0;
				depthGreen = (yDiff - 85) * 6;
				depthBlue = 255;
			}
			if (yDiff > 127 && yDiff < 169) {
				depthRed = 0;
				depthGreen = 255;
				depthBlue = 255 - (yDiff - 128) * 6;
			}
			if (yDiff > 168 && yDiff < 212) {
				depthRed = (yDiff - 169) * 6;
				depthGreen = 255;
				depthBlue = 0;
			}
			if (yDiff > 211 && yDiff < 254) {
				depthRed = 255;
				depthGreen = 255 - (yDiff - 212) * 6;
				depthBlue = 0;
			}
			if (yDiff > 253) {
				depthRed = 255;
				depthGreen = 0;
				depthBlue = 0;
			}
			slamRouteFrame[3 * (z*width + x)] = depthRed;
			slamRouteFrame[3 * (z*width + x) + 1] = depthGreen;
			slamRouteFrame[3 * (z*width + x) + 2] = depthBlue;
		}
		else
			printf("bad x,y = %d %d\n", x, z);
		StreamDisplay(width, height, slamRouteFrame, fmt, typ, 0);
		frameIdx++;
	}
#endif

	if (channelParamsP->displayOutput)
	{
		printf(outputBuffer);
	}

	if (channelParamsP->recordOutput || channelParamsP->recordOutputFrames)
	{
		saveBufferAsTextToDisk(ref, outputBuffer, hdrP);
      //decrease number of remaining frames to record
      if (channelParamsP->recordOutputFrames)
          channelParamsP->recordOutputFrames--;
	}
}

#if defined _OPENCV_
static void yuv422i_to_bgr(uint8_t* src, int width, int height, cv::Mat& bgr)
{
    cv::Mat yuv422i = cv::Mat(height, width, CV_8UC2, src);
    cv::cvtColor(yuv422i, bgr, cv::COLOR_YUV2BGR_YUYV);
}

static void yuv422semi_to_bgr(uint8_t* src, int width, int height, cv::Mat& bgr)
{
    uint8_t* pUV, * pY;
    int plane_size = width * height;
    cv::Mat yuyv(height, width, CV_8UC2);

    uint8_t* dst = yuyv.data;

    pY = src;
    pUV = src + plane_size;
    for (; pY < src + plane_size; ) {
        *dst++ = *pY++;
        *dst++ = *pUV++;
        *dst++ = *pY++;
        *dst++ = *pUV++;
    }

    cv::cvtColor(yuyv, bgr, cv::COLOR_YUV2BGR_YUYV);
}
#endif

UINT8 first_time = 1;
UINT32 ts_frame_index = 0;
void show_tuning_server_stream(UINT8* buff, UINT32 width, UINT32 height, UINT32 type)
{
#if defined _OPENCV_
    cv::Mat img;
    char win_name[45];
    if (type == 0)
    {
        yuv422semi_to_bgr(buff, width, height, img);
    }
    else
    {
        yuv422i_to_bgr(buff, width, height, img);
    }
    sprintf(win_name, "Tuning server output %d x %d", width, height);
    if (first_time)
    {
        cv::namedWindow(win_name, cv::WINDOW_NORMAL);
        cv::resizeWindow(win_name, width / 4, height / 4);
        first_time = 0;
    }
    if (StreamDisplay(&img, win_name))
    {
        char name[512];
        sprintf(name, "%s%stuning_%dx%d_%d.raw", RECORD_FOLDER_NAME, BACK_SLASH, width, height, ts_frame_index);
        saveDataToDisk((char *)buff, (width*height*2), name);

    }
    ts_frame_index++;
#else
    //To fix unused params warning.
    (void)(buff);
    (void)(width);
    (void)(height);
#endif
}

void get_callbacks(FWLib_configParams *paramsP, FWLib_cfgChannelCbsT *chCfgCbsP, char *stream_out_name)
{
	ERRG_codeE status;
	inu_nodeH streamerH;
	inu_nodeH dataH;

	status = inu_graph__findNode(paramsP->configH, stream_out_name, &streamerH);

	if (ERRG_SUCCEEDED(status) && (INU_STREAMER_REF_TYPE == inu_ref__getRefType((inu_refH*)streamerH)))
	{
		//get data
		dataH = inu_node__getNextInputNode(streamerH, NULL);
		if (dataH)
		{
			inu_ref__types ref_type = inu_ref__getRefType((inu_refH*)dataH);

			memset(chCfgCbsP, 0, sizeof(FWLib_cfgChannelCbsT));

			switch (ref_type)
			{
			case(INU_IMAGE_REF_TYPE) :
			{
				chCfgCbsP->frameCallback = imageFrameCb;
				chCfgCbsP->startCallback = imageStartCb;
				chCfgCbsP->stopCallback = imageStopCb;
				chCfgCbsP->failCallback = imagefailCallback;
				chCfgCbsP->recursiveStartDone = imageRecursiveStartDone;
				break;
			}
			case(INU_IMU_DATA_REF_TYPE) :
			{
				chCfgCbsP->frameCallback = imuFrameCb;
				chCfgCbsP->startCallback = startCb;
				chCfgCbsP->stopCallback = stopCb;
				break;
			}
			case(INU_TEMPERATURE_DATA_REF_TYPE) :
			{
				chCfgCbsP->frameCallback = temperatureFrameCb;
				chCfgCbsP->startCallback = startCb;
				chCfgCbsP->stopCallback = stopCb;
				break;
			}
			case(INU_CVA_DATA_REF_TYPE) :
			{
				chCfgCbsP->frameCallback = cvaFrameCb;
				chCfgCbsP->startCallback = startCb;
				chCfgCbsP->stopCallback = stopCb;
				break;
			}
			case(INU_HISTOGRAM_DATA_REF_TYPE) :
			{
				chCfgCbsP->frameCallback = histogramFrameCb;
				chCfgCbsP->startCallback = startCb;
				chCfgCbsP->stopCallback = stopCb;
				break;
			}
			case(INU_CDNN_DATA_REF_TYPE) :
			{
				chCfgCbsP->frameCallback = cdnnFrameCb;
				chCfgCbsP->startCallback = cdnnStartCb;
				chCfgCbsP->stopCallback = stopCb;
				break;
			}
			case(INU_SLAM_DATA_REF_TYPE) :
			{
				chCfgCbsP->frameCallback = slamFrameCb;
				chCfgCbsP->startCallback = slamStartCb;
				chCfgCbsP->stopCallback = stopCb;
				break;
			}
			case(INU_POINT_CLOUD_DATA_REF_TYPE):
			{
				chCfgCbsP->frameCallback = plyFrameCb;
				chCfgCbsP->startCallback = plyStartCb;
				chCfgCbsP->stopCallback = stopCb;
				break;
			}
			default:
				break;
			}
		}
	}
}

void post_process_init()
{
	//create record output dir
	system(CREATE_RECORD_FOLDER);
}
