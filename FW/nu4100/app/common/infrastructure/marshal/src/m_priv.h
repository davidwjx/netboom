#ifndef _M_PRIV_H_
#define _M_PRIV_H_

//#define MARSHAL_TEST
#include "m_tbl.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
***************     G L O B A L        D E F N I T I O N S    **************
****************************************************************************/
//#define M_STATIC_MEMBERS

#define MAX_IOCTL_PER_SERVICE (INUG_SERVICE_IOCTL_INDEX(INTERNAL_CMDG_LAST_E) - 1)
#define MAX_MEMBERS (30)
#define INUG_GENERAL_SID (INUG_SERVICE_MNGR_E)

/****************************************************************************
***************      G L O B A L         T Y P E D E F S     ***************
****************************************************************************/
typedef struct
{
   marshal_id_e m_id;
   UINT32 num_elements;
   UINT32 size_of;
   UINT32 offset_of;
} marshal_memb_t;

typedef struct
{
   marshal_id_e m_id;
   UINT32 size_of;
   UINT32 m_size;
   UINT32 num_members;
#ifdef M_STATIC_MEMBERS
   marshal_memb_t members[MAX_MEMBERS];
#else
   marshal_memb_t *members;
#endif
} marshal_entry_t;

/****************************************************************************
***************     G L O B A L         F U N C T I O N S    ***************
****************************************************************************/
void marshal_u16(INT16 dir, UINT16 *dataP, UINT8 *bufP);
void marshal_u32(INT16 dir, UINT32 *dataP, UINT8 *bufP);
void marshal_u64(INT16 dir, UINT64 *dataP, UINT8 *bufP);
unsigned int marshal_msg(marshal_id_e mid, void *msg_ptr, UINT8* buf);
unsigned int unmarshal_msg(marshal_id_e mid, void *msg_ptr, UINT8* buf);
void show_marshal_tree(marshal_entry_t *entry, const char *tabs, int verbose);
int show_marshal_flat(marshal_entry_t *entry, int top, unsigned int index);

marshal_entry_t *marshal_tbl_entry(unsigned int index);
marshal_id_e  marshal_id_lut_entry(int sid, unsigned int ioctl);
void init_marshal_tbl(void);
void deinit_marshal_tbl(void);
void show_marshal_tbl_stats(void);
UINT32 get_marshal_tbl_hash(void);

#ifdef MARSHAL_TEST
void marshal_test();
typedef struct
{
   UINT32 i;
   UINT8 c;
   UINT32 arr[2];
   UINT8 c2;
} simple_t;

typedef struct
{
   UINT32 i;
   simple_t s;
} compound_t;

typedef struct
{
   UINT32 i;
   simple_t s[2][2];
   compound_t cm;
} compound2_t;

//INIT_M_ENTRY(simple_t, 4, _m(simple_t, i, _B, 1), _m(simple_t, c, _B, 1), _m(simple_t, arr, _B, 2), _m(simple_t, c2, _B, 1));
//INIT_M_ENTRY(compound_t, 2, _m(compound_t, i, _B, 1), _m(compound_t, s, simple_t, 1));
//INIT_M_ENTRY(compound2_t, 3, _m(compound2_t, i, _B, 1), _m(compound2_t, s, simple_t, 4), _m(compound2_t, cm, compound_t, 1));

#endif


#ifdef __cplusplus
}
#endif
#endif //_M_PRIV_H_