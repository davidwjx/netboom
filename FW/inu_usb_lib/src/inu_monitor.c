#define INUUSB_EXPORTS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "inu_usb.h"
#include "inu_boot.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NU3K_READ_MEM			0x100
#define NU3K_WRITE_MEM			0x120
#define NU3K_JUMP_TO_ADDRESS	0x310

#define NU3K_READ_RESPONSE		0x103
#define NU3K_WRITE_DONE			0x123
#define NU3K_JUMP_WILL_DO      	0x313

#define MAX_NU3K_PAYLOAD		4080
#define MAX_NU3K_READ_CHUNK		MAX_NU3K_PAYLOAD
#define MAX_NU3K_WRITE_CHUNK    4084
#define MAX_NU3K_BUFFER			4096

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#pragma pack(push, monitor_hdr, 1)
typedef struct _MsgHdr
{
	unsigned int	Command;
	unsigned int	Len;
} MsgHdr;

typedef struct _IoRdMsgHdr
{
	MsgHdr		Hdr;
	unsigned int	Address;
	unsigned int	SizeToRead;
} IoRdMsgHdr;

typedef struct _IoWrMsgHdr
{
	MsgHdr			Hdr;
	unsigned int	Address;
} IoWrMsgHdr;

typedef struct _IoRspMsgHdr
{
   MsgHdr			Hdr;
   unsigned int		ErrCode;
} IoRspMsgHdr;

typedef struct _IoRdMsg
{
	IoRdMsgHdr	RdHdr;
	unsigned char Data[MAX_NU3K_READ_CHUNK];
} IoRdMsg;

typedef struct _IoWrMsg
{
	IoWrMsgHdr	WrHdr;
	unsigned char		Data[MAX_NU3K_WRITE_CHUNK];
} IoWrMsg;

typedef struct _IoRspMsg
{
   IoRspMsgHdr RspHdr;
   unsigned char		Data[MAX_NU3K_PAYLOAD];
} IoRspMsg;

#pragma pack(pop, monitor_hdr)

unsigned char usbPacketBuf[MAX_NU3K_BUFFER];
#ifdef NU3K_BOOT_READBACK_CHECK

static void dump_buf(unsigned char *bufP, int len)
{
   int i;
   InuUsbParamsT *inuUsbParams = inu_usb_get_contex();
   for(i=0; i < len; i++)
   {  
      if(i && ((i % 16) == 0))
      {
         inuUsbParams->log(INU_USB_LOG_DEBUG,"\n");
      }
      inuUsbParams->log(INU_USB_LOG_DEBUG,"%02x ", bufP[i]);
   }
   inuUsbParams->log(INU_USB_LOG_DEBUG,"\n");
}
#endif
static InuUsbErrorE inu_usb_monitor_write_chunk(unsigned char interfaceIdx,unsigned int address, unsigned char *buffer, unsigned int size)
{
#ifdef NU3K_TEST_VERBOSE
	unsigned char *tempBuff;
#endif
    IoWrMsg Msg = { { { NU3K_WRITE_MEM , size + 4 }, address } };
	IoRspMsgHdr RspHdr;	
	unsigned int transfered_size, size_to_send = 0;
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();	
	unsigned int extra;
	//Can write Max payload
	if (size > MAX_NU3K_WRITE_CHUNK)
		return INU_USB_ERR_IO_FAILED;

	memcpy(Msg.Data, buffer, size);

#ifdef NU3K_TEST_VERBOSE
	tempBuff = (unsigned char *)&Msg;
	inuUsbParams->log(INU_USB_LOG_DEBUG,"The Msg: CMD: 0x%08x, Size: %d, Address: 0x%08x\n", *(unsigned int *)tempBuff,*(unsigned int *)(tempBuff+4), *(unsigned int *)(tempBuff+8));
#endif	

    // Send Write buffer and get Ack	
	// Send full chunk each time with actual length in header
	// Such policy prevents variable residual packet length that can potentially harm USB2 communication (workaround!!! - need to debug in bootrom)	 
	size_to_send = MAX_NU3K_WRITE_CHUNK;
	if (size < MAX_NU3K_WRITE_CHUNK )
	{
		extra = ((size% 4) == 0) ? 0: (4-(size % 4));
		Msg.WrHdr.Hdr.Len = size + extra + 4;
	}
	if (inu_usb_write_bulk(interfaceIdx,(unsigned char *)&Msg, size_to_send + sizeof(IoWrMsgHdr), &transfered_size) == INU_USB_ERR_SUCCESS)
	{
		//Receiver response header (no data)
		if (inu_usb_read_bulk(interfaceIdx, (unsigned char *)&RspHdr, sizeof(RspHdr),&transfered_size) == INU_USB_ERR_SUCCESS)
		{
			if((RspHdr.Hdr.Command == NU3K_WRITE_DONE) && (!RspHdr.ErrCode))
			{
#ifdef NU3K_TEST_VERBOSE
				inuUsbParams->log(INU_USB_LOG_DEBUG,"Ack 'Write done' received from device\n");
#endif
				return INU_USB_ERR_SUCCESS;
			}
			else 
				{
				inuUsbParams->log(INU_USB_LOG_ERROR,"%s(): Error in write response: code=0x%x len=%d errCode=0x%x\n", __FUNCTION__, RspHdr.Hdr.Command, RspHdr.Hdr.Len-4, RspHdr.ErrCode);				
				}
		}
		else
		{
			inuUsbParams->log(INU_USB_LOG_ERROR,"%s(): Ack wasn't received from device\n",__FUNCTION__); 
		}	
	}
	else
	{
		inuUsbParams->log(INU_USB_LOG_ERROR,"%s(): Failed to send request to device\n",__FUNCTION__);
	}
	return INU_USB_ERR_IO_FAILED;
}

INUUSB_API
InuUsbErrorE inu_usb_monitor_write(unsigned int address, unsigned char *buffer, unsigned int size)
{
	unsigned int offset = 0;
	unsigned char interfaceIdx = 0;
#ifdef NU3K_BOOT_READBACK_CHECK
	unsigned char read_buf[MAX_NU3K_WRITE_CHUNK];
	unsigned int read_size;
#endif
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();

	while(size > 0)
	{         		
		unsigned int chunk_size = MIN(size, MAX_NU3K_WRITE_CHUNK);
#ifdef NU3K_TEST_VERBOSE
		inuUsbParams->log(INU_USB_LOG_DEBUG,("chunk [#%d] size: %d\n", chunk_num++, chunk_size);
#endif
		//Assuming source buffer is padded to 4-byte multiple		
		if (inu_usb_monitor_write_chunk(interfaceIdx,address + offset, &buffer[offset], chunk_size) != INU_USB_ERR_SUCCESS)
		{
			inuUsbParams->log(INU_USB_LOG_ERROR,"ERROR: write %08x %d failed\n", address, chunk_size);
			break;
		}      
#ifdef NU3K_BOOT_READBACK_CHECK
		//Read back and compare every chunk
		//bootrom requires 4-byte multiples in read		
		if (inu_usb_monitor_read_chunk(interfaceIdx,read_address + offset, read_buf, chunk_size, &read_size) == INU_USB_ERR_SUCCESS)		
		{
			if((memcmp(&buffer[offset], read_buf, chunk_size) != 0))
			{
				inuUsbParams->log(INU_USB_LOG_DEBUG,"\nFailed read back check wrote=%dB\n",chunk_size);
				inuUsbParams->log(INU_USB_LOG_DEBUG,"wrote:\n");
				dump_buf(&buffer[offset], chunk_size);
				inuUsbParams->log(INU_USB_LOG_DEBUG,"read:\n");
				dump_buf(read_buf, chunk_size);
			}
		}
		else
		{
			inuUsbParams->log(INU_USB_LOG_ERROR,"ERROR: read back on write failed\n");
		}
#endif      
		offset += chunk_size;
		size -= chunk_size;
		inuUsbParams->log(INU_USB_LOG_DEBUG,"#");		
	} //while(size)

	inuUsbParams->log(INU_USB_LOG_DEBUG,"\n");

	// if we reached the end of the buffer
	if (size <= 0)
	{
		inuUsbParams->log(INU_USB_LOG_DEBUG,"Completed writing to target %d Bytes @0x%08x\n", offset, address);
		return INU_USB_ERR_SUCCESS;
	}

	return INU_USB_ERR_IO_FAILED;
}

static InuUsbErrorE inu_usb_monitor_read_chunk(unsigned char interfaceIdx,unsigned int address, unsigned char *buffer, unsigned int size, unsigned int *transfered_size)
{
	IoRdMsg Msg = { { { NU3K_READ_MEM, 8 }, address, size } };
	IoRspMsg *Rsp = (IoRspMsg *)usbPacketBuf;
	int len;
	unsigned int extra;
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();

	*transfered_size = 0;
  	extra = ((size % 16) == 0) ? 0: (16-(size % 16));
	Msg.RdHdr.SizeToRead += extra;
	//Can read: Max payload - 3*dword response header
	if (size > MAX_NU3K_PAYLOAD)
		return INU_USB_ERR_IO_FAILED;
      	
	//When sending: Set Msg.Hdr.SizeToRead to length to read (in bytes). Msg.Hdr.Hdr.Len is not used.
	if (inu_usb_write_bulk(interfaceIdx, (unsigned char *)&Msg, sizeof(IoRdMsgHdr),transfered_size) == INU_USB_ERR_SUCCESS) {
		//Response will hold data				
		if (inu_usb_read_bulk(interfaceIdx, usbPacketBuf, sizeof(usbPacketBuf),transfered_size) == INU_USB_ERR_SUCCESS) {			
			if((Rsp->RspHdr.Hdr.Command == NU3K_READ_RESPONSE) && (!Rsp->RspHdr.ErrCode)) {
#ifdef NU3K_TEST_VERBOSE
				inuUsbParams->log(INU_USB_LOG_DEBUG,"Ack 'Read done' received from device\n");
#endif
				len = Rsp->RspHdr.Hdr.Len-4 -extra;
				memcpy(buffer, &Rsp->Data, len); 
				*transfered_size = len;		
				return INU_USB_ERR_SUCCESS;
			}else {
				inuUsbParams->log(INU_USB_LOG_ERROR,"%s(): Error in read response: code=0x%x len=%d errCode=0x%x\n", __FUNCTION__, Rsp->RspHdr.Hdr.Command, Rsp->RspHdr.Hdr.Len-4, Rsp->RspHdr.ErrCode);
			}
		}else {
			inuUsbParams->log(INU_USB_LOG_ERROR,"%s(): Ack wasn't received from device\n",__FUNCTION__);      
		}
	}else {
		inuUsbParams->log(INU_USB_LOG_ERROR,"%s(): Failed to send request to device\n",__FUNCTION__);
	}

	return INU_USB_ERR_IO_FAILED;
}

INUUSB_API
InuUsbErrorE inu_usb_monitor_read(unsigned int address, unsigned char *buffer, unsigned int size, unsigned int *transfered_size)
{
	unsigned int offset = 0;
	unsigned char interfaceIdx = 0;
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();
	*transfered_size = 0;

	while(size > 0)
	{         
		//unsigned int chunk_size = min(size, MAX_NU3K_PAYLOAD); 
		unsigned int chunk_size = MIN(size, MAX_NU3K_READ_CHUNK);
		unsigned int read_size;		
#ifdef NU3K_TEST_VERBOSE
		inuUsbParams->log(INU_USB_LOG_DEBUG,"chunk [#%d] size: %d\n", chunk_num++, chunk_size);
#endif
		unsigned int read_address=address + offset;
		unsigned int read_back_len = chunk_size;
#ifdef NU3K_TEST_VERBOSE
		inuUsbParams->log(INU_USB_LOG_DEBUG,"read at 0x%08x (%d bytes)\n", read_address, read_back_len);
#endif		
		if (inu_usb_monitor_read_chunk(interfaceIdx, read_address, &buffer[offset], read_back_len, &read_size) != INU_USB_ERR_SUCCESS) {
			inuUsbParams->log(INU_USB_LOG_ERROR,"\nError in inu_usb_monitor_read offset=%d expected=%d got=%d\n", offset, read_back_len, read_size);
			break;
		}else {
			//dump_buf(&out_buffer[offset], read_size);
			if(read_back_len != read_size) {
				inuUsbParams->log(INU_USB_LOG_ERROR,"\nFailed read expected=%dB read=%dB\n", read_size, read_back_len);                       
			}
		}
		offset += chunk_size;
		size -= chunk_size;
		*transfered_size += chunk_size;
	} //while(size)

	// if we reached the end of the buffer
	if (size <= 0)
	{
		inuUsbParams->log(INU_USB_LOG_DEBUG,"Completed reading from target %d Bytes @0x%08x\n", offset, address);
		return INU_USB_ERR_SUCCESS;
	}
	return INU_USB_ERR_IO_FAILED;
}

INUUSB_API
InuUsbErrorE inu_usb_monitor_jump(unsigned int address)
{	
	IoWrMsg Msg = { { { NU3K_JUMP_TO_ADDRESS , 4 }, address } };
	IoRspMsgHdr RspHdr;// for jump&exec done response
	unsigned int transfered_size;
	unsigned char interfaceIdx = 0;
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();


	if (inu_usb_write_bulk(interfaceIdx, (unsigned char *)&Msg, sizeof(Msg),&transfered_size) == INU_USB_ERR_SUCCESS) {      		
		if (inu_usb_read_bulk(interfaceIdx, (unsigned char *)&RspHdr, sizeof(RspHdr),&transfered_size) == INU_USB_ERR_SUCCESS) {
			if((RspHdr.Hdr.Command == NU3K_JUMP_WILL_DO) && (!RspHdr.ErrCode)) {
				inuUsbParams->log(INU_USB_LOG_INFO,"Ack For Jump received from device\n");
				return INU_USB_ERR_SUCCESS;
			}else {
				inuUsbParams->log(INU_USB_LOG_ERROR,"%s(): Error in jump response: code=0x%x errCode=0x%x\n", __FUNCTION__, RspHdr.Hdr.Command, RspHdr.ErrCode);
			}
		}else {
			inuUsbParams->log(INU_USB_LOG_ERROR,"%s(): Ack wasn't received from device\n",__FUNCTION__);      
		}
	}else {
		inuUsbParams->log(INU_USB_LOG_ERROR,"%s(): Failed to send request to device\n",__FUNCTION__);
	}    
	return INU_USB_ERR_IO_FAILED;
}

#ifdef __cplusplus
}
#endif
