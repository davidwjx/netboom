#ifndef __XMODEM_H__
#define __XMODEM_H__

/**
 *  xmodem.h : XMODEM-CRC receive module
 *
 *  Receive a file via the XMODEM-CRC protocol. 
 *
 *  Files: protocol\xmodem.h & protocol\xmodem.c
 *
 *  XMODEM-CRC is a simple file transfer protocol. The file is broken up into a 
 *  series of 128-byte data packets that are sent to the receiver. The packet 
 *  is prefixed with a 3 byte header containing a SOH character, 
 *  a "block number" from 0-255, and the "inverse" block number (255 minus 
 *  the block number). A 16-bit CRC is appended to the data.
 * 
 *  A single character response is sent by the receiver to control the flow of
 *  packets:
 *  - C   (0x43) : ASCII 'C' to start an XMODEM transfer using CRC
 *  - ACK (0x06) : Acknowledge 
 *  - NAK (0x15) : Not Acknowledge
 *  - EOT (0x04) : End of Transmission 
 *  
 *  http://en.wikipedia.org/wiki/XMODEM
 *  
 */

#include "defs.h"

/**
 *  Init  XMODEM-CRC protocol with target buffer
 */
extern void xmodem_init(unsigned int target_address);
/**
 *  Blocking function that receives a file using the XMODEM-CRC protocol.
 */
extern unsigned int xmodem_rx_file(void);


#endif
