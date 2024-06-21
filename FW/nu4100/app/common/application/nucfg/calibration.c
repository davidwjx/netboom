#include "inu_common.h"
#include "calibration.h"
#include "ini.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INI_FILE_NAME            "nu4k_calibration_params.ini"
#define YML_FILE_NAME            "SystemParameters.yml"
#define SYSTEMP_XML_PATH_STR_LEN   1024
#define CALIB_ALL_NECESSARY_INI_FIELDS_FULL  0x7FF3

#if (defined _WIN32) || (defined _WIN64)
#define PATH_SEPARATOR  "\\"
#elif defined(__linux__)
#define PATH_SEPARATOR  "/"
#endif

   typedef enum
   {
      CALIB_SENSOR_OPERATING_MODE_E,
      CALIB_SENSOR_VERTICAL_OFFSET_E,
      CALIB_SENSOR_GAIN_OFFSET_E,
      CALIB_GAIN_OFFSET_PARTNER_E,
      CALIB_VERTICAL_CROP_OFFSET_E,
      CALIB_HORIZONTAL_CROP_OFFSET_E,
      CALIB_SLU_OUTPUT_VERTICAL_SIZE_E,
      CALIB_SLU_OUTPUT_HORIZONTAL_SIZE_E,
      CALIB_IB_VERTICAL_LUT_OFFSET_E,
      CALIB_IB_HORIZONTAL_LUT_OFFSET_E,
      CALIB_IB_OFFSET_E,
      CALIB_DSR_VERTICAL_LUT_OFFSET_E,
      CALIB_DSR_HORIZONTAL_LUT_OFFSET_E,
      CALIB_DSR_OUTPUT_VERTICAL_SIZE_E,
      CALIB_DSR_OUTPUT_HORIZONTAL_SIZE_E,
      CALIB_DSR_VERTICAL_BLOCK_SIZE_E,
      CALIB_DSR_HORIZONTAL_BLOCK_SIZE_E,
      CALIB_DSR_LUT_HORIZONTAL_SIZE_E,
      CALIB_DSR_HORIZONTAL_DELTA_SIZE_E,
      CALIB_DSR_HORIZONTAL_DELTA_FRACTIONAL_BITS_E,
      CALIB_DSR_VERTICAL_DELTA_SIZE_E,
      CALIB_DSR_VERTICAL_DELTA_FRACTIONAL_BITS_E,
      CALIB_IB_VERTICAL_BLOCK_SIZE_E,
      CALIB_IB_HORIZONTAL_BLOCK_SIZE_E,
      CALIB_IB_LUT_HORIZONTAL_SIZE_E,
      CALIB_IB_FIXED_POINT_FRACTIONAL_BITS_E,
      CALIB_SENSOR_ROLE_E,
      CALIB_IB_OFFSET_R_E,
      CALIB_IB_OFFSET_GR_E,
      CALIB_IB_OFFSET_GB_E,
      CALIB_IB_OFFSET_B_E
   } CALIB_sectionDataE;

   static char *sensModeStr[CALIB_NUM_LUT_MODES_E]={"Binning","Full","VerticalBinning"};
   static Calib_YmlSectionParamsT *calibYmlSections[CALIB_NUM_LUT_MODES_E][CALIB_YML_MAX_SECTION_E]={{0},{0},{0}};

   ERRG_codeE CALIB_loadCalibrationData(struct INI* ini, CALIB_sectionDataT* sectionsDataTbl);
   int CALIB_getNumOfSection(struct INI* ini);
   ERRG_codeE CALIB_init(char* path, struct INI** ini);
   void CALIB_fillTbl(CALIB_sectionDataT* sectionsDataTbl, int index, char* key, char* value, UINT32* fieldsFull);
   CALIB_sectionNumE CALIB_getSectionEnum(char * sectionName);
   CALIB_sensorOperatingModeE CALIB_getOpeatingModeEnum(char* value);

char *CALIB_getSensorModeStr(CALIB_sensorLutModeE calibMode)
{
   if (calibMode < CALIB_NUM_LUT_MODES_E)
     return  sensModeStr[calibMode];
   else return NULL;
}

ERRG_codeE CALIB_init(char* path, struct INI** ini)
{
   ERRG_codeE ret = CALIBRATION__RET_SUCCESS;

   *ini = ini_open(path);
   if (!*ini)
      ret = CALIBRATION__ERR_UNEXPECTED;

   return ret;
}

void CALIB_getFullCalibFilePath(char* path, char *fullBinPath, char *fileName, char* fullPath)
{
   strcpy(fullPath, path);
   strcat(fullPath, PATH_SEPARATOR);
   strcat(fullPath, fullBinPath);
   strcat(fullPath, PATH_SEPARATOR);
   strcat(fullPath, "NU4K");
   strcat(fullPath, PATH_SEPARATOR);
   strcat(fullPath, fileName);
}

void CALIBG_ymlGetSectionsDB(CALIB_sensorLutModeE LutMode,CALIB_ymlSectionE section,Calib_YmlSectionParamsT **sectionStart)
{
   *sectionStart =  calibYmlSections[LutMode][section];
}

BOOLEAN CALIBG_ymlGetSectionFcValue(CALIB_sensorLutModeE LutMode,CALIB_ymlSectionE section,float *fx,float *fy)
{
   Calib_YmlSectionParamsT *sectionStart;

   sectionStart =  calibYmlSections[LutMode][section];
   if (sectionStart != NULL)
   {
      if (sectionStart->sensorParams.calibYmlParams[CALIB_YML_VIRTUAL_E][CALIB_YML_FC_E].paramListSize >= 1)
         if (sectionStart->sensorParams.calibYmlParams[CALIB_YML_VIRTUAL_E][CALIB_YML_FC_E].paramList)
         {
            *fx = ((float *)sectionStart->sensorParams.calibYmlParams[CALIB_YML_VIRTUAL_E][CALIB_YML_FC_E].paramList)[0];
            *fy = ((float *)sectionStart->sensorParams.calibYmlParams[CALIB_YML_VIRTUAL_E][CALIB_YML_FC_E].paramList)[1];
           return TRUE;
         }
   }
   return FALSE;
}

BOOLEAN CALIBG_ymlGetSectionCcValue(CALIB_sensorLutModeE LutMode,CALIB_ymlSectionE section,float *cx,float *cy)
{
   Calib_YmlSectionParamsT *sectionStart;

   sectionStart =  calibYmlSections[LutMode][section];
   if (sectionStart != NULL)
   {
      if (sectionStart->sensorParams.calibYmlParams[CALIB_YML_VIRTUAL_E][CALIB_YML_CC_E].paramListSize >= 1)
         if (sectionStart->sensorParams.calibYmlParams[CALIB_YML_VIRTUAL_E][CALIB_YML_CC_E].paramList)
         {
            *cx = ((float *)sectionStart->sensorParams.calibYmlParams[CALIB_YML_VIRTUAL_E][CALIB_YML_CC_E].paramList)[0];
            *cy = ((float *)sectionStart->sensorParams.calibYmlParams[CALIB_YML_VIRTUAL_E][CALIB_YML_CC_E].paramList)[1];
           return TRUE;
         }
   }
   return FALSE;
}

BOOLEAN CALIBG_ymlGetSectionsBLValue(CALIB_sensorLutModeE LutMode,UINT32 sensor0,UINT32 sensor1,float *value)
{
   unsigned int ind,sensor0_inDB,sensor1_inDB;
   char *paramStr;//sensor_0...

   for (ind = CALIB_YML_SECTION_BASELINE_0_E ;ind < CALIB_YML_MAX_SECTION_E;ind++)
   {
      if (calibYmlSections[LutMode][ind] != NULL)
      {
         if (calibYmlSections[LutMode][ind]->baselineParams.calibYmlBaselineParams[CALIB_YML_IMAGERS_E].paramListSize == 2)
         {
             paramStr = (char *)calibYmlSections[LutMode][ind]->baselineParams.calibYmlBaselineParams[CALIB_YML_IMAGERS_E].paramList;

             sensor0_inDB = paramStr[7] - '0';
             sensor1_inDB = paramStr[CALIB_YML_MAX_STR_SIZE + 7] - '0'; // next param sensor_0....
             if (((sensor0_inDB == sensor0) && (sensor1_inDB == sensor1)) ||
               ((sensor1_inDB == sensor0) && (sensor0_inDB == sensor1)))
             {
                if (calibYmlSections[LutMode][ind]->baselineParams.calibYmlBaselineParams[CALIB_YML_VAL_E].paramListSize == 1)
                {
                   *value = ((float *)calibYmlSections[LutMode][ind]->baselineParams.calibYmlBaselineParams[CALIB_YML_VAL_E].paramList)[0];
                    return TRUE;
                }
             }
         }
     }
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "Failed to load baselines for sensors %d %d LutMode %d\n",sensor0 ,sensor1, LutMode);

   return FALSE;
}

void Calib_ymlGetSections(struct INI* ymlIni,char **sectionStart,char **sectionEnd)
{
    unsigned int ind,ind1;
    char *sectionNames[CALIB_YML_MAX_SECTION_E]={"sensor_0:","sensor_1:","sensor_2:","sensor_3:","sensor_4:","sensor_5:","sensor_6:","sensor_7:","baseline_0:","baseline_1","baseline_2:","baseline_3:"};
    char *sectionEntry;

    memset(sectionStart,0,CALIB_YML_MAX_SECTION_E*sizeof(char *));
    memset(sectionEnd,0, CALIB_YML_MAX_SECTION_E*sizeof(char *));

    for (ind=0;ind<CALIB_YML_MAX_SECTION_E;ind++)
    {
       sectionEntry = strstr((char *)ymlIni->buf, sectionNames[ind]);
       if (sectionEntry != NULL)
       {
        sectionStart[ind]=sectionEntry;
       }
    }

    for (ind=0;ind<CALIB_YML_MAX_SECTION_E;ind++)
    {
       sectionEnd[ind] = (char *)ymlIni->end;
       if (sectionStart[ind] == NULL)
          continue;
       for (ind1=0;ind1<CALIB_YML_MAX_SECTION_E;ind1++)
       {
          if (sectionStart[ind1] > sectionStart[ind])
          {
             if (sectionStart[ind1] < sectionEnd[ind])
               sectionEnd[ind] = sectionStart[ind1];
          }
       }
    }
}

void CALIB_ymlLoadParams(Calib_YmlParamsT *ymlParams,char *sectorPtr,Calib_YmlParmsDscrsT *ParmsDscrs,unsigned int numParams)
{
   float aa;
   unsigned int ind, count;
   unsigned char lastSavedChar;
   char *ptr, *startData, *workStr, *str, startBracket = '[', endBracket = ']';

   if (sectorPtr != NULL)
   {
      for (ind = 0; ind < numParams; ind++)
      {
         ptr = strstr(sectorPtr, ParmsDscrs[ind].string);
         if (ptr != NULL)
         {
            ptr += strlen(ParmsDscrs[ind].string);
            startData = ptr;
            while ((*ptr != startBracket) && (*ptr != ',') && (*ptr != '}')
                  && (*ptr != 0xd) && (*ptr != 0xa) &&(*ptr !=0))
            {
               ptr++;
            }
            if (*ptr == startBracket)
            {
               startData = ++ptr;
               while (*ptr != endBracket)
               {
                  ptr++;
               }
            }

            lastSavedChar = *ptr;
            *ptr = 0;

            workStr = (char *) malloc(ptr - startData + 1);
            strcpy(workStr, startData);

            str = strtok(workStr, ",");
            count = 0;
            ymlParams[ind].paramList = NULL;
            while (str)
            {
               switch (ParmsDscrs[ind].type) {
               case CALIB_YML_type_float_E:
                  sscanf(str, "%f", &aa);
                  ymlParams[ind].paramList = (float *) realloc(
                        ymlParams[ind].paramList, (count + 1) * sizeof(float));
                  *((float *) ((float *) ymlParams[ind].paramList + count)) =
                        aa;
                  break;
               case CALIB_YML_type_str_E:
                  ymlParams[ind].paramList = (char *) realloc(
                        ymlParams[ind].paramList,
                        (count + 1) * CALIB_YML_MAX_STR_SIZE);
                  while (*str == ' ') str++;
                  strncpy(
                        (char *) ymlParams[ind].paramList
                              + count * CALIB_YML_MAX_STR_SIZE, str,
                        CALIB_YML_MAX_STR_SIZE - 1);
                  break;
               }
               count++;
               str = strtok(NULL, ",");
            }
            ymlParams[ind].paramListSize = count;

            if (workStr)
               free(workStr);
            *ptr = lastSavedChar;
         }
      }
   }
}


void CALIB_parseYmlFile(struct INI* ymlIni,Calib_YmlSectionParamsT **ymlSectionParams,CALIB_sensorLutModeE LutMode,int *NumOfSections)
{
   unsigned int ind,realVirt,currentSection=*NumOfSections;
   char *sectionStart[CALIB_YML_MAX_SECTION_E],*sectionEnd[CALIB_YML_MAX_SECTION_E];
   char *realVirtStr[CALIB_YML_MAX_REALVIRT_E]={"real","virtual"};
   Calib_YmlParmsDscrsT ymlParmsDscrs[CALIB_YML_NUMPARAMS_E]=                     {{"T:"       ,CALIB_YML_type_float_E},
                                                                               {"om:"          ,CALIB_YML_type_float_E},
                                                                               {"alphac:"      ,CALIB_YML_type_float_E},
                                                                               {"cc:"          ,CALIB_YML_type_float_E},
                                                                               {"fc:"          ,CALIB_YML_type_float_E},
                                                                               {"kc:"          ,CALIB_YML_type_float_E},
                                                                               {"model:"       ,CALIB_YML_type_float_E},
                                                                               {"hor_pix_size:",CALIB_YML_type_float_E},
                                                                               {"hor_res:"     ,CALIB_YML_type_float_E},
                                                                               {"ver_pix_size:",CALIB_YML_type_float_E},
                                                                               {"ver_res:"     ,CALIB_YML_type_float_E}};

   Calib_YmlParmsDscrsT ymlBLParmsDscrs[CALIB_YML_BASELINE_NUMPARAMS_E]=       {{"imagers:"   ,CALIB_YML_type_str_E},
                                                                               {"val:"         ,CALIB_YML_type_float_E}};
   Calib_ymlGetSections(ymlIni,sectionStart,sectionEnd);
   for (ind=0;ind<CALIB_YML_MAX_SECTION_E;ind++)
   {
      if (sectionStart[ind] != NULL)
      {
          (*NumOfSections)++;
          *ymlSectionParams = (Calib_YmlSectionParamsT*)realloc(*ymlSectionParams, sizeof(Calib_YmlSectionParamsT)*(*NumOfSections));
          memset(*ymlSectionParams + currentSection,0,sizeof(Calib_YmlSectionParamsT));
          if ( ind >= CALIB_YML_SECTION_BASELINE_0_E)
          {
             CALIB_ymlLoadParams((*ymlSectionParams + currentSection)->baselineParams.calibYmlBaselineParams,sectionStart[ind],ymlBLParmsDscrs,CALIB_YML_BASELINE_NUMPARAMS_E);
          }
          else
          {
             for (realVirt= CALIB_YML_REAL_E;realVirt<CALIB_YML_MAX_REALVIRT_E;realVirt++)
             {
                CALIB_ymlLoadParams((*ymlSectionParams + currentSection)->sensorParams.calibYmlParams[realVirt],strstr(sectionStart[ind],realVirtStr[realVirt]),ymlParmsDscrs,CALIB_YML_NUMPARAMS_E);
             }
          }
          (*ymlSectionParams + currentSection)->sensorParams.sectionNum = (UINT8)ind;
          (*ymlSectionParams + currentSection)->sensorParams.calibMode = (UINT8)LutMode;
           currentSection = (*NumOfSections);
       }
   }
}

ERRG_codeE CALIB_initYmlCalibData(char* path[])
{
   struct INI* ymlIni = NULL;
   int ind;
   ERRG_codeE ret1 = CALIBRATION__RET_SUCCESS,ret = CALIBRATION__RET_SUCCESS;
   Calib_YmlSectionParamsT *ymlSectionParams= NULL;
   int NumOfSections;
   UINT32 i;
   
   NumOfSections = 0;

   for (i = 0; i < CALIB_NUM_LUT_MODES_E; i++)
   {
      char fullIniPath[SYSTEMP_XML_PATH_STR_LEN];

      CALIB_getFullCalibFilePath(path[0],CALIB_getSensorModeStr((CALIB_sensorLutModeE)i),YML_FILE_NAME, fullIniPath);
      ret1 = CALIB_init(fullIniPath, &ymlIni);

      if (ERRG_FAILED(ret1))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Failed to load YML from: %s\n", fullIniPath);
      }
      else
      {
         CALIB_parseYmlFile(ymlIni,&ymlSectionParams,(CALIB_sensorLutModeE)i,&NumOfSections);
         ini_close(ymlIni);
      }

   }
   for (ind = 0;ind < NumOfSections;ind++)
      calibYmlSections[(ymlSectionParams + ind)->sensorParams.calibMode][(ymlSectionParams +ind)->sensorParams.sectionNum] = ymlSectionParams + ind;

   return ret;
}

void CALIB_printYmlData()
{
   float  value;
   UINT32 ind, calib;
   static Calib_YmlSectionParamsT *sectionStart[CALIB_NUM_LUT_MODES_E][CALIB_YML_MAX_SECTION_E] = { {0},{0} };

   for (calib = 0; calib < CALIB_NUM_LUT_MODES_E; calib++)
   {
      for (ind = 0; ind < CALIB_YML_SECTION_BASELINE_0_E; ind++)
      {
         unsigned int ind1, ind2, realVirt;
         CALIBG_ymlGetSectionsDB((CALIB_sensorLutModeE) calib, (CALIB_ymlSectionE) ind, &sectionStart[calib][ind]);
         if (sectionStart[calib][ind] != NULL)
         {
            printf("\n----calib %d sensor %d----\n", calib, ind);
            for (realVirt = 0; realVirt < 2; realVirt++)

            {
               for (ind1 = 0; ind1 < CALIB_YML_NUMPARAMS_E; ind1++)
               {
                  printf("\nrealVirt %d param %d:   ", realVirt, ind1);
                  for (ind2 = 0;ind2 < sectionStart[calib][ind]->sensorParams.calibYmlParams[realVirt][ind1].paramListSize;ind2++)
                  {
                     printf("%f ", *((float *) sectionStart[calib][ind]->sensorParams.calibYmlParams[realVirt][ind1].paramList + ind2));
                  }
               }
            }
         }
      }
      for (ind = CALIB_YML_SECTION_BASELINE_0_E;
            ind <= CALIB_YML_SECTION_BASELINE_2_E; ind++)
      {
         unsigned int ind1, ind2;
         CALIBG_ymlGetSectionsDB((CALIB_sensorLutModeE) calib, (CALIB_ymlSectionE) ind, &sectionStart[calib][ind]);
         if (sectionStart[calib][ind] != NULL)
         {
            printf("\n----calib %d basline %d----\n", calib, ind - CALIB_YML_SECTION_BASELINE_0_E);
            for (ind1 = 0; ind1 < CALIB_YML_BASELINE_NUMPARAMS_E; ind1++)
            {
               printf("param %d:   ", ind1);
               if (ind1 != CALIB_YML_IMAGERS_E)
               {
                  for (ind2 = 0;
                        ind2 < sectionStart[calib][ind]->baselineParams.calibYmlBaselineParams[ind1].paramListSize;
                        ind2++)
                  {
                     printf("%f ", *((float *) sectionStart[calib][ind]->baselineParams.calibYmlBaselineParams[ind1].paramList + ind2));
                  }
               }
               else
               {
                  for (ind2 = 0;
                        ind2 < sectionStart[calib][ind]->baselineParams.calibYmlBaselineParams[ind1].paramListSize;
                        ind2++)
                  {
                     printf("%s ", (char *) sectionStart[calib][ind]->baselineParams.calibYmlBaselineParams[ind1].paramList + ind2 * CALIB_YML_MAX_STR_SIZE);
                  }

               }
            }

         }
      }
   }
   value = 0;
   CALIBG_ymlGetSectionsBLValue(CALIB_LUT_MODE_FULL_E, 1, 0, &value);
   printf("sensors FULL 1,0 value %f\n", value);
   value = 0;
   CALIBG_ymlGetSectionsBLValue(CALIB_LUT_MODE_FULL_E, 3, 2, &value);
   printf("sensors FULL 2,3 value %f\n", value);
   value = 0;
   CALIBG_ymlGetSectionsBLValue(CALIB_LUT_MODE_BIN_E, 1, 0, &value);
   printf("sensors BIN 1,0 value %f\n", value);
   value = 0;
   CALIBG_ymlGetSectionsBLValue(CALIB_LUT_MODE_BIN_E, 3, 2, &value);
   printf("sensors BIN 2,3 value %f\n", value);
  value = 0;
   CALIBG_ymlGetSectionsBLValue(CALIB_LUT_MODE_FULL_E, 3, 2, &value);
   printf("sensors BIN 2,3 value %f\n", value);
value = 0;
   CALIBG_ymlGetSectionsBLValue(CALIB_LUT_MODE_FULL_E, 5, 4, &value);
   printf("sensors BIN 4,5 value %f\n", value);
}

ERRG_codeE CALIBG_getCalibData(char* path[], CALIB_sectionDataT** sectionsDataTbl, int *NumOfSections)
{
   struct INI* ini = NULL;
   int numSection=0, totalNumSections=0;
   ERRG_codeE ret1 = CALIBRATION__RET_SUCCESS,ret = CALIBRATION__RET_SUCCESS;
   UINT32 i;

   CALIB_initYmlCalibData(path);
   //CALIB_printYmlData();
    
   for (i = 0; i < CALIB_NUM_LUT_MODES_E; i++)
   {
      char fullIniPath[SYSTEMP_XML_PATH_STR_LEN];

      CALIB_getFullCalibFilePath(path[0],CALIB_getSensorModeStr((CALIB_sensorLutModeE)i),INI_FILE_NAME, fullIniPath);
      ret1 = CALIB_init(fullIniPath, &ini);
      if (ERRG_FAILED(ret1))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Failed to load Calibration data from: %s\n", fullIniPath);
         continue;
      }
      LOGG_PRINT(LOG_INFO_E, NULL, "Calibration data was taken from: %s\n", fullIniPath);
      numSection = CALIB_getNumOfSection(ini);
      *sectionsDataTbl = (CALIB_sectionDataT*)realloc(*sectionsDataTbl, sizeof(CALIB_sectionDataT)*(totalNumSections+numSection));

      ret = CALIB_loadCalibrationData(ini, *sectionsDataTbl+totalNumSections);
      totalNumSections += numSection;
      if (ERRG_FAILED(ret))
         return ret;
      ini_close(ini);
   }
   *NumOfSections = totalNumSections;
   return ret;
}

CALIB_sensorRoleE CALIB_getSectionRole(char * sectionName)
{
    if (strcmp(sectionName, CALIB_SENSOR_LEFT) == 0)
    {
        return CALIB_ROLE_LEFT_E;
    }
    else if (strcmp(sectionName, CALIB_SENSOR_RIGHT) == 0)
    {
        return CALIB_ROLE_RIGHT_E;
    }
      else
   {
      return CALIB_ROLE_UNKNOWN_E;
   }
}

CALIB_sectionNumE CALIB_getSectionEnum(char * sectionName)
{
   unsigned int ind;
   char sensorStr[24];


   for (ind=0;ind<=CALIB_SECTION_SENSOR_7_PARAMETERS_E;ind++)
   {
       sprintf(sensorStr,"%s%d",CALIB_SENSOR_START_NAME,ind);
       if (strcmp(sectionName, sensorStr) == 0)
        return (CALIB_sectionNumE)(CALIB_SECTION_SENSOR_0_PARAMETERS_E +ind);
   }
   if (strcmp(sectionName, CALIB_MISC) == 0)
   {
      return CALIB_SECTION_MISC_E;
   }
   // backword compatible
   else if (strcmp(sectionName, CALIB_SENSOR_LEFT) == 0)
   {
      return CALIB_SECTION_SENSOR_1_PARAMETERS_E;
   }
   else if (strcmp(sectionName, CALIB_SENSOR_RIGHT) == 0)
   {
      return CALIB_SECTION_SENSOR_0_PARAMETERS_E;
   }
   else
   {
      return CALIB_SECTION_UNKNOWN_E;
   }
}

ERRG_codeE CALIB_loadCalibrationData(struct INI* ini, CALIB_sectionDataT* sectionsDataTbl)
{
   ERRG_codeE ret = CALIBRATION__RET_SUCCESS;

   const char *buf, *buf2;
   char *key, *value, *sectionName;
   int index = 0;
   size_t name_len, key_len, value_len;
   UINT32 fieldsFull = 0;

   while (1)
   {
      int res = ini_next_section(ini, &buf, &name_len);
      if (!res)
         break;
      sectionName = (char*)malloc(name_len + 1);
      if (!sectionName)
         return CALIBRATION__ERR_OUT_OF_RSRCS;
      sectionName[name_len] = '\0';
      memcpy(sectionName, buf, name_len);

      sectionsDataTbl[index].sectionNum = CALIB_getSectionEnum(sectionName);
      // backword compatible, ini file with 'LeftSensorParameters'&'RightSensorParameters' sections
      sectionsDataTbl[index].SensorRole = CALIB_getSectionRole(sectionName);

      //sectionsDataTbl[index].SensorOperatingMode             = INU_DEF_VAL;
      sectionsDataTbl[index].dsrHorizontalBlockSize          = INU_DEFSG_INVALID;
      sectionsDataTbl[index].dsrHorizontalDeltaFractionalBits= INU_DEFSG_INVALID;
      sectionsDataTbl[index].dsrHorizontalDeltaSize          = INU_DEFSG_INVALID;
      sectionsDataTbl[index].dsrHorizontalLutOffset          = INU_DEFSG_INVALID;
      sectionsDataTbl[index].dsrLutHorizontalSize            = INU_DEFSG_INVALID;
      sectionsDataTbl[index].dsrOutputHorizontalSize         = INU_DEFSG_INVALID;
      sectionsDataTbl[index].dsrOutputVerticalSize           = INU_DEFSG_INVALID;
      sectionsDataTbl[index].dsrVerticalBlockSize            = INU_DEFSG_INVALID;
      sectionsDataTbl[index].dsrVerticalDeltaFractionalBits  = INU_DEFSG_INVALID;
      sectionsDataTbl[index].dsrVerticalDeltaSize            = INU_DEFSG_INVALID;
      sectionsDataTbl[index].dsrVerticalLutOffset            = INU_DEFSG_INVALID;
      sectionsDataTbl[index].gainOffsetPartner               = INU_DEFSG_INVALID;
      sectionsDataTbl[index].horizontalCropOffset            = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibFixedPointFractionalBits      = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibHorizontalBlockSize           = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibHorizontalLutOffset           = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibLutHorizontalSize             = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibOffset                        = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibOffsetR                       = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibOffsetGR                      = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibOffsetGB                      = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibOffsetB                       = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibVerticalBlockSize             = INU_DEFSG_INVALID;
      sectionsDataTbl[index].ibVerticalLutOffset             = INU_DEFSG_INVALID;
      sectionsDataTbl[index].sensorGainOffset                = INU_DEFSG_INVALID;
      sectionsDataTbl[index].sensorVerticalOffset            = INU_DEFSG_INVALID;
      sectionsDataTbl[index].sluOutputHorizontalSize         = INU_DEFSG_INVALID;
      sectionsDataTbl[index].sluOutputVerticalSize           = INU_DEFSG_INVALID;
      sectionsDataTbl[index].verticalCropOffset              = INU_DEFSG_INVALID;

      while (1)
      {
         res = ini_read_pair(ini, &buf, &key_len, &buf2, &value_len);
         if (!res)
            break;

         key = (char*)malloc(key_len + 1);
         if (!key)
            return CALIBRATION__ERR_OUT_OF_RSRCS;
         key[key_len] = '\0';
         memcpy(key, buf, key_len);
         value = (char*)malloc(value_len + 1);
         if (!value)
            return CALIBRATION__ERR_OUT_OF_RSRCS;
         value[value_len] = '\0';
         memcpy(value, buf2, value_len);

         CALIB_fillTbl(sectionsDataTbl, index, key, value, &fieldsFull);
         free(key);
         free(value);
      }
      free(sectionName);
      if (!((fieldsFull & CALIB_ALL_NECESSARY_INI_FIELDS_FULL) == CALIB_ALL_NECESSARY_INI_FIELDS_FULL) &&
         ((sectionsDataTbl[index].sectionNum== CALIB_SECTION_SENSOR_1_PARAMETERS_E) || (sectionsDataTbl[index].sectionNum == CALIB_SECTION_SENSOR_0_PARAMETERS_E)))
         return CALIBRATION__ERR_MISSING_FIELDS_IN_INI_FILE;
     fieldsFull = 0;
      index++;
   }
   return ret;
}

CALIB_sensorOperatingModeE CALIB_getOpeatingModeEnum(char* value)
{
   if (strcmp(value, CALIB_MODE_BIN) == 0)
   {
      return CALIB_MODE_BIN_E;
   }
   if (strcmp(value, CALIB_MODE_VERTICAL_BIN) == 0)
   {
      return CALIB_MODE_VERTICAL_BINNING_E;
   }
   else if (strcmp(value, CALIB_MODE_FULL) == 0)
   {
      return CALIB_MODE_FULL_E;
   }
   else if (strcmp(value, CALIB_MODE_VERTIVAL_BINNING) == 0)
   {
      return CALIB_MODE_VERTICAL_BINNING_E;
   }
   else
   {
      return CALIB_MODE_UNKNOWN;
   }
}

CALIB_sensorRoleE CALIB_getRoleEnum(char* value)
{
    if (strcmp(value, CALIB_ROLE_LEFT) == 0)
    {
        return CALIB_ROLE_LEFT_E;
    }
    else if (strcmp(value, CALIB_ROLE_RIGHT) == 0)
    {
        return CALIB_ROLE_RIGHT_E;
    }
    else
    {
        return CALIB_ROLE_UNKNOWN_E;
    }
}
void CALIB_fillTbl(CALIB_sectionDataT* sectionsDataTbl, int index, char* key, char* value, UINT32* fieldsFull)
{
   if (strstr(key, CALIB_SENSOR_OPERATION_MODE) != NULL)
   {
      sectionsDataTbl[index].SensorOperatingMode = CALIB_getOpeatingModeEnum(value);
      *fieldsFull |= (1 << CALIB_SENSOR_OPERATING_MODE_E);
   }
   if (strstr(key, CALIB_SENSOR_ROLE) != NULL)
   {
       sectionsDataTbl[index].SensorRole = CALIB_getRoleEnum(value);
       *fieldsFull |= (1 << CALIB_SENSOR_ROLE_E);
   }
   else if (strstr(key, CALIB_SENSOR_VERTICAL_OFFSET) != NULL)
   {
      sectionsDataTbl[index].sensorVerticalOffset = atoi(value);
      *fieldsFull |= (1 << CALIB_SENSOR_VERTICAL_OFFSET_E);
   }
   else if (strstr(key, CALIB_SENSOR_GAIN_OFFSET) != NULL)
   {
      sectionsDataTbl[index].sensorGainOffset = atoi(value);
      *fieldsFull |= (1 << CALIB_SENSOR_GAIN_OFFSET_E);
   }
   else if (strstr(key, CALIB_GAIN_OFFSET_PARTNER) != NULL)
   {
      sectionsDataTbl[index].gainOffsetPartner = atoi(value);
      *fieldsFull |= (1 << CALIB_GAIN_OFFSET_PARTNER_E);
   }
   else if (strstr(key, CALIB_VERTICAL_CROP_OFFSET) != NULL)
   {
      sectionsDataTbl[index].verticalCropOffset = atoi(value);
      *fieldsFull |= (1 << CALIB_VERTICAL_CROP_OFFSET_E);
   }
   else if (strstr(key, CALIB_HORIZONTAL_CROP_OFFSET) != NULL)
   {
      sectionsDataTbl[index].horizontalCropOffset = atoi(value);
      *fieldsFull |= (1 << CALIB_HORIZONTAL_CROP_OFFSET_E);
   }
   else if (strstr(key, CALIB_SLU_OUTPUT_VERTICAL_SIZE) != NULL)
   {
      sectionsDataTbl[index].sluOutputVerticalSize = atoi(value);
      *fieldsFull |= (1 << CALIB_SLU_OUTPUT_VERTICAL_SIZE_E);
   }
   else if (strstr(key, CALIB_SLU_OUTPUT_HORIZONTAL_SIZE) != NULL)
   {
      sectionsDataTbl[index].sluOutputHorizontalSize = atoi(value);
      *fieldsFull |= (1 << CALIB_SLU_OUTPUT_HORIZONTAL_SIZE_E);
   }
   else if (strstr(key, CALIB_IB_VERTICAL_LUT_OFFSET) != NULL)
   {
      sectionsDataTbl[index].ibVerticalLutOffset = atoi(value);
      *fieldsFull |= (1 << CALIB_IB_VERTICAL_LUT_OFFSET_E);
   }
   else if (strstr(key, CALIB_IB_HORIZONTAL_LUT_OFFSET) != NULL)
   {
      sectionsDataTbl[index].ibHorizontalLutOffset = atoi(value);
      *fieldsFull |= (1 << CALIB_IB_HORIZONTAL_LUT_OFFSET_E);
   }
   else if ((strcmp(key, CALIB_IB_OFFSET) == 0) || (strcmp(key, CALIB_RIGHT_IB_OFFSET) == 0) || (strcmp(key, CALIB_LEFT_IB_OFFSET) == 0))
   {
      sectionsDataTbl[index].ibOffset = atoi(value);
      *fieldsFull |= (1 << CALIB_IB_OFFSET_E);
   }
   else if (strcmp(key, CALIB_IB_OFFSET_R) == 0)
   {
       sectionsDataTbl[index].ibOffsetR = atoi(value);
       *fieldsFull |= (1 << CALIB_IB_OFFSET_R_E);
   }
   else if (strcmp(key, CALIB_IB_OFFSET_GR) == 0)
   {
       sectionsDataTbl[index].ibOffsetGR = atoi(value);
       *fieldsFull |= (1 << CALIB_IB_OFFSET_GR_E);
   }
   else if (strcmp(key, CALIB_IB_OFFSET_GB) == 0)
   {
       sectionsDataTbl[index].ibOffsetGB = atoi(value);
       *fieldsFull |= (1 << CALIB_IB_OFFSET_GB_E);
   }
   else if (strcmp(key, CALIB_IB_OFFSET_B) == 0)
   {
       sectionsDataTbl[index].ibOffsetB = atoi(value);
       *fieldsFull |= (1 << CALIB_IB_OFFSET_B_E);
   }
   else if (strstr(key, CALIB_DSR_VERTICAL_LUT_OFFSET) != NULL)
   {
      sectionsDataTbl[index].dsrVerticalLutOffset = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_VERTICAL_LUT_OFFSET_E);
   }
   else if (strstr(key, CALIB_DSR_HORIZONTAL_LUT_OFFSET) != NULL)
   {
      sectionsDataTbl[index].dsrHorizontalLutOffset = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_HORIZONTAL_LUT_OFFSET_E);
   }
   else if (strstr(key, CALIB_DSR_OUTPUT_VERTICAL_SIZE) != NULL)
   {
      sectionsDataTbl[index].dsrOutputVerticalSize = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_OUTPUT_VERTICAL_SIZE_E);
   }
   else if (strstr(key, CALIB_DSR_OUTPUT_HORIZONTAL_SIZE) != NULL)
   {
      sectionsDataTbl[index].dsrOutputHorizontalSize = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_OUTPUT_HORIZONTAL_SIZE_E);
   }
   else if (strstr(key, CALIB_DSR_VERTICAL_BLOCK_SIZE) != NULL)
   {
      sectionsDataTbl[index].dsrVerticalBlockSize = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_VERTICAL_BLOCK_SIZE_E);
   }
   else if (strstr(key, CALIB_DSR_HORIZONTAL_BLOCK_SIZE) != NULL)
   {
      sectionsDataTbl[index].dsrHorizontalBlockSize = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_HORIZONTAL_BLOCK_SIZE_E);
   }
   else if (strstr(key, CALIB_DSR_LUT_HORIZONTAL_SIZE) != NULL)
   {
      sectionsDataTbl[index].dsrLutHorizontalSize = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_LUT_HORIZONTAL_SIZE_E);
   }
   else if (strstr(key, CALIB_DSR_HORIZONTAL_DELTA_SIZE) != NULL)
   {
      sectionsDataTbl[index].dsrHorizontalDeltaSize = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_HORIZONTAL_DELTA_SIZE_E);
   }
   else if (strstr(key, CALIB_DSR_HORIZONTAL_DELTA_FRACTIONAL_BITS) != NULL)
   {
      sectionsDataTbl[index].dsrHorizontalDeltaFractionalBits = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_HORIZONTAL_DELTA_FRACTIONAL_BITS_E);
   }
   else if (strstr(key, CALIB_DSR_VERTICAL_DELTA_SIZE) != NULL)
   {
      sectionsDataTbl[index].dsrVerticalDeltaSize = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_VERTICAL_DELTA_SIZE_E);
   }
   else if (strstr(key, CALIB_DSR_VERTICAL_DELTA_FRACTIONAL_BITS) != NULL)
   {
      sectionsDataTbl[index].dsrVerticalDeltaFractionalBits = atoi(value);
      *fieldsFull |= (1 << CALIB_DSR_VERTICAL_DELTA_FRACTIONAL_BITS_E);
   }
   else if (strstr(key, CALIB_IB_VERTICAL_BLOCK_SIZE) != NULL)
   {
      sectionsDataTbl[index].ibVerticalBlockSize = atoi(value);
      *fieldsFull |= (1 << CALIB_IB_VERTICAL_BLOCK_SIZE_E);
   }
   else if (strstr(key, CALIB_IB_HORIZONTAL_BLOCK_SIZE) != NULL)
   {
      sectionsDataTbl[index].ibHorizontalBlockSize = atoi(value);
      *fieldsFull |= (1 << CALIB_IB_HORIZONTAL_BLOCK_SIZE_E);
   }
   else if (strstr(key, CALIB_IB_LUT_HORIZONTAL_SIZE) != NULL)
   {
      sectionsDataTbl[index].ibLutHorizontalSize = atoi(value);
      *fieldsFull |= (1 << CALIB_IB_LUT_HORIZONTAL_SIZE_E);
   }
   else if (strstr(key, CALIB_IB_FIXED_POINT_FRACTIONAL_BITS) != NULL)
   {
      sectionsDataTbl[index].ibFixedPointFractionalBits = atoi(value);
      *fieldsFull |= (1 << CALIB_IB_FIXED_POINT_FRACTIONAL_BITS_E);
   }
   else
   {
      //PUT DEFAULT VALUE
   }
}

int CALIB_getNumOfSection(struct INI* ini)
{
   size_t name_len;
   const char *buf;
   int count = 0;
   int res = 1;
   const char* iniCurrent;

   iniCurrent = ini->curr;
   while (res)
   {
      res = ini_next_section(ini, &buf, &name_len);
      count++;
   }
   ini->curr = iniCurrent;
   return count - 1;
}

void CALIBG_freeCalibArr(CALIB_sectionDataT* sectionsDataTbl)
{
   free(sectionsDataTbl);
}

#ifdef __cplusplus
}
#endif
