// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the COMMONALGO_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// COMMONALGO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#pragma once


#ifdef ALG_WEBCAM_INTEGRATION_EXPORTS
    #ifdef __GNUC__
        #ifndef __linux__
            #define ALG_WEBCAM_INTEGRATION_API __attribute__ ((dllexport))
        #else 
            #define ALG_WEBCAM_INTEGRATION_API __attribute__ ((visibility ("default")))
        #endif
    #else
        #define ALG_WEBCAM_INTEGRATION_API __declspec(dllexport)
    #endif
    #define ALG_WEBCAM_INTEGRATION_API_TEMPLATE
#else
    #ifdef __GNUC__
        #ifndef __linux__
            #define ALG_WEBCAM_INTEGRATION_API
        #else
            #define ALG_WEBCAM_INTEGRATION_API
        #endif
    #else
        #define ALG_WEBCAM_INTEGRATION_API __declspec(dllimport)
    #endif
    #define ALG_WEBCAM_INTEGRATION_API_TEMPLATE extern
#endif
