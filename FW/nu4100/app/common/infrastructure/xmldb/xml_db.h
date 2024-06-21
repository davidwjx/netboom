/*
xml database module
*/
#ifndef __XML_DB_H__
#define __XML_DB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "xml_path.h"
#include "xml_path_extended.h"
#include "err_defs.h"

#define XMLDB_MAX_PATH_DEPTH (20)
#define XMLDB_MAX_TAG_LEN  (128)
#define XMLDB_MAX_TEXT_LEN  (128) 
#define XMLDB_MAX_PATH_LEN  (XMLDB_MAX_TAG_LEN*XMLDB_MAX_PATH_DEPTH)

typedef struct _XMLDB_dbT *XMLDB_dbH;

bool XMLDB_isConst(UINT16 flags);
ERRG_codeE XMLDB_open(XMLDB_dbH *h);
ERRG_codeE XMLDB_duplicate(XMLDB_dbH h, XMLDB_dbH *dup);
void XMLDB_close(XMLDB_dbH h);
ERRG_codeE XMLDB_loadFromBuf(XMLDB_dbH, const char *xmlbuf);
ERRG_codeE XMLDB_getValue(XMLDB_dbH, XMLDB_pathE, UINT32 *value);
ERRG_codeE XMLDB_getResetValue(XMLDB_dbH, XMLDB_pathE, UINT32 *value);
ERRG_codeE XMLDB_getBlockAdd(XMLDB_dbH db, XMLDB_pathE path, UINT32 *resetValue);
ERRG_codeE XMLDB_getRegOffset(XMLDB_dbH db, XMLDB_pathE path, UINT16 *resetValue);
ERRG_codeE XMLDB_getFieldStartBitOffset(XMLDB_dbH db, XMLDB_pathE path, UINT8 *resetValue);
ERRG_codeE XMLDB_getFieldWidth(XMLDB_dbH db, XMLDB_pathE path, UINT8 *value);
ERRG_codeE XMLDB_setValue(XMLDB_dbH, XMLDB_pathE, UINT32 value);
ERRG_codeE XMLDB_setValueAndUpdateFastDb(XMLDB_dbH db, UINT32 blk, UINT32 inst, XMLDB_pathE field, UINT32 value);
ERRG_codeE XMLDB_setRegFields(XMLDB_dbH db, XMLDB_pathE path, UINT32 resetVal, UINT32 blockAdd, UINT16 regOffset, UINT8 startBitOffset, UINT8 width);
ERRG_codeE XMLDB_clearValue(XMLDB_dbH, XMLDB_pathE);
ERRG_codeE XMLDB_saveToBuf(XMLDB_dbH, const char *xmlbuf, char *outbuf, unsigned int *outbufSize);
ERRG_codeE XMLDB_saveDb(XMLDB_dbH db, const char *xmlbuf, unsigned int *outSize, char **outBuf);
void XMLDB_writeXml(const char *xmlbuf, const char *name, size_t size);
int XMLDB_loadNuSocxml(char **xmlbuf, unsigned int *xmlsize, const char *xmlFileName);
ERRG_codeE XMLDB_writeDbToRegs(XMLDB_dbH db, UINT32 isMainGraph);
ERRG_codeE XMLDB_writeFieldToReg(XMLDB_dbH db, XMLDB_pathE path, UINT32 val);
ERRG_codeE XMLDB_writeBlockDbToRegs(XMLDB_dbH db, UINT32 block, UINT32 blkInstance);
ERRG_codeE XMLDB_exportTblP(XMLDB_dbH db, const char **buf, UINT32 *outSize);
ERRG_codeE XMLDB_importTbl(const char *buf, UINT32 bufSize, XMLDB_dbH db);
const char *XMLDB_getPathStr(XMLDB_pathE);
void XMLDB_setDebugMode(int debugMode);
UINT32 XMLDB_extendedToRegularEnumConvert(UINT32 externaUserId);
ERRG_codeE XMLDB_writeFieldToRegFromDb(XMLDB_dbH db, XMLDB_pathE path);
ERRG_codeE XMLDB_fillFastWriteBlock(XMLDB_dbH db, UINT32 block, UINT32 blkInstance);

#ifdef __cplusplus
}
#endif
#endif //__XML_DB_H__