#define INUSTORAGE_EXPORTS
#define INUUSB_EXPORTS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>

#include "inu_usb.h"
#include "inu_boot.h"
#include "inu_bootdetect.h"


#include "inu_storage.h"
#include "inu_storage_layout.h"
#include "bootflash_nu3000b0.h"
#include "bootflash_nu3000a0.h"
#include "bootflash_nu4000a0.h"
#include "bootflash_nu4000b0.h"
#include "bootflash_nu4000c0.h"
#include "bootflash_nu4000c1.h"
#include "bootflash_secondary_nu4000a0.h"
#include "inu_host_utils.h"

#define CRAM_START (0x1000000)
#define CRAM_FREE_OFFSET (0x20000)
#define LRAM_START (0x2000000)
#define LRAM_FREE_OFFSET (0x20000)
#define LRAM_FREE_OFFSET_NO_FLASH (0x30000)
#define LRAM_C0_FREE_OFFSET (0x30000)
#define LRAM_LOADER_ADDR (0x2040000)
#define LRAM_FOR_BOOTFLASH (0x2040000)
#define LRAM_C0_FOR_BOOTFLASH (0x2000000)
#define LRAM_3000_FOR_BOOTFLASH (0x2020000)
#define LRAM_DATA_FOR_BOOTFLASH (0x201ff00)
#define LRAM_C0_DATA_FOR_BOOTFLASH (0x202ff00)
#define MAX_CHUNK  (0x10000)
#define MAX_SPI_CHUNK  (0x4000)
#define EFUSE_SIZE (0x200)

//Note: these regs must not be used by bootrom.
#define GME_SAVE_AND_RESTORE0_ADDR (0x8020104)
#define GME_SAVE_AND_RESTORE1_ADDR (0x8020108)
#define GME_SAVE_AND_RESTORE2_ADDR (0x802010C)
#define GME_SAVE_AND_RESTORE3_ADDR (0x8020110)
#define GME_SAVE_AND_RESTORE4_ADDR (0x8020114)
#define GME_SAVE_AND_RESTORE5_ADDR (0x8020118)
#define GME_SAVE_AND_RESTORE6_ADDR (0x802011C)
#define GME_SAVE_AND_RESTORE7_ADDR (0x8020120)
#define GME_CHIP_VERSION           (0x80200B4)

#define BOOTFLASH_STATUS_READY      (0)
#define BOOTFLASH_STATUS_ACTIVE     (1)
#define BOOTFLASH_STATUS_FINISH     (2)
#define BOOTFLASH_STATUS_UNKNOWN    (0xFF)

#define BOOTFLASH_COMMAND_READ			(1)
#define BOOTFLASH_COMMAND_WRITE			(2)
#define BOOTFLASH_COMMAND_INFO			(3)
#define BOOTFLASH_COMMAND_ERASE			(4)
#define BOOTFLASH_COMMAND_NAND_READ		(5)
#define BOOTFLASH_COMMAND_NAND_WRITE	(6)
#define BOOTFLASH_COMMAND_NAND_ERASE	(7)

#define BOOTFLASH_STORAGE_SPIFLASH	(0)
#define BOOTFLASH_STORAGE_EEPROM	(1)
#define BOOTFLASH_STORAGE_EFUSE		(2)
#define BOOTFLASH_STORAGE_NAND		(3)

// status: BOOTFLASH_STATUS_READY/BOOTFLASH_STATUS_ACTIVE
#define BOOTFLASH_STATUS_REG              (GME_SAVE_AND_RESTORE0_ADDR)
// access command: BOOTFLASH_COMMAND_READ/BOOTFLASH_COMMAND_WRITE
#define BOOTFLASH_COMMAND_REG             (GME_SAVE_AND_RESTORE1_ADDR)
// storage type: BOOTFLASH_STORAGE_SPIFLASH / BOOTFLASH_STORAGE_EEPROM/ BOOTFLASH_STORAGE_EFUSE
#define BOOTFLASH_STORAGE_TYPE_REG        (GME_SAVE_AND_RESTORE2_ADDR)
// offset on storage
#define BOOTFLASH_STORAGE_OFFSET_REG      (GME_SAVE_AND_RESTORE3_ADDR)
// data buffer address
#define BOOTFLASH_STORAGE_DATA_ADDR_REG   (GME_SAVE_AND_RESTORE4_ADDR)
// data buffer length
#define BOOTFLASH_STORAGE_DATA_SIZE_REG   (GME_SAVE_AND_RESTORE5_ADDR)
// command return value
#define BOOTFLASH_COMMAND_RETURN_REG      (GME_SAVE_AND_RESTORE6_ADDR)

#define BOOTFLASH_RX_SIZE_REG             (GME_SAVE_AND_RESTORE7_ADDR)

#define BOOTFIX_ALIGN_SIZE 1024

#define MAX_CALIB_SIZE 0x200000

typedef enum
{
	INU_STORAGE_SPI_READ = 0,
	INU_STORAGE_SPI_INFO = 1,
} InuStorageReadInfoE;

typedef struct
{
	unsigned int blockSize;
	unsigned int numOfBlocks;
}TFlashInfo;

unsigned int chipVersion = NU4000_B0_CHIP_VERSION;
unsigned int overwriteChipVersion = 0; // running an board A and burning board B with this version
unsigned int boardChipId=0; // Can't read correct chipId from chip this is the chip ID 

int inu_storage_ready = 0;
unsigned int secondary_flash = 0;
unsigned int storageMagicNumber[INU_STORAGE_SECTION_APP + 1] = {

   SECTION_BOOTFIX_MAGIC_NUMBER,
   SECTION_BOOTFIX_META_MAGIC_NUMBER,
   SECTION_PRODUCTION_MAGIC_NUMBER,
   SECTION_CALIBRATION_MAGIC_NUMBER,
   SECTION_DYN_CALIBRATION_MAGIC_NUMBER,
   SECTION_BOOTSPL_MAGIC_NUMBER,
   SECTION_DTB_MAGIC_NUMBER,
   SECTION_KERNEL_MAGIC_NUMBER,
   SECTION_ROOTFS_MAGIC_NUMBER,
   SECTION_CEVA_MAGIC_NUMBER,
   SECTION_CNN_LD_MAGIC_NUMBER,
   SECTION_CNN_MAGIC_NUMBER,
   SECTION_APP_MAGIC_NUMBER
};

unsigned int storageAutoInc[INU_STORAGE_SECTION_APP + 1] = {

	SECTION_BOOTFIX_AUTO_SIZE_INCREASE,
	SECTION_BOOTFIX_META_AUTO_SIZE_INCREASE,
	SECTION_PRODUCTION_AUTO_SIZE_INCREASE,
	SECTION_CALIBRATION_AUTO_SIZE_INCREASE,
	SECTION_DYN_CALIBRATION_AUTO_SIZE_INCREASE,
	SECTION_BOOTSPL_AUTO_SIZE_INCREASE,
	SECTION_DTB_AUTO_SIZE_INCREASE,
    SECTION_KERNEL_AUTO_SIZE_INCREASE,
    SECTION_ROOTFS_AUTO_SIZE_INCREASE,
    SECTION_CEVA_AUTO_SIZE_INCREASE,
    SECTION_CNN_LD_AUTO_SIZE_INCREASE,
    SECTION_CNN_AUTO_SIZE_INCREASE,
	SECTION_APP_AUTO_SIZE_INCREASE
};

unsigned int storageMustHaveComponent[INU_STORAGE_SECTION_APP + 1] = {
   1,     //   INU_STORAGE_SECTION_BOOTFIX             =  0,
   1,     //   INU_STORAGE_SECTION_BOOTFIX_METADATA    =  1,   
   1,     //   INU_STORAGE_SECTION_PRODUCTION_DATA     =  2,
   1,     //   INU_STORAGE_SECTION_CALIBRATION_DATA    =  3,
   0,     //   INU_STORAGE_SECTION_DTNAMIC_CALIBRATION =  4,
   1,     //   INU_STORAGE_SECTION_BOOTSPL             =  5,
   1,     //   INU_STORAGE_SECTION_DTB                 =  6,
   1,     //   INU_STORAGE_SECTION_KERNEL              =  7,
   1,     //   INU_STORAGE_SECTION_ROOTFS              =  8,
   0,     //   INU_STORAGE_SECTION_CEVA                =  9,
   0,     //   INU_STORAGE_SECTION_CNN_LD              =  10,
   0,     //   INU_STORAGE_SECTION_CNN                 =  11,
   0      //   INU_STORAGE_SECTION_APP                 =  12
};

char *storageFileName[INU_STORAGE_SECTION_APP + 1] = {

   "BURN_SECTION_BOOTFIX",
   "BURN_SECTION_BOOTFIX_META",
   "BURN_SECTION_PRODUCTION",
   "BURN_SECTION_CALIBRATION",
   "BURN_SECTION_DYNAMIC_CALIBRATION",
   "BURN_SECTION_BOOTSPL",
   "BURN_SECTION_DTB",
   "BURN_SECTION_KERNEL",
   "BURN_SECTION_ROOTFS",
   "BURN_SECTION_CEVA",
   "BURN_SECTION_CNN_LD",
   "BURN_SECTION_CNN",
   "BURN_SECTION_APP"
};

int storageSectorAddr[INU_STORAGE_SECTION_APP + 1]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

TFlashInfo flashInfo = { 64 * 1024,128 };
static unsigned int dryRunMode =0;
static char dryRunOutputDir[BOOT_PATH_LENGTH_MAX]={0};
static InuStorageErrorE storage_read_production_data(InuProductionDataT *productionData, unsigned int offset, unsigned int magic_number);
static InuStorageErrorE inu_storage_read_info(InuStorageTypeE storage_type, unsigned int offset, unsigned char *buffer, unsigned int size, unsigned int *transfered_size);
static InuStorageErrorE inu_storage_write_dry_run_hdr(InuStorageSectionTypeE sectionType,unsigned int hdrAddress,InuSectionHeaderT *sectionHdr);
static InuStorageErrorE inu_storage_write_dry_run_data(InuStorageSectionTypeE sectionType,unsigned int address, unsigned char *data,unsigned int size);
//#define _DEBUG
/*
Protocol description:
Host:
on each data chunk:
Wait for target ready (GME0 == 0)
Configure command (GME1,GME2,GME3,GME4,GME5)
Activate target to pending for commands (GME0 = 1)
Configure target to stop pending for commands (GME0 == 0)

Target:
Target ready (GME0 == 0)
Wait for host request ((GME0 == 1))
start loop:
Perform commands and transfer the data
stop loop:
Wait for host command (GME0 = 0)
*/
InuUsbParamsT inuParams;
SpiParamsT spiGParams = { 0 };

#define STORAGE_PRINT(logErr, ...) storage_print((InuUsbLogLevelE)logErr, __FILE__, __func__, __LINE__, __VA_ARGS__)
void storage_print(InuUsbLogLevelE logErr, const char *filename, const char *func, unsigned int line, const char * fmt, ...)

{
	char fmtStr[300];
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();
	va_list  args;
	va_start(args, fmt);
	vsprintf(fmtStr, fmt, args);
	va_end(args);
	inuUsbParams->log(logErr,"%s %s %d %s",filename,func,line,fmtStr);
}

enum {
   FIO_READ,
   FIO_WRITE
};

struct fioparms {
   int    op;
   char * mode;
   char * iodesc;
   char * dir;
   char * rw;
   unsigned int fileoffset;
};

typedef struct cmdbit {
   int          type;  /* AVR_CMDBIT_* */
   int          bitno; /* which input bit to use for this command bit */
   int          value; /* bit value if type == AVR_CMDBIT_VALUD */
} CMDBIT;

typedef struct opcode {
   CMDBIT        bit[32]; /* opcode bit specs */
} OPCODE;
#define AVR_MEMDESCLEN 64
#define progname "flash_tool"
#define IHEX_MAXDATA 256

#define MAX_LINE_LEN 256  /* max line length for ASCII format input files */
#define TAG_ALLOCATED          1    /* memory byte is allocated */


/*
* AVR serial programming instructions
*/
enum {
   AVR_OP_READ,
   AVR_OP_WRITE,
   AVR_OP_READ_LO,
   AVR_OP_READ_HI,
   AVR_OP_WRITE_LO,
   AVR_OP_WRITE_HI,
   AVR_OP_LOADPAGE_LO,
   AVR_OP_LOADPAGE_HI,
   AVR_OP_LOAD_EXT_ADDR,
   AVR_OP_WRITEPAGE,
   AVR_OP_CHIP_ERASE,
   AVR_OP_PGM_ENABLE,
   AVR_OP_MAX
};

typedef struct avrmem {
   char desc[AVR_MEMDESCLEN];  /* memory description ("flash", "eeprom", etc) */
   int paged;                  /* page addressed (e.g. ATmega flash) */
   int size;                   /* total memory size in bytes */
   int page_size;              /* size of memory page (if page addressed) */
   int num_pages;              /* number of pages (if page addressed) */
   unsigned int offset;        /* offset in IO memory (ATxmega) */
   int min_write_delay;        /* microseconds */
   int max_write_delay;        /* microseconds */
   int pwroff_after_write;     /* after this memory type is written to,
                        the device must be powered off and
                        back on, see errata
                        http://www.atmel.com/dyn/resources/prod_documents/doc1280.pdf */
   unsigned char readback[2];  /* polled read-back values */

   int mode;                   /* stk500 v2 xml file parameter */
   int delay;                  /* stk500 v2 xml file parameter */
   int blocksize;              /* stk500 v2 xml file parameter */
   int readsize;               /* stk500 v2 xml file parameter */
   int pollindex;              /* stk500 v2 xml file parameter */

   unsigned char * buf;        /* pointer to memory buffer */
   unsigned char * tags;       /* allocation tags */
   OPCODE * op[AVR_OP_MAX];    /* opcodes */
} AVRMEM;


struct ihexrec {
   unsigned char    reclen;
   unsigned int     loadofs;
   unsigned char    rectyp;
   unsigned char    data[IHEX_MAXDATA];
   unsigned char    cksum;
};

static int ihex_readrec(struct ihexrec * ihex, char * rec)
{
   int i, j;
   char buf[8];
   int offset, len;
   char * e;
   unsigned char cksum;
   int rc;

   len = strlen(rec);
   offset = 1;
   cksum = 0;

   /* reclen */
   if (offset + 2 > len)
      return -1;
   for (i = 0; i<2; i++)
      buf[i] = rec[offset++];
   buf[i] = 0;
   ihex->reclen = strtoul(buf, &e, 16);
   if (e == buf || *e != 0)
      return -1;

   /* load offset */
   if (offset + 4 > len)
      return -1;
   for (i = 0; i<4; i++)
      buf[i] = rec[offset++];
   buf[i] = 0;
   ihex->loadofs = strtoul(buf, &e, 16);
   if (e == buf || *e != 0)
      return -1;

   /* record type */
   if (offset + 2 > len)
      return -1;
   for (i = 0; i<2; i++)
      buf[i] = rec[offset++];
   buf[i] = 0;
   ihex->rectyp = strtoul(buf, &e, 16);
   if (e == buf || *e != 0)
      return -1;

   cksum = ihex->reclen + ((ihex->loadofs >> 8) & 0x0ff) +
      (ihex->loadofs & 0x0ff) + ihex->rectyp;

   /* data */
   for (j = 0; j<ihex->reclen; j++) {
      if (offset + 2 > len)
         return -1;
      for (i = 0; i<2; i++)
         buf[i] = rec[offset++];
      buf[i] = 0;
      ihex->data[j] = strtoul(buf, &e, 16);
      if (e == buf || *e != 0)
         return -1;
      cksum += ihex->data[j];
   }

   /* cksum */
   if (offset + 2 > len)
      return -1;
   for (i = 0; i<2; i++)
      buf[i] = rec[offset++];
   buf[i] = 0;
   ihex->cksum = strtoul(buf, &e, 16);
   if (e == buf || *e != 0)
      return -1;

   rc = -cksum & 0x000000ff;

   return rc;
}

unsigned int sectionBootsplStartBlock = SECTION_BOOTSPL_START_BLOCK;

/*
* Intel Hex to binary buffer
*
* Given an open file 'inf' which contains Intel Hex formated data,
* parse the file and lay it out within the memory buffer pointed to
* by outbuf.  The size of outbuf, 'bufsize' is honored; if data would
* fall outsize of the memory buffer outbuf, an error is generated.
*
* Return the maximum memory address within 'outbuf' that was written.
* If an error occurs, return -1.
*
* */
static int ihex2b(char * infile, FILE * inf,
   AVRMEM * mem, int bufsize, unsigned int fileoffset)
{
   char buffer[MAX_LINE_LEN];
   unsigned int nextaddr, baseaddr, maxaddr;
   int i;
   int lineno;
   int len;
   struct ihexrec ihex;
   int rc;

   lineno = 0;
   baseaddr = 0;
   maxaddr = 0;
   nextaddr = 0;

   while (fgets((char *)buffer, MAX_LINE_LEN, inf) != NULL) {
      lineno++;
      len = strlen(buffer);
      if (buffer[len - 1] == '\n')
         buffer[--len] = 0;
      if (buffer[0] != ':')
         continue;
      rc = ihex_readrec(&ihex, buffer);
      if (rc < 0) {
         fprintf(stderr, "%s: invalid record at line %d of \"%s\"\n",
            progname, lineno, infile);
         return -1;
      }
      else if (rc != ihex.cksum) {
         fprintf(stderr, "%s: ERROR: checksum mismatch at line %d of \"%s\"\n",
            progname, lineno, infile);
         fprintf(stderr, "%s: checksum=0x%02x, computed checksum=0x%02x\n",
            progname, ihex.cksum, rc);
         return -1;
      }

      switch (ihex.rectyp) {
      case 0: /* data record */
         if (fileoffset != 0 && baseaddr < fileoffset) {
            fprintf(stderr,
               "%s: ERROR: address 0x%04x out of range (below fileoffset 0x%x) at line %d of %s\n",
               progname, baseaddr, fileoffset, lineno, infile);
            return -1;
         }
         nextaddr = ihex.loadofs + baseaddr - fileoffset;
         if (nextaddr + ihex.reclen > bufsize) {
            fprintf(stderr,
               "%s: ERROR: address 0x%04x out of range at line %d of %s\n",
               progname, nextaddr + ihex.reclen, lineno, infile);
            return -1;
         }
         for (i = 0; i<ihex.reclen; i++) {
            mem->buf[nextaddr + i] = ihex.data[i];
            mem->tags[nextaddr + i] = TAG_ALLOCATED;
         }
         if (nextaddr + ihex.reclen > maxaddr)
            maxaddr = nextaddr + ihex.reclen;
         break;

      case 1: /* end of file record */
         return maxaddr;
         break;

      case 2: /* extended segment address record */
         baseaddr = (ihex.data[0] << 8 | ihex.data[1]) << 4;
         break;

      case 3: /* start segment address record */
            /* we don't do anything with the start address */
         break;

      case 4: /* extended linear address record */
         baseaddr = (ihex.data[0] << 8 | ihex.data[1]) << 16;
         break;

      case 5: /* start linear address record */
            /* we don't do anything with the start address */
         break;

      default:
         fprintf(stderr,
            "%s: don't know how to deal with rectype=%d "
            "at line %d of %s\n",
            progname, ihex.rectyp, lineno, infile);
         return -1;
         break;
      }

   } /* while */

   if (maxaddr == 0) {
      fprintf(stderr,
         "%s: ERROR: No valid record found in Intel Hex "
         "file \"%s\"\n",
         progname, infile);

      return -1;
   }
   else {
      fprintf(stderr,
         "%s: WARNING: no end of file record found for Intel Hex "
         "file \"%s\"\n",
         progname, infile);

      return maxaddr;
   }
}

static int fileio_ihex(struct fioparms * fio,
   char * filename, FILE * f, AVRMEM * mem, int size)
{
   int rc;

   switch (fio->op) {
   case FIO_READ:
      rc = ihex2b(filename, f, mem, size, fio->fileoffset);
      if (rc < 0)
         return -1;
      break;

   default:
      fprintf(stderr, "%s: invalid Intex Hex file I/O operation=%d\n",
         progname, fio->op);
      return -1;
      break;
   }

   return rc;
}


void avr_erase(void)
{
   unsigned char cmd[4],rxBuff[4];
   cmd[0] = 0xac;
   cmd[1] = 0x80;
   cmd[2] = 0;
   cmd[3] = 0;
   inu_storage_rw_spi(INU_STORAGE_SPI,cmd, rxBuff, 4,1);
}


void avr_write_enable(void)
{
   unsigned char cmd[4],rxBuff[4];
   cmd[0] = 0xac;
   cmd[1] = 0x53;
   cmd[2] = 0;
   cmd[3] = 0;
   inu_storage_rw_spi(INU_STORAGE_SPI,cmd, rxBuff, 4,1);
}

void write_avr_file(AVRMEM *mem, unsigned int size)
{
   unsigned int ind,numMsgs;
   unsigned char *spiCmds,*currSpiCmd,*spiRx;
   spiCmds = (unsigned char *)malloc(size*4 + size/16 + 4);
   if (spiCmds == NULL)
      return;
   spiRx = (unsigned char *)malloc(size*4 + size/16 + 4);
   if (spiRx == NULL)
   {
      free(spiCmds);
      return;
   }
   currSpiCmd = spiCmds;
   for (ind = 0; ind < size; ind++)
   {
      if ((ind % 2) == 0)
         currSpiCmd[0] = 0x40;
      else currSpiCmd[0] = 0x48;
      currSpiCmd[1] = 0;
      currSpiCmd[2] = (ind / 2)%0x20;
      currSpiCmd[3] = mem->buf[ind];
      currSpiCmd+=4;
      // check for page
      if (((ind + 1) % 0x40) == 0)
      {
        currSpiCmd[0] = 0x4c;
        currSpiCmd[1] = ((ind + 1) / 2 - 0x20)>>8;
        currSpiCmd[2] = (ind+1)/2 - 0x20;
        currSpiCmd[3] = 0;
        currSpiCmd+=4;
      }
   }
   if ((ind % 0x40) != 0) // last page wasn't send
   {
        ind += 0x40 - (ind % 0x40);
        currSpiCmd[0] = 0x4c;
        currSpiCmd[1] = ((ind + 1) / 2 - 0x20)>>8;
        currSpiCmd[2] = (ind+1)/2 - 0x20;
        currSpiCmd[3] = 0;
        currSpiCmd+=4;
   }
   avr_write_enable();
   numMsgs = (currSpiCmd - spiCmds) / 4;
   inu_storage_rw_spi(INU_STORAGE_SPI,spiCmds, spiRx, 4,numMsgs);
   free(spiCmds);
   free(spiRx);
}

void read_avr_file(unsigned char *spiRx, unsigned int size)
{
   unsigned int ind, numMsgs;
   unsigned char *spiCmds, *currSpiCmd;
   spiCmds = (unsigned char *)malloc(size * 4);
   currSpiCmd = spiCmds;
   if (spiCmds == NULL)
      return;
   for (ind = 0; ind < size; ind++)
   {
      if ((ind % 2) == 0)
         currSpiCmd[0] = 0x20;
      else currSpiCmd[0] = 0x28;
      currSpiCmd[1] = (ind / 2) >> 8;
      currSpiCmd[2] = ind / 2;
      currSpiCmd[3] = 0;
      currSpiCmd += 4;
      // check for page
   }
   numMsgs = (currSpiCmd - spiCmds) / 4;
   inu_storage_rw_spi(INU_STORAGE_SPI,spiCmds, spiRx, 4, numMsgs);
   free(spiCmds);
}

INUSTORAGE_API
InuStorageErrorE inu_storage_burn_avr_file(char *avrFile)
{
   int rc;
   FILE * f;
   struct fioparms fio;
   AVRMEM mem1;
   AVRMEM *mem=&mem1;
   unsigned int size = 20000;
   int ind;
   unsigned char *spiRx;
   InuStorageErrorE retCode = INU_STORAGE_ERR_BURN_ATTINY;

   mem->buf = (unsigned char *)malloc(size);
   if (mem->buf == NULL)
      return retCode;

   mem->tags = (unsigned char *)malloc(size);
   if (mem->tags == NULL)
   {
      free(mem->buf);
      return retCode;
   }

   f = fopen(avrFile, "rb");
   if (f != NULL)
   {
      avr_write_enable();
      (&fio)->op = FIO_READ;
      (&fio)->fileoffset = 0;


      rc = fileio_ihex(&fio, "sdc_mcu", f, mem, size);
      if (rc > 0)
      {
         avr_erase();
         avr_write_enable();
         write_avr_file(mem,rc);
      }
      spiRx = (unsigned char *)malloc(rc * 4);
      read_avr_file(spiRx, rc);
      for (ind = 0; ind < rc; ind++)
      {
         if (spiRx[(ind * 4) + 3] != mem->buf[ind])
         {
            printf("\n***********VERIFACTION FAILED ind %d write %x read %x***************\n\n", ind, mem->buf[ind], spiRx[ind * 4 + 3]);
            break;
         }
      }
	  if (ind == rc)
	  {
		  retCode = INU_STORAGE_ERR_SUCCESS;
		  printf("\n************VERIFACTION SUCCSESS**************\n\n");
	  }
	  free(spiRx);
     fclose(f);
   }
   else printf("can't find avr file");

   free(mem->buf);
   free(mem->tags);

   return retCode;
}


static InuStorageErrorE inu_close_interface()
{
	// close flash loop and enable bootflash loading in the next inu_storage_read_raw
	unsigned int ready = BOOTFLASH_STATUS_FINISH;
	inu_usb_monitor_write(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int));
	inu_storage_ready = 0;
	return INU_STORAGE_ERR_SUCCESS;
}

static InuStorageErrorE inu_storage_init(InuStorageTypeE storage_type)
{
	InuUsbErrorE err;
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();
	unsigned int size, transferSize;
	unsigned int lramCodeAddr = LRAM_FOR_BOOTFLASH;
    unsigned int lramDataAddr=LRAM_DATA_FOR_BOOTFLASH;
	unsigned int  securedOffset = 0, lramLoadAdd, c0Board;
	unsigned int  testC0RomData, testC0RomAddr=0x3de0; // C0 ROM data in this address
	if (inu_storage_ready == 0)
	{
		inu_close_interface();
	    err = inu_usb_monitor_write(BOOTFLASH_STORAGE_TYPE_REG, (unsigned char *)&storage_type, sizeof(unsigned int));
	    if (err != INU_USB_ERR_SUCCESS)
	    {
          STORAGE_PRINT(INU_USB_LOG_ERROR, "Fail writng to USB...\n");
	      return INU_STORAGE_ERR_IO_FAILED;
	    }
		err = inu_usb_monitor_read(GME_CHIP_VERSION, (unsigned char *)&(inuUsbParams->version), 4, &size);
		if (err != INU_USB_ERR_SUCCESS)
		{
			STORAGE_PRINT(INU_USB_LOG_ERROR, "can't read version from the chip, check USB connection...\n");
			return INU_STORAGE_ERR_INIT_FAILED;
		}
		chipVersion = inuUsbParams->version;
		if (boardChipId != 0)
			chipVersion = boardChipId;
	//	inuUsbParams->version = NU4000_C0_CHIP_VERSION;//inuUsbParams->version;
		if ((chipVersion == NU4000_B0_CHIP_VERSION) || (chipVersion == NU4000_C0_CHIP_VERSION))
		{
			err = inu_usb_monitor_read(testC0RomAddr, (unsigned char *)&testC0RomData, sizeof(unsigned int), &transferSize);
			if (err != INU_USB_ERR_SUCCESS)
				STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
			else
			{
				if (testC0RomData == 0xe24dd00c)            // bug in C0 have B0 version name
					chipVersion = NU4000_C0_CHIP_VERSION;
				else if (testC0RomData == 0xe59d3004)  // could be C1 version 
                    chipVersion = NU4000_C1_CHIP_VERSION;
			}

		}
		if (chipVersion == NU3000_B0_CHIP_VERSION)
		{
			lramCodeAddr = LRAM_3000_FOR_BOOTFLASH;
			err = inu_usb_monitor_write(lramCodeAddr, bootflash_nu3000b0, (unsigned int)bootflash_nu3000b0_len);
			if (err != INU_USB_ERR_SUCCESS)
			{
				STORAGE_PRINT(INU_USB_LOG_ERROR, "Fail writng to USB...\n");
				return INU_STORAGE_ERR_INIT_FAILED;
			}
		}
		else if (chipVersion == NU4000_A0_CHIP_VERSION)
		{   
			if (secondary_flash)
			{
				err = inu_usb_monitor_write(lramCodeAddr, bootflash_secondary_nu4000a0, (unsigned int)bootflash_secondary_nu4000a0_len);
			}
			else err = inu_usb_monitor_write(lramCodeAddr, bootflash_nu4000a0, (unsigned int)bootflash_nu4000a0_len);

			if (err != INU_USB_ERR_SUCCESS)
			{
				STORAGE_PRINT(INU_USB_LOG_ERROR, "Fail writng to USB...\n");
				return INU_STORAGE_ERR_INIT_FAILED;
			}
		}
		else if (chipVersion == NU4000_B0_CHIP_VERSION)
		{
            err = inu_usb_monitor_write(lramCodeAddr, bootflash_nu4000b0, (unsigned int)bootflash_nu4000b0_len);

			if (err != INU_USB_ERR_SUCCESS)
			{
				STORAGE_PRINT(INU_USB_LOG_ERROR, "Fail writng to USB...\n");
				return INU_STORAGE_ERR_INIT_FAILED;
			}

		}
		else if ((chipVersion == NU4000_C0_CHIP_VERSION ) || (chipVersion == NU4000_C1_CHIP_VERSION ) || (chipVersion == NU4100_CHIP_VERSION))
		{
			lramCodeAddr = LRAM_C0_FOR_BOOTFLASH;
			lramDataAddr = LRAM_C0_DATA_FOR_BOOTFLASH;
			lramLoadAdd = LRAM_C0_FOR_BOOTFLASH;

			err = inu_usb_monitor_read(GME_SAVE_AND_RESTORE7_ADDR, (unsigned char *)&securedOffset, sizeof(unsigned int),&transferSize);
            if (err != INU_USB_ERR_SUCCESS)
               STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
			if (securedOffset != 0)
			{
				lramLoadAdd = 0x2034000;
				err = inu_usb_monitor_write(GME_SAVE_AND_RESTORE6_ADDR, (unsigned char *)&lramLoadAdd, sizeof(unsigned int));
			}
		    if (err != INU_USB_ERR_SUCCESS)
			    STORAGE_PRINT(INU_USB_LOG_ERROR, "Fail writng ispiDividor to USB...\n");
           
			if ((chipVersion == NU4000_C1_CHIP_VERSION)|| (chipVersion == NU4100_CHIP_VERSION))
				err = inu_usb_monitor_write(lramLoadAdd, bootflash_nu4000c1, (unsigned int)bootflash_nu4000c1_len);
			else err = inu_usb_monitor_write(lramLoadAdd, bootflash_nu4000c0, (unsigned int)bootflash_nu4000c0_len);
			if (err != INU_USB_ERR_SUCCESS)
			{
				STORAGE_PRINT(INU_USB_LOG_ERROR, "Fail writng to USB...\n");
				return INU_STORAGE_ERR_INIT_FAILED;
			}
		    
		}

		err = inu_usb_monitor_write(lramDataAddr, (unsigned char *)&spiGParams, sizeof(spiGParams));
		if (err != INU_USB_ERR_SUCCESS)
			inuUsbParams->log(INU_USB_LOG_ERROR, "inu_storage: Fail writng ispiDividor to USB...\n");
		err = inu_usb_monitor_jump(lramCodeAddr);
		if (err != INU_USB_ERR_SUCCESS)
		{
	        STORAGE_PRINT(INU_USB_LOG_ERROR, "Fail writng to USB...\n");
			return INU_STORAGE_ERR_INIT_FAILED;
		}
		inu_storage_ready++;
		if (inu_storage_read_info(INU_STORAGE_SPI_FLASH, 0, (unsigned char *)&flashInfo, sizeof(flashInfo), &transferSize) != INU_STORAGE_ERR_SUCCESS)
		{
			STORAGE_PRINT(INU_USB_LOG_ERROR, "can't read flash info...\n");
			
		}
	}

	return INU_STORAGE_ERR_SUCCESS;
}


/****************************************************************************
*
*  Function Name: inu_storage_read_raw_chunk
*
*  Description: read from flash- configure registers so the bare metal side will be able to use
*                    spiflash_read command
*
*  Inputs:  storage type, offset address,  output buffer, size of buffer, transfered size
*
*  Outputs:
*
*  Returns:
*
*  Context: read flash
*
****************************************************************************/
static InuStorageErrorE inu_storage_read_raw_chunk(InuStorageTypeE storage_type,
	InuStorageReadInfoE read_or_info,
	unsigned int offset,
	unsigned char *buffer,
	unsigned int size,
	unsigned int *transfered_size)
{
	InuStorageErrorE read_error = INU_STORAGE_ERR_SUCCESS;
	InuUsbErrorE err;
	int ready = 0;
	int target_addr = CRAM_START + CRAM_FREE_OFFSET;
	unsigned int read = BOOTFLASH_COMMAND_READ;
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();
    if (chipVersion == NU4000_B0_CHIP_VERSION)
    	target_addr = LRAM_START + LRAM_FREE_OFFSET;
	else if ((chipVersion == NU4000_C0_CHIP_VERSION)||(chipVersion == NU4000_C1_CHIP_VERSION)|| (chipVersion == NU4100_CHIP_VERSION))
   		target_addr = LRAM_START + LRAM_C0_FREE_OFFSET;
	if (read_or_info == INU_STORAGE_SPI_READ)
		read = BOOTFLASH_COMMAND_READ;
	else read = BOOTFLASH_COMMAND_INFO;

	//Configure spiflash_read command
	err = inu_usb_monitor_write(BOOTFLASH_COMMAND_REG, (unsigned char *)&read, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		return INU_STORAGE_ERR_IO_FAILED;
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_TYPE_REG, (unsigned char *)&storage_type, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		return INU_STORAGE_ERR_IO_FAILED;
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_OFFSET_REG, (unsigned char *)&offset, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		return INU_STORAGE_ERR_IO_FAILED;
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_DATA_SIZE_REG, (unsigned char *)&size, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		return INU_STORAGE_ERR_IO_FAILED;
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_DATA_ADDR_REG, (unsigned char *)&target_addr, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		return INU_STORAGE_ERR_IO_FAILED;

	//Activate target to do spiflash_read-read from flash and store the result in address value
	ready = BOOTFLASH_STATUS_ACTIVE;
	err = inu_usb_monitor_write(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		return INU_STORAGE_ERR_IO_FAILED;

	err = inu_usb_monitor_read(BOOTFLASH_STORAGE_OFFSET_REG, (unsigned char *)&read_error, sizeof(unsigned int), transfered_size);
	if (err != INU_USB_ERR_SUCCESS)
		return INU_STORAGE_ERR_IO_FAILED;

	do
	{
		err = inu_usb_monitor_read(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int), transfered_size);
		if (err != INU_USB_ERR_SUCCESS)
			return INU_STORAGE_ERR_IO_FAILED;
		host_millisleep(50);
	} while (ready != BOOTFLASH_STATUS_READY);

	//read from address value
	err = inu_usb_monitor_read(target_addr, buffer, size, transfered_size);
	if (err != INU_USB_ERR_SUCCESS)
		read_error = INU_STORAGE_ERR_IO_FAILED;
	else read_error = INU_STORAGE_ERR_SUCCESS;

	return read_error;
}


/****************************************************************************
*
*  Function Name: inu_storage_read_info
*
*  Description: read from flash- Control the bare metal (bootflash) to start & stop read function.
*
*  Inputs:  storage type, offset address,  output buffer, size of buffer, transfered size
*
*  Outputs:
*
*  Returns:
*
*  Context: read flash
*
****************************************************************************/
static InuStorageErrorE inu_storage_read_info(InuStorageTypeE storage_type, unsigned int offset, unsigned char *buffer, unsigned int size, unsigned int *transfered_size)
{
	InuUsbErrorE err;
	InuStorageErrorE read_error = INU_STORAGE_ERR_SUCCESS;
	unsigned int i = 0;
	int ready = 0;

	if (inu_storage_init(storage_type) != INU_STORAGE_ERR_SUCCESS)
		return INU_STORAGE_ERR_INIT_FAILED;

	//Wait for target ready (GME0 == 0)
	do
	{
		err = inu_usb_monitor_read(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int), transfered_size);
		if (err != INU_USB_ERR_SUCCESS)
			return INU_STORAGE_ERR_IO_FAILED;
		host_millisleep(50);
	} while (ready != BOOTFLASH_STATUS_READY);

	//check if the size of the buffer is bigger than the maximum chunk size, if so then separate read function into chunks.
	read_error = inu_storage_read_raw_chunk(storage_type, INU_STORAGE_SPI_INFO, offset, buffer, size, transfered_size);
	return read_error;
}

/****************************************************************************
*
*  Function Name: inu_storage_get_nand_offset
*
*  Description: returns partition/sector offset
*
*  Inputs:  partition sector
*
*  Outputs:  offset
*
*  Returns:
*
*  Context: flash
*
****************************************************************************/
static InuStorageErrorE inu_storage_get_nand_partition_offset(unsigned int partition, unsigned int *offset)
{
	InuStorageErrorE read_error = INU_STORAGE_ERR_SUCCESS;

	switch (partition)
	{
		case 0:	//raw partition factory default
			*offset = NAND_PARTITION_RAW_FACTORY_OFFSET_BLOCKS * NAND_BLOCK_SIZE;
			break;
		case 1:	//raw partition
			*offset = NAND_PARTITION_RAW_UPGRADE_OFFSET_BLOCKS * NAND_BLOCK_SIZE;
			break;
		case 2:
			*offset = NAND_PARTITION_FS_UPGRADE_OFFSET_BLOCKS * NAND_BLOCK_SIZE;
			break;
		case 3:
			*offset = NAND_PARTITION_APPLICATION_OFFSET_BLOCKS * NAND_BLOCK_SIZE;
			break;
		default:
			return INU_STORAGE_ERR_NOT_SUPPORTED;
	}
}
/****************************************************************************
*
*  Function Name: inu_storage_get_nand_section_offset_size
*
*  Description: returns partition/sector offset
*
*  Inputs:  partition sector
*
*  Outputs:  offset
*
*  Returns:
*
*  Context: flash
*
****************************************************************************/
static InuStorageErrorE inu_storage_get_nand_section_offset_size(InuStorageNandSectionTypE type, unsigned int *offset, unsigned int *size)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;

		switch (type)
		{
		case	NAND_SECTION_TYPE_HEADER:
			*offset = NAND_SECTION_HEADER_START_BLOCK * NAND_BLOCK_SIZE;
			*size   = NAND_SECTION_HEADER_SIZE_BLOCKS * NAND_BLOCK_SIZE;
			break;
		case	NAND_SECTION_TYPE_BOOTSPL:
			*offset = NAND_SECTION_BOOTSPL_START_BLOCK * NAND_BLOCK_SIZE;
			*size = NAND_SECTION_BOOTSPL_SIZE_BLOCKS * NAND_BLOCK_SIZE; 
			break;
		case	NAND_SECTION_TYPE_KERNEL:
			*offset = NAND_SECTION_KERNEL_START_BLOCK * NAND_BLOCK_SIZE;
			*size = NAND_SECTION_KERNEL_SIZE_BLOCKS * NAND_BLOCK_SIZE;
			break;
		case	NAND_SECTION_TYPE_DTB:
			*offset = NAND_SECTION_DTB_START_BLOCK * NAND_BLOCK_SIZE;
			*size = NAND_SECTION_DTB_SIZE_BLOCKS * NAND_BLOCK_SIZE;
			break;
		case	NAND_SECTION_TYPE_CEVA:
			*offset = NAND_SECTION_CEVA_START_BLOCK * NAND_BLOCK_SIZE;
			*size = NAND_SECTION_CEVA_SIZE_BLOCKS * NAND_BLOCK_SIZE;
			break;
		case	NAND_SECTION_TYPE_EV62_BOOT:
			*offset = NAND_SECTION_EV62_BOOT_START_BLOCK * NAND_BLOCK_SIZE;
			*size = NAND_SECTION_EV62_BOOT_SIZE_BLOCKS * NAND_BLOCK_SIZE;
			break;
		case	NAND_SECTION_TYPE_EV62_APP:
			*offset = NAND_SECTION_EV62_APP_START_BLOCK * NAND_BLOCK_SIZE;
			*size = NAND_SECTION_EV62_APP_SIZE_BLOCKS * NAND_BLOCK_SIZE;
			break;
		case	NAND_SECTION_TYPE_INITRAMFS:
			*offset = NAND_SECTION_INITRAMFS_START_BLOCK * NAND_BLOCK_SIZE;
			*size = NAND_SECTION_INITRAMFS_SIZE_BLOCKS * NAND_BLOCK_SIZE;
			break;
		case	NAND_SECTION_TYPE_APP:
			*offset = NAND_SECTION_APP_START_BLOCK * NAND_BLOCK_SIZE;
			*size = NAND_SECTION_APP_SIZE_BLOCKS * NAND_BLOCK_SIZE;
			break;

		default:
			return INU_STORAGE_ERR_NOT_SUPPORTED;
		
		}

	return INU_STORAGE_ERR_SUCCESS;

}


static InuStorageErrorE inu_storage_init_nand_header(InuStorageNandHeaderT* header)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;
	unsigned char type = 0;
	unsigned int sectOffset, sectLen;

	//memset(header, NAND_EMPTY_UINT8, sizeof(InuStorageNandHeaderT));

	header->magicNumber = NAND_HEADER_MAGIC_NUMBER;
	header->headerVersion = NAND_HEADER_HEADER_VERSION;


	for (type = NAND_SECTION_TYPE_HEADER; type <= NAND_SECTION_TYPE_APP; type++)
	{
		ret = inu_storage_get_nand_section_offset_size((InuStorageNandSectionTypE)type, &sectOffset, &sectLen);

		//LOGG_PRINT(LOG_INFO_E, NULL, "Section type %d, offset 0x%x size 0x%x \n", type, sectOffset, sectLen);
		header->section[type].type = (InuStorageNandSectionTypE)type;
		header->section[type].startAddress = sectOffset;
		header->section[type].sizeBytes = sectLen;
		header->section[type].imgStartAddress = NAND_EMPTY_UINT32;
		header->section[type].imgSizeBytes = NAND_EMPTY_UINT32;
		header->section[type].imgVersion.major = NAND_EMPTY_UINT32;
		header->section[type].imgVersion.minor = NAND_EMPTY_UINT32;
		header->section[type].imgVersion.build = NAND_EMPTY_UINT32;
	}

	return INU_STORAGE_ERR_SUCCESS;
}

/****************************************************************************
*
*  Function Name: inu_storage_read_raw
*
*  Description: read from flash- Control the bare metal (bootflash) to start & stop read function.
*
*  Inputs:  storage type, offset address,  output buffer, size of buffer, transfered size
*
*  Outputs:
*
*  Returns:
*
*  Context: read flash
*
****************************************************************************/
INUSTORAGE_API
InuStorageErrorE inu_storage_read_raw(InuStorageTypeE storage_type, unsigned int offset, unsigned char *buffer, unsigned int size, unsigned int *transfered_size)
{
	InuUsbErrorE err;
	InuStorageErrorE read_error = INU_STORAGE_ERR_SUCCESS;
	unsigned int i = 0;
	int ready = 0;

	if (dryRunMode)
	   return INU_STORAGE_ERR_SUCCESS;
	if (inu_storage_init(storage_type) != INU_STORAGE_ERR_SUCCESS)
		return INU_STORAGE_ERR_INIT_FAILED;

	//Wait for target ready (GME0 == 0)
	do
	{
		err = inu_usb_monitor_read(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int), transfered_size);
		if (err != INU_USB_ERR_SUCCESS)
			return INU_STORAGE_ERR_IO_FAILED;
		host_millisleep(50);
	} while (ready != BOOTFLASH_STATUS_READY);

	//check if the size of the buffer is bigger than the maximum chunk size, if so then separate read function into chunks.
	for (i = 0; i<(size / MAX_CHUNK); i++)
		read_error = inu_storage_read_raw_chunk(storage_type, INU_STORAGE_SPI_READ, offset + i*MAX_CHUNK, buffer + i*MAX_CHUNK, MAX_CHUNK, transfered_size);
	if (size%MAX_CHUNK)
		read_error = inu_storage_read_raw_chunk(storage_type, INU_STORAGE_SPI_READ, offset + i*MAX_CHUNK, buffer + i*MAX_CHUNK, (size%MAX_CHUNK), transfered_size);

	return read_error;
}

INUSTORAGE_API
void inu_storage_update_bootsplOffest(unsigned int bootsplOffset)
{
	if (bootsplOffset == 0)
	{
		sectionBootsplStartBlock = SECTION_BOOTSPL_START_BLOCK;
	}
	else if (bootsplOffset == 1)
	{
		// In case of Backup Partition, sectionBootsplStartBlock should be in the middle block of the Flash
		sectionBootsplStartBlock = flashInfo.numOfBlocks/2;
	}
	printf("inu_storage_update_bootsplOffest():: bootsplOffset = %d, sectionBootsplStartBlock = %d\n", bootsplOffset, sectionBootsplStartBlock);
}

INUSTORAGE_API
InuStorageErrorE inu_storage_rw_spi_main(InuStorageTypeE storage_type, unsigned char *txBuffer,unsigned char *rxBuffer, unsigned int txRxSize, unsigned int size)
{
   InuUsbErrorE err;
   unsigned int ready = BOOTFLASH_STATUS_UNKNOWN;
   unsigned int command = BOOTFLASH_COMMAND_WRITE;
   unsigned int targetTxBuffer = CRAM_START + CRAM_FREE_OFFSET;
   unsigned int targetRxBuffer; 
   unsigned int transfered_size = 0;
   InuUsbParamsT *inuUsbParams = inu_usb_get_contex();

   do
   {
      err = inu_usb_monitor_read(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int), &transfered_size);
      host_millisleep(40);
   } while (ready != BOOTFLASH_STATUS_READY);
    if (chipVersion == NU4000_B0_CHIP_VERSION)
		targetTxBuffer = LRAM_START + LRAM_FREE_OFFSET;
    else if ((chipVersion == NU4000_C0_CHIP_VERSION) || (chipVersion == NU4000_C1_CHIP_VERSION) || (chipVersion == NU4100_CHIP_VERSION))
		targetTxBuffer = LRAM_START + LRAM_C0_FREE_OFFSET;
   targetRxBuffer =  targetTxBuffer + MAX_SPI_CHUNK;;
   err = inu_usb_monitor_write(targetTxBuffer, txBuffer, size);
   if (err != INU_USB_ERR_SUCCESS)
      STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
   //Configure command
   err = inu_usb_monitor_write(BOOTFLASH_COMMAND_REG, (unsigned char *)&command, sizeof(unsigned int));
   if (err != INU_USB_ERR_SUCCESS)
      STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
   err = inu_usb_monitor_write(BOOTFLASH_STORAGE_TYPE_REG, (unsigned char *)&storage_type, sizeof(unsigned int));
   if (err != INU_USB_ERR_SUCCESS)
      STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
   err = inu_usb_monitor_write(GME_SAVE_AND_RESTORE3_ADDR, (unsigned char *)&targetTxBuffer, sizeof(unsigned int));
   if (err != INU_USB_ERR_SUCCESS)
      STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
   err = inu_usb_monitor_write(GME_SAVE_AND_RESTORE4_ADDR, (unsigned char *)&targetRxBuffer, sizeof(unsigned int));
   if (err != INU_USB_ERR_SUCCESS)
      STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
   err = inu_usb_monitor_write(GME_SAVE_AND_RESTORE5_ADDR, (unsigned char *)&txRxSize, sizeof(unsigned int));
   if (err != INU_USB_ERR_SUCCESS)
      STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
   err = inu_usb_monitor_write(GME_SAVE_AND_RESTORE7_ADDR, (unsigned char *)&size, sizeof(unsigned int));
   if (err != INU_USB_ERR_SUCCESS)
      STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");

   //Activate target
   ready = BOOTFLASH_STATUS_ACTIVE;
   err = inu_usb_monitor_write(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int));
   if (err != INU_USB_ERR_SUCCESS)
      STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");

   do
   {
      err = inu_usb_monitor_read(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int), &transfered_size);
      if (err != INU_USB_ERR_SUCCESS)
         STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
      host_millisleep(40);
   } while (ready != BOOTFLASH_STATUS_READY);
   if (rxBuffer != NULL)
   {
      err = inu_usb_monitor_read(targetRxBuffer, rxBuffer, size, &transfered_size);
      if (err != INU_USB_ERR_SUCCESS)
         STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
   }
   return INU_STORAGE_ERR_SUCCESS;

}

INUSTORAGE_API
InuStorageErrorE inu_storage_write_raw_chunk(InuStorageTypeE storage_type, unsigned int offset, unsigned char *buffer, unsigned int size)
{
	// send boot_flash if needed
	// send file by chunks of 128K or less
	// send file chunk, togle command (read,write)
	// continue to other chunks
	InuUsbErrorE err;
	unsigned int ready = BOOTFLASH_STATUS_UNKNOWN;
	unsigned int command = BOOTFLASH_COMMAND_WRITE;
	unsigned int targetBuffer = CRAM_START + CRAM_FREE_OFFSET;
	unsigned int transfered_size = 0;
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();

	if (inu_storage_init(storage_type) != INU_STORAGE_ERR_SUCCESS)
		return INU_STORAGE_ERR_INIT_FAILED;

	do
	{
		err = inu_usb_monitor_read(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int), &transfered_size);
		host_millisleep(50);
	} while (ready != BOOTFLASH_STATUS_READY);
    if (chipVersion == NU4000_B0_CHIP_VERSION)
    	targetBuffer = LRAM_START + LRAM_FREE_OFFSET;
    else if ((chipVersion == NU4000_C0_CHIP_VERSION)||(chipVersion == NU4000_C1_CHIP_VERSION)|| (chipVersion == NU4100_CHIP_VERSION))
    	targetBuffer = LRAM_START + LRAM_C0_FREE_OFFSET;

	err = inu_usb_monitor_write(targetBuffer, buffer, size);
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
	//Configure command
	err = inu_usb_monitor_write(BOOTFLASH_COMMAND_REG, (unsigned char *)&command, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_TYPE_REG, (unsigned char *)&storage_type, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_OFFSET_REG, (unsigned char *)&offset, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_DATA_SIZE_REG, (unsigned char *)&size, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_DATA_ADDR_REG, (unsigned char *)&targetBuffer, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");

	//Activate target
	ready = BOOTFLASH_STATUS_ACTIVE;
	err = inu_usb_monitor_write(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");

	do
	{
		err = inu_usb_monitor_read(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int), &transfered_size);
		if (err != INU_USB_ERR_SUCCESS)
			STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
		host_millisleep(50);
	} while (ready != BOOTFLASH_STATUS_READY);
	err = inu_usb_monitor_read(targetBuffer, buffer, size, &transfered_size);
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
	return INU_STORAGE_ERR_SUCCESS;
}

#ifdef _DEBUG
INUSTORAGE_API
#endif
InuStorageErrorE inu_storage_erase(InuStorageTypeE storage_type, unsigned int offset, unsigned int size)
{
	// send boot_flash if needed
	// send file by chunks of 128K or less
	// send file chunk, togle command (read,write)
	// continue to other chunks
	InuUsbErrorE err;
	unsigned int ready = BOOTFLASH_STATUS_UNKNOWN;
	unsigned int command = BOOTFLASH_COMMAND_ERASE;
	unsigned int transfered_size = 0;
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();

	if (inu_storage_init(storage_type) != INU_STORAGE_ERR_SUCCESS)
		return INU_STORAGE_ERR_INIT_FAILED;

	do
	{
		err = inu_usb_monitor_read(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int), &transfered_size);
		host_millisleep(50);
	} while (ready != BOOTFLASH_STATUS_READY);
	//Configure command
	err = inu_usb_monitor_write(BOOTFLASH_COMMAND_REG, (unsigned char *)&command, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_TYPE_REG, (unsigned char *)&storage_type, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_OFFSET_REG, (unsigned char *)&offset, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
	err = inu_usb_monitor_write(BOOTFLASH_STORAGE_DATA_SIZE_REG, (unsigned char *)&size, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");

	//Activate target
	ready = BOOTFLASH_STATUS_ACTIVE;
	err = inu_usb_monitor_write(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int));
	if (err != INU_USB_ERR_SUCCESS)
		STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");

	do
	{
		err = inu_usb_monitor_read(BOOTFLASH_STATUS_REG, (unsigned char *)&ready, sizeof(unsigned int), &transfered_size);
		if (err != INU_USB_ERR_SUCCESS)
			STORAGE_PRINT(INU_USB_LOG_INFO, "error!!!!\n");
		host_millisleep(50);
	} while (ready != BOOTFLASH_STATUS_READY);
	return INU_STORAGE_ERR_SUCCESS;
}

INUSTORAGE_API
InuStorageErrorE inu_storage_write_raw(InuStorageTypeE storage_type, unsigned int offset, unsigned char *buffer, unsigned int size)
{
   unsigned int i = 0;
   if (!dryRunMode)
   {
      for (i = 0; i<(size / MAX_CHUNK); i++)
         inu_storage_write_raw_chunk(storage_type, offset + i*MAX_CHUNK, buffer + i*MAX_CHUNK, MAX_CHUNK);
      if (size%MAX_CHUNK)
         inu_storage_write_raw_chunk(storage_type, offset + i*MAX_CHUNK, buffer + i*MAX_CHUNK, (size%MAX_CHUNK));
   }
   return INU_STORAGE_ERR_SUCCESS;
}

INUSTORAGE_API
InuStorageErrorE inu_storage_rw_spi(InuStorageTypeE storage_type,unsigned char *txBuffer,unsigned char *rxBuffer, unsigned int txRxSize,unsigned int numOfMsgs)
{
   unsigned int ind = 0,size = numOfMsgs*txRxSize,chunkSize=0x4000;
   if (inu_storage_init(storage_type) != INU_STORAGE_ERR_SUCCESS)
	   return INU_STORAGE_ERR_INIT_FAILED;
  for (ind = 0; ind < (size / chunkSize); ind++)
  {

     inu_storage_rw_spi_main(INU_STORAGE_SPI, txBuffer + ind*chunkSize, rxBuffer+ ind*chunkSize,txRxSize,chunkSize);
  }
  if (size%chunkSize)
     inu_storage_rw_spi_main(INU_STORAGE_SPI, txBuffer + ind*chunkSize, rxBuffer+ ind*chunkSize,txRxSize, (size%chunkSize));
   return INU_STORAGE_ERR_SUCCESS;
}


INUSTORAGE_API
InuStorageErrorE inu_storage_program_efuse(unsigned int *efuseBuff)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_INIT_FAILED;
	unsigned int ind;

	for (ind = 0; ind < 8; ind++)
		ret = inu_storage_write_raw_chunk(INU_STORAGE_EFUSE, 0, (unsigned char *)efuseBuff, EFUSE_SIZE);

	return ret;
}

INUSTORAGE_API
InuStorageErrorE inu_flash_write_production_data(InuProductionDataT *prodData, unsigned int offset)
{
	InuStorageErrorE ret;
	InuProductionHeaderT prodHeader;
	InuExtendedProductionHeaderT extProdHeader;
	unsigned int size=sizeof(InuProductionHeaderT);
	unsigned char *dataPtr=(unsigned char *)&prodHeader;

	if (prodData->extKey != EXT_KEY_NUM)
	{
	   memcpy(prodHeader.serialNumber, prodData->serialNumber, PRODUCTION_STRING_SIZE);
	   memcpy(prodHeader.modelNumber, prodData->modelNumber, PRODUCTION_STRING_SIZE);
	   memcpy(prodHeader.partNumber, prodData->partNumber, PRODUCTION_STRING_SIZE);
	   memcpy(prodHeader.sensorRevision, prodData->sensorRevision, PRODUCTION_STRING_SIZE);
	   prodHeader.bootId = prodData->bootId;
	}
	else
	{
	   extProdHeader.extKey = prodData->extKey;
	   memcpy(extProdHeader.serialNumber,   prodData->serialNumber, PRODUCTION_EXT_STRING_SIZE);
	   memcpy(extProdHeader.modelNumber,    prodData->modelNumber, PRODUCTION_STRING_SIZE);
	   memcpy(extProdHeader.partNumber,     prodData->partNumber, PRODUCTION_STRING_SIZE);
	   memcpy(extProdHeader.sensorRevision, prodData->sensorRevision, PRODUCTION_STRING_SIZE);
	   extProdHeader.bootId = prodData->bootId;
	   size=sizeof(InuExtendedProductionHeaderT);
	   dataPtr = (unsigned char *)&extProdHeader;
	}

	ret = inu_storage_write_raw(INU_STORAGE_SPI_FLASH, offset, dataPtr, size);
   if (dryRunMode)
   {
      inu_storage_write_dry_run_data(INU_STORAGE_SECTION_PRODUCTION_DATA,offset,dataPtr, size);
   }
	if (ret != INU_STORAGE_ERR_SUCCESS)
		return ret;

	return INU_STORAGE_ERR_SUCCESS;
}


InuStorageErrorE inu_flash_write_file_data(unsigned char *buffer, unsigned int size, unsigned int offset)
{

	InuStorageErrorE ret;
	unsigned int transfered = 0;
	// write data	
	if (buffer != NULL)
	{
		ret = inu_storage_write_raw(INU_STORAGE_SPI_FLASH, offset, buffer, size);
		if (ret != INU_STORAGE_ERR_SUCCESS)
			return ret;
	}

	return INU_STORAGE_ERR_SUCCESS;
}

InuStorageErrorE inu_flash_write_bootfix_metadata(InuBootfixHeaderT *pInuBootfixHeaderT, unsigned int size, unsigned int offset)
{

	InuStorageErrorE ret;
	unsigned int transfered = 0;

	// write data	
	if (pInuBootfixHeaderT != NULL)
	{
		ret = inu_storage_write_raw(INU_STORAGE_SPI_FLASH, offset, (unsigned char *)pInuBootfixHeaderT, sizeof(InuBootfixHeaderT));
		if (ret != INU_STORAGE_ERR_SUCCESS)
			return ret;
	}

	return INU_STORAGE_ERR_SUCCESS;
}




INUSTORAGE_API
InuStorageErrorE inu_flash_write_bootfix_loader(unsigned char *buffer, unsigned int size)
{
	BootromStorageHeaderT bootromStorageHeader;
	InuStorageErrorE ret;

	memset(&bootromStorageHeader, 0, sizeof(bootromStorageHeader));

	bootromStorageHeader.magic = BOOTROM_STORAGE_NU3000_MAGIC_NUMBER;
	bootromStorageHeader.bootcode_size = *((unsigned int *)buffer);
	buffer += 4;
	size -= 4;
	if ((chipVersion == NU4000_B0_CHIP_VERSION) || (overwriteChipVersion == NU4000_B0_CHIP_VERSION)|| (chipVersion == NU4000_C0_CHIP_VERSION) || (chipVersion == NU4000_C1_CHIP_VERSION)|| (chipVersion == NU4100_CHIP_VERSION))
		bootromStorageHeader.bootcode_load_address = LRAM_LOADER_ADDR;
	else bootromStorageHeader.bootcode_load_address = CRAM_START;
	bootromStorageHeader.storage_type = 1;
	ret = inu_storage_write_raw(INU_STORAGE_SPI_FLASH, 0, (unsigned char *)&bootromStorageHeader, sizeof(bootromStorageHeader));
	if (ret != INU_STORAGE_ERR_SUCCESS)
	{
		return ret;
	}
	ret = inu_storage_write_raw(INU_STORAGE_SPI_FLASH, sizeof(bootromStorageHeader), buffer, size);
	if (dryRunMode)
	{
	   inu_storage_write_dry_run_data(INU_STORAGE_SECTION_BOOTFIX,0,(unsigned char *)&bootromStorageHeader, sizeof(bootromStorageHeader));
	   inu_storage_write_dry_run_data(INU_STORAGE_SECTION_BOOTFIX,sizeof(bootromStorageHeader),buffer, size);
	}
	if (ret != INU_STORAGE_ERR_SUCCESS)
		return ret;
	return INU_STORAGE_ERR_SUCCESS;
}


#if 0
INUSTORAGE_API
InuStorageErrorE inu_flash_write_bootfix_strap(unsigned int strap_value)
{
	InuStorageErrorE ret;
	InuBootfixHeaderT bootfixHeader;
	unsigned int transfered = 0;

	ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, (unsigned int)offsetof(InuStorageLayoutT, bootfixHeader), (unsigned char *)&bootfixHeader, sizeof(InuBootfixHeaderT), &transfered);
	if (ret != INU_STORAGE_ERR_SUCCESS)
		return ret;

	bootfixHeader.strapValue = strap_value;
	bootfixHeader.magicNumber = BOOTFIX_HEADER_MAGIC_NUMBER;
	bootfixHeader.versionNumber = BOOTFIX_HEADER_VERSION_NUMBER;

	ret = inu_storage_write_raw(INU_STORAGE_SPI_FLASH, (unsigned int)offsetof(InuStorageLayoutT, bootfixHeader), (unsigned char *)&bootfixHeader, sizeof(InuBootfixHeaderT));
	if (ret != INU_STORAGE_ERR_SUCCESS)
		return ret;

	return INU_STORAGE_ERR_SUCCESS;
}

InuStorageErrorE inu_flash_read_calibration_data(InuCalibrationDataT *calibrationData)
{
	InuCalibrationHeaderT calibrationHeader;
	InuStorageErrorE ret;
	unsigned int transfered = 0;
	ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, (unsigned int)offsetof(InuStorageLayoutT, calibrationHeader), (unsigned char *)&calibrationHeader, sizeof(InuCalibrationHeaderT), &transfered);
	if (ret != INU_STORAGE_ERR_SUCCESS)
		return ret;
	calibrationData->calibrationDataFormat = (InuCalibratiuonFormatE)calibrationHeader.calibrationDataFormat;
	calibrationData->calibrationDataSize = calibrationHeader.calibrationDataSize;
	if ((calibrationHeader.calibrationDataSize < MAX_CALIB_DATA) && (calibrationHeader.calibrationDataSize != 0))
	{
		if (calibrationData->calibrationDataBuffer == NULL)
			return INU_STORAGE_ERR_INIT_FAILED;
		ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, calibrationHeader.calibrationDataOffset, (unsigned char *)calibrationData->calibrationDataBuffer, calibrationData->calibrationDataSize, &transfered);
		if (ret != INU_STORAGE_ERR_SUCCESS)
			return ret;
	}
	return INU_STORAGE_ERR_SUCCESS;
}


#endif

int usbConnected = 0;
static int hotplug_callback(InuUsbStatusE status, unsigned int version)
{
	if ((status == INU_USB_EVT_INSERTION) || (status == INU_USB_EVT_BOOT_COMPLETE)|| (status == INU_USB_EVT_FILTERID_UPDATED))
		usbConnected = 1;
	else if (status == INU_USB_EVT_REMOVAL)
		usbConnected = 0;
	else
		usbConnected = 0;
	return 0;
}

void reset_storage_magic()
{
	BootromStorageHeaderT bootromStorageHeaderT;
	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;
	unsigned int transferSize;

	ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, 0, (unsigned char *)&bootromStorageHeaderT, sizeof(BootromStorageHeaderT), &transferSize);
	if (ret == INU_STORAGE_ERR_SUCCESS)
	{
		if (bootromStorageHeaderT.magic != BOOTROM_STORAGE_NU3000_MAGIC_NUMBER)
		{
			bootromStorageHeaderT.magic = 0;
			ret = inu_storage_write_raw(INU_STORAGE_SPI_FLASH, 0, (unsigned char *)&bootromStorageHeaderT, sizeof(BootromStorageHeaderT));
		}
	}
}


#ifdef _DEBUG
INUSTORAGE_API
#endif
InuStorageErrorE findBlockAddress(InuStorageSectionTypeE sectionType, unsigned int *pBlockaddress)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;
	InuSectionHeaderT inuSectionHeaderT,InuLastFrmEndSectAddr;
	unsigned int blockAddress, transferSize;
	unsigned int ind,lastBlockAddress=0;
	switch (sectionType)
	{

	case INU_STORAGE_SECTION_BOOTFIX:
		*pBlockaddress = SECTION_BOOTFIX_START_BLOCK*flashInfo.blockSize;
		break;
	case INU_STORAGE_SECTION_PRODUCTION_DATA:
		*pBlockaddress = SECTION_PRODUCTION_START_BLOCK*flashInfo.blockSize;
		break;
	case INU_STORAGE_SECTION_CALIBRATION_DATA:
		// header block in end of flash
		blockAddress = flashInfo.numOfBlocks*flashInfo.blockSize - sizeof(inuSectionHeaderT);
		ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, blockAddress, (unsigned char *)&inuSectionHeaderT, sizeof(inuSectionHeaderT), &transferSize);
		if (ret == INU_STORAGE_ERR_SUCCESS)
		{
			if (storageMagicNumber[sectionType] == inuSectionHeaderT.magicNumber)
				blockAddress = (flashInfo.numOfBlocks - inuSectionHeaderT.sectionSize)*flashInfo.blockSize;
			else blockAddress=(flashInfo.numOfBlocks-1)*flashInfo.blockSize;
		}
		*pBlockaddress = blockAddress;
		break;
	case INU_STORAGE_SECTION_DYNAMIC_CALIBRATION:
		// header block in end of flash
		blockAddress = flashInfo.numOfBlocks*flashInfo.blockSize - sizeof(inuSectionHeaderT);
		ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, blockAddress, (unsigned char *)&InuLastFrmEndSectAddr, sizeof(InuLastFrmEndSectAddr), &transferSize);
		if (ret == INU_STORAGE_ERR_SUCCESS)
		{
			if (storageMagicNumber[INU_STORAGE_SECTION_CALIBRATION_DATA] == InuLastFrmEndSectAddr.magicNumber)
			{
				blockAddress = (flashInfo.numOfBlocks - InuLastFrmEndSectAddr.sectionSize - 1)*flashInfo.blockSize;
			}
			else return INU_STORAGE_ERR_INIT_FAILED;
		}
		*pBlockaddress = blockAddress;
		break;
	case INU_STORAGE_SECTION_BOOTFIX_METADATA:
		*pBlockaddress = SECTION_BOOTFIX_META_START_BLOCK*flashInfo.blockSize;
		break;
	case INU_STORAGE_SECTION_BOOTSPL:
		*pBlockaddress = sectionBootsplStartBlock * flashInfo.blockSize;
		break;
	case INU_STORAGE_SECTION_DTB:
	case INU_STORAGE_SECTION_KERNEL:
	case INU_STORAGE_SECTION_ROOTFS:
	case INU_STORAGE_SECTION_CEVA:
	case INU_STORAGE_SECTION_CNN_LD:
	case INU_STORAGE_SECTION_CNN:
	case INU_STORAGE_SECTION_APP:

	*pBlockaddress = sectionBootsplStartBlock * flashInfo.blockSize;
	for(ind = INU_STORAGE_SECTION_BOOTSPL;ind < sectionType;ind++)
	{
		if (lastBlockAddress != *pBlockaddress) // avoid reading again when skip CEVA/CNN
		ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, *pBlockaddress, (unsigned char *)&inuSectionHeaderT, sizeof(inuSectionHeaderT), &transferSize);
		lastBlockAddress = *pBlockaddress;
		if (ret != INU_STORAGE_ERR_SUCCESS)
		{
			STORAGE_PRINT(INU_USB_LOG_ERROR, "next section not found %d\n",ind);
			return INU_STORAGE_ERR_INIT_FAILED;
		}

		if (storageMagicNumber[ind] != inuSectionHeaderT.magicNumber)
		{
			// this sections can be ignored
			if ((storageMustHaveComponent[ind] == 0)&&(sectionType != ind))
			{
				continue;
			}
			STORAGE_PRINT(INU_USB_LOG_ERROR, "next section not found %d\n",ind);
			return INU_STORAGE_ERR_INIT_FAILED;
		}
		else
		{
			*pBlockaddress += inuSectionHeaderT.sectionSize * flashInfo.blockSize;
		}
 	}
		break;
	default:
		ret = INU_STORAGE_ERR_INIT_FAILED;
		break;

	}
	if ((*pBlockaddress / flashInfo.blockSize) > (flashInfo.numOfBlocks - 1))
		ret = INU_STORAGE_ERR_INIT_FAILED;
    if (ret != INU_STORAGE_ERR_SUCCESS)
        STORAGE_PRINT(INU_USB_LOG_ERROR, "Fail \n");

	return ret;
}


INUSTORAGE_API
InuStorageErrorE inu_storage_read_section_data(InuStorageSectionTypeE sectionType, InuSectionDataT *sectionData)
{

	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;

	InuSectionHeaderT inuSectionHeaderT;
	InuProductionHeaderT* prodHdr;
	InuProductionDataT* productionData = (InuProductionDataT*)sectionData->sectionDataBuffer;
	InuExtendedProductionHeaderT extProd;

	unsigned int blockAddress, transferSize,hdrAddress;

	if (dryRunMode)
	   return INU_STORAGE_ERR_SUCCESS;
	if (inu_storage_init(INU_STORAGE_SPI_FLASH ) != INU_STORAGE_ERR_SUCCESS)
		return INU_STORAGE_ERR_INIT_FAILED;

	memset(&inuSectionHeaderT, 0, sizeof(InuSectionHeaderT));

	if ((sectionData == NULL) || (sectionType > INU_STORAGE_SECTION_APP))
	{
		STORAGE_PRINT(INU_USB_LOG_ERROR,"error In Section\n");
		return INU_STORAGE_ERR_INIT_FAILED;
	}
	if (findBlockAddress(sectionType, &blockAddress) == INU_STORAGE_ERR_INIT_FAILED)
	{
		STORAGE_PRINT(INU_USB_LOG_ERROR,"error In Section\n");
		return INU_STORAGE_ERR_INIT_FAILED;
	}
	hdrAddress = blockAddress;
	if (sectionType == INU_STORAGE_SECTION_BOOTFIX)
	{
		hdrAddress = BOOTFIX_DATA_SIZE;
	}
	if (sectionType == INU_STORAGE_SECTION_CALIBRATION_DATA)
	{
		hdrAddress = flashInfo.blockSize*flashInfo.numOfBlocks - sizeof(inuSectionHeaderT);
	}

	if (sectionType == INU_STORAGE_SECTION_DYNAMIC_CALIBRATION)
	{
		hdrAddress = blockAddress + flashInfo.blockSize  - sizeof(inuSectionHeaderT);
	}

	ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, hdrAddress, (unsigned char *)&inuSectionHeaderT, sizeof(inuSectionHeaderT), &transferSize);

	if (ret != INU_STORAGE_ERR_SUCCESS)
	{
		STORAGE_PRINT(INU_USB_LOG_ERROR, "Read raw error\n");
		return ret;
	}
	if (ret == INU_STORAGE_ERR_SUCCESS)
	{
		sectionData->sectionDataFormat = inuSectionHeaderT.sectionFormat;
		sectionData->timestamp = inuSectionHeaderT.timestamp;
		strncpy(sectionData->versionString, inuSectionHeaderT.versionString, VERSION_STRING_SIZE - 1);

	}

	if (storageMagicNumber[sectionType] != inuSectionHeaderT.magicNumber)
	{
        STORAGE_PRINT(INU_USB_LOG_ERROR, "Wrong magic number\n");
	    return INU_STORAGE_ERR_INIT_FAILED;
	}

	switch (sectionType)
	{

	case INU_STORAGE_SECTION_BOOTFIX:
		if ((sectionData->sectionDataBuffer != NULL) && (sectionData->sectionDataSize <= BOOTFIX_DATA_SIZE))
		{
            //include bootfix loader size in buffer
			ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, sizeof(BootromStorageHeaderT) - 4 , sectionData->sectionDataBuffer, sectionData->sectionDataSize, &transferSize);
			if (ret != INU_STORAGE_ERR_SUCCESS)
			{
			    STORAGE_PRINT(INU_USB_LOG_ERROR, "Read raw error\n");
				return ret;
			}
		}
		break;
	case INU_STORAGE_SECTION_PRODUCTION_DATA:
		if ((sectionData->sectionDataBuffer != NULL))
		{
		  ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, blockAddress + sizeof(inuSectionHeaderT), (unsigned char *)&extProd, sizeof(extProd),  &transferSize);
          if (extProd.extKey == EXT_KEY_NUM)
          {
			  memcpy(productionData,&extProd,sizeof(extProd));
          }
          else
          {
            prodHdr = (InuProductionHeaderT *)&extProd;
            productionData->extKey = 0;
            memcpy(productionData->serialNumber,prodHdr->serialNumber, PRODUCTION_EXT_STRING_SIZE);
            memcpy(productionData->modelNumber, prodHdr->modelNumber, PRODUCTION_STRING_SIZE);
            memcpy(productionData->partNumber, prodHdr->partNumber, PRODUCTION_STRING_SIZE);
            memcpy(productionData->sensorRevision, prodHdr->sensorRevision, PRODUCTION_STRING_SIZE);
            productionData->bootId = prodHdr->bootId; // aligned to extended no need to copy
          }
 		  productionData->chipHardwareVersion = chipVersion;
        }
		break;
	case INU_STORAGE_SECTION_CALIBRATION_DATA:
		if (sectionData->sectionDataBuffer != NULL)
		{

			ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, blockAddress, sectionData->sectionDataBuffer, sectionData->sectionDataSize, &transferSize);
			if (ret != INU_STORAGE_ERR_SUCCESS)
			{
                STORAGE_PRINT(INU_USB_LOG_ERROR, "Read raw error\n");
				return ret;
			}
		}
		break;
	case INU_STORAGE_SECTION_DYNAMIC_CALIBRATION:
		if (sectionData->sectionDataBuffer != NULL)
		{

			ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, blockAddress - (inuSectionHeaderT.sectionSize - 1)* flashInfo.blockSize, sectionData->sectionDataBuffer, sectionData->sectionDataSize, &transferSize);
			if (ret != INU_STORAGE_ERR_SUCCESS)
			{
				STORAGE_PRINT(INU_USB_LOG_ERROR, "Read raw error\n");
				return ret;
			}
		}
		break;
		
	case INU_STORAGE_SECTION_BOOTSPL:
	case INU_STORAGE_SECTION_DTB:
	case INU_STORAGE_SECTION_KERNEL:
   case INU_STORAGE_SECTION_ROOTFS:
   case INU_STORAGE_SECTION_CEVA:
   case INU_STORAGE_SECTION_CNN_LD:
   case INU_STORAGE_SECTION_CNN:
	case INU_STORAGE_SECTION_APP:
	case INU_STORAGE_SECTION_BOOTFIX_METADATA:
		if (sectionData->sectionDataBuffer != NULL)
		{
			ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, blockAddress + sizeof(inuSectionHeaderT), sectionData->sectionDataBuffer, sectionData->sectionDataSize, &transferSize);
			if (ret != INU_STORAGE_ERR_SUCCESS)
			{
                STORAGE_PRINT(INU_USB_LOG_ERROR, "Read raw error\n");
				return ret;
			}
		}
		break;
	default:
		break;

	}
	sectionData->sectionDataSize = inuSectionHeaderT.sectionDataSize;

	return ret;
}


InuStorageErrorE inu_storage_copyBuffToFile(InuStorageSectionTypeE sectionType,unsigned int createNewFile,unsigned int offset, unsigned char *dataPtr, unsigned int writeSize)
{
	FILE *fd;
	int rc = 0;
	char fileName[512];
   InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;

	snprintf(fileName,sizeof(fileName)-1,"%s%s%s_0x%7.7x",dryRunOutputDir,"\\",storageFileName[sectionType],storageSectorAddr[sectionType]);

	if (createNewFile)
	{

	   fd = fopen(fileName, "wb+");
	}
	else fd = fopen(fileName, "rb+");
	if (fd == NULL)
	{
	   printf("Can't open file %s\n", fileName);
	   return INU_STORAGE_ERR_DRYRUN;
	}

	rc = fseek(fd ,offset, SEEK_SET);

	if (rc == 0)
	{
		rc = fwrite(dataPtr, writeSize, 1, fd);
		if (rc != 1)
		{
			ret = INU_STORAGE_ERR_DRYRUN;
		    printf("Fail write file %s\n", fileName);
		}

	}
	else ret = INU_STORAGE_ERR_DRYRUN;

   fclose(fd);
   return ret;
}

InuStorageErrorE inu_storage_write_dry_run_hdr(InuStorageSectionTypeE sectionType,unsigned int hdrAddress,InuSectionHeaderT *sectionHdr)
{
   unsigned int offset;
   InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;
   switch (sectionType)
   {
      case INU_STORAGE_SECTION_BOOTFIX:
         offset = hdrAddress;
      break;
      case INU_STORAGE_SECTION_PRODUCTION_DATA:
		  offset = 0;
       break;
      case INU_STORAGE_SECTION_CALIBRATION_DATA:
		  offset = (sectionHdr->sectionSize * flashInfo.blockSize) - sizeof(InuSectionHeaderT);
		  break;
	   default:
	      return INU_STORAGE_ERR_NOT_SUPPORTED;
      break;

   }
   ret = inu_storage_copyBuffToFile(sectionType,0,offset, (unsigned char *)sectionHdr,sizeof(InuSectionHeaderT));
   return ret;
}

InuStorageErrorE inu_storage_write_dry_run_data(InuStorageSectionTypeE sectionType,unsigned int address,unsigned char *data,unsigned int size)
{
   unsigned int offset,flashAddr,createFile=0;
   InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;
   switch (sectionType)
   {
      case INU_STORAGE_SECTION_BOOTFIX:
		 offset = address;
		 flashAddr = 0;
      break;
      case INU_STORAGE_SECTION_PRODUCTION_DATA:
         offset = sizeof(InuSectionHeaderT);
         flashAddr = address - sizeof(InuSectionHeaderT);
       break;
      case INU_STORAGE_SECTION_CALIBRATION_DATA:
         flashAddr = address;
 		   offset = 0;
		  break;
	   default:
	      return INU_STORAGE_ERR_NOT_SUPPORTED;
      break;

   }
   if (storageSectorAddr[sectionType] == -1)
   {
      storageSectorAddr[sectionType] = flashAddr;
      createFile=1;
   }
   ret = inu_storage_copyBuffToFile(sectionType,createFile,offset,data,size);
   return ret;
}

void inu_storage_find_last_block(unsigned int *blockAddress)
{

	findBlockAddress(INU_STORAGE_SECTION_CALIBRATION_DATA, blockAddress);
}

InuStorageErrorE find_new_section_size(InuStorageSectionTypeE sectionType, InuSectionDataT *sectionData, unsigned int *new_section_size, unsigned int inc_precent)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;
	InuSectionDataT oldInuSectionData;
	unsigned int oldSectionSize = 0, blockAddress, transferSize, maximum_size,last_block;
	InuSectionHeaderT inuSectionHeaderT;

	memset(&oldInuSectionData, 0, sizeof(oldInuSectionData));
	if (findBlockAddress(sectionType, &blockAddress) == INU_STORAGE_ERR_INIT_FAILED)
	{
        STORAGE_PRINT(INU_USB_LOG_ERROR, "Block address error\n");
		return INU_STORAGE_ERR_INIT_FAILED;
	}
	ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, blockAddress, (unsigned char *)&inuSectionHeaderT, sizeof(inuSectionHeaderT), &transferSize);
	if (ret != INU_STORAGE_ERR_SUCCESS)
	{
        STORAGE_PRINT(INU_USB_LOG_ERROR, "Read raw error\n");
		return INU_STORAGE_ERR_INIT_FAILED;
	}
	if (inuSectionHeaderT.sectionSize < 1024)
		oldSectionSize = inuSectionHeaderT.sectionSize;
	maximum_size = (sectionData->sectionDataSize* (100 + inc_precent)) / 100;
	*new_section_size = maximum_size / flashInfo.blockSize;

	if (maximum_size % flashInfo.blockSize)
		*new_section_size += 1;

	if ((oldSectionSize < *new_section_size) && (oldSectionSize != 0) && (sectionType != INU_STORAGE_SECTION_APP))
	{
	    STORAGE_PRINT(INU_USB_LOG_ERROR, "No memory\n");
		return INU_STORAGE_ERR_NO_MEMORY;
	}
	// find calibration offset to test if we are overlapping this offset
	inu_storage_find_last_block(&last_block);
	if ((blockAddress + maximum_size) > last_block)
	{
	    STORAGE_PRINT(INU_USB_LOG_ERROR, "No memory\n");
		return INU_STORAGE_ERR_NO_MEMORY;
	}
	if (oldSectionSize != 0)
		*new_section_size = oldSectionSize;

	return ret;
}
void inu_storage_last_app_block(unsigned int *blockAddress)
{
	unsigned int ret,transferSize;
	InuSectionHeaderT inuSectionHeaderT;
	*blockAddress = sectionBootsplStartBlock * flashInfo.blockSize;
	unsigned int ind;

	for (ind=INU_STORAGE_SECTION_BOOTSPL;ind<(INU_STORAGE_SECTION_APP+1);ind++)
	{
	   ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, *blockAddress, (unsigned char *)&inuSectionHeaderT, sizeof(inuSectionHeaderT), &transferSize);
	   if ((ret != INU_STORAGE_ERR_SUCCESS) || (storageMagicNumber[ind] != inuSectionHeaderT.magicNumber))
	      return;
   	   *blockAddress += inuSectionHeaderT.sectionSize*flashInfo.blockSize;
	}
}

INUSTORAGE_API
InuStorageErrorE inu_storage_write_section_data(InuStorageSectionTypeE sectionType, InuSectionDataT *sectionData)
{
   InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;
   InuSectionHeaderT inuSectionHeaderT;
   unsigned int num_blocks,last_app_block;


   unsigned int blockAddress,hdrAddress;
   if (!dryRunMode)
      if (inu_storage_init(INU_STORAGE_SPI_FLASH) != INU_STORAGE_ERR_SUCCESS)
         return INU_STORAGE_ERR_INIT_FAILED;


   memset(&inuSectionHeaderT, 0, sizeof(InuSectionHeaderT));
   if (findBlockAddress(sectionType, &blockAddress) == INU_STORAGE_ERR_INIT_FAILED)
   {
      STORAGE_PRINT(INU_USB_LOG_ERROR,"Block address error\n");
      return INU_STORAGE_ERR_INIT_FAILED;
   }
   if ((sectionData == NULL) || (sectionData->sectionDataBuffer == NULL))
   {
      return INU_STORAGE_ERR_INIT_FAILED;
   }
   hdrAddress = blockAddress;
   inuSectionHeaderT.sectionDataSize = sectionData->sectionDataSize;
   inuSectionHeaderT.sectionFormat = sectionData->sectionDataFormat;
   inuSectionHeaderT.timestamp = sectionData->timestamp;
   strncpy(inuSectionHeaderT.versionString, sectionData->versionString, VERSION_STRING_SIZE - 1);
   inuSectionHeaderT.magicNumber = storageMagicNumber[sectionType];
   reset_storage_magic();
   switch (sectionType)
   {

   case INU_STORAGE_SECTION_BOOTFIX:

      inuSectionHeaderT.sectionSize = SECTION_BOOTFIX_SIZE_IN_BLOCKS;

      ret = inu_flash_write_bootfix_loader(sectionData->sectionDataBuffer, sectionData->sectionDataSize);
      hdrAddress = BOOTFIX_DATA_SIZE;

      break;
   case INU_STORAGE_SECTION_PRODUCTION_DATA:
      inuSectionHeaderT.sectionSize = SECTION_PRODUCTION_SIZE_IN_BLOCKS;
      inu_flash_write_production_data((InuProductionDataT *)sectionData->sectionDataBuffer, blockAddress + sizeof(inuSectionHeaderT));

      break;
   case INU_STORAGE_SECTION_BOOTFIX_METADATA:

      inuSectionHeaderT.sectionSize = SECTION_BOOTFIX_META_SIZE_IN_BLOCKS;
      if (sectionData->sectionDataBuffer != NULL)
         inu_flash_write_bootfix_metadata((InuBootfixHeaderT *)sectionData->sectionDataBuffer, sectionData->sectionDataSize, blockAddress + sizeof(inuSectionHeaderT));

      break;
   case INU_STORAGE_SECTION_CALIBRATION_DATA:
      num_blocks = (sectionData->sectionDataSize + sizeof(InuSectionHeaderT)) / flashInfo.blockSize;
      if ((sectionData->sectionDataSize + sizeof(InuSectionHeaderT)) % flashInfo.blockSize)
         num_blocks++;
      inuSectionHeaderT.sectionSize = num_blocks;
      blockAddress = (blockAddress - num_blocks) * flashInfo.blockSize;
      inu_storage_last_app_block(&last_app_block);
        // one block always saved for calibration check if calibration overlapping application
      // always save one block space for calibration
      if ((last_app_block > blockAddress) && (num_blocks>1))
      {
         STORAGE_PRINT(INU_USB_LOG_ERROR,"NO memory\n");
         return INU_STORAGE_ERR_NO_MEMORY;
      }
      inu_flash_write_file_data(sectionData->sectionDataBuffer, sectionData->sectionDataSize, blockAddress);
      if (dryRunMode)
      {
         inu_storage_write_dry_run_data(INU_STORAGE_SECTION_CALIBRATION_DATA,blockAddress,sectionData->sectionDataBuffer, sectionData->sectionDataSize);
      }

      hdrAddress = (flashInfo.numOfBlocks * flashInfo.blockSize) - sizeof(InuSectionHeaderT); // header at the end block
      break;

   case INU_STORAGE_SECTION_DYNAMIC_CALIBRATION:
      num_blocks = (sectionData->sectionDataSize + sizeof(InuSectionHeaderT)) / flashInfo.blockSize;
      if ((sectionData->sectionDataSize + sizeof(InuSectionHeaderT)) % flashInfo.blockSize)
         num_blocks++;
      inuSectionHeaderT.sectionSize = num_blocks;
      blockAddress = (blockAddress/flashInfo.blockSize - num_blocks + 1) * flashInfo.blockSize;


      inu_storage_last_app_block(&last_app_block);
        // one block always saved for calibration check if calibration overlapping application
      // always save one block space for calibration
      if ((last_app_block > blockAddress) && (num_blocks>1))
      {
         STORAGE_PRINT(INU_USB_LOG_ERROR,"NO memory\n");
         return INU_STORAGE_ERR_NO_MEMORY;
      }
      inu_flash_write_file_data(sectionData->sectionDataBuffer, sectionData->sectionDataSize, blockAddress);
      if (dryRunMode)
      {
         inu_storage_write_dry_run_data(INU_STORAGE_SECTION_CALIBRATION_DATA,blockAddress,sectionData->sectionDataBuffer, sectionData->sectionDataSize);
      }

      hdrAddress = (blockAddress + flashInfo.blockSize * num_blocks) - sizeof(InuSectionHeaderT);
      break;
   case INU_STORAGE_SECTION_BOOTSPL:
   case INU_STORAGE_SECTION_DTB:
   case INU_STORAGE_SECTION_KERNEL:
   case INU_STORAGE_SECTION_ROOTFS:
   case INU_STORAGE_SECTION_CEVA:
   case INU_STORAGE_SECTION_CNN_LD:
   case INU_STORAGE_SECTION_CNN:
   case INU_STORAGE_SECTION_APP:
      if (dryRunMode)
         return INU_STORAGE_ERR_NOT_SUPPORTED;
      ret = find_new_section_size(sectionType, sectionData, &inuSectionHeaderT.sectionSize, storageAutoInc[sectionType]);
      if (ret != INU_STORAGE_ERR_SUCCESS)
         return ret;
      inu_flash_write_file_data(sectionData->sectionDataBuffer, sectionData->sectionDataSize, blockAddress + sizeof(inuSectionHeaderT));

      break;
   default:
      return INU_STORAGE_ERR_IO_FAILED;

      break;

   }
   if (ret == INU_STORAGE_ERR_SUCCESS)
   {

      ret = inu_storage_write_raw(INU_STORAGE_SPI_FLASH, hdrAddress, (unsigned char *)&inuSectionHeaderT, sizeof(inuSectionHeaderT));
      if (dryRunMode)
         ret = inu_storage_write_dry_run_hdr(sectionType,hdrAddress,&inuSectionHeaderT);
   }
   return ret;
}

INUSTORAGE_API
InuStorageErrorE inu_storage_erase_section(InuStorageSectionTypeE sectionType, int softErase)
{
	unsigned int blockAddress, transferSize, sizeInBlocks, hdrAddress;
	InuSectionHeaderT inuSectionHeaderT;
	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;

	if (findBlockAddress(sectionType, &blockAddress) == INU_STORAGE_ERR_INIT_FAILED)
		return INU_STORAGE_ERR_INIT_FAILED;
	switch (sectionType)
	{

	case INU_STORAGE_SECTION_BOOTFIX:
		sizeInBlocks = SECTION_BOOTFIX_SIZE_IN_BLOCKS;
		break;
	case INU_STORAGE_SECTION_BOOTFIX_METADATA:
		sizeInBlocks = SECTION_BOOTFIX_META_SIZE_IN_BLOCKS;
		break;
	case INU_STORAGE_SECTION_PRODUCTION_DATA:
		sizeInBlocks = SECTION_PRODUCTION_SIZE_IN_BLOCKS;
		break;
	case INU_STORAGE_SECTION_DTB:
		sizeInBlocks = SECTION_DTB_SIZE_IN_BLOCKS;
		break;
	case INU_STORAGE_SECTION_CALIBRATION_DATA:
		sizeInBlocks = (flashInfo.numOfBlocks*flashInfo.blockSize - blockAddress)/flashInfo.blockSize;
		break;
	case INU_STORAGE_SECTION_DYNAMIC_CALIBRATION:
		hdrAddress = blockAddress + flashInfo.blockSize - sizeof(inuSectionHeaderT);
		ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, hdrAddress, (unsigned char*)&inuSectionHeaderT, sizeof(inuSectionHeaderT), &transferSize);
		if (ret == INU_STORAGE_ERR_SUCCESS)
		{
			if (storageMagicNumber[INU_STORAGE_SECTION_DYNAMIC_CALIBRATION] == inuSectionHeaderT.magicNumber)
			{
			   blockAddress = (blockAddress - (inuSectionHeaderT.sectionSize - 1) * flashInfo.blockSize);
			   sizeInBlocks = inuSectionHeaderT.sectionSize;
			}
			else
			{
				sizeInBlocks = 1;
			};
		}

		break;
	case INU_STORAGE_SECTION_BOOTSPL:
	case INU_STORAGE_SECTION_KERNEL:
   case INU_STORAGE_SECTION_ROOTFS:
   case INU_STORAGE_SECTION_CEVA:
   case INU_STORAGE_SECTION_CNN_LD:
   case INU_STORAGE_SECTION_CNN:
	case INU_STORAGE_SECTION_APP:
		ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH, blockAddress, (unsigned char *)&inuSectionHeaderT, sizeof(inuSectionHeaderT), &transferSize);
		sizeInBlocks = inuSectionHeaderT.sectionSize;
		if ((ret != INU_STORAGE_ERR_SUCCESS) || (inuSectionHeaderT.sectionSize > flashInfo.numOfBlocks))
			return INU_STORAGE_ERR_INIT_FAILED;
		break;
	default:
		ret = INU_STORAGE_ERR_INIT_FAILED;
		break;

	}
	if (ret != INU_STORAGE_ERR_INIT_FAILED)
	{
		inu_storage_erase(INU_STORAGE_SPI_FLASH, blockAddress, sizeInBlocks*flashInfo.blockSize);
		printf("ERASE section %x offset %x size %x\n", sectionType, blockAddress, sizeInBlocks);

	}
	return ret;
}

/****************************************************************************
*
*  Function Name: inu_storage_write_nand_data
*
*  Description: Write directly to secondary flash
*
*  Inputs: inuFlashData pointer to data/size/offset in flash
*
*
*  Returns: InuStorageErrorE error code
*
****************************************************************************/

INUSTORAGE_API
InuStorageErrorE inu_storage_write_nand_data(InuNandDataT *inuNandData)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_NO_SECOND_FLASH;

	ret = inu_storage_write_raw(INU_STORAGE_NAND, inuNandData->dataOffset, inuNandData->dataBuffer, inuNandData->dataSize);
	return ret;
}

/****************************************************************************
*
*  Function Name: inu_storageread_data
*
*  Description: Read directly from flash
*
*  Inputs: inuFlashData pointer to data/size/offset in flash
*
*
*  Returns: InuStorageErrorE error code
*
****************************************************************************/

INUSTORAGE_API
InuStorageErrorE inu_storage_read_nand_data(InuNandDataT *inuNandData)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;
	unsigned int transferSize;

	ret = inu_storage_read_raw(INU_STORAGE_NAND, inuNandData->dataOffset, inuNandData->dataBuffer, inuNandData->dataSize, &transferSize);
	return ret;
}

/****************************************************************************
*
*  Function Name: inu_storageread_data
*
*  Description: Read directly from flash
*
*  Inputs: inuFlashData pointer to data/size/offset in flash
*
*
*  Returns: InuStorageErrorE error code
*
****************************************************************************/

INUSTORAGE_API
InuStorageErrorE inu_storage_erase_nand_data(InuNandDataT *inuNandData)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;

	inuNandData->dataSize = ((inuNandData->dataSize % NAND_BLOCK_SIZE) == 0) ? (inuNandData->dataSize) : 
		(((inuNandData->dataSize / NAND_BLOCK_SIZE) + 1) * NAND_BLOCK_SIZE);

	ret = inu_storage_erase(INU_STORAGE_NAND, inuNandData->dataOffset, inuNandData->dataSize);
	return ret;
}

/****************************************************************************
*
*  Function Name: inu_storage_get_nand_offset
*
*  Description: returns partition/sector offset
*
*  Inputs:  partition sector
*
*  Outputs:  offset
*
*  Returns:
*
*  Context: read flash
*
****************************************************************************/
InuStorageNandHeaderT *header;
INUSTORAGE_API
InuStorageErrorE inu_storage_update_nand_image(unsigned int partition, unsigned int section, InuNandDataT *inuNandData)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;
	
	InuNandDataT header_io;
	unsigned int i, partition_off, section_off, sect_size, section_found = 0, busy_loop = 0xFFFFFFFF;
	
	ret = inu_storage_get_nand_partition_offset(partition, &partition_off);
	ret = inu_storage_get_nand_section_offset_size((InuStorageNandSectionTypE)section, &section_off, &sect_size);
	inuNandData->dataOffset = partition_off + section_off;

	header = (InuStorageNandHeaderT *)malloc(sizeof(InuStorageNandHeaderT));
	header_io.dataBuffer = (unsigned char *)header;
	header_io.dataSize = sizeof(InuStorageNandHeaderT);
	ret = inu_storage_get_nand_section_offset_size(NAND_SECTION_TYPE_HEADER, &section_off, &sect_size);
	header_io.dataOffset = partition_off + section_off;
	ret = inu_storage_read_nand_data(&header_io);

	if (header->magicNumber != NAND_HEADER_MAGIC_NUMBER)
	{
		inu_storage_init_nand_header(header);
	}

	for (i = 0; i < NAND_SECTION_MAX_SECTIONS; i++)
	{
		if (header->section[i].type == section)
		{
			printf( "Found Update request for section %d, type %d.\n", i, section);
			section_found = 1;
			break;
		}
	}

	if (section_found == 0)
	{
		printf("Did no found Update request for section %d, type %d.\n", i, section);
		return INU_STORAGE_ERR_NOT_SUPPORTED;
	}

	if ((header->section[i].imgStartAddress != NAND_EMPTY_UINT32) &&
		(header->section[i].imgSizeBytes != NAND_EMPTY_UINT32))			// Section was already written
	{
		printf("Section %d already written. Erasing section.\n", i);

		ret = inu_storage_erase_nand_data(inuNandData);

		header->section[i].imgStartAddress = NAND_EMPTY_UINT32;
		header->section[i].imgSizeBytes = NAND_EMPTY_UINT32;

		header->section[i].imgVersion.major = NAND_EMPTY_UINT32;
		header->section[i].imgVersion.minor = NAND_EMPTY_UINT32;
		header->section[i].imgVersion.build = NAND_EMPTY_UINT32;
	}

	ret = inu_storage_write_nand_data(inuNandData);

	ret = inu_storage_erase_nand_data(&header_io);
	// Update version into header.section[i].imgVersion
	header->section[i].imgStartAddress = header->section[i].startAddress;
	header->section[i].imgSizeBytes = inuNandData->dataSize;

	header_io.dataBuffer = (unsigned char *)header;
	header_io.dataSize = sizeof(InuStorageNandHeaderT);

	ret = inu_storage_write_nand_data(&header_io);
	printf("inu_storage_write_nand_data: after\n");
	free(header);
	return ret;
}

/****************************************************************************
*
*  Function Name: inu_storage_write_data
*
*  Description: Write directly to secondary flash
*
*  Inputs: inuFlashData pointer to data/size/offset in flash 
*          
*
*  Returns: InuStorageErrorE error code
*
****************************************************************************/

INUSTORAGE_API
InuStorageErrorE inu_storage_write_data(InuFlashDataT *inuFlashData)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_NO_SECOND_FLASH;
	
	if(secondary_flash)
		ret = inu_flash_write_file_data(inuFlashData->dataBuffer, inuFlashData->dataSize, inuFlashData->dataOffset);
	return ret;
}

/****************************************************************************
*
*  Function Name: inu_storageread_data
*
*  Description: Read directly from flash
*
*  Inputs: inuFlashData pointer to data/size/offset in flash 
*          
*
*  Returns: InuStorageErrorE error code
*
****************************************************************************/

INUSTORAGE_API
InuStorageErrorE inu_storage_read_data(InuFlashDataT *inuFlashData)
{
	InuStorageErrorE ret = INU_STORAGE_ERR_SUCCESS;
	unsigned int transferSize;
	
	ret = inu_storage_read_raw(INU_STORAGE_SPI_FLASH,inuFlashData->dataOffset , inuFlashData->dataBuffer, inuFlashData->dataSize, &transferSize);
	return ret;
}
/****************************************************************************
*
*  Function Name: inu_storage_open
*
*  Description: Wait for USB insertion event and opens USB link
*
*  Inputs: SpiParamsT spiParams
*          second_flash -    use secondary flash google X
*          outputDir         to copy partitions from flash
*          flashSizeInMega
*          currChipId  -     ex. 0x40000001 for b0
*  Returns: InuStorageErrorE error code
*
****************************************************************************/
INUSTORAGE_API
InuStorageErrorE inu_storage_open(SpiParamsT spiParams,
	                              unsigned int secondFlash,
	                              char         *outputDir,
	                              unsigned int flashSizeInMega,
	                              unsigned int currChipId)
{
   memset(&inuParams, sizeof(inuParams), 0);
   inuParams.hp_callback = hotplug_callback;
   inuParams.logLevel = INU_USB_LOG_ERROR;
   inuParams.filterId = - 1;
   secondary_flash = secondFlash;
   spiGParams = spiParams;
   if (outputDir != NULL)
   {
      strncpy(dryRunOutputDir,outputDir,BOOT_PATH_LENGTH_MAX -1);
      dryRunMode = 1;
      flashInfo.numOfBlocks = (flashSizeInMega*1024)/(flashInfo.blockSize/1024);
   }
   else  dryRunMode = 0;
   if (!dryRunMode)
   {
      if (inu_usb_init(&inuParams) != INU_USB_ERR_SUCCESS)
      {
            return INU_STORAGE_ERR_INIT_FAILED;
      }
      while (usbConnected == 0)
         host_millisleep(500);
   }
   boardChipId = currChipId;
   return INU_STORAGE_ERR_SUCCESS;
}

INUSTORAGE_API
InuStorageErrorE inu_storage_set_version(unsigned int flashChipVersion)
{
	overwriteChipVersion = flashChipVersion;
	return INU_STORAGE_ERR_SUCCESS;
}

/****************************************************************************
*
*  Function Name: inu_storage_close
*
*  Description: Close USB interface
*
*  Inputs: None
*
*  Returns: InuStorageErrorE error code
*
****************************************************************************/
INUSTORAGE_API
InuStorageErrorE inu_storage_close()
{
   if (!dryRunMode)
   {
     inu_close_interface();
     inu_usb_deinit();
     usbConnected = 0;
   }
   return INU_STORAGE_ERR_SUCCESS;
}

INUSTORAGE_API
InuStorageErrorE inu_storage_get_chip_version(unsigned int *chipVersionPtr)
{
	STORAGE_PRINT(INU_USB_LOG_INFO, "chipVersion 0x%x\n", chipVersion);
	if (inu_storage_init(INU_STORAGE_SPI_FLASH) != INU_STORAGE_ERR_SUCCESS)
		return INU_STORAGE_ERR_INIT_FAILED;
	*chipVersionPtr = chipVersion;
	return INU_STORAGE_ERR_SUCCESS;
}


INUSTORAGE_API
InuStorageErrorE inu_storage_resize_section(InuStorageSectionTypeE sectionType, unsigned int size)
{
	return INU_STORAGE_ERR_SUCCESS;
}

#define MAX_FILE_SIZE 0x100000
unsigned char tmpBuff[MAX_FILE_SIZE];

INUSTORAGE_API
InuStorageErrorE inu_storage_get_components(InuComponentDataT *componentsP,const char *boot_path)
{     
	ComponentsErrorE err;
	ComponentsParamsT componentsParams;
	InuSectionDataT sectionData;
	InuProductionDataT *productionDataP;
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();

	memset(tmpBuff, 0, sizeof(tmpBuff));
	sectionData.sectionDataBuffer = tmpBuff;
	sectionData.sectionDataSize = MAX_FILE_SIZE;
	inu_storage_read_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, &sectionData);
	productionDataP = (InuProductionDataT*)(sectionData.sectionDataBuffer);

	STORAGE_PRINT(INU_USB_LOG_INFO, "chipVersion 0x%x, bootpath %s, bootid %d\n", chipVersion, boot_path, productionDataP->bootId);

	err = get_components(&componentsParams, productionDataP->bootId,boot_path,chipVersion);

	if (err < 0)
	{
		STORAGE_PRINT(INU_USB_LOG_ERROR, "error getting components! %d\n", err);
	}
	else
	{
		memcpy(componentsP->linux_file_name,componentsParams.linux_file_name,sizeof(char)*BOOT_PATH_LENGTH_MAX);
		memcpy(componentsP->dtb_file_name,componentsParams.dtb_file_name,sizeof(char)*BOOT_PATH_LENGTH_MAX);
		memcpy(componentsP->spl_file_name,componentsParams.spl_file_name,sizeof(char)*BOOT_PATH_LENGTH_MAX);
		memcpy(componentsP->app_file_name,componentsParams.app_file_name,sizeof(char)*BOOT_PATH_LENGTH_MAX);
		memcpy(componentsP->initramfs_file_name, componentsParams.initramfs_file_name, sizeof(char)*BOOT_PATH_LENGTH_MAX);
	}

	return (InuStorageErrorE)err;
}


