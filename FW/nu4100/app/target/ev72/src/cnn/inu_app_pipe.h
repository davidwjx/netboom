/*
 * File - inu_cnn_structs.h
 *
 * Copyright (C) 2014 All rights reserved to Inuitive
 *
*/

#ifndef _INU_APPLICATION_H_
#define _INU_APPLICATION_H_

#define VER_4100 1

//#define PROFILING_OUTPUT_TEST 1
#define PROFILING_BUFFER_SIZE 300000

#define INU_FPGA 1
#define	USE_CNN_THREAD_TRIGGERING 0   //This flag enables CNN thread waiting for triggering when a job arrives (and not always running in a loop)
#define MAX_CTL_NETS 10
//#define INU_HAPS 1



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>

#include <gtest/gtest.h>
#ifndef USE_OPENCV
#define USE_OPENCV
#endif

#include <cnn_dev.h>

/*
#ifdef CNN_USE_OPENCV
#if __has_include (<cnn_image_util.h>)
#include <cnn_image_util.h>
#else
#include <cnn_image_opencv.h>
#endif
#else
#include <cnn_image_bmetal.h>
#endif
*/
#define EVRT_PROJECT
#include <cnn_image_bmetal.h>

#include <internal/evth_internal.h>
//#include <internal/cnn_util.h>

#ifdef INU_HAPS
#define LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, __VA_ARGS__)   printf(__VA_ARGS__);
#define LOGG_PRINT(LOG_WARNING_E, (ERRG_codeE)NULL, __VA_ARGS__) printf(__VA_ARGS__);
#define LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, __VA_ARGS__)    printf(__VA_ARGS__);
#else
#include "log.h"
#endif

// we use debug_cnn for reading data from ASIP
#ifdef __arc__
#include <internal/cnn_hw_util.h>
//#include <ev/arc_utils.h>
//#include <ev/code.h>
#else
#define debug_cnn(x)
#endif

//#include "cnn_cpp_helper.hpp"
#include "inu_cnn_pipe_api.h"

#include "evthreads.h"




void inu_debug_to_ddr(unsigned int value);

/**
 * \brief Pointers to image data for stream solutions.
 */
struct InputData {
    _ev_cnn_element_type_e element_type;
    _ev_cnn_layout_e layout_type;
	void *input_plane[EV_CNN_MAX_TENSOR_PLANES];
};

typedef void * PlaneMapPointer;

/**
 * \brief Main class for running an example
 */
class InuGraphRunner {

protected:

    EvCnnCtl *ctl;
    uintptr_t cnnBinAddr;      // -- address of CNN binary in memory.
    int sliceSet[EV_CTL_MAX_SLICE];
    int binSet[EV_CTL_MAX_SLICE];
    int numSlice;
    void *externalMemory;
    uint8_t *cnnInputBuffer;
    uint8_t *cnnOutputBuffer;
    uint8_t *blobOutputArea[2];
    int tails_offsets[EV_CNN_MAX_IO_MAPS];
	int active_net_id;
	bool freeOnSwitch;
	void *metadataMemory;

    ev_cnn_status pushStatus, pullStatus, status, status_out;

    PlaneMapPointer *input_plane_map[EV_CNN_MAX_IO_MAPS];

    /* Inuitive */
    uint32_t externalSize = NULL;
    size_t cnnInputBufferSize = NULL;
    size_t cnnOutputBufferSize = NULL;
	void *fmapSpill = NULL;
    int out_index = 0;
    int total_output_size = 0;

    /**
     * Default I/O maps, in case of one in, one out.
     */
    EvCnnMapData imap[EV_CNN_MAX_IO_MAPS], omap[EV_CNN_MAX_IO_MAPS];

    // An object that work with input's preprocessing.
    // Could be defined for every CNN
    EvCnnPreprocessImage *transformer = NULL;

    virtual void initPreprocessingTransformer() {
        transformer = new DefaultPreprocessImage();
        }

    /**
     * Returns image stream for loading input images
     */
    virtual EvCnnLoadImage_stream* createImageStream(int width,
                                                     int height,

                                                   int channels);

	void releaseGraph(void);

public:

    /* Inuitive */
    EvCnnLoadImage_stream *inu_in_data= NULL;

    enum MemBufferKind {
        MB_INPUT_IMAGE,
        MB_CNN_INPUT_BUFFER,
        MB_CNN_EXTERNAL_MEMORY,
        MB_CNN_OUTPUT_BUFFER,
        MB_OUTPUT_DATA
    };

    /**
     * Returns dimentions required for the input image
     */
    virtual void getInputDimentions(int image_num, int& width, int& height,
            int&channels) const;

    ev_cnn_status printGraphOutput(int id, ev_cnn_detection_t *detect, int classify, int numdetect);

    /**
     * Return preferable memory region to allocate buffer of type bufferKind
     */
    virtual EvMemoryRegion getPrefMemRegion(MemBufferKind bufferKind) const {
#ifdef __arc__
        switch (bufferKind) {
        case MB_INPUT_IMAGE:
        case MB_CNN_INPUT_BUFFER:
        case MB_CNN_EXTERNAL_MEMORY:
#ifdef INU_FPGA
        return EV_MR_USE_CNDC;
#else
        return EV_MR_USE_UNCACHED_DRAM;
#endif

        case MB_CNN_OUTPUT_BUFFER:
        case MB_OUTPUT_DATA:
            return EV_MR_GLOBAL_HEAP;

        default:
            return EV_MR_GLOBAL_HEAP;
        };
#else
        return EV_MR_GLOBAL_HEAP;
#endif
    }


    /* Inuitive API */
    ev_cnn_status loadCnnGraph(int net_id, uintptr_t blob_addr, int blob_size, bool free_flag);
    uint8_t * getBlobOutputAddress(void);
    ev_cnn_status prepareForImages(int *width, int *height, int *channels);
    ev_cnn_status getProcessingStatus(ev_cnn_status *pushStatus, ev_cnn_status *pullStatus);
    ev_cnn_status pushOneSet(InputData *input_images);
    ev_cnn_status prepareOutputSet(uint8_t *output_area);
    int ev_cnn_get_total_output_size(void);
	int ev_cnn_num_of_tails(void);
    int ev_cnn_num_of_inputs(void);
    void init_trace(void);
    int save_trace(unsigned char** trace_buffer);
    void pullOneSet(EV_CNN_PROCESS_IMAGE_ACK *ack_msg);
	void * GetImageSpace(int size);
    ev_cnn_status get_profiling(unsigned char* profiling_data_ptr, unsigned int* profiling_data_size, unsigned int profiling_buffer_size);

    InuGraphRunner() {
        ctl = NULL;
        cnnBinAddr = 0;
        blobOutputArea[0] = NULL;
        blobOutputArea[1] = NULL;

        for(int i = 0; i < EV_CNN_MAX_IO_MAPS; i++) {
            tails_offsets[i] = 0;
            input_plane_map[i] = NULL;
        }

        externalMemory = NULL;
        cnnInputBuffer = NULL;
        cnnOutputBuffer = NULL;
		fmapSpill = NULL;
		freeOnSwitch = false;
		metadataMemory = NULL;

        pushStatus = pullStatus = status = status_out = CNN_STAT_OK;
    }

    virtual ~InuGraphRunner() {

        evMemFree(cnnInputBuffer);
        evMemFree(cnnOutputBuffer);
        evMemFree(externalMemory);
		evMemFree(metadataMemory);

		if (fmapSpill) {
			evMemFree(fmapSpill);
		}

        for(int i=0; i<2; i++) {
           if(blobOutputArea[i]) {
             evMemFree(blobOutputArea[i]);
           }
        }

        for(int i=0; i<EV_CNN_MAX_IO_MAPS; i++) {
            if (input_plane_map[i] != NULL) {
                delete[] input_plane_map[i];
            }
        }

		if (ctl != NULL) {
			releaseGraph();
		}

        if (transformer != NULL) {
            delete transformer;
        }

    }

};

//Termpr
#define CNN_ASSERT(COND) {assert(COND);}

typedef EvCnnCtl * ev_cnn_ctl;
typedef uint8_t * ev_cnn_ptru8;



#endif /* _INU_APPLICATION_H_ */

