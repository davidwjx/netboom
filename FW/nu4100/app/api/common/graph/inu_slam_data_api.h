#ifndef _INU_SLAM_DATA_API_H_
#define _INU_SLAM_DATA_API_H_

//#define SLAMG_MATCHED_KP_ZISE_TO_HOST_SIZE (4 + 4 + 800 * (4 + 4))
#define SLAMG_MATCHED_KP_ZISE_TO_HOST_SIZE (0)
#define SLAMG_RECORD_DATA_TO_HOST_SIZE (0)
#define SLAMG_MAX_BLOB_SIZE			(256 + SLAMG_MATCHED_KP_ZISE_TO_HOST_SIZE + SLAMG_RECORD_DATA_TO_HOST_SIZE)

typedef void* inu_slam_dataH;


/***************************************************************************
totOutputSize - total results size
frameId - webcam frame number
****************************************************************************/
typedef struct
{
   inu_data__hdr_t           dataHdr;       //header (timestamp & index)
   UINT8 *ddrP;
   UINT32 totOutputSize;
   UINT32 slamImuDataSampleNum;
   UINT32 frameId;
}inu_slam_data__hdr_t;

typedef struct
{
   inu_data__initParams    dataInitParams;
}inu_slam_data__initParams;


#endif

