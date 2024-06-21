#include "inu_common.h"
#include "inu2.h"

#include <errno.h>
#include "cireg.h"


void CIREG_setI2cReg(inu_device__writeRegT *regP,
                             UINT32 i2cNum,
                             UINT8 tarAddress,
                             UINT8 accessSize,
                             UINT16 regAddress,
                             UINT32 regWidth,
                             UINT32 i2cData,
                             inu_device__dbPhaseE phase)
{
   regP->phase = (inu_device__dbPhaseE)phase;
   regP->regType = INU_DEVICE__REG_I2C_E;
   regP->i2cReg.i2cNum = (inu_device__i2cNumE)i2cNum; 
   regP->i2cReg.tarAddress = tarAddress;
   regP->i2cReg.accessSize = accessSize;
   regP->i2cReg.regAddress = regAddress;
   regP->i2cReg.regWidth =regWidth;
   regP->i2cReg.i2cData = i2cData;
}

void CIREG_setWaitReg(inu_device__writeRegT *regP, UINT32 waitReg, inu_device__dbPhaseE phase)
{
   regP->phase = (inu_device__dbPhaseE)phase;
   regP->regType = INU_DEVICE__REG_WAIT_E;
   regP->waitReg.usecWait = waitReg;
}

void CIREG_setSocReg(inu_device__writeRegT *regP,UINT32 addr, UINT32 val, inu_device__dbPhaseE phase)
{
   regP->phase = (inu_device__dbPhaseE)phase;
   regP->regType = INU_DEVICE__REG_SOC_E;
   regP->socReg.addr= addr;
   regP->socReg.val= val;
}

ERRG_codeE CIREG_parse(char *fileName, inu_device__dbPhaseE phase, inu_device__writeRegT *regTbl, UINT32 regTblSize, CIREG_metaT *metaP)
{
   ERRG_codeE ret = CIREG__RET_SUCCESS;
   char line[100];
   FILE *fIn;
   char *str;
   unsigned int index = 0;
   UINT32 val,reg;
   UINT32 i2cNum, tarAddress, accessSize, regAddress, regWidth, i2cData, waitReg;
   char i2cMode;

    // Open input file and process line by line.
    if ((fIn = fopen (fileName, "r")) == NULL) {
        LOGG_PRINT(LOG_DEBUG_E, NULL, "Cannot open %s, error %s\n", fileName, strerror(errno));
        return CIREG__ERR_INVALID_ARGS;
    }
    
    LOGG_PRINT(LOG_INFO_E, NULL, "open file: %s\n", fileName);

    while (fgets (line, sizeof (line), fIn) != NULL) 
    {
       // Check if line was too long.or
       if ((line[strlen (line) - 1] != '\n') && (!feof(fIn)))
       {
           LOGG_PRINT(LOG_ERROR_E,NULL,"Line too long: [%s...]\n", line);
           ret = CIREG__ERR_INVALID_ARGS;
           break;
       }

       // Output the line and start processing it.
       str = line;

       // Skip white space and scan first ineteger.
       while (*str == ' ') str++;
       // Check for the comment section
       if (*str == '#') 
       {
          if (strncmp(str,"#*# Video Size",14) == 0) 
          {
              str+=14;
              while (*str == ' ') str++;
              if(metaP)
              {
                 if (sscanf (str, "%d %d", &metaP->iaeWidth, &metaP->iaeHeight) != 2)
                 {
                    LOGG_PRINT(LOG_ERROR_E, NULL, "Can't parse meta line\n");
                    ret = CIREG__ERR_INVALID_ARGS;
                    break;
                 }
                 LOGG_PRINT(LOG_DEBUG_E, 0, "Video Size: width=0x%x, height=0x%x\n",metaP->iaeWidth, metaP->iaeHeight);
              }
         }
         else 
         {
            LOGG_PRINT(LOG_DEBUG_E,0,"Skipping comments\n");
            continue;
         }
      }
      else if (strncmp(str,"REG",3) == 0)
      {
         str +=3;
         while (*str == ' ') str++;
         if (sscanf (str, "%x = %x", &reg,&val) != 2)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Can't parse REG line\n");
            ret = CIREG__ERR_INVALID_ARGS;
            break;
         }
         LOGG_PRINT(LOG_DEBUG_E,0,"REG: %p=0x%08x\n", reg, val);

         if(index < regTblSize)
         {
            CIREG_setSocReg(&regTbl[index], reg ,val, phase);
            index++;
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "discard reg %p (max %d)\n", reg, regTblSize);
         }
      }
      else if (strncmp(str,"I2C",3) == 0)
      {
         str +=3;
         while (*str == ' ') str++;
         if (sscanf (str, "%d %c %x %d %x %d %x", &i2cNum, &i2cMode, &tarAddress, &accessSize, &regAddress, &regWidth, &i2cData) != 7)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL,"Can't parse I2C line\n");
            ret = CIREG__ERR_INVALID_ARGS;
            break;
         }
         
         LOGG_PRINT(LOG_DEBUG_E, 0,"I2C: i2cNum=0x%x, tarAddress=0x%x, accessSize=0x%x, regAddress=0x%x, regWidth=0x%x, i2cData=0x%x\n",
            i2cNum, tarAddress, accessSize, regAddress, regWidth, i2cData);

         if(index < regTblSize)
         {
            CIREG_setI2cReg(&regTbl[index], i2cNum, tarAddress, accessSize, regAddress, regWidth, i2cData, phase);
            index++;
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "exceeded reg table size %d\n", index);
         }
      }
      else if (strncmp(str,"WAIT",4) == 0)
      {
         str +=4;
         while (*str == ' ') str++;   
         if (sscanf (str, "%d ", &waitReg) != 1)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL,"Can't parse WAIT line\n");
            ret = CIREG__ERR_INVALID_ARGS;
            break;
         }

         LOGG_PRINT(LOG_DEBUG_E,0,"WAIT: %d\n", waitReg);

         if(index < regTblSize)
         {
            CIREG_setWaitReg(&regTbl[index], waitReg, phase);
            index++;
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "exceeded reg table size %d\n", index);
         }         
      }
   }

   if(metaP)
   {
      metaP->numRegs = index;
   }
   // Close input file and exit.
   fclose (fIn);
   return ret;
}




