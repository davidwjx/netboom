/* stereo_demo.h */

#ifndef STEREO_DEMO_H
#define STEREO_DEMO_H

#define EV_CORE_ID_ZERO 0
#define EV_CORE_ID_ONE  1
#define LEFT_CHANNEL 0
#define RIGHT_CHANNEL 1
#define STEREO_DEMO_MAX_INPUT_IMAGES_PER_CNN 2
#define STEREO_DEMO_MAX_TENSOR_POINTERS 10
#define STEREO_OUTPUT_SIZE                (65536)
#define STEREO_OUTPUT_CNN_BLOB            (564)
#define STEREO_OUTPUT_CNN_BLOB_MAX_SIZE   (2*STEREO_OUTPUT_CNN_BLOB)
#define STEREO_OUTPUT_PROCESS_BLOB        (4)

typedef enum	//	Message Type
{
   STEREO_DEMOG_START_ALG_E = 0,
   STEREO_DEMOG_STOP_ALG_E  = 1,
   STEREO_DEMOG_FRAME_ALG_E  = 2
} STEREO_DEMOG_opcodeE;


typedef struct	//	Algorithm Parameters
{
   UINT32 enableAlg;	//	enable/disable
   UINT32 startX;		//	ROI start 
   UINT32 startY;		//	ROI start
   UINT32 width;		//	ROI width
   UINT32 height;		//	ROI height
} STEREO_DEMOG_algConfigT;


typedef struct	//	GP to EV message structure
{
   STEREO_DEMOG_opcodeE                   opcode;
   STEREO_DEMOG_algConfigT                algConfig;
} STEREO_DEMOG_gpDspMsgProtocolT;

typedef struct
{
   UINT32 enable;
   UINT32 startX;
   UINT32 startY;
   UINT32 picWidth;
   UINT32 picHeight;
   // you may add more parameters here
} STEREO_DEMO_algInfoStructT;

typedef struct
{
   STEREO_DEMO_algInfoStructT algInfo;
   // you may add more parameters here
} STEREO_DEMO_hostGpMsgStructT;

/* STEREO_DEMO_MESSAGES */
typedef enum
{
    STEREO_DEMO_MSG_LOAD_NET = 0,
    STEREO_DEMO_MSG_LOAD_NET_ACK = 1,
    STEREO_DEMO_MSG_RELEASE_NET = 2,
    STEREO_DEMO_MSG_RELEASE_NET_ACK = 3,
    STEREO_DEMO_MSG_INJECT_IMAGES = 4,
    STEREO_DEMO_MSG_INJECT_IMAGES_ACK = 5
} STEREO_DEMO_messagesE;

/* STEREO_DEMO_ELEMENT_TYPES */
typedef enum 
{
    STEREO_DEMO_ELEMENT_TYPE_INT8 = 1,
    STEREO_DEMO_ELEMENT_TYPE_UINT8 = 2,
    STEREO_DEMO_ELEMENT_TYPE_12INT16 = 3, // 12 bit signed in 16 bit container
    STEREO_DEMO_ELEMENT_TYPE_12UINT16 = 4,  // 12 bit unsigned in 16 bit container.
    STEREO_DEMO_ELEMENT_TYPE_16INT16 = 5,   // 16 bit signed in 16 bit container
    STEREO_DEMO_ELEMENT_TYPE_16UINT16 = 6,   // 16 bit unsigned in 16 bit container.
    STEREO_DEMO_ELEMENT_TYPE_FLOAT32 = 7,   // 32 bit floating point
    STEREO_DEMO_ELEMENT_TYPE_FLOAT64 = 8    // 64 bit floating point
}STEREO_DEMO_ElementTypesE;

/* STEREO_DEMO_MESSAGES header */
typedef struct
{
    STEREO_DEMO_messagesE     msg_code;
    int                       msg_len;
}STEREO_DEMO_cnnMessageHdrT;

/* This structure contains the parameters for images to be processed */
typedef struct
{
    int channels;
    int width;
    int height;
    bool channel_swap;
    bool resize;
    bool yuv_format;
    bool tensor_input;
    int num_of_tensor_pointers;
    void* tensor_pointers[STEREO_DEMO_MAX_TENSOR_POINTERS];
    STEREO_DEMO_ElementTypesE element_type;
    void* images_ptr; // All images of the same type are located one after the other
}STEREO_DEMO_injectImageParamsT;


#endif   // STEREO_DEMO_H

