/****************************************************************************
 *
 *   FileName: inu_os_lyr.h
 *
 *   Author:  David Wan
 *
 *   Date: 2022.10.28
 *
 *   Description: Inuitive user space abstraction for USB access
 *   
 ****************************************************************************/
#if !defined __INU_OS_LYR_H__
#define __INU_OS_LYR_H__

#if defined(__linux__) || defined(__LINUX__)

#include <pthread.h>
typedef pthread_mutex_t mutexT;

#elif defined(WIN32) || defined(WIN64)

typedef void* mutexT;

#endif

#ifdef __cplusplus
extern "C" {
#endif

int minit(mutexT *mutexP);
void mlock(mutexT *mutexP);
void munlock(mutexT *mutexP);
void mdeinit(mutexT *mutexP);
void inu_usb_usleep(unsigned long usec);
int inu_usb_get_sec_time();

void set_locked(int *var, int value, mutexT *m);
unsigned int get_locked(int *var, mutexT *m);

#ifdef __cplusplus
}
#endif

#endif //__INU_OS_LYR_H__
