// xmodem.cpp : Defines the entry point for the console application.
//
#include "xmodem_boot.h"
#include "xmodem.h"
#include "common.h"
#include "verify.h"

#define XMODEM_TARGET_ADDRESS 0x2000000

void xmodem_boot_init()
{
	unsigned int target_addr,ret;
	
	dbg_log(DEBUG_BUFFER,"xmodem_boot_init ...\n");
	target_addr = XMODEM_TARGET_ADDRESS;
	xmodem_init(target_addr);
	if ((ret = xmodem_rx_file()) == 0)
	{
      if (getEfuseSecureControlData()!=NOT_SECURED_IMAGE)//support integrity format or integrity & encryption
      {
         secureBoot(target_addr);
      }
		rel_log("xmodem_rx_file: success, jumping to %x ...\n",target_addr);
		system_jump_to_address(target_addr);
	}
	else
	{		
		abort_log("xmodem_rx_file() - failure(err=%d)\n",ret);
	}
}

