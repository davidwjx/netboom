/****************************************************************************
 * 
 *   FileName: config.c
 *
 *   Author: Igal Krivoshey, Konstantin Sinyuk
 *
 *   Date:  11/02/2016
 *
 *   Description: support for parsing configuration folder
 *
 ****************************************************************************/


/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "inu2.h"
#include "config.h"
#include <stdint.h>

#include <stdio.h>
#include <assert.h> 
#include <string.h>
#include <errno.h>
#include <time.h>

#if (defined _WIN32) || (defined _WIN64)  
  #define SLESH "\\"
#elif defined(__linux__)
  #define SLESH "//"
#endif  

#ifdef __cplusplus
extern "C" {
#endif

#if 0

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define CONFP_ASSERT(x) (void)(x)
#define CONFP_CONFIG_REGS_FILE_LINE_SIZE (1000)

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

char filePath[1024];
char configFolderFullPath[1024];
char configFileLine[CONFP_CONFIG_REGS_FILE_LINE_SIZE];

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF           ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static void CONFP_setITbl2cReg(UINT32 i2cNum, UINT8 tarAddress, UINT8 accessSize, UINT16 regAddress,  UINT32 regWidth, UINT32 i2cData, UINT32 phase, CONFG_writeRegCbT writeRegCbFunc);
static void CONFP_setTblSocReg(UINT32 addr, UINT32 val, UINT32 phase, CONFG_writeRegCbT writeRegCbFunc);
static void CONFP_setITbWaitReg(UINT32 waitReg, UINT32 phase, CONFG_writeRegCbT writeRegCbFunc);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: CONFP_setITbl2cReg
*
*  Description: Set an I2C register entry in the config table
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
static void CONFP_setITbl2cReg(UINT32 i2cNum, UINT8 tarAddress, UINT8 accessSize, UINT16 regAddress,  UINT32 regWidth, UINT32 i2cData, UINT32 phase, CONFG_writeRegCbT writeRegCbFunc)
{
   inu_device__writeRegT writeReg;

   writeReg.phase = (inu_device__dbPhaseE)phase;
   writeReg.regType = INU_DEVICE__REG_I2C_E;
   writeReg.i2cReg.i2cNum = (inu_device__i2cNumE)i2cNum;
   writeReg.i2cReg.tarAddress = tarAddress;
   writeReg.i2cReg.accessSize = accessSize;
   writeReg.i2cReg.regAddress = regAddress;
   writeReg.i2cReg.regWidth =regWidth;
   writeReg.i2cReg.i2cData = i2cData;

   writeRegCbFunc(&writeReg);
}



/****************************************************************************
*
*  Function Name: CONFP_setITbl2cReg
*
*  Description: Set an I2C register entry in the config table
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
static void CONFP_setITbWaitReg(UINT32 waitReg, UINT32 phase, CONFG_writeRegCbT writeRegCbFunc)
{
   inu_device__writeRegT writeReg;

   writeReg.phase   = (inu_device__dbPhaseE)phase;
   writeReg.regType = INU_DEVICE__REG_WAIT_E;
   writeReg.waitReg.usecWait = waitReg;

   writeRegCbFunc(&writeReg);
}

/****************************************************************************
*
*  Function Name: CONFP_setTblSocReg
*
*  Description: Set an SOC register entry in the config table
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
static void CONFP_setTblSocReg(UINT32 addr, UINT32 val, UINT32 phase, CONFG_writeRegCbT writeRegCbFunc)
{
   inu_device__writeRegT writeReg;

   writeReg.phase       = (inu_device__dbPhaseE)phase;
   writeReg.regType     = INU_DEVICE__REG_SOC_E;
   writeReg.socReg.addr = addr;
   writeReg.socReg.val  = val;

   writeRegCbFunc(&writeReg);
}


/****************************************************************************
*
*  Function Name: CONFP_getFileSize
*
*  Description:
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
/*
static int CONFP_getFileSize(const char *name)
{
   FILE *file;
   int size=0;

    file = fopen(name,"rb");  // r for read, b for binary
    if (file)
    {
       fseek(file, 0, SEEK_END);
       size=ftell(file);
       fseek(file, 0, SEEK_SET);
      fclose(file);
    }
    else
    {
        printf("ERROR: can't open %s\n",name);
    }
   return size;
}*/


/****************************************************************************
*
*  Function Name: CONFP_readBinFile
*
*  Description:
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
/*
static int CONFP_readBinFile(const char *name, char *buffer, int size)
{
    FILE *file;   

    file = fopen(name,"rb");  // r for read, b for binary
    if (!file)    
    {
        printf("ERROR: can't open %s\n",name);
      return 1;
    }
      
    fread(buffer, size, 1, file);
      fclose(file);
   
   return 0;
}*/


/****************************************************************************
*
*  Function Name: CONFG_loadPreset
*
*  Description: load config
*
*  Inputs: 
*
*  Returns:
*
****************************************************************************/
static int CONFP_loadConfigIReg(CONFG_writeRegCbT writeRegCbFunc, INUG_configMetaT *metaDataP, char *fileName)
{
   FILE *fIn;
   char *str;    
   unsigned int val,reg;
   unsigned int i2cNum, tarAddress, accessSize, regAddress, regWidth, i2cData, waitReg;
   unsigned int phase;
   char i2cMode;

    // Open input file and process line by line.
    if ((fIn = fopen (fileName, "r")) == NULL) {
        fprintf (stderr, "Cannot open %s, errno = %d\n", fileName, errno);
        return 1;
    }

    while (fgets (configFileLine, sizeof (configFileLine), fIn) != NULL) 
    {
    // Check if line was too long.
        if (!feof(fIn)  && configFileLine[strlen (configFileLine) - 1] != '\n') 
        {
            fprintf (stderr, "Line too long: [%s...]\n", configFileLine);
            fclose (fIn);
            return 1;
        }

         // Output the line and start processing it.
         //printf ("%s   ", line);
         str = configFileLine;
         // setup the default phase
         phase = INU_DEVICE__DB_PHASE_1_E;

         // Skip white space and scan first inetegr.
         while (*str == ' ') str++;
         // Check for the comment section
         if (*str == '#') 
         {
            if (strncmp(str,"#*# Video Size",14) == 0) 
            {
               str+=14;
               while (*str == ' ') str++;
               if (sscanf (str, "%hd %hd", &metaDataP->video_width, &metaDataP->video_height) != 2)
               {
                  printf("Can't parse metaDataP line\n");
                  break;
               }
               //printf("Video Size: width=0x%x, height=0x%x\n",metaDataP->video_width, metaDataP->video_height);
            }
            else if (strncmp(str,"#*# Depth Size",14) == 0)
            {
               str+=14;
               while (*str == ' ') str++;
               if (sscanf (str, "%hd %hd", &metaDataP->depth_width, &metaDataP->depth_height) != 2)
               {
                  printf("Can't parse meta line\n");
                  break;
               }
               //printf("Depth Size: width=0x%x, height=0x%x\n",metaDataP->depth_width, metaDataP->depth_height);
            }
            else if (strncmp(str,"#*# Webcam Size",15) == 0) 
            {
               str+=15;
               while (*str == ' ') str++;
               if (sscanf (str, "%hd %hd", &metaDataP->webcam_width, &metaDataP->webcam_height) != 2)
               {
                  printf("Can't parse meta line\n");
                  break;
               }
               //printf("Webcam Size: width=0x%x, height=0x%x\n",metaDataP->webcam_width, metaDataP->webcam_height);
            } 
            else
            {
               //printf("Skipping comments\n");
               continue;
            }
      }
      else if (strncmp(str,"REG",3) == 0)
      {
         str +=3;
         while (*str == ' ') str++;
         if (sscanf (str, "%x = %x %x", &reg,&val,&phase) != 3)
         {
            if (sscanf (str, "%x = %x", &reg,&val) != 2)
            {
               printf("Can't parse REG line\n");
               break;
            }
         }
         //printf("REG: address=0%x, value=0%x\n",reg, val);
         CONFP_setTblSocReg(reg ,val, phase,writeRegCbFunc);
      }
      else if (strncmp(str,"I2C",3) == 0)
      {
         str +=3;
         while (*str == ' ') str++;
         if (sscanf (str, "%d %c %x %d %x %d %x %x", &i2cNum, &i2cMode, &tarAddress, &accessSize, &regAddress, &regWidth, &i2cData, &phase) != 8)
         {
            if (sscanf (str, "%d %c %x %d %x %d %x", &i2cNum, &i2cMode, &tarAddress, &accessSize, &regAddress, &regWidth, &i2cData) != 7)
            {
               printf("Can't parse I2C line\n");
               break;
            }
         }
         //printf("I2C: i2cNum=0x%x, tarAddress=0x%x, accessSize=0x%x, regAddress=0x%x, regWidth=0x%x, i2cData=0x%x\n",i2cNum, tarAddress, accessSize, regAddress, regWidth, i2cData);
         CONFP_setITbl2cReg(i2cNum, (UINT8)tarAddress, (UINT8)accessSize, (UINT16)regAddress, regWidth, i2cData, phase,writeRegCbFunc);
      }
      else if (strncmp(str,"WAIT",4) == 0)
      {
          str +=4;
          while (*str == ' ') str++;   
          if (sscanf (str, "%d %x", &waitReg, &phase) != 2)
          {
             if (sscanf (str, "%d ", &waitReg) != 1)
             {
                printf("Can't parse WAIT line\n");
                break;
             }
          }
          //printf("WAIT: waitReg= %d\n", waitReg);
          CONFP_setITbWaitReg(waitReg, phase,writeRegCbFunc);
      }
    }
    // Close input file and exit.
    fclose (fIn);
    return 0;

}

/****************************************************************************
*
*  Function Name: CONFP_getOdpData
*
*  Description: load config
*
*  Inputs: 
*
*  Returns:
*
****************************************************************************/
static int CONFP_getOdpData(char *fileName, INUG_configMetaT *metaDataP)
{
   FILE *fIn;
   char *str; 

   // Open input file and process line by line.
   if ((fIn = fopen (fileName, "r")) == NULL) {
       fprintf (stderr, "Cannot open %s, errno = %d\n", fileName, errno);
       return 1;
   }

   while (fgets (configFileLine, sizeof (configFileLine), fIn) != NULL) 
   {
       // Check if line was too long.
     if (!feof(fIn)  && configFileLine[strlen (configFileLine) - 1] != '\n') 
     {
         fprintf (stderr, "Line too long: [%s...]\n", configFileLine);
         fclose (fIn);
         return 1;
     }
   
      // Output the line and start processing it.
      //printf ("%s   ", line);
      str = configFileLine;
   
      // Skip white space and scan first inetegr.
      while (*str == ' ') str++;
         
      if (strncmp(str,"fc_left=",8) == 0)
      {
         str +=8;
         while (*str == ' ') str++;
         metaDataP->fc_left = atof(str);   
      }

      if (strncmp(str,"cc_left_x=",10) == 0)
      {
         str +=10;
         while (*str == ' ') str++;
         metaDataP->cc_left_x = atof(str); 
      }

      if (strncmp(str,"cc_left_y=",10) == 0)
      {
         str +=10;
         while (*str == ' ') str++;
         metaDataP->cc_left_y = atof(str); 
      }

      if (strncmp(str,"fc_right=",9) == 0)
      {
         str +=9;
         while (*str == ' ') str++;
         metaDataP->fc_right = atof(str);  
      }

      if (strncmp(str,"cc_right_x=",11) == 0)
      {
         str +=11;
         while (*str == ' ') str++;
         metaDataP->cc_right_x = atof(str);
      }

      if (strncmp(str,"cc_right_y=",11) == 0)
      {
         str +=11;
         while (*str == ' ') str++;
         metaDataP->cc_right_y = atof(str);
      }

      if (strncmp(str,"T=",2) == 0)
      {
         str +=2;
         while (*str == ' ') str++;
         metaDataP->T = atof(str);
      }

      if (strncmp(str,"Nx=",3) == 0)
      {
         str +=3;
         while (*str == ' ') str++;
         metaDataP->Nx = atof(str);
      }

      if (strncmp(str,"Ny=",3) == 0)
      {
         str +=3;
         while (*str == ' ') str++;
         metaDataP->Ny = atof(str);
      }

   }

   // Close input file and exit.
   fclose (fIn);
   return 0;
}


/****************************************************************************
*
*  Function Name: CONFP_loadLut
*
*  Description: load lut
*
*  Inputs: 
*
*  Returns:
*
****************************************************************************/
/*static int CONFP_loadLut(CONFG_writeBufCbT WriteBufCbFunc, INU_DEFSG_iaeLutIdE lutId, char lutFilePath[])
{
   char  *lutPtr = NULL;
   int   lutSize = 0;
   int   lutFileSize = CONFP_getFileSize(lutFilePath);
   ERRG_codeE status;

   if (lutFileSize)
   {
      lutSize=sizeof(INU_DEFSG_lutHeaderT) + lutFileSize;
      lutPtr=(char* )malloc(lutSize);
      if (CONFP_readBinFile(lutFilePath, lutPtr + sizeof(INU_DEFSG_lutHeaderT), lutFileSize))
         return 1;
      ((INU_DEFSG_lutHeaderT *)lutPtr)->lutId = lutId;
      status = WriteBufCbFunc(lutPtr, lutSize);
      free(lutPtr);
      if(ERRG_FAILED(status))
         return 2;
   }
   return 0;
}*/


/****************************************************************************
*
*  Function Name: CONFG_loadConfig
*
*  Description: parses and load configIreg, load LUTS and parse ODP data
*
*  Inputs: 
*
*  Returns:
*
****************************************************************************/
int CONFG_loadConfig(CONFG_writeRegCbT writeRegCbFunc, CONFG_writeBufCbT WriteBufCbFunc, char *configFolder, CONFG_modeE mode, INUG_configMetaT *metaData)
{
   char configMode[50];
   (void)WriteBufCbFunc;

   switch (mode)
   {
      case CONFG_BINNNG:
         strcpy(configMode, "Binning");
         break;
      case CONFG_FULL:
         strcpy(configMode, "Full");
         break;
      case CONFG_ALTERNATING:
         strcpy(configMode, "Alternating");
         break;
      case CONFG_USER:
         strcpy(configMode, "user");
         break;
   }
   strcpy(configFolderFullPath, configFolder);
   strcat(configFolderFullPath,SLESH"Rev001");
   strcat(configFolderFullPath,SLESH);
   strcat(configFolderFullPath,configMode);
   strcat(configFolderFullPath,SLESH"HW");

   strcpy(filePath, configFolderFullPath);
   strcat(filePath,SLESH"iae_ib_left.bin");
/*   if (CONFP_loadLut(WriteBufCbFunc, INU_DEFSG_IAE_LUT_IB_0_E,  filePath))
      return 1;
   strcpy(filePath, configFolderFullPath);
   strcat(filePath,SLESH"iae_ib_right.bin");
   if (CONFP_loadLut(WriteBufCbFunc, INU_DEFSG_IAE_LUT_IB_1_E,  filePath))
      return 2;
   strcpy(filePath, configFolderFullPath);
   strcat(filePath,SLESH"iae_dsr_left.bin");
   if (CONFP_loadLut(WriteBufCbFunc, INU_DEFSG_IAE_LUT_DSR_0_E, filePath))
      return 3;
   strcpy(filePath, configFolderFullPath);
   strcat(filePath,SLESH"iae_dsr_right.bin");
   if (CONFP_loadLut(WriteBufCbFunc, INU_DEFSG_IAE_LUT_DSR_1_E, filePath))
      return 4;*/
   strcpy(filePath, configFolderFullPath);
   strcat(filePath,SLESH"config.ireg");
   if (CONFP_loadConfigIReg(writeRegCbFunc, metaData, filePath))
      return 5;
   strcpy(filePath, configFolderFullPath);
   strcat(filePath,SLESH"odp_data.txt");
   if (CONFP_getOdpData(filePath, metaData))
      return 5;

   return 0;
}
#endif
#ifdef __cplusplus
}
#endif
