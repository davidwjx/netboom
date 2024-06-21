#ifndef _DDR_APP_H_
#define _DDR_APP_H_

#define DDR_SIZE 		(0x40000000) // 1G bytes

// Assume that our kernel+rootfs uncompressed is less than 21Mb
// Assume that nu3000.zip is less than 16Mb
#define TEXT_OFFSET (0x1600000) // 22Mb
#define DTB_OFFSET  (0x1500000) // 21Mb, note: dtb and text should be in used memory region
#define CEVA_OFFSET (TEXT_OFFSET + 0x800000) //assume initial kernel image is not larger then 8MB
#define FS_OFFSET 0x4000000
#ifndef APP_OFFSET
	#define APP_OFFSET  (DDR_SIZE - 0x4000000)  
#endif	

void ddr_pll_config(unsigned int freq);
unsigned int ddrm_umctl_init_seq(unsigned int freq);
#endif
