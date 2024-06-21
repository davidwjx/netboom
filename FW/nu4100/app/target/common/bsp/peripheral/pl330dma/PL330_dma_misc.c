
#include "nu4100_dma0_regs.h"

#include "PL330_dma_misc.h"




static unsigned int PL330_dmaG_deviceBaseAddress;


void PL330_DMA_initBaseAddress(unsigned int dmaRegistersBaseAddr)
{
   PL330_dmaG_deviceBaseAddress = dmaRegistersBaseAddr;
}


UINT32   PL330_DMA_getBaseAddress(void)
{
   return PL330_dmaG_deviceBaseAddress;
}

void  PL330_DMA_enableFrameDoneInterrupt(UINT8 core, UINT8 channel)
{
   UINT32 reg = PL330_DMA_REG_RD( core, DMA_INTEN_OFFSET );
   
   PL330_DMA_REG_WR( core, DMA_INTEN_OFFSET, ((PL330_DMA_FRAME_DONE_INT_BIT << channel) | reg) );
}

void  PL330_DMA_enableChannelDoneInterrupt(UINT8 core, UINT8 channel)
{
   UINT32 reg = PL330_DMA_REG_RD( core, DMA_INTEN_OFFSET );
   
   PL330_DMA_REG_WR( core, DMA_INTEN_OFFSET, ((PL330_DMA_CHANNEL_DONE_INT_BIT << channel) | reg) );
}

void  PL330_DMA_disableFrameDoneInterrupt(UINT8 core, UINT8 channel)
{
   UINT32 reg = PL330_DMA_REG_RD( core, DMA_INTEN_OFFSET );
   
   PL330_DMA_REG_WR( core, DMA_INTEN_OFFSET, ((~(PL330_DMA_FRAME_DONE_INT_BIT << channel)) & reg) );
}

void  PL330_DMA_disableChannelDoneInterrupt(UINT8 core, UINT8 channel)
{
   UINT32 reg = PL330_DMA_REG_RD( core, DMA_INTEN_OFFSET );
   
   PL330_DMA_REG_WR( core, DMA_INTEN_OFFSET, ((~(PL330_DMA_CHANNEL_DONE_INT_BIT << channel)) & reg) );
}

void  PL330_DMA_clearFrameDoneInterrupt(UINT8 core, UINT8 channel)
{  
   UINT32 reg = PL330_DMA_REG_RD( core, DMA_INTCLR_OFFSET );
   
   PL330_DMA_REG_WR( core, DMA_INTCLR_OFFSET, ((PL330_DMA_FRAME_DONE_INT_BIT << channel) | reg) );

}

void  PL330_DMA_clearChannelDoneInterrupt(UINT8 core, UINT8 channel)
{  
   UINT32 reg = PL330_DMA_REG_RD( core, DMA_INTCLR_OFFSET );
   
   PL330_DMA_REG_WR( core, DMA_INTCLR_OFFSET, ((PL330_DMA_CHANNEL_DONE_INT_BIT << channel) | reg) );
}

UINT32   PL330_DMA_getInterruptStatus(UINT8 core)
{
   return PL330_DMA_REG_RD( core, DMA_INTMIS_OFFSET );
}

PL330_DMA_StatusE    PL330_DMA_getChannelStatus(UINT8 core, UINT8 channel)
{   

   UINT32 reg = PL330_DMA_REG_RD( core, DMA_CSR0_OFFSET + (0x08 * channel) );
   return (reg & 0x0F);
}

UINT32   PL330_DMA_getFaultStatusManager(UINT8 core)
{
   return PL330_DMA_REG_RD( core, DMA_FSRD_OFFSET );
}

UINT32   PL330_DMA_getFaultStatusChannel(UINT8 core)
{
   return PL330_DMA_REG_RD( core, DMA_FSRC_OFFSET );
}

UINT32   PL330_DMA_getFaultTypeChannel(UINT8 core, UINT8 channel)
{
   return PL330_DMA_REG_RD( core, DMA_FTR0_OFFSET + (0x04 * channel) );
}

UINT32   PL330_DMA_getSAR(UINT8 core, UINT8 channel)
{
   return PL330_DMA_REG_RD( core, DMA_SAR0_OFFSET + (0x20 * channel) );
}

UINT32   PL330_DMA_getDAR(UINT8 core, UINT8 channel)
{
   return PL330_DMA_REG_RD( core, DMA_DAR0_OFFSET + (0x20 * channel) );
}

UINT32   PL330_DMA_getPC(UINT8 core, UINT8 channel)
{
   return PL330_DMA_REG_RD( core, DMA_CPC0_OFFSET + (0x08 * channel) );
}


void  PL330_DMA_setEvent0FromChannel(UINT8 core, UINT8 channel);
void  PL330_DMA_setEvent1FromChannel(UINT8 core, UINT8 channel);
void  PL330_DMA_waitForEvent0(UINT8 core, UINT8 channel);
void  PL330_DMA_waitForEvent1(UINT8 core, UINT8 channel);

