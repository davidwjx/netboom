#ifndef _CIREG_H_
#define _CIREG_H_


typedef struct
{
   UINT32 iaeWidth;
   UINT32 iaeHeight;
   UINT32 numRegs;
} CIREG_metaT;

void CIREG_setI2cReg(inu_device__writeRegT *regP,  UINT32 i2cNum, UINT8 targetAddress, UINT8 accessSize, UINT16 regAddress,  UINT32 regWidth, UINT32 i2cData, inu_device__dbPhaseE phase);
void CIREG_setWaitReg(inu_device__writeRegT *regP, UINT32 waitReg, inu_device__dbPhaseE phase);
void CIREG_setSocReg(inu_device__writeRegT *regP,UINT32 addr, UINT32 val, inu_device__dbPhaseE phase);
ERRG_codeE CIREG_parse(char *fileName, inu_device__dbPhaseE phase, inu_device__writeRegT *regTbl, UINT32 regTblSize,CIREG_metaT *metaP);


#endif //_CIREG_H_