#ifndef __INU_LOAD_BACKGROUND_API_H__
#define __INU_LOAD_BACKGROUND_API_H__

typedef struct
{
   UINT32          backgroundId;
} inu_load_background_hdr_t;

typedef struct
{
   char            *bufP;
   UINT32          bufsize;
   char            *path;
} inu_load_background_memory_t;

typedef struct
{
   inu_deviceH deviceH;
   inu_graphH  graphH;
} inu_load_background__initParams;


//API functions
ERRG_codeE inu_load_background__new(inu_load_backgroundH *meH, inu_load_background__initParams *initParamsP);
void inu_load_background__delete(inu_load_backgroundH meH);
ERRG_codeE inu_load_background__loadBackground(inu_load_backgroundH meH, inu_load_background_memory_t *memP, inu_load_background_hdr_t *backgroundHdrP);
ERRG_codeE inu_load_background__release(inu_load_backgroundH meH);

#endif



