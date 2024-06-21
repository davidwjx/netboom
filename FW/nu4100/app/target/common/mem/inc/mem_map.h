/****************************************************************************
 *
 *   FileName: mem_map.h
 *
 *   Author: Eyal A
 *
 *   Date: 27/11/2013
 *
 *   Description: Memory mapping API
 *
 *****************************************************************************/
#ifndef MEM_MAP_H
#define MEM_MAP_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "err_defs.h"
#include "nu4100_cram_regs.h"
#include "nu4100_lram_regs.h"

#include "inu_mem_def.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define MEM_MAPG_CNN_SPACE_ADDRESS        (LRAM_BASE)
#define MEM_MAPG_CNN_SPACE_SIZE           (0x1000)
#define MEM_MAPG_CNN_BOOT_MAILBOX_ADDRESS (LRAM_BASE + MEM_MAPG_CNN_SPACE_SIZE)
#define MEM_MAPG_CNN_BOOT_MAILBOX_SIZE    (0x8)
#define MEM_MAPG_DATA_BASE_ADRESS         (MEM_MAPG_CNN_BOOT_MAILBOX_ADDRESS + MEM_MAPG_CNN_BOOT_MAILBOX_SIZE)
#define MEM_MAPG_DATA_BASE_SIZE           (0xFF8)  // NOTE: the size is not round in order to create certain alignment for reuse section
#define MEM_MAPG_REUSE_BASE_ADRESS        (MEM_MAPG_DATA_BASE_ADRESS + MEM_MAPG_DATA_BASE_SIZE)
#define MEM_MAPG_REUSE_BASE_SIZE          (0x8000) // NOTE: Reuse memory for DDRless scenario
#define MEM_MAPG_CDE_DESCRIPTORS_ADDRESS  (MEM_MAPG_REUSE_BASE_ADRESS + MEM_MAPG_REUSE_BASE_SIZE)
#define MEM_MAPG_CDE_DESCRIPTORS_SIZE     (0x00002000)                                       // TODO: remove. obsolete
#define MEM_MAPG_STATS_ADDRESS            (MEM_MAPG_CDE_DESCRIPTORS_ADDRESS + MEM_MAPG_CDE_DESCRIPTORS_SIZE)
#define MEM_MAPG_STATS_SIZE               (0x4)
#define MEM_MAPG_CDE_PARAMS_ADDRESS       (MEM_MAPG_STATS_ADDRESS + MEM_MAPG_STATS_SIZE)
#define MEM_MAPG_CDE_PARAMS_SIZE          (0x00006000)                                       // TODO: BUGGY . Need to be sizeof(CDE_DRVG_channelParamsT), but taking extra 2k for safety (can be optimized)
#define MEM_MAPG_CDE_MNGR_DB_ADDRESS      (MEM_MAPG_CDE_PARAMS_ADDRESS + MEM_MAPG_CDE_PARAMS_SIZE)
#define MEM_MAPG_CDE_MNGR_DB_SIZE         (0x00001000)                                       // TODO: BUGGY . Need to be sizeof(CDE_MNGRG_dmaInfoT), but taking extra  for safety (can be optimized)


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   MEM_MAPG_START_REGION_E   = (0),
   MEM_MAPG_HW_REGION_E      = (MEM_MAPG_START_REGION_E),
   MEM_MAPG_BUFF_REGION_E,
   MEM_MAPG_DDR_REGION_E,
   MEM_MAPG_LAST_REGION_E
} MEM_MAPG_memRegionE;

typedef enum
{
   MEM_MAPG_FIRST_TABLE_ENTRY_E = (0),
   // NU3000 reg enum
   MEM_MAPG_REG_CRAM_E        = (MEM_MAPG_FIRST_TABLE_ENTRY_E),
   MEM_MAPG_REG_ISP0_E  ,
   MEM_MAPG_REG_ISP1_E  ,
   MEM_MAPG_REG_IPE_E  ,
   MEM_MAPG_REG_IAE_0_DATA_E  ,
   MEM_MAPG_REG_IAE_1_DATA_E  ,
   MEM_MAPG_REG_DPE_DATA_E    ,
   MEM_MAPG_REG_IAE_E         ,
   MEM_MAPG_REG_DPE_E         ,
   MEM_MAPG_REG_GME_E         ,
   MEM_MAPG_REG_GPV_E         ,
   MEM_MAPG_REG_ISPI_E        ,
   MEM_MAPG_REG_I2C_0_E       ,
   MEM_MAPG_REG_I2C_1_E       ,
   MEM_MAPG_REG_I2C_2_E       ,
   MEM_MAPG_REG_I2C_3_E       ,
   MEM_MAPG_REG_I2C_4_E       ,
   MEM_MAPG_REG_I2C_5_E       ,
   MEM_MAPG_REG_GIO0_E        ,
   MEM_MAPG_REG_GIO1_E        ,
   MEM_MAPG_REG_PSS_E         ,
   MEM_MAPG_REG_CDE_E         ,
   MEM_MAPG_REG_DSE_E         ,
   MEM_MAPG_REG_PPE_E         ,
   MEM_MAPG_REG_CVA_E         ,
   MEM_MAPG_REG_DDRC_MP_E     ,
   MEM_MAPG_REG_LAST_E        ,

   // AXI slave enum
   MEM_MAPG_AXI_RD0_E                  = (MEM_MAPG_REG_LAST_E),
   MEM_MAPG_AXI_RD1_E                  ,
   MEM_MAPG_AXI_RD2_E                  ,
   MEM_MAPG_AXI_RD3_E                  ,
   MEM_MAPG_AXI_CVA0_E                 ,
   MEM_MAPG_AXI_CVA1_E                 ,
   MEM_MAPG_AXI_CVA2_E                 ,
   MEM_MAPG_AXI_WB0_E                  ,
   MEM_MAPG_AXI_CVJ0_E                 ,
   MEM_MAPG_AXI_RD4_E                  ,
   MEM_MAPG_AXI_RD5_E                  ,
   MEM_MAPG_AXI_CVA3_E                 ,
   MEM_MAPG_AXI_CVA4_E                 ,
   MEM_MAPG_AXI_CVA5_E                 ,
   MEM_MAPG_AXI_WB1_E                  ,
   MEM_MAPG_AXI_CVJ1_E                 ,
   MEM_MAPG_AXI_RD8_E                  ,
   MEM_MAPG_AXI_RD9_E                  ,
   MEM_MAPG_AXI_CVA6_E                 ,
   MEM_MAPG_AXI_WB2_E                  ,
   MEM_MAPG_AXI_CVJ2_E                 ,
   MEM_MAPG_AXI_SCL_0_LUT_E            ,
   MEM_MAPG_AXI_SCL_1_LUT_E            ,
   MEM_MAPG_AXI_SCL_2_LUT_E            ,
   MEM_MAPG_AXI_SCL_3_LUT_E            ,
   MEM_MAPG_DPE_LUT_E                  ,
   MEM_MAPG_AXI_SLAVE_2_LAST_E         ,

   // IAE LUT module enum
   MEM_MAPG_IAE_LUT_IB_0_0_E           = (MEM_MAPG_AXI_SLAVE_2_LAST_E),
   MEM_MAPG_IAE_LUT_IB_0_1_E           ,
   MEM_MAPG_IAE_LUT_IB_0_2_E           ,
   MEM_MAPG_IAE_LUT_IB_0_3_E           ,
   MEM_MAPG_IAE_LUT_IB_1_0_E           ,
   MEM_MAPG_IAE_LUT_IB_1_1_E           ,
   MEM_MAPG_IAE_LUT_IB_1_2_E           ,
   MEM_MAPG_IAE_LUT_IB_1_3_E           ,
   MEM_MAPG_IAE_LUT_IB_2_0_E           ,
   MEM_MAPG_IAE_LUT_IB_2_1_E           ,
   MEM_MAPG_IAE_LUT_IB_2_2_E           ,
   MEM_MAPG_IAE_LUT_IB_2_3_E           ,
   MEM_MAPG_IAE_LUT_IB_3_0_E           ,
   MEM_MAPG_IAE_LUT_IB_3_1_E           ,
   MEM_MAPG_IAE_LUT_IB_3_2_E           ,
   MEM_MAPG_IAE_LUT_IB_3_3_E           ,
   MEM_MAPG_IAE_LUT_DSR_0_0_E          ,
   MEM_MAPG_IAE_LUT_DSR_0_1_E          ,
   MEM_MAPG_IAE_LUT_DSR_0_2_E          ,
   MEM_MAPG_IAE_LUT_DSR_0_3_E          ,
   MEM_MAPG_IAE_LUT_DSR_1_0_E          ,
   MEM_MAPG_IAE_LUT_DSR_1_1_E          ,
   MEM_MAPG_IAE_LUT_DSR_1_2_E          ,
   MEM_MAPG_IAE_LUT_DSR_1_3_E          ,
   MEM_MAPG_IAE_LUT_DSR_2_0_E          ,
   MEM_MAPG_IAE_LUT_DSR_2_1_E          ,
   MEM_MAPG_IAE_LUT_DSR_2_2_E          ,
   MEM_MAPG_IAE_LUT_DSR_2_3_E          ,
   MEM_MAPG_IAE_LUT_DSR_3_0_E          ,
   MEM_MAPG_IAE_LUT_DSR_3_1_E          ,
   MEM_MAPG_IAE_LUT_DSR_3_2_E          ,
   MEM_MAPG_IAE_LUT_DSR_3_3_E          ,
   MEM_MAPG_IAE_LUT_GCR_0_E            ,
   MEM_MAPG_IAE_LUT_GCR_1_E            ,
   MEM_MAPG_IAE_LUT_HIST_0_E           ,
   MEM_MAPG_IAE_LUT_HIST_1_E           ,
   MEM_MAPG_IAE_LUT_EHE_0_E            ,
   MEM_MAPG_IAE_LUT_EHE_1_E            ,
   MEM_MAPG_IAE_LUT_MRS_E              ,
   MEM_MAPG_IAE_LUT_ERR_E              ,
   MEM_MAPG_IAE_LUT_LAST_E             ,

   // CRAM module enum
   MEM_MAPG_CRAM_CP_MUTEX_E            = (MEM_MAPG_IAE_LUT_LAST_E),
   MEM_MAPG_CRAM_CEVA_LOG_E,
   MEM_MAPG_CRAM_EV61_LOG_E,
   MEM_MAPG_CRAM_CDE_DESCRIPTORS_E,
   MEM_MAPG_CRAM_STATS_E,
   MEM_MAPG_CRAM_CDE_PARAMS_E,
   MEM_MAPG_CRAM_CDE_MNGR_DB_E,
   MEM_MAPG_CRAM_CEVA_BOOT_PARAMS ,
   MEM_MAPG_CRAM_LAST_E                ,

   // LRAM module enum
   MEM_MAPG_LRAM_CNN_BOOT_E            = (MEM_MAPG_CRAM_LAST_E),
   MEM_MAPG_LRAM_CNN_BOOT_MAIL_BOX_E,
   MEM_MAPG_LRAM_DATA_BASE_E                 ,
   MEM_MAPG_LRAM_REUSE_BASE_E                ,
   MEM_MAPG_LRAM_LAST_E,

   // DSP enum
   MEM_MAPG_DDR_LRAM_BACKUP_E          = (MEM_MAPG_LRAM_LAST_E),
   MEM_MAPG_DDR_EV62_BACKUP_CODE_E     ,
   MEM_MAPG_DDR_EV62_CODE_E              ,

   MEM_MAPG_DDR_CEVA_LOAD_TCM_E          ,
   MEM_MAPG_DDR_CEVA_LOAD_SMEM_E         ,

   //General lut buffer
   MEM_MAPG_DDR_LUT_BUF_E             ,

   MEM_MAPG_DDR_LUT_GCR_0_A_E ,
   MEM_MAPG_DDR_LUT_GCR_1_A_E         ,

   //LUT mode B
   MEM_MAPG_DDR_LUT_GCR_0_B_E         ,
   MEM_MAPG_DDR_LUT_GCR_1_B_E         ,

   MEM_MAPG_DDR_BUFF_E,
   MEM_MAPG_DDR_CMEM_LAST_E,

   // HW debugging enum
   MEM_MAPG_NU3000_REGS_E              = (MEM_MAPG_DDR_CMEM_LAST_E),
   MEM_MAPG_LAST_TABLE_ENTRY_E

} MEM_MAPG_memMappingE;

typedef void *MEM_MAPG_addrT;


typedef enum
{
   MEM_MAPP_MAP_METHOD_MMAP_E,
   MEM_MAPP_MAP_METHOD_BUFF_MMAP_E,
   MEM_MAPP_MAP_METHOD_CMEM_E
} MEM_MAPP_memRegionMapMethodE;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT32                        start;
   UINT32                        size;
   UINT32                        id;
   MEM_MAPP_memRegionMapMethodE  mapMethod;
   INT8                          name[40];
   UINT32                        virtAddr;
} MEM_MAPP_memRegionPropT;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT32                         offset;
   UINT32                         size;
   UINT32                         isCached;
   UINT8                          name[40];
   MEM_MAPG_memRegionE            region;
   MEM_MAPG_addrT                 memPhysAddr;
   MEM_MAPG_addrT                 memVirtAddr;
}MEM_MAPP_memMappingPropT;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE MEM_MAPG_setMemMapExportAddress(MEM_MAPP_memMappingPropT *address);
ERRG_codeE MEM_MAPG_getSize(MEM_MAPG_memMappingE mappingNum, UINT32 *oSize);
ERRG_codeE MEM_MAPG_getOffset(MEM_MAPG_memMappingE mappingNum, UINT32 *oOffset);
ERRG_codeE MEM_MAPG_getPhyAddr(MEM_MAPG_memMappingE mappingNum, MEM_MAPG_addrT *oAddressP);
ERRG_codeE MEM_MAPG_getVirtAddr(MEM_MAPG_memMappingE mappingNum, MEM_MAPG_addrT *oAddressP);
ERRG_codeE MEM_MAPG_convertPhysicalToVirtual(UINT32 physicalAddr, UINT32 *oVirtualAddrP);
ERRG_codeE MEM_MAPG_convertPhysicalToVirtual2(UINT32 physicalAddr, UINT32 *oVirtualAddrP,MEM_MAPG_memMappingE mappingNum);
ERRG_codeE MEM_MAPG_convertPhysicalToVirtual3(UINT32 physicalAddr, UINT32 *oVirtualAddrP);
ERRG_codeE MEM_MAPG_convertVirtualToPhysical(UINT32 virtualAddr, UINT32 *oPhysicalAddrP);
ERRG_codeE MEM_MAPG_convertVirtualToPhysical2(UINT32 virtualAddr, UINT32 *oPhysicalAddrP,MEM_MAPG_memMappingE mappingNum);
ERRG_codeE MEM_MAPG_convertVirtualToPhysical3(UINT32 virtualAddr, UINT32 *oPhysicalAddrP);

#if (DEFSG_PROCESSOR == DEFSG_GP)
ERRG_codeE MEM_MAPG_open();
ERRG_codeE MEM_MAPG_close();
ERRG_codeE MEM_MAPG_showMappings();
ERRG_codeE MEM_MAPG_mapRegion(UINT32 *phyAddrP, UINT32 regionSize, BYTE isCached, UINT32 *virtAddrP, INT32 regionId);
ERRG_codeE MEM_MAPG_alloc(UINT32 mappingSize, UINT32 *mappingPhysAddr, UINT32 *mappingVirtAddr, BYTE mappingIsCached);
ERRG_codeE MEM_MAPG_free(UINT32 *mappingVirtAddr);
/**
 * @brief Allocates a buffer and also updates dmaBufFD with the file descriptor of the buffer
 * 
 * 
 * @param mappingSize Size of the mapping
 * @param mappingPhysAddr Pointer to store the Physical address
 * @param mappingVirtAddr Pointer to store the virtual address
 * @param mappingIsCached 0 = CMEM buffer
 * @param dmaBufFd Pointer to store DMA buffer
 * @return 
 */
ERRG_codeE MEM_MAPG_allocWithDMABuf(UINT32 mappingSize, UINT32 *mappingPhysAddr, UINT32 *mappingVirtAddr, BYTE mappingIsCached, INT32 *dmaBufFd);
#endif

#endif //MEM_MAP_H
