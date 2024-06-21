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

/****************************************************************************
***************     P R E    D E F I N I T I O N     OF      ***************
***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/

/****************************************************************************
***************       L O C A L         D A T A              ***************
****************************************************************************/

/****************************************************************************
***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/
static void show_builtin_member_flat(marshal_memb_t *memb, unsigned int *index)
{
   unsigned int j;

   if (memb->size_of / memb->num_elements == 1)
   {
      //char arrays - print as single element
      printf("[%d]sizeof=%d offsetof=%d\n", *index, memb->size_of, memb->offset_of);
      (*index)++;
   }
   else
   {
      for (j = 0; j < memb->num_elements; j++)
      {
         printf("[%d]sizeof=%d offsetof=%d\n", *index, (memb->size_of / memb->num_elements), memb->offset_of);
         (*index)++;
      }
   }
}

static void show_comp_member_flat(marshal_memb_t *memb, unsigned int *index)
{
   unsigned int j;
   for (j = 0; j < memb->num_elements; j++)
   {
      *index = show_marshal_flat(marshal_tbl_entry(memb->m_id), 0, *index);
   }
}

static void show_builtin_member_tree(marshal_memb_t *memb, const char *tabs, int verbose)
{
   unsigned int j;
   for (j = 0; j < memb->num_elements && (memb->num_elements > 1); j++)
   {
      printf("%s    |--sizeof=%d offsetof=%d\n", tabs, (memb->size_of / memb->num_elements),
         memb->offset_of + (memb->size_of / memb->num_elements)*j);
      if (!verbose)
      {
         printf("%s    |--repeat %d elements\n", tabs, memb->num_elements);
         break;
      }
   }
}

static void show_comp_member_tree(marshal_memb_t *memb, const char *tabs, int verbose)
{
   unsigned int j;
   char tabstr[64];
   const char *tabptr = tabs;

   for (j = 0; j < memb->num_elements; j++)
   {
      if (strlen(tabs) + 4 < sizeof(tabstr))
      {
         sprintf(tabstr, "%s\t\t", tabs);
         tabptr = tabstr;
      }
      show_marshal_tree(marshal_tbl_entry(memb->m_id), tabptr, verbose);
      if (memb->num_elements > 1)
      {
         printf("%s    |--c sizeof=%d offsetof=%d\n", tabs, memb->size_of / memb->num_elements,
            memb->offset_of + (memb->size_of / memb->num_elements)*j);
         if (!verbose)
         {
            printf("%s    |--repeat %d elements\n", tabs, memb->num_elements);
            break;
         }
      }
   }
}

/*
*  marshal built-in types
*/
static unsigned int marshal_builtin_type(UINT16 dir, marshal_id_e mid, unsigned int num_elements, UINT8 *msg_ptr, UINT8 *buf)
{
   unsigned int i;
   unsigned int ret = 0;
   int do_copy = (msg_ptr && buf);

   switch (mid)
   {
   case(M_INT8) :
   {
      if (do_copy)
      {
         if (dir == MARSHALG_DO)
            memcpy(buf, msg_ptr, num_elements);
         else
            memcpy(msg_ptr, buf, num_elements);
      }
      ret = num_elements;
   }
   break;
   case(M_INT16) :
   {
      for (i = 0; i < num_elements; i++)
      {
         if (do_copy)
            marshal_u16(dir, (UINT16 *)(msg_ptr + (i*sizeof(UINT16))), (buf + (i*sizeof(UINT16))));
         ret += sizeof(UINT16);
      }
   }
   break;
   case(M_INT32) :
   {
      for (i = 0; i < num_elements; i++)
      {
         if (do_copy)
            marshal_u32(dir, (UINT32 *)(msg_ptr + (i*sizeof(UINT32))), (buf + (i*sizeof(UINT32))));
         ret += sizeof(UINT32);
      }
   }
   break;
   case(M_INT64) :
   {
      for (i = 0; i < num_elements; i++)
      {
         if (do_copy)
            marshal_u64(dir, (UINT64 *)(msg_ptr + (i*sizeof(UINT64))), (buf + (i*sizeof(UINT64))));
         ret += sizeof(UINT64);
      }
   }
   break;

   case(M_BPTR) :
   {
      //ptrs are not really sent over the Wire. But local structs may have them.
      //Even non-local structs have them and they're just ignored. So comment out the assert.
      //assert_func((!do_copy), "marshal: marshalling a pointer");
      ret = 0;
   }
   break;

   default:
      break;
   }

   return ret;
}

/*
marshal messsage - recursively
*/
static unsigned int _marshal_msg(UINT16 dir, marshal_id_e mid, UINT8 *msg_ptr, UINT8* buf)
{
   unsigned int i, j;
   unsigned int offset = 0;
   marshal_entry_t *entry;
   marshal_memb_t *memb;
   UINT8 *msg_offset_ptr, *buf_offset_ptr;
   int do_copy = (msg_ptr && buf);

   entry = marshal_tbl_entry(mid);
   for (i = 0; i < entry->num_members; i++)
   {
      memb = &entry->members[i];
      if (memb->m_id >= M_FIRST_BUILTIN_TYPE_E)
      {
         msg_offset_ptr = (do_copy) ? &msg_ptr[memb->offset_of] : NULL;
         buf_offset_ptr = (do_copy) ? &buf[offset] : NULL;
 //        LOGG_PRINT(LOG_DEBUG_E, NULL, " marshal builtin id=%d msg_in=%p msg_out=%p offset=%d\n", memb->m_id, msg_offset_ptr, buf_offset_ptr, offset);
         offset += marshal_builtin_type(dir, memb->m_id, memb->num_elements, msg_offset_ptr, buf_offset_ptr);
      }
      else
      {
         for (j = 0; j < memb->num_elements; j++)
         {
            msg_offset_ptr = (do_copy) ? &msg_ptr[memb->offset_of + (j*(memb->size_of / memb->num_elements))] : NULL;
            buf_offset_ptr = (do_copy) ? &buf[offset] : NULL;
 //           LOGG_PRINT(LOG_DEBUG_E, NULL, " r_marshal j=%d id=%d msg_in=%p msg_out=%p offset=%d\n", j, memb->m_id, msg_offset_ptr, buf_offset_ptr, offset);
            offset += _marshal_msg(dir, memb->m_id, msg_offset_ptr, buf_offset_ptr);
         }
      }
   }
   return offset;
}


/****************************************************************************
***************     G L O B A L         F U N C T I O N S    ***************
****************************************************************************/

void marshal_u16(INT16 dir, UINT16 *dataP, UINT8 *bufP)
{
   if (dir == MARSHALG_DO)
   {
      UINT16 data = *dataP;
      //Convert to byte little-endian
      bufP[0] = (UINT8)(data & 0xFF);
      bufP[1] = (UINT8)((data & 0xFF00) >> 8);
   }
   else
   {
      //Convert from little-endian to local format. 
      *dataP = (UINT16)(bufP[0] | (bufP[1] << 8));
   }
}

void marshal_u32(INT16 dir, UINT32 *dataP, UINT8 *bufP)
{
   if (dir == MARSHALG_DO)
   {
      UINT32 data = *dataP;
      //Convert to byte little-endian
      bufP[0] = (UINT8)(data & 0xFF);
      bufP[1] = (UINT8)((data & 0xFF00) >> 8);
      bufP[2] = (UINT8)((data & 0xFF0000) >> 16);
      bufP[3] = (UINT8)((data & 0xFF000000) >> 24);
   }
   else
   {
      //Convert from little-endian to local format. 
      *dataP = (UINT32)(bufP[0] | (bufP[1] << 8) | (bufP[2] << 16) | (bufP[3] << 24));
   }
}

void marshal_u64(INT16 dir, UINT64 *dataP, UINT8 *bufP)
{
   if (dir == MARSHALG_DO)
   {
      UINT64 data = *dataP;
      //Convert to byte little-endian
      bufP[0] = (UINT8)(data & 0xFF);
      bufP[1] = (UINT8)((data & 0xFF00) >> 8);
      bufP[2] = (UINT8)((data & 0xFF0000) >> 16);
      bufP[3] = (UINT8)((data & 0xFF000000) >> 24);
      bufP[4] = (UINT8)((data & 0xFF00000000) >> 32);
      bufP[5] = (UINT8)((data & 0xFF0000000000) >> 40);
      bufP[6] = (UINT8)((data & 0xFF000000000000) >> 48);
      bufP[7] = (UINT8)((data & 0xFF00000000000000) >> 56);
   }
   else
   {
      UINT32 dataHigh, dataLow;
      *dataP = 0;
      dataHigh = (bufP[4]) | (bufP[5] << 8) | (bufP[6] << 16) | (bufP[7] << 24);
      dataLow = (bufP[0]) | (bufP[1] << 8) | (bufP[2] << 16) | (bufP[3] << 24);
      *dataP = ((UINT64)(dataHigh) << 32) | (UINT64)dataLow;
   }
}

/*
   marshal messsage - recursively
   Return marshaled message size
*/
unsigned int marshal_msg(marshal_id_e mid, void *msg_ptr, UINT8* buf)
{
   return _marshal_msg(MARSHALG_DO, mid, (UINT8 *)msg_ptr, buf);
}
/*
   unmarshal message - recursively.
   Return unmarshaled message size (native sizeof)
*/
unsigned int unmarshal_msg(marshal_id_e mid, void *msg_ptr, UINT8* buf)
{
   (void)_marshal_msg(MARSHALG_UNDO, mid, (UINT8 *)msg_ptr, buf);
   return marshal_tbl_entry(mid)->size_of;
}

/*
   show marshal entry in tree format.
*/
void show_marshal_tree(marshal_entry_t *entry, const char *tabs, int verbose)
{
   unsigned int i;
   marshal_memb_t *memb;

   printf("%sm_entry: id=%d sizeof=%d m_size=%d members=%d\n", tabs, entry->m_id, entry->size_of, entry->m_size, entry->num_members);

   for (i = 0; i < entry->num_members; i++)
   {
      memb = &entry->members[i];

      printf("%s[%d]%s id=%d sizeof=%d ne=%d offset=%d\n", tabs, i, (memb->m_id >= M_FIRST_BUILTIN_TYPE_E) ? "b" : "c",
         memb->m_id, memb->size_of, memb->num_elements, memb->offset_of);

      if (memb->m_id >= M_FIRST_BUILTIN_TYPE_E)
         show_builtin_member_tree(memb, tabs, verbose);
      else
         show_comp_member_tree(memb, tabs, verbose);
   }

   if (strlen(tabs) == 0)
      printf("\n");
}

/*
   show marshal entry in flat format (with accumulating member index).
*/
int show_marshal_flat(marshal_entry_t *entry, int top, unsigned int index)
{
   unsigned int i;
   marshal_memb_t *memb;

   if (top)
      printf("top ");
   printf("m_entry: id=%d sizeof=%d m_size=%d members=%d\n", entry->m_id, entry->size_of, entry->m_size, entry->num_members);

   for (i = 0; i < entry->num_members; i++)
   {
      memb = &entry->members[i];
      if (memb->m_id >= M_FIRST_BUILTIN_TYPE_E)
         show_builtin_member_flat(memb, &index);
      else
         show_comp_member_flat(memb, &index);
   }

   if (top)
      printf("\n");
   return index;
}

#ifdef MARSHAL_TEST
#include "utils.h"
void marshal_test()
{
   UINT8 buf[1024];
   unsigned int len;
   compound_t com;
   compound2_t com2;
   simple_t sim;

   MARSHALG_showTbl(0);

   ///////////////////////////////////////////
   sim.i = 0x12345678;
   sim.c = 0x66;
   sim.arr[0] = 0xdeadbeef;
   sim.arr[1] = 0x55aa9944;
   sim.c2 = 0xbb;

   ///////////////////////////////////////////
   com.i = 0x9abcdef0;
   com.s.i = 0x22223333;
   com.s.c = 0x7f;
   com.s.arr[0] = 0x55556666;
   com.s.arr[1] = 0x77778888;
   com.s.c2 = 0xcc;

   ///////////////////////////////////////////
   com2.i = 0x9abcdef0;
   com2.s[0][0].i = 0x02223333;
   com2.s[0][0].c = 0x0a;
   com2.s[0][0].arr[0] = 0x05556666;
   com2.s[0][0].arr[1] = 0x07778888;
   com2.s[0][0].c2 = 0x0a;

   com2.s[0][1].i = 0x12223333;
   com2.s[0][1].c = 0x1a;
   com2.s[0][1].arr[0] = 0x15556666;
   com2.s[0][1].arr[1] = 0x17778888;
   com2.s[0][1].c2 = 0x1a;

   com2.s[1][0].i = 0x22223333;
   com2.s[1][0].c = 0x2a;
   com2.s[1][0].arr[0] = 0x25556666;
   com2.s[1][0].arr[1] = 0x27778888;
   com2.s[1][0].c2 = 0x2a;

   com2.s[1][1].i = 0x32223333;
   com2.s[1][1].c = 0x3a;
   com2.s[1][1].arr[0] = 0x35556666;
   com2.s[1][1].arr[1] = 0x37778888;
   com2.s[1][1].c2 = 0x3a;

   com2.cm.i = 0x88889999;
   com2.cm.s.i = 0xaaaabbbb;
   com2.cm.s.c = 0xcf;
   com2.cm.s.arr[0] = 0xc5556666;
   com2.cm.s.arr[1] = 0xc7778888;
   com2.cm.s.c2 = 0xca;
   ///////////////////////////////////////////

   len = marshal_msg(M_simple_t, &sim, buf);
   LOGG_PRINT(LOG_INFO_E, NULL, "simple_t marshal len = %d sizeof=%d\n", len, sizeof(simple_t));
   UTILSG_dump_hex(buf, len);
   memset(&sim, 0x0, sizeof(simple_t));
   len = unmarshal_msg(M_simple_t, &sim, buf);
   LOGG_PRINT(LOG_INFO_E, NULL, "simple_t unmarshal len = %d sizeof=%d\n", len, sizeof(simple_t));

   len = marshal_msg(M_compound_t, &com, buf);
   LOGG_PRINT(LOG_INFO_E, NULL, "compound_t marshal len = %d sizeof=%d\n", len, sizeof(compound_t));
   UTILSG_dump_hex(buf, len);
   memset(&com, 0x0, sizeof(compound_t));
   len = unmarshal_msg(M_compound_t, &com, buf);
   LOGG_PRINT(LOG_INFO_E, NULL, "compound_t unmarshal len = %d sizeof=%d\n", len, sizeof(compound_t));

   len = marshal_msg(M_compound2_t, &com2, buf);
   LOGG_PRINT(LOG_INFO_E, NULL, "compound2_t marshal len = %d sizeof=%d\n", len, sizeof(compound2_t));
   UTILSG_dump_hex(buf, len);
   memset(&com2, 0x0, sizeof(compound2_t));
   len = unmarshal_msg(M_compound2_t, &com2, buf);
   LOGG_PRINT(LOG_INFO_E, NULL, "compound2_t unmarshal len = %d sizeof=%d\n", len, sizeof(compound2_t));
}
#endif

#ifdef __cplusplus
}
#endif
