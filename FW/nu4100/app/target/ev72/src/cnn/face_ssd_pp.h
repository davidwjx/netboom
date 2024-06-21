#ifndef FACE_SSD_UTILS_H
#define FACE_SSD_UTILS_H

#include "cnn_hal.h"
#include "inu_utils.h"

//#define FACE_SSD_PP_PROCESS_TIME 1

#define CENTER_VARIANCE         0.1F
#define SIZE_VARIANCE           0.2F
#define CONF_THRESH             0.7F
#define SSD_NUM_OF_IMAGE_SIZES  3
#define IOU_THRESH              0.3
#define SSD_OBJECT_MAX_NUMBER   100

#if SSD_NUM_OF_IMAGE_SIZES == 3
#define MAX_TAIL_ELEMENTS       17640
#else
#define MAX_TAIL_ELEMENTS       70500
#endif

typedef struct
{
	unsigned int jobDescriptor;
	short *confs;
	short *locations;
	float confs_scale;
	float locations_scale;
	int image_rows;
	int image_cols;
	int num_of_classes;
	int num_of_elements;
	int net_id;
	unsigned int frame_id;
	void *blob_output_header;
	void *output_ptr;
} face_ssd_pp_infoT;

typedef struct {
	float min_x, max_x, min_y, max_y; // corners form
	Box center_box;                 //center form
	float confidence;
	int class_id;
} box_struct;

typedef struct {
	float x_center, y_center, w, h;
} prior_struct;

void face_ssd_pp_init(int coreId);
void face_ssd_set_input_size(int face_ssd_width);
void *ev_inu_face_ssd_post_thread(void *arg);
void ssd_post_process_prepare_priors(int ssd_image_size);
void ssd_post_process(short *locs, float locs_scale, short *confs, float conf_scale, int elements, int number_of_classes);


#endif

