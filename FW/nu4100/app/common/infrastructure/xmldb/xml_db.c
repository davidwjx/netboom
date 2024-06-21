/*
xml database module
*/
#include "inu_common.h"
#include <assert.h>
#include <inttypes.h>
#include "xml_path.h"
#include "xml_db.h"
#include "hash.h"
#include "sxmlc.h"
#include "xml_path.h"
#include "xml_path_extended.h"
#include "nufld.h"
#if DEFSG_IS_GP
#include "mem_map.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

//#define XMLDB_SAFE
#ifdef XMLDB_SAFE
#define XMLDB_PATH_CHECK(p) assert((p >= 0 && p < XMLDB_NUM_PATHS_E))
#else
#define XMLDB_PATH_CHECK(p)
#endif

#define XMLDB_MAX_DBS (3)
#define XMLDB_PATH_SEPARATOR '.'

/*
  dirty - db entry has been modified by set or clear. loadxml/savexml cleans the entry.
  empty - db entry is empty on open or clear. not empty on set/loadxml
*/
#define XMLDB_EMPTY_BIT  (0)
#define XMLDB_DIRTY_BIT  (1)
#define XMLDB_VIRTUAL_BIT  (2)
#define XMLDB_CONST_BIT  (3)
#define XMLDB_SET_EMPTY(f)  (f |= (1<<XMLDB_EMPTY_BIT))
#define XMLDB_CLR_EMPTY(f)  (f &= ~(1<<XMLDB_EMPTY_BIT))
#define XMLDB_IS_EMPTY(f)   (f & (1<<XMLDB_EMPTY_BIT))
#define XMLDB_SET_DIRTY(f)  (f |= (1<<XMLDB_DIRTY_BIT))
#define XMLDB_CLR_DIRTY(f)  (f &= ~(1<<XMLDB_DIRTY_BIT))
#define XMLDB_IS_DIRTY(f)   (f & (1<<XMLDB_DIRTY_BIT))
#define XMLDB_SET_VIRTUAL(f)  (f |= (1<<XMLDB_VIRTUAL_BIT))
#define XMLDB_CLR_VIRTUAL(f)  (f &= ~(1<<XMLDB_VIRTUAL_BIT))
#define XMLDB_IS_VIRTUAL(f)   (f & (1<<XMLDB_VIRTUAL_BIT))
#define XMLDB_SET_CONST(f)  (f |= (1<<XMLDB_CONST_BIT))
#define XMLDB_CLR_CONST(f)  (f &= ~(1<<XMLDB_CONST_BIT))
#define XMLDB_IS_CONST(f)   ((f & (1<<XMLDB_CONST_BIT) && enableConstMode))


//                                           first in his enum                                      -      end enum of previous            +     previous define
#define XMLDB_PROJECTOR_ENUM_JUMP    ((((UINT32)PROJECTORS_PROJ_0_MODEL_EXT_E)                        - (UINT32)END_OF_SENSORS_EXT_E))
#define XMLDB_MEDIATOR_ENUM_JUMP     ((((UINT32)MEDIATORS_MED_0_SRC_SEL0_EXT_E)                       - ((UINT32)END_OF_PROJECTORS_EXT_E)) + ((UINT32)XMLDB_PROJECTOR_ENUM_JUMP))
#define XMLDB_BUS_ENUM_JUMP          ((((UINT32)BUS_MIPI_CSI_RX0_LANES_EXT_E)                         - ((UINT32)END_OF_MEDIATORS_EXT_E))  + ((UINT32)XMLDB_MEDIATOR_ENUM_JUMP))
#define XMLDB_ISP_ENUM_JUMP          ((((UINT32)ISPS_ISP0_NUM_FRAMES_TO_SKIP_EXT_E)                    - ((UINT32)END_OF_BUS_EXT_E))        + ((UINT32)XMLDB_BUS_ENUM_JUMP))
#define XMLDB_IAE_ENUM_JUMP          ((((UINT32)NU4100_IAE_ENABLE_SLU0_EN_EXT_E)                      - ((UINT32)END_OF_ISPS_EXT_E))       + ((UINT32)XMLDB_ISP_ENUM_JUMP))
#define XMLDB_PPE_ENUM_JUMP          ((((UINT32)NU4100_PPE_PPE_MISC_MIPI_VI0_CONTROL_STRM_SEL_EXT_E)  - ((UINT32)END_OF_BLOCK_IAE_EXT_E))  + ((UINT32)XMLDB_IAE_ENUM_JUMP))
#define XMLDB_DPE_ENUM_JUMP          ((((UINT32)NU4100_DPE_HYBRID_CFG0_EN_EXT_E)                      - ((UINT32)END_OF_BLOCK_PPE_EXT_E))  + ((UINT32)XMLDB_PPE_ENUM_JUMP))
#define XMLDB_CVA_ENUM_JUMP          ((((UINT32)NU4100_CVA_REGISTERS_READY_DONE_EXT_E)                - ((UINT32)END_OF_BLOCK_DPE_EXT_E))  + ((UINT32)XMLDB_DPE_ENUM_JUMP))
#define XMLDB_IPE_ENUM_JUMP          ((((UINT32)NU4100_IPE_IPE_PREPROC_CTRL0_ISP0_CH0_EXP0_SEL_EXT_E) - ((UINT32)END_OF_BLOCK_CVA_EXT_E))  + ((UINT32)XMLDB_CVA_ENUM_JUMP))
#define XMLDB_META_ENUM_JUMP         ((((UINT32)META_READERS_RD_0_ENABLE_EXT_E)                       - ((UINT32)END_OF_BLOCK_IPE_EXT_E))  + ((UINT32)XMLDB_IPE_ENUM_JUMP))

//for fewer collisions set hash entries to twice num paths
#define XMLDB_NUM_HASH_ENTRIES  (XMLDB_NUM_PATHS_E * 2)

#define XMLDB_FAST_WRITE
#ifdef XMLDB_FAST_WRITE
#define XMLDB_MAX_NUM_REGS_FAST_WRITE      (300)
#define XMLDB_MAX_NUM_BLKS_INST_FAST_WRITE (12)

typedef struct
{
   UINT32 virtAddress;
   UINT32 val;
} XMLDB_fastWriteReg;

typedef struct
{
   XMLDB_fastWriteReg regs[XMLDB_MAX_NUM_REGS_FAST_WRITE];
   UINT32             numRegs;
   UINT32             configured;
} XMLDB_fastWriteBlock;

typedef struct
{
   XMLDB_fastWriteBlock blocks[NUFLD_META_READERS_E][XMLDB_MAX_NUM_BLKS_INST_FAST_WRITE];
} XMLDB_fastWriteDb;
#endif

typedef struct
{
   UINT32 blockAdd;
   UINT16 regOffset;
   UINT8 startBitOffset;
   UINT8 width;
} XMLDB_addressT;

typedef struct
{
   UINT32 val; //text converted from xml
   UINT32 resetVal;
   XMLDB_addressT regFieldParams;
   UINT16 flags;//dirty,empty
} XMLDB_valueT;

typedef struct _XMLDB_dbT
{
   XMLDB_valueT valueTbl[XMLDB_NUM_PATHS_E];
   char pathbuf[XMLDB_MAX_PATH_LEN];//work area for load and save functions
#ifdef XMLDB_FAST_WRITE
   XMLDB_fastWriteDb *fastWriteDbP;
#endif
} XMLDB_dbT;

typedef struct _XMLDB_senderT
{
    XMLDB_valueT valueTbl;
    UINT32 origIndex;
} XMLDB_senderT;

typedef void(*nodeCbT)(XMLNode *node, void *arg);

static int enableConstMode;

/*
   walk sub-tree recursively invoking the cb
   post-order walk (cb is called as the recursion returns from all children)
*/
static void walkNodes(XMLNode* node, nodeCbT cb, void *arg)
{
   int i;

   for (i = 0; i < node->n_children; i++)
   {
      walkNodes(node->children[i], cb, arg);
   }
   cb(node, arg);
   //printf("%s\n", node->tag);
}

/*
   walk the xml buffer invoking the cb
*/
static ERRG_codeE walkXml(XMLDoc *docp, char *buf, nodeCbT cb, void *arg)
{
   ERRG_codeE retval = XMLDB__RET_SUCCESS;
   int ret = 0;

   //parse xml buffer and set values in hash table

   ret = XMLDoc_parse_buffer_DOM(buf, NULL, docp);
   if (ret)
   {
      if (docp->i_root >= 0)
      {
         walkNodes(docp->nodes[docp->i_root], cb, arg);
      }
      else
      {
         retval = XMLDB__ERR_INVALID_ARGS;
      }
   }
   else
   {
      retval = XMLDB__ERR_UNEXPECTED;
   }

   return retval;
}


/*
   Convert text to u32.
   Return 0 on success, -1 on error: empty string, conversion error or larger than max u32.
*/
static int text2u32(const char *text, UINT32 *value)
{
   int ret = -1;
   char *endptr;
   uintmax_t intval;

   if ((!text) || (strlen(text) == 0))
      return -1; //no string or empty string

   //input string is not empty, so if conversion is OK endptr
   //should point to its eos . If not then either part or all of the string is invalid.
   intval = strtoumax(text, &endptr, 0);
   if (*endptr == '\0')
   {
      //now verify does not exceed u32 maximum
      if (intval <= (UINT32)(~0))
      {
         *value = (UINT32)intval;
         ret = 0;
      }
   }
   return ret;
}

/*
Convert text to u16.
Return 0 on success, -1 on error: empty string, conversion error or larger than max u32.
*/
static int text2u16(const char *text, UINT16 *value)
{
    int ret = -1;
    char *endptr;
    uintmax_t intval;

    if ((!text) || (strlen(text) == 0))
        return -1; //no string or empty string

                   //input string is not empty, so if conversion is OK endptr
                   //should point to its eos . If not then either part or all of the string is invalid.
    intval = strtoumax(text, &endptr, 0);
    if (*endptr == '\0')
    {
        //now verify does not exceed u16 maximum
        if (intval <= (UINT16)(~0))
        {
            *value = (UINT16)intval;
            ret = 0;
        }
    }
    return ret;
}

/*
Convert text to u8.
Return 0 on success, -1 on error: empty string, conversion error or larger than max u32.
*/
static int text2u8(const char *text, UINT8 *value)
{
    int ret = -1;
    char *endptr;
    uintmax_t intval;

    if ((!text) || (strlen(text) == 0))
        return -1; //no string or empty string

                   //input string is not empty, so if conversion is OK endptr
                   //should point to its eos . If not then either part or all of the string is invalid.
    intval = strtoumax(text, &endptr, 0);
    if (*endptr == '\0')
    {
        //now verify does not exceed u8 maximum
        if (intval <= (UINT8)(~0))
        {
            *value = (UINT8)intval;
            ret = 0;
        }
    }
    return ret;
}

int getAttributeValue(XMLNode* node, char* attName, const char** val)
{
    return XMLNode_get_attribute_with_default(node, attName, val, "-1");
}

void getRegFields(XMLNode* node, const char** regOffset, const char** blockStartAddress, const char** subblockStartAddress, const char *defaultVal )
{
    XMLNode *tmpNode;
    (void)defaultVal;
    tmpNode = node;
    while (tmpNode->father)
    {
        if (strcmp((char*)tmpNode->father->tag, "REGISTER") == 0)
        {
            free((void*)*regOffset);
            getAttributeValue(tmpNode->father, "ADDR_OFFSET", regOffset);
        }
        else if (strcmp((char*)tmpNode->father->tag, "SUBBLOCK") == 0)
        {
            free((void*)*subblockStartAddress);
            getAttributeValue(tmpNode->father, "START_ADDR", subblockStartAddress);
        }
        else if (strcmp((char*)tmpNode->father->tag, "BLOCK") == 0)
        {
            free((void*)*blockStartAddress);
            getAttributeValue(tmpNode->father, "START_ADDR", blockStartAddress);    
        }
        tmpNode = tmpNode->father;
    }   
}

/*
   init db flags
*/
static void initDbFlags(XMLDB_dbH db)
{
   INT32 i;
   for (i = 0; i < XMLDB_NUM_PATHS_E; i++)
   {
      db->valueTbl[i].flags = (1 << XMLDB_EMPTY_BIT);
   }
}

static ERRG_codeE validateValByWidth(UINT32 val, UINT8 width)
{
   ERRG_codeE ret = XMLDB__RET_SUCCESS;
   UINT32 mask, tmpVal;

   if(width)
   {
      mask = 0xFFFFFFFF;
      mask = mask >> (32 - width);
      tmpVal = val & mask;
      if ((tmpVal != val) && (((~mask)&val)!=(~mask)))//(~mask&val != ~mask)-->for negativ values. become as 32bit. 4294966821 with width of 14 bit mean -475
          ret = XMLDB__ERR_UNEXPECTED;
   }
   else
   {
      //printf("Width = 0: assumption not register value (not IDVE) do not validate width\n");
   }
   return ret;
}

UINT32 XMLDB_extendedToRegularEnumConvert(UINT32 externaUserId)
{
	////for debug
	//int x = PROJECTOR_ENUM_JUMP;
	//int x1 = BUS_ENUM_JUMP;
	//int x2 = IAE_ENUM_JUMP;
	//int x3 = PPE_ENUM_JUMP;
	//int x4 = DPE_ENUM_JUMP;
	//int x5 = CVA_ENUM_JUMP;
	//int x6 = META_ENUM_JUMP;

	//sensores
   if (externaUserId < END_OF_SENSORS_EXT_E)
   {
	   return externaUserId;
   }
   //projectors
   else if (externaUserId < END_OF_PROJECTORS_EXT_E)//last in his enum
   {
	   return (externaUserId - XMLDB_PROJECTOR_ENUM_JUMP);
   }
   //mediator
   else if (externaUserId < END_OF_MEDIATORS_EXT_E)
   {
	   return (externaUserId - XMLDB_MEDIATOR_ENUM_JUMP);
   }
   //bus
   else if (externaUserId < END_OF_BUS_EXT_E)
   {
	   return (externaUserId - XMLDB_BUS_ENUM_JUMP);
   }
   //ISP - external
   else if (externaUserId < END_OF_ISPS_EXT_E)
   {
       UINT32 s = XMLDB_BUS_ENUM_JUMP;
       UINT32 d = ((((UINT32)ISPS_ISP0_FUNCTIONALITY_AWB_EXT_E) - ((UINT32)END_OF_ISPS_EXT_E)) + ((UINT32)XMLDB_BUS_ENUM_JUMP));
       return (externaUserId - XMLDB_ISP_ENUM_JUMP);
   }
   //IAE
   else if (externaUserId < END_OF_BLOCK_IAE_EXT_E)
   {
	   return (externaUserId - XMLDB_IAE_ENUM_JUMP);
   }
   //PPE
   else if (externaUserId <= NU4100_PPE_RPU1_STITCH_PAD_CFG_PAD_VAL_EXT_E)
   {
	   return (externaUserId - XMLDB_PPE_ENUM_JUMP);
   }
   //DPE
   else if (externaUserId <= NU4100_DPE_UIM_RIGHT_P1_UIM_BLOB_CFG_MAX_SIZE_EXT_E)
   {
	   return (externaUserId - XMLDB_DPE_ENUM_JUMP);
   }
   //CVA
   else if (externaUserId <= NU4100_CVA_AXI_IF6_CMD_CREDIT_FLUSH_EXT_E)
   {
	   return (externaUserId - XMLDB_CVA_ENUM_JUMP);
   }
   //IPE
   else if (externaUserId <= NU4100_IPE_ISP1_CH1_FRAME_INFO_FRAME_TS_MSB_TS_EXT_E)
   {
       return (externaUserId - XMLDB_IPE_ENUM_JUMP);
   }
   //META
   else if (externaUserId <= XMLDB_NUM_PATHS_EXT_E)
   {
	   return (externaUserId - XMLDB_META_ENUM_JUMP);
   }

   assert(0);
   return 0;
}

/*
Set the text value of node as u32 in the matching db.
If the text is not empty - marks the entry as not empty and clean.
If the text is empty or could not be converted the entry is marked empty.
If lookup fails - do nothing to the entry.
*/
static void readValueFromDoc(XMLNode* node, void *arg)
{
   int ret;
   XMLDB_valueT *valp;
   UINT32 newval = 0;
   XMLDB_dbT *dbp = (XMLDB_dbT *)arg;
   const char* userId = NULL;
   const char* value = NULL;
   const char* startBitOffset = NULL;
   const char* width = NULL;
   const char* blockStartAddress = NULL;
   const char* subblockStartAddress = NULL;
   const char* regOffset = NULL;
   const char* resetVal = NULL;
   const char* virtualField = NULL;
   UINT32 blockAddInt = 0, subblockAddInt = 0;
   INT32 userIdInt;

   //if there is no user_id - it is no field, do nothing.
   ret = getAttributeValue(node, "USER_ID",&userId);//It is assumed that each field has a user_ID
   userIdInt = atoi(userId);
   if ((ret == true) && (userIdInt!=-1) && (userIdInt < XMLDB_NUM_PATHS_EXT_E))
   {
       ret = getAttributeValue(node, "VALUE", &value);
       if ((ret == true) && (strcmp(value, "-1") != 0))
       {
           getAttributeValue(node, "START_BIT_OFFSET", &startBitOffset);
           getAttributeValue(node, "WIDTH", &width);
           getAttributeValue(node, "VIRTUAL", &virtualField);

           getAttributeValue(node, "RESET_VAL", &resetVal);
           getRegFields(node, &regOffset, &blockStartAddress, &subblockStartAddress, "-1");

           valp = &dbp->valueTbl[XMLDB_extendedToRegularEnumConvert(userIdInt)];
           ret = text2u32(value, &newval);
           if (ret >= 0)
           {
               valp->val = newval;
               XMLDB_CLR_EMPTY(valp->flags);
               XMLDB_SET_CONST(valp->flags); //value taken from XML are treated as const
           }
           else //empty string
           {
               XMLDB_SET_EMPTY(valp->flags);
           }


           if (strcmp(virtualField,"-1") != 0)//we don't want to fill address values in virtual field cases. if()->elseif()
           {
               XMLDB_SET_VIRTUAL(valp->flags);
           }
           else if ((regOffset) && (blockStartAddress))
           {
               text2u8(startBitOffset, &(valp->regFieldParams.startBitOffset));
               text2u8(width, &valp->regFieldParams.width);
               text2u32(resetVal, &valp->resetVal);
               text2u16(regOffset, &valp->regFieldParams.regOffset);
               text2u32(blockStartAddress, &blockAddInt);
               text2u32(subblockStartAddress, &subblockAddInt);
               if (subblockStartAddress)
               {
                   blockAddInt += subblockAddInt;
               }
               valp->regFieldParams.blockAdd = blockAddInt;
            if (!XMLDB_IS_EMPTY(valp->flags))
            {
               ret = validateValByWidth(newval, valp->regFieldParams.width);
               if (ERRG_FAILED(ret))
               {
                  LOGG_PRINT(LOG_ERROR_E, NULL, "Mismatch between value and width entered in XML. USER_ID %d, width %d, val %d\n", userIdInt, width, newval);
                  assert(0);
               }
            }
           }
           XMLDB_CLR_DIRTY(valp->flags);
       }
   }
   free((void*)userId);
   free((void*)value);
   free((void*)startBitOffset);
   free((void*)width);
   free((void*)blockStartAddress);
   free((void*)subblockStartAddress);
   free((void*)regOffset);
   free((void*)resetVal);
   free((void*)virtualField);
}

/*
   Save value in db to xml node (as text).
   This is called for each node in the xml tree but only values
   marked modified are written.
   If lookup fails - ignore.
*/
static void saveValueToDoc(XMLNode* node, XMLDB_dbT *dbp, int clearDirty)
{
   char text[XMLDB_MAX_TEXT_LEN];
   XMLDB_valueT *valp;
   const char* userId = NULL;
   int ret;

   ret = getAttributeValue(node, "USER_ID", &userId);
   if ((ret == true) && (strcmp(userId, "-1") != 0))
   {
       valp = &dbp->valueTbl[XMLDB_extendedToRegularEnumConvert(atoi(userId))];
       if (XMLDB_IS_DIRTY(valp->flags))
       {
           if (!(XMLDB_IS_EMPTY(valp->flags)))
           {
               int len = snprintf(text, sizeof(text), "0x%08x", valp->val);
               if ((len > 0) && ((unsigned)len < sizeof(text)))
                   XMLNode_set_attribute(node,"VALUE", text); //TODO: handle error
           }
           else
           {
               XMLNode_remove_text(node);
           }
           if (clearDirty)
               XMLDB_CLR_DIRTY(valp->flags);
       }
   }
       free((void*)userId);

}

static void saveToDocAndClearDb(XMLNode* node, void *arg)
{
   saveValueToDoc(node, (XMLDB_dbT *)arg, 1);
}

static void saveToDoc(XMLNode* node, void *arg)
{
   saveValueToDoc(node, (XMLDB_dbT *)arg, 0);
}


bool XMLDB_isConst(UINT16 flags)
{
   return XMLDB_IS_CONST(flags);
}


/*
   Open a database. Allocates a values table accessed by hash index.
*/
ERRG_codeE XMLDB_open(XMLDB_dbH *h)
{
   ERRG_codeE retval = XMLDB__RET_SUCCESS;

   //Allocate hash value table and clear to 0
   *h = (XMLDB_dbT*)calloc(sizeof(XMLDB_dbT), 1);
   if (*h)
   {
      initDbFlags(*h);
#ifdef XMLDB_FAST_WRITE
      (*h)->fastWriteDbP = (XMLDB_fastWriteDb*)malloc(sizeof(XMLDB_fastWriteDb));
      if (!(*h)->fastWriteDbP)
      {
         retval = XMLDB__ERR_OUT_OF_MEM;;
         free(*h);
      }
      else
      {
         memset((*h)->fastWriteDbP,0,sizeof(XMLDB_fastWriteDb));
      }
#endif
   }
   else
   {
      retval = XMLDB__ERR_OUT_OF_MEM;
   }

   return retval;
}

ERRG_codeE XMLDB_duplicate(XMLDB_dbH h, XMLDB_dbH *dup)
{
   ERRG_codeE retval = XMLDB__RET_SUCCESS;

   //Allocate hash value table and clear to 0
   *dup = (XMLDB_dbT *)calloc(sizeof(XMLDB_dbT), 1);
   if (*dup)
   {
      memcpy(*dup,h,sizeof(XMLDB_dbT));
#ifdef XMLDB_FAST_WRITE
      (*dup)->fastWriteDbP = (XMLDB_fastWriteDb*)malloc(sizeof(XMLDB_fastWriteDb));
      if (!(*dup)->fastWriteDbP)
      {
         retval = XMLDB__ERR_OUT_OF_MEM;;
         free(*dup);
      }
      else
      {
         memcpy((*dup)->fastWriteDbP,h->fastWriteDbP,sizeof(XMLDB_fastWriteDb));
      }
#endif
   }
   else
   {
      retval = XMLDB__ERR_OUT_OF_MEM;
   }

   return retval;
}

/*
   Close the database
*/
void XMLDB_close(XMLDB_dbH h)
{
#ifdef XMLDB_FAST_WRITE
   if (h->fastWriteDbP) free(h->fastWriteDbP);
#endif
   if (h) free(h);
}

/*
   Return db value at path, or empty if value isn't set.
   value can be null to check for emptiness.
*/
ERRG_codeE XMLDB_getValue(XMLDB_dbH db, XMLDB_pathE path, UINT32 *value)
{
   XMLDB_PATH_CHECK(path);
   XMLDB_valueT *valp = &db->valueTbl[path];

   if (XMLDB_IS_EMPTY(valp->flags))
   {
      *value = valp->resetVal;
      return XMLDB__EMPTY_ENTRY;
   }

   if (value)
      *value = valp->val;

   return XMLDB__RET_SUCCESS;
}
//choose this func or the separate below
ERRG_codeE XMLDB_getRegFields(XMLDB_dbH db, XMLDB_pathE path, UINT32* resetVal, UINT32* blockAdd, UINT16* regOffset, UINT8* startBitOffset, UINT8* width)
{
   XMLDB_PATH_CHECK(path);
   (void)resetVal;
   XMLDB_valueT *valp = &db->valueTbl[path];

   if (XMLDB_IS_EMPTY(valp->flags))
      return XMLDB__EMPTY_ENTRY;

   *resetVal = valp->resetVal;
   *blockAdd = valp->regFieldParams.blockAdd;
   *regOffset = valp->regFieldParams.regOffset;
   *startBitOffset = valp->regFieldParams.startBitOffset;
   *width = valp->regFieldParams.width;

   return XMLDB__RET_SUCCESS;
}

ERRG_codeE XMLDB_getResetValue(XMLDB_dbH db, XMLDB_pathE path, UINT32 *value)
{
    XMLDB_PATH_CHECK(path);
    XMLDB_valueT *valp = &db->valueTbl[path];

    if (XMLDB_IS_EMPTY(valp->flags))
        return XMLDB__EMPTY_ENTRY;

    if (value)
      *value = valp->resetVal;
    (void)value;
    return XMLDB__RET_SUCCESS;
}

ERRG_codeE XMLDB_getBlockAdd(XMLDB_dbH db, XMLDB_pathE path, UINT32 *blockAddress)
{
    XMLDB_PATH_CHECK(path);
    XMLDB_valueT *valp = &db->valueTbl[path];

    if (XMLDB_IS_EMPTY(valp->flags))
        return XMLDB__EMPTY_ENTRY;

    if (blockAddress)
        *blockAddress = valp->regFieldParams.blockAdd;

    return XMLDB__RET_SUCCESS;
}

ERRG_codeE XMLDB_getRegOffset(XMLDB_dbH db, XMLDB_pathE path, UINT16 *regOffset)
{
    XMLDB_PATH_CHECK(path);
    XMLDB_valueT *valp = &db->valueTbl[path];

    if (XMLDB_IS_EMPTY(valp->flags))
        return XMLDB__EMPTY_ENTRY;

    if (regOffset)
        *regOffset = valp->regFieldParams.regOffset;

    return XMLDB__RET_SUCCESS;
}

ERRG_codeE XMLDB_getFieldStartBitOffset(XMLDB_dbH db, XMLDB_pathE path, UINT8 *startBitOffset)
{
    XMLDB_PATH_CHECK(path);
    XMLDB_valueT *valp = &db->valueTbl[path];

    if (XMLDB_IS_EMPTY(valp->flags))
        return XMLDB__EMPTY_ENTRY;

    if (startBitOffset)
        *startBitOffset = valp->regFieldParams.startBitOffset;

    return XMLDB__RET_SUCCESS;
}

ERRG_codeE XMLDB_getFieldWidth(XMLDB_dbH db, XMLDB_pathE path, UINT8 *value)
{
    XMLDB_PATH_CHECK(path);
    XMLDB_valueT *valp = &db->valueTbl[path];

    if (XMLDB_IS_EMPTY(valp->flags))
        return XMLDB__EMPTY_ENTRY;

    if (value)
        *value = valp->regFieldParams.width;

   return XMLDB__RET_SUCCESS;
}

#if DEFSG_IS_GP
//this function is wrapering 'XMLDB_setValue' and update fast db. from GP, you should use this function when you set value in xmldb.
ERRG_codeE XMLDB_setValueAndUpdateFastDb(XMLDB_dbH db, UINT32 blk, UINT32 inst, XMLDB_pathE field, UINT32 value)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   XMLDB_valueT *valp;
   UINT32 virtAddress,i;
   XMLDB_fastWriteBlock *fastWriteBlockP;

   //1. set value for db
   XMLDB_setValue(db, NUFLD_calcPath((NUFLD_blkE)blk, inst,field),value);

   //2. update fast db
   valp = &db->valueTbl[NUFLD_calcPath((NUFLD_blkE)blk, inst,field)];
   ret = MEM_MAPG_convertPhysicalToVirtual(valp->regFieldParams.blockAdd, &virtAddress);
   virtAddress += valp->regFieldParams.regOffset;
   fastWriteBlockP = &db->fastWriteDbP->blocks[(NUFLD_blkE)blk][inst];

   for (i=0;i<=fastWriteBlockP->numRegs;i++)
   {
      if (fastWriteBlockP->regs[i].virtAddress == virtAddress)
      {
         //LOGG_PRINT(LOG_DEBUG_E, 0, "found value for update, address 0x%x old vlaue %d new value %d\n",valp->regFieldParams.blockAdd + valp->regFieldParams.regOffset,fastWriteBlockP->regs[i].val,value);
         fastWriteBlockP->regs[i].val &= ~(((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width)) << valp->regFieldParams.startBitOffset);
         fastWriteBlockP->regs[i].val |= ((valp->val & ((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width))) << valp->regFieldParams.startBitOffset);
         return NUCFG__RET_SUCCESS;
      }
   }
   //here, we didn't find this register, add it
   fastWriteBlockP->numRegs++;
   //LOGG_PRINT(LOG_DEBUG_E, 0, "add new register, virt 0x%x old value %x, num regs %d\n",virtAddress,fastWriteBlockP->regs[fastWriteBlockP->numRegs].val,fastWriteBlockP->numRegs);
   fastWriteBlockP->regs[fastWriteBlockP->numRegs].virtAddress = virtAddress;
   fastWriteBlockP->regs[fastWriteBlockP->numRegs].val &= ~(((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width)) << valp->regFieldParams.startBitOffset);
   fastWriteBlockP->regs[fastWriteBlockP->numRegs].val |= ((valp->val & ((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width))) << valp->regFieldParams.startBitOffset);
   //LOGG_PRINT(LOG_DEBUG_E, 0, "add new register, virt 0x%x new value %x\n",virtAddress,fastWriteBlockP->regs[fastWriteBlockP->numRegs].val);


   return NUCFG__RET_SUCCESS;
}
#endif

ERRG_codeE XMLDB_setValue(XMLDB_dbH db, XMLDB_pathE path, UINT32 value)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   XMLDB_PATH_CHECK(path);
   XMLDB_valueT *valp = &db->valueTbl[path];

   if ((!XMLDB_IS_CONST(valp->flags)) && ((XMLDB_IS_EMPTY(valp->flags) || (valp->val != value))))
   {
      valp->val = value;
      XMLDB_CLR_EMPTY(valp->flags);
      XMLDB_SET_DIRTY(valp->flags);

      ret = validateValByWidth(value, valp->regFieldParams.width);
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_WARN_E, NULL, "Mismatch between value and width entered in XML. USER_ID %d, width %d, val %d\n", path, valp->regFieldParams.width, value);
         //assert(0);
      }
   }
   else
   {
      if (XMLDB_IS_CONST(valp->flags))
      {
          LOGG_PRINT(LOG_WARN_E, NULL, "Ignore write value %d to userid %d, value is const %d\n",value,path,valp->val);
          //assert(0);// - test assert, to check that xml is "clean"
      }
   }
   return XMLDB__RET_SUCCESS;
}

ERRG_codeE XMLDB_setRegFields(XMLDB_dbH db, XMLDB_pathE path, UINT32 resetVal,UINT32 blockAdd, UINT16 regOffset, UINT8 startBitOffset, UINT8 width)
{
   XMLDB_PATH_CHECK(path);
   XMLDB_valueT *valp = &db->valueTbl[path];

   if ((XMLDB_IS_EMPTY(valp->flags) || (valp->val != resetVal)|| (valp->val != blockAdd)|| (valp->val != regOffset)|| (valp->val != startBitOffset)|| (valp->val != width)))
   {
      valp->resetVal= resetVal;
      valp->regFieldParams.blockAdd= blockAdd;
      valp->regFieldParams.regOffset= regOffset;
      valp->regFieldParams.startBitOffset= startBitOffset;
      valp->regFieldParams.width= width;
      XMLDB_CLR_EMPTY(valp->flags);
      XMLDB_SET_DIRTY(valp->flags);
   }
   return XMLDB__RET_SUCCESS;
}


ERRG_codeE XMLDB_clearValue(XMLDB_dbH db, XMLDB_pathE path)
{
   XMLDB_PATH_CHECK(path);
   XMLDB_valueT *valp = &db->valueTbl[path];

   if (!(XMLDB_IS_EMPTY(valp->flags)))
   {
      XMLDB_SET_EMPTY(valp->flags);
      XMLDB_SET_DIRTY(valp->flags);
   }
   return XMLDB__RET_SUCCESS;
}
/*
   Load values from xml buffer into db.
*/
ERRG_codeE XMLDB_loadFromBuf(XMLDB_dbH db, const char *buf)
{
   XMLDoc doc;
   ERRG_codeE retval;

   XMLDoc_init(&doc);
   retval = walkXml(&doc, (char *)buf, readValueFromDoc, db);
   XMLDoc_free(&doc);

   return retval;
}

/*
   Write changed values back to xml buffer
   Parse the input xml and convert all modified db values back to output xml.
   Input: db - input db to write to buffer
          xmlbuf - input xml buffer matching the db
          xmlbufSize are the xml buffer and its size.
          outbuf - output buffer to write to with modified xml buffer - can be the same as xmlbuf.
          outBufSize - size of outbuf, on return will hold the size of the output plus 1 (EOS).
   if outbuf is null will only return the outBufSize;
   Returns success, error if an IO error occurred.

   Implementation Notes:
   sxmlc only writes to a file (not buffer).
   Although linux has fmemopen write to a buffer like writing to a file, windows doesn't
   So open a temp file and write to it, copy to user buffer, and then delete the file.
   Another option is since sxmlc uses fprintf we could replace that with sprintf
   (directly to user buffer) in the sxmlc code.
*/
ERRG_codeE XMLDB_saveToBuf(XMLDB_dbH db, const char *xmlbuf, char *outbuf, unsigned int *outbufSize)
{
   ERRG_codeE retval = XMLDB__RET_SUCCESS;
   FILE *tmpFile;
   size_t  xmlsize, readlen;
   XMLDoc doc;
   int ret;

   //Open a tmpfile - automatically deleted on close by system
   retval = OS_LYRG_createTmpFile(&tmpFile);
   if ERRG_FAILED(retval)
   {
      return retval;
   }

   doc.init_value = 0;
   if (ERRG_SUCCEEDED(retval))
   {
      //Initialize a doc and walk the xml writing modified values.
      //We clear the dirty bits in the db only if outbuf is not null - so changes are not lost.
      XMLDoc_init(&doc);
      retval = walkXml(&doc, (char *)xmlbuf, (outbuf) ? saveToDocAndClearDb : saveToDoc, db);
   }

   if (ERRG_SUCCEEDED(retval))
   {
      //Print doc to tmp file
      ret = XMLDoc_print_attr_sep(&doc, tmpFile, "\n", "\t", NULL, 0, 0, 0);
      if (ret)
         fflush(tmpFile);
      else
         retval = XMLDB__ERR_IO_ERROR;
   }

   if (ERRG_SUCCEEDED(retval))
   {
      //Get file size
      fseek(tmpFile, 0L, SEEK_END);
      xmlsize = ftell(tmpFile);
      fseek(tmpFile, 0L, SEEK_SET);

      //read to output buffer requeseted
      if (outbuf && (xmlsize > 0))
      {
         readlen = (outbufSize && (*outbufSize - 1 < xmlsize)) ? *outbufSize - 1 : xmlsize;
         size_t rret = fread((void *)outbuf, 1, readlen, tmpFile);
         if (rret != readlen)
         {
            retval = XMLDB__ERR_IO_ERROR;
         }
         outbuf[readlen] = '\0';//EOS
      }
      if (outbufSize)
         *outbufSize = (unsigned int)xmlsize + 1; //set xml output size
   }

   //cleanup
   if (tmpFile)
      fclose(tmpFile);
   if (doc.init_value == XML_INIT_DONE)
      XMLDoc_free(&doc);

   return retval;
}

int XMLDB_loadNuSocxml(char **xmlbuf, unsigned int *xmlsize, const char *xmlFileName)
{
   int ret = 0;
   char *buf = NULL;
   unsigned int size;

   //open and load xml file
   FILE *xmlfile = fopen(xmlFileName, "r");
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
         printf("read file error\n");
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

void XMLDB_writeXml(const char *xmlbuf, const char *name, size_t size)
{
   FILE *xmlfile = fopen(name, "w");
   if(xmlfile)
   {
      fwrite(xmlbuf, size, 1, xmlfile);
      fflush(xmlfile);
      fclose(xmlfile);
   }
}

ERRG_codeE XMLDB_saveDb(XMLDB_dbH db, const char *xmlbuf, unsigned int *outSize, char **outBuf)
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
   // nothing is written on second pass because db is
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


ERRG_codeE XMLDB_exportTblP(XMLDB_dbH db, const char **buf, UINT32 *outSize)
{
#ifdef __UART_ON_FPGA__
//to send just modified db value
    int i,j;
    UINT32 cnt = 0;
    XMLDB_senderT *newArr;
    for (i = 0; i < XMLDB_NUM_PATHS_E; i++)
    {
        if (!XMLDB_IS_EMPTY(db->valueTbl[i].flags))
        {
            cnt++;
        }
    }
    newArr = (XMLDB_senderT *)malloc(cnt * sizeof(XMLDB_senderT));
    j = 0;
    for (i = 0; i < XMLDB_NUM_PATHS_E; i++)
    {
        if (!XMLDB_IS_EMPTY(db->valueTbl[i].flags))
        {
            newArr[j].valueTbl = db->valueTbl[i];
            //printf("J=%d I=%d, value = %d\n",j,i,newArr[j].valueTbl.val);
            newArr[j++].origIndex = i;
        }
    }
    *buf = (char*)newArr;
    *outSize = sizeof(XMLDB_senderT) * cnt;
#else
//to send all db value
    *buf = (char*)&(db->valueTbl);
    *outSize = sizeof(XMLDB_valueT) * XMLDB_NUM_PATHS_E;
#endif
    return XMLDB__RET_SUCCESS;
}

ERRG_codeE XMLDB_importTbl(const char *buf, UINT32 bufSize, XMLDB_dbH db)
{

#ifdef __UART_ON_FPGA__
//to send just modified db value
   int i,index;
   int arrSize = bufSize / sizeof(XMLDB_senderT);
   XMLDB_senderT* arr = (XMLDB_senderT*) buf;

  //printf("XML_DB arr size %d\n",arrSize);

   for (i=0;i< arrSize;i++)
   {
      index=arr[i].origIndex;
      //printf("XML_DB origIndex %d val %d (%d)\n",index,arr[i].valueTbl.val,i);
      db->valueTbl[index] = arr[i].valueTbl;
   }
#else
//to send all db value
   if (bufSize != (sizeof(XMLDB_valueT) * XMLDB_NUM_PATHS_E))
   {
       LOGG_PRINT(LOG_ERROR_E, NULL, "Wrong size of data (%d != %d)\n",bufSize,sizeof(XMLDB_valueT) * XMLDB_NUM_PATHS_E);
       return XMLDB__ERR_UNEXPECTED;
   }
   memcpy(db->valueTbl, buf, bufSize);
#endif

   return XMLDB__RET_SUCCESS;

}

bool XMLDB_checkIfAllTblIsEmpty(XMLDB_dbH db)
{
   UINT32 i=0;
   bool isEmpty =TRUE;

   for (;i<XMLDB_NUM_PATHS_E;i++)
   {
      if (XMLDB_IS_EMPTY(db->valueTbl[i].flags))
         isEmpty=FALSE;
   }
   return isEmpty;
}

void XMLDB_setDebugMode(int debugMode)
{
   enableConstMode = debugMode;
}

#if DEFSG_IS_GP
static ERRG_codeE XMLDB_writeEntryToReg(XMLDB_valueT *valp)
{
   UINT32 virtAddress;
   UINT32 tmp;
   ERRG_codeE ret = XMLDB__RET_SUCCESS;

   if ((!(XMLDB_IS_EMPTY(valp->flags))) && (!(XMLDB_IS_VIRTUAL(valp->flags))) && (valp->regFieldParams.blockAdd))
   {
      ret = MEM_MAPG_convertPhysicalToVirtual(valp->regFieldParams.blockAdd, &virtAddress);
      if (ERRG_SUCCEEDED(ret))
      {
         ret = validateValByWidth(valp->val, valp->regFieldParams.width);
         if (ERRG_FAILED(ret))
         {
            //LOGG_PRINT(LOG_ERROR_E, NULL, "Mismatch between value and width entered in XML. USER_ID %d, width %d, val %d\n", i, valp->regFieldParams.width, valp->val);
            //assert(0);
         }

         virtAddress += valp->regFieldParams.regOffset;
         tmp = *((volatile UINT32 *)virtAddress);
         tmp &= ~(((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width)) << valp->regFieldParams.startBitOffset);
         tmp |= ((valp->val & ((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width))) << valp->regFieldParams.startBitOffset);
         //printf("%d: Reg 0x%x = 0x%x  pys block 0x%x reg offset 0x%x userid %d\n",i, ((volatile UINT32 *)virtAddress), tmp, valp->regFieldParams.blockAdd, valp->regFieldParams.regOffset,i);
         *((volatile UINT32 *)virtAddress) = tmp;
      }
   }
   return ret;
}


ERRG_codeE XMLDB_writeFieldToRegFromDb(XMLDB_dbH db, XMLDB_pathE path)
{
   ERRG_codeE ret = XMLDB__RET_SUCCESS;
   XMLDB_valueT *valp = &db->valueTbl[path];
   return XMLDB_writeEntryToReg(valp);
}


ERRG_codeE XMLDB_writeFieldToReg(XMLDB_dbH db, XMLDB_pathE path, UINT32 val)
{
   UINT32 virtAddress;
   UINT32 tmp;
   ERRG_codeE ret = XMLDB__RET_SUCCESS;

   XMLDB_valueT *valp = &db->valueTbl[path];
   ret = MEM_MAPG_convertPhysicalToVirtual(valp->regFieldParams.blockAdd, &virtAddress);

   if (ERRG_SUCCEEDED(ret))
   {
      ret = validateValByWidth(val, valp->regFieldParams.width);
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Mismatch between value and width entered in XML. USER_ID %d, width %d, val %d\n", path, valp->regFieldParams.width, val);
         assert(0);
      }

      virtAddress += valp->regFieldParams.regOffset;
      tmp = *((volatile UINT32 *)virtAddress);
      tmp &= ~(((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width)) << valp->regFieldParams.startBitOffset);
      tmp |= ((val & ((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width))) << valp->regFieldParams.startBitOffset);
      *((volatile UINT32 *)virtAddress) = tmp;
   }
   return ret;
}

//#define SANITY_VIRT_CHECK
ERRG_codeE XMLDB_writeBlockDbToRegs(XMLDB_dbH db, UINT32 block, UINT32 blkInstance)
{
   ERRG_codeE ret = XMLDB__RET_SUCCESS;
   XMLDB_pathE startBlockPath;
   UINT32 i, blkSize;
#ifdef XMLDB_FAST_WRITE
   XMLDB_fastWriteBlock *fastWriteBlockP;
#endif

   if (block < NUFLD_META_READERS_E)
   {
      if (block == NUFLD_DPP_E)
      {
         blkInstance = 0;
      }
#ifdef XMLDB_FAST_WRITE
      fastWriteBlockP = &db->fastWriteDbP->blocks[block][blkInstance];

      if ((fastWriteBlockP->numRegs == 1) && (!fastWriteBlockP->regs[0].virtAddress))
         return ret;

      if ((fastWriteBlockP->numRegs == 0) && (!fastWriteBlockP->regs[0].virtAddress))
         return ret;


      //printf("Start write block %d, instance %d, numRegs %d\n",block,blkInstance,fastWriteBlockP->numRegs);
      for (i = 0; i <= fastWriteBlockP->numRegs; i++)
      {
#ifdef SANITY_VIRT_CHECK
         if (!fastWriteBlockP->regs[i].virtAddress)
         {
            printf("virtual address 0 at reg %d! (blk %d, instance %d, numRegs %d)\n",i,block,blkInstance,fastWriteBlockP->numRegs);
            assert(0);
         }
#endif
         *(volatile UINT32*)fastWriteBlockP->regs[i].virtAddress = fastWriteBlockP->regs[i].val;
#ifdef SANITY_VIRT_CHECK
         if(*(volatile UINT32*)fastWriteBlockP->regs[i].virtAddress != fastWriteBlockP->regs[i].val)
         {
            printf("0x%x: 0x%.8x != 0x%.8x (blk %d, inst %d)\n",
                    fastWriteBlockP->regs[i].virtAddress,*(volatile UINT32*)fastWriteBlockP->regs[i].virtAddress,fastWriteBlockP->regs[i].val,
                    block,blkInstance);
         }
#endif
      }
#else
      //printf("WRITE %d instance %d\n",block,blkInstance);
      startBlockPath = NUFLD_startBlockPath(block, blkInstance);
      blkSize        = NUFLD_getBlkSize(block, blkInstance);
      for (i = 0; i < blkSize; i++)
      {
         ret = XMLDB_writeEntryToReg(&db->valueTbl[startBlockPath + i]);
      }
#endif
   }
   else
   {
      ret = XMLDB__ERR_INVALID_ARGS;
   }

   return ret;
}

ERRG_codeE XMLDB_fillFastWriteBlock(XMLDB_dbH db, UINT32 block, UINT32 blkInstance)
{
   ERRG_codeE ret = XMLDB__RET_SUCCESS;
#ifdef XMLDB_FAST_WRITE
   XMLDB_pathE startBlockPath;
   UINT32 i, blkSize,resetVal;
   XMLDB_valueT *valp;
   XMLDB_fastWriteBlock *fastWriteBlockP;
   UINT32 virtAddress,lastVirtAddress = ~(0);

   if (block < NUFLD_META_READERS_E)
   {
      if (block == NUFLD_DPP_E)
      {
         blkInstance = 0;
      }
      fastWriteBlockP = &db->fastWriteDbP->blocks[block][blkInstance];
      if (fastWriteBlockP->configured)
         return ret;

      startBlockPath = NUFLD_startBlockPath(block, blkInstance);
      blkSize        = NUFLD_getBlkSize(block, blkInstance);
      fastWriteBlockP->configured = 1;
      for (i = 0; i < blkSize; i++)
      {
         valp = &db->valueTbl[startBlockPath + i];
         if ((!(XMLDB_IS_EMPTY(valp->flags))) && (!(XMLDB_IS_VIRTUAL(valp->flags))) && (valp->regFieldParams.blockAdd))
         {
            ret = MEM_MAPG_convertPhysicalToVirtual(valp->regFieldParams.blockAdd, &virtAddress);
            if (ERRG_SUCCEEDED(ret))
            {
               ret = validateValByWidth(valp->val, valp->regFieldParams.width);

               if (blkInstance < XMLDB_MAX_NUM_BLKS_INST_FAST_WRITE)
               {
                  virtAddress += valp->regFieldParams.regOffset;

                  //if new register
                  if (lastVirtAddress == ((UINT32)(~(0))))
                  {
                     fastWriteBlockP->numRegs = 0;
                     resetVal = valp->resetVal;
                  }
                  else
                  {
                     if (virtAddress != lastVirtAddress)
                     {
                        if (resetVal != fastWriteBlockP->regs[fastWriteBlockP->numRegs].val)
                        {
                           //printf("new reg (0x%x). current num regs %d. 0x%x = 0x%x\n",(valp - 1)->regFieldParams.regOffset,
                           //         fastWriteBlockP->numRegs,fastWriteBlockP->regs[fastWriteBlockP->numRegs].virtAddress,
                           //         fastWriteBlockP->regs[fastWriteBlockP->numRegs].val);

                           fastWriteBlockP->numRegs++;
                           if (fastWriteBlockP->numRegs == XMLDB_MAX_NUM_REGS_FAST_WRITE)
                           {
                              LOGG_PRINT(LOG_ERROR_E, NULL, "Reached max number of registers for block %d instance %d\n", block, blkInstance);
                              assert(0);
                           }
                        }
                        else
                        {
                           //the built register is same as the reset value, discard it
                           fastWriteBlockP->regs[fastWriteBlockP->numRegs].val = 0;
                        }
                        resetVal = valp->resetVal;
                     }
                     else
                     {
                        //same register, fill field
                     }
                  }

                  fastWriteBlockP->regs[fastWriteBlockP->numRegs].virtAddress = virtAddress;
                  fastWriteBlockP->regs[fastWriteBlockP->numRegs].val &= ~(((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width)) << valp->regFieldParams.startBitOffset);
                  fastWriteBlockP->regs[fastWriteBlockP->numRegs].val |= ((valp->val & ((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width))) << valp->regFieldParams.startBitOffset);
                  lastVirtAddress = virtAddress;
               }
            }
         }
      }
   }
   else
   {
      ret = XMLDB__ERR_INVALID_ARGS;
   }
#endif
   return ret;
}

ERRG_codeE XMLDB_writeDbToRegs(XMLDB_dbH db, UINT32 isMainGraph)
{
   ERRG_codeE ret = XMLDB__RET_SUCCESS;
   UINT32 virtAddress;
   int i,j=0,z,k=0;
   UINT32 tmp;
   UINT32 arrLutPpeSclBilin[384];
   UINT32 arrLutDpeSclBilin[512];

// PPE - Horizontal0
   arrLutPpeSclBilin[0] =0x00000000;
   arrLutPpeSclBilin[1] =0x7fff0000;
   arrLutPpeSclBilin[2] =0x00000000;
   arrLutPpeSclBilin[3] =0x00000000;
   arrLutPpeSclBilin[4] =0x00000000;
   arrLutPpeSclBilin[5] =0x77ff0000;
   arrLutPpeSclBilin[6] =0x00000800;
   arrLutPpeSclBilin[7] =0x00000000;
   arrLutPpeSclBilin[8] =0x00000000;
   arrLutPpeSclBilin[9] =0x6fff0000;
   arrLutPpeSclBilin[10] =0x00001000;
   arrLutPpeSclBilin[11] =0x00000000;
   arrLutPpeSclBilin[12] =0x00000000;
   arrLutPpeSclBilin[13] =0x67ff0000;
   arrLutPpeSclBilin[14] =0x00001800;
   arrLutPpeSclBilin[15] =0x00000000;
   arrLutPpeSclBilin[16] =0x00000000;
   arrLutPpeSclBilin[17] =0x5fff0000;
   arrLutPpeSclBilin[18] =0x00002000;
   arrLutPpeSclBilin[19] =0x00000000;
   arrLutPpeSclBilin[20] =0x00000000;
   arrLutPpeSclBilin[21] =0x57ff0000;
   arrLutPpeSclBilin[22] =0x00002800;
   arrLutPpeSclBilin[23] =0x00000000;
   arrLutPpeSclBilin[24] =0x00000000;
   arrLutPpeSclBilin[25] =0x4fff0000;
   arrLutPpeSclBilin[26] =0x00003000;
   arrLutPpeSclBilin[27] =0x00000000;
   arrLutPpeSclBilin[28] =0x00000000;
   arrLutPpeSclBilin[29] =0x47ff0000;
   arrLutPpeSclBilin[30] =0x00003800;
   arrLutPpeSclBilin[31] =0x00000000;
   arrLutPpeSclBilin[32] =0x00000000;
   arrLutPpeSclBilin[33] =0x40000000;
   arrLutPpeSclBilin[34] =0x00004000;
   arrLutPpeSclBilin[35] =0x00000000;
   arrLutPpeSclBilin[36] =0x00000000;
   arrLutPpeSclBilin[37] =0x38000000;
   arrLutPpeSclBilin[38] =0x000047ff;
   arrLutPpeSclBilin[39] =0x00000000;
   arrLutPpeSclBilin[40] =0x00000000;
   arrLutPpeSclBilin[41] =0x30000000;
   arrLutPpeSclBilin[42] =0x00004fff;
   arrLutPpeSclBilin[43] =0x00000000;
   arrLutPpeSclBilin[44] =0x00000000;
   arrLutPpeSclBilin[45] =0x28000000;
   arrLutPpeSclBilin[46] =0x000057ff;
   arrLutPpeSclBilin[47] =0x00000000;
   arrLutPpeSclBilin[48] =0x00000000;
   arrLutPpeSclBilin[49] =0x20000000;
   arrLutPpeSclBilin[50] =0x00005fff;
   arrLutPpeSclBilin[51] =0x00000000;
   arrLutPpeSclBilin[52] =0x00000000;
   arrLutPpeSclBilin[53] =0x18000000;
   arrLutPpeSclBilin[54] =0x000067ff;
   arrLutPpeSclBilin[55] =0x00000000;
   arrLutPpeSclBilin[56] =0x00000000;
   arrLutPpeSclBilin[57] =0x10000000;
   arrLutPpeSclBilin[58] =0x00006fff;
   arrLutPpeSclBilin[59] =0x00000000;
   arrLutPpeSclBilin[60] =0x00000000;
   arrLutPpeSclBilin[61] =0x08000000;
   arrLutPpeSclBilin[62] =0x000077ff;
   arrLutPpeSclBilin[63] =0x00000000;

// PPE - Vertical1
   arrLutPpeSclBilin[64] =0x00000000;
   arrLutPpeSclBilin[65] =0x00007fff;
   arrLutPpeSclBilin[66] =0x00000000;
   arrLutPpeSclBilin[67] =0x00000000;
   arrLutPpeSclBilin[68] =0x00000000;
   arrLutPpeSclBilin[69] =0x080077ff;
   arrLutPpeSclBilin[70] =0x00000000;
   arrLutPpeSclBilin[71] =0x00000000;
   arrLutPpeSclBilin[72] =0x00000000;
   arrLutPpeSclBilin[73] =0x10006fff;
   arrLutPpeSclBilin[74] =0x00000000;
   arrLutPpeSclBilin[75] =0x00000000;
   arrLutPpeSclBilin[76] =0x00000000;
   arrLutPpeSclBilin[77] =0x180067ff;
   arrLutPpeSclBilin[78] =0x00000000;
   arrLutPpeSclBilin[79] =0x00000000;
   arrLutPpeSclBilin[80] =0x00000000;
   arrLutPpeSclBilin[81] =0x20005fff;
   arrLutPpeSclBilin[82] =0x00000000;
   arrLutPpeSclBilin[83] =0x00000000;
   arrLutPpeSclBilin[84] =0x00000000;
   arrLutPpeSclBilin[85] =0x280057ff;
   arrLutPpeSclBilin[86] =0x00000000;
   arrLutPpeSclBilin[87] =0x00000000;
   arrLutPpeSclBilin[88] =0x00000000;
   arrLutPpeSclBilin[89] =0x30004fff;
   arrLutPpeSclBilin[90] =0x00000000;
   arrLutPpeSclBilin[91] =0x00000000;
   arrLutPpeSclBilin[92] =0x00000000;
   arrLutPpeSclBilin[93] =0x380047ff;
   arrLutPpeSclBilin[94] =0x00000000;
   arrLutPpeSclBilin[95] =0x00000000;
   arrLutPpeSclBilin[96] =0x00000000;
   arrLutPpeSclBilin[97] =0x40004000;
   arrLutPpeSclBilin[98] =0x00000000;
   arrLutPpeSclBilin[99] =0x00000000;
   arrLutPpeSclBilin[100] =0x00000000;
   arrLutPpeSclBilin[101] =0x47ff3800;
   arrLutPpeSclBilin[102] =0x00000000;
   arrLutPpeSclBilin[103] =0x00000000;
   arrLutPpeSclBilin[104] =0x00000000;
   arrLutPpeSclBilin[105] =0x4fff3000;
   arrLutPpeSclBilin[106] =0x00000000;
   arrLutPpeSclBilin[107] =0x00000000;
   arrLutPpeSclBilin[108] =0x00000000;
   arrLutPpeSclBilin[109] =0x57ff2800;
   arrLutPpeSclBilin[110] =0x00000000;
   arrLutPpeSclBilin[111] =0x00000000;
   arrLutPpeSclBilin[112] =0x00000000;
   arrLutPpeSclBilin[113] =0x5fff2000;
   arrLutPpeSclBilin[114] =0x00000000;
   arrLutPpeSclBilin[115] =0x00000000;
   arrLutPpeSclBilin[116] =0x00000000;
   arrLutPpeSclBilin[117] =0x67ff1800;
   arrLutPpeSclBilin[118] =0x00000000;
   arrLutPpeSclBilin[119] =0x00000000;
   arrLutPpeSclBilin[120] =0x00000000;
   arrLutPpeSclBilin[121] =0x6fff1000;
   arrLutPpeSclBilin[122] =0x00000000;
   arrLutPpeSclBilin[123] =0x00000000;
   arrLutPpeSclBilin[124] =0x00000000;
   arrLutPpeSclBilin[125] =0x77ff0800;
   arrLutPpeSclBilin[126] =0x00000000;
   arrLutPpeSclBilin[127] =0x00000000;

// PPE - Horizontal2
   arrLutPpeSclBilin[128] =0x00000000;
   arrLutPpeSclBilin[129] =0x7fff0000;
   arrLutPpeSclBilin[130] =0x00000000;
   arrLutPpeSclBilin[131] =0x00000000;
   arrLutPpeSclBilin[132] =0x00000000;
   arrLutPpeSclBilin[133] =0x77ff0000;
   arrLutPpeSclBilin[134] =0x00000800;
   arrLutPpeSclBilin[135] =0x00000000;
   arrLutPpeSclBilin[136] =0x00000000;
   arrLutPpeSclBilin[137] =0x6fff0000;
   arrLutPpeSclBilin[138] =0x00001000;
   arrLutPpeSclBilin[139] =0x00000000;
   arrLutPpeSclBilin[140] =0x00000000;
   arrLutPpeSclBilin[141] =0x67ff0000;
   arrLutPpeSclBilin[142] =0x00001800;
   arrLutPpeSclBilin[143] =0x00000000;
   arrLutPpeSclBilin[144] =0x00000000;
   arrLutPpeSclBilin[145] =0x5fff0000;
   arrLutPpeSclBilin[146] =0x00002000;
   arrLutPpeSclBilin[147] =0x00000000;
   arrLutPpeSclBilin[148] =0x00000000;
   arrLutPpeSclBilin[149] =0x57ff0000;
   arrLutPpeSclBilin[150] =0x00002800;
   arrLutPpeSclBilin[151] =0x00000000;
   arrLutPpeSclBilin[152] =0x00000000;
   arrLutPpeSclBilin[153] =0x4fff0000;
   arrLutPpeSclBilin[154] =0x00003000;
   arrLutPpeSclBilin[155] =0x00000000;
   arrLutPpeSclBilin[156] =0x00000000;
   arrLutPpeSclBilin[157] =0x47ff0000;
   arrLutPpeSclBilin[158] =0x00003800;
   arrLutPpeSclBilin[159] =0x00000000;
   arrLutPpeSclBilin[160] =0x00000000;
   arrLutPpeSclBilin[161] =0x40000000;
   arrLutPpeSclBilin[162] =0x00004000;
   arrLutPpeSclBilin[163] =0x00000000;
   arrLutPpeSclBilin[164] =0x00000000;
   arrLutPpeSclBilin[165] =0x38000000;
   arrLutPpeSclBilin[166] =0x000047ff;
   arrLutPpeSclBilin[167] =0x00000000;
   arrLutPpeSclBilin[168] =0x00000000;
   arrLutPpeSclBilin[169] =0x30000000;
   arrLutPpeSclBilin[170] =0x00004fff;
   arrLutPpeSclBilin[171] =0x00000000;
   arrLutPpeSclBilin[172] =0x00000000;
   arrLutPpeSclBilin[173] =0x28000000;
   arrLutPpeSclBilin[174] =0x000057ff;
   arrLutPpeSclBilin[175] =0x00000000;
   arrLutPpeSclBilin[176] =0x00000000;
   arrLutPpeSclBilin[177] =0x20000000;
   arrLutPpeSclBilin[178] =0x00005fff;
   arrLutPpeSclBilin[179] =0x00000000;
   arrLutPpeSclBilin[180] =0x00000000;
   arrLutPpeSclBilin[181] =0x18000000;
   arrLutPpeSclBilin[182] =0x000067ff;
   arrLutPpeSclBilin[183] =0x00000000;
   arrLutPpeSclBilin[184] =0x00000000;
   arrLutPpeSclBilin[185] =0x10000000;
   arrLutPpeSclBilin[186] =0x00006fff;
   arrLutPpeSclBilin[187] =0x00000000;
   arrLutPpeSclBilin[188] =0x00000000;
   arrLutPpeSclBilin[189] =0x08000000;
   arrLutPpeSclBilin[190] =0x000077ff;
   arrLutPpeSclBilin[191] =0x00000000;

// PPE - Vertical3
   arrLutPpeSclBilin[192] =0x00000000;
   arrLutPpeSclBilin[193] =0x00007fff;
   arrLutPpeSclBilin[194] =0x00000000;
   arrLutPpeSclBilin[195] =0x00000000;
   arrLutPpeSclBilin[196] =0x00000000;
   arrLutPpeSclBilin[197] =0x080077ff;
   arrLutPpeSclBilin[198] =0x00000000;
   arrLutPpeSclBilin[199] =0x00000000;
   arrLutPpeSclBilin[200] =0x00000000;
   arrLutPpeSclBilin[201] =0x10006fff;
   arrLutPpeSclBilin[202] =0x00000000;
   arrLutPpeSclBilin[203] =0x00000000;
   arrLutPpeSclBilin[204] =0x00000000;
   arrLutPpeSclBilin[205] =0x180067ff;
   arrLutPpeSclBilin[206] =0x00000000;
   arrLutPpeSclBilin[207] =0x00000000;
   arrLutPpeSclBilin[208] =0x00000000;
   arrLutPpeSclBilin[209] =0x20005fff;
   arrLutPpeSclBilin[210] =0x00000000;
   arrLutPpeSclBilin[211] =0x00000000;
   arrLutPpeSclBilin[212] =0x00000000;
   arrLutPpeSclBilin[213] =0x280057ff;
   arrLutPpeSclBilin[214] =0x00000000;
   arrLutPpeSclBilin[215] =0x00000000;
   arrLutPpeSclBilin[216] =0x00000000;
   arrLutPpeSclBilin[217] =0x30004fff;
   arrLutPpeSclBilin[218] =0x00000000;
   arrLutPpeSclBilin[219] =0x00000000;
   arrLutPpeSclBilin[220] =0x00000000;
   arrLutPpeSclBilin[221] =0x380047ff;
   arrLutPpeSclBilin[222] =0x00000000;
   arrLutPpeSclBilin[223] =0x00000000;
   arrLutPpeSclBilin[224] =0x00000000;
   arrLutPpeSclBilin[225] =0x40004000;
   arrLutPpeSclBilin[226] =0x00000000;
   arrLutPpeSclBilin[227] =0x00000000;
   arrLutPpeSclBilin[228] =0x00000000;
   arrLutPpeSclBilin[229] =0x47ff3800;
   arrLutPpeSclBilin[230] =0x00000000;
   arrLutPpeSclBilin[231] =0x00000000;
   arrLutPpeSclBilin[232] =0x00000000;
   arrLutPpeSclBilin[233] =0x4fff3000;
   arrLutPpeSclBilin[234] =0x00000000;
   arrLutPpeSclBilin[235] =0x00000000;
   arrLutPpeSclBilin[236] =0x00000000;
   arrLutPpeSclBilin[237] =0x57ff2800;
   arrLutPpeSclBilin[238] =0x00000000;
   arrLutPpeSclBilin[239] =0x00000000;
   arrLutPpeSclBilin[240] =0x00000000;
   arrLutPpeSclBilin[241] =0x5fff2000;
   arrLutPpeSclBilin[242] =0x00000000;
   arrLutPpeSclBilin[243] =0x00000000;
   arrLutPpeSclBilin[244] =0x00000000;
   arrLutPpeSclBilin[245] =0x67ff1800;
   arrLutPpeSclBilin[246] =0x00000000;
   arrLutPpeSclBilin[247] =0x00000000;
   arrLutPpeSclBilin[248] =0x00000000;
   arrLutPpeSclBilin[249] =0x6fff1000;
   arrLutPpeSclBilin[250] =0x00000000;
   arrLutPpeSclBilin[251] =0x00000000;
   arrLutPpeSclBilin[252] =0x00000000;
   arrLutPpeSclBilin[253] =0x77ff0800;
   arrLutPpeSclBilin[254] =0x00000000;
   arrLutPpeSclBilin[255] =0x00000000;

// PPE - Horizontal4
   arrLutPpeSclBilin[256] =0x00000000;
   arrLutPpeSclBilin[257] =0x7fff0000;
   arrLutPpeSclBilin[258] =0x00000000;
   arrLutPpeSclBilin[259] =0x00000000;
   arrLutPpeSclBilin[260] =0x00000000;
   arrLutPpeSclBilin[261] =0x77ff0000;
   arrLutPpeSclBilin[262] =0x00000800;
   arrLutPpeSclBilin[263] =0x00000000;
   arrLutPpeSclBilin[264] =0x00000000;
   arrLutPpeSclBilin[265] =0x6fff0000;
   arrLutPpeSclBilin[266] =0x00001000;
   arrLutPpeSclBilin[267] =0x00000000;
   arrLutPpeSclBilin[268] =0x00000000;
   arrLutPpeSclBilin[269] =0x67ff0000;
   arrLutPpeSclBilin[270] =0x00001800;
   arrLutPpeSclBilin[271] =0x00000000;
   arrLutPpeSclBilin[272] =0x00000000;
   arrLutPpeSclBilin[273] =0x5fff0000;
   arrLutPpeSclBilin[274] =0x00002000;
   arrLutPpeSclBilin[275] =0x00000000;
   arrLutPpeSclBilin[276] =0x00000000;
   arrLutPpeSclBilin[277] =0x57ff0000;
   arrLutPpeSclBilin[278] =0x00002800;
   arrLutPpeSclBilin[279] =0x00000000;
   arrLutPpeSclBilin[280] =0x00000000;
   arrLutPpeSclBilin[281] =0x4fff0000;
   arrLutPpeSclBilin[282] =0x00003000;
   arrLutPpeSclBilin[283] =0x00000000;
   arrLutPpeSclBilin[284] =0x00000000;
   arrLutPpeSclBilin[285] =0x47ff0000;
   arrLutPpeSclBilin[286] =0x00003800;
   arrLutPpeSclBilin[287] =0x00000000;
   arrLutPpeSclBilin[288] =0x00000000;
   arrLutPpeSclBilin[289] =0x40000000;
   arrLutPpeSclBilin[290] =0x00004000;
   arrLutPpeSclBilin[291] =0x00000000;
   arrLutPpeSclBilin[292] =0x00000000;
   arrLutPpeSclBilin[293] =0x38000000;
   arrLutPpeSclBilin[294] =0x000047ff;
   arrLutPpeSclBilin[295] =0x00000000;
   arrLutPpeSclBilin[296] =0x00000000;
   arrLutPpeSclBilin[297] =0x30000000;
   arrLutPpeSclBilin[298] =0x00004fff;
   arrLutPpeSclBilin[299] =0x00000000;
   arrLutPpeSclBilin[300] =0x00000000;
   arrLutPpeSclBilin[301] =0x28000000;
   arrLutPpeSclBilin[302] =0x000057ff;
   arrLutPpeSclBilin[303] =0x00000000;
   arrLutPpeSclBilin[304] =0x00000000;
   arrLutPpeSclBilin[305] =0x20000000;
   arrLutPpeSclBilin[306] =0x00005fff;
   arrLutPpeSclBilin[307] =0x00000000;
   arrLutPpeSclBilin[308] =0x00000000;
   arrLutPpeSclBilin[309] =0x18000000;
   arrLutPpeSclBilin[310] =0x000067ff;
   arrLutPpeSclBilin[311] =0x00000000;
   arrLutPpeSclBilin[312] =0x00000000;
   arrLutPpeSclBilin[313] =0x10000000;
   arrLutPpeSclBilin[314] =0x00006fff;
   arrLutPpeSclBilin[315] =0x00000000;
   arrLutPpeSclBilin[316] =0x00000000;
   arrLutPpeSclBilin[317] =0x08000000;
   arrLutPpeSclBilin[318] =0x000077ff;
   arrLutPpeSclBilin[319] =0x00000000;

// PPE - Vertical5
   arrLutPpeSclBilin[320] =0x00000000;
   arrLutPpeSclBilin[321] =0x00007fff;
   arrLutPpeSclBilin[322] =0x00000000;
   arrLutPpeSclBilin[323] =0x00000000;
   arrLutPpeSclBilin[324] =0x00000000;
   arrLutPpeSclBilin[325] =0x080077ff;
   arrLutPpeSclBilin[326] =0x00000000;
   arrLutPpeSclBilin[327] =0x00000000;
   arrLutPpeSclBilin[328] =0x00000000;
   arrLutPpeSclBilin[329] =0x10006fff;
   arrLutPpeSclBilin[330] =0x00000000;
   arrLutPpeSclBilin[331] =0x00000000;
   arrLutPpeSclBilin[332] =0x00000000;
   arrLutPpeSclBilin[333] =0x180067ff;
   arrLutPpeSclBilin[334] =0x00000000;
   arrLutPpeSclBilin[335] =0x00000000;
   arrLutPpeSclBilin[336] =0x00000000;
   arrLutPpeSclBilin[337] =0x20005fff;
   arrLutPpeSclBilin[338] =0x00000000;
   arrLutPpeSclBilin[339] =0x00000000;
   arrLutPpeSclBilin[340] =0x00000000;
   arrLutPpeSclBilin[341] =0x280057ff;
   arrLutPpeSclBilin[342] =0x00000000;
   arrLutPpeSclBilin[343] =0x00000000;
   arrLutPpeSclBilin[344] =0x00000000;
   arrLutPpeSclBilin[345] =0x30004fff;
   arrLutPpeSclBilin[346] =0x00000000;
   arrLutPpeSclBilin[347] =0x00000000;
   arrLutPpeSclBilin[348] =0x00000000;
   arrLutPpeSclBilin[349] =0x380047ff;
   arrLutPpeSclBilin[350] =0x00000000;
   arrLutPpeSclBilin[351] =0x00000000;
   arrLutPpeSclBilin[352] =0x00000000;
   arrLutPpeSclBilin[353] =0x40004000;
   arrLutPpeSclBilin[354] =0x00000000;
   arrLutPpeSclBilin[355] =0x00000000;
   arrLutPpeSclBilin[356] =0x00000000;
   arrLutPpeSclBilin[357] =0x47ff3800;
   arrLutPpeSclBilin[358] =0x00000000;
   arrLutPpeSclBilin[359] =0x00000000;
   arrLutPpeSclBilin[360] =0x00000000;
   arrLutPpeSclBilin[361] =0x4fff3000;
   arrLutPpeSclBilin[362] =0x00000000;
   arrLutPpeSclBilin[363] =0x00000000;
   arrLutPpeSclBilin[364] =0x00000000;
   arrLutPpeSclBilin[365] =0x57ff2800;
   arrLutPpeSclBilin[366] =0x00000000;
   arrLutPpeSclBilin[367] =0x00000000;
   arrLutPpeSclBilin[368] =0x00000000;
   arrLutPpeSclBilin[369] =0x5fff2000;
   arrLutPpeSclBilin[370] =0x00000000;
   arrLutPpeSclBilin[371] =0x00000000;
   arrLutPpeSclBilin[372] =0x00000000;
   arrLutPpeSclBilin[373] =0x67ff1800;
   arrLutPpeSclBilin[374] =0x00000000;
   arrLutPpeSclBilin[375] =0x00000000;
   arrLutPpeSclBilin[376] =0x00000000;
   arrLutPpeSclBilin[377] =0x6fff1000;
   arrLutPpeSclBilin[378] =0x00000000;
   arrLutPpeSclBilin[379] =0x00000000;
   arrLutPpeSclBilin[380] =0x00000000;
   arrLutPpeSclBilin[381] =0x77ff0800;
   arrLutPpeSclBilin[382] =0x00000000;
   arrLutPpeSclBilin[383] =0x00000000;


   // DPE - Vertical - L0
   arrLutDpeSclBilin[0]  =0x7FFF0000;
   arrLutDpeSclBilin[1]  =0x0;
   arrLutDpeSclBilin[2]  =0x78000800;
   arrLutDpeSclBilin[3]  =0x0;
   arrLutDpeSclBilin[4]  =0x70001000;
   arrLutDpeSclBilin[5]  =0x0;
   arrLutDpeSclBilin[6]  =0x68001800;
   arrLutDpeSclBilin[7]  =0x0;
   arrLutDpeSclBilin[8]  =0x60002000;
   arrLutDpeSclBilin[9]  =0x0;
   arrLutDpeSclBilin[10] =0x58002800;
   arrLutDpeSclBilin[11] =0x0;
   arrLutDpeSclBilin[12] =0x50003000;
   arrLutDpeSclBilin[13] =0x0;
   arrLutDpeSclBilin[14] =0x48003800;
   arrLutDpeSclBilin[15] =0x0;
   arrLutDpeSclBilin[16] =0x40004000;
   arrLutDpeSclBilin[17] =0x0;
   arrLutDpeSclBilin[18] =0x38004800;
   arrLutDpeSclBilin[19] =0x0;
   arrLutDpeSclBilin[20] =0x30005000;
   arrLutDpeSclBilin[21] =0x0;
   arrLutDpeSclBilin[22] =0x28005800;
   arrLutDpeSclBilin[23] =0x0;
   arrLutDpeSclBilin[24] =0x20006000;
   arrLutDpeSclBilin[25] =0x0;
   arrLutDpeSclBilin[26] =0x18006800;
   arrLutDpeSclBilin[27] =0x0;
   arrLutDpeSclBilin[28] =0x10007000;
   arrLutDpeSclBilin[29] =0x0;
   arrLutDpeSclBilin[30] =0x08007800;
   arrLutDpeSclBilin[31] =0x0;
   arrLutDpeSclBilin[32] =0x0;
   arrLutDpeSclBilin[33] =0x0;
   arrLutDpeSclBilin[34] =0x0;
   arrLutDpeSclBilin[35] =0x0;
   arrLutDpeSclBilin[36] =0x0;
   arrLutDpeSclBilin[37] =0x0;
   arrLutDpeSclBilin[38] =0x0;
   arrLutDpeSclBilin[39] =0x0;
   arrLutDpeSclBilin[40] =0x0;
   arrLutDpeSclBilin[41] =0x0;
   arrLutDpeSclBilin[42] =0x0;
   arrLutDpeSclBilin[43] =0x0;
   arrLutDpeSclBilin[44] =0x0;
   arrLutDpeSclBilin[45] =0x0;
   arrLutDpeSclBilin[46] =0x0;
   arrLutDpeSclBilin[47] =0x0;
   arrLutDpeSclBilin[48] =0x0;
   arrLutDpeSclBilin[49] =0x0;
   arrLutDpeSclBilin[50] =0x0;
   arrLutDpeSclBilin[51] =0x0;
   arrLutDpeSclBilin[52] =0x0;
   arrLutDpeSclBilin[53] =0x0;
   arrLutDpeSclBilin[54] =0x0;
   arrLutDpeSclBilin[55] =0x0;
   arrLutDpeSclBilin[56] =0x0;
   arrLutDpeSclBilin[57] =0x0;
   arrLutDpeSclBilin[58] =0x0;
   arrLutDpeSclBilin[59] =0x0;
   arrLutDpeSclBilin[60] =0x0;
   arrLutDpeSclBilin[61] =0x0;
   arrLutDpeSclBilin[62] =0x0;
   arrLutDpeSclBilin[63] =0x0;

   // DPE - Vertical - L1
   arrLutDpeSclBilin[64] =0x7FFF0000;
   arrLutDpeSclBilin[65] =0x0;
   arrLutDpeSclBilin[66] =0x78000800;
   arrLutDpeSclBilin[67] =0x0;
   arrLutDpeSclBilin[68] =0x70001000;
   arrLutDpeSclBilin[69] =0x0;
   arrLutDpeSclBilin[70] =0x68001800;
   arrLutDpeSclBilin[71] =0x0;
   arrLutDpeSclBilin[72] =0x60002000;
   arrLutDpeSclBilin[73] =0x0;
   arrLutDpeSclBilin[74] =0x58002800;
   arrLutDpeSclBilin[75] =0x0;
   arrLutDpeSclBilin[76] =0x50003000;
   arrLutDpeSclBilin[77] =0x0;
   arrLutDpeSclBilin[78] =0x48003800;
   arrLutDpeSclBilin[79] =0x0;
   arrLutDpeSclBilin[80] =0x40004000;
   arrLutDpeSclBilin[81] =0x0;
   arrLutDpeSclBilin[82] =0x38004800;
   arrLutDpeSclBilin[83] =0x0;
   arrLutDpeSclBilin[84] =0x30005000;
   arrLutDpeSclBilin[85] =0x0;
   arrLutDpeSclBilin[86] =0x28005800;
   arrLutDpeSclBilin[87] =0x0;
   arrLutDpeSclBilin[88] =0x20006000;
   arrLutDpeSclBilin[89] =0x0;
   arrLutDpeSclBilin[90] =0x18006800;
   arrLutDpeSclBilin[91] =0x0;
   arrLutDpeSclBilin[92] =0x10007000;
   arrLutDpeSclBilin[93] =0x0;
   arrLutDpeSclBilin[94] =0x08007800;
   arrLutDpeSclBilin[95] =0x0;
   arrLutDpeSclBilin[96] =0x0;
   arrLutDpeSclBilin[97] =0x0;
   arrLutDpeSclBilin[98] =0x0;
   arrLutDpeSclBilin[99] =0x0;
   arrLutDpeSclBilin[100]=0x0;
   arrLutDpeSclBilin[101]=0x0;
   arrLutDpeSclBilin[102]=0x0;
   arrLutDpeSclBilin[103]=0x0;
   arrLutDpeSclBilin[104]=0x0;
   arrLutDpeSclBilin[105]=0x0;
   arrLutDpeSclBilin[106]=0x0;
   arrLutDpeSclBilin[107]=0x0;
   arrLutDpeSclBilin[108]=0x0;
   arrLutDpeSclBilin[109]=0x0;
   arrLutDpeSclBilin[110]=0x0;
   arrLutDpeSclBilin[111]=0x0;
   arrLutDpeSclBilin[112]=0x0;
   arrLutDpeSclBilin[113]=0x0;
   arrLutDpeSclBilin[114]=0x0;
   arrLutDpeSclBilin[115]=0x0;
   arrLutDpeSclBilin[116]=0x0;
   arrLutDpeSclBilin[117]=0x0;
   arrLutDpeSclBilin[118]=0x0;
   arrLutDpeSclBilin[119]=0x0;
   arrLutDpeSclBilin[120]=0x0;
   arrLutDpeSclBilin[121]=0x0;
   arrLutDpeSclBilin[122]=0x0;
   arrLutDpeSclBilin[123]=0x0;
   arrLutDpeSclBilin[124]=0x0;
   arrLutDpeSclBilin[125]=0x0;
   arrLutDpeSclBilin[126]=0x0;
   arrLutDpeSclBilin[127]=0x0;

   // DPE - Vertical - R0
   arrLutDpeSclBilin[128]=0x7FFF0000;
   arrLutDpeSclBilin[129]=0x0;
   arrLutDpeSclBilin[130]=0x78000800;
   arrLutDpeSclBilin[131]=0x0;
   arrLutDpeSclBilin[132]=0x70001000;
   arrLutDpeSclBilin[133]=0x0;
   arrLutDpeSclBilin[134]=0x68001800;
   arrLutDpeSclBilin[135]=0x0;
   arrLutDpeSclBilin[136]=0x60002000;
   arrLutDpeSclBilin[137]=0x0;
   arrLutDpeSclBilin[138]=0x58002800;
   arrLutDpeSclBilin[139]=0x0;
   arrLutDpeSclBilin[140]=0x50003000;
   arrLutDpeSclBilin[141]=0x0;
   arrLutDpeSclBilin[142]=0x48003800;
   arrLutDpeSclBilin[143]=0x0;
   arrLutDpeSclBilin[144]=0x40004000;
   arrLutDpeSclBilin[145]=0x0;
   arrLutDpeSclBilin[146]=0x38004800;
   arrLutDpeSclBilin[147]=0x0;
   arrLutDpeSclBilin[148]=0x30005000;
   arrLutDpeSclBilin[149]=0x0;
   arrLutDpeSclBilin[150]=0x28005800;
   arrLutDpeSclBilin[151]=0x0;
   arrLutDpeSclBilin[152]=0x20006000;
   arrLutDpeSclBilin[153]=0x0;
   arrLutDpeSclBilin[154]=0x18006800;
   arrLutDpeSclBilin[155]=0x0;
   arrLutDpeSclBilin[156]=0x10007000;
   arrLutDpeSclBilin[157]=0x0;
   arrLutDpeSclBilin[158]=0x08007800;
   arrLutDpeSclBilin[159]=0x0;
   arrLutDpeSclBilin[160]=0x0;
   arrLutDpeSclBilin[161]=0x0;
   arrLutDpeSclBilin[162]=0x0;
   arrLutDpeSclBilin[163]=0x0;
   arrLutDpeSclBilin[164]=0x0;
   arrLutDpeSclBilin[165]=0x0;
   arrLutDpeSclBilin[166]=0x0;
   arrLutDpeSclBilin[167]=0x0;
   arrLutDpeSclBilin[168]=0x0;
   arrLutDpeSclBilin[169]=0x0;
   arrLutDpeSclBilin[170]=0x0;
   arrLutDpeSclBilin[171]=0x0;
   arrLutDpeSclBilin[172]=0x0;
   arrLutDpeSclBilin[173]=0x0;
   arrLutDpeSclBilin[174]=0x0;
   arrLutDpeSclBilin[175]=0x0;
   arrLutDpeSclBilin[176]=0x0;
   arrLutDpeSclBilin[177]=0x0;
   arrLutDpeSclBilin[178]=0x0;
   arrLutDpeSclBilin[179]=0x0;
   arrLutDpeSclBilin[180]=0x0;
   arrLutDpeSclBilin[181]=0x0;
   arrLutDpeSclBilin[182]=0x0;
   arrLutDpeSclBilin[183]=0x0;
   arrLutDpeSclBilin[184]=0x0;
   arrLutDpeSclBilin[185]=0x0;
   arrLutDpeSclBilin[186]=0x0;
   arrLutDpeSclBilin[187]=0x0;
   arrLutDpeSclBilin[188]=0x0;
   arrLutDpeSclBilin[189]=0x0;
   arrLutDpeSclBilin[190]=0x0;
   arrLutDpeSclBilin[191]=0x0;

   // DPE - Vertical - R1
   arrLutDpeSclBilin[192]=0x7FFF0000;
   arrLutDpeSclBilin[193]=0x0;
   arrLutDpeSclBilin[194]=0x78000800;
   arrLutDpeSclBilin[195]=0x0;
   arrLutDpeSclBilin[196]=0x70001000;
   arrLutDpeSclBilin[197]=0x0;
   arrLutDpeSclBilin[198]=0x68001800;
   arrLutDpeSclBilin[199]=0x0;
   arrLutDpeSclBilin[200]=0x60002000;
   arrLutDpeSclBilin[201]=0x0;
   arrLutDpeSclBilin[202]=0x58002800;
   arrLutDpeSclBilin[203]=0x0;
   arrLutDpeSclBilin[204]=0x50003000;
   arrLutDpeSclBilin[205]=0x0;
   arrLutDpeSclBilin[206]=0x48003800;
   arrLutDpeSclBilin[207]=0x0;
   arrLutDpeSclBilin[208]=0x40004000;
   arrLutDpeSclBilin[209]=0x0;
   arrLutDpeSclBilin[210]=0x38004800;
   arrLutDpeSclBilin[211]=0x0;
   arrLutDpeSclBilin[212]=0x30005000;
   arrLutDpeSclBilin[213]=0x0;
   arrLutDpeSclBilin[214]=0x28005800;
   arrLutDpeSclBilin[215]=0x0;
   arrLutDpeSclBilin[216]=0x20006000;
   arrLutDpeSclBilin[217]=0x0;
   arrLutDpeSclBilin[218]=0x18006800;
   arrLutDpeSclBilin[219]=0x0;
   arrLutDpeSclBilin[220]=0x10007000;
   arrLutDpeSclBilin[221]=0x0;
   arrLutDpeSclBilin[222]=0x08007800;
   arrLutDpeSclBilin[223]=0x0;
   arrLutDpeSclBilin[224]=0x0;
   arrLutDpeSclBilin[225]=0x0;
   arrLutDpeSclBilin[226]=0x0;
   arrLutDpeSclBilin[227]=0x0;
   arrLutDpeSclBilin[228]=0x0;
   arrLutDpeSclBilin[229]=0x0;
   arrLutDpeSclBilin[230]=0x0;
   arrLutDpeSclBilin[231]=0x0;
   arrLutDpeSclBilin[232]=0x0;
   arrLutDpeSclBilin[233]=0x0;
   arrLutDpeSclBilin[234]=0x0;
   arrLutDpeSclBilin[235]=0x0;
   arrLutDpeSclBilin[236]=0x0;
   arrLutDpeSclBilin[237]=0x0;
   arrLutDpeSclBilin[238]=0x0;
   arrLutDpeSclBilin[239]=0x0;
   arrLutDpeSclBilin[240]=0x0;
   arrLutDpeSclBilin[241]=0x0;
   arrLutDpeSclBilin[242]=0x0;
   arrLutDpeSclBilin[243]=0x0;
   arrLutDpeSclBilin[244]=0x0;
   arrLutDpeSclBilin[245]=0x0;
   arrLutDpeSclBilin[246]=0x0;
   arrLutDpeSclBilin[247]=0x0;
   arrLutDpeSclBilin[248]=0x0;
   arrLutDpeSclBilin[249]=0x0;
   arrLutDpeSclBilin[250]=0x0;
   arrLutDpeSclBilin[251]=0x0;
   arrLutDpeSclBilin[252]=0x0;
   arrLutDpeSclBilin[253]=0x0;
   arrLutDpeSclBilin[254]=0x0;
   arrLutDpeSclBilin[255]=0x0;

   // DPE - Horizontal - L0
   arrLutDpeSclBilin[256] = 0x10C708FB;
   arrLutDpeSclBilin[257] = 0x1BA91869;
   arrLutDpeSclBilin[258] = 0x10C71869;
   arrLutDpeSclBilin[259] = 0x000008FB;
   arrLutDpeSclBilin[260] = 0x114E0969;
   arrLutDpeSclBilin[261] = 0x1BA718CA;
   arrLutDpeSclBilin[262] = 0x10421806;
   arrLutDpeSclBilin[263] = 0x00000891;
   arrLutDpeSclBilin[264] = 0x11D609DA;
   arrLutDpeSclBilin[265] = 0x1BA01927;
   arrLutDpeSclBilin[266] = 0x0FBE17A1;
   arrLutDpeSclBilin[267] = 0x0000082A;
   arrLutDpeSclBilin[268] = 0x125F0A4E;
   arrLutDpeSclBilin[269] = 0x1B941982;
   arrLutDpeSclBilin[270] = 0x0F3B1739;
   arrLutDpeSclBilin[271] = 0x000007C7;
   arrLutDpeSclBilin[272] = 0x12E90AC7;
   arrLutDpeSclBilin[273] = 0x1B8419D9;
   arrLutDpeSclBilin[274] = 0x0EBB16D0;
   arrLutDpeSclBilin[275] = 0x00000768;
   arrLutDpeSclBilin[276] = 0x13740B42;
   arrLutDpeSclBilin[277] = 0x1B6F1A2E;
   arrLutDpeSclBilin[278] = 0x0E3C1664;
   arrLutDpeSclBilin[279] = 0x0000070C;
   arrLutDpeSclBilin[280] = 0x14000BC2;
   arrLutDpeSclBilin[281] = 0x1B561A7F;
   arrLutDpeSclBilin[282] = 0x0DBF15F7;
   arrLutDpeSclBilin[283] = 0x000006B3;
   arrLutDpeSclBilin[284] = 0x148C0C45;
   arrLutDpeSclBilin[285] = 0x1B381ACC;
   arrLutDpeSclBilin[286] = 0x0D441588;
   arrLutDpeSclBilin[287] = 0x0000065D;
   arrLutDpeSclBilin[288] = 0x15180CCC;
   arrLutDpeSclBilin[289] = 0x1B161B16;
   arrLutDpeSclBilin[290] = 0x0CCC1518;
   arrLutDpeSclBilin[291] = 0x0000060B;
   arrLutDpeSclBilin[292] = 0x15A50D56;
   arrLutDpeSclBilin[293] = 0x1AF01B5D;
   arrLutDpeSclBilin[294] = 0x0C5514A7;
   arrLutDpeSclBilin[295] = 0x000005BC;
   arrLutDpeSclBilin[296] = 0x16320DE3;
   arrLutDpeSclBilin[297] = 0x1AC51B9F;
   arrLutDpeSclBilin[298] = 0x0BE11435;
   arrLutDpeSclBilin[299] = 0x00000570;
   arrLutDpeSclBilin[300] = 0x16BE0E75;
   arrLutDpeSclBilin[301] = 0x1A971BDD;
   arrLutDpeSclBilin[302] = 0x0B7013C2;
   arrLutDpeSclBilin[303] = 0x00000528;
   arrLutDpeSclBilin[304] = 0x174A0F09;
   arrLutDpeSclBilin[305] = 0x1A641C18;
   arrLutDpeSclBilin[306] = 0x0B00134F;
   arrLutDpeSclBilin[307] = 0x000004E2;
   arrLutDpeSclBilin[308] = 0x17D60FA2;
   arrLutDpeSclBilin[309] = 0x1A2D1C4E;
   arrLutDpeSclBilin[310] = 0x0A9412DB;
   arrLutDpeSclBilin[311] = 0x0000049F;
   arrLutDpeSclBilin[312] = 0x1861103D;
   arrLutDpeSclBilin[313] = 0x19F31C80;
   arrLutDpeSclBilin[314] = 0x0A291267;
   arrLutDpeSclBilin[315] = 0x0000045F;
   arrLutDpeSclBilin[316] = 0x18EB10DC;
   arrLutDpeSclBilin[317] = 0x19B51CAE;
   arrLutDpeSclBilin[318] = 0x09C211F2;
   arrLutDpeSclBilin[319] = 0x00000422;

   // DPE - Horizontal - L1
   arrLutDpeSclBilin[320] = 0x10C708FB;
   arrLutDpeSclBilin[321] = 0x1BA91869;
   arrLutDpeSclBilin[322] = 0x10C71869;
   arrLutDpeSclBilin[323] = 0x000008FB;
   arrLutDpeSclBilin[324] = 0x114E0969;
   arrLutDpeSclBilin[325] = 0x1BA718CA;
   arrLutDpeSclBilin[326] = 0x10421806;
   arrLutDpeSclBilin[327] = 0x00000891;
   arrLutDpeSclBilin[328] = 0x11D609DA;
   arrLutDpeSclBilin[329] = 0x1BA01927;
   arrLutDpeSclBilin[330] = 0x0FBE17A1;
   arrLutDpeSclBilin[331] = 0x0000082A;
   arrLutDpeSclBilin[332] = 0x125F0A4E;
   arrLutDpeSclBilin[333] = 0x1B941982;
   arrLutDpeSclBilin[334] = 0x0F3B1739;
   arrLutDpeSclBilin[335] = 0x000007C7;
   arrLutDpeSclBilin[336] = 0x12E90AC7;
   arrLutDpeSclBilin[337] = 0x1B8419D9;
   arrLutDpeSclBilin[338] = 0x0EBB16D0;
   arrLutDpeSclBilin[339] = 0x00000768;
   arrLutDpeSclBilin[340] = 0x13740B42;
   arrLutDpeSclBilin[341] = 0x1B6F1A2E;
   arrLutDpeSclBilin[342] = 0x0E3C1664;
   arrLutDpeSclBilin[343] = 0x0000070C;
   arrLutDpeSclBilin[344] = 0x14000BC2;
   arrLutDpeSclBilin[345] = 0x1B561A7F;
   arrLutDpeSclBilin[346] = 0x0DBF15F7;
   arrLutDpeSclBilin[347] = 0x000006B3;
   arrLutDpeSclBilin[348] = 0x148C0C45;
   arrLutDpeSclBilin[349] = 0x1B381ACC;
   arrLutDpeSclBilin[350] = 0x0D441588;
   arrLutDpeSclBilin[351] = 0x0000065D;
   arrLutDpeSclBilin[352] = 0x15180CCC;
   arrLutDpeSclBilin[353] = 0x1B161B16;
   arrLutDpeSclBilin[354] = 0x0CCC1518;
   arrLutDpeSclBilin[355] = 0x0000060B;
   arrLutDpeSclBilin[356] = 0x15A50D56;
   arrLutDpeSclBilin[357] = 0x1AF01B5D;
   arrLutDpeSclBilin[358] = 0x0C5514A7;
   arrLutDpeSclBilin[359] = 0x000005BC;
   arrLutDpeSclBilin[360] = 0x16320DE3;
   arrLutDpeSclBilin[361] = 0x1AC51B9F;
   arrLutDpeSclBilin[362] = 0x0BE11435;
   arrLutDpeSclBilin[363] = 0x00000570;
   arrLutDpeSclBilin[364] = 0x16BE0E75;
   arrLutDpeSclBilin[365] = 0x1A971BDD;
   arrLutDpeSclBilin[366] = 0x0B7013C2;
   arrLutDpeSclBilin[367] = 0x00000528;
   arrLutDpeSclBilin[368] = 0x174A0F09;
   arrLutDpeSclBilin[369] = 0x1A641C18;
   arrLutDpeSclBilin[370] = 0x0B00134F;
   arrLutDpeSclBilin[371] = 0x000004E2;
   arrLutDpeSclBilin[372] = 0x17D60FA2;
   arrLutDpeSclBilin[373] = 0x1A2D1C4E;
   arrLutDpeSclBilin[374] = 0x0A9412DB;
   arrLutDpeSclBilin[375] = 0x0000049F;
   arrLutDpeSclBilin[376] = 0x1861103D;
   arrLutDpeSclBilin[377] = 0x19F31C80;
   arrLutDpeSclBilin[378] = 0x0A291267;
   arrLutDpeSclBilin[379] = 0x0000045F;
   arrLutDpeSclBilin[380] = 0x18EB10DC;
   arrLutDpeSclBilin[381] = 0x19B51CAE;
   arrLutDpeSclBilin[382] = 0x09C211F2;
   arrLutDpeSclBilin[383] = 0x00000422;

   // DPE - Horizontal - R0
   arrLutDpeSclBilin[384] = 0x10C708FB;
   arrLutDpeSclBilin[385] = 0x1BA91869;
   arrLutDpeSclBilin[386] = 0x10C71869;
   arrLutDpeSclBilin[387] = 0x000008FB;
   arrLutDpeSclBilin[388] = 0x114E0969;
   arrLutDpeSclBilin[389] = 0x1BA718CA;
   arrLutDpeSclBilin[390] = 0x10421806;
   arrLutDpeSclBilin[391] = 0x00000891;
   arrLutDpeSclBilin[392] = 0x11D609DA;
   arrLutDpeSclBilin[393] = 0x1BA01927;
   arrLutDpeSclBilin[394] = 0x0FBE17A1;
   arrLutDpeSclBilin[395] = 0x0000082A;
   arrLutDpeSclBilin[396] = 0x125F0A4E;
   arrLutDpeSclBilin[397] = 0x1B941982;
   arrLutDpeSclBilin[398] = 0x0F3B1739;
   arrLutDpeSclBilin[399] = 0x000007C7;
   arrLutDpeSclBilin[400] = 0x12E90AC7;
   arrLutDpeSclBilin[401] = 0x1B8419D9;
   arrLutDpeSclBilin[402] = 0x0EBB16D0;
   arrLutDpeSclBilin[403] = 0x00000768;
   arrLutDpeSclBilin[404] = 0x13740B42;
   arrLutDpeSclBilin[405] = 0x1B6F1A2E;
   arrLutDpeSclBilin[406] = 0x0E3C1664;
   arrLutDpeSclBilin[407] = 0x0000070C;
   arrLutDpeSclBilin[408] = 0x14000BC2;
   arrLutDpeSclBilin[409] = 0x1B561A7F;
   arrLutDpeSclBilin[410] = 0x0DBF15F7;
   arrLutDpeSclBilin[411] = 0x000006B3;
   arrLutDpeSclBilin[412] = 0x148C0C45;
   arrLutDpeSclBilin[413] = 0x1B381ACC;
   arrLutDpeSclBilin[414] = 0x0D441588;
   arrLutDpeSclBilin[415] = 0x0000065D;
   arrLutDpeSclBilin[416] = 0x15180CCC;
   arrLutDpeSclBilin[417] = 0x1B161B16;
   arrLutDpeSclBilin[418] = 0x0CCC1518;
   arrLutDpeSclBilin[419] = 0x0000060B;
   arrLutDpeSclBilin[420] = 0x15A50D56;
   arrLutDpeSclBilin[421] = 0x1AF01B5D;
   arrLutDpeSclBilin[422] = 0x0C5514A7;
   arrLutDpeSclBilin[423] = 0x000005BC;
   arrLutDpeSclBilin[424] = 0x16320DE3;
   arrLutDpeSclBilin[425] = 0x1AC51B9F;
   arrLutDpeSclBilin[426] = 0x0BE11435;
   arrLutDpeSclBilin[427] = 0x00000570;
   arrLutDpeSclBilin[428] = 0x16BE0E75;
   arrLutDpeSclBilin[429] = 0x1A971BDD;
   arrLutDpeSclBilin[430] = 0x0B7013C2;
   arrLutDpeSclBilin[431] = 0x00000528;
   arrLutDpeSclBilin[432] = 0x174A0F09;
   arrLutDpeSclBilin[433] = 0x1A641C18;
   arrLutDpeSclBilin[434] = 0x0B00134F;
   arrLutDpeSclBilin[435] = 0x000004E2;
   arrLutDpeSclBilin[436] = 0x17D60FA2;
   arrLutDpeSclBilin[437] = 0x1A2D1C4E;
   arrLutDpeSclBilin[438] = 0x0A9412DB;
   arrLutDpeSclBilin[439] = 0x0000049F;
   arrLutDpeSclBilin[440] = 0x1861103D;
   arrLutDpeSclBilin[441] = 0x19F31C80;
   arrLutDpeSclBilin[442] = 0x0A291267;
   arrLutDpeSclBilin[443] = 0x0000045F;
   arrLutDpeSclBilin[444] = 0x18EB10DC;
   arrLutDpeSclBilin[445] = 0x19B51CAE;
   arrLutDpeSclBilin[446] = 0x09C211F2;
   arrLutDpeSclBilin[447] = 0x00000422;

   // DPE - Horizontal - R1
   arrLutDpeSclBilin[448] = 0x10C708FB;
   arrLutDpeSclBilin[449] = 0x1BA91869;
   arrLutDpeSclBilin[450] = 0x10C71869;
   arrLutDpeSclBilin[451] = 0x000008FB;
   arrLutDpeSclBilin[452] = 0x114E0969;
   arrLutDpeSclBilin[453] = 0x1BA718CA;
   arrLutDpeSclBilin[454] = 0x10421806;
   arrLutDpeSclBilin[455] = 0x00000891;
   arrLutDpeSclBilin[456] = 0x11D609DA;
   arrLutDpeSclBilin[457] = 0x1BA01927;
   arrLutDpeSclBilin[458] = 0x0FBE17A1;
   arrLutDpeSclBilin[459] = 0x0000082A;
   arrLutDpeSclBilin[460] = 0x125F0A4E;
   arrLutDpeSclBilin[461] = 0x1B941982;
   arrLutDpeSclBilin[462] = 0x0F3B1739;
   arrLutDpeSclBilin[463] = 0x000007C7;
   arrLutDpeSclBilin[464] = 0x12E90AC7;
   arrLutDpeSclBilin[465] = 0x1B8419D9;
   arrLutDpeSclBilin[466] = 0x0EBB16D0;
   arrLutDpeSclBilin[467] = 0x00000768;
   arrLutDpeSclBilin[468] = 0x13740B42;
   arrLutDpeSclBilin[469] = 0x1B6F1A2E;
   arrLutDpeSclBilin[470] = 0x0E3C1664;
   arrLutDpeSclBilin[471] = 0x0000070C;
   arrLutDpeSclBilin[472] = 0x14000BC2;
   arrLutDpeSclBilin[473] = 0x1B561A7F;
   arrLutDpeSclBilin[474] = 0x0DBF15F7;
   arrLutDpeSclBilin[475] = 0x000006B3;
   arrLutDpeSclBilin[476] = 0x148C0C45;
   arrLutDpeSclBilin[477] = 0x1B381ACC;
   arrLutDpeSclBilin[478] = 0x0D441588;
   arrLutDpeSclBilin[479] = 0x0000065D;
   arrLutDpeSclBilin[480] = 0x15180CCC;
   arrLutDpeSclBilin[481] = 0x1B161B16;
   arrLutDpeSclBilin[482] = 0x0CCC1518;
   arrLutDpeSclBilin[483] = 0x0000060B;
   arrLutDpeSclBilin[484] = 0x15A50D56;
   arrLutDpeSclBilin[485] = 0x1AF01B5D;
   arrLutDpeSclBilin[486] = 0x0C5514A7;
   arrLutDpeSclBilin[487] = 0x000005BC;
   arrLutDpeSclBilin[488] = 0x16320DE3;
   arrLutDpeSclBilin[489] = 0x1AC51B9F;
   arrLutDpeSclBilin[490] = 0x0BE11435;
   arrLutDpeSclBilin[491] = 0x00000570;
   arrLutDpeSclBilin[492] = 0x16BE0E75;
   arrLutDpeSclBilin[493] = 0x1A971BDD;
   arrLutDpeSclBilin[494] = 0x0B7013C2;
   arrLutDpeSclBilin[495] = 0x00000528;
   arrLutDpeSclBilin[496] = 0x174A0F09;
   arrLutDpeSclBilin[497] = 0x1A641C18;
   arrLutDpeSclBilin[498] = 0x0B00134F;
   arrLutDpeSclBilin[499] = 0x000004E2;
   arrLutDpeSclBilin[500] = 0x17D60FA2;
   arrLutDpeSclBilin[501] = 0x1A2D1C4E;
   arrLutDpeSclBilin[502] = 0x0A9412DB;
   arrLutDpeSclBilin[503] = 0x0000049F;
   arrLutDpeSclBilin[504] = 0x1861103D;
   arrLutDpeSclBilin[505] = 0x19F31C80;
   arrLutDpeSclBilin[506] = 0x0A291267;
   arrLutDpeSclBilin[507] = 0x0000045F;
   arrLutDpeSclBilin[508] = 0x18EB10DC;
   arrLutDpeSclBilin[509] = 0x19B51CAE;
   arrLutDpeSclBilin[510] = 0x09C211F2;
   arrLutDpeSclBilin[511] = 0x00000422;

#if 1
   NUFLD_blkE blk;
   UINT32 blkInstance;

   for (i = NU4100_IAE_MIPI_MUX_SLU0_LANES23_SEL_E; i < NU4100_IAE_IIM_CONTROL_IAU0_SEL_E; i++)  // till the last non-meta value
   {
      XMLDB_writeEntryToReg(&db->valueTbl[i]);
   }

   for (i = NU4100_IAE_BYPASS_YUV0_E; i < NU4100_IAE_SLU0_CROP_OFFSET_HORZ_E; i++)  // till the last non-meta value
   {
      XMLDB_writeEntryToReg(&db->valueTbl[i]);
   }

   for (i = NU4100_IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG_CNT_EN_E; i < NU4100_IAE_SPARE_RW0_SPARE_E; i++)
   {
      XMLDB_writeEntryToReg(&db->valueTbl[i]);
   }
   //SPARE register, only in main graph we will write this register.
   // to prevent override it by inj graph which does not use the slu. as following our policy if the path without IAU appearance the SLU will be at ratio of 1:1.
   // in inj graph in case there is no IAUs we don't want to override the SLU ratio by register spare, and to may cause damage to main graph
   if(isMainGraph)
   {
      //printf("MAIN GRAPH, write SPARE  (for slu ratio of 1:1) reg val 0x%x\n",db->valueTbl[NU4100_IAE_SPARE_RW0_SPARE_E]);
      XMLDB_writeEntryToReg(&db->valueTbl[NU4100_IAE_SPARE_RW0_SPARE_E]);
   }

   for (i = NU4100_PPE_PPE_MISC_MIPI_VI0_CONTROL_STRM_SEL_E; i < NU4100_PPE_PPU0_MISC_CROP_START_X_E; i++)
   {
      //mipi interleavers must not be written before VSC configuration.
      //the configuration will be done during the start channel in the sequencer
      if ((i != NU4100_PPE_PPE_MISC_MIPI_VI0_CONTROL_INT_MODE_E) &&
         (i != NU4100_PPE_PPE_MISC_MIPI_VI1_CONTROL_INT_MODE_E))
      {
         XMLDB_writeEntryToReg(&db->valueTbl[i]);
      }
   }

   for (i = NU4100_CVA_AXI_IF0_CTRL_DATA_WIDTH_E; i < META_READERS_RD_0_ENABLE_E; i++)
   {
      XMLDB_writeEntryToReg(&db->valueTbl[i]);
   }

   //temp fix - there is a problem with the SLU's and NUFLD - not all SLU's 0-3 has the same size!
   for (i = NU4100_IAE_SLU0_CROP_OFFSET_HORZ_E; i < (NU4100_IAE_FGEN0_CONTROL_MODE_E - 1); i++)  // till the last non-meta value
   {
      XMLDB_valueT *valp = &db->valueTbl[i];
      if ((!(XMLDB_IS_EMPTY(valp->flags))) && (!(XMLDB_IS_VIRTUAL(valp->flags))))
      {
         ret = MEM_MAPG_convertPhysicalToVirtual(valp->regFieldParams.blockAdd, &virtAddress);
         if (ERRG_SUCCEEDED(ret))
         {
            ret = validateValByWidth(valp->val, valp->regFieldParams.width);
            if (ERRG_FAILED(ret))
            {
               LOGG_PRINT(LOG_ERROR_E, NULL, "Mismatch between value and width entered in XML. USER_ID %d, width %d, val %d\n", i, valp->regFieldParams.width, valp->val);
               //assert(0);
            }

            virtAddress += valp->regFieldParams.regOffset;
            tmp = *((volatile UINT32 *)virtAddress);
            tmp &= ~(((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width)) << valp->regFieldParams.startBitOffset);
            tmp |= ((valp->val & ((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width))) << valp->regFieldParams.startBitOffset);
            //printf("%d: Reg 0x%x = 0x%x  pys block 0x%x reg offset 0x%x userid %d\n",i, ((volatile UINT32 *)virtAddress), tmp, valp->regFieldParams.blockAdd, valp->regFieldParams.regOffset,i);
            *((volatile UINT32 *)virtAddress) = tmp;
         }
      }
   }

   for (i = NU4100_IPE_IPE_PREPROC_CTRL0_ISP0_CH0_EXP0_SEL_E; i < META_READERS_RD_0_ENABLE_E; i++)  // till the last non-meta value
   {
      XMLDB_writeEntryToReg(&db->valueTbl[i]);
   }
#else
   for (i = NU4100_IAE_ENABLE_SLU0_EN_E; i < (META_READERS_RD_0_ENABLE_E - 1); i++)  // till the last non-meta value
   {
      XMLDB_valueT *valp = &db->valueTbl[i];
      if ((!(XMLDB_IS_EMPTY(valp->flags))) && (!(XMLDB_IS_VIRTUAL(valp->flags))))
      {
         ret = MEM_MAPG_convertPhysicalToVirtual(valp->regFieldParams.blockAdd, &virtAddress);
         if (ERRG_SUCCEEDED(ret))
         {
            ret = validateValByWidth(valp->val, valp->regFieldParams.width);
            if (ERRG_FAILED(ret))
            {
               LOGG_PRINT(LOG_ERROR_E, NULL, "Mismatch between value and width entered in XML. USER_ID %d, width %d, val %d\n", i, valp->regFieldParams.width, valp->val);
               //assert(0);
            }

            virtAddress += valp->regFieldParams.regOffset;
            tmp = *((volatile UINT32 *)virtAddress);
            tmp &= ~(((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width)) << valp->regFieldParams.startBitOffset);
            tmp |= ((valp->val & ((UINT32)0xFFFFFFFF >> (32 - valp->regFieldParams.width))) << valp->regFieldParams.startBitOffset);
            //printf("%d: Reg 0x%x = 0x%x  pys block 0x%x reg offset 0x%x userid %d\n",i, ((volatile UINT32 *)virtAddress), tmp, valp->regFieldParams.blockAdd, valp->regFieldParams.regOffset,i);
            *((volatile UINT32 *)virtAddress) = tmp;
         }
      }
   }
#endif
///////
//write ppe scl lut
// A0,B0 & C0 HW bug - scalers for PPU7 memory is unreachable (memory overlaps with AXI reader)
//                               write only PPU4&5 scalers
//////
   for ( z = 0; z < 2; z++)
   {
      ret = MEM_MAPG_getVirtAddr(MEM_MAPG_AXI_SCL_0_LUT_E+z,(MEM_MAPG_addrT*)&virtAddress);
      j = 0;
      for (i=0;i<=0x5fc;i+=4)
      {
          *((volatile UINT32 *)(virtAddress+i)) = arrLutPpeSclBilin[j++];
      }
   }
////////
//write dpe lut
////////
    ret = MEM_MAPG_getVirtAddr(MEM_MAPG_DPE_LUT_E,(MEM_MAPG_addrT*)&virtAddress);

#define WRITE_VERT_DPE_LUT
#define WRITE_HORZ_DPE_LUT
#define DPE_LUT_BASE_ADDR 0x4800000

//write vertical luts
#ifdef WRITE_VERT_DPE_LUT
// The vertical data should be written 4 times to offsets:
// Left0:   0x07F-0x000
// Left1:   0x17F-0x100
// Right0:  0x27F-0x200
// Right1:  0x37F-0x300
   j=0;
   //printf("Write DPE vertical LUTs:\n");
   for (k=0; k<0x400;k+=0x100)
   {
      for (i=k;i<=(k+0x7c);i+=4)
      {
//         printf("0x%x = 0x%x\n",DPE_LUT_BASE_ADDR+i,arrLutDpeSclBilin[j]);
         *((volatile UINT32 *)(virtAddress+i)) = arrLutDpeSclBilin[j++];
      }
      j+=32;
   }
#endif

//write horizontal luts
#ifdef WRITE_HORZ_DPE_LUT
   j=256;
//   printf("Write DPE horz LUTs:\n");
   for (i=0x400;i<=(0x7fc);i+=4)
   {
      //printf("0x%x = 0x%x\n",DPE_LUT_BASE_ADDR+i,arrLutDpeSclBilin[j]);
      *((volatile UINT32 *)(virtAddress+i)) = arrLutDpeSclBilin[j++];
    }
#endif
    return ret;
}
#endif

//////////////////////////////////////////////////////
//unit test
#if 0
static  int loadNuSocxml(char **xmlbuf, unsigned int *xmlsize)
{
   int ret = 0;
   char *buf = NULL;
   unsigned int size;
   //open and load xml file
   FILE *xmlfile = fopen("nu4k.xml", "r");
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

//unit test
static void setRandom(XMLDB_dbH db, INT32 *randPath, UINT32 *randValues, unsigned int numValues)
{
   unsigned int i;

   randPath[0] = rand() % XMLDB_NUM_PATHS_E;
   randValues[0] = 0;
   XMLDB_setValue(db, (XMLDB_pathE)randPath[0], randValues[0]);

   for (i = 1; i < numValues; i++)
   {
      randPath[i] = rand() % XMLDB_NUM_PATHS_E;
      randValues[i] = (rand() * 33);
      XMLDB_setValue(db, (XMLDB_pathE)randPath[i], randValues[i]);
   }
}

static int setGetClearTest(XMLDB_dbH db)
{
   int ret = 0;
   enum { NUM_VALUES = 10 };
   UINT32 randValues[NUM_VALUES];
   INT32 randPaths[NUM_VALUES];
   INT32 i;

   setRandom(db, randPaths, randValues, NUM_VALUES);

   for (i = 0; i < NUM_VALUES; i++)
   {
      UINT32 getVal = 0;
      ERRG_codeE retc = XMLDB_getValue(db, (XMLDB_pathE)randPaths[i], &getVal);
      if (ERRG_FAILED(retc) || (getVal != randValues[i]))
      {
         ret = -1;
         break;
      }
      retc = XMLDB_clearValue(db, (XMLDB_pathE)randPaths[i]);
      if (ERRG_FAILED(retc))
      {
         ret = -1;
         break;
      }
      retc = XMLDB_getValue(db, (XMLDB_pathE)randPaths[i], NULL);
      if (retc != XMLDB__EMPTY_ENTRY)
      {
         ret = -1;
         break;
      }
   }
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
   // nothing is written on second pass because db is
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
static ERRG_codeE loadAndCompare(XMLDB_dbH db, const char *testbuf, INT32 *compIndices, UINT32 *compValues, unsigned int numValues)
{
   ERRG_codeE ret;
   unsigned int i;

   //load xml and compare to expected values
   ret = XMLDB_loadFromBuf(db, testbuf);
   if (ERRG_FAILED(ret))
      return ret;

   for (i = 0; i < numValues; i++)
   {
      UINT32 val;
      ERRG_codeE retc = XMLDB_getValue(db, (XMLDB_pathE)compIndices[i], &val);
      if (ERRG_FAILED(retc) || (val != compValues[i]))
         ret = XMLDB__ERR_UNEXPECTED;
   }
   return ret;
}


static ERRG_codeE randomSaveLoadTest(XMLDB_dbH db, const char *xmlbuf)
{
   ERRG_codeE ret;
   enum { NUM_VALUES = 10 };
   UINT32 randValues[NUM_VALUES];
   INT32 randIndices[NUM_VALUES];
   unsigned int outSize = 0;
   char *outBuf = NULL;

   //load xml
   ret = XMLDB_loadFromBuf(db, xmlbuf);
   if (ERRG_FAILED(ret))
      return ret;

   //set some random values
   setRandom(db, randIndices, randValues, NUM_VALUES);

   //save changes to new buffer
   ret = saveDb(db, xmlbuf, &outSize, &outBuf);
   if (ERRG_SUCCEEDED(ret))
   {
      //load back the buffer and save
      ret = loadAndCompare(db, outBuf, randIndices, randValues, NUM_VALUES);
   }

   if (outBuf)
      free(outBuf);

   return ret;
}

void xmldb_test(void)
{
   ERRG_codeE ret;
   int ret2;
   XMLDB_dbH db1 = NULL;
   unsigned int xmlsize;
   char *xmlbuf = NULL;
   //srand((unsigned int)&ret);//seed
   srand(0);//seed

   ret2 = loadNuSocxml(&xmlbuf, &xmlsize);
   if (ret2 < 0)
      goto test_err;


   ret = XMLDB_open(&db1);
   if (ERRG_FAILED(ret))
      goto test_err;

   ret2 = setGetClearTest(db1);
   if (ret2 < 0)
      goto test_err;

   ret = randomSaveLoadTest(db1, xmlbuf);
   if (ERRG_FAILED(ret))
      goto test_err;

   printf("xmldb_test passed\n");
   goto test_cleanup;

test_err:
   printf("xmldb_test failed\n");
test_cleanup:
   XMLDB_close(db1);
   if (xmlbuf)
      free(xmlbuf);
}
#endif
#ifdef __cplusplus
}
#endif
