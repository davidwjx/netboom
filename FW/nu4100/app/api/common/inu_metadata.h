#pragma once
#define METADATA
#ifdef METADATA
#define ISP_BUFFER_ALIGN_SIZE 1024
#define METADATA_SIZE_ALIGN(s) ((((s) + ISP_BUFFER_ALIGN_SIZE -1) / ISP_BUFFER_ALIGN_SIZE) * ISP_BUFFER_ALIGN_SIZE)
#ifdef __cplusplus
extern "C" {
#endif
#include <metadata.pb.h>
#include "inu_types.h"
#include "inu2_types.h"
#include "err_defs.h"
/*Metadata structure */
typedef struct {
	/*Fixed location data (For variables copied over by the DMAC from registers */
	UINT64 timestamp;		/*Top 32 bits = Seconds since epoch, Bottom 32 bits = Fraction of a second, where 1 bit is equal to (1/(1<<32) - 1) seconds
								This is always measured from the RTC 
								Note in this case of the IMU, this timestamp shouldn't be used because of the batching.
								Instead the normal timestamp should be used and this is calculated based off the RTC time too.
								*/
	UINT32 frameID;
	UINT32 serialization_counter;
	UINT32 framebuffer_offset;
	UINT64 software_injection_timestamp_rtc; /*Top 32 bits = Seconds since epoch, Bottom 32 bits = Fraction of a second, where 1 bit is equal to (1/(1<<32) - 1) seconds
											This is always measured from the RTC 
											This can be used for debugging and making sure that the metadata was serialized at the correct time
											*/
	UINT32 eofFrameID;			/*End of frame ID*/
	UINT32 software_injection_call_counter;
	UINT32 isp0FrameID;						/*ISP 0 frame ID (Debugging)*/
	UINT32 isp1FrameID;						/*ISP 1 frame ID (Debugging)*/
	UINT32 droppedFrameCounter;				/*How many frames have been dropped  (Used for debugging)*/
	UINT32 compensatedDroppedFrameCounter;  /*How many frames have been dropped and compensated for (Used for debugging)*/
	UINT32 excessDequeues;					/*How many excessive frame IDs have been dequeued (used for debugging)*/
	/*Protobuf data serialzied by the GP*/
	INU_metadata_TopLevel protobuf_packet;
} INU_Metadata_T;
#define ZEROED_OUT_METADATA { 0,0,0,0,0,0,0,0,0,0,0,0,INU_metadata_TopLevel_init_zero }
/**
 * @brief Returns the size of the Metadata for the CVA streams in bytes
 * 
 * @return UINT32 Size of the CVA metadata in bytes;
 */
UINT32 inu_metadata__getCVAMetadataSize();
/*@brief Gets the number of metadata rows
	@return Returns the number of metadata rows
*/
UINT32 inu_metadata__getNumberMetadataRows();
#if DEFSG_IS_HOST
/*This function is not delcared in inu_data_api.h due to a C++ template being in pb.h, 
inu_Data_api.h gets included with many ifdef c++ include guards which causes the pb.h to give a build time error
*/
/*@brief Deserializes metadata  from dataH into the INU_Metadata_T pointer named output
  @param dataH inu_dataH to deserialize metadata from (normally read by Inu_streamer_Read()
  @param output INU_Metadata_T Pointer to store deserialized data into
  @return Returns an error code (ERRG_codeE)
*/
ERRG_codeE inu_data__getMetadata(inu_dataH dataH, INU_Metadata_T* output);

#endif
/*Maximum possible size message that can be sent*/
#define INU_METADATA_MAX_SIZE INU_metadata_TopLevel_size+32
#define INU_IMU_METADATA_SIZE 128 	/*The IMU Metadata doesn't need to be the maximum size and a conservative 128 bytes can be used*/
#ifdef __cplusplus
}
#endif
#endif
