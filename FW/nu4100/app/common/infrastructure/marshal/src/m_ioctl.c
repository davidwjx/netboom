/****************************************************************************
 * 
 *   FileName: marshal.c
 *
 *   Author: Ram B
 *
 *   Date: 10/2012
 *
 *   Description: Marshalling implementation 
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "marshal.h"
#include "m_priv.h"

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef struct
{
   UINT32 max_size;
   UINT32 max_m_size;
} svc_ioctl_info_t;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static svc_ioctl_info_t svc_ioctl_tbl[INUG_NUM_SERVICES_E];

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

static void set_ioctl_max_sizes()
{
   int i,j;
   for(i = 0; i < INUG_NUM_SERVICES_E; i++)
   {
      svc_ioctl_tbl[i].max_size = 0;
      svc_ioctl_tbl[i].max_m_size = 0;
      for(j = 0 ; j < MAX_IOCTL_PER_SERVICE;j++)
      {
         marshal_id_e mid = marshal_id_lut_entry(i,j);
         if(mid != M_INVALID_ENTRY)
         {
            if(svc_ioctl_tbl[i].max_size < marshal_tbl_entry(mid)->size_of)
               svc_ioctl_tbl[i].max_size = marshal_tbl_entry(mid)->size_of;

            if(svc_ioctl_tbl[i].max_m_size < marshal_tbl_entry(mid)->m_size)
               svc_ioctl_tbl[i].max_m_size = marshal_tbl_entry(mid)->m_size;
         }
      }
   }
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void MARSHALG_u16(INT16 dir, UINT16 *dataP, UINT8 *bufP)
{
   marshal_u16(dir,dataP,bufP);
}

void MARSHALG_u32(INT16 dir, UINT32 *dataP, UINT8 *bufP)
{
   marshal_u32(dir,dataP,bufP);
}

void MARSHALG_u64(INT16 dir, UINT64 *dataP, UINT8 *bufP)
{
   marshal_u64(dir,dataP,bufP);
}

void MARSHALG_init()
{
   init_marshal_tbl();
   set_ioctl_max_sizes();

   //MARSHALG_showIoctlTbl(0);
   //MARSHALG_showMaxSizes();
   //show_marshal_tbl_stats();
   LOGG_PRINT(LOG_INFO_E, NULL,"Marshal hash 0x%08x\n", get_marshal_tbl_hash());
}

UINT32 MARSHALG_getTblHash(void)
{
   return get_marshal_tbl_hash();
}

void MARSHALG_deInit()
{
   deinit_marshal_tbl();
}

void MARSHALG_convertIoctl(INT16 dir, INUG_ioctlCmdE cmd, UINT8* pStIoctl, UINT8* pBufOut, UINT32* pLen)
{
   UINT32 serviceId;
   marshal_id_e mid;

   serviceId = INUG_SERVICE_INDEX(cmd);
   if(serviceId < INUG_NUM_SERVICES_E)
   {
      mid = marshal_id_lut_entry(serviceId,INUG_SERVICE_IOCTL_INDEX(cmd)-1);
      if(mid != M_INVALID_ENTRY)
      {
         if (dir == MARSHALG_DO)
            *pLen = marshal_msg(mid, pStIoctl, pBufOut);
         else
            *pLen = unmarshal_msg(mid, pStIoctl, pBufOut);
      }
   }
}

UINT32 MARSHALG_getSize(INUG_ioctlCmdE cmd)
{
   UINT32 serviceId;
   marshal_id_e mid;
   UINT32 ret = 0;

   serviceId = INUG_SERVICE_INDEX(cmd);
   if(serviceId < INUG_NUM_SERVICES_E)
   {
      mid = marshal_id_lut_entry(serviceId,INUG_SERVICE_IOCTL_INDEX(cmd)-1);
      if(mid != M_INVALID_ENTRY)
         ret = marshal_tbl_entry(mid)->size_of;
   }
   return ret;
}

UINT32 MARSHALG_getMarshalSize(INUG_ioctlCmdE cmd)
{
   UINT32 serviceId;
   marshal_id_e mid;
   UINT32 ret = 0;

   serviceId = INUG_SERVICE_INDEX(cmd);
   if(serviceId < INUG_NUM_SERVICES_E)
   {
      mid = marshal_id_lut_entry(serviceId,INUG_SERVICE_IOCTL_INDEX(cmd)-1);
      if(mid != M_INVALID_ENTRY)
         ret = marshal_tbl_entry(mid)->m_size;
   }
   return ret;
}

//TODO: these have to return the max of the specific service and the general id
UINT32 MARSHALG_getMaxSize(INUG_serviceIdE sid)
{
   if(sid < INUG_NUM_SERVICES_E)
   {
      return MAX(svc_ioctl_tbl[sid].max_size, svc_ioctl_tbl[INUG_GENERAL_SID].max_size);
   }
   return 0;
}

UINT32 MARSHALG_getMaxMarshalSize(INUG_serviceIdE sid)
{
   if(sid < INUG_NUM_SERVICES_E)
   {
      return MAX(svc_ioctl_tbl[sid].max_m_size, svc_ioctl_tbl[INUG_GENERAL_SID].max_m_size);
   }
   return 0;
}

void MARSHALG_showIoctlTbl(int flat)
{
   unsigned int i, j;

   printf("\nmarshal ioctl tbl (flat=%d):\n", flat);
   show_marshal_tbl_stats();
   LOGG_PRINT(LOG_INFO_E, NULL, "svc_ioctl_tbl_size=%d\n",sizeof(svc_ioctl_tbl));

   for (i = 0; i < INUG_NUM_SERVICES_E; i++)
   {
      printf("sid %d: max_size %d max_m_size=%d\n", i, svc_ioctl_tbl[i].max_size, svc_ioctl_tbl[i].max_m_size);
      for (j = 0; j < MAX_IOCTL_PER_SERVICE; j++)
      {
         if (marshal_id_lut_entry(i,j) != M_INVALID_ENTRY)
         {
            printf( "ioctl to marshal : %d -> %d\n", j + 1, marshal_id_lut_entry(i,j));
            if (flat)
               show_marshal_flat(marshal_tbl_entry(marshal_id_lut_entry(i,j)), 1, 0);
            else
               show_marshal_tree(marshal_tbl_entry(marshal_id_lut_entry(i,j)),"",0);
         }
      }
   }
}

void MARSHALG_showMaxSizes(void)
{
   unsigned int i;
   for (i = 0; i < INUG_NUM_SERVICES_E; i++)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "sid %d: max_size %d max_m_size=%d\n", i, svc_ioctl_tbl[i].max_size, svc_ioctl_tbl[i].max_m_size);
   }
}

void MARSHALG_showTbl(int flat)
{
   unsigned int i;

   printf("marshal table (flat=%d):\n", flat);
   show_marshal_tbl_stats();
   LOGG_PRINT(LOG_INFO_E, NULL, "svc_ioctl_tbl_size=%d\n", sizeof(svc_ioctl_tbl));

   if(flat)
   {
      for (i = 0; i < M_NUM_COMPOUND_TYPES_E; i++)
         show_marshal_flat(marshal_tbl_entry(i), 1, 0);
   }
   else
   {
      for (i = 0; i < M_NUM_COMPOUND_TYPES_E; i++)
         show_marshal_tree(marshal_tbl_entry(i), "",0);
   }
}



#ifdef __cplusplus
}
#endif