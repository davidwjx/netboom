#ifndef _INU_TSNR_DATA_API_H_
#define _INU_TSNR_DATA_API_H_


typedef void* inu_tsnr_dataH;

#define TSNR_MAX_BLOB_SIZE			(8*1024)


typedef enum
{
   INU_TSNR_DATA_LEFT,
   INU_TSNR_DATA_RIGHT,
   INU_TSNR_DATA_BOTH,
}INU_TSNR_DATA_sideToCalcE;
	

typedef struct
{
   inu_data__hdr_t                 dataHdr;       //header (timestamp & index)
   inu_tsnr_calc__parameter_list_t paramsList;

   //DSP to GP	
   UINT32 nValid;
   float  tsnr;
   UINT32 satIndex; 
}inu_tsnr_data__hdr_t;

typedef struct
{
   inu_data__initParams    dataInitParams;
}inu_tsnr_data__initParams;


#endif

