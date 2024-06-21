/* This code performs board_end() and prints some debug prints */
#include "common.h"
#include "nu4000_c0_gme_regs.h"
#include "nu4000_c0_ictl0_regs.h"
#include "nu4100_ipe_regs.h"
#include "nu4100_isp_0_regs.h"
#include "nu4100_isp_1_regs.h"

/**********************************************
 * local defines
***********************************************/
#define NULL 0

#define BASE_CSM 0x01000000
#define BYTES_CSM (1536*1024)
#define BASE_VMEM 0x01400000
#define BYTES_VMEM (512*1024)
#define BASE_IAE_DSR 0x04500000
#define BYTES_IAE_DSR (1755*1024)
#define BASE_LPDDR4 0x80000000
#define BYTES_LPDDR4 (1024*1024*1024)
#define DDR_TEST_SPLIT_NUM (8)


/**********************************************
 * functions
***********************************************/
unsigned int *memTestAddressBus(volatile unsigned int * baseAddress, unsigned long nBytes);
unsigned int memTestDataBus(volatile unsigned int * address);
unsigned int *memTestDevice(volatile unsigned int * baseAddress, unsigned long nBytes);

unsigned short *memTestAddressBus_s(volatile unsigned short * baseAddress, unsigned long nBytes);
unsigned short memTestDataBus_s(volatile unsigned short * address);
unsigned short *memTestDevice_s(volatile unsigned short * baseAddress, unsigned long nBytes);

unsigned int memTest(unsigned int addressG,  unsigned long nBytes);
unsigned int memTest_s(unsigned int addressG,  unsigned long nBytes);

void clocksInit(void);
void clocksDeinit(void);

unsigned int ib_dsr_lut_test (void);
int *memTestDevice_ib_dsr(unsigned int addressG, unsigned long nBytes, int divide, int modulo, int mask);

void ddr_pll_config(void);
void ddrm_umctl_init_seq(void);

/**********************************************
 * local defines
***********************************************/
#define BASE_ADDRESS (volatile unsigned int *) 0x00000000
#ifdef BOOT_500
//int mailbox_bits[4] = {3,4,5,6};

/**********************************************
 * functions
***********************************************/

void ISP_W (volatile unsigned int *address,int value)
{
   *address=value;
}

int ISP_R (volatile unsigned int *address)
{

   return *address;
}

void ipe_init()
{
   GME_POWER_MODE_IAE_POWER_UP_W(1);
   GME_POWER_MODE_EVP_POWER_UP_W(1);
   GME_CONTROL_START_POWER_CHANGE_W(1);

   while(!GME_POWER_MODE_STATUS_IAE_POWER_UP_R);
   while(!GME_POWER_MODE_STATUS_EVP_POWER_UP_R);
   GME_CONTROL_START_POWER_CHANGE_W(0);
   debug_printf("ISP: IAE & EVP power up\n");

   BOARD_setIaeClk(1);// The IAE CLK is 1 for the 1st GC
   debug_printf("ISP: IAE CLK Start\n");

   BOARD_setEVPClk(1);// The EVP CLK is 1 for the 2nd GC
   debug_printf("ISP: EVP CLK Start\n");
}



void ipe_isp_0_init() // Need to change the register header of IPE (not updated)
{
   IPE_ENABLE_ISP0_CLK_EN_W(1);
   debug_printf("ISP_0: ISP0 CLK EN\n");
}

void ipe_isp_1_init() // Need to change the register header of IPE (not updated)
{
   IPE_ENABLE_ISP1_CLK_EN_W(1);
   debug_printf("ISP_1: ISP1 CLK EN\n");
}

void isp_01_bypass_config(unsigned int isp_id)
{
   ISP_W((volatile unsigned int *)0x03010008, 0x8030C000);
   ISP_W((volatile unsigned int *)(0x03000000 + (isp_id * 0x00008000)), 0x00000000);
   ISP_W((volatile unsigned int *)(0x03000010 + (isp_id * 0x00008000)), 0x00019f7b);
   ISP_W((volatile unsigned int *)(0x03000018 + (isp_id * 0x00008000)), 0x0300700c);
   ISP_W((volatile unsigned int *)(0x03000c6c + (isp_id * 0x00008000)), 0x000000f6);
   ISP_W((volatile unsigned int *)(0x0300106c + (isp_id * 0x00008000)), 0x0000016e);
   ISP_W((volatile unsigned int *)(0x0300116c + (isp_id * 0x00008000)), 0x0000036e);
   ISP_W((volatile unsigned int *)(0x03001200 + (isp_id * 0x00008000)), 0x0000e000);
   ISP_W((volatile unsigned int *)(0x03001204 + (isp_id * 0x00008000)), 0x01000280);
   ISP_W((volatile unsigned int *)(0x03001208 + (isp_id * 0x00008000)), 0x01000280);
   ISP_W((volatile unsigned int *)(0x03001280 + (isp_id * 0x00008000)), 0x00000001);
   ISP_W((volatile unsigned int *)(0x03001284 + (isp_id * 0x00008000)), 0x006e2801);
   ISP_W((volatile unsigned int *)(0x03001288 + (isp_id * 0x00008000)), 0x00006009);
   ISP_W((volatile unsigned int *)(0x03001290 + (isp_id * 0x00008000)), 0x006e2801);
   ISP_W((volatile unsigned int *)(0x03001294 + (isp_id * 0x00008000)), 0x00006006);
   ISP_W((volatile unsigned int *)(0x03001300 + (isp_id * 0x00008000)), 0x00004001);
   ISP_W((volatile unsigned int *)(0x03001310 + (isp_id * 0x00008000)), 0x00000030);
   ISP_W((volatile unsigned int *)(0x03001314 + (isp_id * 0x00008000)), 0x00020128);
   ISP_W((volatile unsigned int *)(0x03001318 + (isp_id * 0x00008000)), 0x00000005);
   ISP_W((volatile unsigned int *)(0x0300131c + (isp_id * 0x00008000)), 0x03670155);
   ISP_W((volatile unsigned int *)(0x03001324 + (isp_id * 0x00008000)), 0x010A3000);
   ISP_W((volatile unsigned int *)(0x03001328 + (isp_id * 0x00008000)), 0x00030000);
   ISP_W((volatile unsigned int *)(0x0300132c + (isp_id * 0x00008000)), 0x00000000);
   ISP_W((volatile unsigned int *)(0x03001330 + (isp_id * 0x00008000)), 0x00000300);
   ISP_W((volatile unsigned int *)(0x03001334 + (isp_id * 0x00008000)), 0x00000280);
   ISP_W((volatile unsigned int *)(0x03001338 + (isp_id * 0x00008000)), 0x00000100);
   ISP_W((volatile unsigned int *)(0x0300133c + (isp_id * 0x00008000)), 0x00030000);
   ISP_W((volatile unsigned int *)(0x03001340 + (isp_id * 0x00008000)), 0x01041000);
   ISP_W((volatile unsigned int *)(0x03001344 + (isp_id * 0x00008000)), 0x00030000);
   ISP_W((volatile unsigned int *)(0x03001348 + (isp_id * 0x00008000)), 0x00000000);
   ISP_W((volatile unsigned int *)(0x0300134c + (isp_id * 0x00008000)), 0x01072000);
   ISP_W((volatile unsigned int *)(0x03001350 + (isp_id * 0x00008000)), 0x00030000);
   ISP_W((volatile unsigned int *)(0x03001354 + (isp_id * 0x00008000)), 0x00000000);
   ISP_W((volatile unsigned int *)(0x03001408 + (isp_id * 0x00008000)), 0x00000280);
   ISP_W((volatile unsigned int *)(0x03001600 + (isp_id * 0x00008000)), 0x0000001a);
   ISP_W((volatile unsigned int *)(0x03001604 + (isp_id * 0x00008000)), 0x00000111);
   ISP_W((volatile unsigned int *)(0x03001608 + (isp_id * 0x00008000)), 0x00000001);
   ISP_W((volatile unsigned int *)(0x0300160c + (isp_id * 0x00008000)), 0x0f940146);
   ISP_W((volatile unsigned int *)(0x03001610 + (isp_id * 0x00008000)), 0x00000001);
   ISP_W((volatile unsigned int *)(0x0300166c + (isp_id * 0x00008000)), 0x01000000);
   ISP_W((volatile unsigned int *)(0x03001670 + (isp_id * 0x00008000)), 0x00000280);
   ISP_W((volatile unsigned int *)(0x03001674 + (isp_id * 0x00008000)), 0x00000400);
   ISP_W((volatile unsigned int *)(0x03001678 + (isp_id * 0x00008000)), 0x00040000);
   ISP_W((volatile unsigned int *)(0x03001690 + (isp_id * 0x00008000)), 0x00000320);
   ISP_W((volatile unsigned int *)(0x030016c0 + (isp_id * 0x00008000)), 0x0fffffff);
   ISP_W((volatile unsigned int *)(0x030016c4 + (isp_id * 0x00008000)), 0x07ffffff);
   ISP_W((volatile unsigned int *)(0x030016e8 + (isp_id * 0x00008000)), 0x07ffffff);
   ISP_W((volatile unsigned int *)(0x03001700 + (isp_id * 0x00008000)), 0x0fae4b00);
   ISP_W((volatile unsigned int *)(0x03000400 + (isp_id * 0x00008000)), 0x00006007);
   ISP_W((volatile unsigned int *)(0x03000404 + (isp_id * 0x00008000)), 0xb0903078);
   ISP_W((volatile unsigned int *)(0x03000410 + (isp_id * 0x00008000)), 0x00000280);
   ISP_W((volatile unsigned int *)(0x03000414 + (isp_id * 0x00008000)), 0x00000100);
   ISP_W((volatile unsigned int *)(0x03000418 + (isp_id * 0x00008000)), 0x00000000);
   ISP_W((volatile unsigned int *)(0x0300059c + (isp_id * 0x00008000)), 0x00000280);
   ISP_W((volatile unsigned int *)(0x030005a0 + (isp_id * 0x00008000)), 0x00000100);
   ISP_W((volatile unsigned int *)(0x030005bc + (isp_id * 0x00008000)), 0x07ffff7f);
   ISP_W((volatile unsigned int *)(0x03002310 + (isp_id * 0x00008000)), 0x00000280);
   ISP_W((volatile unsigned int *)(0x03002314 + (isp_id * 0x00008000)), 0x00000100);
   ISP_W((volatile unsigned int *)(0x0300295c + (isp_id * 0x00008000)), 0x00000070);
   ISP_W((volatile unsigned int *)(0x03002c00 + (isp_id * 0x00008000)), 0x000001c8);
   ISP_W((volatile unsigned int *)(0x03002f00 + (isp_id * 0x00008000)), 0x00000000);
   ISP_W((volatile unsigned int *)(0x03003200 + (isp_id * 0x00008000)), 0x00000000);
   ISP_W((volatile unsigned int *)(0x03003e00 + (isp_id * 0x00008000)), 0xbf1e012b);
   ISP_W((volatile unsigned int *)(0x03003e60 + (isp_id * 0x00008000)), 0x00600280);
   ISP_W((volatile unsigned int *)(0x03005690 + (isp_id * 0x00008000)), 0x00000010);
   ISP_W((volatile unsigned int *)(0x030056d0 + (isp_id * 0x00008000)), 0x0000003f);
   ISP_W((volatile unsigned int *)(0x03001568 + (isp_id * 0x00008000)), 0x002d8000);
   ISP_W((volatile unsigned int *)(0x030015bc + (isp_id * 0x00008000)), 0x002d8000);
   ISP_W((volatile unsigned int *)(0x03005620 + (isp_id * 0x00008000)), 0x00328000);
   ISP_W((volatile unsigned int *)(0x03005660 + (isp_id * 0x00008000)), 0x00328000);
   ISP_W((volatile unsigned int *)(0x03000c00 + (isp_id * 0x00008000)), 0x00000680);
   ISP_W((volatile unsigned int *)(0x03001000 + (isp_id * 0x00008000)), 0x000006f0);
   ISP_W((volatile unsigned int *)(0x03001100 + (isp_id * 0x00008000)), 0x00000650);
   ISP_W((volatile unsigned int *)(0x03001310 + (isp_id * 0x00008000)), 0x0000003a);
   ISP_W((volatile unsigned int *)(0x0300142c + (isp_id * 0x00008000)), 0x0000003a);
   ISP_W((volatile unsigned int *)(0x030014e4 + (isp_id * 0x00008000)), 0x000003ba);
   ISP_W((volatile unsigned int *)(0x03001600 + (isp_id * 0x00008000)), 0x0000007f);
   ISP_W((volatile unsigned int *)(0x03003300 + (isp_id * 0x00008000)), 0x03464638);
   ISP_W((volatile unsigned int *)(0x03005600 + (isp_id * 0x00008000)), 0x0000033a);
   ISP_W((volatile unsigned int *)(0x03005700 + (isp_id * 0x00008000)), 0x0000003f);
   ISP_W((volatile unsigned int *)(0x03005300 + (isp_id * 0x00008000)), 0x00000ea8);
   ISP_W((volatile unsigned int *)(0x03000400 + (isp_id * 0x00008000)), 0x00006217);
   ISP_W((volatile unsigned int *)(0x03000700 + (isp_id * 0x00008000)), 0x00010130);
   ISP_W((volatile unsigned int *)(0x03001300 + (isp_id * 0x00008000)), 0x0000c001);
   ISP_W((volatile unsigned int *)(0x03003d60 + (isp_id * 0x00008000)), 0x00000000);

   debug_printf("ISP_%d: Configure ISP completed\n", isp_id);

}

void isp_01_interrupt(unsigned int isp_id)
{
   int R_ISP;
   while (!ICTL0_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_R); // ICTL polling
   if (isp_id == 0)
   {
      while (!IPE_IRQ_STATUS_ISP0_MAIN_R); // ISP int
      while (!ISP_0_ISP_MIS_MIS_FRAME_IN_R);
   }
   else
   {
      while (!IPE_IRQ_STATUS_ISP1_MAIN_R); // ISP int
      while (!ISP_1_ISP_MIS_MIS_FRAME_IN_R);
   }
   R_ISP = ISP_R((volatile unsigned int *)(0x030005c4 + (isp_id * 0x00008000)));
   debug_printf("ISP_%d: value in this reg : %08x\n", isp_id, R_ISP);

   while (!((R_ISP >> 1) & (0x1)))
   {
     R_ISP = ISP_R((volatile unsigned int *)(0x030005c4 + (isp_id * 0x00008000)));
     debug_printf("ISP_%d: value in this reg : %08x\n", isp_id, R_ISP);
   }
   debug_printf("ISP_%d: ISR completed\n", isp_id);
   ISP_W((volatile unsigned int *)(0x030005c8 + (isp_id * 0x00008000)), R_ISP); // Clear Interrupt
   R_ISP = ISP_R((volatile unsigned int *)(0x030005c4 + (isp_id * 0x00008000)));
   debug_printf("ISP_%d: value in this reg : %08x\n", isp_id, R_ISP);
   if (R_ISP == 0)
   {
      debug_printf("ISP_%d: ISR Cleared\n", isp_id);
   }
   // just read and clean it
   R_ISP = ISP_R((volatile unsigned int *)(0x030033ec + (isp_id * 0x00008000)));
   ISP_W((volatile unsigned int *)(0x030033f0 + (isp_id * 0x00008000)), R_ISP); // stitching int0
   R_ISP = ISP_R((volatile unsigned int *)(0x030038ec + (isp_id * 0x00008000)));
   ISP_W((volatile unsigned int *)(0x030038F0 + (isp_id * 0x00008000)), R_ISP); // stitching int1
   if (IPE_IRQ_STATUS_ISP0_MI_R) // ISP MI int , just read and clean it
   {
       R_ISP = ISP_R((volatile unsigned int *)(0x030016d0 + (isp_id * 0x00008000)));
       ISP_W((volatile unsigned int *)(0x030016d8 + (isp_id * 0x00008000)), R_ISP); // MI0
       R_ISP = ISP_R((volatile unsigned int *)(0x030016d4 + (isp_id * 0x00008000)));
       ISP_W((volatile unsigned int *)(0x030016dc + (isp_id * 0x00008000)), R_ISP); // MI1
       R_ISP = ISP_R((volatile unsigned int *)(0x030016f0 + (isp_id * 0x00008000)));
       ISP_W((volatile unsigned int *)(0x030016f4 + (isp_id * 0x00008000)), R_ISP); // MI2
       R_ISP = ISP_R((volatile unsigned int *)(0x030056d8 + (isp_id * 0x00008000)));
       ISP_W((volatile unsigned int *)(0x030056dc + (isp_id * 0x00008000)), R_ISP); // MI3
   }
   if (IPE_IRQ_STATUS_ISP0_FE_R) // ISP FE int , just read and clean it
   {
       R_ISP = ISP_R((volatile unsigned int *)(0x03003D74 + (isp_id * 0x00008000)));
       ISP_W((volatile unsigned int *)(0x03003D78 + (isp_id * 0x00008000)),R_ISP);
   }
}

void test_isp()
{
   /*****************************************************************
   *                         Begin Section                          *
   *****************************************************************/
   //Always call gme_init first at test begin section
   debug_printf("***************** ISP: Begin Test *****************\n");
   // gme_init();
   // configure_gpio();
   // enable_usb3_pll();
   // evp_init();
   ipe_init();
   ipe_isp_0_init();
   // sync2test(mailbox_bits,1);
   isp_01_bypass_config(0);
   isp_01_interrupt(0);

   ipe_isp_1_init();
   isp_01_bypass_config(1);
   isp_01_interrupt(1);
   // sync2test(mailbox_bits,2);



   /*****************************************************************
   *                         Body Section                           *
   *****************************************************************/
/**********************************************************************
 *
 * Description:   1. Do system initialization
 *                2. Configure IPE & ISP
 *                3. ISP will start reading frame in and do the conversion
 *                4. Wait on interrupt from IPE that process is finished
 *                5. Trig to SV
 * Notes:
 *
 * Returns:     0 if the test succeeds.
 *              A non-zero result is the first pattern that failed.
 *
 **********************************************************************/


   /*****************************************************************
   *                         End Section                            *
   *****************************************************************/
   debug_printf("ISP Test: PASSED\n");
   debug_printf("***************** ISP: End Test *****************\n");
}
#endif //BOOT_500
void evp_clk_init()
{
   GME_POWER_MODE_DSP_POWER_UP_W(1);
   GME_CONTROL_START_POWER_CHANGE_W(1);
   while(!GME_POWER_MODE_STATUS_DSP_POWER_UP_R);
   GME_CONTROL_START_POWER_CHANGE_W(0);

   GME_CLOCK_ENABLE_DSPA_CLK_EN_W(1);    // The DSP CLK EN is 1 by default (for the lram) / DSPA clock is for the processor/memories
   while(!GME_CLOCK_ENABLE_STATUS_DSPA_CLK_EN_R);
}

void evp_clk_deinit()
{
   GME_CLOCK_ENABLE_DSPA_CLK_EN_W(0);
   while(GME_CLOCK_ENABLE_STATUS_DSPA_CLK_EN_R);

   GME_POWER_MODE_DSP_POWER_UP_W(1);
   GME_CONTROL_START_POWER_CHANGE_W(0);
   GME_CONTROL_START_POWER_CHANGE_W(0);
}

void run_mem_screening_tests()
{
   unsigned int errors, i;

   errors = 0;

   debug_printf("all_mem_test: Start test\n");
   // GPIO clocks configuration
   clocksInit();
   evp_clk_init();
   debug_printf("all_mem_test: frequency change done\n");

   debug_printf("all_mem_test: *** ev_csm ***\n");
   errors+=memTest(BASE_CSM, BYTES_CSM);

   debug_printf("all_mem_test: *** ev_vmem ***\n");
   errors+=memTest(BASE_VMEM, BYTES_VMEM);

   debug_printf("all_mem_test: *** iae_dsr ***\n");
   errors+=memTest(BASE_IAE_DSR, BYTES_IAE_DSR);

   debug_printf("all_mem_test: *** ib_dsr_lut ***\n");
   errors+=ib_dsr_lut_test();

   for (i = 0; i < DDR_TEST_SPLIT_NUM; i++)
   {
       debug_printf("all_mem_test: *** lpddr4 %d: (%x, %d)***\n",i,BASE_LPDDR4 + i * (BYTES_LPDDR4 / DDR_TEST_SPLIT_NUM),(BYTES_LPDDR4 / DDR_TEST_SPLIT_NUM) / 1024);
       errors+=memTest(BASE_LPDDR4 + i * (BYTES_LPDDR4 / DDR_TEST_SPLIT_NUM), (BYTES_LPDDR4 / DDR_TEST_SPLIT_NUM) / 1024);
   }
#ifdef BOOT_500
   test_isp();
#endif //BOOT_500
   if (errors == 0)
   {
      debug_printf("\nall_mem_test: TEST PASSED\n");
   }
   else
   {
      debug_printf("\nall_mem_test: TEST FAILED\n");
   }

   evp_clk_deinit();
   clocksDeinit();

   debug_printf("all_mem_test: End test\n");
   //write to DDR the result value
   *((unsigned int*)(0xBF000000)) = errors;
}


unsigned int memTest(unsigned int addressG,  unsigned long nBytes)
{
   volatile unsigned int *address = (volatile unsigned int*)addressG;

  unsigned int data_res_base_0 = 0;
  unsigned int data_res_base_4 = 0;
  unsigned int data_res_base_8 = 0;
  unsigned int data_res_base_c = 0;

  unsigned int *addr_res = NULL;
  unsigned int *device_res = NULL;

  unsigned int errors;

  errors = 0;

  data_res_base_0 = memTestDataBus(address);
  data_res_base_4 = memTestDataBus(address+4);
  data_res_base_8 = memTestDataBus(address+8);
  data_res_base_c = memTestDataBus(address+12);

  debug_printf("Memory Test: Start ***********************************************\nNUM_BYTES is: %d\n",nBytes);

  if (data_res_base_0 != 0)
    {
      debug_printf("Memory Test: Data Bus Test FAILED, Address Base 0, pattern 0x%08x\n",data_res_base_0);
      errors++;
    }
  else if (data_res_base_4 != 0)
    {
      debug_printf("Memory Test: Data Bus Test FAILED, Address Base 4, pattern 0x%08x\n",data_res_base_4);
      errors++;
    }
  else if (data_res_base_8 != 0)
    {
      debug_printf("Memory Test: Data Bus Test FAILED, Address Base 8, pattern 0x%08x\n",data_res_base_8);
      errors++;
    }
  else if (data_res_base_c != 0)
    {
      debug_printf("Memory Test: Data Bus Test FAILED, Address Base C, pattern 0x%08x\n",data_res_base_c);
      errors++;
    }
  else
    {
      debug_printf("Memory Test: Data Bus Test PASSED\n");
    }

  addr_res = memTestAddressBus(address, nBytes);

  if (addr_res != NULL)
    {
      debug_printf("Memory Test: Address Bus Test FAILED, Address 0x%08x\n", addr_res);
      errors++;
    }
  else
    {
      debug_printf("Memory Test: Address Bus Test PASSED\n");
    }


#ifdef SKIP_DEVICE_TEST

  debug_printf("Memory Test: Device Test SKIPPED\n");

#else

  device_res = memTestDevice(address, nBytes);

  if (device_res != NULL)
    {
      errors++;
    }
  else
    {
      debug_printf("Memory Test: Device Test PASSED\n");
    }

#endif
  return errors;

}   /* memTest() */


unsigned int *memTestAddressBus(volatile unsigned int * baseAddress, unsigned long nBytes)
{
    unsigned long addressMask = (nBytes/sizeof(unsigned int) - 1);
    unsigned long offset;
    unsigned long testOffset;

    unsigned int pattern     = (unsigned int) 0xAAAAAAAA;
    unsigned int antipattern = (unsigned int) 0x55555555;


    /*
     * Write the default pattern at each of the power-of-two offsets.
     */
    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        baseAddress[offset] = pattern;
    }

    /*
     * Check for address bits stuck high.
     */
    testOffset = 0;
    baseAddress[testOffset] = antipattern;

    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        if (baseAddress[offset] != pattern)
        {
            return ((unsigned int *) &baseAddress[offset]);
        }
    }

    baseAddress[testOffset] = pattern;

    /*
     * Check for address bits stuck low or shorted.
     */
    for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1)
    {
        baseAddress[testOffset] = antipattern;

		if (baseAddress[0] != pattern)
		{
			return ((unsigned int *) &baseAddress[testOffset]);
		}

        for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
        {
            if ((baseAddress[offset] != pattern) && (offset != testOffset))
            {
                return ((unsigned int *) &baseAddress[testOffset]);
            }
        }

        baseAddress[testOffset] = pattern;
    }

    return (NULL);

}   /* memTestAddressBus() */


unsigned int memTestDataBus(volatile unsigned int * address)
{
  unsigned int pattern;


    /*
     * Perform a walking 1's test at the given address.
     */
    for (pattern = 1; pattern != 0; pattern <<= 1)
    {
        /*
         * Write the test pattern.
         */
        *address = pattern;

        /*
         * Read it back (immediately is okay for this test).
         */
        if (*address != pattern)
        {
            return (pattern);
        }
    }

    return (0);

}   /* memTestDataBus() */


unsigned int *memTestDevice(volatile unsigned int * baseAddress, unsigned long nBytes)
{
  unsigned long offset;
  unsigned long nWords = nBytes / sizeof(unsigned int);

  unsigned int pattern;
  unsigned int antipattern;
  int error_number;


  /*
   * Fill memory with a known pattern.
   */
  for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
      baseAddress[offset] = pattern;
    }

  debug_printf ("Check First Pass Pattern\n");

  /*
   * Check each location and invert it for the second pass.
   */
  error_number = 0;

  for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
      if (baseAddress[offset] != pattern)
        {
          debug_printf("FAILED   pattern is: 0x%08x\nEXPECTED pattern is: 0x%08x\nXOR      pattern is: 0x%08x\nADDRESS          is: 0x%08x\n******** ********\n"
                  ,pattern, baseAddress[offset],(pattern^baseAddress[offset]), &baseAddress[offset]);
          error_number++;
          if (error_number == 16)
            return ((unsigned int *) &baseAddress[offset]);
        }

      antipattern = ~pattern;
      baseAddress[offset] = antipattern;
    }

  debug_printf ("Check Second Pass Anti-Pattern\n");

  /*
   * Check each location for the inverted pattern and zero it.
   */
  error_number = 0;

  for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
      antipattern = ~pattern;
      if (baseAddress[offset] != antipattern)
        {
          debug_printf("FAILED   pattern is: 0x%08x\nEXPECTED pattern is: 0x%08x\nXOR      pattern is: 0x%08x\nADDRESS          is: 0x%08x\n******** ********\n"
                  ,antipattern, baseAddress[offset],(pattern^baseAddress[offset]), &baseAddress[offset]);
          error_number++;
          if (error_number == 16)
            return ((unsigned int *) &baseAddress[offset]);
        }
    }

  return (NULL);

}   /* memTestDevice() */





unsigned int memTest_s(unsigned int addressG,  unsigned long nBytes)
{
  volatile unsigned short * address = (volatile unsigned short*)addressG;
  unsigned short data_res_base_0 = 0;
  unsigned short data_res_base_4 = 0;
  unsigned short data_res_base_8 = 0;
  unsigned short data_res_base_c = 0;

  unsigned short *addr_res = NULL;
  unsigned short *device_res = NULL;

  unsigned int errors;

  errors = 0;

  data_res_base_0 = memTestDataBus_s(address);
  data_res_base_4 = memTestDataBus_s(address+4);
  data_res_base_8 = memTestDataBus_s(address+8);
  data_res_base_c = memTestDataBus_s(address+12);


  debug_printf("Memory Test: Start ***********************************************\nNUM_BYTES is: %d\n",nBytes);

  if (data_res_base_0 != 0)
    {
      debug_printf("Memory Test: Data Bus Test FAILED, Address Base 0, pattern 0x%08x\n",data_res_base_0);
      errors++;
    }
  else if (data_res_base_4 != 0)
    {
      debug_printf("Memory Test: Data Bus Test FAILED, Address Base 4, pattern 0x%08x\n",data_res_base_4);
      errors++;
    }
  else if (data_res_base_8 != 0)
    {
      debug_printf("Memory Test: Data Bus Test FAILED, Address Base 8, pattern 0x%08x\n",data_res_base_8);
      errors++;
    }
  else if (data_res_base_c != 0)
    {
      debug_printf("Memory Test: Data Bus Test FAILED, Address Base C, pattern 0x%08x\n",data_res_base_c);
      errors++;
    }
  else
    {
      debug_printf("Memory Test: Data Bus Test PASSED\n");
    }

  addr_res = memTestAddressBus_s(address, nBytes);

  if (addr_res != NULL)
    {
      debug_printf("Memory Test: Address Bus Test FAILED, Address 0x%08x\n", addr_res);
      errors++;
    }
  else
    {
      debug_printf("Memory Test: Address Bus Test PASSED\n");
    }


#ifdef SKIP_DEVICE_TEST

  debug_printf("Memory Test: Device Test SKIPPED\n");

#else

  device_res = memTestDevice_s(address, nBytes);

  if (device_res != NULL)
    {
      errors++;
    }
  else
    {
      debug_printf("Memory Test: Device Test PASSED\n");
    }

#endif
  return errors;

}   /* memTest() */


unsigned short *memTestAddressBus_s(volatile unsigned short * baseAddress, unsigned long nBytes)
{
    unsigned long addressMask = (nBytes/sizeof(unsigned short) - 1);
    unsigned long offset;
    unsigned long testOffset;

    unsigned short pattern     = (unsigned short) 0xAAAAAAAA;
    unsigned short antipattern = (unsigned short) 0x55555555;


    /*
     * Write the default pattern at each of the power-of-two offsets.
     */
    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        baseAddress[offset] = pattern;
    }

    /*
     * Check for address bits stuck high.
     */
    testOffset = 0;
    baseAddress[testOffset] = antipattern;

    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        if (baseAddress[offset] != pattern)
        {
            return ((unsigned short *) &baseAddress[offset]);
        }
    }

    baseAddress[testOffset] = pattern;

    /*
     * Check for address bits stuck low or shorted.
     */
    for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1)
    {
        baseAddress[testOffset] = antipattern;

		if (baseAddress[0] != pattern)
		{
			return ((unsigned short *) &baseAddress[testOffset]);
		}

        for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
        {
            if ((baseAddress[offset] != pattern) && (offset != testOffset))
            {
                return ((unsigned short *) &baseAddress[testOffset]);
            }
        }

        baseAddress[testOffset] = pattern;
    }

    return (NULL);

}   /* memTestAddressBus() */


unsigned short memTestDataBus_s(volatile unsigned short * address)
{
  unsigned short pattern;


    /*
     * Perform a walking 1's test at the given address.
     */
    for (pattern = 1; pattern != 0; pattern <<= 1)
    {
        /*
         * Write the test pattern.
         */
        *address = pattern;

        /*
         * Read it back (immediately is okay for this test).
         */
        if (*address != pattern)
        {
            return (pattern);
        }
    }

    return (0);

}   /* memTestDataBus() */


unsigned short *memTestDevice_s(volatile unsigned short * baseAddress, unsigned long nBytes)
{
  unsigned long offset;
  unsigned long nWords = nBytes / sizeof(unsigned short);

  unsigned short pattern;
  unsigned short antipattern;
  int error_number;


  /*
   * Fill memory with a known pattern.
   */
  for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
      baseAddress[offset] = pattern;
    }

  debug_printf ("Check First Pass Pattern\n");

  /*
   * Check each location and invert it for the second pass.
   */
  error_number = 0;

  for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
      if (baseAddress[offset] != pattern)
        {
          debug_printf("FAILED   pattern is: 0x%08x\nEXPECTED pattern is: 0x%08x\nXOR      pattern is: 0x%08x\nADDRESS          is: 0x%08x\n******** ********\n"
                  ,pattern, baseAddress[offset],(pattern^baseAddress[offset]), &baseAddress[offset]);
          error_number++;
          if (error_number == 16)
            return ((unsigned short *) &baseAddress[offset]);
        }

      antipattern = ~pattern;
      baseAddress[offset] = antipattern;
    }

  debug_printf ("Check Second Pass Anti-Pattern\n");

  /*
   * Check each location for the inverted pattern and zero it.
   */
  error_number = 0;

  for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
      antipattern = ~pattern;
      if (baseAddress[offset] != antipattern)
        {
          debug_printf("FAILED   pattern is: 0x%08x\nEXPECTED pattern is: 0x%08x\nXOR      pattern is: 0x%08x\nADDRESS          is: 0x%08x\n******** ********\n"
                  ,antipattern, baseAddress[offset],(pattern^baseAddress[offset]), &baseAddress[offset]);
          error_number++;
          if (error_number == 16)
            return ((unsigned short *) &baseAddress[offset]);
        }
    }

  return (NULL);

}   /* memTestDevice() */


unsigned int ib_dsr_lut_test()
{
  unsigned int errorNum = 0;
  int *device_res = NULL;

  // IB0,0:  0x04044EFF - 0x04040000
  // IB0,1:  0x04064EFF - 0x04060000
  // IB0,2:  0x04084EFF - 0x04080000
  // IB0,3:  0x040A4EFF - 0x040A0000

  device_res = memTestDevice_ib_dsr(0x04040000, 20224, 2, 1, 0x0000000f);
  if (device_res != NULL)
    {
      debug_printf("IB0:0 Test: Device Test FAILED, Address 0x%08x\n", device_res);
      errorNum++;
    }
  else
    {
      debug_printf("IB0:0 Test: Device Test PASSED\n");
    }

  device_res = memTestDevice_ib_dsr(0x04060000, 20224, 2, 1, 0x0000000f);
  if (device_res != NULL)
    {
      debug_printf("IB0:1 Test: Device Test FAILED, Address 0x%08x\n", device_res);
      errorNum++;
    }
  else
    {
      debug_printf("IB0:1 Test: Device Test PASSED\n");
    }

  device_res = memTestDevice_ib_dsr(0x04080000, 20224, 2, 1, 0x0000000f);
  if (device_res != NULL)
      {
        debug_printf("IB0:2 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
  else
    {
      debug_printf("IB0:2 Test: Device Test PASSED\n");
    }

    device_res = memTestDevice_ib_dsr(0x040A0000, 20224, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB0:3 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB0:3 Test: Device Test PASSED\n");
      }

    // IB1,0:  0x040C4EFF - 0x040C0000
    // IB1,1:  0x040E4EFF - 0x040E0000
    // IB1,2:  0x04104EFF - 0x04100000
    // IB1,3:  0x04124EFF - 0x04120000

    device_res = memTestDevice_ib_dsr(0x040C0000, 20224, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB1:0 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB1:0 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x040E0000, 20224, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB1:1 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB1:1 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x04100000, 20224, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB1:2 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB1:2 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x04120000, 20224, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB1:3 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB1:3 Test: Device Test PASSED\n");
      }

    // IB2,0:  0x041491FF - 0x04140000
    // IB2,1:  0x041691FF - 0x04160000
    // IB2,2:  0x041891FF - 0x04180000
    // IB2,3:  0x041A91FF - 0x041A0000

    device_res = memTestDevice_ib_dsr(0x04140000, 37376, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB2:0 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB2:0 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x04160000, 37376, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB2:1 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB2:1 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x04180000, 37376, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB2:2 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB2:2 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x041A0000, 37376, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB2:3 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB2:3 Test: Device Test PASSED\n");
      }

    // IB3,0:  0x041C91FF - 0x041C0000
    // IB3,1:  0x041E91FF - 0x041E0000
    // IB3,2:  0x042091FF - 0x04200000
    // IB3,3:  0x042291FF - 0x04220000

    device_res = memTestDevice_ib_dsr(0x041C0000, 37376, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB3:0 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB3:0 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x041E0000, 37376, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB3:1 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB3:1 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x04200000, 37376, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB3:2 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB3:2 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x04220000, 37376, 2, 1, 0x0000000f);
    if (device_res != NULL)
      {
        debug_printf("IB3:3 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("IB3:3 Test: Device Test PASSED\n");
      }

    // DSR0,0: 0x042827FF - 0x04280000
    // DSR0,1: 0x042A27FF - 0x042A0000
    // DSR0,2: 0x042C27FF - 0x042C0000
    // DSR0,3: 0x042E27FF - 0x042E0000

    device_res = memTestDevice_ib_dsr(0x04280000, 10240, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR0:0 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR0:0 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x042A0000, 10240, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR0:1 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR0:1 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x042C0000, 10240, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR0:2 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR0:2 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x042E0000, 10240, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR0:3 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR0:3 Test: Device Test PASSED\n");
      }

    // DSR1,0: 0x043027FF - 0x04300000
    // DSR1,1: 0x043227FF - 0x04320000
    // DSR1,2: 0x043427FF - 0x04340000
    // DSR1,3: 0x043627FF - 0x04360000

    device_res = memTestDevice_ib_dsr(0x04300000, 10240, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR1:0 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR1:0 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x04320000, 10240, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR1:1 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR1:1 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x04340000, 10240, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR1:2 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR1:2 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x04360000, 10240, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR1:3 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR1:3 Test: Device Test PASSED\n");
      }

    // DSR2,0: 0x043923FF - 0x04380000
    // DSR2,1: 0x043B23FF - 0x043A0000
    // DSR2,2: 0x043D23FF - 0x043C0000
    // DSR2,3: 0x043F23FF - 0x043E0000

    device_res = memTestDevice_ib_dsr(0x04380000, 74752, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR2:0 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR2:0 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x043A0000, 74752, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR2:1 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR2:1 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x043C0000, 74752, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR2:2 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR2:2 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x043E0000, 74752, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR2:3 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR2:3 Test: Device Test PASSED\n");
      }

    // DSR3,0: 0x044123FF - 0x04400000
    // DSR3,1: 0x044323FF - 0x04420000
    // DSR3,2: 0x044523FF - 0x04440000
    // DSR3,3: 0x044723FF - 0x04460000

    device_res = memTestDevice_ib_dsr(0x04380000, 74752, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR3:0 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR3:0 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x043A0000, 74752, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR3:1 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR3:1 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x043C0000, 74752, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR3:2 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR3:2 Test: Device Test PASSED\n");
      }

    device_res = memTestDevice_ib_dsr(0x043E0000, 74752, 4, 3, 0x00000fff);
    if (device_res != NULL)
      {
        debug_printf("DSR3:3 Test: Device Test FAILED, Address 0x%08x\n", device_res);
        errorNum++;
      }
    else
      {
        debug_printf("DSR3:3 Test: Device Test PASSED\n");
      }

    return errorNum;
}

int *memTestDevice_ib_dsr(unsigned int baseAddressG, unsigned long nBytes, int divide, int modulo, int mask)
{
   volatile int *baseAddress = (volatile int*)baseAddressG;

    unsigned long offset;
    unsigned long nWords = nBytes / sizeof(int);

    int pattern;
    int antipattern;

    int pattern1;
    int antipattern1;

    /*
     * Fill memory with a known pattern.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        baseAddress[offset] = pattern;
    }

    /*
     * Check each location and invert it for the second pass.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
      pattern1 = pattern;
      if (offset%divide == modulo)
        {
          pattern1 &= mask;
        }

      if (baseAddress[offset] != pattern1)
        {
          return ((int *) &baseAddress[offset]);
        }

      antipattern = ~pattern;
      baseAddress[offset] = antipattern;
    }

    /*
     * Check each location for the inverted pattern and zero it.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
      {
        antipattern = ~pattern;
        antipattern1 = antipattern;
        if (offset%divide == modulo)
          {
            antipattern1 &= mask;
          }
        if (baseAddress[offset] != antipattern1)
          {
            return ((int *) &baseAddress[offset]);
          }
      }

    return (NULL);

}   /* memTestDevice() */


