
#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include "inu_types.h"
#include "inu_defs.h"
#include "err_defs.h"

void myLogCallbackFunc(const char* str);
int usb_boot_device(int bootId);
int autoDetectComPort(UINT32* comPortHandle);

#endif //__TEST_UTILS_H__

