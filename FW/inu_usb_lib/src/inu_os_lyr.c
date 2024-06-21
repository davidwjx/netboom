#include <unistd.h>
#include <sys/time.h>
#include "inu_os_lyr.h"

int minit(mutexT *mutexP)
{
	int ret = 0;
#if defined(__LINUX__) || defined(__linux__)
	pthread_mutexattr_t attr;
	ret = pthread_mutexattr_init(&attr);
	if(!ret) 
	{
		//init mutex with attributes
		ret = pthread_mutex_init(mutexP, &attr);
		if(!ret)
		{
			ret = pthread_mutexattr_destroy(&attr);
		}
		else
		{
			(void)pthread_mutexattr_destroy(&attr);
		}
	}
#elif defined(WIN32) || defined(WIN64)
	*mutexP = CreateMutex(NULL, FALSE, NULL);
	if (NULL == *mutexP)
		ret = -1;
#endif

	return ret;
}

void mlock(mutexT *mutexP)
{
#if defined(__LINUX__) || defined(__linux__)
	pthread_mutex_lock(mutexP);
#elif defined(WIN32) || defined(WIN64)
	//(WaitForSingleObject(*mutexP, INFINITE) == WAIT_OBJECT_0) ? 0 : 1;
	WaitForSingleObject(*mutexP, INFINITE);
#endif
}

void munlock(mutexT *mutexP)
{
#if defined(__LINUX__) || defined(__linux__)
	pthread_mutex_unlock(mutexP);
#elif defined(WIN32) || defined(WIN64)
	ReleaseMutex(*mutexP);
#endif
}

void mdeinit(mutexT *mutexP)
{
#if defined(__LINUX__) || defined(__linux__)
	pthread_mutex_destroy(mutexP);
#elif defined(WIN32) || defined(WIN64)
	CloseHandle(*mutexP);
#endif
}

void inu_usb_usleep(unsigned long usec)
{
#if defined(__LINUX__) || defined(__linux__)
	usleep(usec);
#elif defined(WIN32) || defined(WIN64)
	Sleep(usec/1000);
#endif
}


int inu_usb_get_sec_time()
{
	int sec = 0;
#if defined(__LINUX__) || defined(__linux__)
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec;
#elif defined(WIN32) || defined(WIN64)

	sec = GetTickCount() / 1000;
#endif
	return sec;
}

inline void set_locked(int *var, int value, mutexT *m)
{
	mlock(m);
	*var = value;
	munlock(m);
}

inline unsigned int get_locked(int *var, mutexT *m)
{
	unsigned int value;

	mlock(m);
	value = *var;
	munlock(m);

	return value;
}


