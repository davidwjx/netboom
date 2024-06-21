#pragma once

#include "inu2.h"



void post_process_init();

void startCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie);
void frameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);
void stopCb(inu_refH ref, void *cookie);

void imageStopCb(inu_refH ref, void *cookie);
void imageStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie);
void imageFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);

void cdnnFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);

void temperatureFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);

void imuFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);

void histogramFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);

void cvaFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *pdata, int size, void *cookie);

void matcherFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);

void gaussianFromFileFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);
void iicFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);

void fdkBasicDemoStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie);
void fdkBasicDemoFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);

void slamStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie);
void slamFromFileStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie);
void slamFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);
void cdnnStartCb(inu_refH ref, inu_data__hdr_t* hdrP, void* cookie);
void plyStartCb(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie);
void plyFrameCb(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie);
void get_callbacks(FWLib_configParams *paramsP, FWLib_cfgChannelCbsT *chCfgCbsP, char *stream_out_name);
void show_tuning_server_stream(UINT8* buff, UINT32 width, UINT32 height, UINT32 type);