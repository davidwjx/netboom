#ifndef __HASH_TBL__
#define __HASH_TBL__

/*
* hash table api.
*/

#ifdef __cplusplus
extern "C" {
#endif

#define HASHG_EMPTY_ITEM     (-1)
#define HASHG_DELETED_ITEM   (-2)

typedef struct HASHG_tbl_t * HASHG_handleT;
typedef UINT32 (* HASHG_hfuncT)(const UINT8 *buf, UINT32 len);
typedef int (* HASHG_keycmpT)(const UINT8 *k1, const UINT8 *k2, UINT32 len); //like memcmp

typedef union
{
   UINT32 ui;
   void *ptr;
} HASHG_dataU;

typedef struct
{
   INT16 keylen;
   UINT8 *key;
   HASHG_dataU data;
} HASHG_itemT;

typedef struct HASHG_tbl_t
{
   HASHG_itemT *mem; //hash table memory (user supplied). Will contain hash items inserted by user.
   UINT32 size;// hash table size  or number of hash item entries in table. Number of expected items in table should be much lower to avoid collisions.   
   HASHG_hfuncT hfunc;//hash function - optional
   HASHG_keycmpT kcmp; //key compare function (optional memcmp is default)

} HASHG_tblT;

void HASHG_initTbl(HASHG_tblT *htbl, HASHG_itemT *m, UINT32 s, HASHG_hfuncT f, HASHG_keycmpT c);
HASHG_itemT *HASHG_find(HASHG_tblT *htbl, const UINT8 *key, UINT32 klen);
INT32 HASHG_insert(HASHG_tblT *htbl, HASHG_itemT *item);
INT32 HASHG_delete(HASHG_tblT *htbl, const UINT8 *key, UINT32 klen);
void HASHG_display(HASHG_tblT *htbl);

#ifdef __cplusplus
}
#endif
#endif //__HASH_TBL__
