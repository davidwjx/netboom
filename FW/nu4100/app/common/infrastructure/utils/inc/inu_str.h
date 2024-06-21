/****************************************************************************
 *
 *   FileName: err.h
 *
 *   Author: Benny V.
 *
 *   Date: 
 *
 *   Description: error handling
 *   
 ****************************************************************************/
#ifndef _INU_STR_H_
#define _INU_STR_H_

#include "inu_types.h"
#include "inu2.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define  INU_STRG_IOCTL_OPCODE_STR_LEN  140

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L       D A T A           ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void INU_STRG_getErrString(ERRG_codeE err, char *stringP);
char *INU_STRG_cmd2String(UINT32 command, char *stringP);
char *INU_STRG_revisionId2String(inu_device__chipRevisionIdE revision, char *stringP);
char *INU_STRG_versionId2String(inu_device__chipVersionIdIdE version, char *stringP);

#ifdef __cplusplus
}
#endif
#endif
