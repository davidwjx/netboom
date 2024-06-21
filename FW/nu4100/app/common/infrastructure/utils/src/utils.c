/****************************************************************************
 *
 * FileName: utils.h
 *
 * Author: Benny V
 *
 * Date: 04/07/13
 *
 * Description: general purpose utilities functions
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_types.h"
#include "err_defs.h"
#include "log.h"
#include "utils.h"
#if DEFSG_IS_GP
#include <execinfo.h>
#endif
#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define UTILSP_FILE_NAME_MAX_LEN   30
#define UTILSP_FULL_PATH_MAX_LEN   50

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

// memory functions
/****************************************************************************
*
*  Function Name: UTILSG_initBufferVal
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void UTILSG_initBufferVal(UINT8* bufPtr, UINT32 bufSize, UINT8 val)
{
   UINT32 i;

   for(i=0; i < bufSize; i++)
   {
      *(bufPtr+i) = val;
   }
}

/****************************************************************************
*
*  Function Name: UTILSG_initBufferCounter
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void UTILSG_initBufferCounter(UINT16* bufPtr, UINT32 bufSize, UINT16 initVal, UINT32 wrapAround)
{
   UINT32 i;
   UINT16 counter = initVal;

   for(i=0; i < bufSize; i++)
   {
      *(bufPtr+i) = counter++;
      if(counter >= wrapAround)
         counter = initVal;
   }
}

/****************************************************************************
*
*  Function Name: UTILSG_initBufferSpecialPatten
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void UTILSG_initBufferSpecialPatten(UINT16* bufPtr, UINT32 width, UINT32 height)
{
   UINT16 i, j;
   UINT16 val = 0;

   for(i=0; i <= 255; i++)
   {
      val = i << 8;
      for(j=0; j <= 255; j++)
      {
         *(bufPtr + i*width + j) = val;
         val++;
      }

      val--;

      for( ; j <= 511; j++)
      {
         *(bufPtr + i*width + j) = val;
         val--;
      }

//      val = i << 8;
      for( ; j < width; j++)
      {
         *(bufPtr + i*width + j) = 0;
//         val++;
      }
   }

   for( ; i <= 510; i++)
   {
      if(i == height)
      {
         printf("i=height=%d\n", i);
         break;
      }

      val = (510- i) << 8;
      for(j=0; j <= 255; j++)
      {
         *(bufPtr + i*width + j) = val;
         val++;
      }

      val--;

      for( ; j <= 511; j++)
      {
         *(bufPtr + i*width + j) = val;
         val--;
      }

//      val = (510- i) << 8;
      for( ; j < width; j++)
      {
         *(bufPtr + i*width + j) = 0;
//         val++;
      }
   }

   for( ; i < height; i++)
   {
      for(j=0; j < width; j++)
      {
         *(bufPtr + i*width + j) = 0;
      }
   }

   for(i=0 ; i < 1; i++)
   {
      for(j=0; j < width; j++)
      {
         printf("0x%X ", *(bufPtr + i*width + j));
//         printf("0x%X (i=%d, j=%d)", *(bufPtr + i*width + j), i, j);
      }
      printf("\n");
   }

}

/****************************************************************************
*
*  Function Name: UTILSG_compBufToVal
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
UINT32 UTILSG_compBufToVal(UINT8* bufPtr, UINT32 bufSize, UINT8 val)
{
   UINT32 i=0;
//   LOGG_PRINT(LOG_ERROR_E, NULL, "compare buffer to val: bufPtr = 0x%X, bufSize = 0x%X, val = %d\n", (UINT32)bufPtr, bufSize, val);
   for(i=0; i < bufSize; i++)
   {
      if(*(bufPtr+i) != val)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "buffer compare to val fails !!!\n");
         LOGG_PRINT(LOG_ERROR_E, NULL, "bufPtr[%d]=%d != %d (expected) \n ", i, *(bufPtr+i), val);
         return(FAIL_E);
      }
   }
   return(SUCCESS_E);
}

/****************************************************************************
*
*  Function Name: UTILSG_compBufToCounter
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
UINT32 UTILSG_compBufToCounter(UINT16* bufPtr, UINT32 bufSize, UINT16 initVal, UINT32 wrapAround, UINT32 *failurePixelP)
{
   UINT32 i=0;
//   LOGG_PRINT(LOG_ERROR_E, NULL, "compare buffer to val: bufPtr = 0x%X, bufSize = 0x%X, val = %d\n", (UINT32)bufPtr, bufSize, val);
   for(i=0; i < bufSize; i++)
   {
      if(initVal == wrapAround)
         initVal = 0;
      if(*(bufPtr+i) != initVal++)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "buffer compare to counter fails !!!\n");
         LOGG_PRINT(LOG_ERROR_E, NULL, "bufPtr[%d]=%d != %d (expected) \n ", i, *(bufPtr+i), initVal);
         *failurePixelP = i + 1;
         return(FAIL_E);
      }
   }
   return(SUCCESS_E);
}

/****************************************************************************
*
*  Function Name: UTILSG_compBuffers
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
UINT32 UTILSG_compBuffers(UINT8* buf1Ptr, UINT8* buf2Ptr, UINT32 bufSize)
{
   UINT32 i=0;
//   LOGG_PRINT(LOG_ERROR_E, NULL, "compare DDR buffers: buf1Ptr = 0x%X, buf2Ptr = 0x%X, bufSize = 0x%X\n", (UINT32)buf1Ptr, (UINT32)buf2Ptr, bufSize);
   for(i=0; i < bufSize; i++)
   {
      if(*(buf1Ptr+i) != *(buf2Ptr+i))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "buffers compare fails !!!\n");
         LOGG_PRINT(LOG_ERROR_E, NULL, "buf1Ptr[%d]=%d != buf2Ptr[%d]=%d \n ", i, *(buf1Ptr+i), i, *(buf2Ptr+i));
         return(FAIL_E);
      }
   }
//   LOGG_PRINT(LOG_ERROR_E, NULL, "buffer compare test PASS!!!\n");
   return(SUCCESS_E);
}

/****************************************************************************
*
*  Function Name: UTILSG_printBufValues
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void UTILSG_printBufValues(UINT16* bufPtr, UINT32 bufSize)
{
   UINT32 i;
   for(i=0; i < bufSize; i++)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "bufffer pointer = 0x%X, value = 0x%X\n", (bufPtr+i), *(bufPtr+i));
   }
}

/****************************************************************************
*
*  Function Name: UTILSG_dump_hex
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void UTILSG_dump_hex(UINT8 *buf,int len)
{
   int i = 0;
   for(i = 0; i < len; i++)
   {
      fprintf(stderr, "0x%02x ", buf[i]);
      if( ((i+1) % 16) == 0)
      {
         fprintf(stderr, "\n");
      }
     
   }
   fprintf(stderr, "\n");
}

// IO functions
/****************************************************************************
*
*  Function Name: UTILSG_writeToFile
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void UTILSG_writeToFile(UINT8* bufPtr, UINT32 bufSize, char fileName[])
{
   char fullPath[UTILSP_FULL_PATH_MAX_LEN] = "/media/tmp/";
   FILE *pFile;
   if( strlen(fileName) > UTILSP_FILE_NAME_MAX_LEN)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "fail to create file. file name length too long. %d > %d", strlen(fileName), UTILSP_FILE_NAME_MAX_LEN);
   }
   else
   {
      strncat(fullPath, fileName, UTILSP_FILE_NAME_MAX_LEN);
      LOGG_PRINT(LOG_ERROR_E, NULL, "open file: %s\n", fullPath);
      pFile = fopen(fullPath,"wb");
      if (pFile == NULL)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "fopen fails.\n");
      }
      LOGG_PRINT(LOG_ERROR_E, NULL, "write %d bytes to file\n", bufSize);
      if(fwrite(bufPtr , 1 , bufSize, pFile) != bufSize)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "fwrite fails\n");
      }
      fclose(pFile);
   }
}

// CRC functions
#define WIDTH  (8 * sizeof(UINT8))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8  /* 11011 followed by 0's */

/****************************************************************************
*
*  Function Name: UTILSG_crcInit
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void UTILSG_crcInit(UINT8 *crcTableP)
{
   UINT8  remainder, bit;   
   int dividend;

   // Compute the remainder of each possible dividend
   for (dividend = 0; dividend < UTILSG_CRC_TABLE_SIZE; ++dividend)
   {
      // Start with the dividend followed by zeros.
      remainder = (UINT8)(dividend << (WIDTH - 8));

      // Perform modulo-2 division, a bit at a time.
      for (bit = 8; bit > 0; --bit)
      {
         // Try to divide the current data bit.
         if (remainder & TOPBIT)
         {
         remainder = (remainder << 1) ^ POLYNOMIAL;
         }
         else
         {
         remainder = (remainder << 1);
         }
      }

      // Store the result into the table.
      *(crcTableP + dividend) = remainder;
   }

}

/****************************************************************************
*
*  Function Name: UTILSG_calcCrc
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
UINT8 UTILSG_calcCrc(UINT8 const message[], int nBytes, UINT8 *crcTableP)
{
   UINT8 data;
   UINT8 remainder = 0;
   int byte;

   // Divide the message by the polynomial, a byte at a time.
   for (byte = 0; byte < nBytes; ++byte)
   {
      data = message[byte] ^ (remainder >> (WIDTH - 8));
      remainder = *(crcTableP + data) ^ (remainder << 8);
   }

   // The final remainder is the CRC.
   return (remainder);

}
#if DEFSG_IS_GP
/****************************************************************************
*
*  Function Name: UTILSG_print_trace
*
*  Description: Obtain a backtrace and print it to stdout
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void UTILSG_print_trace (void)
{
  void *array[10];
  char **strings;
  int size, i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);
  if (strings != NULL)
  {

    printf ("Obtained %d stack frames.\n", size);
    for (i = 0; i < size; i++)
      printf ("%s\n", strings[i]);
  }

  free (strings);
}
#endif

#ifdef __cplusplus
}
#endif

