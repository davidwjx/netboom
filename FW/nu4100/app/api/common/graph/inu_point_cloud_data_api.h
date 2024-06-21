#ifndef _INU_POINT_CLOUD_DATA_API_H_
#define _INU_POINT_CLOUD_DATA_API_H_

#define POINT_CLOUDG_MAX_BLOB_SIZE			(1280*1080*6)

typedef void* inu_point_cloud_dataH;

/***************************************************************************
inu_point_cloud_data__hdr_t - is the header at the beginning of the result buffer

****************************************************************************/
typedef struct
{
   inu_data__hdr_t           dataHdr;       //header (timestamp & index)
   UINT32 pointCloudListSize;
}inu_point_cloud_data__hdr_t;

typedef struct
{
   inu_data__initParams    dataInitParams;
}inu_point_cloud_data__initParams;


#endif
