/****************************************************************************
 * 
 *   FileName: inu_fw_internal.h
 *
 *   Author: Konstantin Sinyuk
 *
 *   Date: 11/02/2016
 *
 *   Description: INU FW API
 *
 ****************************************************************************/


/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

typedef ERRG_codeE (*CONFG_writeRegCbT) (inu_device__writeRegT *writeRegParamsP);
typedef ERRG_codeE (*CONFG_writeBufCbT) (void *bufPtr, UINT32 bufSize);

/**************************************************************************** 
 *        Functions to support configuration folder delivery to target      *
 ****************************************************************************/
int CONFG_loadConfig(CONFG_writeRegCbT writeRegCbFunc, CONFG_writeBufCbT WriteBufCbFunc, char *configFolder, CONFG_modeE mode, INUG_configMetaT *metaData);



#ifdef __cplusplus
}
#endif
