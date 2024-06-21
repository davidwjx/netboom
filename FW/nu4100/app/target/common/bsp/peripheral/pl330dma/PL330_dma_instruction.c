/****************************************************************************
 *
 *   FileName: PL330_dma_instruction.c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: DMA PL330 instructions implementation
 *   
 ****************************************************************************/
#include "nu4100_dma0_regs.h"
#include "PL330_dma_instruction.h"
#include "PL330_dma_misc.h"
#include "inu_common.h"


/****************************************************************************
 ***************        I N C L U D E   F I L E S               *************
 ****************************************************************************/



/****************************************************************************
 ***************       L O C A L   D E F N I T I O N S       ***************
 ****************************************************************************/

#define DMA_DBGSTATUS_BUSY      ( 0x01 )         /* debug status busy mask */

#define PL330DMA_MAX_LOOPS		( 4 )

typedef enum
{
	PL330DMA_LOOP_TYPE_NONE_E     = (0x00),
	PL330DMA_LOOP_TYPE_INFINITE_E = (0x2C),
	PL330DMA_LOOP_TYPE_FINITE_0_E = (0x38),
	PL330DMA_LOOP_TYPE_FINITE_1_E = (0x3C),
	PL330DMA_LOOP_TYPE_LAST_E,
}PL330DMA_loopTypeE;


/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/

typedef struct
{
   UINT32 				offset;
   PL330DMA_loopTypeE 	type;
}PL330DMA_loopParamT;

typedef struct
{
	PL330DMA_loopParamT loopArray[PL330DMA_MAX_LOOPS];
	UINT32				finiteRefCnt;
}PL330DMA_loopArrayParamsT;

/****************************************************************************
 ***************       L O C A L    D A T A                   ***************
 ****************************************************************************/

static PL330DMA_loopArrayParamsT progLoops;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/




/****************************************************************************
***************      L O C A L      F U N C T I O N S      ***************
****************************************************************************/

void PL330DMAP_PrintProgToRegister(UINT32 core, UINT32 ch, dmaChannelProg endprog, dmaChannelProg prog, UINT32 numOfLoops)
{
#if 0
   UINT32 dmaEndofProg = 0;
   UINT8  buf[1024];
   UINT32 codeSize = 0; 
   UINT32 i = 0; 

   dmaEndofProg = (UINT32)endprog;
   codeSize = dmaEndofProg - (UINT32)prog;

   memset(buf,0,sizeof(buf));
   sprintf(buf + strlen(buf), " core %d, ch %d\n",core,ch);
   sprintf(buf + strlen(buf), " numOfLoops = %d\n",numOfLoops);
   sprintf(buf + strlen(buf), " prog = 0x%x\n",prog);
   sprintf(buf + strlen(buf), " dmaEndofProg = 0x%x\n",dmaEndofProg);
   sprintf(buf + strlen(buf), " codeSize = 0x%x \n",codeSize);
   sprintf(buf + strlen(buf), " Printing DMA programm\n");
   for( i = 0; i < codeSize; i+=4)
   {
      sprintf(buf + strlen(buf), "\t\t 0x%08x \n",*(UINT32*)(prog + i));
   }
   sprintf(buf + strlen(buf), " \n");
   printf("%s",buf);
#else
   (void)core;
   (void)ch;
   (void)endprog;
   (void)prog;
   (void)numOfLoops;
#endif
}


dmaChannelProg PL330DMAP_ConstrInit    ( dmaChannelProg dmaProg )
{
   memset(&progLoops, 0x00, sizeof(PL330DMA_loopArrayParamsT));
   return dmaProg;
}

dmaChannelProg PL330DMAP_Inst_Dmamov       ( dmaChannelProg dmaProg, PL330DMA_RegE dmaReg, UINT32 value )
{
   *(dmaProg)       = (UINT8)(0xBC);
   *(dmaProg + 1)    = (UINT8)(dmaReg);
   *(dmaProg + 2)    = (UINT8)((value >> 0)  & 0xFF);
   *(dmaProg + 3)    = (UINT8)((value >> 8)  & 0xFF);   
   *(dmaProg + 4)    = (UINT8)((value >> 16) & 0xFF);
   *(dmaProg + 5)    = (UINT8)((value >> 24) & 0xFF);   

   return (dmaProg + 6);

}   
dmaChannelProg PL330DMAP_Inst_Dmaaddh      ( dmaChannelProg dmaProg, PL330DMA_RegE dmaReg, UINT16 offset )
{
   *(dmaProg)       = (UINT8)(0x54 | dmaReg);
   *(dmaProg + 1)    = (UINT8)((offset >> 0) & 0xFF);
   *(dmaProg + 2)    = (UINT8)((offset >> 8) & 0xFF);   

   return (dmaProg + 3);
}    
dmaChannelProg PL330DMAP_Inst_Dmaadnh       ( dmaChannelProg dmaProg, PL330DMA_RegE dmaReg, UINT16 offset )
{
   *(dmaProg)       = (UINT8)(0x5C | dmaReg);
   *(dmaProg + 1)    = (UINT8)((offset >> 0) & 0xFF);
   *(dmaProg + 2)    = (UINT8)((offset >> 8) & 0xFF);   

   return (dmaProg + 3);
}

dmaChannelProg PL330DMAP_Inst_Dmawfp       ( dmaChannelProg dmaProg, UINT8 periph, PL330DMA_RequestTypeE waitType )
{
   *(dmaProg)       = (UINT8)(0x30 | (waitType & 0x03));      // waitType: 00 - single, 01 - periph, 10 - burst
   *(dmaProg + 1)    = (UINT8)((periph & 0x1F) << 0x03);

   return (dmaProg + 2);
}   
dmaChannelProg PL330DMAP_Inst_Dmaldp       ( dmaChannelProg dmaProg, UINT8 periph, PL330DMA_RequestTypeE reqType )
{
   *(dmaProg)       = (UINT8)(0x25 | (reqType & 0x03));         // reqType: 00 - single, 10 - burst
   *(dmaProg + 1)    = (UINT8)((periph & 0x1F) << 0x03);

   return (dmaProg + 2);
}   
dmaChannelProg PL330DMAP_Inst_Dmastp       ( dmaChannelProg dmaProg, UINT8 periph, PL330DMA_RequestTypeE reqType )
{
   *(dmaProg)       = (UINT8)(0x29 | (reqType & 0x03));         // reqType: 00 - single, 10 - burst
   *(dmaProg + 1)    = (UINT8)((periph & 0x1F) << 0x03);

   return (dmaProg + 2);
}   
dmaChannelProg PL330DMAP_Inst_Dmaflushp   ( dmaChannelProg dmaProg, UINT8 periph )
{
   *(dmaProg)       = (UINT8)(0x35);
   *(dmaProg + 1)    = (UINT8)((periph & 0x1F) << 0x03);

   return (dmaProg + 2);
}   
dmaChannelProg PL330DMAP_Inst_Dmawfe       ( dmaChannelProg dmaProg, UINT8 event, UINT8 invalidate )
{
   *(dmaProg)       = (UINT8)(0x36);
   *(dmaProg + 1)    = (UINT8)( ((event & 0x1F) << 0x03) | ((invalidate & 0x01) << 1) );

   return (dmaProg + 2);
}   
dmaChannelProg PL330DMAP_Inst_Dmasev       ( dmaChannelProg dmaProg, UINT8 event )
{
   *(dmaProg)       = (UINT8)(0x34);
   *(dmaProg + 1)    = (UINT8)((event & 0x1F) << 0x03);

   return (dmaProg + 2);
}   

#if 1	//new loop implementation with unlimited infinite loops

dmaChannelProg PL330DMAP_Inst_Dmalp32 		( dmaChannelProg dmaProg, UINT32 iterations )
{
	UINT8 i, reg = 0;
    UINT32 iterations_m1 = iterations;

	for( i = 0; i < PL330DMA_MAX_LOOPS; i++ )
	{
		if(progLoops.loopArray[i].offset == 0)
		{
			if(progLoops.finiteRefCnt == 0)
			{
				progLoops.loopArray[i].offset = (UINT32)(dmaProg + 6);
				progLoops.loopArray[i].type = PL330DMA_LOOP_TYPE_FINITE_0_E;
				progLoops.finiteRefCnt++;
				reg = 5;
				break;
			}
			else if(progLoops.finiteRefCnt == 1)
			{
				progLoops.loopArray[i].offset = (UINT32)(dmaProg +  6);
				progLoops.loopArray[i].type = PL330DMA_LOOP_TYPE_FINITE_1_E;
				progLoops.finiteRefCnt++;
				reg = 6;
				break;
			}
			else
			{
			
				printf("PL330DMAP_Inst_Dmalp32: The channel exeeds max alowed loop counters (%d)\n",progLoops.finiteRefCnt);   
				return (dmaChannelProg)0xFF;
			}
				
		}
	}

	if(i >= PL330DMA_MAX_LOOPS)
	{
		printf("PL330DMAP_Inst_Dmalp32: The channel exeeds max total counters (%d)\n", i); 		
		return (dmaChannelProg)0xFF;
	}

		
   *(dmaProg)        = (UINT8)(0xBC);
   *(dmaProg + 1)    = (UINT8)(reg);
   *(dmaProg + 2)    = (UINT8)((iterations_m1 >> 0)  & 0xFF);
   *(dmaProg + 3)    = (UINT8)((iterations_m1 >> 8)  & 0xFF); 
   *(dmaProg + 4)    = (UINT8)((iterations_m1 >> 16) & 0xFF);
   *(dmaProg + 5)    = (UINT8)((iterations_m1 >> 24) & 0xFF); 
   return (dmaProg + 6);
}


dmaChannelProg PL330DMAP_Inst_Dmalpfe       ( dmaChannelProg dmaProg )
{
	UINT8 i;

	for( i = 0; i < PL330DMA_MAX_LOOPS; i++ )
	{
		if(progLoops.loopArray[i].offset == 0)
		{
			progLoops.loopArray[i].offset = (UINT32)(dmaProg); 
			progLoops.loopArray[i].type = PL330DMA_LOOP_TYPE_INFINITE_E;
			break;
		}
	}

	if(i >= PL330DMA_MAX_LOOPS)
	{
		printf("PL330DMAP_Inst_Dmalpfe: The channel exeeds max total counters (%d)\n", i); 		
		return (dmaChannelProg)0xFF;
	}

      
   return (dmaProg);

}


dmaChannelProg PL330DMAP_Inst_Dmalpend    ( dmaChannelProg dmaProg )         // NOTE: options [S|B] are not implemented 
{
	UINT8 backwardJump = 0;   
	UINT8 loopOpcode = 0;
	UINT8 i, index;

	for( i = 0; i < PL330DMA_MAX_LOOPS; i++ )
	{
		index = PL330DMA_MAX_LOOPS - i - 1;
		if(progLoops.loopArray[index].offset != 0)
		{
			backwardJump = (UINT32)dmaProg - progLoops.loopArray[index].offset; 
			loopOpcode   = progLoops.loopArray[index].type;
			if( (progLoops.loopArray[index].type == PL330DMA_LOOP_TYPE_FINITE_0_E) || 
				(progLoops.loopArray[index].type == PL330DMA_LOOP_TYPE_FINITE_1_E) )
			{		
				progLoops.finiteRefCnt--;
			}
			progLoops.loopArray[index].offset = 0;
			progLoops.loopArray[index].type = PL330DMA_LOOP_TYPE_NONE_E;
			break;
		}
	}
	
	*(dmaProg)		= (UINT8)(loopOpcode);
	*(dmaProg + 1)	= (UINT8)(backwardJump);
	
	return (dmaProg + 2);
      
}



#else
dmaChannelProg PL330DMAP_Inst_Dmalp32 		( dmaChannelProg dmaProg, UINT32 iterations )
{
	UINT8 loopNumber, reg;
        UINT32 iterations_m1;

        iterations_m1 =iterations;//(iterations - 1);

	if( loopArr[0].offset == 0 )
	{
		loopArr[0].offset = (UINT32)(dmaProg + 6); 
		loopArr[0].infinite = 0x10;
		loopNumber = 0;
		reg = 5;
	}
	else if( loopArr[1].offset == 0 )
	{
		loopArr[1].offset = (UINT32)(dmaProg + 6); 
		loopArr[1].infinite = 0x10;
		loopNumber = 2;
		reg = 6;
	}
	else
	{
		return (dmaChannelProg)0xFF;	// TODO: return error here. pl330 does not support more than 2 loops
	}
		
   *(dmaProg)        = (UINT8)(0xBC);
   *(dmaProg + 1)    = (UINT8)(reg);
   *(dmaProg + 2)    = (UINT8)((iterations_m1 >> 0)  & 0xFF);
   *(dmaProg + 3)    = (UINT8)((iterations_m1 >> 8)  & 0xFF); 
   *(dmaProg + 4)    = (UINT8)((iterations_m1 >> 16) & 0xFF);
   *(dmaProg + 5)    = (UINT8)((iterations_m1 >> 24) & 0xFF); 
   
   return (dmaProg + 6);
}


dmaChannelProg PL330DMAP_Inst_Dmalp       ( dmaChannelProg dmaProg, UINT8 iterations )
{
   UINT8 loopNumber;

   if( loopArr[0].offset == 0 )
   {
      loopArr[0].offset = (UINT32)(dmaProg + 2); 
      loopArr[0].infinite = 0x10;
      loopNumber = 0;
   }
   else if( loopArr[1].offset == 0 )
   {
      loopArr[1].offset = (UINT32)(dmaProg + 2); 
      loopArr[1].infinite = 0x10;
      loopNumber = 2;
   }
   else
   {
      return (dmaChannelProg)0xFF;   // TODO: return error here. pl330 does not support more than 2 loops
   }
      
   *(dmaProg)       = (UINT8)(0x20 | loopNumber);
   *(dmaProg + 1)    = (UINT8)(iterations - 1);

   return (dmaProg + 2);
}   
dmaChannelProg PL330DMAP_Inst_Dmalpfe       ( dmaChannelProg dmaProg )
{
   if( loopArr[0].offset == 0 )
   {
      loopArr[0].offset = (UINT32)(dmaProg); 
      loopArr[0].infinite = 0x00;
   }
   else if( loopArr[1].offset == 0 )
   {
      loopArr[1].offset = (UINT32)(dmaProg); 
      loopArr[1].infinite = 0x00;
   }
   else
   {
      return (dmaChannelProg)0xFF;   // TODO: return error here. pl330 does not support more than 2 loops
   }
      
   return (dmaProg);

}
dmaChannelProg PL330DMAP_Inst_Dmalpend    ( dmaChannelProg dmaProg )         // NOTE: options [S|B] are not implemented 
{
   UINT8 backwardJump = 0;   
   UINT8 infiniteLoop = 0;
   UINT8 loopNumber;
   
   if( loopArr[1].offset != 0 )
   {
      backwardJump = (UINT32)dmaProg - loopArr[1].offset;
      infiniteLoop = loopArr[1].infinite;
      loopNumber = 0x04;                  //(loopArr[1].infinite == 0) ? (0x04):(0x04);
      loopArr[1].offset = 0;
      loopArr[1].infinite = 0;
   }
   else if( loopArr[0].offset != 0 )
   {
      backwardJump =  (UINT32)dmaProg - loopArr[0].offset;
      infiniteLoop = loopArr[0].infinite;   
      loopNumber  = (loopArr[0].infinite == 0x00) ? (0x04):(0x00);      
      loopArr[0].offset = 0;
      loopArr[0].infinite = 0;
   }
   else
   {
      return (dmaChannelProg)0xFE;   // TODO: return error here. In code there are more lpend then lp/lpfe
   }

   *(dmaProg)      = (UINT8)(0x28 | loopNumber | infiniteLoop);
   *(dmaProg + 1)   = (UINT8)(backwardJump);

   return (dmaProg + 2);
   
      
}

#endif
dmaChannelProg PL330DMAP_Inst_Dmald       ( dmaChannelProg dmaProg, PL330DMA_RequestTypeE reqType )
{
   UINT8 request = ( reqType == PL330DMA_REQ_NONE) ? (0) : (( reqType == PL330DMA_REQ_BURST) ? (3) : (1));

   *(dmaProg)      = (UINT8)(0x04 | request);         
   return (dmaProg + 1);
}   
dmaChannelProg PL330DMAP_Inst_Dmast       ( dmaChannelProg dmaProg, PL330DMA_RequestTypeE reqType )
{
   UINT8 request = ( reqType == PL330DMA_REQ_NONE) ? (0) : (( reqType == PL330DMA_REQ_BURST) ? (3) : (1));

   *(dmaProg)       = (UINT8)(0x08 | request);         

   return (dmaProg + 1);
}   
dmaChannelProg PL330DMAP_Inst_Dmastz       ( dmaChannelProg dmaProg )
{
   *(dmaProg)       = (UINT8)(0x0C);         

   return (dmaProg + 1);
}   
dmaChannelProg PL330DMAP_Inst_Dmawmb       ( dmaChannelProg dmaProg )
{
   *(dmaProg)       = (UINT8)(0x13);         

   return (dmaProg + 1);
}   
dmaChannelProg PL330DMAP_Inst_Dmarmb       ( dmaChannelProg dmaProg )
{
   *(dmaProg)      = (UINT8)(0x12);         

   return (dmaProg + 1);
}   
dmaChannelProg PL330DMAP_Inst_Dmanop       ( dmaChannelProg dmaProg )
{
   *(dmaProg)      = (UINT8)(0x18);         

   return (dmaProg + 1);
}   
dmaChannelProg PL330DMAP_Inst_Dmaend       ( dmaChannelProg dmaProg )
{
   *(dmaProg)      = (UINT8)(0x00);         

   return (dmaProg + 1);
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/


UINT8 PL330DMAG_SendCommand( dmaChannelProg dmaProg, UINT8 core, UINT8 channel, PL330DMA_CmdE command, UINT8 event )
{
   INT16 busy_loop = 4000;

   while( (PL330_DMA_REG_RD( core, DMA_DBGSTATUS_OFFSET ) & DMA_DBGSTATUS_BUSY ) && (--busy_loop) );

   if( busy_loop < 1 )
   {
      // TODO: error message that command failed
      //string_to_gpio("SW: DMA execute fail");
      return 0xFF;
   }

   switch( command )
   {
      case PL330DMA_CMD_DMAGO     :
         PL330_DMA_REG_WR( core, DMA_DBGINST0_OFFSET, (((channel << 8) | (command)) << 16) | (channel << 8) );
         PL330_DMA_REG_WR( core, DMA_DBGINST1_OFFSET, (UINT32)dmaProg);
         break;
      case PL330DMA_CMD_DMAKILL :      
         
         PL330_DMA_REG_WR( core, DMA_DBGINST0_OFFSET, ((command) << 16) | (channel << 8) | 0x01 );   //Execute kill from manager thread
         PL330_DMA_REG_WR( core, DMA_DBGINST1_OFFSET, (UINT32)0x00 );
         break;      
      case PL330DMA_CMD_DMASEV  :
         PL330_DMA_REG_WR( core, DMA_DBGINST0_OFFSET, ((((event & 0x1F) << 11) | (command)) << 16) | (channel << 8) );
         PL330_DMA_REG_WR( core, DMA_DBGINST1_OFFSET, (UINT32)0x00 );
         break;      
      default:
         break;
   }

   PL330_DMA_REG_WR( core, DMA_DBGCMD_OFFSET, (UINT32)0x00 );

   return 0x00;
}





