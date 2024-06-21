/* basic_demo.h */

#ifndef BASIC_DEMO_H
#define BASIC_DEMO_H

#define DATA_RGB_INPUT  0
#define DATA_RAW_OUTPUT 1 //FDK_data in sw_graph (<type>RAW_DATA)

typedef enum   // Message Type
{
   BASIC_DEMOG_START_ALG_E = 0,
   BASIC_DEMOG_STOP_ALG_E  = 1,
   BASIC_DEMOG_FRAME_ALG_E  = 2
} BASIC_DEMOG_opcodeE;


typedef struct // Algorithm Parameters
{
   UINT32 enableAlg; // enable/disable
   UINT32 startX;    // ROI start 
   UINT32 startY;    // ROI start
   UINT32 width;     // ROI width
   UINT32 height;    // ROI height
} BASIC_DEMOG_algConfigT;



typedef struct
{
   UINT32 gpUsec;
   UINT32 counter;
} BASIC_DEMO_gpHostMsgStructT;

typedef struct
{
   UINT32 enable;
   UINT32 startX;
   UINT32 startY;
   UINT32 picWidth;
   UINT32 picHeight;
   // you may add more parameters here
} BASIC_DEMO_algInfoStructT;

typedef struct
{
   BASIC_DEMO_algInfoStructT algInfo;
   // you may add more parameters here
} BASIC_DEMO_hostGpMsgStructT;

#endif   // BASIC_DEMO_H

