
#include "inu_common.h"
#include "inu2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inu_sensorsync.h"
#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <marshal.h>
#include <inu_data.h>
#include <inu_data_api.h>
#include "inu_sensorsync_serializer.h"
static ERRG_codeE inu_sensorsync__serializeProtobuf(char *buffer, SENSOR_SYNC_topLevel *toplevel, UINT32 bufferSize )
{   
    const UINT32 protobuf_buffer_size = bufferSize;
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Writing protobuf buffer \n");
    pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)&buffer[0], protobuf_buffer_size);
    const bool returnValue = pb_encode_ex(&stream,SENSOR_SYNC_topLevel_fields,toplevel,PB_ENCODE_DELIMITED);
    if(!returnValue)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Protobuf Encoding has failed,bufferSize=%d,buffer=%X,errorcode=%s \n",bufferSize,buffer,PB_GET_ERROR(&stream));
        return INU_METADATA__ERR_METADATA_PB_ENCODE_FAIL;
    }
    else
    {
        return INU_METADATA__RET_SUCCESS;
    }
}
static ERRG_codeE inu_sensorsync__deSerializeProtobuf(char* buffer, SENSOR_SYNC_topLevel* toplevel, UINT32 bufferSize)
{

    const UINT32 protobuf_buffer_size = bufferSize;
    /*Create input stream */
    pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)&buffer[0], protobuf_buffer_size);
    const bool returnValue = pb_decode_ex(&stream, SENSOR_SYNC_topLevel_fields, toplevel, PB_DECODE_DELIMITED);

    if (!returnValue)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Protobuf Decoding has faield. bufferSize=%d,buffer=%X,errorcode=%s \n", bufferSize, buffer, PB_GET_ERROR(&stream));
        return INU_METADATA__ERR_METADATA_PB_DECODE_FAIL;
    }
    else
    {
        return INU_METADATA__RET_SUCCESS;
    }
}
ERRG_codeE inu_sensorsync__serialize(char *buffer,UINT32 size,inu_sensorSync__updateT*toplevel)
{
    return inu_sensorsync__serializeProtobuf(buffer, &toplevel->protobuf_packet, size);
}
ERRG_codeE inu_sensorsync__deserialize(char* buffer, UINT32 size, inu_sensorSync__updateT* toplevel)
{
    return inu_sensorsync__deSerializeProtobuf(buffer, &toplevel->protobuf_packet, size);
}
