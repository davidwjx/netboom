#if !defined __IINFO_H__
#define __IINFO_H__

#include <stdio.h>
#if !defined _WIN32
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#else
#include <windows.h>
#endif

#include <time.h>


#if !defined _WIN32

//static void getCurThdName(char* const thdname) {
#if defined LINUX || defined _ANDROID_LINUX
//	prctl(PR_GET_NAME, (unsigned long)thdname, 0, 0, 0); 
#endif
//}

//#define SECOND_DIGIT 10000000000ll
//unsigned int msec = lltime / SECOND_DIGIT > 0 ? lltime % 1000 : 0; 
//time_t sec = (time_t)(lltime / SECOND_DIGIT > 0 ? lltime / 1000 : lltime); 
//char thdname[32] = {0}; getCurThdName(thdname);

#define __FILENAME__ strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
#define iinfo(fmt, args...) { \
	long long lltime = 0; \
	char ct[32] = {0}; \
	struct timeval tv; \
	if (gettimeofday(&tv, NULL) == 0) \
		lltime = (long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000; \
	time_t sec = (time_t)(lltime / 1000); \
	unsigned int msec = lltime % 1000; \
	struct tm tt; \
	localtime_r(&sec, &tt); \
	sprintf(ct, "%02d-%02d %02d:%02d:%02d.%03d [%x] ", tt.tm_mon, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, msec, (int)pthread_self()); \
	printf("%s %-32s %-32s %-4d\t" fmt, ct, __FILENAME__, __FUNCTION__, __LINE__, ##args); }

//#define ierror(fmt, args...) iinfo(fmt, args...)

// log to file 
#define log2file(fmt, args...) { \
	FILE* fp = fopen("/tmp/iinfo.log", "a+"); \
	if (fp) { \
		long long lltime = 0; \
		char ct[32] = {0}; \
		struct timeval tv; \
		if (gettimeofday(&tv, NULL) == 0) \
			lltime = (long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000; \
		time_t sec = (time_t)(lltime / 1000); \
		unsigned int msec = lltime % 1000; \
		struct tm tt; \
		localtime_r(&sec, &tt); \
		sprintf(ct, "%02d-%02d %02d:%02d:%02d.%03d [%x] ", tt.tm_mon, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, msec, (int)pthread_self()); \
		fprintf(fp, "%s %-32s %-32s %-4d\t" fmt "\n", ct, __FILENAME__, __FUNCTION__, __LINE__, ##args); \
		fflush(fp); \
		fclose(fp); \
	} }

#else  // _WIN32

#define __FILENAME__ strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__
#define iinfo(fmt,...) { \
	SYSTEMTIME tt; \
	GetLocalTime(&tt); \
	char ct[32] = {0}; \
	sprintf_s(ct, 32, "%02d:%02d:%02d.%03d [%x] ", tt.wHour, tt.wMinute, tt.wSecond, tt.wMilliseconds, (unsigned int)GetCurrentThreadId()); \
	printf( "%s %-32s %-32s %-4d:\t" fmt "\n", ct, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	fflush(stdout); \
	};


#endif 

#define ierror iinfo
#endif // __IINFO_H__
