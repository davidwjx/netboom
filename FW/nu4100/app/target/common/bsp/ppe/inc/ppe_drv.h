/****************************************************************************
 *
 *   FileName: ppe_drv.h
 *
 *   Author: 	Dima S.
 *
 *   Date: 
 *
 *   Description: Inuitive PPE Driver
 *   
 ****************************************************************************/
#ifndef __PPE_DRV_H__
#define __PPE_DRV_H__

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "err_defs.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define PPE_MAX_AXI_READER 12
#define PPE_MAX_AXI_WRITER 3

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum																					//	Source for:
{
	PPE_DRVG_SOURCE_SELECT_SLU_0_E								=	0,							//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_SLU_1_E								=	1,							//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_SLU_2_E								=	2,							//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_SLU_3_E								=	3,							//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_SLU_4_E								=	4,							//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_SLU_5_E								=	5,							//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_IAU_COLOR_0_E						=	6,							//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_IAU_COLOR_1_E						=	7,							//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_IAU_CVL_E							=	8,							//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_IAU_CVR_E							=	9,							//	PPU, AXI, MIPI, Parallel, CVA
		
	PPE_DRVG_SOURCE_SELECT_AXI_WR_0_0_E 						=	10, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_AXI_WR_0_1_E 						=	11, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_AXI_WR_1_0_E 						=	12, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_AXI_WR_1_1_E 						=	13, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_AXI_WR_2_0_E 						=	14, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_AXI_WR_2_1_E 						=	15, 						//	PPU, AXI, MIPI, Parallel, CVA
		
	PPE_DRVG_SOURCE_SELECT_CVJ_WR_0_0_E 						=	16, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_CVJ_WR_0_1_E 						=	17, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_CVJ_WR_1_0_E 						=	18, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_CVJ_WR_1_1_E 						=	19, 						//	PPU, AXI, MIPI, Parallel, CVA
		
	PPE_DRVG_SOURCE_SELECT_CVJ_2_KP_E							=	20, 						//	CVA
		
	PPE_DRVG_SOURCE_SELECT_DPE_E								=	21, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_REGISERED_WEBCAM_E					=	22, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_DEPTH_HOST_E 						=	23, 						//	PPU, AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_DEPTH_CVA_E							=	24, 						//	PPU, AXI, MIPI, Parallel, CVA
		
	PPE_DRVG_SOURCE_SELECT_PPU_0_STRM_0_E						=	32, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_0_STRM_1_E						=	33, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_0_STRM_2_E						=	34, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_0_STRM_3_E						=	35, 						//	AXI, MIPI, Parallel, CVA

	PPE_DRVG_SOURCE_SELECT_PPU_1_STRM_0_E						=	36, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_1_STRM_1_E						=	37, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_1_STRM_2_E						=	38, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_1_STRM_3_E						=	39, 						//	AXI, MIPI, Parallel, CVA

	PPE_DRVG_SOURCE_SELECT_PPU_2_STRM_0_E						=	40, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_2_STRM_1_E						=	41, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_2_STRM_2_E						=	42, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_2_STRM_3_E						=	43, 						//	AXI, MIPI, Parallel, CVA

	PPE_DRVG_SOURCE_SELECT_PPU_3_STRM_0_E						=	44, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_3_STRM_1_E						=	45, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_3_STRM_2_E						=	46, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_3_STRM_3_E						=	47, 						//	AXI, MIPI, Parallel, CVA

	PPE_DRVG_SOURCE_SELECT_PPU_4_STRM_0_E						=	48, 						//	AXI, MIPI, Parallel, CVA, PPUs w/ hybrid (streams 0 only)
	PPE_DRVG_SOURCE_SELECT_PPU_4_STRM_1_E						=	49, 						//	AXI, MIPI, Parallel, CVA, PPUs w/ hybrid (streams 0 only)
	PPE_DRVG_SOURCE_SELECT_PPU_4_STRM_2_E						=	50, 						//	AXI, MIPI, Parallel, CVA, PPUs w/ hybrid (streams 0 only)
	PPE_DRVG_SOURCE_SELECT_PPU_4_STRM_3_E						=	51, 						//	AXI, MIPI, Parallel, CVA, PPUs w/ hybrid (streams 0 only)

	PPE_DRVG_SOURCE_SELECT_PPU_5_STRM_0_E						=	52, 						//	AXI, MIPI, Parallel, CVA, PPUs w/ hybrid (streams 0 only)
	PPE_DRVG_SOURCE_SELECT_PPU_5_STRM_1_E						=	53, 						//	AXI, MIPI, Parallel, CVA, PPUs w/ hybrid (streams 0 only)
	PPE_DRVG_SOURCE_SELECT_PPU_5_STRM_2_E						=	54, 						//	AXI, MIPI, Parallel, CVA, PPUs w/ hybrid (streams 0 only)
	PPE_DRVG_SOURCE_SELECT_PPU_5_STRM_3_E						=	55, 						//	AXI, MIPI, Parallel, CVA, PPUs w/ hybrid (streams 0 only)

	PPE_DRVG_SOURCE_SELECT_PPU_7_STRM_0_E						=	68, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_7_STRM_1_E						=	69, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_7_STRM_2_E						=	70, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_7_STRM_3_E						=	71, 						//	AXI, MIPI, Parallel, CVA

	PPE_DRVG_SOURCE_SELECT_PPU_8_STRM_0_E						=	80, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_8_STRM_1_E						=	81, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_8_STRM_2_E						=	82, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_8_STRM_3_E						=	83, 						//	AXI, MIPI, Parallel, CVA

	PPE_DRVG_SOURCE_SELECT_PPU_9_STRM_0_E						=	84, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_9_STRM_1_E						=	85, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_9_STRM_2_E						=	86, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_9_STRM_3_E						=	87, 						//	AXI, MIPI, Parallel, CVA

	PPE_DRVG_SOURCE_SELECT_PPU_10_STRM_0_E						=	88, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_10_STRM_1_E						=	89, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_10_STRM_2_E						=	90, 						//	AXI, MIPI, Parallel, CVA
	PPE_DRVG_SOURCE_SELECT_PPU_10_STRM_3_E						=	91, 						//	AXI, MIPI, Parallel, CVA
	
}PPE_DRVG_sourceSelectPpeE;

//This enum represent every ISR type, add more if needed
typedef enum
{
   PPE_DRVG_ISR_AXI_READER,
   PPE_DRVG_ISR_MIPI_VSC_FRAME_END,
   PPE_DRVG_ISR_MAX_TYPE_NUM,
}PPE_DRVG_isrTypeE;

typedef void (*PPE_DRVG_isrCbT)(UINT64 timestamp, UINT32 blockNum, void *argP);

typedef struct
{
	PPE_DRVG_sourceSelectPpeE 	srcSel;
	UINT8 						splitRepRate;
	UINT8 						repackerBypass;
	UINT8 						cscBypass;
	UINT8 						cscMode;
}PPE_DRVG_ppuControlT;

typedef struct
{
	UINT16 startX;
	UINT16 startY;
	UINT16 endX;
	UINT16 endY;
}PPE_DRVG_ppuCropT;

typedef struct 
{
	UINT16 a11;
	UINT16 a12;
	UINT16 a13;
	UINT16 b1;
	UINT16 a21;
	UINT16 a22;
	UINT16 a23;
	UINT16 b2;
	UINT16 a31;
	UINT16 a32;
	UINT16 a33;
	UINT16 b3;
}PPE_DRVG_ppuCscT;


typedef struct
{
	PPE_DRVG_sourceSelectPpeE	srcSel;
	UINT8  						interleaveMode;
	UINT8  						interleaveEnable;				//int_strm_en
	UINT8  						pixelDataWidth;					//0 - 8 bits  1 - 12 bits  2 - 16 bits  3 - 24 bits
	UINT8  						pixelInterleaveWidth;			//0 - 8 bits  1 - 12 bits  2 - 16 bits  3 - 24 bits  4 - 32 bits
	UINT8  						frameIdEn;
	UINT8  						errorRecoveryEn;
	UINT8  						dmaType;
	UINT16 						mutualStrms;
}PPE_DRVG_axiReaderControlT;

typedef struct
{
	UINT8 line_buffer_overflow;
	UINT8 frame_start;
	UINT8 frame_end;
}PPE_DRVG_axiReaderInterruptT;



typedef struct
{
	PPE_DRVG_ppuControlT control;
	PPE_DRVG_ppuCropT crop;
 	PPE_DRVG_ppuCscT csc;
 }PPE_DRVG_ppuCfgT;

typedef struct
{

	UINT8  							ppuWithoutSclEn:4;
	UINT8  							ppuWithSclEn:2;	
	UINT8  							reserved0:2;
	UINT8  							ppuWithHybsclEn:2;
	UINT8  							ppuWithHybEn:3;	
	UINT8  							reserved1:1;
 }PPE_DRVG_ppuEnableT;

typedef union
{
	PPE_DRVG_ppuEnableT				field;
	UINT16							word;
}PPE_DRVG_ppuEnableU;

typedef struct
{
 	UINT32 go;
	UINT16 vecEn;
	UINT8  vecSize;
 }PPE_DRVG_axiReaderFrrT;


typedef struct
{
	PPE_DRVG_axiReaderControlT 		control;
	PPE_DRVG_axiReaderInterruptT 	interrupt;
	PPE_DRVG_axiReaderFrrT			frr;
	UINT32  						axiBeatLast;
	UINT8  							burstLen;
	UINT8  							singleEnd;
 }PPE_DRVG_axiReaderCfgT;

/****************************************************************************
 ***************	  G L O B A L		 D A T A			  ***************
 ****************************************************************************/



/****************************************************************************
 ***************	 G L O B A L		 F U N C T I O N S	  ***************
 ****************************************************************************/
 
ERRG_codeE PPE_DRVG_init( UINT32 memVirtAddr );
ERRG_codeE PPE_DRVG_setupPpuWithoutScl(  UINT8 ppuNum, PPE_DRVG_ppuCfgT   *ppuCfg);
ERRG_codeE PPE_DRVG_enablePpu(  PPE_DRVG_ppuEnableU enable );

ERRG_codeE PPE_DRVG_setupAxiReader(  UINT8 readerNum, PPE_DRVG_axiReaderCfgT   *readerCfg);
ERRG_codeE PPE_DRVG_ppeReady( void );
void PPE_DRVG_showStats();
void PPE_DRVG_updateWriterFrameId ( UINT8 writerNum, UINT32 frameId );
void PPE_DRVG_csiControllerTxCfg(UINT32 tx_num, UINT32 pktSize, UINT32 videoFormat, UINT32 minDelayBetweenPckts);
void PPE_DRVG_mipiVscCsiTxEn( UINT32 vscNum, PPE_DRVG_isrCbT cb, void *argP, void **handle );
void PPE_DRVG_mipiVscCsiTxDis( UINT32 vscNum, void *handle );
void PPE_DRVG_mipiViEn(INT32 ViNum, UINT32 intMode);
ERRG_codeE PPE_DRVG_registerIsr(PPE_DRVG_isrTypeE isrType, UINT32 blkNum, PPE_DRVG_isrCbT cb, void* argP, void **handle);
void PPE_DRVG_unregisterIsr(void *handle);
/**
 * @brief Updates the writer timestamp (TS0) for writerNum with value timestamp
 * 
 * @param writerNum Writer number (0->5)
 * @param timestamp 64 bit timestamp
 */
void PPE_DRVG_updateWriterTimestamp ( UINT8 writerNum, UINT64 timestamp );

#endif //__PPE_DRV_H__

