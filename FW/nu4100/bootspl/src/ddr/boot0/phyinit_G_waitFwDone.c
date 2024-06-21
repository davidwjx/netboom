// ===========================================================================
// This C code is derived from Inomize's SystemVerilog of "ddrm_phy_mailbox_get_mail_seq.sv"
//
// PUB sepc says to use the function dwc_ddrphy_phyinit_userCustom_G_waitFwDone().
// However, in
// /tools/snps/dw/dwc_lpddr4_multiphy_v2_tsmc12ffc18_2.00a/Latest/phyinit/Latest/software/lpddr4/src/
// there is no definition for that function.
// So we reverse-engineer Inomize's SystemVerilog code...



#include "phyinit_G_waitFwDone.h"
#include "nu4000_c0_gme_regs.h"
#include "nu4000_c0_ddrc_mp_regs.h"
#include "nu4000_c0_ddrc_regs.h"
#include "nu4000_c0_ddrp_regs.h"
#include "phyinit.h"
#include "debug.h"

// ===========================================================================
#define FW_COMPLETE_MSG_FAIL (0x00FF)
#define FW_COMPLETE_MSG_PASS (0x0007)
#define FW_DEBUG_MSG_PENDING (0x0008)


typedef struct
{
	char  message_index;
	char *message;
}phyinit_main_message_t;


const phyinit_main_message_t msg_list[17] = 
{
	{0x00, "End of initialization\n"									},
	{0x01, "End of fine write leveling\n" 								},
	{0x02, "End of read enable training\n"								},
	{0x03, "End of read delay center optimization\n"					},
	{0x04, "End of write delay center optimization\n" 					},
	{0x05, "End of 2D read delay/voltage center optimization\n"			},
	{0x06, "End of 2D write delay /voltage center optimization\n" 		},
	{0x07, "Training has run successfully (firmware complete)\n"		},
	{0x08, "Debug message is following\n"								},
	{0x09, "End of max read latency training\n"							},
	{0x0a, "End of read dq deskew training\n" 							},
	{0x0b, "Reserved\n"													},
	{0x0c, "End of LRDIMM Specific training (DWL, MREP, MRD and MWD)\n"	},
	{0x0d, "End of CA training\n" 										},
	{0xfd, "End of MPR read delay center optimization\n"				},
	{0xfe, "End of Write leveling coarse delay\n" 						},
	{0xff, "Training has failed (firmware complete)\n"					},
};


char* phyinit_message_get_string( unsigned int string_index )
{
	unsigned int tbl_index  = string_index;

	// String indexes currently available are 0x00-0x0d and 0xfd-0xff

	if(tbl_index < 0x0E)
	{
		return msg_list[tbl_index].message;
	}
	else if(tbl_index >= 0xFD)
	{
		return msg_list[tbl_index - 0xFD + 0x0E].message;
	}
	else
	{
		return (char*)0;
	}
}



unsigned int phyinit_G_waitFwDone(unsigned int train_2D)
{
  
  volatile unsigned int mail_message;
  
  GME_SPARE_0_SPARE_BITS_W(0xa07a0001); // write
  
  
  // Protocol initialization:
  
  // Register: DCT downstream mailbox protocol CSR.
  // Field: By setting this register to 0, the user acknowledges the receipt of the message.
  DDRP_APBONLY_DCTWRITEPROT_VAL = 0x1; // rf.DWC_DDRPHYA_APBONLY0.DctWriteProt.write(.status(status), .value(1));
  
  // Register UCT upstream mailbox protocol CSR.
  // Field: Reserved for PHY training firmware use (@@@@ so why did Inomize write to it?)
  DDRP_DRTUB_UCTWRITEPROT_VAL = 0x1; // rf.DWC_DDRPHYA_DRTUB0.UctWriteProt.write(.status(status), .value(1));
  
  
  // Poll the training F/W progress
  mail_message = 0;
#ifndef _ZEBU
  while((mail_message != FW_COMPLETE_MSG_PASS) && (mail_message != FW_COMPLETE_MSG_FAIL))
  {
    mail_message = get_mail(0); // read 16 bit message
    if(mail_message == FW_DEBUG_MSG_PENDING ) 	// print debug messages. Needed only if debug defined (e.g. DDR_MSG_COARSE_DEBUG) 
	{ 
		decode_streaming_message(train_2D);	
	} 
	else	// print main message		
	{
		//debug_printf(phyinit_message_get_string(mail_message));
	}
  }
#else
    mail_message = FW_COMPLETE_MSG_PASS;
#endif //_ZEBU
  GME_SPARE_0_SPARE_BITS_W(0xa07a0002); // write
  
  
  // Report training result
  if (mail_message == FW_COMPLETE_MSG_FAIL) 
  {
  	return 1;
    GME_SPARE_0_SPARE_BITS_W(0xa07a0003); // write
  }

  GME_SPARE_0_SPARE_BITS_W(0xa07a0004); // write
  return 0;
}


// ===========================================================================

// Note: 
// This function implements steps 9-13 as listed in
// uMCTL2 Databook Table 6-7 - "DWC_ddr_umctl2 and Memory Initialization with LPDDR4 mPHY_v2".

unsigned int get_mail(unsigned int mode) {
  
  unsigned int message; // return value
  volatile unsigned int rd_val;
  volatile unsigned int delay_cnt;
  
  // Poll for message from PMU (PHY microcontroller unit, same as uCtl)
  rd_val = 1;
  while (rd_val != 0) {
    // Register: UctShadowRegs = This is used for the mailbox protocol between the firmware and the system
    // Field:    UctWriteProtShadow = When set to 0, the PMU has a message for the user (Read Only)
    // [ Step9: "Poll the PUB register APBONLY.UctShadowRegs[0] =1'b0" ]
    rd_val = DDRP_APBONLY_UCTSHADOWREGS_UCTWRITEPROTSHADOW_R; // rf.DWC_DDRPHYA_APBONLY0.UctShadowRegs.UctWriteProtShadow.read(.status(status), .value(rd_val));
    delay_cnt=0;
    while (delay_cnt<100) delay_cnt++; // polling delay
  }
  GME_SPARE_0_SPARE_BITS_W(0xa07a00f1); // write
  
  // Read message / lower 16 bits
  // Register: UctWriteOnlyShadow = Read-only view of the CSR UctDatWriteOnly
  // Field: UctWriteOnlyShadow = Used to pass the message ID for major messages.
  //.............................Also used to pass the lower 16 bits for streaming messages.
  // [ Step10: "Read the PUB Register APBONLY.UctWriteOnlyShadow for training status" ]
  rd_val = DDRP_APBONLY_UCTWRITEONLYSHADOW_VAL; // rf.DWC_DDRPHYA_APBONLY0.UctWriteOnlyShadow.read(.status(status), .value(rd_val));
  rd_val &= 0x0000FFFF; // just make sure upper 16 bits are zero
  message = rd_val;
  
  // Read upper 16 bits
  if(mode == 1) {
    // Register: UctDatWriteOnlyShadow = Read-only view of the CSR UctDatWriteOnly
    // Field: UctDatWriteOnlyShadow = Not used in passing major messages.
    // ...............................Used to pass the upper 16 bits for streaming messages.
    rd_val = DDRP_APBONLY_UCTDATWRITEONLYSHADOW_VAL; // rf.DWC_DDRPHYA_APBONLY0.UctDatWriteOnlyShadow.read(.status(status), .value(rd_val));
    rd_val <<= 16; // (shifts left and adds zeros at the right end)
    message |= rd_val;
  }
  
  // Acknowledge the receipt of the message
  // Register: DCT downstream mailbox protocol CSR.
  // Field: By setting this register to 0, the user acknowledges the receipt of the message.
  // [ Step11: "Write the PUB Register APBONLY.DctWriteProt = 0" ]
  DDRP_APBONLY_DCTWRITEPROT_VAL = 0x0; // rf.DWC_DDRPHYA_APBONLY0.DctWriteProt.write(.status(status), .value(0));

  // Wait for indication from the PMU that message is cleared
  rd_val = 0; 
  while (rd_val == 0) {
    // Register: UctShadowRegs = This is used for the mailbox protocol between the firmware and the system
    // Field:    UctWriteProtShadow = When set to 0, the PMU has a message for the user (Read Only)
    // [ Step12: "Poll the PUB register APBONLY.UctShadowRegs[0] =1'b1" ]
    rd_val = DDRP_APBONLY_UCTSHADOWREGS_UCTWRITEPROTSHADOW_R; // rf.DWC_DDRPHYA_APBONLY0.UctShadowRegs.UctWriteProtShadow.read(.status(status), .value(rd_val));
    delay_cnt=0;
    while (delay_cnt<100) delay_cnt++; // polling delay
  }
  GME_SPARE_0_SPARE_BITS_W(0xa07a00f2); // write

  // Final handshake - complete the protocol
  // Register: DCT downstream mailbox protocol CSR.
  // Field: By setting this register to 0, the user acknowledges the receipt of the message.
  // Step13: "Write the PUB Register APBONLY.DctWriteProt = 1" ]
  DDRP_APBONLY_DCTWRITEPROT_VAL = 0x1; // rf.DWC_DDRPHYA_APBONLY0.DctWriteProt.write(.status(status), .value(1));
  
  return message;
  
}



#define putchar(c) debug_writeb(c)
extern void putchar(int c);
#include <stdarg.h>
static void printchar(char **str, int c)
{		
if (str)	
{
		**str = c;		++(*str);	
}	else	{
		putchar(c);	
}
}
#define PAD_RIGHT 1
#define PAD_ZERO 2
static int prints(char **out, const char *string, int width, int pad)
{	
register int pc = 0, padchar = ' ';	if (width > 0) {		register int len = 0;		register const char *ptr;		for (ptr = string; *ptr; ++ptr) ++len;		if (len >= width) width = 0;		else width -= len;		if (pad & PAD_ZERO) padchar = '0';	}	if (!(pad & PAD_RIGHT)) {		for ( ; width > 0; --width) {			printchar (out, padchar);			++pc;		}	}	for ( ; *string ; ++string) {		printchar (out, *string);		++pc;	}	for ( ; width > 0; --width) {		printchar (out, padchar);		++pc;	}	return pc;
}
/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12
static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{	
char print_buf[PRINT_BUF_LEN];	register char *s;	register int t, neg = 0, pc = 0;	register unsigned int u = i;	if (i == 0) {		print_buf[0] = '0';		print_buf[1] = '\0';		return prints (out, print_buf, width, pad);	}	if (sg && b == 10 && i < 0) {		neg = 1;		u = -i;	}	s = print_buf + PRINT_BUF_LEN-1;	*s = '\0';	while (u) {		t = u % b;		if( t >= 10 )			t += letbase - '0' - 10;		*--s = t + '0';		u /= b;	}	if (neg) {		if( width && (pad & PAD_ZERO) ) {			printchar (out, '-');			++pc;			--width;		}		else {			*--s = '-';		}	}	return pc + prints (out, s, width, pad);
}


static int print(char **out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];
	for (; *format != 0; ++format) {		if (*format == '%') 
	{
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') 
			{				++format;				pad = PAD_RIGHT;
			}			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = (char *)va_arg( args, int );
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;			}			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';	va_end( args );
	return pc;
}


int sprintf(char *out, const char *format, ...)
{        
	va_list args;                
	va_start( args, format );        
	return print( &out, format, args );
}


void inu_printf (const char *format, int myarg0, int myarg1, int myarg2, int myarg3, int myarg4, int myarg5, int myarg6, int myarg7, int myarg8, int myarg9, int myarg10, int myarg11, int myarg12, int myarg13, int myarg14, int myarg15)
{
   char new_str[1024];
   sprintf (new_str, format, myarg0, myarg1, myarg2, myarg3, myarg4, myarg5, myarg6, myarg7, myarg8, myarg9, myarg10, myarg11, myarg12,  myarg13,  myarg14, myarg15);
   debug_printf (new_str);
}

// ===========================================================================
// This function is translated from Inomize task.
// It seems read some PMU messages and just translate them into UVM_INFO.
// 
// But, maybe the reads themselves are required for the correct operation
// of the training logic, so we run this function, only without printing...
// @@@@ what is the meaning of the messages contents?
void decode_streaming_message(unsigned int train_2D) 
{
  
  int args[32];
  int string_index;
  int i = 0;
  
  string_index = get_mail(1); // read 32 bit message (why? we only use lower 16 bits anyway...)
  while (i < (string_index & 0x0000ffff)) 
  {
    args[i] = get_mail(1); // read 32 bit message
    i += 1;
    // @@@@ I guess they assume "i" will never be more than 15...
  }
  
  //   debug_printf(phyinit_debug_get_string( string_index ), args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7], args[8],args[9],args[10],args[11],args[12],args[13],args[14],args[15]);
  if (train_2D == 0)
      debug_printf("%s",phyinit_debug_get_string( string_index ));
  else
      debug_printf("%s",phyinit_debug_get_string_2D( string_index ));
}



