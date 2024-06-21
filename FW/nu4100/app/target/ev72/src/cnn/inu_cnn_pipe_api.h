/*
 * File - inu_cnn_api.h
 *
 * Copyright (C) 2014 All rights reserved to Inuitive  
 *
 */

#ifndef _INU_CNN_API_H_
#define _INU_CNN_API_H_

#ifdef __cplusplus
extern "C" {
#endif

//#define  INU_EVTRACER_ENABLED 	1

//#define malloc(len) 					inu_malloc(len, __FILE__, __LINE__)
#define malloc(len) 					inu_malloc(len)

//#define INU_FPGA_DEBUG 1
#define INU_DEBUG_SIZE 8192

#define EV_MAX_INPUT_IMAGES_PER_CNN 10
#define EV_CNN_MAX_NETS 10
#define EV_CNN_MAX_TAILS 30
#define EV_CNN_MAX_USER_FLOWS 10
#define EV_CNN_MAX_TENSOR_POINTERS 10

#define EV_CNN_MAX_PROCESS_FIFO 15

#define EV_CNN_MAX_TENSOR_PLANES 10

/* Processing flags */
#define PROC_FLAGS_YOLO_V3_POSTPROCESS 1
#define PROC_FLAGS_CONVERT_TO_888         2
#define PROC_FLAGS_CONVERT_12_BITS_TO_8   4
#define PROC_FLAGS_CONVERT_888_BITS_TO_8  8
#define PROC_FLAGS_FACE_SSD_POSTPROCESS   16
#define PROC_FLAGS_FREE_ON_SWITCH         32
#define PROC_FLAGS_YOLACT_POSTPROCESS     64
#define PROC_FLAGS_SWAP_YUV               128
#define PROC_FLAGS_YOLO_V7_POSTPROCESS    256

#define EV_CORE_ID_ZERO 0
#define EV_CORE_ID_ONE  1

#define CYCLES_PER_MS 820000 //830000 //850000
#define TIME_DEBUG_INFO  LOG_DEBUG_E

typedef struct 
{
    int number_of_injected_sets;
    int number_of_processed_sets;
    unsigned char *current_injected_output_ptr;
    unsigned char *total_injected_output_ptr;
    bool internal_allocation;
}EV_CNN_INJECT_INFO;

/* EV_CNN_MESSAGES */
typedef enum 
{
    EV_CNN_MSG_LOAD_NET = 0,
    EV_CNN_MSG_LOAD_NET_ACK = 1,
    EV_CNN_MSG_RELEASE_NET = 2,
    EV_CNN_MSG_RELEASE_NET_ACK = 3,
    EV_CNN_MSG_INJECT_IMAGES = 4,
    EV_CNN_MSG_INJECT_IMAGES_ACK = 5
} EV_CNN_MESSAGES;

/* EV_CNN_TYPE */
typedef enum  {
    EV_CNN_ELEMENT_TYPE_INT8 = 1, 
	 EV_CNN_ELEMENT_TYPE_UINT8 = 2, 
    EV_CNN_ELEMENT_TYPE_12INT16 = 3, // 12 bit signed in 16 bit container
    EV_CNN_ELEMENT_TYPE_12UINT16 = 4,  // 12 bit unsigned in 16 bit container.
    EV_CNN_ELEMENT_TYPE_16INT16 = 5,   // 16 bit signed in 16 bit container
    EV_CNN_ELEMENT_TYPE_16UINT16 = 6,   // 16 bit unsigned in 16 bit container.
    EV_CNN_ELEMENT_TYPE_FLOAT32 = 7,   // 32 bit floating point
    EV_CNN_ELEMENT_TYPE_FLOAT64 = 8    // 64 bit floating point
}EV_CNN_TYPE;

typedef enum 
{
    EV_CNN_STATUS_OK = 0,
    EV_CNN_STATUS_FAIL = 1,
}EV_CNN_STATUS_INT;

/* EV_MSG_HEADER */
typedef struct 
{
    EV_CNN_MESSAGES msg_code;
    int             msg_len;
}EV_MSG_HEADER;

/* EV_CNN_LOAD_NET  - This message is a request for loading a network to the evcnn */
typedef struct 
{
   EV_MSG_HEADER msg_header;
   int           net_id;
   intptr_t      bin_address;
   int           bin_size;     // In bytes
   int          pipe_depth;
   unsigned int ProcesssingFlags;
}EV_CNN_LOAD_NET;

typedef struct
{
   intptr_t      bin_address;
   int           bin_size;     // In bytes
   int           pipe_depth;
   unsigned int  ProcesssingFlags;
   int           channelSwap;
}CDNN_EVG_netInfoT;

/* EV_CNN_LOAD_NET_ACK - This message is sent when the network loading operation is done */
typedef struct 
{
	EV_MSG_HEADER msg_header;
	int net_id;
	int status;
}EV_CNN_LOAD_NET_ACK;

/* EV_CNN_RELEASE_NET - This message is a request to stop the current evcnn network */
typedef struct 
{
	EV_MSG_HEADER msg_header;
	int net_id;
	int jobDescriptor;
}EV_CNN_RELEASE_NET;

/* EV_CNN_RELEASE_NET_ACK -This message is sent when the network release operation is done */
typedef struct 
{
	EV_MSG_HEADER msg_header;
	int net_id;
	int status;
}EV_CNN_RELEASE_NET_ACK;

/* EV_CNN_INPUT_IMAGE - This structure contains the parameters for an image to be processed */
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
   void *tensor_pointers[EV_CNN_MAX_TENSOR_POINTERS];
   EV_CNN_TYPE element_type;
   void *image_ptr;
}EV_CNN_INPUT_IMAGE;

/* EV_CNN_PROCESS_IMAGES - This message is a request to process an image (or more if the network expects more than one image as input) */
typedef struct 
{
	EV_MSG_HEADER msg_header;
	int net_id;
   unsigned int frame_id;
	uint8_t* blob_output_address;
	uint8_t* blob_output_header;
   unsigned int jobDescriptor;
   EvIntCondType *intCondSignal;
   int number_of_images;
   EV_CNN_INPUT_IMAGE input_images[EV_MAX_INPUT_IMAGES_PER_CNN];
}EV_CNN_PROCESS_IMAGES;

typedef struct 
{
	int           net_id;
   intptr_t      bin_address;
	int           bin_size;     // In bytes
}EV_CNN_NET_INFO;

/* EV_CNN_TAIL */
typedef struct 
{
	int elementSize;
	int elementCount;
   int channels;
	int width;
	int height;
	float scale;		// Scale factor
	void *ptr;		// Pointer to output
}EV_CNN_TAIL;

/* EV_CNN_PROCESS_IMAGE_ACK - THis message is sent when a process of an image is done, and sends the CDNN output */
typedef struct 
{
	EV_MSG_HEADER msg_header;
	int status;
	int net_id;
   unsigned int frame_id;
	uint8_t* blob_output_header;
	unsigned int jobDescriptor;
	EvIntCondType *intCondSignal;
	int num_of_tails;
	EV_CNN_TAIL tails[EV_CNN_MAX_TAILS];
}EV_CNN_PROCESS_IMAGE_ACK;


/* EV_CNN_USER_FLOW_PROCESS */
typedef struct 
{
	EV_MSG_HEADER msg_header;
   int user_flow_id;
   unsigned int frame_id;
   int number_of_nets;
	EV_CNN_NET_INFO nets[EV_CNN_MAX_NETS];
	int number_of_images;
   EV_CNN_INPUT_IMAGE *input_images;
}EV_CNN_USER_FLOW_PROCESS;

/* EV_CNN_USER_FLOW_PROCESS_ACK */
typedef struct 
{
	EV_MSG_HEADER msg_header;
	int status;
	int user_flow_id;
   unsigned int frame_id;
	int num_of_tails;
	EV_CNN_TAIL tails[EV_CNN_MAX_TAILS];
}EV_CNN_USER_FLOW_PROCESS_ACK;

/* EV_CNN_FRAME_PREPROCESS_IND - Indicates that the passed preprocess, and can be released on the GP side. */
/*  The image processing has not been done yet. When the process is done, the EV_CNN_PROCESS_IMAGE_ACK message is sent */
typedef struct 
{
	EV_MSG_HEADER msg_header;
   unsigned int frame_id;
	bool dropped;                
}EV_CNN_FRAME_PREPROCESS_IND;

/* EV_CNN_INJECTED_IMAGE - This structure contains the parameters for images to be processed */
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
   void *tensor_pointers[EV_CNN_MAX_TENSOR_POINTERS];
   EV_CNN_TYPE element_type;
   void *images_ptr; // All images of the same type are located one after the other
}EV_CNN_INJECTED_IMAGE;

/* EV_CNN_INJECT_IMAGES - This message is a request to process multiple image sets (a set may have more than one image if the network expects more than one image as input) */
typedef struct 
{
	EV_MSG_HEADER msg_header;
	int net_id;
   unsigned int frame_id;
   uint8_t *blob_output_address;
	uint8_t* blob_output_header;
   unsigned int jobDescriptor;
   EvIntCondType *intCondSignal;
   int number_of_images_in_a_set; // Number of input images that the CNN expects *?
   int number_of_injected_sets; // Number of injected sets. (How many times to run the CNN)
   EV_CNN_INJECTED_IMAGE input_sets[EV_MAX_INPUT_IMAGES_PER_CNN];
}EV_CNN_INJECT_IMAGES;

typedef void (*ev_cnn_user_flow_func)(EV_CNN_USER_FLOW_PROCESS *, EV_CNN_USER_FLOW_PROCESS_ACK *);

//void* inu_malloc(int len, char *file, int line);
void* inu_malloc(int len);
void inu_free(void* ptr);
void ev_inu_set_preprocess_thread_cpu(int cpu);
int ev_inu_api_init(int coreId);
void inu_debug_init(void);
void ev_inu_handle_pipe(void);
int ev_inu_handle_message(EV_MSG_HEADER *message);
int ev_inu_register_user_flow_function(int user_flow_id, ev_cnn_user_flow_func user_flow_func);
int ev_inu_new_request(EV_CNN_PROCESS_IMAGES *new_req, EV_CNN_INJECT_INFO *inject_info);
int ev_inu_inject_request(EV_CNN_INJECT_IMAGES *inject_req);
bool ev_inu_is_pipe_free(void);
bool ev_inu_is_pending_free(void);
int inu_get_element_type_size(EV_CNN_TYPE element_type);
uint8_t * ev_inu_getBlobOutputAddress(void);
void FORMAT_CONVERTP_VYUY16ToRgb888Kernel_c(unsigned char *src, unsigned char *dst, int processW, int processH, int srcProcessStride, int dstProcessStride, int size_factor, bool do_yuv_split, uint8_t **yuv_split, int scaler_frame_pixels, bool yuv_swap);
EV_CNN_STATUS_INT ev_prepare_req_to_pipe(EV_CNN_PROCESS_IMAGES *process_image_msg, int pending_index);
int ev_inu_handle_load_net(EV_CNN_LOAD_NET *load_net_msg);
EV_CNN_PROCESS_IMAGE_ACK * ev_inu_handle_process_images(EV_CNN_PROCESS_IMAGES *process_image_msg);
void ev_inu_handle_release_net_pipe(EV_CNN_RELEASE_NET *release_net_msg);
void ev_inu_set_vision_proc_mode(void);
void ev_inu_reset_keep_alive_counter(void);
void ev_inu_set_keep_alive(bool state);
void set_yolact_input_image_size(int width, int height);

void inu_release_and_wait(int net_id);
UINT32 CDNN_EVG_netInfoGetChannelSwap(UINT32 netId);
UINT32 CDNN_EVG_netInfoGetProcesssingFlags(UINT32 netId);
#ifdef __cplusplus
}
#endif

#endif


