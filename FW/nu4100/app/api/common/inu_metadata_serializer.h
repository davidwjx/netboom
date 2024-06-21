
#ifdef __cplusplus
extern "C" {
#endif
#include "inu_metadata.h"
#include "err_defs.h"
/*Defines for metadata */
#define METADATA_FIXED_DATA_WORD 0x01        /*First FIXED_METADATA_SIZE bytes are for fixed location metadata, this byte is a preamble to say that fixed data is following*/
#define METADATA_PROTOBUF_DATA_WORD 0x02     /*This byte is a preamble to indicate that the following data is protobuf data */
#define FIXED_METADATA_SIZE 72
#define PROTOBUF_PAYLOAD_OFFSET FIXED_METADATA_SIZE + 1
#define PROTOCOL_VERSION	0x02	/*High level protocol version */
#define PROTOBUF_VERSION	0x01	/*Protobuf protocol version */
#define METADATA_FIXED_LOCATION_PROTOCOL_VERSION_OFFSET 1
/*Frame ID (Copied into by the DMAC metadata thread from reading out APB registers) */
#define METADATA_FIXED_LOCATION_FRAME_ID_OFFSET  4
#define METADATA_FIXED_LOCATION_FRAME_ID_SIZE    4
/*Timestamp  (Copied into by the DMAC metadata thread from reading out APB registers) */
#define METADATA_FIXED_LOCATION_TIMESTAMP_OFFSET 12
#define METADATA_FIXED_LOCATION_TIMESTAMP_SIZE 8
/*Serialization counter which is incremented everytime the GP serializes a message */
#define METADATA_FIXED_LOCATION_SERIALIZATION_COUNTER_OFFSET 20
#define METADATA_FIXED_LOCATION_SERIALIZATION_COUNTER_SIZE 4
/*Frame buffer offset (Used by the host to change where the frame buffer starts dynamically depending on the number of metadata rows) */
#define METADATA_FIXED_LOCATION_FRAME_BUFFER_OFFSET 24
#define METADATA_FIXED_LOCATION_FRAME_BUFFER_OFFSET_SIZE 4
/*Added in V2 of the metadata for debugging metadata*/
#define METADATA_FIXED_LOCATION_SW_INJECTION_RTC_TIMESTAMP 28
#define METADATA_FIXED_LOCATION_SW_INJECTION_RTC_TIMESTAMP_SIZE 8
/*EOF frame ID for debugging*/
#define METADATA_FIXED_LOCATION_EOF_FRAME_ID 36
#define METADATA_FIXED_LOCATION_EOF_FRAME_ID_SIZE 4
/*Software injection counter for debugging*/
#define METADATA_FIXED_LOCATION_SOFTWARE_INJECTION_COUNTER 40
#define METADATA_FIXED_LOCATION_SOFTWARE_INJECTION_COUNTER_SIZE 4
#define METADATA_FIXED_LOCATION_ISP0_FRAME_ID 44
#define METADATA_FIXED_LOCATION_ISP0_FRAME_ID_SIZE 4
#define METADATA_FIXED_LOCATION_ISP1_FRAME_ID 48
#define METADATA_FIXED_LOCATION_ISP1_FRAME_ID_SIZE 4
#define METADATA_FIXED_LOCATION_DROPPED_FRAME_COUNTER 52
#define METADATA_FIXED_LOCATION_DROPPED_FRAME_COUNTER_SIZE 4
#define METADATA_FIXED_LOCATION_DROPPED_FRAME_COMPENSATED 56
#define METADATA_FIXED_LOCATION_DROPPED_FRAME_COMPENSATED_SIZE 4
#define METADATA_FIXED_LOCATION_EXCESS_DEQUEUES 60
#define METADATA_FIXED_LOCATION_EXCESS_DEQUEUES_SIZE 4
/*	The sandbox will allocate this many rows for metadata, the target can choose to send less 
	and this isn't a problem since the frame buffer offset is being sent over with every frame
*/
#define ALLOCATED_NUMBER_OF_METADATA_ROWS 1		// modified by david @ 2022.11.08 to make it consistent with the size actually used 
#define CVA_LINE_LENGTH   84
#define METADATA_CVA_SIZE 20*CVA_LINE_LENGTH                 // Size of the CVA metadata in bytes

/*@brief Gets the high level metadata protocol version
  @return Returns the high level metadata protocol version 
*/
UINT8 inu_metadata__getProtocol_Version();
/*@brief Serializes metadata structure into a buffer
  @param buffer Buffer to serialize toplevel into
  @param buffer Size of the buffer
  @param metadata INU_Metadata_T Pointer containing data to serialize
  @return Returns an error code (ERRG_codeE)
*/
ERRG_codeE inu_metadata__serialize(char* buffer, UINT32 size,INU_Metadata_T* metadata );
/**
 * @brief Serializes metadata structure into a buffer but doesn't prepend a VARINT, this is useful for Python decoding which cannot handle the Varint natively and also cannot handle a padded protobuf payload
 * 
 * 
 * @param buffer Buffer to serialize toplevel into
 * @param size Buffer to serialize toplevel into
 * @param metadata INU_Metadata_T Pointer containing data to serialize
 * @param bytesWritten How many bytes were written into the buffer
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE inu_metadata__serializeWithoutDelimiter(char* buffer, UINT32 size,INU_Metadata_T* metadata, UINT32 *bytesWritten);
/*@brief Deserializes metadata  from a buffer into metadata
  @param buffer Buffer to deserialize the metadata from
  @param size Size of the buffer
  @param metadata INU_Metadata_T Pointer to store deserialized data into
  @return Returns an error code (ERRG_codeE)
*/
ERRG_codeE inu_metadata__deserialize(char* buffer, UINT32 size, INU_Metadata_T* metadata );
/**
 * @brief Faster version of inu_metadata__deserialize that doesn't decode the protobuf payload
 * 
 *
 * @param buffer Buffer
 * @param size Size of the buffer
 * @return Returns an error code
 */
ERRG_codeE inu_metadata__deserialize_withoutProtobufDecode(char* buffer, UINT32 size, INU_Metadata_T* toplevel);
/*@brief Verifies that the preamble and protocol version bytes are okay
   @param Buffer to verify that it contains metadata
   @param size Size of the buffer 
 */
ERRG_codeE inu_metadata__verifyPreambleBytesOK(char* buffer, UINT32 size);
/* @brief Gets the frame buffer offset from looking within the metadata
   @param buffer - Buffer to deserialize the frame buffer offset from
   @param size - Size of the buffer
   @param fboffset - Pointer to store frame buffer offset into
 */
ERRG_codeE inu_metadata__getFrameBufferOffset(char* buffer, UINT32 size, UINT32* fboffset);
/**
 * @brief Gets the metadata size from a buffer and excludes line padding. 
 * 
 *  This function is used when sending packets over UART to calculate the actual size of the metadata excluding padding.
 *  Normally there's either line padding added (for metadata sent from the target with frames that have to fill out a complete line or multiple lines) 
 *  or structure padding added (for messages sent from the host where the size has to be fixed for the nu4k serialization layer) 
 *  within the metadata buffers.
 * 
 * @param buffer Buffer to calculate the metadata size for
 * @param size Size of the buffer
 * @return int Returns a negative number on an error or the size when there's not an error
 */
int inu_metadata__getMetadataSizeWithoutLinePadding(char *buffer, UINT32 size);
/*@brief Serializes the fixed location part of the metadata structure into a buffer
  @param buffer Buffer to serialize toplevel into
  @param buffer Size of the buffer
  @param metadata INU_Metadata_T Pointer containing data to serialize
  @return Returns an error code (ERRG_codeE)
*/
ERRG_codeE inu_metadata__serializeFixedLocationOnly(char *buffer,UINT32 size,INU_Metadata_T* toplevel);
#ifdef __cplusplus
}
#endif

