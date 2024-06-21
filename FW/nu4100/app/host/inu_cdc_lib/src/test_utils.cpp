
#include "process.h"
#include "assert.h"
#include "stdarg.h"

#include "inu_common.h"
#include "inu_usb.h"

#include <cstring>
#include <cfgmgr32.h>
#include <mutex>

#define INU_DEV_BIN_FOLDER      "C:\\program files\\Inuitive\\InuVE\\bin"

volatile bool g_device_operational = false;
volatile bool g_device_boot = false;
volatile bool g_device_chipVersion = false;

const char* usbEventToString(INU_DEFSG_usbStatusE event)
{
    switch (event)
    {
    case(INU_DEFSG_USB_EVT_REMOVAL):
        return "INU_DEFSG_USB_EVT_REMOVAL";
    case(INU_DEFSG_USB_EVT_INSERTION):
        return "INU_DEFSG_USB_EVT_INSERTION";
    case(INU_DEFSG_USB_EVT_BOOT_COMPLETE):
        return "INU_DEFSG_USB_EVT_BOOT_COMPLETE";
    case(INU_DEFSG_USB_EVT_BOOT_FAILED):
        return "INU_DEFSG_USB_EVT_BOOT_FAILED";
    default:
        return "UNKNOWN EVENT";
    }
}

int usbEventCallbackWrapper(INU_DEFSG_usbStatusE event, UINT32 val)
{
    printf("USB BOOT callback!: event = %s\n", usbEventToString(event));
    if ((event == INU_DEFSG_USB_EVT_BOOT_COMPLETE) || (event == INU_DEFSG_USB_EVT_REMOVAL))
    {
        g_device_boot = true;
    }
    else
    {
        g_device_boot = false;
    }
    return 0;
}

ERRG_codeE inu__usb_boot(int (*hotplugCallbackFunc)(INU_DEFSG_usbStatusE, UINT32), const char* bootPath, int usbId, int externalHp, int bootid_override, int thrdSleepUs)
{
    ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
    InuUsbParamsT inuParams;
    Sleep(2000);            // 2 seconds delay before reboot to ensure keep alive sync

    memset(&inuParams, 0, sizeof(inuParams));

    inuParams.hp_callback = (InuHotplugCallback)hotplugCallbackFunc;
    strcpy(inuParams.boot_path, bootPath);
    inuParams.logLevel = INU_USB_LOG_INFO;
    inuParams.usbFd = 0;
    inuParams.usbDeviceAddress = usbId;
    inuParams.usbUseExternalHpCallback = externalHp;
    inuParams.bootId = bootid_override;
    inuParams.filterId = -1;
    inuParams.thrSleepTimeUs = thrdSleepUs;
    inu_usb_boot_set_reset_in_progress(0);

    if (inu_usb_init(&inuParams) != INU_USB_ERR_SUCCESS)
    {
        printf("boot: can't initialize inu_usb_lib\n");
        ret = INU_DEVICE__ERR_UNEXPECTED;
    }

    return ret;
}

/*-- boot device */
ERRG_codeE FwLib_boot(int bootId)
{
    ERRG_codeE ret;
    //boot via usb
    ret = inu__usb_boot(usbEventCallbackWrapper, INU_DEV_BIN_FOLDER, _getpid(), 0, bootId, 5000);

    if (ERRG_SUCCEEDED(ret))
    {
        //wait for boot to complete
        while (!g_device_boot)
        {
#if (defined _WIN32) || (defined _WIN64)
            Sleep(10);
#else
            sleep(10);
#endif
        }
    }
    return ret;
}

ERRG_codeE FwLib_boot_deinit()
{
    ERRG_codeE status = (ERRG_codeE)RET_SUCCESS;

    inu_usb_deinit();
    g_device_boot = false;

    return status;
}

int usb_boot_device(int bootId)
{
    ERRG_codeE ret = FwLib_boot(bootId);
    if (ERRG_FAILED(ret))
        assert(ret);

    inu_usb_deinit();

    return 0;
}

void myLogCallbackFunc(const char* str)
{
    printf(str);
}

struct S
{
    HANDLE ev;
    HANDLE Com;
    std::mutex mutex;
} data;

UINT32* g_comPortHandle;

DWORD MyCMInterfaceNotification(HCMNOTIFICATION hNotify, PVOID dataPtr, CM_NOTIFY_ACTION action, PCM_NOTIFY_EVENT_DATA ev, DWORD EventDataSize)
{
    auto& data = *(S*)dataPtr;
    if (action != CM_NOTIFY_ACTION_DEVICEINTERFACEARRIVAL)
    {
        return ERROR_SUCCESS;
    }
    // Note iterate over the interfaces and use CM_Get_Device_Interface_PropertyW with DEVPKEY_Device_HardwareIds to find your camera

    wchar_t* symLink = ev->u.DeviceInterface.SymbolicLink;
    if (symLink != wcsstr(symLink, L"\\\\?\\USB#VID_03F0&PID_089C&MI_02"))
    {
        return ERROR_SUCCESS;
    }

    OutputDebugStringW(L"Found Device");
    OutputDebugStringW(ev->u.DeviceInterface.SymbolicLink);
    {
        //auto mutexGuard = std::lock_guard(data.mutex);
        std::lock_guard<std::mutex> lock(data.mutex);
        const auto waitStat = WaitForSingleObject(data.ev, 0);
        if (waitStat == WAIT_OBJECT_0) return ERROR_SUCCESS;
        data.Com = CreateFileW(ev->u.DeviceInterface.SymbolicLink, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, NULL);
        if (data.Com == INVALID_HANDLE_VALUE) return ERROR_SUCCESS;
        OutputDebugStringW(L"Set device from callback");
        SetEvent(data.ev);

        *g_comPortHandle = (UINT32)data.Com;
    }

    printf("------------------------- Module is ready to work -------------------------");

    return ERROR_SUCCESS;
}

int autoDetectComPort(UINT32* comPortHandle)
{
#if 0
    CM_NOTIFY_FILTER NotifyFilter = { 0 };
    NotifyFilter.cbSize = sizeof(NotifyFilter);
    NotifyFilter.FilterType = CM_NOTIFY_FILTER_TYPE_DEVICEINTERFACE;
    NotifyFilter.u.DeviceInterface.ClassGuid = GUID_DEVINTERFACE_COMPORT;
    
    g_comPortHandle = comPortHandle;

    struct S data;
    
    data.ev = CreateEvent(nullptr, true, FALSE, nullptr);
    //orig code if (!data.ev) return -8;
    if (!data.ev)
    {
        return -1;
    }
    HCMNOTIFICATION reg;
    auto regStat = CM_Register_Notification(&NotifyFilter, &data.ev, (PCM_NOTIFY_CALLBACK)&MyCMInterfaceNotification, &reg);
    if (regStat != CR_SUCCESS)
    {
        return -1;
    }

    printf("------------------------ Wait for COM port search  -----------------------");
#endif
    return 0;
}