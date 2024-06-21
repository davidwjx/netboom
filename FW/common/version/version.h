#ifndef  _VERSION_H
#define  _VERSION_H


#define MAJOR_VERSION      1
#define MINOR_VERSION      2
#define BUILD_VERSION      9
#define SUB_BUILD_VERSION  161

#define MAJOR_VERSION_STR     "1"
#define MINOR_VERSION_STR     "2"
#define BUILD_VERSION_STR     "009"
#define SUB_BUILD_VERSION_STR "161"
#if DEFSG_IS_HOST
#define VERSION_PRINT   LOGG_PRINT(LOG_INFO_E, NULL, "FW HOST version: %s.%s.%s.%s. Date: %s. Time: %s.\n", MAJOR_VERSION_STR, MINOR_VERSION_STR, BUILD_VERSION_STR, SUB_BUILD_VERSION_STR, __DATE__, __TIME__);
#else
#define VERSION_PRINT   LOGG_PRINT(LOG_INFO_E, NULL, "FW GP   version: %s.%s.%s.%s. Date: %s. Time: %s.\n", MAJOR_VERSION_STR, MINOR_VERSION_STR, BUILD_VERSION_STR, SUB_BUILD_VERSION_STR, __DATE__, __TIME__);
#endif

#endif

