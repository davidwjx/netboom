/* This code performs board_end() and prints some debug prints */
#include "common.h"
#include "nu4000_b0_gme_regs.h"
#include "usb_drv.h"

#include "board.h"
#include "spl.h"
#include "inu_spl_info.h"
#include "inu_storage_layout.h"
#include "inu_storage.h"
#include "storage.h"
//#define FPGA_BOARD



//#define LINUX_OS

#ifdef LINUX_OS
#include <sys/mman.h> 
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#endif

#include <stdio.h>


//typedef unsigned int        UINT32;

typedef enum {
    SUCCESS_E = 0,
    FAIL_E    = 1
} RETURN_ENUM;

typedef struct
{
	UINT32 *startMemBlockP;
	UINT32 memBlockSize;
	UINT32 memBlockDataAccess;
} CHIP_MEMP_memBlockInfoT;

CHIP_MEMP_memBlockInfoT	memBlockInfo;

//#define MEM_TESTP_DEFAULT_START_ADR  (0x81900000) - DDR
#define MEM_TESTP_DEFAULT_START_ADR  (0x80000000) //lram
#define MEM_TESTP_DEFAULT_BLOCK_SIZE (0x100)
#define MEM_TESTP_DEFAULT_NUM_ITER   (1)
#define MEM_TESTP_DEFAULT_TEST_TYPE  (6)

UINT32 numIteration = MEM_TESTP_DEFAULT_NUM_ITER;
UINT32 testType     = MEM_TESTP_DEFAULT_TEST_TYPE;

void MEM_TESTP_printParams()
{
   rel_log("Start address = %x\n",memBlockInfo.startMemBlockP);
   rel_log("Block size = %d\n",memBlockInfo.memBlockSize);
   rel_log("Number of iterations = %d\n",numIteration);
   rel_log("Test type = %d\n",testType);
}

RETURN_ENUM MEM_TESTG_dataBusTest(volatile UINT32 *testAddressP)
{
	RETURN_ENUM retcode = SUCCESS_E;

	UINT32 pattern;
	UINT32 antiPattern;
	UINT32 readValue;

	//rel_log("before test:addr = %x, val = %x\n",(UINT32)testAddressP,*testAddressP);


	for (pattern = 1; pattern != 0 ; pattern <<=1 )
	{
		*testAddressP = pattern;
		readValue = *testAddressP;
		if (readValue != pattern)
      {
			rel_log("XXXXXXX MEM_TESTG_dataBusTest error: addressP = %x write value is %x read value is %x\n", testAddressP, pattern, readValue);
			retcode = (FAIL_E);
		}

		antiPattern = ~pattern;
		*testAddressP = antiPattern;

		readValue = *testAddressP;
		if (readValue != antiPattern)
      {
			rel_log("XXXXXXX MEM_TESTG_dataBusTest error: addressP = %x write value is %x read value is %x\n", testAddressP, antiPattern, readValue);
			retcode = (FAIL_E);
		}
	}

	return (retcode);
}

RETURN_ENUM MEM_TESTG_dataBusTestBlock(CHIP_MEMP_memBlockInfoT *memBlockInfo)
{
	RETURN_ENUM retcode = SUCCESS_E;
	UINT32 *startMemBlockP;
	UINT32 *lastMemBlockP;
	volatile UINT32 *testAddress;

	startMemBlockP = memBlockInfo->startMemBlockP;
	lastMemBlockP = startMemBlockP + ( memBlockInfo->memBlockSize/memBlockInfo->memBlockDataAccess ) - 1;

	for (testAddress = startMemBlockP; testAddress <= lastMemBlockP ; testAddress++)
	{
		retcode = MEM_TESTG_dataBusTest(testAddress);
	}

	rel_log("XXXXXXX MEM_TESTG_dataBusTestBlock test done\n");

	return (retcode);
}

RETURN_ENUM MEM_TESTG_addressBusTestBlock(CHIP_MEMP_memBlockInfoT *memBlockInfo)
{
	RETURN_ENUM retcode = SUCCESS_E;
	UINT32 *startMemBlockP;
	UINT32 *lastMemBlockP;
	volatile UINT32 *testAddressP;
	UINT32 pattern;
	UINT32 antiPattern;
	UINT32 readValue;

	startMemBlockP = memBlockInfo->startMemBlockP;
	lastMemBlockP = startMemBlockP + ( memBlockInfo->memBlockSize/memBlockInfo->memBlockDataAccess ) - 1;

	pattern = 0x55555555;
	antiPattern = ~pattern;
	for (testAddressP = startMemBlockP; testAddressP <= lastMemBlockP ; testAddressP++)
	{
		//rel_log("XXXXXXX MEM_TESTG_addressBusTestBlock 0x55555555 Test: testAddress = %x write value is %x\n", testAddressP, pattern);
		*testAddressP = pattern;
	}

	*startMemBlockP = antiPattern;
	*lastMemBlockP = antiPattern;
	for (testAddressP = (startMemBlockP + 1); testAddressP <= (lastMemBlockP - 1) ; testAddressP++)
	{
		readValue = *testAddressP;
		if ( readValue != pattern )
		{
			rel_log("XXXXXXX MEM_TESTG_addressBusTestBlock 0x55555555 Test error: testAddress = %x write value is %x read value is %x\n", testAddressP, pattern, readValue);
			retcode = (FAIL_E);
		}
	}

	pattern = 0xAAAAAAAA;
	antiPattern = ~pattern;
	for (testAddressP = startMemBlockP; testAddressP <= lastMemBlockP ; testAddressP++)
	{
		*testAddressP = pattern;
	}

	*startMemBlockP = antiPattern;
	*lastMemBlockP = antiPattern;
	for (testAddressP = (startMemBlockP + 1); testAddressP <= (lastMemBlockP - 1) ; testAddressP++)
	{
		readValue = *testAddressP;
		if ( readValue != pattern )
		{
			rel_log("XXXXXXX MEM_TESTG_addressBusTestBlock 0xAAAAAAAA Test error: testAddress = %x write value is %x read value is %x\n", testAddressP, pattern, readValue);
			retcode = (FAIL_E);
		}
	}

	rel_log("XXXXXXX MEM_TESTG_addressBusTestBlock test done\n");

	return (retcode);
}


RETURN_ENUM MEM_TESTG_memTestBlock(CHIP_MEMP_memBlockInfoT *memBlockInfo)
{
	RETURN_ENUM retcode = SUCCESS_E;

	UINT32 *startMemBlockP;
	UINT32 *lastMemBlockP;
	volatile UINT32 *testAddressP;
	UINT32 pattern;
	UINT32 antiPattern;
	UINT32 readValue;

	startMemBlockP = memBlockInfo->startMemBlockP;
	lastMemBlockP = startMemBlockP + ( memBlockInfo->memBlockSize/memBlockInfo->memBlockDataAccess ) - 1;

	pattern = 0x03020100;
	for (testAddressP = startMemBlockP; testAddressP <= lastMemBlockP ; testAddressP++)
	{
		*testAddressP = pattern;
		pattern += 0x01010101;
	}

	pattern = 0x03020100;
	for (testAddressP = startMemBlockP; testAddressP <= lastMemBlockP ; testAddressP++)
	{
		readValue = *testAddressP;
		if ( readValue != pattern )
		{
			rel_log("XXXXXXX MEM_TESTG_memTestBlock counter Test error: testAddress = %x write value is %x read value is %x\n", testAddressP, pattern, readValue);
			retcode = (FAIL_E);
		}
		antiPattern = ~pattern;
		*testAddressP = antiPattern; 
		pattern += 0x01010101;
	}

	pattern = 0x03020100;
	for (testAddressP = startMemBlockP; testAddressP <= lastMemBlockP ; testAddressP++)
	{
		antiPattern = ~pattern;
		readValue = *testAddressP;
		if ( readValue != antiPattern )
		{
			rel_log("XXXXXXX MEM_TESTG_memTestBlock anti counter Test  error: testAddress = %x write value is %x read value is %x\n", testAddressP, antiPattern, readValue);
			retcode = (FAIL_E);
		}
		pattern += 0x01010101;
	}

	rel_log("XXXXXXX MEM_TESTG_memTestBlock test done\n");

	return (retcode);
}


RETURN_ENUM MEM_TESTG_altOnesTestBlock(CHIP_MEMP_memBlockInfoT *memBlockInfo)
{
	RETURN_ENUM retcode = SUCCESS_E;

	UINT32 *startMemBlockP;
	UINT32 *lastMemBlockP;
	volatile UINT32 *testAddressP;
	UINT32 pattern;
	UINT32 readValue;

	startMemBlockP = memBlockInfo->startMemBlockP;
	lastMemBlockP = startMemBlockP + ( memBlockInfo->memBlockSize/memBlockInfo->memBlockDataAccess ) - 1;

	pattern = 0xFFFFFFFF;
	for (testAddressP = startMemBlockP; testAddressP <= lastMemBlockP ; testAddressP++)
	{
		*testAddressP = pattern;
		pattern ^= 0xFFFFFFFF;
	}

	pattern = 0xFFFFFFFF;
	for (testAddressP = startMemBlockP; testAddressP <= lastMemBlockP ; testAddressP++)
	{
		readValue = *testAddressP;
		if ( readValue != pattern )
		{
			rel_log("XXXXXXX MEM_TESTG_altOnesTestBlock Test error: testAddress = %x write value is %x read value is %x\n", testAddressP, pattern, readValue);
			retcode = (FAIL_E);
		}
		pattern ^= 0xFFFFFFFF;
	}

	rel_log("XXXXXXX MEM_TESTG_altOnesTestBlock test done\n");

	return (retcode);
}


static inline UINT32 MEM_TESTG_generateSeed(void)
{
  UINT32 val = 1;
#ifdef LINUX_OS
  val = random();
#endif
  return val;
}


static UINT32 MEM_TESTG_randomReg32 (UINT32 val)
{
   val = ((((val >> 31)  /*Shift the 32nd bit to the first bit*/
              ^ (val >> 6)    /*XOR it with the seventh bit*/
              ^ (val >> 4)    /*XOR it with the fifth bit*/
              ^ (val >> 2)    /*XOR it with the third bit*/
              ^ (val >> 1)    /*XOR it with the second bit*/
              ^ val)          /*and XOR it with the first bit.*/
              & 0x0000001)         /*Strip all the other bits off and*/
              <<31)                /*move it back to the 32nd bit.*/
              | (val >> 1);   /*Or with the register shifted right.*/

   return val;
}

RETURN_ENUM MEM_TESTG_randomBlock (CHIP_MEMP_memBlockInfoT *memBlockInfo)
{
	RETURN_ENUM retcode = SUCCESS_E;

	UINT32 *startMemBlockP;
	UINT32 *lastMemBlockP;
	volatile UINT32 *testAddressP;
	UINT32 seed,runningSeed;
	UINT32 readValue;

	startMemBlockP = memBlockInfo->startMemBlockP;
	lastMemBlockP = startMemBlockP + ( memBlockInfo->memBlockSize/memBlockInfo->memBlockDataAccess ) - 1;

	seed = MEM_TESTG_generateSeed();

	runningSeed = seed;
	for (testAddressP = startMemBlockP; testAddressP <= lastMemBlockP ; testAddressP++)
	{
		runningSeed = MEM_TESTG_randomReg32(runningSeed);
		*testAddressP = runningSeed;
	}

	runningSeed = seed;
	for (testAddressP = startMemBlockP; testAddressP <= lastMemBlockP ; testAddressP++)
	{
		readValue = *testAddressP;
		runningSeed = MEM_TESTG_randomReg32(runningSeed);
		if ( readValue != runningSeed)
		{
			rel_log("XXXXXXX MEM_TESTG_randomBlock Test error: testAddress = %x write value is %x read value is %x\n", testAddressP, runningSeed, readValue);
			retcode = (FAIL_E);
		}
	}

	rel_log("XXXXXXX MEM_TESTG_randomBlock test done\n");

	return (retcode);
}

int MEM_TESTG_start(UINT32 addrs, UINT32 size)
{
	rel_log("MEM_TESTG_start: address = %x, size = %d\n",addrs,size);

	memBlockInfo.memBlockDataAccess = 4;
	memBlockInfo.startMemBlockP	  = (UINT32*)addrs;
	memBlockInfo.memBlockSize		  = size;

	MEM_TESTP_printParams();

	UINT32 iter;
	for (iter = 0; iter < numIteration; iter++)
	{
	   switch(testType)
	   {
			case(1):
				MEM_TESTG_addressBusTestBlock(&memBlockInfo);
				break;
			case(2):
				MEM_TESTG_altOnesTestBlock(&memBlockInfo);
            break;
			case(3):
				MEM_TESTG_dataBusTestBlock(&memBlockInfo);
				break;
			case(4):
				MEM_TESTG_memTestBlock(&memBlockInfo);
				break;
			case(5):
				MEM_TESTG_randomBlock(&memBlockInfo);
				break;

			case(6):
				MEM_TESTG_dataBusTestBlock(&memBlockInfo);
				MEM_TESTG_addressBusTestBlock(&memBlockInfo);
				MEM_TESTG_altOnesTestBlock(&memBlockInfo);
				MEM_TESTG_memTestBlock(&memBlockInfo);
				MEM_TESTG_randomBlock(&memBlockInfo);
				break;
			default:
	      	rel_log("Unknown test type given, exit\n");
				return 0;
      }
   }

	return 0;
}


RETURN_ENUM MEM_TESTG_simple(UINT32 addrs, UINT32 size)
{
	RETURN_ENUM retcode = SUCCESS_E;
	int i;
	unsigned int *ddr;
	rel_log("MEM_TESTG_simple: address = %x, size = %d\n", addrs, size);
	
	ddr = (unsigned int *)addrs;

	for (i=0; i< (size)/4; i++)
	{
		ddr[i]=i;
	}
	for (i=0; i< (size)/4; i++)	
	{
		if (ddr[i] != i) 
		{
			rel_log("error: addr = %x, val = %x\n", i, ddr[i]);
			retcode = FAIL_E;
		}
	}
	
	return (retcode);
}

