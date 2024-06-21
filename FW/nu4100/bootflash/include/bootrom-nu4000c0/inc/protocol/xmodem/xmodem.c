

/**************************** INCLUDES ********************************/
#include "common.h"
#include "uart.h"
#include "xmodem.h"

/**************************** LOCAL DEFINES ********************************/

/* XMODEM protocol definitions */
#define XMODEM_PACKET_SIZE       133
#define XMODEM_DATA_SIZE         128
#define XMODEM_MAX_RETRIES       4

/* XMODEM - protocol timeouts */
#define XMODEM_C_TIMEOUT_IN_SEC			3
#define XMODEM_READ_TIMEOUT_IN_SEC		3

/* XMODEM flow control characters */
#define XMODEM_SOH               0x01 // Start of Header
#define XMODEM_EOT               0x04 // End of Transmission
#define XMODEM_ACK               0x06 // Acknowledge
#define XMODEM_NAK               0x15 // Not Acknowledge
#define XMODEM_C                 0x43 // ASCII “C”

/* XMODEM error list */
typedef enum
{
    XMODEM_NO_ERROR,
    XMODEM_RECEIVED_EOT,
    XMODEM_ERR_RECEIVED_NOTHING,
    XMODEM_ERR_RECEIVE_TIMEOUT,
    XMODEM_ERR_INCORRECT_HEADER,
    XMODEM_ERR_INCORRECT_PACKET_NUMBER,
    XMODEM_ERR_DUPLICATE_PACKET_NUMBER,
    XMODEM_ERR_INCORRECT_CRC
} xmodem_error_t;


// Macro to extract the high 8 bits of a 16-bit value (Most Significant Byte)
#define U16_HI8(data) ((unsigned char)((data>>8)&0xff))
// Macro to extract the low 8 bits of a 16-bit value (Least Significant Byte)
#define U16_LO8(data) ((unsigned char)(data&0xff))
// The number of timer ticks per second
#define SYSTEM_TICKS_PER_SEC   system_ticks_per_msec() *1000


/* Table of CRC constants - implements x^16+x^12+x^5+1 */
static const unsigned short crc16_tab[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

/**************************** LOCAL DATA ********************************/

// variable to keep track of current packet number
static unsigned char xmodem_packet_number;
// target buffer for receive data
static unsigned char *target_buffer;

// xmodem read timeout in ticks
static unsigned int xmodem_read_timeout_in_ticks;
// xmodem start timeout timeout in ticks
static unsigned int xmodem_start_timeout_in_ticks;

void xmodem_init(unsigned int target_address)
{
	target_buffer = (unsigned char *) target_address;
	xmodem_read_timeout_in_ticks = XMODEM_READ_TIMEOUT_IN_SEC * SYSTEM_TICKS_PER_SEC;
	xmodem_start_timeout_in_ticks = XMODEM_C_TIMEOUT_IN_SEC * SYSTEM_TICKS_PER_SEC;
}


static BOOL xmodem_wait_rx_char(unsigned char* data)
{
    /* try to read character as fast as possible */
    if(uart_boot_read(data))
		return TRUE;
	/* If no character in FIFO, wait for it for specified delay */
	if (system_tick_delay(xmodem_read_timeout_in_ticks, uart_boot_is_ready))
	{
		if(uart_boot_read(data))
			return TRUE;
	}
    return FALSE;
}

/*
   Function that verifies that packet checksum is correct
	Optimized CRC-XMODEM calculation.

	Polynomial: x^16 + x^12 + x^5 + 1 (0x1021)
	Initial value: 0x0

	This is the CRC used by the Xmodem-CRC protocol.
 */

unsigned short xmodem_calculate_crc16(unsigned char *buf, int len)
{
    int i;
    unsigned short cksum;

    cksum = 0;
    for (i = 0;  i < len;  i++)
    	cksum = crc16_tab[((cksum>>8) ^ *buf++) & 0xFF] ^ (cksum << 8);

    return cksum;
}

static BOOL xmodem_verify_crc16(unsigned short  crc, unsigned char crc_high, unsigned char crc_low)
{
	dbg_log(DEBUG_BUFFER, "In crc_check -crc=0x%x, hi=0x%x,low=0x%x\n",crc, U16_HI8(crc), U16_LO8(crc));
    // Compare received CRC with calculated value
    if(crc_high != U16_HI8(crc))
    {    
        return FALSE;
    }
    if(crc_low != U16_LO8(crc))
    {    
        return FALSE;
    }

    return TRUE;
}

/// Blocking function with a 1s timeout that tries to receive an XMODEM packet
static xmodem_error_t xmodem_rx_packet(void)
{
    unsigned char  i=0;
    unsigned char number_checksum1=0;
    unsigned char number_checksum2=0;
    unsigned char crc_checksum1=0;
    unsigned char crc_checksum2=0;
    unsigned char *data_buffer;
    unsigned short crc;
	dbg_log(DEBUG_BUFFER, "In xmodem_rx_packet \n");

	// Read packet number checksums
	if (!xmodem_wait_rx_char (&number_checksum1))
		return XMODEM_ERR_RECEIVED_NOTHING;
	dbg_log(DEBUG_BUFFER, "received number_checksum1=%d\n",number_checksum1);
	if (!xmodem_wait_rx_char (&number_checksum2))
		return XMODEM_ERR_RECEIVED_NOTHING;
	dbg_log(DEBUG_BUFFER, "received number_checksum2=%d\n",number_checksum2);
	data_buffer = target_buffer;
	for (i=0; i< XMODEM_DATA_SIZE; i++)
	{
		if (!xmodem_wait_rx_char (target_buffer))
			return XMODEM_ERR_RECEIVED_NOTHING;
		target_buffer++;
	}
	dbg_log(DEBUG_BUFFER, "xmodem_rx_packet: buffer read complete\n");

	// Read packet CRC-16
	if (!xmodem_wait_rx_char (&crc_checksum1))
			return XMODEM_ERR_RECEIVED_NOTHING;
	dbg_log(DEBUG_BUFFER, "received crc_checksum1=%d\n",crc_checksum1);	
	if (!xmodem_wait_rx_char (&crc_checksum2))
			return XMODEM_ERR_RECEIVED_NOTHING;
	dbg_log(DEBUG_BUFFER, "received crc_checksum2=%d\n",crc_checksum2);

    // Check packet number checksum
    if((number_checksum1 ^ number_checksum2) != 0xFF)
    {
		dbg_log(DEBUG_BUFFER, "In xmodem_rx_packet - Error packet index\n");
        return XMODEM_ERR_INCORRECT_PACKET_NUMBER;
    }

	dbg_log(DEBUG_BUFFER, "In xmodem_rx_packet - expected xmodem_packet_number = %d\n",xmodem_packet_number);
    // See if duplicate packet was received
    if(number_checksum1 == (xmodem_packet_number-1))
    {
		dbg_log(DEBUG_BUFFER, "In xmodem_rx_packet - Error packet duplicate\n");
        return XMODEM_ERR_DUPLICATE_PACKET_NUMBER;
    }

    // Make sure correct packet was received
    if(number_checksum1 != xmodem_packet_number)
    {
		dbg_log(DEBUG_BUFFER, "In xmodem_rx_packet - Error packet number\n");
        return XMODEM_ERR_INCORRECT_PACKET_NUMBER;
    }

    // Verify Checksum
    crc = xmodem_calculate_crc16(data_buffer, XMODEM_DATA_SIZE);
    if(!xmodem_verify_crc16(crc, crc_checksum1, crc_checksum2))
    {
		dbg_log(DEBUG_BUFFER, "In xmodem_rx_packet - Error packet checksum\n");
        return XMODEM_ERR_INCORRECT_CRC;
    }
	dbg_log(DEBUG_BUFFER, "In xmodem_rx_packet - success\n");
    return XMODEM_NO_ERROR;
}


unsigned int xmodem_rx_file(void)
{
    BOOL         			first_ack_sent = FALSE;
    unsigned char           data;
    xmodem_error_t error;
	unsigned int retry_count;


    // Reset packet number
    xmodem_packet_number = 1;
	dbg_log(DEBUG_BUFFER, "In xmodem_rx_file\n");
    // Repeat until transfer is finished or error count is exceeded

	// Wait and put 'C' till start xmodem transfer
	while(1)
	{
		// Send initial start character to start transfer (with CRC checking)
		dbg_log(DEBUG_BUFFER, "In xmodem_rx_file-  sending XMODEM_C\n");
		uart_boot_write(XMODEM_C);

		/* wait for the first available character to read */
		if (system_tick_delay(xmodem_start_timeout_in_ticks, uart_boot_is_ready))
			break;
	}

	retry_count = XMODEM_MAX_RETRIES;
	while (retry_count--)
	{		
		// See if character has been received		
		if (xmodem_wait_rx_char (&data))
		{				
			dbg_log(DEBUG_BUFFER, "read start of packet =%d\n",data);
			switch(data)
			{
				/* Start of transfer */
				case XMODEM_SOH:
					dbg_log(DEBUG_BUFFER, "received SOH\n");
					error = xmodem_rx_packet();
					dbg_log(DEBUG_BUFFER, "xmodem_rx_packet error=%d\n",error);
					break;

				/* End of transfer */
				case XMODEM_EOT:
					error = XMODEM_RECEIVED_EOT;
					dbg_log(DEBUG_BUFFER, "received EOT\n");
					break;
				default:
					error = XMODEM_ERR_INCORRECT_HEADER;
					break;
			}
		}
		else 
		{
			data = 0;
			error = XMODEM_ERR_RECEIVED_NOTHING;
		}

		// Handle state
		switch(error)
		{
			case XMODEM_NO_ERROR:
				// Acknowledge packet
				dbg_log(DEBUG_BUFFER, "In xmodem_rx_file - send XMODEM_ACK(0x06)\n");
				uart_boot_write(XMODEM_ACK);
				// Set flag to indicate that first packet has been correctly received
				first_ack_sent = TRUE;
				// Next packet
				xmodem_packet_number++;
				// Reset retry count
				retry_count = XMODEM_MAX_RETRIES;
				break;

			case XMODEM_RECEIVED_EOT:
				// Acknowledge EOT 
				// NOTE: BootROM will not make sure that sender has received ACK for EOT
				//		Such policy will save ~3s for EOT ACK timeout.
					dbg_log(DEBUG_BUFFER, "In xmodem_rx_file - EOT: sending XMODEM_ACK(0x06)\n");
				uart_boot_write(XMODEM_ACK);
				// File successfully transferred
				return 0;
			case XMODEM_ERR_DUPLICATE_PACKET_NUMBER:
				// Acknowledge packet
				uart_boot_write(XMODEM_ACK);
				break;

			case XMODEM_ERR_RECEIVED_NOTHING:
				// Fall through...
			case XMODEM_ERR_RECEIVE_TIMEOUT:
				// Fall through...
			case XMODEM_ERR_INCORRECT_HEADER:
				// Fall through...
			case XMODEM_ERR_INCORRECT_PACKET_NUMBER:
				// Fall through...
			case XMODEM_ERR_INCORRECT_CRC:
				// Fall through...
			default:
				if(first_ack_sent)
				{
					dbg_log(DEBUG_BUFFER, "XMODEM: sending NACK (err=0x%x)\n",error);
					// Send NAK
					uart_boot_write(XMODEM_NAK);
				}
				break;
		}
	}

    // File not successfully transferred
    return error;
}
