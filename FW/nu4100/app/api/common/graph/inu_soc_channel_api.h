#ifndef _INU_SOC_CHANNEL_API_H_
#define _INU_SOC_CHANNEL_API_H_

#include "inu2.h"

typedef void* inu_soc_channelH;

typedef struct
{
   UINT16 xStart;
   UINT16 yStart;
} inu_soc_channel__cropParamsT;

typedef struct
{
   UINT32 param1;
   UINT32 param2;
} inu_soc_channel__userParamsT;

UINT32 inu_soc_channel__getId(inu_soc_channelH meH);

ERRG_codeE inu_soc_channel__updateCropWindow(inu_soc_channelH meH, inu_soc_channel__cropParamsT *crop);
ERRG_codeE inu_soc_channel__updateUserParams(inu_soc_channelH meH, inu_soc_channel__userParamsT *user);
ERRG_codeE inu_soc_channel__resetFrameCnt(inu_soc_channelH meH);


#endif
