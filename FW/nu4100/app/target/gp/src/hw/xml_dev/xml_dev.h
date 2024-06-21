#ifndef __XMLDEV_H__
#define __XMLDEV_H__

#include "xml_db.h"

#define XMLDEV_NA_REG  ((UINT32)(~0))
#define XMLDEV_NA_PATH ((UINT32)XMLDB_NUM_PATHS_E)
typedef struct 
{
   XMLDB_pathE path;
   UINT32 offset;
   UINT16 pos;
   UINT16 len;
} XMLDEV_fieldT;

typedef struct _xmldevT *XMLDEV_devH;

int XMLDEV_init(XMLDEV_devH *dev, const char *name, UINT32 base, XMLDEV_fieldT *fieldTbl, unsigned int numFields);
void XMLDEV_deinit(XMLDEV_devH);
void XMLDEV_writeField(XMLDEV_devH, XMLDB_pathE field, UINT32 value);
UINT32 XMLDEV_readField(XMLDEV_devH, XMLDB_pathE field);
int XMLDEV_writeDb(XMLDEV_devH, XMLDB_dbH dbh);

UINT32 XMLDEV_getBase(XMLDEV_devH);

#endif //__XMLDEV_H__