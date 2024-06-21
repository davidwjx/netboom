#ifndef _EXAMPLE_HEADER_
#define _EXAMPLE_HEADER_

#define _CRT_SECURE_NO_WARNINGS

#ifdef _MSC_VER
#include <process.h>
#include <Windows.h>

#else
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <memory>
#include <queue>
#include <mutex>
#include <stdarg.h>
#include <condition_variable>

#include "InuSensorExt.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define MAX_MUTEXES		1
#define MUTEX_DISPLAY	0

/* Net ID's */
#define SSD_NET_ID      0


typedef struct
{
	int debug;
	int label;
	float confidence;
	float x_min, y_min, x_max, y_max;
} BBOX;

std::string build_file_name(int num_of_names, ...);

void mutex_create(int mutex_id);
void mutex_lock(int mutex_id);
void mutex_unlock(int mutex_id);

#endif
