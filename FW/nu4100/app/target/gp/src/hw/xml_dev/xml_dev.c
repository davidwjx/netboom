/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#include <assert.h>

#include "xml_db.h"
#include "xml_dev.h"
#include "mem_map.h"

typedef struct _xmldevT
{
   const char *name;
   UINT16 *pathMapTbl;
   XMLDB_pathE pathMax;
   XMLDB_pathE pathMin;
   unsigned int pathRange;
   XMLDEV_fieldT *regFieldTbl;
   unsigned int numFields;
   UINT32 virtbase;
} xmldevT;

static XMLDB_pathE findMaxPath(xmldevT *dev)
{
   unsigned int i;
   XMLDB_pathE max = (XMLDB_pathE)0;

   for(i = 0; i < dev->numFields; i++)
   {
      if((dev->regFieldTbl[i].path != XMLDEV_NA_PATH) && (max < dev->regFieldTbl[i].path))
         max = dev->regFieldTbl[i].path;
   }
   return max;
}

static XMLDB_pathE findMinPath(xmldevT *dev)
{
   unsigned int i;
   XMLDB_pathE min = XMLDB_NUM_PATHS_E;
   
   for(i = 0; i < dev->numFields; i++)
   {
      if(min > dev->regFieldTbl[i].path) 
         min = dev->regFieldTbl[i].path;
   }
   return min;
}

static void setBits(UINT32 addr, UINT16 pos, UINT16 len, UINT32 value)
{
   UINT32 tmp = *((volatile UINT32 *)addr);
   tmp &=~(((UINT32)0xFFFFFFFF >> (32-len))<<pos);
   tmp |=((value & ((UINT32)0xFFFFFFFF>>(32-len)))<<pos);
   *((volatile UINT32 *)addr) = tmp;
}

static UINT32 getBits(UINT32 addr, UINT16 pos, UINT16 len)
{
   UINT32 reg = *((volatile UINT32 *)addr);
   return ((reg & (((UINT32)0xFFFFFFFF>>(32-len))<<pos)) >> pos);
}

static XMLDEV_fieldT *getFieldEntry(XMLDEV_devH dev, XMLDB_pathE field)
{
   XMLDEV_fieldT *entry= NULL;
   unsigned int offset;

   offset = field - dev->pathMin;
   if(offset < dev->pathRange)
   {
      entry = &dev->regFieldTbl[dev->pathMapTbl[offset]];
   }
   return entry;
}


/*
 *Initialize table mapping from xml paths to indices in register table.
*/
int XMLDEV_init(XMLDEV_devH *devp, const char *name, UINT32 base, XMLDEV_fieldT *regFieldTbl, unsigned int numFields)
{
   unsigned int i;
   xmldevT *dev;

   *devp = (xmldevT *)malloc(sizeof(xmldevT));
   if(!(*devp))
      return -1;

   dev = *devp;
   memset(dev, 0, sizeof(xmldevT));

   //get minimum/max path value and allocate mapping table 
   dev->virtbase = base;
   dev->name = name;
   dev->regFieldTbl = regFieldTbl;
   dev->numFields = numFields;
   dev->pathMax = findMaxPath(dev);
   dev->pathMin = findMinPath(dev);
   dev->pathRange = dev->pathMax-dev->pathMin+1;
   assert(dev->pathRange > 0);
   dev->pathMapTbl =(UINT16 *)malloc(sizeof(UINT16) * dev->pathRange);
   assert(dev->pathMapTbl);

   //initialize table with (-1)
   memset(dev->pathMapTbl, (UINT16)~0, dev->pathRange);

   //Fill table - for each entry in the register table, store its index in the at offset from min: path-min
   for(i = 0; i < dev->numFields; i++)
   {

      if(dev->regFieldTbl[i].path == XMLDEV_NA_PATH)
         continue;
      unsigned int offset;
      offset = dev->regFieldTbl[i].path - dev->pathMin;
      dev->pathMapTbl[offset] = i;
   }
   LOGG_PRINT(LOG_DEBUG_E,NULL,"xml_dev init done %p %s %d fields paths [%d,%d] %d: \n",*devp, dev->name, dev->numFields,dev->pathMin,dev->pathMax,dev->pathRange);
   return 0;
}

void XMLDEV_deinit(XMLDEV_devH dev)
{
   if(dev->pathMapTbl)
      free(dev->pathMapTbl);
   free(dev);
}

void XMLDEV_writeField(XMLDEV_devH dev, XMLDB_pathE field, UINT32 value)
{
   XMLDEV_fieldT *entry = getFieldEntry(dev, field);
   if(!entry)
      return;

   if(entry->offset == XMLDEV_NA_REG)
      return;
      
   setBits(dev->virtbase + entry->offset, entry->pos, entry->len, value);
}

UINT32 XMLDEV_readField(XMLDEV_devH dev, XMLDB_pathE field)
{
   XMLDEV_fieldT *entry = getFieldEntry(dev, field);
   if((!entry) || (entry->offset == XMLDEV_NA_REG))
      return 0;

   return getBits(dev->virtbase + entry->offset , entry->pos, entry->len);
}

int XMLDEV_writeDb(XMLDEV_devH dev, XMLDB_dbH dbh)
{
   ERRG_codeE retx = RET_SUCCESS;
   UINT32 value = 0;
   int ret = 0;

   LOGG_PRINT(LOG_DEBUG_E,0,"dev %s: write db begin \n", dev->name);

   unsigned int i;

   for(i = 0; i < dev->numFields; i++)
   {
      retx = XMLDB_getValue(dbh, dev->regFieldTbl[i].path, &value);
      
      if(ERRG_SUCCEEDED(retx))
      {
         if ((dev->regFieldTbl[i].path != XMLDEV_NA_PATH))
         {
            LOGG_PRINT(LOG_DEBUG_E,0,"f=%d reg=%x %d,%d val=%d\n", dev->regFieldTbl[i].path, dev->regFieldTbl[i].offset, 
               dev->regFieldTbl[i].pos, dev->regFieldTbl[i].len, value);
            XMLDEV_writeField(dev, dev->regFieldTbl[i].path, value);
         }
      }
      else if(retx != XMLDB__EMPTY_ENTRY)
      {
         ret = -1; //some error
         break;
      }
   }

   if(ERRG_SUCCEEDED(ret))
      LOGG_PRINT(LOG_DEBUG_E,0,"dev %s write db done\n", dev->name);
   else
      LOGG_PRINT(LOG_ERROR_E, ret ,"dev %s: write db failed\n", dev->name);

   return ret;
}

UINT32 XMLDEV_getBase(XMLDEV_devH dev)
{
   return dev->virtbase;
}

