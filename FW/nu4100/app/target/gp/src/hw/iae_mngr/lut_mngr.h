#ifndef LUT_MNGR_H
#define LUT_MNGR_H

#define LUT_MNGR_NUM_NUM_MODES   (2) // A or B
//#define LUT_MNGR_NUM_IB_DSR      (3)
#define LUT_MNGR_NUM_IB_DSR      (2)
#define LUT_MNGR_NUM_IAE_LUT     (4)
#define LUT_MNGR_NUM_IAE_SUB_LUT (4)
#define LUT_MNGR_NUM_SENSORS     (8)
#define LUT_MNGR_NUM_IDSR_LUT    (1)
#define LUT_MNGR_NUM_LSC_LUT     (1)
#define LUT_MNGR_NUM_LUT_SIZE    ((LUT_MNGR_NUM_SENSORS * LUT_MNGR_NUM_IAE_SUB_LUT * LUT_MNGR_NUM_IB_DSR) + (LUT_MNGR_NUM_SENSORS * LUT_MNGR_NUM_IDSR_LUT) + (LUT_MNGR_NUM_SENSORS * LUT_MNGR_NUM_LSC_LUT))
#define LUT_MNGR_NUM_TOTAL_SIZE  (LUT_MNGR_NUM_LUT_SIZE * LUT_MNGR_NUM_NUM_MODES)
#define LUT_MNGR_NUM_OF_BUFF     (2) // Also defined in "inu_device.c". Once updated, need to update it in there as well.
// lut params
#define LUT_MNGR_CHUNK_DATA_LEN                      (8*1024)
#define LUT_MNGR_CHUNK_SIZE_FACTOR                   (13)
#define LUT_MNGR_IDSR_LUT_ID_OFFSET                  (LUT_MNGR_NUM_SENSORS * LUT_MNGR_NUM_IAE_SUB_LUT * LUT_MNGR_NUM_IB_DSR)

#define LUT_MNGR_IB_START_ID  (0)
#define LUT_MNGR_DSR_START_ID   (LUT_MNGR_NUM_SENSORS * LUT_MNGR_NUM_IAE_SUB_LUT)
#define LUT_MNGR_IDSR_START_ID (LUT_MNGR_DSR_START_ID + (LUT_MNGR_NUM_SENSORS * LUT_MNGR_NUM_IAE_SUB_LUT))
#define LUT_MNGR_LSC_START_ID  (LUT_MNGR_IDSR_START_ID + (LUT_MNGR_NUM_SENSORS * LUT_MNGR_NUM_IDSR_LUT))



typedef struct
{
   UINT32 lutSize;
   UINT8  data[1]; // real size as allocation in mem map
}lut_mngr_ddrLut;

ERRG_codeE lut_mngr_gpHandleLutChunkMsg(inu_deviceH me, void *lutChunkHdrP, UINT8 *chunkP);
ERRG_codeE lut_mngr_gpActivateSensorLut(unsigned int iauInd,unsigned int sensorId,CALIB_sensorLutModeE calibMode);
void lut_mngr_gpClearLutDb();
void lut_mngr_testLutFiles(char *calibPath);
ERRG_codeE lut_mngr_init();
ERRG_codeE lut_mngr_deinit();
void lut_mngr_updateDblBuffId(inu_deviceH me, UINT32 dblBuffId);
int lut_mngr_lockMutex();
int lut_mngr_unlockMutex();
ERRG_codeE lut_mngr_getIdsrLutAddress(void **idsrLutVirtAddress, void **idsrLutPhysAddress);

#endif


