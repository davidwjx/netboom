
#include "inu_common.h"
#include "xml_db.h"
#include "nucfg.h"
#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//nucfg unit test
//  load/open a an xml file.
//  call init function with xml
//  output the "working" xml 


#define _XML_EXT(p,e)  p ## e
#define XML_EXT(p)  _XML_EXT(p,".xml")

#define XML_FILE_NAME_IN       "../../nucfg/nu4k"
#define XML_FILE_NAME_OUT      "../../nucfg/nu4k_meta"
#define XML_FILE_NAME_IN_EXT   XML_EXT("../../nucfg/nu4k")
#define XML_FILE_NAME_OUT_EXT  XML_EXT("../../nucfg/nu4k_meta")
#define CALIB_FILE_NAME       "../../nucfg/calib.ini"
//#define CALIB_FILE_NAME       ""

#define _XML_OUT_GROUP_MODE(n,g,m)  n ## "_g" #g ## "_m" ## #m
#define XML_OUT_GROUP_MODE(g,m)      _XML_OUT_GROUP_MODE(XML_FILE_NAME_OUT, g, m)
#define XML_OUT_GROUP_MODE_EXT(g,m)  XML_EXT(XML_OUT_GROUP_MODE(g,m))

//#define TEST_GROUP_MODES
#ifdef TEST_GROUP_MODES
#define GROUP_0       0
#define GROUP_0_MODE  0
#define GROUP_1       0
#define GROUP_1_MODE  1
#endif


static int loadCalib(const char *path, CALIB_sectionDataT **tbl, unsigned int *numSections)
{
   ERRG_codeE err;

   if(strlen(path) == 0)
   {
      *numSections = 0;
      *tbl = NULL;
      return (0);
   }
   err = CALIBG_getCalibData((char **)path, tbl, (int *)numSections);
   if(ERRG_FAILED(err))
      return -1;

   return 0;
}

void freeCalib(CALIB_sectionDataT *tbl)
{
   if(tbl)
   {
      CALIBG_freeCalibArr(tbl);
   }
}

static int loadNuSocxml(const char *infile, char **xmlbuf, unsigned int *xmlsize)
{
   int ret = 0;
   char *buf = NULL;
   unsigned int size;
   //open and load xml file
   FILE *xmlfile = fopen(infile, "r");
   if (!xmlfile)
      return -1;

   fseek(xmlfile, 0L, SEEK_END);
   size = ftell(xmlfile);
   fseek(xmlfile, 0L, SEEK_SET);

   buf = (char *)malloc(size + 1);
   if (buf)
   {
      size_t r = fread(buf, size, 1, xmlfile);
      if ((r != 1) && (!feof(xmlfile)))
      {
         printf("read file error %s\n", strerror(errno));
         ret = -1;
         free(buf);
      }
      buf[size] = '\0';//otherwise sxmlc parser chokes
   }
   else
   {
      ret = -1;
   }

   if (ret >= 0)
   {
      *xmlbuf = buf;
      *xmlsize = size;
   }

   fclose(xmlfile);
   return ret;
}
#if 0
static int writeXml(const char *xmlbuf, const char *name, size_t size)
{
   int ret = 0;
   FILE *xmlfile = fopen(name, "w");
   if(xmlfile)
   {
      if(fwrite(xmlbuf, size, 1, xmlfile) == 1)
      {
         fflush(xmlfile);
         fclose(xmlfile);
      }
      else
         ret = -1; //failed write
   }
   else
      ret = -1;

   return ret;
}

static ERRG_codeE saveDb(XMLDB_dbH db, const char *xmlbuf, unsigned int *outSize, char **outBuf)
{
   ERRG_codeE ret;
   char *buf = NULL;
   unsigned int size;

   //first pass - get size of required buffer
   ret = XMLDB_saveToBuf(db, xmlbuf, NULL, &size);
   if (ERRG_SUCCEEDED(ret))
   {
      buf = (char *)malloc(size);
      if (!buf)
         ret = XMLDB__ERR_OUT_OF_RSRCS;
   }

   //second pass - save the buffer as xml to the same buffer 
   if (ERRG_SUCCEEDED(ret))
   {
      ret = XMLDB_saveToBuf(db, xmlbuf, buf, NULL);
   }

   if (ERRG_SUCCEEDED(ret))
   {
      *outBuf = buf;
      *outSize = size;
   }
   else
   {
      if (buf)
         free(buf);
   }

   return ret;
}
#endif
static int outputDb(char **xmlbuf, char *outname)
{
   ERRG_codeE ret;
   inu_nucfgH nucfgH;
   ret = NUCFG_saveModDbHAndWriteXml(nucfgH, *xmlbuf, outname);
   if (ERRG_FAILED(ret))
      return -1;

   return 0;
}

int main(void)
{
   int main_ret = 1;
   ERRG_codeE ret;
   unsigned int xmlsize;
   char *xmlbuf = NULL;   
   CALIB_sectionDataT *caltbl = NULL;
   NUCFG_calibT calib;
   NUCFG_calibT *calibp = NULL;
   inu_nucfgH cfgH;

   printf("nucfg_test: \n");
   printf("   input : %s\n", XML_FILE_NAME_IN_EXT);

//   XMLDB_init();

   if(loadNuSocxml(XML_FILE_NAME_IN_EXT, &xmlbuf, &xmlsize) < 0)
   {
      printf("error: failed load xml file.exiting\n");
      goto err_exit;
   }

   if(loadCalib(CALIB_FILE_NAME, &caltbl, &calib.numSections) < 0)
   {
      printf("error: failed to load calibration file %s. exiting\n",CALIB_FILE_NAME);
      goto err_exit;
   }
   else
   {
      calib.tbl = caltbl;
      calibp = &calib;
   }

   ret = NUCFG_init(calibp, xmlbuf, XML_FILE_NAME_IN_EXT, "test", &cfgH);
   if(ERRG_FAILED(ret))
   {
      printf("error: failed xml parsing. exiting\n");
      goto err_exit;
   }

   NUCFG_showChannelGroup(cfgH, LOG_INFO_E, NUCFG_CHAN_GROUP_ANY);
   if(outputDb(&xmlbuf, XML_FILE_NAME_OUT_EXT) < 0)
   {  
      printf("exiting\n");
      goto err_exit;
   }

#ifdef TEST_GROUP_MODES
   printf("set mode group %d mode %d \n",GROUP_0,GROUP_0_MODE);
   NUCFG_setGroupMode(cfgH, GROUP_0, GROUP_0_MODE);
   NUCFG_showChannelGroup(cfgH, LOG_INFO_E, GROUP_0);
   if(outputDb(&xmlbuf, XML_OUT_GROUP_MODE_EXT(GROUP_0,GROUP_0_MODE)) < 0)
   {  
      printf("exiting\n");
      goto err_exit;
   }

   printf("set mode group %d mode %d \n",GROUP_1,GROUP_1_MODE);
   NUCFG_setGroupMode(cfgH, GROUP_1, GROUP_1_MODE);
   NUCFG_showChannelGroup(cfgH, LOG_INFO_E, GROUP_1);
   if(outputDb(&xmlbuf, XML_OUT_GROUP_MODE_EXT(GROUP_1,GROUP_1_MODE)) < 0)
   {  
      printf("exiting\n");
      goto err_exit;
   }
#endif
   printf("nucfg_test done\n");
   main_ret = 0;

err_exit:
   NUCFG_deInit(cfgH);
   freeCalib(caltbl);

   exit(main_ret);
}


#ifdef __cplusplus
}
#endif