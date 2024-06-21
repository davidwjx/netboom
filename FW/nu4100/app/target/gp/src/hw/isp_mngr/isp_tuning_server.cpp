
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "inu_common.h"
#include "mem_pool.h"
#include "isp_tuning_server.hpp"
#include "sequence_mngr.h"
#include "sensors_mngr.h"
#include "isp_mngr.hpp"
#include "tuning-server/tuning-server.h"

#define ENABLE_TUNING_SERVER

#define TUNINIG_GET_ISP_MP(_sensor_)    ((_sensor_ == 2)?(23U):(26U)) //HP case
ERRG_codeE TUNING_SERVERG_start(void *params);

struct TuningCallbackContext
{
    UINT32 sensor_num;
    void *snode;
};

TuningCallbackContext tuning_cb_ctx;

//Enable ISP Front End
static void start_tuning_cb(void *userdata) {
    TuningCallbackContext *cb_ctx = (TuningCallbackContext *)userdata;
    if(ERRG_FAILED(SEQ_MNGRG_enableIspFe(cb_ctx->sensor_num, cb_ctx->snode)))
    {
        printf("start_tuning_cb: SEQ_MNGRG_enableIspFe failed\n");
        return;
    }
    printf("start_tuning_cb: SEQ_MNGRG_enableIspFe succsessfully\n");
};

static void TUNING_SERVERG__frameDoneCb(void *pPhyBuff, void *arg)
{
    printf("\t %s:: address %p node %p\n", __func__ , pPhyBuff, arg);
}


ERRG_codeE TUNING_SERVERG_startTuningThread(UINT32 sensor_num, void * snode)
{
    ERRG_codeE ret = ISP_MNGR__RET_SUCCESS;
    OS_LYRG_threadHandle thrTuningHandle = NULL;
    OS_LYRG_threadParams thrParams;
    TuningCallbackContext thrTuningParam = {.sensor_num = sensor_num, .snode = snode};


    //Launch interrupt handling thread
    thrParams.func    = (OS_LYRG_threadFunction)TUNING_SERVERG_start;
    thrParams.id      = OS_LYRG_ISP_MONITOR_THREAD_ID_E;
    thrParams.event   = NULL;
    thrParams.param   = &thrTuningParam;
    thrTuningHandle   =  OS_LYRG_createThread(&thrParams);

    if(!thrTuningHandle)
    {
        ret = OS_LYR__ERR_UNEXPECTED;
    }
    //set thread name for ps and trace
    pthread_setname_np((pthread_t)thrTuningHandle, "TuningSvrTh");
    return ret;

}

ERRG_codeE TUNING_SERVERG_start(void *params)
{
    ERRG_codeE ret = ISP_MNGR__RET_SUCCESS;
    TuningCallbackContext *paramsP = (TuningCallbackContext *)params;
	UINT32 sensor_num = paramsP->sensor_num;
	void * snode = paramsP->snode;
    UINT32 ispInst;
    char senModel[10]="";
    SENSORS_MNGRG_sensorInfoT *senInfo = NULL;
    ISP_MNGRG_streamReadCfgT cfg;
    XMLDB_dbH db = NULL;
    isp_tuning_args isp_args;
    isp_args.isp_id = -1;
    //int ispid = -1;

    //Configure ISP Front End
    ret = SEQ_MNGRG_configIspFe(sensor_num, snode, &isp_args.isp_id);

    //Get ISP configuration
    ret = SEQ_MNGRG_getIspInstance(sensor_num, &ispInst, &db);
    if(ERRG_FAILED(ret))
    {
        return ISP_MNGR__ERR_UNEXPECTED;
    }
    ret = SEQ_MNGRG_getIspConfig(db, ispInst, &cfg );
    if(ERRG_FAILED(ret))
    {
        return ISP_MNGR__ERR_UNEXPECTED;
    }

    senInfo = SENSORS_MNGRG_getSensorInfo((INU_DEFSG_senSelectE)sensor_num);
    sprintf(senModel,"%d",senInfo->sensorCfg.sensorModel);
    sprintf(isp_args.sensor_driver_name, "%d", (unsigned int)senInfo->sensorCfg.sensorModel);
    sprintf(isp_args.sensor_calibration_file, "/media/inuitive/%d.xml", (unsigned int)senInfo->sensorCfg.sensorModel);
    
    //printf("id =%d, driver name=%s, calib file =%s \n", isp_args.isp_id,isp_args.sensor_driver_name,isp_args.sensor_calibration_file );
    // Enable ISP
#if defined(ENABLE_TUNING_SERVER)
    ISP_MNGRG_DdrConfig();

    tuning_cb_ctx.sensor_num = sensor_num;
    tuning_cb_ctx.snode = snode;

    if (tuning_server_main(&isp_args, start_tuning_cb, &tuning_cb_ctx) < 0)
    {
        printf("failed to start tuning server %d\n", isp_args.isp_id);
        return ISP_MNGR__ERR_UNEXPECTED;
    }
#else
    ret = ISP_MNGRG_setConfig(ispInst, senModel, cfg);
    if(ERRG_FAILED(ret))
    {
        return ISP_MNGR__ERR_UNEXPECTED;
    }
    ret = ISP_MNGRG_ThreadInit((void*)snode, TUNINIG_GET_ISP_MP(sensor_num) , (void*)TUNING_SERVERG__frameDoneCb);
    if(ERRG_FAILED(ret))
    {
        return ISP_MNGR__ERR_UNEXPECTED;
    }
    ret = ISP_MNGRG_setEnable(ispInst, &cfg);
    if(ERRG_FAILED(ret))
    {
        return ISP_MNGR__ERR_UNEXPECTED;
    }
    //Enable ISP Front End
    ret = SEQ_MNGRG_enableIspFe(sensor_num, snode);
    if(ERRG_FAILED(ret))
    {
        printf("SEQ_MNGRG_enableIspFe failed\n");
        return ISP_MNGR__ERR_UNEXPECTED;
    }
#endif

    //while(TRUE)
    {
        OS_LYRG_usleep(1000);
    }

}



