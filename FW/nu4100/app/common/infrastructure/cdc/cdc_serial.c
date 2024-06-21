/****************************************************************************
 *
 *   FileName: cdc_serial.c
 *
 *   Author: Oshri A.
 *
 *   Date:
 *
 *   Description: UART com interface
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "cdc_serial.h"
#include <stdio.h>
#include <ctype.h>

#if (DEFSG_PROCESSOR == DEFSG_GP || DEFSG_GP_HOST)
#include "os_lyr.h"
#endif

#if (DEFSG_PROCESSOR == DEFSG_GP)
#include "gme_drv.h"
#include "gme_mngr.h"
#endif


#if (DEFSG_OS == DEFSG_WINDOWS)
#include <windows.h>
#pragma warning( disable : 4996)
#pragma warning( disable : 4312)
#pragma warning( disable : 4311)
#endif



/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#if (DEFSG_IS_HOST_LINUX)
#define MODEMDEVICE "/dev/ttyUSB"
#else
#define MODEMDEVICE "/dev/ttyS"
#endif

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        *************
 ***************     L O C A L         F U N C T I O N S        *************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

static int is_digits_only_l(const char *str)
{
   while (*str)
   {
      if (isdigit(*str++) == 0)
      {
         return 0;
      }
    }

    return 1;
}

#if (DEFSG_OS == DEFSG_WINDOWS)
static ERRG_codeE CDC_SERIALP_readComBlock(PUART *pUartHandle, INT8 *lpszBlock, INT32 nMaxLength , OVERLAPPED *osRead, UINT32 *readLen);

/****************************************************************************
*
*  Function Name: CDC_SERIALP_createX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
static ERRG_codeE CDC_SERIALP_createX(PUART* pUartHandle, char* pPortName)
{
   // open COMM device
   char        szPort[15];
   int         i = 0;

   if (strlen(pPortName) == 0)
   {
      printf("Port Name can't be empty. Aborting.\n");
      return SERIAL__ERR_GENERAL_ERR;
   }
   else if (is_digits_only_l(pPortName))
   {
      sprintf(szPort, "\\\\.\\COM%s", pPortName);
   }
   else
   {
      sprintf(szPort, "\\\\.\\%s", pPortName);
   }

   if (!pUartHandle)
   {
       return SERIAL__ERR_NULL_PTR;
   }

#if DEFSG_IS_GP
   for (i = 0; i < 20; i++)
   {
       pUartHandle->uartHandle = CreateFile( szPort,
                                             GENERIC_READ | GENERIC_WRITE,
                                             0,                // exclusive access
                                             NULL,              // no security attrs
                                             OPEN_EXISTING,
                                             FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // overlapped I/O
                                             NULL);

      if (pUartHandle->uartHandle == INVALID_HANDLE_VALUE)
      {
         Sleep(100);
      }
      else
      {
         return SERIAL__RET_SUCCESS;
      }
   }

   // failed to create Serial port.
   return SERIAL__ERR_OPEN_FAIL_INVALID_HANDLE;
#else
   pUartHandle->uartHandle = HANDLE(atoi(pPortName));

   return SERIAL__RET_SUCCESS;
#endif
}

/****************************************************************************
*
*  Function Name: CDC_SERIALP_openX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
static ERRG_codeE CDC_SERIALP_openX(PUART *pUartHandle, char *pPortName, UINT32 baudRate, SERIALG_serialTypeE serialType)
{
   // open COMM device
   ERRG_codeE     retVal = SERIAL__RET_SUCCESS;
   DCB            dcb;
   COMMTIMEOUTS   CommTimeOuts;
   FIX_UNUSED_PARAM_WARN(serialType);

   retVal = CDC_SERIALP_createX(pUartHandle, pPortName);
   if (ERRG_FAILED(retVal))
   {
      printf("Couldn't open COM%s due to: 0x%x. Aborting.\n", pPortName, retVal);
      return retVal;
   }

   // get any early notifications
   SetCommMask(pUartHandle->uartHandle, EV_RXCHAR);

   // setup device buffers
   SetupComm(pUartHandle->uartHandle, 4096, 10*1024);    // Make room to tx a bit more than the 8K boot

   // purge any information in the buffer
   PurgeComm(pUartHandle->uartHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

   // set up for overlapped I/O

   CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
   CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
   CommTimeOuts.ReadTotalTimeoutConstant = 1000;
   CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
   CommTimeOuts.WriteTotalTimeoutConstant = 1000;
   SetCommTimeouts( pUartHandle->uartHandle, &CommTimeOuts );

   dcb.DCBlength = sizeof(DCB);
   GetCommState( pUartHandle->uartHandle, &dcb);

   dcb.BaudRate = baudRate;
   dcb.ByteSize = 8;
   dcb.Parity = NOPARITY;
   dcb.StopBits = ONESTOPBIT;

   // setup hardware flow control
   dcb.fOutxDsrFlow = 0;
   dcb.fDtrControl = DTR_CONTROL_ENABLE;

   dcb.fOutxCtsFlow = 0;
   dcb.fRtsControl = RTS_CONTROL_ENABLE;

   // setup software flow control
   dcb.fInX = dcb.fOutX = 0;

   // other various settings
   dcb.fBinary = TRUE;
   dcb.fParity = FALSE;
   dcb.fOutxCtsFlow = FALSE;
   dcb.fDsrSensitivity = FALSE;

   SetCommState(pUartHandle->uartHandle, &dcb);
   pUartHandle->osRead.hEvent = CreateEvent(NULL,   // no security
                                            TRUE,   // explicit reset req
                                            FALSE,  // initial event reset
                                            NULL);  // no name

   pUartHandle->osWrite.hEvent = CreateEvent(NULL,   // no security
                                             TRUE,   // explicit reset req
                                             FALSE,  // initial event reset
                                             NULL);  // no name

   pUartHandle->osWrite.Offset = 0;
   pUartHandle->osWrite.OffsetHigh = 0;
   pUartHandle->osRead.Offset = 0;
   pUartHandle->osRead.OffsetHigh = 0;
   return retVal;
}

/****************************************************************************
*
*  Function Name: CDC_SERIALP_closeX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
static ERRG_codeE CDC_SERIALP_closeX(PUART *pUartHandle)
{
   if (pUartHandle->uartHandle != INVALID_HANDLE_VALUE)
   {
      // disable event notification and wait for thread to halt
      SetCommMask(pUartHandle->uartHandle, 0);

      // purge any outstanding reads/writes and close device handle
      PurgeComm(pUartHandle->uartHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

      CloseHandle(pUartHandle->osRead.hEvent);
      CloseHandle(pUartHandle->osWrite.hEvent);
      CloseHandle(pUartHandle->uartHandle);
      pUartHandle->uartHandle = INVALID_HANDLE_VALUE;

      return UART_COM__RET_SUCCESS;
   }
   return UART_COM__ERR_NULL_PTR;
}

/****************************************************************************
*
*  Function Name: CDC_SERIALP_readComBlock
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
static ERRG_codeE CDC_SERIALP_readComBlock(PUART *pUartHandle, INT8 *lpszBlock, INT32 nMaxLength , OVERLAPPED *osRead, UINT32 *readLen)
{
   BOOL       fReadStat;
   COMSTAT    ComStat;
   DWORD      dwErrorFlags;
   DWORD      dwLength;
   DWORD      dwError;
   ERRG_codeE retVal = SERIAL__RET_SUCCESS;

   if (pUartHandle->uartHandle == INVALID_HANDLE_VALUE)
   {
      return SERIAL__ERR_READ_BLOCK_FAIL_INVALID_HANDLE;
   }

   *readLen = 0;
   // only try to read number of bytes in queue
//   ClearCommError( pUartHandle->uartHandle, &dwErrorFlags, &ComStat);
//   dwLength = min( (UINT32) nMaxLength, ComStat.cbInQue);
   dwLength =nMaxLength;
 //  if (dwLength > 0)
   {
      fReadStat = ReadFile( pUartHandle->uartHandle, lpszBlock, dwLength, &dwLength, osRead ) ;
      if (!fReadStat)
      {
         if (GetLastError() == ERROR_IO_PENDING)
         {
            // We have to wait for read to complete.
            // This function will timeout according to the
            // CommTimeOuts.ReadTotalTimeoutConstant variable
            // Every time it times out, check for port errors
            while (!GetOverlappedResult( pUartHandle->uartHandle, osRead, &dwLength, TRUE ))
            {
               dwError = GetLastError();
               if (dwError == ERROR_IO_INCOMPLETE)
               {
                  //Sleep(1);
                  // normal result if not finished
                  continue;
               }
               else
               {
                  // an error occurred, try to recover
                  ClearCommError( pUartHandle->uartHandle, &dwErrorFlags, &ComStat ) ;
                  retVal = SERIAL__ERR_READ_FAIL;
                  break;
               }
            }
         }
            else
            {
               // some other error occurred
               dwLength = 0 ;
               ClearCommError( pUartHandle->uartHandle, &dwErrorFlags, &ComStat ) ;
               retVal = SERIAL__ERR_READ_FAIL;
            }
        }
    }
   *readLen=dwLength;
   return retVal;
}

/****************************************************************************
*
*  Function Name: CDC_SERIALP_sendX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
static ERRG_codeE CDC_SERIALP_sendX(PUART *pUartHandle, INT8 *lpByte, UINT32 dwBytesToWrite, UINT32 timeout)
{
#define CHUNK_SIZE_0 (1024)
#define CHUNK_SIZE_1 (4)

   ERRG_codeE retVal = SERIAL__RET_SUCCESS;
   BOOL        fWriteStat;
   DWORD       dwBytesWritten;
   DWORD       dwErrorFlags;
   DWORD       dwError;
   COMSTAT     ComStat;

   FIX_UNUSED_PARAM_WARN(timeout);

   if(pUartHandle->uartHandle == INVALID_HANDLE_VALUE)
   {
      return SERIAL__ERR_SEND_FAIL_INVALID_HANDLE;
   }
   //  PurgeComm( pUartHandle->uartHandle , PURGE_RXABORT | PURGE_RXCLEAR);

      fWriteStat = WriteFile(pUartHandle->uartHandle, lpByte, dwBytesToWrite, &dwBytesWritten, &pUartHandle->osWrite);
      //Sleep(0);

      // Note that normally the code will not execute the following
      // because the driver caches write operations. Small I/O requests
      // (up to several thousand bytes) will normally be accepted
      // immediately and WriteFile will return true even though an
      // overlapped operation was specified

      if (!fWriteStat)
      {
         if (GetLastError() == ERROR_IO_PENDING)
         {
            // We should wait for the completion of the write operation
            // so we know if it worked or not

            // This is only one way to do this. It might be beneficial to
            // place the writing operation in a separate thread
            // so that blocking on completion will not negatively
            // affect the responsiveness of the UI

            // If the write takes long enough to complete, this
            // function will timeout according to the
            // CommTimeOuts.WriteTotalTimeoutConstant variable.
            // At that time we can check for errors and then wait
            // some more.

            while (!GetOverlappedResult( pUartHandle->uartHandle,
                                         &pUartHandle->osWrite, &dwBytesWritten, TRUE ))
            {
               dwError = GetLastError();
               if (dwError == ERROR_IO_INCOMPLETE)
               {
                  //Sleep(1);
                  // normal result if not finished
                  continue;
               }
               else
               {
                  // an error occurred, try to recover
                  retVal = UART_COM__ERR_SEND_FAIL;
                  ClearCommError( pUartHandle->uartHandle, &dwErrorFlags, &ComStat ) ;
                  break;
               }
            }
         }
         else
         {
            // some other error occurred
            ClearCommError( pUartHandle->uartHandle, &dwErrorFlags, &ComStat ) ;
            retVal = UART_COM__ERR_SEND_FAIL;
         }
      }

   return retVal;
}

/****************************************************************************
*
*  Function Name: CDC_SERIALP_recvX
*
*  Description:
*  Read the exact amount of data requested from the UART input buffer
*  to the specified buffer
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
static ERRG_codeE CDC_SERIALP_recvX(PUART *pUartHandle, UINT8 *inBuf, UINT32 nBytesToRead, UINT32 *pBytesRead, UINT32 timeout)
{
   ERRG_codeE retVal = SERIAL__RET_SUCCESS;
   UINT32  nBytesRead = 0;
   UINT32  nLength;
   UINT32  m_refTimeTimeInMillisec;
   UINT8 bRlaxSpining = 0;

   *pBytesRead = 0;
   if (pUartHandle->uartHandle == INVALID_HANDLE_VALUE)
   {
      return SERIAL__ERR_RECV_FAIL_INVALID_HANDLE;
   }

   m_refTimeTimeInMillisec = GetTickCount();
   while (nBytesRead < nBytesToRead)
   {
      do
      {
         if (bRlaxSpining)
         {
            Sleep(0); // give some slack to the OS (don't busy spin-wait)
         }
         retVal = CDC_SERIALP_readComBlock(pUartHandle, (INT8*)(inBuf + nBytesRead), nBytesToRead - nBytesRead, &pUartHandle->osRead, &nLength);
         nBytesRead +=nLength;

         if (GetTickCount() > (m_refTimeTimeInMillisec + timeout))
         {
            *pBytesRead = nBytesRead;
            return SERIAL__ERR_READ_TIME_OUT;
         }

         //if the time in millisec is greater than 50 give cpu time to other tasks in OS
         if (GetTickCount() - m_refTimeTimeInMillisec > 50)
         {
            bRlaxSpining = 1;
         }
         else
         {
            bRlaxSpining = 0;
         }
      } while ((nLength > 0) && (nBytesToRead > nBytesRead));
   }
   *pBytesRead = nBytesRead;
   return retVal;
}
#endif



#if ((DEFSG_PROCESSOR == DEFSG_GP) || DEFSG_GP_HOST || DEFSG_IS_HOST_ANDROID || DEFSG_IS_OS_LINUX)

#ifdef UART_CRC16
#define MAX_TRANSACTION_SIZE_BYTE   1024
#else
#define MAX_TRANSACTION_SIZE_BYTE   255
#endif

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/* change this definition for the correct port */
#define UART_FLAG       0

/****************************************************************************
*
*  Function Name: CDC_SERIALP_openX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
static int set_interface_attribs_l (int fd, int speed, int parity)
{
   struct termios tty;
   memset (&tty, 0, sizeof tty);
   if (tcgetattr (fd, &tty) != 0)
   {
      return -1;
   }

   cfsetospeed (&tty, speed);
   cfsetispeed (&tty, speed);

   tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;    // 8-bit chars
   // disable IGNBRK for mismatched speed tests; otherwise receive break
   // as \000 chars
   tty.c_iflag &= ~IGNBRK;       // disable break processing
   tty.c_lflag = 0;            // no signaling chars, no echo,
                           // no canonical processing
   tty.c_oflag = 0;            // no remapping, no delays
   tty.c_cc[VMIN]   = 0;         // read doesn't block
   tty.c_cc[VTIME] = 5;         // 0.5 seconds read timeout

   tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

   tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                           // enable reading
   tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
   tty.c_cflag |= parity;
   tty.c_cflag &= ~CSTOPB;
   tty.c_cflag &= ~CRTSCTS;

   if (tcsetattr (fd, TCSANOW, &tty) != 0)
   {
      return -1;
   }
   return 0;
}

static int set_blocking_l (int fd, int should_block)
{
   struct termios tty;
   memset (&tty, 0, sizeof tty);
   if (tcgetattr (fd, &tty) != 0)
   {
      return -1;
   }
   tty.c_cc[VMIN]   = should_block ? 1 : 0;
   tty.c_cc[VTIME] = 5;         // 0.5 seconds read timeout

   if (tcsetattr (fd, TCSANOW, &tty) != 0)
   {
      return -1;
   }
   return 0;
}

static ERRG_codeE CDC_SERIALP_openX(PUART *pUartHandle, char *pPortName, UINT32 baudRate, SERIALG_serialTypeE serialType)
{
   // open COMM device
   ERRG_codeE     retVal = SERIAL__RET_SUCCESS;
   struct termios oldtio,newtio;
   speed_t speed;
#if (DEFSG_PROCESSOR == DEFSG_GP)
   int changeToMipiClk = 0;
#endif
   char deviceName[MAX_UART_PORT_NAME_LEN] = {0};
   char portNumStr[3] = {0};

   switch (baudRate)
   {
      case 9600:
         speed = B9600;
         break;
      case 19200:
         speed = B19200;
         break;
      case 38400:
         speed = B38400;
         break;
      case 115200:
         speed = B115200;
#if (DEFSG_PROCESSOR == DEFSG_GP)
         changeToMipiClk = 0;
#endif
         break;
      case 230400:
         speed = B230400;
#if (DEFSG_PROCESSOR == DEFSG_GP)
         changeToMipiClk = 1;
#endif
         break;
      case 460800:
         speed = B460800;
#if (DEFSG_PROCESSOR == DEFSG_GP)
         changeToMipiClk = 1;
#endif
         break;
      case 921600:
         speed = B921600;
#if (DEFSG_PROCESSOR == DEFSG_GP)
         changeToMipiClk = 1;
#endif
         break;
      default:
         speed = B115200;
#if (DEFSG_PROCESSOR == DEFSG_GP)
         changeToMipiClk = 0;
#endif
         break;
   }

   pUartHandle->serialType = serialType;

   /*
      Open modem device for reading and writing and not as controlling tty
      because we don't want to get killed if linenoise sends CTRL-C.
   */

   if (strlen(pPortName) == 0)
   {
      printf("Port Name can't be empty. Aborting.\n");
      return SERIAL__ERR_GENERAL_ERR;
   }
   else if (is_digits_only_l(pPortName))
   {
      sprintf(deviceName, "%s%s", MODEMDEVICE, pPortName);
   }
   else
   {
      sprintf(deviceName, "%s", pPortName);
   }

   int fd = (INT32)open(deviceName, O_RDWR | UART_FLAG );
   if (fd<0)
   {
      printf("Open failed %s\n",deviceName);
      return SERIAL__ERR_OPEN_FAIL_INVALID_HANDLE;
   }
   pUartHandle->uartHandle = (UINT32)fd;

#if (DEFSG_PROCESSOR == DEFSG_GP)
      if((serialType == SERIALG_TYPE_UART) && changeToMipiClk)
      {
         retVal = GME_MNGRG_setUART0ClkSrc();

         if(ERRG_FAILED(retVal))
         {
            return retVal;
         }
      }
#endif

   tcgetattr(pUartHandle->uartHandle,&oldtio); /* save current serial port settings */
   bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
   /*
     BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
     CRTSCTS : output hardware flow control (only used if the cable has
               all necessary lines. See sect. 7 of Serial-HOWTO)
     CS8     : 8n1 (8bit,no parity,1 stopbit)
     CLOCAL  : local connection, no modem contol
     CREAD    : enable receiving characters
   */

   cfsetispeed (&newtio, speed);
   cfsetospeed (&newtio, speed);

   newtio.c_cflag = (CS8 | CLOCAL | CREAD);

   /*
   IGNPAR   : ignore bytes with parity errors
   ICRNL    : map CR to NL (otherwise a CR input on the other computer
   will not terminate input)
   otherwise make device raw (no other input processing)
   */
   //newtio.c_iflag = IGNPAR | ICRNL;

   /*
   Raw output.
   */
   //newtio.c_oflag = 0;
   newtio.c_oflag &= ~(OPOST | ONLCR);

   newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOCTL | ECHOKE | IEXTEN | ICRNL | ECHOK);

   /*
   initialize all control characters
   default values can be found in /usr/include/termios.h, and are given
   in the comments, but we don't need them here
   */
   newtio.c_cc[VINTR]      = 0;  /* Ctrl-c */
   newtio.c_cc[VQUIT]      = 0;  /* Ctrl-\ */
   newtio.c_cc[VERASE]     = 0;  /* del */
   newtio.c_cc[VKILL]      = 0;  /* @ */
   newtio.c_cc[VEOF]       = 4;  /* Ctrl-d */
   newtio.c_cc[VTIME]      = 0;  /* inter-character timer unused */
   newtio.c_cc[VMIN]       = 1;  /* blocking read until 1 character arrives */
   newtio.c_cc[VSWTC]      = 0;  /* '\0' */
   newtio.c_cc[VSTART]     = 0;  /* Ctrl-q */
   newtio.c_cc[VSTOP]      = 0;  /* Ctrl-s */
   newtio.c_cc[VSUSP]      = 0;  /* Ctrl-z */
   newtio.c_cc[VEOL]       = 0;  /* '\0' */
   newtio.c_cc[VREPRINT]   = 0;  /* Ctrl-r */
   newtio.c_cc[VDISCARD]   = 0;  /* Ctrl-u */
   newtio.c_cc[VWERASE]    = 0;  /* Ctrl-w */
   newtio.c_cc[VLNEXT]     = 0;  /* Ctrl-v */
   newtio.c_cc[VEOL2]      = 0;  /* '\0' */

   /*
   now clean the modem line and activate the settings for the port
   */
   tcflush(pUartHandle->uartHandle, TCIFLUSH);
   tcsetattr(pUartHandle->uartHandle,TCSANOW,&newtio);

   set_interface_attribs_l (pUartHandle->uartHandle, speed, 0);
   set_blocking_l (pUartHandle->uartHandle, 0);

   return retVal;
}

/****************************************************************************
*
*  Function Name: CDC_SERIALP_closeX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
static ERRG_codeE CDC_SERIALP_closeX(PUART *pUartHandle)
{
   ERRG_codeE     retVal = SERIAL__RET_SUCCESS;

   if ((INT32)pUartHandle->uartHandle > 0)
   {
      close((INT32)pUartHandle->uartHandle);
      pUartHandle->uartHandle = -1;
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: CDC_SERIALP_sendX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
static ERRG_codeE CDC_SERIALP_sendX(PUART *pUartHandle,INT8 *lpByte, UINT32 dwBytesToWrite, UINT32 timeout)
{
   ERRG_codeE retVal = SERIAL__RET_SUCCESS;
   UINT32 chunkLen;
   INT32 retLen=0;
   FIX_UNUSED_PARAM_WARN(timeout);
   FIX_UNUSED_PARAM_WARN(retLen);
   if ((INT32)pUartHandle->uartHandle < 0)
   {
      return SERIAL__ERR_SEND_FAIL_INVALID_HANDLE;
   }
   while (dwBytesToWrite)
   {
      chunkLen = MIN(dwBytesToWrite , MAX_TRANSACTION_SIZE_BYTE);
      retLen = write ((INT32)pUartHandle->uartHandle,lpByte,chunkLen);
      lpByte+=chunkLen;
      dwBytesToWrite-=chunkLen;
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: CDC_SERIALP_recvX
*
*  Description:
*  Read the exact amount of data requested from the UART input buffer
*  to the specified buffer
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
static ERRG_codeE CDC_SERIALP_recvX(PUART *pUartHandle, UINT8 *inBuf, UINT32 nBytesToRead, UINT32 *pBytesRead, UINT32 timeout)
{
   ERRG_codeE retVal = SERIAL__RET_SUCCESS;
   UINT32  nBytesRead = 0;
   UINT32  nLength = 0;
   UINT32  m_refTimeTimeInMillisec;
   UINT8 bRlaxSpining = 0;
   UINT32 currentTime=0;
   *pBytesRead = 0;
   if ((INT32)pUartHandle->uartHandle < 0)
   {
      return SERIAL__ERR_RECV_FAIL_INVALID_HANDLE;
   }

//#if ((DEFSG_PROCESSOR == DEFSG_GP))
//   printf("CDC_SERIALP_recvX: Start receive -  bytes to Read %d\n", nBytesToRead);
//#endif

   OS_LYRG_getMsecTime(&m_refTimeTimeInMillisec);
   while (nBytesRead < nBytesToRead)
   {
#ifndef UART_CRC16
      do
      {
         if (bRlaxSpining)
         {
            OS_LYRG_usleep(0); // give some slack to the OS (don't busy spin-wait)
         }
#endif
         nLength= read (pUartHandle->uartHandle,  (UINT8*)(inBuf + nBytesRead), nBytesToRead - nBytesRead);
         nBytesRead +=nLength;

      //  printf("CDC_SERIALP_recvX 3: InBuff 0x%x, length %d, total read %d, err %s \n", (UINT8*)(inBuf + nBytesRead), nLength, nBytesRead, strerror(errno));
#ifndef UART_CRC16
         OS_LYRG_getMsecTime(&currentTime);
         if ( currentTime > (m_refTimeTimeInMillisec + timeout) )
         {
            *pBytesRead = nBytesRead;
#if ((DEFSG_PROCESSOR == DEFSG_GP))
            //printf("CDC_SERIALP_recvX: InBuff %p, bytes to Read %d , bytes read %d, length %d, timeout = %d, currentTime = %d, m_refTimeTimeInMillisec = %d \n", inBuf, nBytesToRead, nBytesRead, nLength, timeout, currentTime, m_refTimeTimeInMillisec);
            if (pUartHandle->serialType == SERIALG_TYPE_UART || (nBytesToRead != nBytesRead))
#endif
            {
               return SERIAL__ERR_READ_TIME_OUT;
            }
         }
         if(nLength == 0)
         {
            // printf("CDC_SERIALP_recvX 2: bytes to Read %d , bytes read %d, length %d, timeout %d currentTime %d\n", nBytesToRead, nBytesRead, nLength, (m_refTimeTimeInMillisec + timeout), currentTime);
         }
         //if the time in millisec is greater than 50 give cpu time to other tasks in OS
         if (currentTime - m_refTimeTimeInMillisec > 50)
         {
            bRlaxSpining = 1;
         }
         else
         {
            bRlaxSpining = 0;
         }
      } while ((nLength > 0) && (nBytesToRead > nBytesRead));
#endif
   }
// #if ((DEFSG_PROCESSOR == DEFSG_GP))
//             printf("CDC_SERIALP_recvX: Done receive -  bytes to Read %d, bytes read so far %d, length %d \n", nBytesToRead, nBytesRead, nLength);
// #endif
   *pBytesRead = nBytesRead;
   return retVal;
}
#endif
/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: CDC_SERIALG_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
ERRG_codeE CDC_SERIALG_close(PUART *uartHandle)
{
   ERRG_codeE retVal = SERIAL__RET_SUCCESS;
   retVal = CDC_SERIALP_closeX(uartHandle);
   return retVal;
}

/****************************************************************************
*
*  Function Name: CDC_SERIALG_recv
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
ERRG_codeE CDC_SERIALG_recv(PUART *uartHandle, UINT8 *inBuf, UINT32 nBytesToRead, UINT32 *pBytesRead, UINT32 timeout )
{
   return CDC_SERIALP_recvX(uartHandle, inBuf, nBytesToRead, pBytesRead, timeout);
}

/****************************************************************************
*
*  Function Name: CDC_SERIALG_send
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
ERRG_codeE CDC_SERIALG_send(PUART *uartHandle, INT8 *lpByte, UINT32 dwBytesToWrite, UINT32 timeout)
{
   return CDC_SERIALP_sendX(uartHandle, lpByte, dwBytesToWrite, timeout);
}

/****************************************************************************
*
*  Function Name: CDC_SERIALG_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: serial infrastructure
*
****************************************************************************/
ERRG_codeE CDC_SERIALG_open(PUART *pUartHandle, char *pPortName, UINT32 baudRate, SERIALG_serialTypeE serialType)
{
   return CDC_SERIALP_openX(pUartHandle, pPortName, baudRate, serialType);
}

