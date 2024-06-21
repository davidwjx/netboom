
#include "inu_common.h"
#include "inu2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inu_metadata.h"
#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <marshal.h>
#include <inu_data.h>
#include <inu_data_api.h>
#include "inu_metadata_serializer.h"
static ERRG_codeE inu_metadata__serializeProtobuf(char *mempoolHandle, INU_metadata_TopLevel *toplevel, UINT32 bufferSize,const UINT32 NANO_PB_FLAGS,
    UINT32 *outputSize )
{   
    if(bufferSize <  FIXED_METADATA_SIZE)
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Warning: Metadata buffer size is not > FIXED_METADATA_SIZE \n");
        return INU_METADATA__ERR_METADATA_BUFFER_SIZE_FAIL;
    }
    const UINT32 protobuf_buffer_size = bufferSize-FIXED_METADATA_SIZE;
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Writing protobuf buffer \n");
    pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)&mempoolHandle[PROTOBUF_PAYLOAD_OFFSET], protobuf_buffer_size);
    const bool returnValue = pb_encode_ex(&stream,INU_metadata_TopLevel_fields,toplevel,NANO_PB_FLAGS);
    if(!returnValue)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Protobuf Encoding has failed,bufferSize=%d,buffer=%X,errorcode=%s \n",bufferSize,mempoolHandle,PB_GET_ERROR(&stream));
        return INU_METADATA__ERR_METADATA_PB_ENCODE_FAIL;
    }
    else
    {
        *outputSize = stream.bytes_written + PROTOBUF_PAYLOAD_OFFSET;
        return INU_METADATA__RET_SUCCESS;
    }
}
static ERRG_codeE inu_metadata__deSerializeProtobuf(char* mempoolHandle, INU_metadata_TopLevel* toplevel, UINT32 bufferSize)
{
    if (bufferSize < FIXED_METADATA_SIZE)
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "Metadata buffer size is not > FIXED_METADATA_SIZE \n");
        return INU_METADATA__ERR_METADATA_BUFFER_SIZE_FAIL;
    }
    if (mempoolHandle[0] != METADATA_FIXED_DATA_WORD)
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "Metadata byte[0]!=METADATA_FIXED_DATA_WORD \n");
        return INU_METADATA__ERR_METADATA_BUFFER_FAIL;
    }
    if (mempoolHandle[FIXED_METADATA_SIZE] != METADATA_PROTOBUF_DATA_WORD)
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "Metadata byte[FIXED_METADATA_SIZE]!=METADATA_PROTOBUF_DATA_WORD \n");
        return INU_METADATA__ERR_METADATA_BUFFER_FAIL;
    }
    const UINT32 protobuf_buffer_size = bufferSize - FIXED_METADATA_SIZE;
    /*Create input stream */
    pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)&mempoolHandle[PROTOBUF_PAYLOAD_OFFSET], protobuf_buffer_size);
    const bool returnValue = pb_decode_ex(&stream, INU_metadata_TopLevel_fields, toplevel, PB_DECODE_DELIMITED);

    if (!returnValue)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Protobuf Deoding has faield. bufferSize=%d,buffer=%X,errorcode=%s \n", bufferSize, mempoolHandle, PB_GET_ERROR(&stream));
        return INU_METADATA__ERR_METADATA_PB_DECODE_FAIL;
    }
    else
    {
        return INU_METADATA__RET_SUCCESS;
    }
}
 UINT8 inu_metadata__getProtocol_Version()
{
    return PROTOCOL_VERSION;
}
ERRG_codeE inu_metadata__serializeFixedLocationOnly(char *buffer,UINT32 size,INU_Metadata_T*toplevel)
{
    if (size < FIXED_METADATA_SIZE)
    {
        return INU_METADATA__ERR_METADATA_BUFFER_SIZE_FAIL;
    }
    /*First FIXED_METADATA_SIZE bytes are for fixed data (I.E data transferred from a register via the DMAC) */
    buffer[0] = METADATA_FIXED_DATA_WORD;        
    buffer[FIXED_METADATA_SIZE] = METADATA_PROTOBUF_DATA_WORD;
    buffer[METADATA_FIXED_LOCATION_PROTOCOL_VERSION_OFFSET] = inu_metadata__getProtocol_Version();
    /*Example of Fixed location serialization, this will be done by the DMAC for the finished software*/
    /*Note that these two fields will be overwritten by the DMAC */
    MARSHALG_u32(MARSHALG_DO, &toplevel->frameID, (UINT8*)&buffer[METADATA_FIXED_LOCATION_FRAME_ID_OFFSET]);
    MARSHALG_u64(MARSHALG_DO, &toplevel->timestamp, (UINT8 *) &buffer[METADATA_FIXED_LOCATION_TIMESTAMP_OFFSET]);
    MARSHALG_u32(MARSHALG_DO,&toplevel->serialization_counter,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_SERIALIZATION_COUNTER_OFFSET]);
    MARSHALG_u32(MARSHALG_DO,&toplevel->framebuffer_offset,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_FRAME_BUFFER_OFFSET]);
    MARSHALG_u64(MARSHALG_DO,&toplevel->software_injection_timestamp_rtc,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_SW_INJECTION_RTC_TIMESTAMP]);
    MARSHALG_u32(MARSHALG_DO,&toplevel->eofFrameID,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_EOF_FRAME_ID]);
    MARSHALG_u32(MARSHALG_DO,&toplevel->software_injection_call_counter,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_SOFTWARE_INJECTION_COUNTER]);
    MARSHALG_u32(MARSHALG_DO,&toplevel->isp0FrameID,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_ISP0_FRAME_ID]);
    MARSHALG_u32(MARSHALG_DO,&toplevel->isp1FrameID,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_ISP1_FRAME_ID]);
    MARSHALG_u32(MARSHALG_DO,&toplevel->droppedFrameCounter,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_DROPPED_FRAME_COUNTER]);
    MARSHALG_u32(MARSHALG_DO,&toplevel->compensatedDroppedFrameCounter,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_DROPPED_FRAME_COMPENSATED]);
    MARSHALG_u32(MARSHALG_DO,&toplevel->excessDequeues,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_EXCESS_DEQUEUES]);
    return INU_METADATA__RET_SUCCESS;
}
ERRG_codeE inu_metadata__serialize(char *buffer,UINT32 size,INU_Metadata_T*toplevel)
{
    UINT32 bytesWritten = 0;
    ERRG_codeE ret = inu_metadata__serializeFixedLocationOnly(buffer,size,toplevel);
    if(ERRG_FAILED(ret))
        return ret;
    return inu_metadata__serializeProtobuf(buffer, &toplevel->protobuf_packet, size, PB_ENCODE_DELIMITED,&bytesWritten );
}
ERRG_codeE inu_metadata__serializeWithoutDelimiter(char *buffer,UINT32 size,INU_Metadata_T*toplevel, UINT32 *outputSize)
{
    ERRG_codeE ret = inu_metadata__serializeFixedLocationOnly(buffer,size,toplevel);
    if(ERRG_FAILED(ret))
        return ret;
    return inu_metadata__serializeProtobuf(buffer, &toplevel->protobuf_packet, size,0, outputSize);
}
ERRG_codeE inu_metadata__deserialize(char* buffer, UINT32 size, INU_Metadata_T* toplevel)
{
    ERRG_codeE ret = inu_metadata__deserialize_withoutProtobufDecode(buffer,size,toplevel);
    if (ret != INU_METADATA__RET_SUCCESS)
    {
        return ret;
    };
    return inu_metadata__deSerializeProtobuf(buffer, &toplevel->protobuf_packet, size);
}
/**
 * @brief Decodes a Varint
 * 
 *
 * @param buffer Buffer
 * @param size 
 * @return Returns the Varint size if positive, if negative then an error has occured
 */
int inu_metadata__decodeVarint(char *buffer, UINT32 size, UINT32 *varintSize)
{
    /*Taken from https://wiki.vg/Protocol#VarInt_and_VarLong*/
    int value = 0;
    int position = 0;
    UINT8 currentByte = 0;
    UINT8 counter = 0;
    *varintSize = 1;
    while (true) {
        if(counter > size)
        {
            /*Size error*/
            return -2; 
        }
        currentByte = buffer[counter];
        value |= (currentByte & 0x7F) << position;

        if ((currentByte & 0x80) == 0) 
            break;

        position += 7;

        if (position >= 32) 
            /*Position errror*/
            return -1;
        *varintSize++;
        counter++;
    }

    return value;
}
int inu_metadata__getMetadataSizeWithoutLinePadding(char *buffer, UINT32 size)
{   /*We are using encoding our protobuf messages with PB_DECODE_DELIMITED so that we can */
    if (size < FIXED_METADATA_SIZE + sizeof(UINT32))
    {
         /* Something is wrong if you don't have the fixed location metadata + 4 bytes spare for the Protobuf Varint*/
        LOGG_PRINT(LOG_ERROR_E,NULL,"Buffer size too small \n");
        return -1;
    }
    const UINT32 protobufSize = size-FIXED_METADATA_SIZE;
    UINT32 varintBytes = 0;
    int measuredProtobufSize = inu_metadata__decodeVarint(&buffer[PROTOBUF_PAYLOAD_OFFSET],protobufSize,&varintBytes);
    if(measuredProtobufSize < 0)
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Error Decoding Varint RetCode=%d \n", protobufSize);
        return measuredProtobufSize;
    }
    /*The total metadata size without padding is
    The layout of the metadata is as follows:
    Bytes 0->32             | Byte 33             | Byte 34 Onwards
    FIXED_LOCATION_METADATA | PROTOBUF PREAMBLE   |  Protobuf prepended with a varint containing the size of the protobuf 
    */
    /*We need to add 1 byte extra because some programs expect the Protobuf byte array to be null terminated*/
    return PROTOBUF_PAYLOAD_OFFSET + measuredProtobufSize + varintBytes + sizeof(char); 
}
ERRG_codeE inu_metadata__deserialize_withoutProtobufDecode(char* buffer, UINT32 size, INU_Metadata_T* toplevel)
{
    ERRG_codeE ret = inu_metadata__verifyPreambleBytesOK(buffer, size);
    if (ret != INU_METADATA__RET_SUCCESS)
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Preamble bytes invalid \n");
        return ret;
    }
    /*Marshal code treats the buffer as little Endian which matches the registers*/
    MARSHALG_u32(MARSHALG_UNDO, &toplevel->frameID, (UINT8*)&buffer[METADATA_FIXED_LOCATION_FRAME_ID_OFFSET]);
    MARSHALG_u64(MARSHALG_UNDO, &toplevel->timestamp, (UINT8*)&buffer[METADATA_FIXED_LOCATION_TIMESTAMP_OFFSET]);
    MARSHALG_u32(MARSHALG_UNDO,&toplevel->serialization_counter,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_SERIALIZATION_COUNTER_OFFSET]);
    MARSHALG_u32(MARSHALG_UNDO,&toplevel->framebuffer_offset,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_FRAME_BUFFER_OFFSET]);
    MARSHALG_u64(MARSHALG_UNDO,&toplevel->software_injection_timestamp_rtc,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_SW_INJECTION_RTC_TIMESTAMP]);
    MARSHALG_u32(MARSHALG_UNDO,&toplevel->eofFrameID,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_EOF_FRAME_ID]);
    MARSHALG_u32(MARSHALG_UNDO,&toplevel->software_injection_call_counter,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_SOFTWARE_INJECTION_COUNTER]);
    MARSHALG_u32(MARSHALG_UNDO,&toplevel->isp0FrameID,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_ISP0_FRAME_ID]);
    MARSHALG_u32(MARSHALG_UNDO,&toplevel->isp1FrameID,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_ISP1_FRAME_ID]);
    MARSHALG_u32(MARSHALG_UNDO,&toplevel->droppedFrameCounter,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_DROPPED_FRAME_COUNTER]);
    MARSHALG_u32(MARSHALG_UNDO,&toplevel->compensatedDroppedFrameCounter,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_DROPPED_FRAME_COMPENSATED]);
    MARSHALG_u32(MARSHALG_UNDO,&toplevel->excessDequeues,(UINT8 *) &buffer[METADATA_FIXED_LOCATION_EXCESS_DEQUEUES]);
    return ret;
}
UINT32 inu_metadata__getCVAMetadataSize()
{
    return METADATA_CVA_SIZE;
}
UINT32 inu_metadata__getNumberMetadataRows()
{
    /*Change this number to increase the amount of extra rows to allocate for */
    return ALLOCATED_NUMBER_OF_METADATA_ROWS;
}
UINT32 inu_metadata__getMetadataSize()
{
    /*Change this number to increase the amount of extra rows to allocate for */
    return ALLOCATED_NUMBER_OF_METADATA_ROWS;
}
ERRG_codeE inu_metadata__verifyPreambleBytesOK(char* buffer, UINT32 size)
{
    if (size < FIXED_METADATA_SIZE)
    {
        /*Buffer is not large enough fro the fixed location metadata */
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Buffer size too small\n");
        return INU_METADATA__ERR_METADATA_BUFFER_SIZE_FAIL;
    }
    if (buffer[METADATA_FIXED_LOCATION_PROTOCOL_VERSION_OFFSET] != inu_metadata__getProtocol_Version())
    {
        /*Version mismatch detected between metadata on the target and the host */
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Protocol version mismatch\n");
        return INU_METADATA__ERR_METADATA_PROTOCOL_VERSION_MISMATCH;
    }
    if (buffer[0] != METADATA_FIXED_DATA_WORD)
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"First byte mismatch\n");
        return INU_METADATA__ERR_METADATA_PREAMBLE_WRONG;
    }
    if (buffer[FIXED_METADATA_SIZE] != METADATA_PROTOBUF_DATA_WORD)
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"FIXED_METADATA_SIZE byte mismatch\n");
        return INU_METADATA__ERR_METADATA_PREAMBLE_WRONG;
    }
    return INU_METADATA__RET_SUCCESS;
}

ERRG_codeE inu_metadata__getFrameBufferOffset(char* buffer, UINT32 size, UINT32 *fboffset)
{
    if (size > FIXED_METADATA_SIZE)
    {
        MARSHALG_u32(MARSHALG_UNDO, fboffset, (UINT8*)&buffer[METADATA_FIXED_LOCATION_FRAME_BUFFER_OFFSET]);
        return INU_METADATA__RET_SUCCESS;
    }
    else 
    {
        return INU_METADATA__ERR_METADATA_BUFFER_SIZE_FAIL;
    }
}

