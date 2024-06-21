#include "inu_sensorsync.h"
#include "err_defs.h"
/*@brief Serializes sensor sync structure into a buffer
  @param buffer Buffer to serialize toplevel into
  @param buffer Size of the buffer
  @param metadata inu_sensorSync__updateT Pointer containing data to serialize
  @return Returns an error code (ERRG_codeE)
*/
ERRG_codeE inu_sensorsync__serialize(char *buffer,UINT32 size,inu_sensorSync__updateT*toplevel);

/*@brief Deserializes sensor sync  from a buffer into sensorSync
  @param buffer Buffer to deserialize the metadata from
  @param size Size of the buffer
  @param sensorSync inu_sensorSync__updateT Pointer to store deserialized data into
  @return Returns an error code (ERRG_codeE)
*/
ERRG_codeE inu_sensorsync__deserialize(char* buffer, UINT32 size, inu_sensorSync__updateT* toplevel);
