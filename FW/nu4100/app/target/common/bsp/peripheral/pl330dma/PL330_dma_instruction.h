#ifndef __PL330_DMA_INSTRUCTION_H__
#define __PL330_DMA_INSTRUCTION_H__

//__________________________________
//
//		TEMP HERE
//__________________________________

#include "inu_types.h"


//__________________________________



typedef UINT8* dmaChannelProg;

// Registers that are used in DMAMOV
typedef enum
{
	PL330DMA_REG_SAR =	0x00,
	PL330DMA_REG_CCR =	0x01,
	PL330DMA_REG_DAR =	0x02
}PL330DMA_RegE;

// Commands that can be sent via APB interface
typedef enum
{
	PL330DMA_CMD_DMAGO   =	0xA0,
	PL330DMA_CMD_DMAKILL =	0x01,
	PL330DMA_CMD_DMASEV  =	0x34
}PL330DMA_CmdE;

// Channel Control Register
typedef union
{
	UINT32 word;
	struct 
	{
		UINT32 srcInc:1;
		UINT32 srcBurstSize:3;
		UINT32 srcBurstLen:4;
		UINT32 srcProtCtrl:3;
		UINT32 srcCacheCtrl:3;
		UINT32 dstInc:1;
		UINT32 dstBurstSize:3;
		UINT32 dstBurstLen:4;
		UINT32 dstProtCtrl:3;
		UINT32 dstCacheCtrl:3;
		UINT32 endianSwapSize:4;
	}field;
}PL330DMA_CcrU;

typedef enum
{
	PL330DMA_REQ_SINGLE = 0x00,
	PL330DMA_REQ_PERIPH = 0x01,
	PL330DMA_REQ_BURST  = 0x02,
	PL330DMA_REQ_NONE   = 0x03
}PL330DMA_RequestTypeE;

#define PL330DMA_LOOP_EVNT_NUM 7
#define PL330DMA_LOOP_EVNT_NUM_2 8

dmaChannelProg PL330DMAP_Inst_Dmamov 		( dmaChannelProg dmaProg, PL330DMA_RegE dmaReg, UINT32 value );	
dmaChannelProg PL330DMAP_Inst_Dmaaddh		( dmaChannelProg dmaProg, PL330DMA_RegE dmaReg, UINT16 offset ); 	
dmaChannelProg PL330DMAP_Inst_Dmaadnh 		( dmaChannelProg dmaProg, PL330DMA_RegE dmaReg, UINT16 offset );
dmaChannelProg PL330DMAP_Inst_Dmawfp 		( dmaChannelProg dmaProg, UINT8 periph, PL330DMA_RequestTypeE waitType );	
dmaChannelProg PL330DMAP_Inst_Dmaldp 		( dmaChannelProg dmaProg, UINT8 periph, PL330DMA_RequestTypeE reqType );	
dmaChannelProg PL330DMAP_Inst_Dmastp 		( dmaChannelProg dmaProg, UINT8 periph, PL330DMA_RequestTypeE reqType );	
dmaChannelProg PL330DMAP_Inst_Dmaflushp	( dmaChannelProg dmaProg, UINT8 periph );	
dmaChannelProg PL330DMAP_Inst_Dmawfe 		( dmaChannelProg dmaProg, UINT8 event, UINT8 invalidate );	
dmaChannelProg PL330DMAP_Inst_Dmasev 		( dmaChannelProg dmaProg, UINT8 event );	
dmaChannelProg PL330DMAP_Inst_Dmalp32 		( dmaChannelProg dmaProg, UINT32 iterations );
dmaChannelProg PL330DMAP_Inst_Dmalp 		( dmaChannelProg dmaProg, UINT8 iterations );	
dmaChannelProg PL330DMAP_Inst_Dmalpfe 		( dmaChannelProg dmaProg );
dmaChannelProg PL330DMAP_Inst_Dmalpend 	( dmaChannelProg dmaProg );
dmaChannelProg PL330DMAP_Inst_Dmald 		( dmaChannelProg dmaProg, PL330DMA_RequestTypeE reqType  );	
dmaChannelProg PL330DMAP_Inst_Dmast 		( dmaChannelProg dmaProg, PL330DMA_RequestTypeE reqType  );	
dmaChannelProg PL330DMAP_Inst_Dmastz 		( dmaChannelProg dmaProg );	
dmaChannelProg PL330DMAP_Inst_Dmawmb 		( dmaChannelProg dmaProg );	
dmaChannelProg PL330DMAP_Inst_Dmarmb 		( dmaChannelProg dmaProg );	
dmaChannelProg PL330DMAP_Inst_Dmanop 		( dmaChannelProg dmaProg );	
dmaChannelProg PL330DMAP_Inst_Dmaend 		( dmaChannelProg dmaProg );	

dmaChannelProg PL330DMAP_ConstrInit 		( dmaChannelProg dmaProg );
dmaChannelProg PL330DMAP_GetProgPtr 		( void );
void 		   PL330DMAP_SetProgPtr 		( dmaChannelProg dmaProg );
void PL330DMAP_PrintProgToRegister(UINT32 core, UINT32 ch, dmaChannelProg endprog, dmaChannelProg prog, UINT32 numOfLoops);


UINT8 		   PL330DMAG_SendCommand 		( dmaChannelProg dmaProg, UINT8 core, UINT8 channel, PL330DMA_CmdE command, UINT8 event );



#define PL330DMAP_DMAMOV( _progPtr_, _register_, _value_ )	_progPtr_ = ( PL330DMAP_Inst_Dmamov 		   ( _progPtr_, _register_, _value_ )		)
#define PL330DMAP_DMAADDH( _progPtr_, _register_, _offset_ ) 	_progPtr_ = ( PL330DMAP_Inst_Dmaaddh		( _progPtr_, _register_, _offset_ ) 	)
#define PL330DMAP_DMAADNH( _progPtr_, _register_, _offset_ )  _progPtr_ = ( PL330DMAP_Inst_Dmaadnh 		( _progPtr_, _register_, _offset_ )    )
#define PL330DMAP_DMAWFP( _progPtr_, _periph_, _waitType_ )	_progPtr_ = ( PL330DMAP_Inst_Dmawfp 		   ( _progPtr_, _periph_, _waitType_ )	)
#define PL330DMAP_DMALDP( _progPtr_, _periph_, _reqType_ )	_progPtr_ = ( PL330DMAP_Inst_Dmaldp 		   ( _progPtr_, _periph_, _reqType_ )		)
#define PL330DMAP_DMASTP( _progPtr_, _periph_, _reqType_ )    _progPtr_ = ( PL330DMAP_Inst_Dmastp 		( _progPtr_, _periph_, _reqType_ ) 	)
#define PL330DMAP_DMAFLUSHP( _progPtr_, _periph_ )	        _progPtr_ = ( PL330DMAP_Inst_Dmaflushp		( _progPtr_, _periph_ )	        	)
#define PL330DMAP_DMAWFE( _progPtr_, _event_, _invalidate_ )  _progPtr_ = ( PL330DMAP_Inst_Dmawfe 		( _progPtr_, _event_, _invalidate_ )   )
#define PL330DMAP_DMASEV( _progPtr_, _event_ )              	_progPtr_ = ( PL330DMAP_Inst_Dmasev 		( _progPtr_, _event_ )              	)
#define PL330DMAP_DMALP( _progPtr_, _iterations_ )	        _progPtr_ = ( PL330DMAP_Inst_Dmalp32 			( _progPtr_, _iterations_ )	        )
#define PL330DMAP_DMALPFE( _progPtr_ )                     	_progPtr_ = ( PL330DMAP_Inst_Dmalpfe 		( _progPtr_ ) 							)
#define PL330DMAP_DMALPEND( _progPtr_ )                     	_progPtr_ = ( PL330DMAP_Inst_Dmalpend 	( _progPtr_ ) 							)
#define PL330DMAP_DMALD( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmald 			( _progPtr_, PL330DMA_REQ_NONE )		)	
#define PL330DMAP_DMAST( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmast 			( _progPtr_, PL330DMA_REQ_NONE )		)	
#define PL330DMAP_DMALDS( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmald 			( _progPtr_, PL330DMA_REQ_SINGLE)		)	
#define PL330DMAP_DMASTS( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmast 			( _progPtr_, PL330DMA_REQ_SINGLE)		)	
#define PL330DMAP_DMALDB( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmald 			( _progPtr_, PL330DMA_REQ_BURST)		)	
#define PL330DMAP_DMASTB( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmast 			( _progPtr_, PL330DMA_REQ_BURST)		)	
#define PL330DMAP_DMASTZ( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmastz 		   ( _progPtr_ ) 							)	
#define PL330DMAP_DMAWMB( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmawmb 		   ( _progPtr_ ) 							)	
#define PL330DMAP_DMARMB( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmarmb 		   ( _progPtr_ ) 							)	
#define PL330DMAP_DMANOP( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmanop 		   ( _progPtr_ ) 							)	
#define PL330DMAP_DMAEND( _progPtr_ )	                    	_progPtr_ = ( PL330DMAP_Inst_Dmaend 		   ( _progPtr_ ) 							)	


#endif //__PL330_DMA_INSTRUCTION_H__



