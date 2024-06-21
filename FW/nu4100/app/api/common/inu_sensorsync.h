#pragma once
#define METADATA
#ifdef METADATA
#include <sensor_sync.pb.h>
#ifdef __cplusplus
extern "C" {
#endif
/*Sensor Sync Structure */
typedef struct {

	/*Protobuf data serialzied by the GP*/
	SENSOR_SYNC_topLevel protobuf_packet;
} inu_sensorSync__updateT;
#ifdef __cplusplus
}
#endif
#endif