/*
* hash table api.
* Features:
*    Open addressing with linear probing for handling collisions
*    Hashing on memory buffers of any length (strings, integers, etc)
*    Memory for hash table is allocated by user. No internal memory.
*    Includes a simple 32bit multiplier hash, user can input own hash.
*/

#include "inu_common.h"
#include <string.h>
#include <stdlib.h>
#include "hash.h"

#ifdef __cplusplus
extern "C" {
#endif

static UINT32 hashMult(const UINT8 *buf, UINT32 len)
{
   enum {MULTIPLIER = 37};
   UINT32 hash = 0;
   unsigned int i;
   
   for(i = 0; i < len; i++)
   {
      hash = hash * MULTIPLIER + buf[i]; //implicit mod 2^32
   }
   return hash;
}

void HASHG_initTbl(HASHG_tblT *htbl, HASHG_itemT *m, UINT32 s, HASHG_hfuncT f, HASHG_keycmpT c)
{
   unsigned int i;

   
   (htbl)->mem = m;
   (htbl)->size = s;
   (htbl)->hfunc = f;
   (htbl)->kcmp = c;
   for (i = 0; i < htbl->size; i++)
   {
      (htbl)->mem[i].keylen = HASHG_EMPTY_ITEM;
   }
}
/*
   Return pointer to item in memory of hash table or null if not found.
*/

HASHG_itemT *HASHG_find(HASHG_tblT *htbl, const UINT8 *key, UINT32 klen)
{
   UINT32 index,count;
   HASHG_itemT *item = NULL;
   int diff = 1;
      
   if(htbl->hfunc)
      index = htbl->hfunc(key,klen); 
   else
      index = hashMult(key,klen) % (htbl->size);

   count = 0;
   item = &htbl->mem[index];
   while((item->keylen > 0) || (item->keylen == HASHG_DELETED_ITEM))
   {   
      if(item->keylen != HASHG_DELETED_ITEM)
      {
         if(htbl->kcmp) 
            diff = htbl->kcmp(item->key, key, klen);
         else
            diff = memcmp(item->key,key, klen);

         if((diff == 0))
            break; //found
      }

      index++; //next index - linear probe with wrap
      if(index >= htbl->size)
         index = 0;

      count++;
      if(count >= htbl->size)
         break; //full wrap, 

      item = &htbl->mem[index];
   }

   if(!diff)
      return item;

   return NULL;

}

/*
* insert returns index in table, and -1 if table is full
*/
INT32 HASHG_insert(HASHG_tblT *htbl, HASHG_itemT *item)
{
   UINT32 index,count;
   INT32 ret = -1;
   HASHG_itemT *currItem;

   if(htbl->hfunc)
      index = htbl->hfunc(item->key,item->keylen); 
   else
      index = hashMult(item->key,item->keylen) % (htbl->size);

   count = 0;
   
   currItem = &htbl->mem[index];
   while(currItem->keylen > 0)
   {   
      index++; //next index - linear probe with wrap
      if(index >= htbl->size)
         index = 0;

      count++;
      if(count >= htbl->size)
         break; //full wrap, table full

      currItem = &htbl->mem[index];
   }

   if(count < htbl->size)
   {
      memcpy(&htbl->mem[index], item, sizeof(HASHG_itemT));
      ret = index;
   }
   
   return ret;

}

int HASHG_delete(HASHG_tblT *htbl, const UINT8 *key, UINT32 klen)
{
   UINT32 index,count;
   HASHG_itemT *item = NULL;
   int diff = 1;
   INT32 ret = -1;
      
   if(htbl->hfunc)
      index = htbl->hfunc(key,klen); 
   else
      index = hashMult(key,klen) % (htbl->size);

   count = 0;

   item = &htbl->mem[index];
   while((item->keylen >0) || (item->keylen == HASHG_DELETED_ITEM))
   {   
      if(item->keylen != HASHG_DELETED_ITEM)
      {
         if(htbl->kcmp)
            diff = htbl->kcmp(item->key, key,klen);
         else
            diff = memcmp(item->key,key,klen);
      }

      if(diff == 0)
      {
         item->keylen = HASHG_DELETED_ITEM;
         ret = index;
         break; //found and deleted
      }

      index++; //next index - linear probe with wrap
      if(index >= htbl->size)
         index = 0;

      count++;
      if(count >= htbl->size)
         break; //full wrap
      
      item = &htbl->mem[index];
   }

   return ret;

}

void HASHG_display(HASHG_tblT *htbl) 
{
   unsigned int i = 0;
   
   for(i = 0; i<htbl->size; i++) 
   {
      LOGG_PRINT(LOG_INFO_E,0,"[%d]key_len=%d,key_ptr=%p,data_ptr=%p\n",i,htbl->mem[i].keylen, htbl->mem[i].key, htbl->mem[i].data.ptr);
   }
}

#if 0
#define TEST_SIZE (8)
UINT32 test_hfunc(const UINT8 *key, UINT32 len)
{
   return hashMult(key, len) % (TEST_SIZE);
}
int test_kcmp(const UINT8 *k1, const UINT8 *k2, unsigned int len)
{
   return memcmp(k1, k2, len);
}

void hash_test() 
{
   struct test_item
   {
      const char *str;
      char c;
   };

   unsigned int i;
   HASHG_itemT tblMem[TEST_SIZE];
   HASHG_tblT hashTbl;
   HASHG_itemT item;
   HASHG_itemT *itemP;
   int ret;
   struct test_item test_item_tbl[] = { { "abcd",'a' },{ "bke",'b' },{ "ab",'c' },{ "d",'d' } ,
                                        { "1111",'1' },{ "222",'2' },{ "33",'3' },{ "444",'4' },
                                        /*{ "5555",'5' },{ "66",'6' },{ "77",'7' },{ "888",'8' },*/
   };
   int error_cnt = 0;

   //HASHG_initTbl(&hashTbl, tblMem, TEST_SIZE, test_hfunc, test_kcmp);
   HASHG_initTbl(&hashTbl, tblMem, TEST_SIZE, NULL,NULL);
   
   for (i = 0; i < sizeof(test_item_tbl) / sizeof(test_item_tbl[0]); i++)
   {
      item.key = (UINT8 *)test_item_tbl[i].str;
      item.keylen = (UINT16)strlen(test_item_tbl[i].str);
      item.data.ui = (UINT32)test_item_tbl[i].c;
      ret = HASHG_insert(&hashTbl, &item);
      if (ret < 0)
      {
         LOGG_PRINT(LOG_INFO_E, 0, "insert failed item %d\n", i);
         error_cnt++;
      }
   }

   HASHG_display(&hashTbl);

   //TEST inserts and find
   for (i = 0; i < sizeof(test_item_tbl) / sizeof(test_item_tbl[0]); i++)
   {
      itemP = HASHG_find(&hashTbl, (const UINT8 *)test_item_tbl[i].str, (UINT32)strlen(test_item_tbl[i].str));
      if (!itemP)
      {
         LOGG_PRINT(LOG_INFO_E, 0, "error not found %s\n", test_item_tbl[i].str);
         error_cnt++;
      }
      else
         LOGG_PRINT(LOG_INFO_E, 0, "found %s %p\n", itemP->key, itemP->data);
   }

   //Test deletes and find
   for (i = 0; i < sizeof(test_item_tbl) / sizeof(test_item_tbl[0]); i++)
   {
      ret = HASHG_delete(&hashTbl, (const UINT8 *)test_item_tbl[i].str, (UINT32)strlen(test_item_tbl[i].str));
      if (ret < 0)
      {
         LOGG_PRINT(LOG_INFO_E, 0, "delete failed\n");
         error_cnt++;
      }

      itemP = HASHG_find(&hashTbl, (const UINT8 *)test_item_tbl[i].str, (UINT32)strlen(test_item_tbl[i].str));
      if (itemP)
      {
         LOGG_PRINT(LOG_INFO_E, 0, "error found deleted item %s\n", test_item_tbl[i].str);
         error_cnt++;
      }
      else
         LOGG_PRINT(LOG_INFO_E, 0, "not found item %s\n", test_item_tbl[i].str);
   }
   HASHG_display(&hashTbl);
   if(!error_cnt)
      LOGG_PRINT(LOG_INFO_E, 0, "hash test passed\n");
   else
      LOGG_PRINT(LOG_INFO_E, 0, "hash test failed %d errors\n", error_cnt);
   while (1);
}
#endif

#ifdef __cplusplus
}
#endif
