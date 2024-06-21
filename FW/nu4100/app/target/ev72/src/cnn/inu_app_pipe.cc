/*
 * File - inu_cnn_structs.h
 *
 * Copyright (C) 2014 All rights reserved to Inuitive  
 *
*/

#include <inu_app_pipe.h>
#include <evdev.h>
#include <vector>
#include <iterator>
#include "inu2_internal.h"
#include "sched.h"
#include "inu_utils.h"

using namespace std;
#define  FMAP_SPILL_IN_CSM_SIZE (1400 * 1024)
static void *trace_ptr;
static unsigned int trace_size;

/* debug_to_ddr */
#ifdef INU_FPGA_DEBUG
static volatile int *start_debug_ptr=NULL;
static volatile int *end_debug_ptr=NULL;
static volatile int *debug_ptr=NULL;
#endif

static EvCnnCtl *nets_ctl[MAX_CTL_NETS] = { NULL };
void inu_debug_init(void)
{
#ifdef INU_FPGA_DEBUG

	start_debug_ptr = (volatile int *)evMemAlloc(INU_DEBUG_SIZE+100, EV_MR_USE_CSM, -1);
    //start_debug_ptr = (volatile int*)0x2029000; // use LRAM memory
	end_debug_ptr = start_debug_ptr + (INU_DEBUG_SIZE/sizeof(int));
	debug_ptr = start_debug_ptr;

	if (start_debug_ptr) {
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "inu_debug_init: allocated start:%p end:%p cpu:%d\n", start_debug_ptr, end_debug_ptr, evGetCpu());
	}
	else {
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "inu_debug_init: Failed to allocate for debug\n");
	}
#endif
}

void inu_debug_to_ddr(unsigned int value)
{
#ifdef INU_FPGA_DEBUG
        static int first_time = 1;

		if (!start_debug_ptr) {
			return;
		}

        if(first_time) {
		volatile int *ptr = start_debug_ptr;
                while(ptr <= end_debug_ptr) {
                    *ptr++ = 0;
                }
                first_time = 0;
        }

        if(debug_ptr >= end_debug_ptr) {
          debug_ptr  = start_debug_ptr;
		  LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "debug_ptr wrapped to %p\n", start_debug_ptr);
        }
		else {
			*debug_ptr = value;
			debug_ptr++;
			*debug_ptr = 0xEEEEDDDD;
		}
#endif
}

/* printGraphOutput */
ev_cnn_status InuGraphRunner::printGraphOutput(int id, ev_cnn_detection_t *detect,
        int classify, int numdetect) {


    ev_cnn_status status = CNN_STAT_OK;
#if 0
int values_to_display = min(numdetect, 10);

#ifdef INU_FPGA
    volatile int *output_ptr = (volatile int *)0x0205C000;

    inu_debug_to_ddr(0xAAAABBBB);

    *output_ptr++ = 0xAAAAAAAA;
    *output_ptr++ = omap[id].num_maps;
    *output_ptr++ = omap[id].width;
    *output_ptr++ = omap[id].height;

    *output_ptr++ = 0xCAFECAFE;
    *output_ptr++ = omap[id].scale;
    *output_ptr++ = numdetect;
    for(int i=0; i<numdetect; i++) {
	*output_ptr++ = detect[i].value;
    }
    for(int i=0; i<numdetect; i++) {
	*output_ptr++ = detect[i].z;
    }
    *output_ptr++ = 0xEEEEEEEE;

#else
    printf("Top %d results:\n", values_to_display);
    printf(" ix   [map]  [y]  [x]   val\n");
    printf("---------------------   -----\n");

    if (classify >= 0) {

        bool top1 = false;
        bool top5 = false;

        for (uint32_t i = 0; i < values_to_display; i++) {
            bool found = detect[i].z == classify;

            printf("%3d - [%5d][%3d][%3d] %5d %s\n", i, detect[i].z,
                    detect[i].y, detect[i].x, detect[i].value,
                    found ? "<- expected classification" : "");

            if (found) {
                if (i == 0) {
                    top1 = true;
                }
                if (i < 5) {
                    top5 = true;
                }
            }
        }

        if (top5 == false) {
            status = CNN_STAT_FAIL_TOP5;
        }

#define OK(B) B ? "OK" : "FAIL"
        printf("top5 expected %d %s\n", classify, OK(top5));

    } else {
        for (int i = 0; i < values_to_display; i++) {
            printf("%3d - [%5d][%3d][%3d] %5d\n", i, detect[i].z, detect[i].y,
                    detect[i].x, detect[i].value);
        }
    }
#endif
#endif
    return status;

}

/* getBlobOutputAddress */
uint8_t * InuGraphRunner::getBlobOutputAddress(void)
{
   uint8_t *output_area = blobOutputArea[out_index];

   /* Switch output buffer */
   if(out_index) {
      out_index = 0;
   }
   else {
     out_index = 1;
   }

   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"getBlobOutputAddress: output_area: %p\n", output_area);

   return(output_area);
}

/* getInputDimentions */
void InuGraphRunner::getInputDimentions(int image_num, int &width, int &height,
        int &channels) const {
    width = imap[image_num].width;
    height = imap[image_num].height;
    channels = imap[image_num].num_maps;
}

/* GetImageSpace */
void * InuGraphRunner::GetImageSpace(int size)
{
	return(evMemAlloc(size, getPrefMemRegion(MB_INPUT_IMAGE), evGetCpu()));
}

/* createImageStream */
EvCnnLoadImage_stream* InuGraphRunner::createImageStream(int width,
                                                         int height,
                                                         int channels) {
    return getCnnLoadImage(width, height, channels, *transformer,
                           getPrefMemRegion(MB_INPUT_IMAGE));

}
/* getProcessingStatus */
ev_cnn_status InuGraphRunner::getProcessingStatus(ev_cnn_status *pushStatus, ev_cnn_status *pullStatus)
{
   ev_cnn_status status;
   status = evCnnStreamProcess(ctl, pushStatus, pullStatus);
   return status;
}

void InuGraphRunner::releaseGraph(void)
{
	if (freeOnSwitch) {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Free %d\n", active_net_id);
		evCnnFree(ctl);
		nets_ctl[active_net_id] = NULL;
	}
	else {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Done %d\n", active_net_id);
		evCnnDone(ctl);
	}
}

/* Inuitive - Load a new CNN */
ev_cnn_status InuGraphRunner::loadCnnGraph(int net_id, uintptr_t blob_addr, int blob_size, bool free_flag) 
{
	ev_cnn_status stat;
	bool first_time;

	cnnBinAddr = blob_addr;

    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"InuGraphRunner::loadCnnGrap: loading net:%d blob_addr:%p  blob_size:%d\n", net_id, (void *)blob_addr, blob_size);

#ifdef EVSS_CFG_HAS_XDMA
    evXdmaInit();
#endif

        inu_debug_to_ddr(0x100);
		if (!nets_ctl[net_id]) {
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "***** evCnnInit net %d *****\n", net_id);
			ctl = evCnnInit((void *)blob_addr, blob_size);
			nets_ctl[net_id] = ctl;
			first_time = true;
		}
		else {
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "***** using prev evCnnInit net %d *****\n", net_id);
			ctl = nets_ctl[net_id];
			first_time = false;
		}
        inu_debug_to_ddr(0x101);

	if (ctl == NULL) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "***** evCnnInit failed*****\n");
		return CNN_STAT_FAIL;
	}

#ifdef PROFILING_OUTPUT_TEST
    stat = evCnnLayersProfilingInit(ctl, false);
    if (CNN_STAT_OK != stat)
        LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Unable to initialize profilig\n");
    else
        LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Profiling initialized\n");
#endif

        inu_debug_to_ddr(0x102);

    	if(ctl->status != CNN_STAT_OK) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Could not load CNN binary! status returned from evCnnInit: %d\n", ctl->status);
    	    return ctl->status;
    	}

        inu_debug_to_ddr(0x103);
        // Run dispatch layers on core #1	
        int my_core = CNN_DISPATCH_CPU;	
		int my_dispatch_priority = 8;//9;

        stat = evCnnSetCore(ctl, CNN_CORE_USE_DISPATCH, &my_core, 1, my_dispatch_priority);  
        //stat = evCnnSetCore(ctl, CNN_CORE_USE_DISPATCH, &my_core, 1);
 		
        if (stat != CNN_STAT_OK) {
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "evCnnSetCore failed\n");
            return stat;
        }

        //int num_slices = evCnnGetNumSlices(ctl);
        //LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "num_slices:%d!\n", num_slices);

		if (first_time) {
			int num_slices = evCnnGetNumSlices(ctl);
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "num_slices:%d!\n", num_slices);
			numSlice = num_slices;

            if (numSlice == 1) {
                binSet[0] = sliceSet[0] = 0;
                sliceSet[0] = 1; // force running on slice 1, due to HW bug in the DMA ports.
            }
            else {
                binSet[0] = 0; sliceSet[0] = 0;
                binSet[1] = 0; sliceSet[1] = 1;
            }

    	    stat = evCnnSetSlice(ctl, sliceSet, binSet, numSlice);
			if (stat != CNN_STAT_OK) {
				LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Could not load evCnnSetSlice for net_id:%d  numSlice:%d stat:%d max_slice:%d!\n", net_id, numSlice, stat, EV_CTL_MAX_SLICE);
    	    return ctl->status;
			}
    	} 

        inu_debug_to_ddr(0x104);
		uint32_t metadataSize = evCnnGetMetaBufferSize(ctl);
		//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL,"metadataSize: %d\n", metadataSize);
		if (metadataSize > 0)
			{
				// Do not use CSM for metadata, because it could cause problems with
				// the ELF loader in the host interface.
				//metadataMemory = evMemAlloc(metadataSize,EV_MR_TRY_UNCACHED_DRAM, evGetCpu());
				metadataMemory = evMemAlloc(metadataSize,EV_MR_USE_CNDC, evGetCpu());
				
				if (metadataMemory == NULL)
					{
					LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"Failed to allocate metadataMemory\n");
					return CNN_STAT_FAIL;
					}

				stat = evCnnSetMeta(ctl, metadataMemory);
				if (stat != CNN_STAT_OK) 
					{
					LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"Failed to evCnnSetMeta\n");
					return stat;
					}
			}

        /* Init the inputs */
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"loadCnnGraph: Number of inputs: %d *****\n", evCnnInputNumGet(ctl));
        for(int i=0; i<evCnnInputNumGet(ctl); i++) {
            evCnnInputMapInit(ctl, i, &imap[i]);
            LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"input name: %s\n", imap[i].name);
        }

        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Start: Booting CNN  net_id:%d\n", net_id);

        inu_debug_to_ddr(0x105);

        if (evCnnBoot(ctl) != CNN_STAT_OK) {
           inu_debug_to_ddr(0x106);
		   LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Problem booting the CNN, status is %d  net_id;%d!\n", ctl->status, net_id);
           return ctl->status;
        }

        inu_debug_to_ddr(0x107);

        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Done: Booting CNN. net_id:%d\n", net_id);

        externalSize = evCnnGetExternalMemorySize(ctl);
        cnnInputBufferSize = evCnnInputBufferSize(ctl);
        cnnOutputBufferSize = evCnnOutputBufferSize(ctl);

    /* Allocate memory for all the outputs */
    EvCnnMapData om;
 
    total_output_size = 0;
    for(int i=0; i<evCnnOutputNumGet(ctl); i++) {
        evCnnOutputMapInit(ctl, i, &om);
        tails_offsets[i] = total_output_size;
        size_t outputDataSize = evMapSize(&om);
        total_output_size += outputDataSize;
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Tail %d  size:%d offset:%d\n", i, outputDataSize, tails_offsets[i]);
    }

#if 0
    blobOutputArea[0] = (uint8_t *) evMemAlloc(total_output_size,
            getPrefMemRegion(MB_OUTPUT_DATA), evGetCpu());
    blobOutputArea[1] = (uint8_t *) evMemAlloc(total_output_size,
            getPrefMemRegion(MB_OUTPUT_DATA), evGetCpu());
    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Allocated output buffer size: %d\n", total_output_size);

    EV_ASSERT(blobOutputArea[0] != NULL);
    EV_ASSERT(blobOutputArea[1] != NULL);
#endif

    /* Prepare the omaps */
    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"loadCnnGraph: Number of output tails: %d *****\n", evCnnOutputNumGet(ctl));
    for(int i=0; i<evCnnOutputNumGet(ctl); i++) {
        evCnnOutputMapInit(ctl, i, &om);
        size_t outputDataSize = evMapSize(&om);
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Output data size for tail %s: %d: %d   offset_int_blob:%d\n", om.name, i, outputDataSize, tails_offsets[i]);
        evCnnOutputMapInit(ctl, i, &omap[i]);
    }

    inu_debug_to_ddr(0x108);


    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"external size:%d\n", externalSize);

    if (externalSize > 0) {
        externalMemory = evMemAlloc(externalSize,
                getPrefMemRegion(MB_CNN_EXTERNAL_MEMORY), evGetCpu());

        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"externalMemory:%p\n", externalMemory);

        if (externalMemory == NULL) {
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"Could not allocate CNN external memory! Cannot allocate %d bytes.  NET_ID:%d\n", externalSize, net_id);
            return CNN_STAT_FAIL;
        }
        evCnnSetExternalMemory(ctl, externalMemory);
    }

    inu_debug_to_ddr(0x109);

    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Allocating memory buffers\n");

    cnnInputBuffer = (uint8_t *) evMemAlloc(cnnInputBufferSize,
            getPrefMemRegion(MB_CNN_INPUT_BUFFER), evGetCpu());
    cnnOutputBuffer = (uint8_t *) evMemAlloc(cnnOutputBufferSize,
            getPrefMemRegion(MB_CNN_OUTPUT_BUFFER), evGetCpu());

    inu_debug_to_ddr(0x110);

    //CSM fmap spill
#if 0
    int csmSz = FMAP_SPILL_IN_CSM_SIZE;
    fmapSpill = evMemAlloc(csmSz, EV_MR_USE_CSM, evGetCpu());
    if (fmapSpill) 
    {
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Setting FMap spill buf in csm address %p size=%d Bytes\n", fmapSpill, csmSz);        
        evCnnSetFmapTempMemory(ctl, fmapSpill, csmSz);
    } 
    else 
    {
        LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"Error: Could not allocate %d KB CSM\n", csmSz / 1024);
    }    
#endif    
    
    EV_ASSERT(cnnInputBuffer != NULL);
    EV_ASSERT(cnnOutputBuffer != NULL);
    EV_ASSERT(ctl->output_size > 0);

	active_net_id = net_id;
	freeOnSwitch = free_flag;

    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Done: loadCnnGraph. net_id:%d\n", net_id);

    ctl->sleep_when_inactive = 1; // This enables the sleep of the HAL thread, when it waits for a CNN stage to finish

    inu_debug_to_ddr(0x111);
    
#ifdef PROFILING_OUTPUT_TEST
    stat = evCnnLayersProfilingInit(ctl, false);
    if (CNN_STAT_OK != stat)
        LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Unable to initialize profilig\n");
    else 
        LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Profiling initialized\n");
#endif

    return(stat);
}

/* pushOneSet */
ev_cnn_status InuGraphRunner::pushOneSet(InputData *input_images)
{
   ev_cnn_status status;

    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"pushOneSet start\n");

    for(int image_id=0; image_id < evCnnInputNumGet(ctl); image_id++) {
        inu_debug_to_ddr(0x200);

        if (input_plane_map[image_id] == NULL) {
            input_plane_map[image_id] = new PlaneMapPointer[imap[image_id].num_maps + 1];
        }

		for (int i = 0; i < imap[image_id].num_maps; i++) {
			input_plane_map[image_id][i] = input_images[image_id].input_plane[i];
		}

        imap[image_id].element_type = input_images[image_id].element_type; //CNN_ELEMENT_TYPE_UINT8;
        imap[image_id].layout_type = input_images[image_id].layout_type; //CNN_LAYOUT_3DFMAPS;
        imap[image_id].fmap.pointers = input_plane_map[image_id];
        imap[image_id].fmap.strides = NULL;

        inu_debug_to_ddr(imap[image_id].element_type);

        status = evCnnSetStreamInputs(ctl, image_id, &imap[image_id]);

        inu_debug_to_ddr(0x202);

        if (status != CNN_STAT_OK) {
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"pushOneSet: Problem in evCnnSetStreamInputs, status is %d!\n", status);
            return status;
        }
	else {
            LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"pushOneSet: Images pushed to pip!!!\n");
	}

        inu_debug_to_ddr(0x203);
    }

    return status;
}

/* pushOneSet */
void InuGraphRunner::pullOneSet(EV_CNN_PROCESS_IMAGE_ACK *ack_msg)
{
   ev_cnn_status pushStatus, pullStatus;
   /* Get all outputs */
   ack_msg->num_of_tails = evCnnOutputNumGet(ctl);

   /* Wait for done on all outputs */
   int num_of_done = 0;
   while(num_of_done < ack_msg->num_of_tails) {
     num_of_done = 0;
     for(int i=0; i<ack_msg->num_of_tails; i++) {
       if(evCnnOutputChkDone(ctl, i)) {
          num_of_done++;
       }
     }
	 evCnnStreamProcess(ctl, &pushStatus, &pullStatus);
     LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"num_of_done: %d\n", num_of_done);
   }

   /* Clear the done indications */
   for(int i=0; i<ack_msg->num_of_tails; i++) {
       evCnnOutputClrDone(ctl, i) ;
   }

   /* Take the outputs */
   for(int i=0; i<ack_msg->num_of_tails; i++) {
      int element_size = evElementByteSize(omap[i].element_type);
      LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"OUTPUT(%d): num_maps:%d width:%d height:%d c_ptr:%p layout_type:%d scale:%f element_type:%d element_size:%d  layout_type:%d\n", 
             i, omap[i].num_maps, omap[i].width, omap[i].height, omap[i].c_ptr, omap[i].layout_type, omap[i].scale, omap[i].element_type, element_size, omap[i].layout_type);

      if(omap[i].layout_type != CNN_LAYOUT_BBOX) {
	      ack_msg->tails[i].elementSize = element_size;
	      ack_msg->tails[i].elementCount = omap[i].num_maps * omap[i].width * omap[i].height;
	      ack_msg->tails[i].channels = omap[i].num_maps;
	      ack_msg->tails[i].width = omap[i].width;
	      ack_msg->tails[i].height =omap[i].height;
	      ack_msg->tails[i].scale = omap[i].scale;
	      ack_msg->tails[i].ptr = omap[i].c_ptr;
     }
     else {
	      ack_msg->tails[i].elementSize = sizeof(EvCnnBoundingBox);
	      ack_msg->tails[i].elementCount = omap[i].bbox.alloc_count;
	      ack_msg->tails[i].channels = 0;
	      ack_msg->tails[i].width = 0;
	      ack_msg->tails[i].height =0;
	      ack_msg->tails[i].scale = omap[i].scale;
	      ack_msg->tails[i].ptr = omap[i].c_ptr;
     }

	  //LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "c_ptr %x\n", omap[i].c_ptr);

      inu_debug_to_ddr(0x2175);
//#ifdef INU_FPGA
#if 0
      { // Prints the results
	uint32_t reportDetectionNumber;
        reportDetectionNumber = 20;
        //reportDetectionNumber = omap[i].num_maps;

        printf("reportDetectionNumber: %d  tail addr:%p\n", reportDetectionNumber, ack_msg->tails[i].ptr);
        cnn_VLA(ev_cnn_detection_t,detect,reportDetectionNumber + 1);
        ev_cnn_status status_out_loc = CNN_STAT_OK;
        evCnnOutputMapDetections(ctl, i, detect, &reportDetectionNumber);
        //CHK_STAT(ctl, "Problems with evCnnOutputDetections!");
        status_out_loc = printGraphOutput(i, detect, -1, reportDetectionNumber);
      }
#endif
   }
   inu_debug_to_ddr(0x2201);
#ifdef INU_EVTRACER_ENABLED
   unsigned char *end_ptr = (unsigned char *)trace_ptr + trace_size;
   evDataCacheFlushLines(trace_ptr, end_ptr);
   //unsigned int *ttt = (unsigned int *)trace_ptr;
   //LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "trace after flush: %X %X %X %X %X\n", ttt[0], ttt[1], ttt[2], ttt[3], ttt[4]);
#endif
}

/* prepareOutputSet */
ev_cnn_status InuGraphRunner::prepareOutputSet(uint8_t *output_area)
{
    if(!output_area) {
       LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "prepareOutputSet: ERROR: output_Area is NULL\n");
       status = CNN_STAT_FAIL;
    }
    else {
       /* Set stream Outputs */
       for(int i=0; i<evCnnOutputNumGet(ctl); i++) {
          omap[i].c_ptr = output_area + tails_offsets[i];
          LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Set stream output to: %p (offset:%d)\n",  omap[i].c_ptr, tails_offsets[i]);
          status = evCnnSetStreamOutputs(ctl, i, &omap[i]);
          if (status != CNN_STAT_OK) {
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"prepareOutputSet: Problem updating output %d (ptr=%p) status, status is %d!\n", i, omap[i].c_ptr, status);
	    return (status);
          }
       }
    }
    
    return(status);
}

/* prepareForImages */
ev_cnn_status InuGraphRunner::prepareForImages(int *width, int *height, int *channels)
{
    inu_debug_to_ddr(0x300);

    for(int i=0; i<evCnnInputNumGet(ctl); i++) {
        getInputDimentions(i, width[i], height[i], channels[i]);
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"prepareForImages: image:%d width:%d height:%d channels:%d\n", i, width[i], height[i], channels[i]);
    }
    initPreprocessingTransformer();
    //inu_in_data = createImageStream(width, height, channels);


    inu_debug_to_ddr(0x302);

    return(CNN_STAT_OK);
}

/* ev_cnn_get_total_output_size */
int InuGraphRunner::ev_cnn_get_total_output_size(void)
{
    return(total_output_size);
}

/* ev_cnn_num_of_tails */
int InuGraphRunner::ev_cnn_num_of_tails(void)
{
	return(evCnnOutputNumGet(ctl));
}

/* ev_cnn_num_of_inputs */
int InuGraphRunner::ev_cnn_num_of_inputs(void)
{
    return(evCnnInputNumGet(ctl));
}

void InuGraphRunner::init_trace(void)
{
#ifdef INU_EVTRACER_ENABLED
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "------- init_trace --------\n");
	bool rc;
	rc = CNN_TRACE_ACTIVATE();
	if (!rc) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "##### Failed to do trace activate #####\n");
	}
	else {
		evTraceGetBuffer(&trace_ptr, &trace_size);
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "----- trace activate OK size:%d ptr:%p-----n", trace_size, trace_ptr);
	}
   ctl->sleep_when_inactive = 0;
#endif
}

int InuGraphRunner::save_trace(unsigned char **trace_buffer)
{
    int size = trace_size;
#ifdef INU_EVTRACER_ENABLED
   LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL,"------- Calling CNN_TRACE_DUMP --------\n");
   CNN_TRACE_DUMP();
#endif
   return(size);
}

ev_cnn_status InuGraphRunner::get_profiling(unsigned char *profiling_data_ptr, unsigned int *profiling_data_size, unsigned int profiling_buffer_size)
{
    ev_cnn_status ret;

    static unsigned int profiling_frame_num = 1;
    if (profiling_frame_num == 1) {
        memset(profiling_data_ptr, 0, PROFILING_BUFFER_SIZE);
    }
    LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Before evCnnLayersProfilingBuffer ctl:%p\n", ctl);
    ret = evCnnLayersProfilingBuffer(ctl, profiling_frame_num, profiling_data_ptr, profiling_data_size, profiling_buffer_size);
    LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "After evCnnLayersProfilingBuffer\n");
    profiling_frame_num++;
    return(ret);
}



