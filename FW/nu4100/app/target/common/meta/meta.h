#ifndef _META_DATA_
#define _META_DATA_

//Meta section offsets\sizes
#define METAG_ALT_MODE_OFFSET        (0)
#define METAG_ALT_MODE_SIZE_BYTES    (8)

ERRG_codeE METAG_init(UINT8 *basePtr, UINT32 size);
unsigned int METAG_getOffsetBytes(void);
ERRG_codeE METAG_initChanMeta(INU_DEFSG_sysChannelIdE ch, UINT8 *bufferPhysTbl[], unsigned int numBuffers);
UINT8 *    METAG_getPtr(UINT8 *bufferPtr);
UINT8 *    METAG_getPtr2(INU_DEFSG_sysChannelIdE ch, unsigned int index);
unsigned int METAG_getSize(UINT8 *bufferPtr);
INU_DEFSG_sysChannelIdE METAG_getChannel(UINT8 *bufferPtr);

void METAG_showChannelMeta(INU_DEFSG_sysChannelIdE ch, INU_DEFSG_logLevelE loglevel);


#endif// _META_DATA_
