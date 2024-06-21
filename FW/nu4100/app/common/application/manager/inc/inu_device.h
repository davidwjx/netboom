#ifndef __INU_DEVICE__H__
#define __INU_DEVICE__H__

#include "inu2.h"
#include "inu_ref.h"
#include "inu_device_api.h"
#include "inu_sensorsync.h"
#ifdef __cplusplus
extern "C" {
#endif


#if DEFSG_IS_GP
#define SLESH "//"
#endif
struct inu_device;

typedef struct
{
	UINT32            type;
	UINT32            bufferSize;
	UINT32            numBuffers;
	int               rstptr;
}inu_device__memParams;

typedef struct inu_device_CtorParamsTag
{
	inu_ref__CtorParams ref_params;
	inu_device__initParams deviceInitParams;
	inu_device__memParams mem_params;
	unsigned int 			version;
	inu_device_standalone_mode_e standAloneMode;
}inu_device__CtorParams;

typedef struct inu_device
{
	inu_ref                ref;
	void                   *privP;
}inu_device;
typedef struct
{
	char buffer[SENSOR_SYNC_topLevel_size];
} inu_device__sensorSync;

void inu_device__vtable_init();
const inu_ref__VTable *inu_device__vtable_get(void);

inu_ref *inu_device__getRefById(inu_device *me, inu_ref__id_t refId);
void inu_device__rxNotify(void *me);
ERRG_codeE inu_device__addRef(inu_device *me, inu_ref *ref);
ERRG_codeE inu_device__registerRef(inu_device *me, inu_ref *ref);
void inu_device__removeRef(inu_device *me, inu_ref *ref);
const char *inu_device__getBootPathFromRef(inu_ref *ref);
INT32 inu_device__getBootIdFromRef(inu_ref *ref);
int inu_device__connState(inu_device *me);
BOOLEAN inu_dev__standAloneMode(inu_deviceH meH);
void       inu_device__fillDeviceRefList(void *me, void *msgP);
UINT32     inu_device__useShareMem(inu_deviceH meH);
ERRG_codeE inu_device__gpCheckFwUpdateBootSuccess(void);
ERRG_codeE inu_device__saveCalibPath(inu_device *me, char* path[], BOOLEAN calibrationLoaded);
ERRG_codeE inu_device__getCalibPath(inu_device *me, const char **path);
ERRG_codeE inu_device__getCalibMode(inu_device *me, UINT32* calibMode);
ERRG_codeE inu_device__sendDspSyncMsg(inu_device *me, UINT32 alg, UINT32 data, UINT32 dspTarget);
ERRG_codeE inu_device__getIdsrLutAddress(void **idsrLutVirtAddress, void **idsrLutPhysAddress);
ERRG_codeE inu_device__enableTuningServer(inu_deviceH meH, inu_device__tuningParamsT* tsParams);
int inu_device__linkEventCallbackWrapper(int sid, int serviceLinkEvent, void* argP);
#ifdef __cplusplus
}
#endif

#endif //__INU_DEVICE__H__
