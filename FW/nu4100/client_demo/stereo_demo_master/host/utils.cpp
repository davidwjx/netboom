
#define _CRT_SECURE_NO_WARNINGS

#include "stereo_demo_example.h"

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif


using  namespace std;
using namespace InuDev;

#ifdef _MSC_VER
static HANDLE mutexes[MAX_MUTEXES];
#else
pthread_mutex_t mutexes[MAX_MUTEXES] = { PTHREAD_MUTEX_INITIALIZER };
#endif


#ifdef _MSC_VER
/* mutex_create */
void mutex_create(int mutex_id)
{
	mutexes[mutex_id] = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (mutexes[mutex_id] == NULL) {
		printf("mutex_create: Failed to create mutex ID: %d\n", mutex_id);
	}
}

/* mutex_lock */
void mutex_lock(int mutex_id)
{
	WaitForSingleObject(mutexes[mutex_id], INFINITE);
}

/* mutex_unlock */
void mutex_unlock(int mutex_id)
{
	if (!ReleaseMutex(mutexes[mutex_id])) {
		printf("mutex_unlock: Failed to release mutex ID: %d\n", mutex_id);
	}
}
#else
/* mutex_create */
void mutex_create(int mutex_id)
{

}

/* mutex_lock */
void mutex_lock(int mutex_id)
{
	pthread_mutex_lock(&mutexes[mutex_id]);
}

/* mutex_unlock */
void mutex_unlock(int mutex_id)
{
	pthread_mutex_unlock(&mutexes[mutex_id]);
}

#endif


/* build_file_name */
string build_file_name(int num_of_names, ...)
{
	string output_string;
	va_list ap;
	char *str;
	int i;
#ifdef _MSC_VER
	char *delimiter_str = (char *)"\\";
#else
	char *delimiter_str = (char *)"//";
#endif

	va_start(ap, num_of_names);

	str = va_arg(ap, char *);
	output_string = str;

	for (i = 2; i <= num_of_names; i++) {
		str = va_arg(ap, char *);
		output_string.append(delimiter_str);
		output_string.append(str);
	}

	va_end(ap);

	return(output_string);
}
