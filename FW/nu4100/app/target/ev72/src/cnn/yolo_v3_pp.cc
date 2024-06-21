/****************************************************************************
 *
 *   FileName: yolo_v3_pp.cc
 *
 *   Author:  
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/
#include "client_api.h"
#include "yolo_v3_pp.h"
#include "yolo_logistic_lut.h"
#include "inu_logistic.h"
#include "inu_utils.h"
#include <cnn_dev.h>

#ifndef INU_HAPS
#include "inu2_internal.h"
#include "log.h"
#include "sched.h"
#include "ictl_drv.h"
#include "ev_init.h"
#endif

#define USE_OCL_LOGISTIC 1

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

static int Max_int = 6;
static int Max_int_scaled = 0;
static int Frac_bits = 10;
static int Frac_bits_power = 1 << Frac_bits;



static int                  num_of_objects = 0;
static EvCnnBoundingBox     object_boxes[OBJECT_MAX_NUMBER];
static const char           YOLO = 'y';	// layer type.
static const bool           enable_trace = false;
static Layer                layers[3];
static bool                 layers_initialized = false;
static bool                 full_yolo_mode = true;
static int                  anchors[MAX_ANCHORS*2];
CLIENT_APIG_msgQueT			yolo_v3_msgQue;

/****************************************************************************
***************                 E X T E R N                   ***************
****************************************************************************/

/****************************************************************************
**************   G L O B A L      F U N C T I O N S           ***************
****************************************************************************/


void YOLO_V3_PPG_init(int coreId)
{
    int rc;
    EvThreadType t;

	yolo_v3_msgQue.maxMsgs = CLIENT_APIG_QUE_MAX_ELEMENTS;
	yolo_v3_msgQue.msgSize = sizeof(YOLO_V3_PPG_infoT);

	if (CLIENT_APIG_createMsgQue(&yolo_v3_msgQue, 0) != SUCCESS_E)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "yoloV3 msg queue create failed\n");
		return;
	}
   
	t = evNewThread(ev_inu_yolo_post_thread, EVTH_INIT_CPU, coreId, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_PRIORITY, YOLO_PP_THREAD_PRIORITY, EVTH_INIT_PREEMPTIBLE, 1, EVTH_INIT_LAST);
	if (t == NULL)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to open ev_inu_yolo_post_thread thread\n");
		return;
	}
}

/* ev_inu_yolo_post_thread */
void *ev_inu_yolo_post_thread(void *arg)
{
#ifndef INU_HAPS
	inu_cdnn_data__hdr_t        *resultsHeaderP;
	inu_cdnn_data__tailHeaderT  *tailHeaderP;
	int status;
	UINT32						localThreadParamsSize;
	YOLO_V3_PPG_infoT           yolo_v3_post_params;

//		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "evGetStackUsage: EVTH_THREADS_PER_CPU: %d\n", evthThreadsPerCpu);
//		for (int i = 0; i < evthThreadsPerCpu; i++)
//	    {
//			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "evGetStackUsage: 0:%d 1:%d\n", evGetStackUsage(0, i), evGetStackUsage(1, i));
//		}
    LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "YOLO_V3 post process thread started on core %d\n", evGetCpu());

	while (1)
    {
		localThreadParamsSize = sizeof(yolo_v3_post_params);
		status = CLIENT_APIG_recvMsg(&yolo_v3_msgQue, (UINT8*)&yolo_v3_post_params, &localThreadParamsSize, 0 /*not used*/);
		if (status == SUCCESS_E) {
			//			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_yolo_post_thread started on core %d frame:%d\n", evGetCpu(), post_params.frame_id);

#ifdef YOLO_V3_PP_PROCESS_TIME
			uint64_t yolo_clock1, yolo_clock2;
			yolo_clock1 = getRTC();
#endif

			yolo_fixed(yolo_v3_post_params.outputs, 3, 3, yolo_v3_post_params.image_rows, yolo_v3_post_params.image_cols, "image_name");

#ifdef YOLO_V3_PP_PROCESS_TIME
		yolo_clock2 = getRTC();
		float ev_clock_rate = EV_INITG_getEvFreqMhz();
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "yolo post time (%d) is cycles:%lld %fms\n", (int)ev_clock_rate, (yolo_clock2 - yolo_clock1), (float)(yolo_clock2 - yolo_clock1) / (ev_clock_rate*1000));
#endif

			/* Fill in the result header and result buffer */
			resultsHeaderP = (inu_cdnn_data__hdr_t *)yolo_v3_post_params.blob_output_header;
			tailHeaderP = resultsHeaderP->tailHeader;
			resultsHeaderP->network_id = yolo_v3_post_params.net_id;
			resultsHeaderP->engineType = INU_LOAD_NETWORK__EV61_E;
			resultsHeaderP->frameId = yolo_v3_post_params.frame_id;
			resultsHeaderP->numOfTails = 1;
			resultsHeaderP->tailHeader[0].elementSize = sizeof(EvCnnBoundingBoxes);
			resultsHeaderP->tailHeader[0].elementCount = num_of_objects;
			resultsHeaderP->tailHeader[0].inputs = num_of_objects;
			resultsHeaderP->tailHeader[0].width = 0;
			resultsHeaderP->tailHeader[0].height = 0;
			resultsHeaderP->tailHeader[0].cnnFields.tailSynopsysFields.scale = 1;
			resultsHeaderP->offsetToBlob[0] = 0;

			int *out_p;
			out_p = (int *)yolo_v3_post_params.output_ptr;
			out_p[0] = num_of_objects;
			if (num_of_objects)
			{
				memcpy(&out_p[1], object_boxes, num_of_objects * sizeof(EvCnnBoundingBox));
			}
			resultsHeaderP->totOutputSize = sizeof(int) + num_of_objects * sizeof(EvCnnBoundingBox);
			if (resultsHeaderP->totOutputSize == sizeof(int))
			{ // For some reason if the length is only 4, it does not reach the host
				resultsHeaderP->totOutputSize += sizeof(EvCnnBoundingBox);
			}

			evDataCacheFlushLines(resultsHeaderP, resultsHeaderP + sizeof(inu_cdnn_data__hdr_t));
			evDataCacheFlushLines(yolo_v3_post_params.output_ptr, (unsigned char *)(yolo_v3_post_params.output_ptr) + resultsHeaderP->totOutputSize);
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "tails num %d total output size %d cache line %d\n", resultsHeaderP->numOfTails, resultsHeaderP->totOutputSize, evDataCacheLineSize());
			SCHEDG_pushFinishedJob(yolo_v3_post_params.jobDescriptor);
			evIntCondSignal(&SCHEDG_Condition);
#endif
			//LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_yolo_post_thread ended on core %d\n", evGetCpu());
		}
	}
    return(NULL);
}



/****************************************************************************
**************   L O C A L      F U N C T I O N S             ***************
****************************************************************************/

Box get_yolo_box(float *x, float *biases, int n, int index, int i, int j, int lw, int lh, int w, int h, int stride)
{
//	0 && printf("!get yolo box n %d index %d i %d j %d lw %d lh %d w %d h %d stride %d\n",
//	 n, index, i, j, lw, lh, w, h, stride);
    Box b;
    b.x = (i + x[index + 0*stride]) / lw;
    b.y = (j + x[index + 1*stride]) / lh;

    b.w = exp(x[index + 2*stride]) * biases[2*n]   / w;
    b.h = exp(x[index + 3*stride]) * biases[2*n+1] / h;
    return b;
}

Box get_yolo7_box(float* x, float* biases, int n, int index, int i, int j, int lw, int lh, int w, int h, int stride) {
    //0 && printf("!get yolo box n %d index %d i %d j %d lw %d lh %d w %d h %d stride %d\n",
    //    n, index, i, j, lw, lh, w, h, stride);
    Box b;
    int self_stride;
    float tmp;

    if (w == 640) {
        switch (lw) {
        case 80:
            self_stride = 8;
            break;
        case 40:
            self_stride = 16;
            break;
        case 20:
            self_stride = 32;
            break;
        default:
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "get_yolo7_box: unexpected lw: %d\n", lw);
            break;
        }
    }
    else if (w == 320) {
        switch (lw) {
        case 40:
            self_stride = 8;
            break;
        case 20:
            self_stride = 16;
            break;
        case 10:
            self_stride = 32;
            break;
        default:
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "get_yolo7_box: unexpected lw: %d\n", lw);
            break;
        }
    }

    /*
        y[..., 0:2] = (y[..., 0:2] * 2. - 0.5 + self.grid[i]) * self.stride[i]  # xy
        y[..., 2:4] = (y[..., 2:4] * 2) * *2 * self.anchor_grid[i]  # wh
    */
    b.x = ((x[index + 0 * stride] * 2. - 0.5 + i) * self_stride) / w;
    b.y = ((x[index + 1 * stride] * 2. - 0.5 + j) * self_stride) / h;

    tmp = x[index + 2 * stride] * 2.;
    b.w = ((tmp * tmp) * biases[2 * n]) / w;

    tmp = x[index + 3 * stride] * 2.;
    b.h = ((tmp * tmp) * biases[2 * n + 1]) / h;

    //printf("index: %d x:%f y:%f w:%f h:%f self_stride:%d lw:%d n:%d biases: %f %f\n", index, b.x, b.y, b.w, b.h, self_stride, lw, n, biases[2*n], biases[2*n+1]);
    return b;
}

void avg_flipped_yolo(Layer l)
{
    int i,j,n,z;
    float *flip = l.output + l.outputs;
    for (j = 0; j < l.h; ++j) 
    {
        for (i = 0; i < l.w/2; ++i)
        {
            for (n = 0; n < l.n; ++n)
            {
                for (z = 0; z < l.classes + 4 + 1; ++z)
                {
                    int i1 = z*l.w*l.h*l.n + n*l.w*l.h + j*l.w + i;
                    int i2 = z*l.w*l.h*l.n + n*l.w*l.h + j*l.w + (l.w - i - 1);
                    float swap = flip[i1];
                    flip[i1] = flip[i2];
                    flip[i2] = swap;
                    if (z == 0)
                    {
                        flip[i1] = -flip[i1];
                        flip[i2] = -flip[i2];
                    }
                }
            }
        }
    }
    for (i = 0; i < l.outputs; ++i)
    {
        l.output[i] = (l.output[i] + flip[i])/2.;
    }
}

static int entry_index(Layer l, int batch, int location, int entry)
{
    int n =   location / (l.w*l.h);
    int loc = location % (l.w*l.h);
    return batch*l.outputs + n*l.w*l.h*(4+l.classes+1) + entry*l.w*l.h + loc;
}

int get_yolo_detections(Layer l, int w, int h, int netw, int neth, float thresh, int *map, int relative, Detection *dets)
{
    int i,j,n;
    float *predictions = l.output;
    int count = 0;

    
    if (l.batch == 2) avg_flipped_yolo(l);

    for (i = 0; i < l.w*l.h; ++i)
    {
        int row = i / l.w;
        int col = i % l.w;
        for (n = 0; n < l.n; ++n)
        {
            int obj_index  = entry_index(l, 0, n*l.w*l.h + i, 4);
            float objectness = predictions[obj_index];
            if (objectness <= thresh) continue;
            int box_index  = entry_index(l, 0, n*l.w*l.h + i, 0);
            if (l.version == 7) {
                dets[count].bbox = get_yolo7_box(predictions, l.biases, l.mask[n], box_index, col, row, l.w, l.h, netw, neth, l.w * l.h);
            }
            else {
                dets[count].bbox = get_yolo_box(predictions, l.biases, l.mask[n], box_index, col, row, l.w, l.h, netw, neth, l.w * l.h);
            }
            dets[count].objectness = objectness;
            dets[count].classes = l.classes;
            for (j = 0; j < l.classes; ++j)
            {
                int class_index = entry_index(l, 0, n*l.w*l.h + i, 4 + 1 + j);
                float prob = objectness*predictions[class_index];
                dets[count].prob[j] = (prob > thresh) ? prob : 0;
            }
            ++count;
        }
    }
    return count;
}


float logistic_activate(short x, float resolution)
{
	float y;

	if (x > Max_int_scaled)
    {
		y = 1;
	}
	else if (x < -Max_int_scaled)
    {
		y = 0;
	}
	else
    {
		y = logistic_lut[(int)(x * resolution + 0.5) + Frac_bits_power];
	}
	return(y);
}


float activate(short x, ACTIVATION a, float resolution)
{
#if 1
    return logistic_activate(x, resolution);
#else
    switch(a)
    {
        case LOGISTIC:
            return logistic_activate(x, resolution);
        default:
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "activation not supported\n");
            exit(1);
    }
#endif
}

/* activate_array - short inputs */
void activate_array(short *in_x, float *out_x, const int n, const ACTIVATION a, float resolution) {
	int i;
	for (i = 0; i < n; ++i) {
		//out_x[i] = activate(in_x[i], a, resolution);
        out_x[i] = logistic_activate(in_x[i], resolution);
	}
}

/* activate_array - char inputs */
void activate_array(signed char *in_x, float *out_x, const int n, const ACTIVATION a, float resolution) {
	int i;
	for (i = 0; i < n; ++i) {
		//[i] = activate(in_x[i], a, resolution);
        out_x[i] = logistic_activate(in_x[i], resolution);
	}
}

void forward_yolo_layer(const Layer l, Network net, float scale, int element_size)
{
    int b, n, last_index;
	float scale_factor = 1. / scale;

    // Copy input to output.  net.input is the input from the layer feeding
    // the yolo layer.
    enable_trace && printf("!fwd yolo layer.  outputs=%d n=%d\n",l.outputs,l.n);
    const int cnt = enable_trace ? 10 : 0;
    for (int i = 0; i < cnt; i++)
    {
		printf("!input[%d] = %8.4f\n", i, l.output[i]);
    }

    b = 0;	// batch is always 1
    for (n = 0; n < l.n; ++n)
    {
		float resolution = (float)(Frac_bits_power / Max_int) / scale;
		Max_int_scaled = (int)(Max_int * scale);

		//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "res:%f max_int_scaled:%d\n", resolution, Max_int_scaled);

        int index = entry_index(l, b, n*l.w*l.h, 0);
        enable_trace && printf("!n=%d ix=%d\n",n,index);

        switch (element_size) {
        case 1:
        {
            signed char* p_char = (signed char*)net.input;
#ifdef USE_OCL_LOGISTIC
            evDataCacheFlushInvalidateLines(l.output + index, l.output + index + 2 * l.w * l.h);
            inu_logistic(p_char + index, l.output + index, 2 * l.w * l.h, scale, sizeof(char), true, 1.);
#else
            activate_array(p_char + index, l.output + index, 2 * l.w * l.h, LOGISTIC, resolution);
#endif
        }
        break;
        case 2:
#ifdef USE_OCL_LOGISTIC
            evDataCacheFlushInvalidateLines(l.output + index, l.output + index + 2 * l.w * l.h);
            inu_logistic(net.input + index, l.output + index, 2 * l.w * l.h, scale, sizeof(short), true, 1.);

#if 0
            {
                static int times = 200;
                int max_diff_index, err_count = 0;
                float diff, max_diff = -99999;

                if (times) {
                    // compare
                    float* ppp = (float*)malloc(2 * l.w * l.h * sizeof(float));
                    activate_array(net.input + index, ppp, 2 * l.w * l.h, LOGISTIC, resolution);

                    for (int i = 0; i < 2 * l.w * l.h; i++) {
                        diff = abs(ppp[i] - l.output[i + index]);
                        if (diff > 0.1) {
                            err_count++;
                            if (diff > max_diff) {
                                max_diff = diff;
                                max_diff_index = i;
                            }
                        }
                    }
                    times--;
                    if (err_count > 0) {
                        LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, " ---> short err_count: %d  MAX:%f  w:%d h:%d\n", err_count, max_diff, l.w, l.h);
                        if (err_count) {
                            short* orig_p = net.input + index;
                            LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "orig:%d scale:%f benny:%f scalar:%f\n", orig_p[max_diff_index + index], scale, l.output[max_diff_index + index], ppp[max_diff_index]);
                            //float* tmp_p = l.output + index;
                           // LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "OUTPUT: %f %f %f %f %f %f %f %f %f %f\n", tmp_p[0], tmp_p[1], tmp_p[2], tmp_p[3], tmp_p[4], tmp_p[5], tmp_p[6], tmp_p[7], tmp_p[8], tmp_p[9]);
                            //LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "in:%p out:%p\n", net.input + index, l.output + index);
                        }
                    }
                    free(ppp);
                }
            }
#endif // The debug compare code

#else
				activate_array(net.input + index, l.output + index, 2 * l.w*l.h, LOGISTIC, resolution);
#endif
				break;
		default:
				printf("forward_yolo_layer: Unexpected element_size: %d\n", element_size);
				break;
		}

		last_index = index + 2 * l.w*l.h;
		index = entry_index(l, b, n*l.w*l.h, 4);

		switch (element_size) {
			case 1:
				{
					signed char *p_char = (signed char *)net.input;
					/* Convert the gap from char to float */
					for (int j = last_index + 1; j < index; j++) {
						l.output[j] = (float)p_char[j] * scale_factor;
					}
#ifdef USE_OCL_LOGISTIC
                    evDataCacheFlushInvalidateLines(l.output + index, l.output + index + (1 + l.classes) * l.w * l.h);
                    inu_logistic(p_char + index, l.output + index, (1 + l.classes) * l.w * l.h, scale, sizeof(char), true, 1.);
#else
					activate_array(p_char + index, l.output + index, (1 + l.classes)*l.w*l.h, LOGISTIC, resolution);
#endif
				}
				break;
			case 2:
				/* Convert the gap from short to float */
				for (int j = last_index + 1; j < index; j++) {
					l.output[j] = (float)net.input[j] * scale_factor;
				}
#ifdef USE_OCL_LOGISTIC
                evDataCacheFlushInvalidateLines(l.output + index, l.output + index + (1 + l.classes) * l.w * l.h);
                inu_logistic(net.input + index, l.output + index, (1 + l.classes) * l.w * l.h, scale, sizeof(short), true, 1.);
#else
				activate_array(net.input + index, l.output + index, (1 + l.classes)*l.w*l.h, LOGISTIC, resolution);
#endif
				break;
			default:
				printf("forward_yolo_layer: Unexpected element_size: %d\n", element_size);
				break;
		}
    }
}

void forward_yolo7_layer(const Layer l, Network net, float scale, int element_size) {
    int i, j, n;
    float scale_factor = 1. / scale;

    // Copy input to output.  net.input is the input from the layer feeding
    // the yolo layer.
    float resolution = (float)(Frac_bits_power / Max_int) / scale;
    Max_int_scaled = (int)(Max_int * scale);

    switch (element_size) {
    case 1:
    {
        signed char* p_char = (signed char*)net.input;
#ifdef USE_OCL_LOGISTIC
        evDataCacheFlushInvalidateLines(l.output, l.output + l.outputs);
        inu_logistic(p_char, l.output, l.outputs, scale, sizeof(char), true, 1.);
#else
        activate_array(p_char, l.output, l.inputs, LOGISTIC, resolution);
#endif
    }
    break;
    case 2:
#ifdef USE_OCL_LOGISTIC
        evDataCacheFlushInvalidateLines(l.output, l.output + l.outputs);
        inu_logistic(net.input, l.output, l.outputs, scale, sizeof(short), true, 1.);
#else
        activate_array(net.input, l.output, l.inputs, LOGISTIC, resolution);
#endif
        break;
    default:
        printf("forward_yolo7_layer: Unexpected element_size: %d\n", element_size);
        break;
    }
}

void fill_network_boxes(Network *net, int w, int h, float thresh, float hier, int *map, int relative, Detection *dets)
{
    int j;
    for (j = 0; j < net->n; ++j)
    {
        Layer l = net->layers[j];
        if (l.type == YOLO)
        {
            enable_trace && printf("filling nboxes for yolo j=%d netw %d h %d\n",j, net->w,net->h);
            int count = get_yolo_detections(l, w, h, net->w, net->h, thresh, map, relative, dets);
            dets += count;
        }
    }
}

int yolo_num_detections(Layer l, float thresh)
{
    int i, n;
    int count = 0;
    for (i = 0; i < l.w*l.h; ++i)
    {
        for (n = 0; n < l.n; ++n)
        {
            int obj_index  = entry_index(l, 0, n*l.w*l.h + i, 4);
            if (l.output[obj_index] > thresh)
            {
                enable_trace && printf("!o %f > %f\n",l.output[obj_index] , thresh);
                ++count;
            }
        }
    }
    enable_trace && printf("!I have %d detections\n",count);
    return count;
}

int num_detections(Network *net, float thresh)
{
    int i;
    int s = 0;
    for (i = 0; i < net->n; ++i)
    {
        Layer l = net->layers[i];
        if (l.type == YOLO)
        {
            s += yolo_num_detections(l, thresh);
        }
    }
    return s;
}

float* yolo_falloc(int elements)
{
    float* ptr;
    ptr = (float*)evMemAlloc(elements * sizeof(float), EV_MR_USE_CNDC, -1); // Add memory before and after, as we do invalidate to this area.
    return(ptr);
}

inline float *falloc(int a)
{
    0 && printf("!falloc %d by %d\n",a,sizeof(float));
    return (float*)calloc(a,sizeof(float)); 
}

Detection *make_network_boxes(Network *net, float thresh, int *num)
{
    Layer l = net->layers[net->n - 1];
    int i;
    int nboxes = num_detections(net, thresh);
    if (num) *num = nboxes;
    Detection *dets = (Detection *)calloc(nboxes, sizeof(Detection));
    for (i = 0; i < nboxes; ++i)
    {
        dets[i].prob = falloc(l.classes);
        if (l.coords > 4)
        {
            dets[i].mask = falloc(l.coords-4);
        }
    }
    return dets;
}

Detection *get_network_boxes(Network *net, int w, int h, float thresh, float hier, int *map, int relative, int *num)
{
    Detection *dets = make_network_boxes(net, thresh, num);
    fill_network_boxes(net, w, h, thresh, hier, map, relative, dets);
    return dets;
}

Layer make_yolo_layer(int batch, int w, int h, int n, int total, int *mask, int classes)
{
    0 && printf("!make yolo layer w %d h %d n %d total %d\n",w,h,n,total);
    // [yolo] parm:
    // num => total
    // number of masks => n
    // anchors go into l.biases, I suppose a convenient place to store them.
    // There are 2*num anchor pairs.
    int i;
    Layer l = {0};
    l.type = YOLO;

    l.n = n;
    l.total = total;
    l.batch = batch;
    l.h = h;
    l.w = w;
    l.c = n*(classes + 4 + 1);
    l.out_w = l.w;
    l.out_h = l.h;
    l.out_c = l.c;
    l.classes = classes;
    l.cost = falloc(1);
    l.biases = falloc(total*2);
    if (mask)
    {
        l.mask = mask;
    }
    else
    {
        l.mask = (int*)calloc(n, sizeof(int));
        for (i = 0; i < n; ++i)
        {
            l.mask[i] = i;
        }
    }

	//for (volatile int i = 0; i < n; i++) {
	//	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Mask %d) - %d\n", i, l.mask[i]);
	//}

    l.bias_updates = falloc(n*2);
    l.outputs = h*w*n*(classes + 4 + 1);
    l.inputs = l.outputs;
    l.truths = 90*(4 + 1);
    //l.delta = falloc(batch*l.outputs);
    //l.output = falloc(batch*l.outputs);
    for (i = 0; i < total*2; ++i)
    {
        l.biases[i] = .5;
    }

    //l.forward = forward_yolo_layer;
    //l.backward = backward_yolo_layer;
    return l;
}

int nms_comparator(const void *pa, const void *pb)
{
    Detection a = *(Detection *)pa;
    Detection b = *(Detection *)pb;
    float diff = 0;
    if (b.sort_class >= 0)
    {
        diff = a.prob[b.sort_class] - b.prob[b.sort_class];
    } 
    else
    {
        diff = a.objectness - b.objectness;
    }
    if (diff < 0)
    {
        return 1;
    }
    else if (diff > 0)
    {
        return -1;
    }
    return 0;
}

void do_nms_sort(Detection *dets, int total, int classes, float thresh)
{
    int i, j, k;
    k = total-1;
    for (i = 0; i <= k; ++i)
    {
        if (dets[i].objectness == 0)
        {
            Detection swap = dets[i];
            dets[i] = dets[k];
            dets[k] = swap;
            --k;
            --i;
        }
    }
    total = k+1;

    for (k = 0; k < classes; ++k)
    {
        for (i = 0; i < total; ++i)
        {
            dets[i].sort_class = k;
        }
        qsort(dets, total, sizeof(Detection), nms_comparator);
        for (i = 0; i < total; ++i)
        {
            if (dets[i].prob[k] == 0) continue;
            Box a = dets[i].bbox;
            for (j = i+1; j < total; ++j)
            {
                Box b = dets[j].bbox;
                if (box_iou(a, b) > thresh)
                {
                    dets[j].prob[k] = 0;
                }
            }
        }
    }
}

void draw_detections(Detection *dets, int num, float thresh, int classes)
{
	int i, j;
	enable_trace && printf("!draw detections for claasses:%d\n", classes);

	num_of_objects = 0;
	for (i = 0; i < num; ++i)
    {
		int klass = -1;
		float max_prob = 0;
		for (j = 0; j < classes; ++j)
        {
			if (dets[i].prob[j] > thresh && dets[i].prob[j] > max_prob)
            {
				klass = j;
				max_prob = dets[i].prob[j];
			}
		}
		if (klass >= 0)
        {
            
			Box b = dets[i].bbox;
			// pbox(&b);
			//printf("%f %f %f %f\n", b.x, b.y, b.w, b.h);

			/* Fill in the objects information */
			if (num_of_objects < OBJECT_MAX_NUMBER)
            {
				object_boxes[num_of_objects].x_min = fmax(0., (b.x - b.w / 2.));
				object_boxes[num_of_objects].x_max = fmin(1., (b.x + b.w / 2.));
				object_boxes[num_of_objects].y_min = fmax(0., (b.y - b.h / 2.));
				object_boxes[num_of_objects].y_max = fmin(1, (b.y + b.h / 2.));
				object_boxes[num_of_objects].confidence = max_prob;
				object_boxes[num_of_objects].label = klass;
				object_boxes[num_of_objects].debug = 0;
				num_of_objects++;
			}
		}
	}
}
    
void free_detections(Detection *dets, int n)
{
    int i;
    for (i = 0; i < n; ++i)
    {
        free(dets[i].prob);
        if (dets[i].mask) free(dets[i].mask);
    }
    free(dets);
}

namespace Yolo_parms
{
    // Not specified in the prototxt, but hard-coded.  See src/detector.c from darknet.
    // (darknet version as of 18Nov04).
	static float nms = inu_yolo_nms_threshold; // 0.45;	// examples/detector.c darknet.

    static int num = 9;	// num.  Terrible name for a constant.
    // This matches the network size in yolov3.cfg.
	static int network_dimension = inu_yolo_network_dimension;
	static int classes = inu_yolo_number_of_classes;
	static float conf_th = inu_yolo_conf_threshold;
    static int version = 3;
}
/* set_yolo_anchores */
void set_yolo_anchores(uintptr_t *bin_anchores, int number_of_anchores)
{
	int i, j;

	for (i = 0; i < number_of_anchores; i++) {
		if (number_of_anchores == 12) {
			if (i >= 6) {
				j = i - 6;
			}
			else {
				j = i + 6;
			}
			tiny_anchors[j] = bin_anchores[i];
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "anchors=%d\n", tiny_anchors[i]);
		}
		else {
			j = i;
			full_anchors[j] = bin_anchores[i];
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "anchors=%d\n", full_anchors[i]);
		}
	}
}
/* set_full_yolo */
void set_full_yolo(void)
{
	full_yolo_mode = true;
//		Yolo_parms::num = 9;
	memcpy(anchors, full_anchors, sizeof(full_anchors));
    Yolo_parms::version = 3;
}

/* set_full_yolo7 */
void set_full_yolo7(void)
{
    full_yolo_mode = true;
 //   Yolo_parms::num = 9;
    Yolo_parms::version = 7;
    memcpy(anchors, yolo7_anchors, sizeof(yolo7_anchors));
}

/* set_tiny_yolo */
void set_tiny_yolo(void)
{
	full_yolo_mode = false;
	Yolo_parms::num = 6;
	memcpy(anchors, tiny_anchors, sizeof(tiny_anchors));
}


/* yolo_set_number_of_classes */
void yolo_set_number_of_classes(int num_classes)
{
	Yolo_parms::classes = num_classes;
}

void yolo_set_nms(float nms)
{
	Yolo_parms::nms = nms;
}
void yolo_set_conf(float conf_th)
{
	Yolo_parms::conf_th = conf_th;
}
void yolo_set_num_of_anchors(int num_of_anchors)
{
	Yolo_parms::num = num_of_anchors;
}

/* yolo_clear_layers */
void yolo_clear_layers(void)
{
	int num_of_layers;

	if (full_yolo_mode)
    {
		num_of_layers = 3;
	}
	else
    {
		num_of_layers = 2;
	}

	for (int i = 0; i < num_of_layers; i++)
    {
		//free(layers[i].output);
		free(layers[i].cost);
		free(layers[i].biases);
		free(layers[i].bias_updates);
		//free(layers[i].delta);
	}

	layers_initialized = false;
}

void test_yolo(Network *net, Image &im)
{
    float thresh = Yolo_parms::conf_th;	// darknet command line -thresh default 0.5.
    float hier_thresh = Yolo_parms::conf_th;	// From darknet.c: test_detector call.

    int nboxes = 0;
	//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "im.w:%d im.h:%d thresh:%f hier_thresh:%f\n", im.w, im.h, thresh, hier_thresh);
    Detection *dets = get_network_boxes(net, im.w, im.h, thresh, hier_thresh, 
        0, 1, &nboxes);

	//for (int bbb = 0; bbb < nboxes; bbb++) {
	//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "box %d: classes:%d bbox: %f %f %f %f\n", bbb, dets[bbb].classes, dets[bbb].bbox.x, dets[bbb].bbox.y, dets[bbb].bbox.w, dets[bbb].bbox.h);
	//}

    float nms = Yolo_parms::nms;
    Layer l = net->layers[net->n-1];
    // Why are there two yolo layers but we do nms etc only on the last one?
    if (nms) 
        if (nms) do_nms_sort(dets, nboxes, l.classes, nms);

	draw_detections(dets, nboxes, thresh, l.classes);

    free_detections(dets, nboxes);
}


void convert_anchors_to_biases(Layer &L,const int *anchors,int nanchors)
{
	for (int i = 0; i < nanchors; i++)
    {
            L.biases[i] = anchors[i];
	}
}

template <typename data_type>
void run_yolo_layer_with_input(Network &net, int lnum, Blob_and_size<data_type>&B, bool convert)
{

    if (convert)
    { // always false
    
#ifdef YOLO_V3_PP_PROCESS_TIME
		uint64_t conv_clock1, conv_clock2;
		conv_clock1 = getRTC();
#endif        

		float scale_factor = 1. / B.scale;
		for (int i = 0; i < B.num_pixels(); i++)
		{
			//net.input[i] = B.blob[i]/B.scale;
			net.input[i] = B.blob[i] * scale_factor;
		}

#ifdef YOLO_V3_PP_PROCESS_TIME
		conv_clock2 = getRTC();
		float ev_clock_rate = EV_INITG_getEvFreqMhz();;
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "conv time (%d) is cycles:%lld %fms\n", (int)ev_clock_rate, (conv_clock2 - conv_clock1), (float)(conv_clock2 - conv_clock1) / (ev_clock_rate * 1000));
#endif
    }
	else {
		net.input = (short*)B.blob;
	}
#ifdef YOLO_V3_PP_PROCESS_TIME
    uint64_t conv_clock1, conv_clock2;
    conv_clock1 = getRTC();
#endif
    if (net.version == 7) {
        forward_yolo7_layer(net.layers[lnum], net, B.scale, B.element_size);
    }
    else {
        forward_yolo_layer(net.layers[lnum], net, B.scale, B.element_size);
    }
#ifdef YOLO_V3_PP_PROCESS_TIME
    conv_clock2 = getRTC();
    float ev_clock_rate = EV_INITG_getEvFreqMhz();
    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "forward_yolo_layer time (%d) is cycles:%lld %fms  inputs:%d\n", (int)ev_clock_rate, (conv_clock2 - conv_clock1), (float)(conv_clock2 - conv_clock1) / (ev_clock_rate * 1000), net.layers[lnum].inputs);
#endif
}

template <typename data_type>
Layer make_yolo_layer(Blob_and_size<data_type> &B, int mask[])
{
    Layer L = make_yolo_layer(1, B.X,B.Y,3/* 3 = number of mask entries*/,	Yolo_parms::num, mask, Yolo_parms::classes);
    //L.output = falloc(L.outputs);
    return L;
}

template <typename data_type>
void run_tiny_yolo(Blob_and_size<data_type> *outputs, IN_SIZE, bool fixed = false)
{
	typedef Blob_and_size<data_type> BS;
	// Create a yolo layer for the two outputs.
	// From examples/detector.c
	Network net;
	// Network dimensions.  This is fixed in the graph.
	Yolo_parms::network_dimension = inu_yolo_network_dimension;
	net.h = net.w = Yolo_parms::network_dimension;
    net.version = Yolo_parms::version;
	// Image dimensions.
	Image im = { data_y,data_x,img_name };

	BS *ptrs[3];


	if (!full_yolo_mode)
    {
		// Create the two yolo layers.

		// We happen to know that the output list from the tool is backwards; last 
		// yolo layer appers first in the list.
		BS &B0 = outputs[1];
		BS &B1 = outputs[0];

		//BS *ptrs[] = { &B0,&B1 };
		ptrs[0] = &B0;
		ptrs[1] = &B1;

		/*
			[yolo]
			mask = 3,4,5
			anchors = 10,14,  23,27,  37,58,  81,82,  135,169,  344,319
			classes=80 num=6

			[yolo]
			mask = 0,1,2
			anchors = 10,14,  23,27,  37,58,  81,82,  135,169,  344,319
			classes=80 num=6
		*/

		static int M0[] = { 3,4,5 }, M1[] = { 0,1,2 };

		net.n = 2;
		if (!layers_initialized)
        {
			layers[0] = make_yolo_layer(1, B0.X, B0.Y, 3, Yolo_parms::num, M0, Yolo_parms::classes);
			layers[0].output = yolo_falloc(layers[0].outputs);

			layers[1] = make_yolo_layer(1, B1.X, B1.Y, 3, Yolo_parms::num, M1, Yolo_parms::classes);
			layers[1].output = yolo_falloc(layers[1].outputs);

			layers_initialized = true;
		}

//			static Layer layers[] = {
//				make_yolo_layer<data_type>(B0,M0),
//				make_yolo_layer<data_type>(B1,M1) };
	}
	else
    {
		// Entries in this array are in a weird order.
		BS &B0 = outputs[0];	// layer82
		BS &B1 = outputs[1];	// layer94
		BS &B2 = outputs[2];	// layer106

		//BS *ptrs[] = { &B0,&B1,&B2 };
		ptrs[0] = &B0;
		ptrs[1] = &B1;
		ptrs[2] = &B2;

		static int M0[] = { 6,7,8 }, M1[] = { 3,4,5 }, M2[] = { 0,1,2 };

		net.n = 3;
		if (!layers_initialized)
        {
			layers[0] = make_yolo_layer(1, B0.X, B0.Y, 3, Yolo_parms::num, M0, Yolo_parms::classes);
			layers[0].output = yolo_falloc(layers[0].outputs);
            layers[0].version = net.version;

			layers[1] = make_yolo_layer(1, B1.X, B1.Y, 3, Yolo_parms::num, M1, Yolo_parms::classes);
			layers[1].output = yolo_falloc(layers[1].outputs);
            layers[1].version = net.version;

			layers[2] = make_yolo_layer(1, B2.X, B2.Y, 3, Yolo_parms::num, M2, Yolo_parms::classes);
			layers[2].output = yolo_falloc(layers[2].outputs);
            layers[2].version = net.version;

			layers_initialized = true;
		}
	}

	net.layers = layers;
	//net.n = sizeof(layers) / sizeof(*layers);

//		BS &B0 = outputs[0];

	for (int i = 0; i < net.n; i++)
    {
		convert_anchors_to_biases(net.layers[i], anchors, Yolo_parms::num*2);
		run_yolo_layer_with_input(net, i, *ptrs[i], false);
	}

	test_yolo(&net, im);
}

void yolo_fixed(void *_outputs, IN_SIZE)
{
    Blob_and_size<short> *outputs = (Blob_and_size<short> *)_outputs;
    run_tiny_yolo<short>(outputs,noutputs,data_ch,data_y,data_x,img_name,true);
}

#if 0
extern void yolo_double(void*_outputs, IN_SIZE) {
    Blob_and_size<double>*outputs = (Blob_and_size<double>*)_outputs;
    run_tiny_yolo<double>(outputs,noutputs,data_ch,data_y,data_x,img_name);
    }


extern void yolo_fixed_8bit(void *_outputs, IN_SIZE) {
    Blob_and_size<signed char> *outputs = (Blob_and_size<signed char> *)_outputs;
    run_tiny_yolo<signed char>(outputs,noutputs,data_ch,data_y,data_x,img_name,true);
    }
#endif

#if YOLO_PP_TEST
void yolo_float(void *_outputs,  IN_SIZE) {
    Blob_and_size<float>*outputs = (Blob_and_size<float>*)_outputs;
    run_tiny_yolo<float>(outputs,noutputs,data_ch,data_y,data_x,img_name);
    }

int main()
{
    Blob_and_size<float> BS[] = {
        { "b1","l1","yolo",0,0,0,255,26,26,(float*)0,1 },
        { "b2","l2","yolo",0,0,0,255,13,13,(float*)0,1 },
        };
    yolo_float(BS,2,3, network_input_dimensions, network_input_dimensions);
}
#endif
