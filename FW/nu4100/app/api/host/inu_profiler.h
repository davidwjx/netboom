/****************************************************************************
 *
 *   FileName: inu_.h
 *
 *   Author:  Konstantin Sinyuk.
 *
 *   Date: 
 *
 *   Description: Inuitive perfomace profiler API 
 *   
 ****************************************************************************/
#ifndef INU__PROFILER_H
#define INU__PROFILER_H

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
//#define INU_PROFILER
//#define INU_PROFILER_CONN_LYR
//#define INU_PROFILER_USB_HOST
//#define INU_PROFILER_IPC_GP

#define INU_PROFILER_MAX_TRACES 		10000 
#define INU_PROFILER_MAX_TRACE_LENGTH	30
#define HISTOGRAM_MAX_STEPS	20
#define HISTOGRAM_RESOLUTION_MICROS	1000


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

#ifdef INUPROFILER_EXPORTS
    #ifdef __GNUC__
              #ifdef __linux__
                     #define INUPROFILER_API __attribute__((visibility("default")))
              #else
                     #define INUPROFILER_API __attribute__ ((visibility ("default")))
              #endif
    #else
        #define INUPROFILER_API __declspec(dllexport)
    #endif
    #define INUPROFILER_API_TEMPLATE
#else
    #ifdef __GNUC__
              #ifdef __linux__
                     #define INUPROFILER_API
              #else
                     #define INUPROFILER_API __attribute__ ((visibility ("default")))
              #endif
    #else
        #define INUPROFILER_API __declspec(dllimport)
    #endif
    #define INUPROFILER_API_TEMPLATE extern
#endif

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
#ifdef INU_PROFILER
#define INUG_profiler_get_timestamp()   INUG_profiler_get_timestamp_ex (__FUNCTION__,__LINE__)	
INUPROFILER_API void INUG_profiler_get_timestamp_ex(const char * functionName,const int lineNumber);
INUPROFILER_API void INUG_profiler_start_trace();
INUPROFILER_API void INUG_profiler_end_trace();
INUPROFILER_API void INUG_profiler_init();
INUPROFILER_API void INUG_profiler_print_statistics(int tresholdInMicroS);
#else
	#define INUG_profiler_get_timestamp()  do { } while (0)
	#define INUG_profiler_get_timestamp_ex(a,b); do {} while(0)
	#define INUG_profiler_start_trace() do { } while (0)
	#define INUG_profiler_end_trace() do { } while (0)
	#define INUG_profiler_init() do { } while (0)
	#define INUG_profiler_print_statistics(a) do { } while (0)
#endif

#ifdef __cplusplus
}
#endif
#endif // INU_PROFILER_H 
