
#include <stdlib.h>
#include "assert.h"

#include "inu_common.h"
#include "test_utils.h"
#include "cdc_mngr.h"
#include "ve_mngr.h"
#include "inu_fw_update_api.h"
#include "log.h"

#define PATH_SEPARATOR  "\\"

static inline const char* get_installation_path(void) { return "C:\\program files\\Inuitive\\InuVE"; }

int main(int argc, char* argv[])
{
    ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
    CDC_MNGRG_handle handle;
    VE_MNGRG_bokehParamsT bokehParams;
    UINT32 fwVersion[1];
    UINT32 securityVersion = 1;
    UINT32 bootFlags[1];
    UINT32 bootId = 10085;
    UINT32 comPort = 5;
    UINT32 baudrate = 921600;
    INU_FW_UPDATEG_copyFwImageParamsT copyFwImageParams;
    char key[1];

    if (argc > 1)
    {
        bootId = strtoul(argv[1], NULL, 0);
    }
    if (argc > 2)
    {
        comPort = strtoul(argv[2], NULL, 0);
    }
    if (argc > 3)
    {
        baudrate = strtoul(argv[3], NULL, 0);
    }

    if (comPort == -1)
    {
        int retVal = 0;// autoDetectComPort(&comPort);
        if (!retVal)
        {
            while (comPort == -1) {};
        }
        else
        {
            return -1;
        }
    }

    ret = CDC_MNGRG_init();
    if (ERRG_FAILED(ret))
        assert(ret);

    printf("open com port %lu baudrate %lu\n", comPort, baudrate);
    ret = CDC_MNGRG_open(&handle, comPort, baudrate);
    if (ERRG_FAILED(ret))
        return -1;

    printf("\n\nPING\n\n\n");
    ret = CDC_MNGRG_ping(handle);
    if (ERRG_FAILED(ret))
        assert(ret);

    printf("\n\nFW UPDATE get FW version\n\n\n");
    memset(&fwVersion, 0, sizeof(fwVersion));
    ret = inu_fw_update__getFwVersion(fwVersion);
    if (ERRG_FAILED(ret))
        assert(ret);
    printf("\n\nFW version %d\n\n\n", fwVersion[0]);

    printf("\n\nFW UPDATE get BTBP flag status\n\n\n");
    memset(&bootFlags, 0, sizeof(bootFlags));
    ret = inu_fw_update__getBootFlags(bootFlags);
    if (ERRG_FAILED(ret))
        assert(ret);
    printf("\n\nBTBP %d\n\n\n", bootFlags[0]);

    printf("\n\nFW UPDATE copy FW Image\n\n\n");
    char filebin[500];

/*    memset(filebin, 0, sizeof(filebin));
    strcpy(filebin, get_installation_path());
    strcat(filebin, PATH_SEPARATOR"config");*/
    strcpy(filebin, "C:\\tmp\\fwImage.bin");

    copyFwImageParams.filebin = filebin;
    copyFwImageParams.fwVersion = fwVersion[0];
    copyFwImageParams.securityVersion = securityVersion;

    ret = inu_fw_update__copyFwImage(&copyFwImageParams);
    if (ERRG_FAILED(ret))
        assert(ret);

    printf("\n\nFW UPDATE flash FW Image\n\n\n");
    ret = inu_fw_update__flashFwImage();

    if (ERRG_FAILED(ret))
        assert(ret);

    //while (1) {};

    printf("\n\nLOG_ENABLE\n\n\n");
    ret = CDC_MNGRG_logEnable(handle, TRUE, myLogCallbackFunc);
    if (ERRG_FAILED(ret))
        assert(ret);

#if 1
    printf("\n\nSW RESET\n\n\n");
    ret = CDC_MNGRG_swReset(handle);
    if (ERRG_FAILED(ret))
        assert(ret);
#endif

    printf("\n\nINIT VE\n\n\n");
    ret = CDC_MNGRG_initVeMngr(handle);
    if (ERRG_FAILED(ret))
        assert(ret);

    bokehParams.visionBokehParams.blurLevel = 0.24f;
    bokehParams.visionBokehParams.visionProcResolution = VISION_PROC_1080P;
    bokehParams.visionBokehParams.depthOutput = false;
    bokehParams.visionBokehParams.noFaceFramesToKeepBlur = 888;

    printf("\n\nENABLE BOKEH\n\n\n");
    ret = CDC_MNGRG_enableBokeh(handle, &bokehParams);
    if (ERRG_FAILED(ret))
        assert(ret);

    printf("\n\nENABLE BOKEH (NULL)\n\n\n");
    ret = CDC_MNGRG_enableBokeh(handle, NULL);
    if (ERRG_FAILED(ret))
        assert(ret);

    printf("\n\nSET BOKEH PARAMS\n\n\n");
    ret = CDC_MNGRG_setBokehParams(handle, &bokehParams);
    if (ERRG_FAILED(ret))
        assert(ret);

    printf("\n\nGET BOKEH PARAMS\n\n\n");
    memset(&bokehParams, 0, sizeof(bokehParams));
    ret = CDC_MNGRG_getBokehParams(handle, &bokehParams);
    if (ERRG_FAILED(ret))
        assert(ret);

    printf("\n\nDISABLE BOKEH\n\n\n");
    ret = CDC_MNGRG_disableBokeh(handle, NULL);
    if (ERRG_FAILED(ret))
        assert(ret);

    while (1) {};

    ret = CDC_MNGRG_close(handle);
    if (ERRG_FAILED(ret))
        assert(ret);

    return 0;
}
