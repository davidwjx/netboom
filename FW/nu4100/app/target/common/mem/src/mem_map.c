/****************************************************************************
 *
 *   FileName: mem_map.c
 *
 *   Author: Eyal A
 *
 *   Date: 27/11/2013
 *
 *   Description: Memory mapping API
 *
 *****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include <string.h>
#include <sys/ioctl.h>
#include "inu_types.h"
#include "inu_defs.h"
#include "err_defs.h"
#include "log.h"
#include "os_lyr.h"
#include "mem_map.h"
#include "nu4100_lram_regs.h" //LRAM base
#include "nu4100_cram_regs.h" //CRAM base
#include "gme_mngr.h"
#include "nucfg.h"
#include "inu_mem_def.h"
#if (DEFSG_OS == DEFSG_CEVA_OS)
#define IDMA_200_ADDRESS_SPAN (0)
#else
#include "IDMA_200_regs.h"
#endif

#if (DEFSG_PROCESSOR == DEFSG_CEVA)
#include "MM3K_defines.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

#define MEM_MAPP_BINARY_TREE
#ifdef MEM_MAPP_BINARY_TREE
//Sanity check on BST. Uses origianl searching on map, and compare results
#include "assert.h"
//#define MEM_MAPP_BINARY_TREE_SANITY
#endif

#ifdef MEM_MAPP_BINARY_TREE
typedef struct
{
   MEM_MAPG_addrT memPhysAddr;
   MEM_MAPG_addrT memVirtAddr;
   MEM_MAPG_memMappingE mappingNum;
}MEM_MAPP_infoT;

typedef struct node
{
   MEM_MAPP_infoT info;
   struct node *left, *right;
}node;

struct node *MEM_MAPP_newNode(MEM_MAPP_infoT *infoP)
{
    struct node *temp = (node*)malloc(sizeof(node));
    temp->info = *infoP;
    temp->left = temp->right = NULL;
    return temp;
}

struct node* MEM_MAPP_sortedArrayToBST(MEM_MAPP_infoT arr[], int start, int end)
{
    /* Base Case */
    if (start > end)
      return NULL;

    /* Get the middle element and make it root */
    int mid = (start + end)/2;
    struct node *root = MEM_MAPP_newNode(&arr[mid]);

    /* Recursively construct the left subtree and make it
       left child of root */
    root->left =  MEM_MAPP_sortedArrayToBST(arr, start, mid-1);

    /* Recursively construct the right subtree and make it
       right child of root */
    root->right = MEM_MAPP_sortedArrayToBST(arr, mid+1, end);

    return root;
}

static void MEM_MAPP_getPhyRegion(UINT32 mappingNum, UINT32 *base, UINT32 *size)
{
   MEM_MAPG_getPhyAddr((MEM_MAPG_memMappingE)mappingNum,(MEM_MAPG_addrT*)base);
   MEM_MAPG_getSize((MEM_MAPG_memMappingE)mappingNum,size);
}

static void MEM_MAPP_getVirtRegion(UINT32 mappingNum, UINT32 *base, UINT32 *size)
{
   MEM_MAPG_getVirtAddr((MEM_MAPG_memMappingE)mappingNum,(MEM_MAPG_addrT*)base);
   MEM_MAPG_getSize((MEM_MAPG_memMappingE)mappingNum,size);
}

static struct node* MEM_MAPP_searchTree(struct node* root, UINT32 physicalAddr, void (*getRegion)(UINT32 mappingNum, UINT32 *base, UINT32 *size))
{
   UINT32            startRegionPhysicalAddr, endRegionPhysicalAddr;
   MEM_MAPG_addrT    regionPhyBaseAddr =NULL;
   UINT32            regionSize;

   // Base Cases: root is null or key is present at root
   if (root == NULL)
      return root;

   getRegion(root->info.mappingNum,&startRegionPhysicalAddr,&regionSize);
   endRegionPhysicalAddr = startRegionPhysicalAddr + regionSize;

   if ( (physicalAddr < endRegionPhysicalAddr) && (physicalAddr >= startRegionPhysicalAddr) )
   {
      return root;
   }

   // Key is greater than root's key
   if (physicalAddr > startRegionPhysicalAddr)
   {
      return MEM_MAPP_searchTree(root->right, physicalAddr, getRegion);
   }

   // Key is smaller than root's key
   return MEM_MAPP_searchTree(root->left, physicalAddr, getRegion);
}


#define COUNT 3

// Function to print binary tree in 2D
// It does reverse inorder traversal
static void MEM_MAPP_print2DUtil(struct node *root, int space)
{
    // Base case
    if (root == NULL)
        return;

    // Increase distance between levels
    space += COUNT;

    // Process right child first
    MEM_MAPP_print2DUtil(root->right, space);

    // Print current node after space
    // count
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("%d\n", root->info.mappingNum);


    // Process left child
    MEM_MAPP_print2DUtil(root->left, space);
}

// Wrapper over print2DUtil()
static void MEM_MAPP_treePrint2D(struct node *root)
{
   // Pass initial space count as 0
   MEM_MAPP_print2DUtil(root, 0);
}

static node *MEM_MAPP_physicalAddressBST;
static node *MEM_MAPP_virtualAddressBST;
#endif

#define MEM_MAPP_SIZE_OF_MEM_MAP_TABLE sizeof(MEM_MAPP_memMappingTbl)


/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/



/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

#if (DEFSG_PROCESSOR == DEFSG_GP)
INT32 MEM_MAPP_memfd;
INT32 MEM_MAPP_memBufffd;
INT32 MEM_MAPP_memBuffphysAddrStart=0;
static MEM_MAPP_memRegionPropT MEM_MAPP_regionsTbl[MEM_MAPG_LAST_REGION_E];
static MEM_MAPP_memMappingPropT MEM_MAPP_memMappingTbl[MEM_MAPG_LAST_TABLE_ENTRY_E];
#ifdef MEM_MAPP_BINARY_TREE
static MEM_MAPP_memMappingPropT MEM_MAPP_memMappingDebug;
#endif
static MEM_MAPP_memMappingPropT *MEM_MAPP_memMappingTblP[MEM_MAPG_LAST_TABLE_ENTRY_E];
#elif (DEFSG_PROCESSOR == DEFSG_CEVA)
MEM_MAPP_memMappingPropT *MEM_MAPP_memMappingTblP[MEM_MAPG_LAST_TABLE_ENTRY_E] PRAGMA_DSECT_NO_LOAD("mem_map_sec");
#endif



/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/
#define __R (MEM_MAPP_memRegionPropT)
#define __M (MEM_MAPP_memMappingPropT)

#if (DEFSG_PROCESSOR == DEFSG_GP)

UINT32 MEM_MAPG_getMemRegionTable()
{
    return MEM_MAPP_memBuffphysAddrStart;
}

void MEM_MAPP_initMemRegionTable()
{
   MEM_MAPP_regionsTbl[MEM_MAPG_HW_REGION_E]    = __R {0x00000000, 0x0A200000, 0, MEM_MAPP_MAP_METHOD_MMAP_E, "NU4000 HW", 0};
   MEM_MAPP_regionsTbl[MEM_MAPG_DDR_REGION_E]   = __R {0x00000000, 0x00000000, 0, MEM_MAPP_MAP_METHOD_CMEM_E, "DDR Region",0};
}

void MEM_MAPP_initMemMappingTable()
{
#ifdef MEM_MAPP_BINARY_TREE
   MEM_MAPP_memMappingDebug                           = __M {0x00000000,         0x0A200000, 0, "NU4000 HW REGS", MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
#endif

   // NU3000 Registers Region
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_CRAM_E]        = __M {CRAM_BASE,          0x00080000, 0, "CSM",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)}; //EV62 shared memory
//   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_LRAM_E]      = __M {LRAM_BASE,          0x00060000, 0, "LRAM",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_ISP0_E]        = __M {0x03000000,         0x00006000, 0, "ISP0",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_ISP1_E]        = __M {0x03008000,         0x00006000, 0, "ISP1",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_IPE_E]         = __M {0x03010000,         0x00002400, 0, "IPE",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_IAE_0_DATA_E]  = __M {0x05000000,         0x01000000, 0, "IAE_0 Data", MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_IAE_1_DATA_E]  = __M {0x06000000,         0x01000000, 0, "IAE_1 Data", MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_DPE_DATA_E]    = __M {0x07000000,         0x01000000, 0, "DPE Data",   MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_IAE_E]         = __M {0x08000000,         0x00010000, 0, "IAE",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_DPE_E]         = __M {0x08010000,         0x00010000, 0, "DPE",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_GME_E]         = __M {0x08020000,         0x00010000, 0, "GME",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_GPV_E]         = __M {0x0A000000,         0x00010000, 0, "GPV",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_PPE_E]         = __M {0x08030000,         0x0000F000, 0, "PPE",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_CVA_E]         = __M {0x0803F000,         0x00001000, 0, "CVA",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_ISPI_E]        = __M {0x08040000,         0x00010000, 0, "ISPI",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_I2C_0_E]       = __M {0x08050000,         0x00010000, 0, "I2C_0",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_I2C_1_E]       = __M {0x08060000,         0x00010000, 0, "I2C_1",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_I2C_2_E]       = __M {0x08070000,         0x00010000, 0, "I2C_2",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_I2C_3_E]       = __M {0x08080000,         0x00010000, 0, "I2C_3",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_I2C_4_E]       = __M {0x08084000,         0x00004000, 0, "I2C_4",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_I2C_5_E]       = __M {0x08088000,         0x00004000, 0, "I2C_5",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_GIO0_E]        = __M {0x080B0000,         0x00008000, 0, "GIO0",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_GIO1_E]        = __M {0x080B8000,         0x00008000, 0, "GIO1",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_PSS_E]         = __M {0x080D0000,         0x00010000, 0, "PSS",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_CDE_E]         = __M {0x08140000,         0x00010000, 0, "CDE",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_DSE_E]         = __M {0x08150000,         0x00010000, 0, "DSE",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_GPV_E]         = __M {0x0A000000,         0x000C5108, 0, "GPV",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_REG_DDRC_MP_E]     = __M {0X081503F8,         0x00000A00, 0, "DDRC_MP",    MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};

#ifndef MEM_MAPP_BINARY_TREE
   // Global debuuging support
   MEM_MAPP_memMappingTbl[MEM_MAPG_NU3000_REGS_E]     = __M {0x00000000, 0x0A200000, 0, "NU4000 HW REGS", MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
#endif

   //CRAM mappings
   MEM_MAPP_memMappingTbl[MEM_MAPG_CRAM_CP_MUTEX_E]            = __M {MEM_MAPG_CP_MUTEX_ADDRESS,         MEM_MAPG_CRAM_CP_MUTEX_SIZE,     0, "CRAM_CP_MUTEX",              MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_CRAM_CEVA_LOG_E]            = __M {MEM_MAPG_CEVA_LOG_ADDRESS,         sizeof(LOGG_cevaLogbufferT),     0, "CRAM_CEVA_LOG",              MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_CRAM_EV61_LOG_E]            = __M {MEM_MAPG_EV61_LOG_ADDRESS,         sizeof(LOGG_cevaLogbufferT),     0, "CRAM_EV61_LOG",              MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_CRAM_CDE_DESCRIPTORS_E]     = __M {MEM_MAPG_CDE_DESCRIPTORS_ADDRESS,  MEM_MAPG_CDE_DESCRIPTORS_SIZE,   0, "CDE Descriptors",            MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_CRAM_STATS_E]               = __M {MEM_MAPG_STATS_ADDRESS,            MEM_MAPG_STATS_SIZE,             0, "Stats",                      MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_CRAM_CDE_PARAMS_E]          = __M {MEM_MAPG_CDE_PARAMS_ADDRESS,       MEM_MAPG_CDE_PARAMS_SIZE,        0, "CDE_DRVG_channelParamsT",    MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_CRAM_CDE_MNGR_DB_E]         = __M {MEM_MAPG_CDE_MNGR_DB_ADDRESS,       MEM_MAPG_CDE_MNGR_DB_SIZE,       0, "CDE_MNGRG_dmaInfoT",          MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_CRAM_CEVA_BOOT_PARAMS]      = __M {MEM_MAPG_CEVA_BOOT_PARAMS_ADDRESS, MEM_MAPG_CEVA_BOOT_PARAMS_SIZE,  0, "CRAM_CEVA_BOOT_PARAMS",      MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};

   //LRAM mappings
   MEM_MAPP_memMappingTbl[MEM_MAPG_LRAM_CNN_BOOT_E]           = __M {MEM_MAPG_CNN_SPACE_ADDRESS,        MEM_MAPG_CNN_SPACE_SIZE,        0, "CNN_BOOT",              MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_LRAM_CNN_BOOT_MAIL_BOX_E]  = __M {MEM_MAPG_CNN_BOOT_MAILBOX_ADDRESS, MEM_MAPG_CNN_BOOT_MAILBOX_SIZE, 0, "CNN_BOOT_MAIL_BOX",     MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_LRAM_DATA_BASE_E]          = __M {MEM_MAPG_DATA_BASE_ADRESS,         MEM_MAPG_DATA_BASE_SIZE,        0, "Data Base",             MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_LRAM_REUSE_BASE_E]         = __M {MEM_MAPG_REUSE_BASE_ADRESS,        MEM_MAPG_REUSE_BASE_SIZE,       0, "Reuse Base Address",    MEM_MAPG_HW_REGION_E,   (MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};

   //AXI slaves mappings
//   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_SLAVE_0_E]           = __M {0x05000000, 0x01000000, 0, "AXI SLAVE 0",            MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_RD0_E]               = __M {0x05000000, 0x00100000, 0, "AXI SLAVE 0 RD 0",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_RD1_E]               = __M {0x05100000, 0x00100000, 0, "AXI SLAVE 0 RD 1",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_RD2_E]               = __M {0x05200000, 0x00100000, 0, "AXI SLAVE 0 RD 2",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_RD3_E]               = __M {0x05300000, 0x00100000, 0, "AXI SLAVE 0 RD 3",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_CVA0_E]              = __M {0x05400000, 0x00100000, 0, "AXI SLAVE 0 CVA 0",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_CVA1_E]              = __M {0x05500000, 0x00100000, 0, "AXI SLAVE 0 CVA 1",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_WB0_E]               = __M {0x05700000, 0x00100000, 0, "AXI SLAVE 0 WB 0",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_CVJ0_E]              = __M {0x05800000, 0x00100000, 0, "AXI SLAVE 0 CVJ 0",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
//   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_SLAVE_1_E]           = __M {0x06000000, 0x01000000, 0, "AXI SLAVE 1",            MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_RD4_E]               = __M {0x06000000, 0x00100000, 0, "AXI SLAVE 1 RD 4",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_RD5_E]               = __M {0x06100000, 0x00100000, 0, "AXI SLAVE 1 RD 5",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_CVA2_E]              = __M {0x06400000, 0x00100000, 0, "AXI SLAVE 1 CVA 2",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_CVA3_E]              = __M {0x06500000, 0x00100000, 0, "AXI SLAVE 1 CVA 3",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_WB1_E]               = __M {0x06700000, 0x00100000, 0, "AXI SLAVE 1 WB 1",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_CVJ1_E]              = __M {0x06800000, 0x00100000, 0, "AXI SLAVE 1 CVJ 1",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
//   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_SLAVE_2_E]           = __M {0x07000000, 0x01000000, 0, "AXI SLAVE 2",            MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_RD8_E]               = __M {0x07000000, 0x00100000, 0, "AXI SLAVE 2 RD 8",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_RD9_E]               = __M {0x07100000, 0x00100000, 0, "AXI SLAVE 2 RD 9",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_CVA4_E]              = __M {0x07400000, 0x00100000, 0, "AXI SLAVE 2 CVA 4",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_CVA5_E]              = __M {0x07500000, 0x00100000, 0, "AXI SLAVE 2 CVA 5",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_CVA6_E]              = __M {0x07600000, 0x00100000, 0, "AXI SLAVE 2 CVA 6",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_WB2_E]               = __M {0x07700000, 0x00100000, 0, "AXI SLAVE 2 WB 2",       MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_CVJ2_E]              = __M {0x07800000, 0x00100000, 0, "AXI SLAVE 2 CVJ 2",      MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};


   //IAE Data LUT mappings
//   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_E]               = __M {0x04000000, 0x00200000, 0, "IAE LUT",                MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_0_0_E]        = __M {0x04040000, 0x00004F00, 0, "IAE_LUT_IB_0_0",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_0_1_E]        = __M {0x04060000, 0x00004F00, 0, "IAE_LUT_IB_0_1",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_0_2_E]        = __M {0x04080000, 0x00004F00, 0, "IAE_LUT_IB_0_2",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_0_3_E]        = __M {0x040A0000, 0x00004F00, 0, "IAE_LUT_IB_0_3",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_1_0_E]        = __M {0x040C0000, 0x00004F00, 0, "IAE_LUT_IB_1_0",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_1_1_E]        = __M {0x040E0000, 0x00004F00, 0, "IAE_LUT_IB_1_1",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_1_2_E]        = __M {0x04100000, 0x00004F00, 0, "IAE_LUT_IB_1_2",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_1_3_E]        = __M {0x04120000, 0x00004F00, 0, "IAE_LUT_IB_1_3",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_2_0_E]        = __M {0x04140000, 0x00009200, 0, "IAE_LUT_IB_2_0",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_2_1_E]        = __M {0x04160000, 0x00009200, 0, "IAE_LUT_IB_2_1",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_2_2_E]        = __M {0x04180000, 0x00009200, 0, "IAE_LUT_IB_2_2",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_2_3_E]        = __M {0x041A0000, 0x00009200, 0, "IAE_LUT_IB_2_3",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_3_0_E]        = __M {0x041C0000, 0x00009200, 0, "IAE_LUT_IB_3_0",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_3_1_E]        = __M {0x041E0000, 0x00009200, 0, "IAE_LUT_IB_3_1",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_3_2_E]        = __M {0x04200000, 0x00009200, 0, "IAE_LUT_IB_3_2",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_IB_3_3_E]        = __M {0x04220000, 0x00009200, 0, "IAE_LUT_IB_3_3",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_0_0_E]       = __M {0x04280000, 0x00002800, 0, "IAE_LUT_DSR_0_0",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_0_1_E]       = __M {0x042A0000, 0x00002800, 0, "IAE_LUT_DSR_0_1",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_0_2_E]       = __M {0x042C0000, 0x00002800, 0, "IAE_LUT_DSR_0_2",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_0_3_E]       = __M {0x042E0000, 0x00002800, 0, "IAE_LUT_DSR_0_3",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_1_0_E]       = __M {0x04300000, 0x00002800, 0, "IAE_LUT_DSR_1_0",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_1_1_E]       = __M {0x04320000, 0x00002800, 0, "IAE_LUT_DSR_1_1",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_1_2_E]       = __M {0x04340000, 0x00002800, 0, "IAE_LUT_DSR_1_2",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_1_3_E]       = __M {0x04360000, 0x00002800, 0, "IAE_LUT_DSR_1_3",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_2_0_E]       = __M {0x04380000, 0x00012400, 0, "IAE_LUT_DSR_2_0",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_2_1_E]       = __M {0x043A0000, 0x00012400, 0, "IAE_LUT_DSR_2_1",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_2_2_E]       = __M {0x043C0000, 0x00012400, 0, "IAE_LUT_DSR_2_2",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_2_3_E]       = __M {0x043E0000, 0x00012400, 0, "IAE_LUT_DSR_2_3",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_3_0_E]       = __M {0x04400000, 0x00012400, 0, "IAE_LUT_DSR_3_0",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_3_1_E]       = __M {0x04420000, 0x00012400, 0, "IAE_LUT_DSR_3_1",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_3_2_E]       = __M {0x04440000, 0x00012400, 0, "IAE_LUT_DSR_3_2",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_DSR_3_3_E]       = __M {0x04460000, 0x00012400, 0, "IAE_LUT_DSR_3_3",        MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_GCR_0_E ]        = __M {0x04240000, 0x00003000, 0, "IAE LUT_GCR_0",          MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_GCR_1_E ]        = __M {0x04260000, 0x00003000, 0, "IAE LUT_GCR_1",          MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_HIST_0_E]        = __M {0x04480000, 0x00002428, 0, "IAE LUT_HIST_0",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_HIST_1_E]        = __M {0x044A0000, 0x00002428, 0, "IAE LUT_HIST_1",         MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_EHE_0_E]         = __M {0x044C0000, 0x00012100, 0, "IAE LUT_EHE_0",          MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_EHE_1_E]         = __M {0x044E0000, 0x00012100, 0, "IAE LUT_EHE_1",          MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_MRS_E]           = __M {0x04500000, 0x001B6C00, 0, "IAE_LUT_MRS",            MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_IAE_LUT_ERR_E]           = __M {0x046C0000, 0x00140000, 0, "IAE_LUT_ERR",            MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_DPE_LUT_E]               = __M {0x04800000, 0x000007FF, 0, "DPE_LUT",                MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_SCL_0_LUT_E]         = __M {0x04C00000, 0x000005FF, 0, "SCL_0_LUT",              MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_SCL_1_LUT_E]         = __M {0x04D00000, 0x000005FF, 0, "SCL_1_LUT",              MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_SCL_2_LUT_E]         = __M {0x05000000, 0x000005FF, 0, "SCL_2_LUT",              MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_AXI_SCL_3_LUT_E]         = __M {0x05100000, 0x000005FF, 0, "SCL_3_LUT",              MEM_MAPG_HW_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};

   //DDR Region
   //DSP & EV62 sections are synced to the linker scripts of the cores. If changed, linker scripts needs also to be changed
   //DSP Sections
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_LRAM_BACKUP_E]       = __M {0x00000000, 0x00010000, 0, "LRAM backup",            MEM_MAPG_DDR_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_EV62_BACKUP_CODE_E]  = __M {0x00000000, 0x004F0000, 0, "EV62 backup code",       MEM_MAPG_DDR_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
#ifndef __UART_ON_FPGA__
   //EV62 Section
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_EV62_CODE_E]                  = __M {0x00000000, 0x1C000000, 0, "EV62 code",               MEM_MAPG_DDR_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
#endif
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_CEVA_LOAD_TCM_E]              = __M {0x00000000, 0x00008000, 0, "CEVA Load TCM",           MEM_MAPG_DDR_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)}; // 32KB
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_CEVA_LOAD_SMEM_E]             = __M {0x00000000, 0x00020000, 0, "CEVA Load SMEM",          MEM_MAPG_DDR_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)}; // 128KB

   //DDR LUT Region


   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_LUT_BUF_E        ]  = __M {0x00000000, 0x00002000,                     0, "LUT Buf",                          MEM_MAPG_DDR_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};

   // i case of 8 sensors
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_LUT_GCR_0_A_E    ]  = __M {0x00000000, 0x00003004 * CALIB_NUM_LUT_MODES_E*CALIB_NUM_TEMPER_LUT_MODES_E,      0, "LUT_GCR_0_A",     MEM_MAPG_DDR_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_LUT_GCR_1_A_E    ]  = __M {0x00000000, 0x00003004 * CALIB_NUM_LUT_MODES_E*CALIB_NUM_TEMPER_LUT_MODES_E,      0, "LUT_GCR_1_A",     MEM_MAPG_DDR_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_LUT_GCR_0_B_E    ]  = __M {0x00000000, 0x00003004 * CALIB_NUM_LUT_MODES_E*CALIB_NUM_TEMPER_LUT_MODES_E,      0, "LUT_GCR_0_B",     MEM_MAPG_DDR_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_LUT_GCR_1_B_E    ]  = __M {0x00000000, 0x00003004 * CALIB_NUM_LUT_MODES_E*CALIB_NUM_TEMPER_LUT_MODES_E,      0, "LUT_GCR_1_B",     MEM_MAPG_DDR_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_BUFF_E]        =      __M {0x00000000, 0x02800000,                                                           0, "BUFF",            MEM_MAPG_BUFF_REGION_E,(MEM_MAPG_addrT)(0), (MEM_MAPG_addrT)(0)};
}
#endif
void setDdrBuff(void *virtualBuff)
{
   MEM_MAPP_memMappingTbl[MEM_MAPG_DDR_BUFF_E].memVirtAddr =  virtualBuff;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S     **************
 ****************************************************************************/


/****************************************************************************
*
*  Function Name: MEM_MAPG_setMemMapExportAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_setMemMapExportAddress(MEM_MAPP_memMappingPropT *address)
{
   UINT32 index;
   ERRG_codeE ret = MEM_MAP__RET_SUCCESS;

   for(index = 0; index < MEM_MAPG_LAST_TABLE_ENTRY_E; index++)
   {
      MEM_MAPP_memMappingTblP[index] = address++;
   }
   return(ret);
}

/****************************************************************************
*
*  Function Name: MEM_MAPG_getSize
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_getSize(MEM_MAPG_memMappingE mappingNum, UINT32 *oSize)
{
   ERRG_codeE ret = MEM_MAP__RET_SUCCESS;
   *oSize = MEM_MAPP_memMappingTblP[mappingNum]->size;
   return(ret);
}

/****************************************************************************
*
*  Function Name: MEM_MAPG_getOffset
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_getOffset(MEM_MAPG_memMappingE mappingNum, UINT32 *oOffset)
{
   ERRG_codeE ret = MEM_MAP__RET_SUCCESS;
   *oOffset = MEM_MAPP_memMappingTblP[mappingNum]->offset;
   return(ret);
}

/****************************************************************************
*
*  Function Name: MEM_MAPG_getPhyAddr
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_getPhyAddr(MEM_MAPG_memMappingE mappingNum, MEM_MAPG_addrT *oAddressP)
{
   ERRG_codeE ret = MEM_MAP__RET_SUCCESS;
   *oAddressP = MEM_MAPP_memMappingTblP[mappingNum]->memPhysAddr;
   return(ret);
}

/****************************************************************************
*
*  Function Name: MEM_MAPG_getVirtAddr
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_getVirtAddr(MEM_MAPG_memMappingE mappingNum, MEM_MAPG_addrT *oAddressP)
{
   ERRG_codeE ret = MEM_MAP__RET_SUCCESS;
#if (DEFSG_PROCESSOR == DEFSG_CEVA)
   *oAddressP = MEM_MAPP_memMappingTblP[mappingNum]->memPhysAddr;
#else
   *oAddressP = MEM_MAPP_memMappingTblP[mappingNum]->memVirtAddr;
#endif
   return(ret);
}

/****************************************************************************
*
*  Function Name: MEM_MAPG_convertPhysicalToVirtual
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_convertPhysicalToVirtual(UINT32 physicalAddr, UINT32 *oVirtualAddrP)
{
   ERRG_codeE        ret = MEM_MAP__ERR_REGION_NO_FOUND;
   MEM_MAPG_memMappingE mappingNum;
   UINT32            startRegionPhysicalAddr, endRegionPhysicalAddr;
   MEM_MAPG_addrT    regionPhyBaseAddr =NULL;
   UINT32            regionSize;
   UINT32            addrOffset;
   MEM_MAPG_addrT    baseAddr;

#ifdef MEM_MAPP_BINARY_TREE
   struct node* resultNode = NULL;
   resultNode = MEM_MAPP_searchTree(MEM_MAPP_physicalAddressBST, physicalAddr, MEM_MAPP_getPhyRegion);

   if (resultNode)
   {
      addrOffset = physicalAddr - ((UINT32)resultNode->info.memPhysAddr);
      MEM_MAPG_getVirtAddr(resultNode->info.mappingNum, &baseAddr);
      *oVirtualAddrP = ((UINT32)baseAddr + addrOffset);
      ret = MEM_MAP__RET_SUCCESS;
   }
#ifdef MEM_MAPP_BINARY_TREE_SANITY
   MEM_MAPG_memMappingE resultMappingNum = 9999; //something invalid here
   for(mappingNum = MEM_MAPG_FIRST_TABLE_ENTRY_E; mappingNum < MEM_MAPG_LAST_TABLE_ENTRY_E; mappingNum++)
   {
      MEM_MAPG_getPhyAddr(mappingNum,&regionPhyBaseAddr);
      MEM_MAPG_getSize(mappingNum, &regionSize);
      startRegionPhysicalAddr = (UINT32)(regionPhyBaseAddr);
      endRegionPhysicalAddr   = startRegionPhysicalAddr + regionSize;

      if( (physicalAddr < endRegionPhysicalAddr) && (physicalAddr >= startRegionPhysicalAddr) )
      {
         resultMappingNum = mappingNum;
         break;
      }
   }
#endif

#else
   for(mappingNum = MEM_MAPG_FIRST_TABLE_ENTRY_E; mappingNum < MEM_MAPG_LAST_TABLE_ENTRY_E; mappingNum++)
   {
      MEM_MAPG_getPhyAddr(mappingNum,&regionPhyBaseAddr);
      MEM_MAPG_getSize(mappingNum, &regionSize);
      startRegionPhysicalAddr = (UINT32)(regionPhyBaseAddr);
      endRegionPhysicalAddr   = startRegionPhysicalAddr + regionSize;

      if( (physicalAddr < endRegionPhysicalAddr) && (physicalAddr >= startRegionPhysicalAddr) )
      {
         addrOffset = physicalAddr - startRegionPhysicalAddr;
         MEM_MAPG_getVirtAddr(mappingNum, &baseAddr);
         *oVirtualAddrP = ((UINT32)baseAddr + addrOffset);
         ret = MEM_MAP__RET_SUCCESS;
         break;
      }
   }
#endif

   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Mapping not found for: physAddress = 0x%x\n", physicalAddr);
      *oVirtualAddrP = 0;
#ifdef MEM_MAPP_BINARY_TREE_SANITY
      if (resultMappingNum != 9999)
      {
         printf("did not find node in tree for physicalAddr 0x%x, map mapping found %d\n",physicalAddr,resultMappingNum);
      }
#endif
   }

#ifdef MEM_MAPP_BINARY_TREE_SANITY
   else if (resultMappingNum != resultNode->info.mappingNum)
   {
      printf("ERROR! for physAddress = 0x%x, orig mapping %d, tree mapping %d\n", physicalAddr, resultMappingNum, resultNode->info.mappingNum);
      assert(0);
   }
#endif

   return ret;
}

/****************************************************************************
*
*  Function Name: MEM_MAPG_convertPhysicalToVirtual
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_convertPhysicalToVirtual2(UINT32 physicalAddr, UINT32 *oVirtualAddrP,MEM_MAPG_memMappingE mappingNum)
{
   ERRG_codeE        ret = MEM_MAP__ERR_REGION_NO_FOUND;
   UINT32            startRegionPhysicalAddr, endRegionPhysicalAddr;
   MEM_MAPG_addrT    regionPhyBaseAddr = NULL;
   UINT32            regionSize;
   UINT32            addrOffset;
   MEM_MAPG_addrT    baseAddr;

   MEM_MAPG_getPhyAddr(mappingNum,&regionPhyBaseAddr);
   MEM_MAPG_getSize(mappingNum, &regionSize);
   startRegionPhysicalAddr = (UINT32)(regionPhyBaseAddr);
   endRegionPhysicalAddr   = startRegionPhysicalAddr + regionSize;

   if( (physicalAddr < endRegionPhysicalAddr) && (physicalAddr >= startRegionPhysicalAddr) )
   {
      addrOffset = physicalAddr - startRegionPhysicalAddr;
      MEM_MAPG_getVirtAddr(mappingNum, &baseAddr);
      *oVirtualAddrP = ((UINT32)baseAddr + addrOffset);
      ret = MEM_MAP__RET_SUCCESS;
   }

   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Mapping not found for: physAddress = 0x%x, mapping =%d\n", physicalAddr, mappingNum);
      *oVirtualAddrP = 0;
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: MEM_MAPG_convertPhysicalToVirtual3
*
*  Description: used for debug access
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_convertPhysicalToVirtual3(UINT32 physicalAddr, UINT32 *oVirtualAddrP)
{
#ifdef MEM_MAPP_BINARY_TREE
   ERRG_codeE        ret = MEM_MAP__ERR_REGION_NO_FOUND;
   UINT32            startRegionPhysicalAddr, endRegionPhysicalAddr;
   MEM_MAPG_addrT    regionPhyBaseAddr = NULL;
   UINT32            regionSize;
   UINT32            addrOffset;
   MEM_MAPG_addrT    baseAddr;

   regionPhyBaseAddr = MEM_MAPP_memMappingDebug.memPhysAddr;
   regionSize = MEM_MAPP_memMappingDebug.size;
   startRegionPhysicalAddr = (UINT32)(regionPhyBaseAddr);
   endRegionPhysicalAddr   = startRegionPhysicalAddr + regionSize;

   if( (physicalAddr < endRegionPhysicalAddr) && (physicalAddr >= startRegionPhysicalAddr) )
   {
      addrOffset = physicalAddr - startRegionPhysicalAddr;
      baseAddr = MEM_MAPP_memMappingDebug.memVirtAddr;
      *oVirtualAddrP = ((UINT32)baseAddr + addrOffset);
      ret = MEM_MAP__RET_SUCCESS;
   }

   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Mapping not found for: physAddress = 0x%x\n", physicalAddr);
      *oVirtualAddrP = 0;
   }

   return ret;
#else
   return MEM_MAPG_convertPhysicalToVirtual(physicalAddr, oVirtualAddrP);
#endif
}


/****************************************************************************
*
*  Function Name: MEM_MAPG_convertVirtualToPhysical
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_convertVirtualToPhysical(UINT32 virtualAddr, UINT32 *oPhysicalAddrP)
{
   ERRG_codeE        ret = MEM_MAP__ERR_REGION_NO_FOUND;
   MEM_MAPG_memMappingE mappingNum;
   UINT32            startRegionVirtualAddr, endRegionVirtualAddr;
   MEM_MAPG_addrT    regionVirtBaseAddr;
   UINT32            regionSize;
   UINT32            addrOffset;
   MEM_MAPG_addrT    baseAddr;

#ifdef MEM_MAPP_BINARY_TREE
   struct node* resultNode = NULL;
   resultNode = MEM_MAPP_searchTree(MEM_MAPP_virtualAddressBST, virtualAddr, MEM_MAPP_getVirtRegion);

   if (resultNode)
   {
      addrOffset = virtualAddr - ((UINT32)resultNode->info.memVirtAddr);
      MEM_MAPG_getPhyAddr(resultNode->info.mappingNum, &baseAddr);
      *oPhysicalAddrP = ((UINT32)baseAddr + addrOffset);
      ret = MEM_MAP__RET_SUCCESS;
   }
#ifdef MEM_MAPP_BINARY_TREE_SANITY
   MEM_MAPG_memMappingE resultMappingNum = 9999; //something invalid here
   for(mappingNum = MEM_MAPG_FIRST_TABLE_ENTRY_E; mappingNum < MEM_MAPG_LAST_TABLE_ENTRY_E; mappingNum++)
   {
      MEM_MAPG_getVirtAddr(mappingNum,&regionVirtBaseAddr);
      MEM_MAPG_getSize(mappingNum, &regionSize);
      startRegionVirtualAddr = (UINT32)(regionVirtBaseAddr);
      endRegionVirtualAddr   = startRegionVirtualAddr + regionSize;

      if( (virtualAddr < endRegionVirtualAddr) && (virtualAddr >= startRegionVirtualAddr) )
      {
         resultMappingNum = mappingNum;
         break;
      }
   }
#endif

#else
   for(mappingNum = MEM_MAPG_FIRST_TABLE_ENTRY_E; mappingNum < MEM_MAPG_LAST_TABLE_ENTRY_E; mappingNum++)
   {
      MEM_MAPG_getVirtAddr(mappingNum,&regionVirtBaseAddr);
      MEM_MAPG_getSize(mappingNum, &regionSize);
      startRegionVirtualAddr  = (UINT32)(regionVirtBaseAddr);
      endRegionVirtualAddr    = startRegionVirtualAddr + regionSize;

      if( (virtualAddr < endRegionVirtualAddr) && (virtualAddr >= startRegionVirtualAddr) )
      {
         addrOffset = virtualAddr - startRegionVirtualAddr;
         MEM_MAPG_getPhyAddr(mappingNum, &baseAddr);
         *oPhysicalAddrP = ((UINT32)baseAddr + addrOffset);
         ret = MEM_MAP__RET_SUCCESS;
         break;
      }
   }
#endif

   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Mapping not found for: virtAddress = 0x%x\n", virtualAddr);
      *oPhysicalAddrP = 0;
#ifdef MEM_MAPP_BINARY_TREE_SANITY
      if (resultMappingNum != 9999)
      {
         printf("did not find node in tree for virtualAddr 0x%x, map mapping found %d\n",virtualAddr,resultMappingNum);
      }
#endif
   }
#ifdef MEM_MAPP_BINARY_TREE_SANITY
   else if (resultMappingNum != resultNode->info.mappingNum)
   {
      printf("ERROR! for virtualAddr = 0x%x, orig mapping %d, tree mapping %d\n", virtualAddr, resultMappingNum, resultNode->info.mappingNum);
      assert(0);
   }
#endif

   return ret;
}

ERRG_codeE MEM_MAPG_convertVirtualToPhysical2(UINT32 virtualAddr, UINT32 *oPhysicalAddrP,MEM_MAPG_memMappingE mappingNum)
{
   ERRG_codeE        ret = MEM_MAP__ERR_REGION_NO_FOUND;
   UINT32            startRegionVirtualAddr, endRegionVirtualAddr;
   MEM_MAPG_addrT    regionVirtBaseAddr;
   UINT32            regionSize;
   UINT32            addrOffset;
   MEM_MAPG_addrT    baseAddr;

   MEM_MAPG_getVirtAddr(mappingNum,&regionVirtBaseAddr);
   MEM_MAPG_getSize(mappingNum, &regionSize);
   startRegionVirtualAddr  = (UINT32)(regionVirtBaseAddr);
   endRegionVirtualAddr    = startRegionVirtualAddr + regionSize;

   if( (virtualAddr < endRegionVirtualAddr) && (virtualAddr >= startRegionVirtualAddr) )
   {
      addrOffset = virtualAddr - startRegionVirtualAddr;
      MEM_MAPG_getPhyAddr(mappingNum, &baseAddr);
      *oPhysicalAddrP = ((UINT32)baseAddr + addrOffset);
      ret = MEM_MAP__RET_SUCCESS;
   }

   if(ERRG_FAILED(ret))
   {
      //LOGG_PRINT(LOG_DEBUG_E, NULL, "Mapping not found for: virtAddress = 0x%x, mapping =%d\n", virtualAddr, mappingNum);
      *oPhysicalAddrP = 0;
   }

   return ret;
}


ERRG_codeE MEM_MAPG_convertVirtualToPhysical3(UINT32 virtualAddr, UINT32 *oPhysicalAddrP)
{
#ifdef MEM_MAPP_BINARY_TREE
   ERRG_codeE        ret = MEM_MAP__ERR_REGION_NO_FOUND;
   UINT32            startRegionVirtualAddr, endRegionVirtualAddr;
   MEM_MAPG_addrT    regionVirtBaseAddr;
   UINT32            regionSize;
   UINT32            addrOffset;
   MEM_MAPG_addrT    baseAddr;

   regionVirtBaseAddr = MEM_MAPP_memMappingDebug.memVirtAddr;
   regionSize = MEM_MAPP_memMappingDebug.size;
   startRegionVirtualAddr  = (UINT32)(regionVirtBaseAddr);
   endRegionVirtualAddr    = startRegionVirtualAddr + regionSize;

   if( (virtualAddr < endRegionVirtualAddr) && (virtualAddr >= startRegionVirtualAddr) )
   {
      addrOffset = virtualAddr - startRegionVirtualAddr;
      baseAddr = MEM_MAPP_memMappingDebug.memPhysAddr;
      *oPhysicalAddrP = ((UINT32)baseAddr + addrOffset);
      ret = MEM_MAP__RET_SUCCESS;
   }

   if(ERRG_FAILED(ret))
   {
      //LOGG_PRINT(LOG_DEBUG_E, NULL, "Mapping not found for: virtAddress = 0x%x, mapping =%d\n", virtualAddr, mappingNum);
      *oPhysicalAddrP = 0;
   }

   return ret;
#else
   return MEM_MAPG_convertVirtualToPhysical(virtualAddr, oPhysicalAddrP);
#endif
}


ERRG_codeE MEM_MAPG_mapRegion(UINT32 *phyAddrP, UINT32 regionSize, BYTE isCached, UINT32 *virtAddrP, INT32 regionId)
{
   ERRG_codeE ret = MEM_MAP__ERR_REGION_NO_FOUND;

   ret = OS_LYRG_mapRegion(MEM_MAPP_memfd, phyAddrP ,regionSize, isCached, virtAddrP, regionId,NULL);

   return ret;
}


#if (DEFSG_PROCESSOR == DEFSG_GP)
/****************************************************************************
*
*  Function Name: MEM_MAPG_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
#ifdef MEM_MAPP_BINARY_TREE
static int cmp_by_phyAddress(const void *a, const void *b)
{
   MEM_MAPP_infoT *entryA = (MEM_MAPP_infoT *)a;
   MEM_MAPP_infoT *entryB = (MEM_MAPP_infoT *)b;

   if (entryB->memPhysAddr > entryA->memPhysAddr)
   {
      return -1;
   }
   else
   {
      return 1;
   }
}

static int cmp_by_virtualAddress(const void *a, const void *b)
{
   MEM_MAPP_infoT *entryA = (MEM_MAPP_infoT *)a;
   MEM_MAPP_infoT *entryB = (MEM_MAPP_infoT *)b;

   if (entryB->memVirtAddr > entryA->memVirtAddr)
   {
      return -1;
   }
   else
   {
      return 1;
   }
}
#endif

ERRG_codeE MEM_MAPG_open()
{
   ERRG_codeE        ret = MEM_MAP__RET_SUCCESS;
   UINT32            regionPhysAddr, regionSize, regionVirtAddr, regionId;
   UINT32            mappingOffset, mappingIsCached, mappingPhysAddr, mappingVirtAddr, mappingSize;
   MEM_MAPG_memRegionE  regionNum;
   MEM_MAPG_memMappingE mappingNum;

   ret = OS_LYRG_openBuffMemFd(&MEM_MAPP_memBufffd);
   if (MEM_MAPP_memBufffd > 0)
   {
      ret = ioctl(MEM_MAPP_memBufffd,1,&MEM_MAPP_memBuffphysAddrStart);
      LOGG_PRINT(LOG_INFO_E ,ret, "Physical addresss kernel Buffer %x\n ",MEM_MAPP_memBuffphysAddrStart);
   }

   ret = OS_LYRG_openMemFd(&MEM_MAPP_memfd);
   if (!ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "open mem descriptor failed.\n");
      return (ret);
   }

   // Init Mapping tables
   MEM_MAPP_initMemRegionTable();
   MEM_MAPP_initMemMappingTable();

   // Map all mmap regions
   for(regionNum = MEM_MAPG_START_REGION_E; regionNum < MEM_MAPG_LAST_REGION_E; regionNum++)
   {
      if ((MEM_MAPP_regionsTbl[regionNum].mapMethod == MEM_MAPP_MAP_METHOD_MMAP_E)||(MEM_MAPP_regionsTbl[regionNum].mapMethod == MEM_MAPP_MAP_METHOD_BUFF_MMAP_E))
      {
         regionPhysAddr = MEM_MAPP_regionsTbl[regionNum].start;
         regionSize     = MEM_MAPP_regionsTbl[regionNum].size;
         if ((MEM_MAPP_regionsTbl[regionNum].mapMethod == MEM_MAPP_MAP_METHOD_MMAP_E))
            OS_LYRG_mapRegion(MEM_MAPP_memfd, &regionPhysAddr, regionSize, 0, &regionVirtAddr, -1, NULL);
         else
         {

              OS_LYRG_mapRegion(MEM_MAPP_memBufffd, &regionPhysAddr, regionSize, 0, &regionVirtAddr, 2, NULL);
         }

         if(ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "map memory region %d failed.\n", regionNum);
            return (ret);
         }
         MEM_MAPP_regionsTbl[regionNum].virtAddr = regionVirtAddr;
      }
   }

#ifdef MEM_MAPP_BINARY_TREE
   MEM_MAPP_infoT sortedPhysicalArray[MEM_MAPG_LAST_TABLE_ENTRY_E];
   MEM_MAPP_infoT sortedVirtualArray[MEM_MAPG_LAST_TABLE_ENTRY_E];

   regionNum      = MEM_MAPP_memMappingDebug.region;
   regionPhysAddr = MEM_MAPP_regionsTbl[regionNum].start;
   regionVirtAddr = MEM_MAPP_regionsTbl[regionNum].virtAddr;
   regionId       = MEM_MAPP_regionsTbl[regionNum].id;
   mappingOffset  = MEM_MAPP_memMappingDebug.offset;
   mappingIsCached= MEM_MAPP_memMappingDebug.isCached;
   mappingSize    = MEM_MAPP_memMappingDebug.size;
   if ((MEM_MAPP_regionsTbl[regionNum].mapMethod == MEM_MAPP_MAP_METHOD_MMAP_E)||
         (MEM_MAPP_regionsTbl[regionNum].mapMethod == MEM_MAPP_MAP_METHOD_BUFF_MMAP_E))
   {
      MEM_MAPP_memMappingDebug.memPhysAddr = (MEM_MAPG_addrT)(mappingOffset + regionPhysAddr);
      MEM_MAPP_memMappingDebug.memVirtAddr = (MEM_MAPG_addrT)(mappingOffset + regionVirtAddr);
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "mapping debug section\n");
      mappingPhysAddr = 0;
      mappingVirtAddr = 0;
      if(mappingSize >0)
      {
         ret = OS_LYRG_mapRegion(MEM_MAPP_memfd, &mappingPhysAddr, mappingSize, mappingIsCached, &mappingVirtAddr, regionId,NULL);
         if(ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "map memory region %d, %s failed.\n", regionNum, MEM_MAPP_memMappingDebug.name);
            return (ret);
         }
      }
      MEM_MAPP_memMappingDebug.memPhysAddr = (MEM_MAPG_addrT)mappingPhysAddr;
      MEM_MAPP_memMappingDebug.memVirtAddr = (MEM_MAPG_addrT)mappingVirtAddr;
   }
#endif

   // 1) Map all CMEM mappings
   // 2) Update Phys and Virtual mappings for MemMappings table
   for(mappingNum = MEM_MAPG_FIRST_TABLE_ENTRY_E; mappingNum < MEM_MAPG_LAST_TABLE_ENTRY_E; mappingNum++)
   {
      regionNum      = MEM_MAPP_memMappingTbl[mappingNum].region;
      regionPhysAddr = MEM_MAPP_regionsTbl[regionNum].start;
      regionVirtAddr = MEM_MAPP_regionsTbl[regionNum].virtAddr;
      regionId       = MEM_MAPP_regionsTbl[regionNum].id;
      mappingOffset  = MEM_MAPP_memMappingTbl[mappingNum].offset;
      mappingIsCached= MEM_MAPP_memMappingTbl[mappingNum].isCached;
      mappingSize    = MEM_MAPP_memMappingTbl[mappingNum].size;

      if ((MEM_MAPP_regionsTbl[regionNum].mapMethod == MEM_MAPP_MAP_METHOD_MMAP_E)||
            (MEM_MAPP_regionsTbl[regionNum].mapMethod == MEM_MAPP_MAP_METHOD_BUFF_MMAP_E))
      {
         MEM_MAPP_memMappingTbl[mappingNum].memPhysAddr = (MEM_MAPG_addrT)(mappingOffset + regionPhysAddr);
         MEM_MAPP_memMappingTbl[mappingNum].memVirtAddr = (MEM_MAPG_addrT)(mappingOffset + regionVirtAddr);
      }
      else
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "mapping num %d\n", mappingNum);
         mappingPhysAddr = 0;
         mappingVirtAddr = 0;
         if(mappingSize >0)
         {
            ret = OS_LYRG_mapRegion(MEM_MAPP_memfd, &mappingPhysAddr, mappingSize, mappingIsCached, &mappingVirtAddr, regionId,NULL);
            if(ERRG_FAILED(ret))
            {
               LOGG_PRINT(LOG_ERROR_E, ret, "map memory region %d, mappingNum %d (%s) failed.\n", regionNum, mappingNum,MEM_MAPP_memMappingTbl[mappingNum].name);
               return (ret);
            }
         }
         MEM_MAPP_memMappingTbl[mappingNum].memPhysAddr = (MEM_MAPG_addrT)mappingPhysAddr;
         MEM_MAPP_memMappingTbl[mappingNum].memVirtAddr = (MEM_MAPG_addrT)mappingVirtAddr;
      }
#ifdef MEM_MAPP_BINARY_TREE
      sortedPhysicalArray[mappingNum].mappingNum  = mappingNum;
      sortedPhysicalArray[mappingNum].memPhysAddr = MEM_MAPP_memMappingTbl[mappingNum].memPhysAddr;
      sortedPhysicalArray[mappingNum].memVirtAddr = MEM_MAPP_memMappingTbl[mappingNum].memVirtAddr;
      sortedVirtualArray[mappingNum].mappingNum  = mappingNum;
      sortedVirtualArray[mappingNum].memPhysAddr = MEM_MAPP_memMappingTbl[mappingNum].memPhysAddr;
      sortedVirtualArray[mappingNum].memVirtAddr = MEM_MAPP_memMappingTbl[mappingNum].memVirtAddr;
#endif
   }

   MEM_MAPG_setMemMapExportAddress(MEM_MAPP_memMappingTbl);

#ifdef MEM_MAPP_BINARY_TREE
   qsort(sortedPhysicalArray, MEM_MAPG_LAST_TABLE_ENTRY_E, sizeof(MEM_MAPP_infoT), cmp_by_phyAddress);
   qsort(sortedVirtualArray, MEM_MAPG_LAST_TABLE_ENTRY_E, sizeof(MEM_MAPP_infoT), cmp_by_virtualAddress);

   MEM_MAPP_physicalAddressBST = MEM_MAPP_sortedArrayToBST(sortedPhysicalArray, 0, MEM_MAPG_LAST_TABLE_ENTRY_E - 1);
   MEM_MAPP_virtualAddressBST  = MEM_MAPP_sortedArrayToBST(sortedVirtualArray, 0, MEM_MAPG_LAST_TABLE_ENTRY_E - 1);
#endif
   return(ret);
}

/****************************************************************************
*
*  Function Name: MEM_MAPG_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_close()
{
   ERRG_codeE           ret = MEM_MAP__RET_SUCCESS;
   UINT32               regionSize, regionVirtAddr, regionId;
   UINT32               mappingVirtAddr, mappingSize;
   MEM_MAPG_memRegionE  regionNum;
   MEM_MAPG_memMappingE mappingNum;

   // Unmap all mmap regions
   for(regionNum = MEM_MAPG_START_REGION_E; regionNum < MEM_MAPG_LAST_REGION_E; regionNum++)
   {
      if (MEM_MAPP_regionsTbl[regionNum].mapMethod == MEM_MAPP_MAP_METHOD_MMAP_E)
      {
         regionVirtAddr = MEM_MAPP_regionsTbl[regionNum].virtAddr;
         regionSize     = MEM_MAPP_regionsTbl[regionNum].size;

         ret = OS_LYRG_unmapRegion((void *)regionVirtAddr, regionSize, -1);

         if(ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "map memory region %d failed !!!\n", regionNum);
            return (ret);
         }
         MEM_MAPP_regionsTbl[regionNum].virtAddr = regionVirtAddr;
      }
   }

   // Unmap CMEM based mappings
   for(mappingNum = MEM_MAPG_FIRST_TABLE_ENTRY_E; mappingNum < MEM_MAPG_LAST_TABLE_ENTRY_E; mappingNum++)
   {
      regionNum      = MEM_MAPP_memMappingTbl[mappingNum].region;
      regionId       = MEM_MAPP_regionsTbl[regionNum].id;

      if (MEM_MAPP_regionsTbl[regionNum].mapMethod == MEM_MAPP_MAP_METHOD_MMAP_E)
      {
         MEM_MAPP_memMappingTbl[mappingNum].memPhysAddr = (MEM_MAPG_addrT)(0);
         MEM_MAPP_memMappingTbl[mappingNum].memVirtAddr = (MEM_MAPG_addrT)(0);
      }
      else
      {
         mappingVirtAddr = (UINT32)MEM_MAPP_memMappingTbl[mappingNum].memVirtAddr;
         mappingSize     = MEM_MAPP_memMappingTbl[mappingNum].size;

         ret = OS_LYRG_unmapRegion((void *)mappingVirtAddr, mappingSize, (INT32)regionId);
         if(ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "map memory region %d failed !!!\n", regionNum);
            return (ret);
         }
         MEM_MAPP_memMappingTbl[mappingNum].memPhysAddr = (MEM_MAPG_addrT)0;
         MEM_MAPP_memMappingTbl[mappingNum].memVirtAddr = (MEM_MAPG_addrT)0;
      }
   }
   ret = OS_LYRG_closeMemFd(MEM_MAPP_memfd);
   return(ret);
}

/****************************************************************************
*
*  Function Name: MEM_MAPG_showMappings
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE MEM_MAPG_showMappings()
{
   ERRG_codeE           ret = MEM_MAP__RET_SUCCESS;
   MEM_MAPG_memRegionE  regionNum;
   MEM_MAPG_memMappingE mappingNum;

   for(regionNum = MEM_MAPG_START_REGION_E; regionNum < MEM_MAPG_LAST_REGION_E; regionNum++)
   {
      printf("Region %s:\n", MEM_MAPP_regionsTbl[regionNum].name);
      printf("num  physical    virtual     size        c    name\n");
      printf("------------------------------------------------\n");
      //LOGG_PRINT(LOG_INFO_E, NULL, "Region %s:\n", MEM_MAPP_regionsTbl[regionNum].name);
      //LOGG_PRINT(LOG_INFO_E, NULL, "physical    virtual     size        c    name\n");
      //LOGG_PRINT(LOG_INFO_E, NULL, "------------------------------------------------\n");

      for(mappingNum = MEM_MAPG_FIRST_TABLE_ENTRY_E; mappingNum < MEM_MAPG_LAST_TABLE_ENTRY_E; mappingNum++)
      {
         if (MEM_MAPP_memMappingTblP[mappingNum]->region != regionNum)
            continue;
         if(!MEM_MAPP_memMappingTblP[mappingNum]->size)
            continue;

         printf("%03d: 0x%08x  0x%08x  0x%08x  %-3s  %s\n",mappingNum,
            (unsigned int)MEM_MAPP_memMappingTblP[mappingNum]->memPhysAddr,
            (unsigned int)MEM_MAPP_memMappingTblP[mappingNum]->memVirtAddr,
            MEM_MAPP_memMappingTblP[mappingNum]->size,
            (MEM_MAPP_memMappingTblP[mappingNum]->isCached) ? "yes":"no",
            MEM_MAPP_memMappingTblP[mappingNum]->name);

         /*LOGG_PRINT(LOG_INFO_E, NULL, "0x%08x  0x%08x  0x%08x  %-3s  %s\n",
            MEM_MAPP_memMappingTblP[mappingNum]->memPhysAddr,
            MEM_MAPP_memMappingTblP[mappingNum]->memVirtAddr,
            MEM_MAPP_memMappingTblP[mappingNum]->size,
            (MEM_MAPP_memMappingTblP[mappingNum]->isCached) ? "yes":"no",
            MEM_MAPP_memMappingTblP[mappingNum]->name);*/
      }
      //LOGG_PRINT(LOG_INFO_E, NULL, "\n");
      printf("\n");
   }

 //  printf("Physical Tree:\n");
 //  MEM_MAPP_treePrint2D(MEM_MAPP_physicalAddressBST);
 //  printf("\n\n\nVirtual Tree:\n");
 //  MEM_MAPP_treePrint2D(MEM_MAPP_virtualAddressBST);

   return(ret);
}

ERRG_codeE MEM_MAPG_alloc(UINT32 mappingSize, UINT32 *mappingPhysAddr, UINT32 *mappingVirtAddr, BYTE mappingIsCached)
{
   if (mappingIsCached == 2)
   {
      return OS_LYRG_mapRegion(MEM_MAPP_memBufffd, mappingPhysAddr, mappingSize, mappingIsCached, mappingVirtAddr, 2,NULL);

   }
   else return OS_LYRG_mapRegion(MEM_MAPP_memfd, mappingPhysAddr, mappingSize, mappingIsCached, mappingVirtAddr, 0, NULL);
}
ERRG_codeE MEM_MAPG_allocWithDMABuf(UINT32 mappingSize, UINT32 *mappingPhysAddr, UINT32 *mappingVirtAddr, BYTE mappingIsCached, INT32 *dmaBufFd)
{
   if (mappingIsCached == 2)
   {
      return OS_LYRG_mapRegion(MEM_MAPP_memBufffd, mappingPhysAddr, mappingSize, mappingIsCached, mappingVirtAddr, 2, dmaBufFd);

   }
   else return OS_LYRG_mapRegion(MEM_MAPP_memfd, mappingPhysAddr, mappingSize, mappingIsCached, mappingVirtAddr, 0,dmaBufFd);
}


ERRG_codeE MEM_MAPG_free(UINT32 *mappingVirtAddr)
{
   return OS_LYRG_unmapRegion((void*)mappingVirtAddr, 0, 0);
}


#endif

#ifdef __cplusplus
 }
#endif

