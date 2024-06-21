/****************************************************************************
 *
 *   FileName: yolo_v3_pp.h
 *
 *   Author:  
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

#ifndef YOLO_PP_H
#define YOLO_PP_H


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cnn_hal.h"
#include "inu_utils.h"


//Default values (will be updated from network binary
#define INU_YOLO_NUMBER_OF_CLASSES 80
#define CONF_THRESHOLD 0.3F
#define NMS_THRESHOLD 0.2F
static int full_anchors[] =
{10, 13, 16, 30, 33, 23, 30, 61, 62, 45, 59, 119, 116, 90, 156, 198, 373, 326};

static const int yolo7_anchors[] =
{ 12, 16, 19, 36, 40, 28, 36, 75, 76, 55, 72, 146, 142, 110, 192, 243, 459, 401 };

#if __has_include("yolo_tiny_anchors.h")
#include "yolo_tiny_anchors.h"
#else
static int tiny_anchors[] =
{10, 14, 23, 27, 37, 58, 81, 82, 135, 169, 344, 319};
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/


//#define YOLO_V3_PP_PROCESS_TIME 1
#define MAX_LABEL_LENGTH 30
#define OBJECT_MAX_NUMBER 100
#define IN_SIZE int noutputs, int data_ch, int data_y, int data_x, const char *img_name
#define MAX_ANCHORS 9

enum ACTIVATION
{
    LOGISTIC,
    RELU,
    RELIE,
    LINEAR,
    RAMP,
    TANH,
    PLSE, 
    LEAKY,
    ELU, 
    LOGGY,
    STAIR,
    HARDTAN,
    LHTAN,
    SELU
};

typedef float Prob;
typedef float Result;

struct Layer {
    int batch;
    int side;
    int n;		// number of output maps in a layer.
    int outputs; 	// Total number of output pixels: maps x h x w
    int inputs;
    int sqrt;
    Result *output;
    int classes;
    int c, h, w;	// Input to a layer.
    int out_c, out_h, out_w;
    Result *delta;
    Result *cost, *biases;
    // Needed for the yolo layer.
    char type;
    int total;	// yolo num value
    int *mask;
    float *bias_updates;
    int truths;
    int coords;
    int version; // 3 or 7
    };
    
struct Detection{
    Box bbox;
    int classes;
    float *prob;
    float *mask;
    float objectness;
    int sort_class;
    };

struct Network {
    int h, w;	// network dimensions.
	short *input;
    Layer *layers;
    int n;	// # of layers.
    int version; // 3 or 7
    };


// For yolo, the result dimensions are 1470 x 1 x 1 (1470 "channels").
template <typename data_type>
     struct Blob_and_size {
        const char *name;       // blob name
        const char *layer_name; // layer generating this blob
        const char *layer_type; // type of layer
        unsigned size;          // total size in bytes
        unsigned element_size;  // element size in bytes (contains the pixel)
        unsigned pixel_size;    // blob pixel size in bits
        unsigned Z,Y,X;         // dimensions.
        data_type *blob;        // ptr to blob
        double scale;		// scale of blob
        int num_pixels() { return size/element_size; }
        };
typedef Blob_and_size<short> FixedOutput;

typedef struct
{
	unsigned int jobDescriptor;
	FixedOutput *outputs;
	int image_rows;
	int image_cols;
	int net_id;
	unsigned int frame_id;
	void *blob_output_header;
	void *output_ptr;
    int version; // 3 or 7
} YOLO_V3_PPG_infoT;


struct Image 
{
    int h,w;
    const char *name;
};

/****************************************************************************
 ***************      E X T E R N A L       D A T A           ***************
****************************************************************************/


extern int inu_yolo_network_dimension;
extern int inu_yolo_number_of_classes;
extern float inu_yolo_nms_threshold;
extern float inu_yolo_conf_threshold;

/****************************************************************************
 *************************     PROROTYPES    ********************************
 ****************************************************************************/

void yolo_fixed(void *_outputs, IN_SIZE);
void *ev_inu_yolo_post_thread(void *arg);
void set_full_yolo(void);
void set_full_yolo7(void);
void set_tiny_yolo(void);
void set_yolo_anchores(uintptr_t *bin_anchores, int number_of_anchores);
void YOLO_V3_PPG_init(int coreId);
void yolo_set_number_of_classes(int num_classes);
void yolo_set_nms(float nms);
void yolo_set_conf(float conf_th);
void yolo_set_num_of_anchors(int num_of_anchors); //number of [x,y] anchors pairs



#endif
