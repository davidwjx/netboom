#ifndef PRIVECY_SHUTTER_ARRAY_H
#define PRIVECY_SHUTTER_ARRAY_H


#define UVC_MAIN_BLACK32BIT 0x80048004 // 0x8004 black 16 bit
#define UVC_MAIN_WHITE 0x80FC          // white
#define UVC_MAIN_GREY1 0x8014          // grey1
#define UVC_MAIN_GREY2 0x80d4          // grey2..
#define UVC_MAIN_GREY3 0x8054          
#define UVC_MAIN_GREY4 0x8044          
#define UVC_MAIN_GREY5 0x8024    
#define UVC_MAIN_GREY6 0x80fb
#define UVC_MAIN_GREY7 0x80f9
#define UVC_MAIN_GREY8 0x80ec
#define UVC_MAIN_GREY9 0x80dc
#define UVC_MAIN_GREY10 0x8098
#define UVC_MAIN_GREY11 0x8006
#define UVC_MAIN_GREY12 0x8010
#define UVC_MAIN_GREY13 0x8030
#define UVC_MAIN_GREY14 0x8080
#define UVC_MAIN_GREY15 0x80c6



int PRIVACY_SHUTTER_ARRAYSG_BuildHDFile(UINT8 *ptr );
int PRIVACY_SHUTTER_ARRAYSG_BuildFullHDFile(UINT8 *ptr);

#endif
