#ifndef _INU_STREAMER_API_H_
#define _INU_STREAMER_API_H_

#include "inu2.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef void* inu_streamerH;

    /**
    * tx done callback
    */
    typedef void (inu_streamer__buffWriteCompleteCB)(inu_streamerH meH, UINT8*, void *cookie);

    typedef enum
    {
        INU_STREAMER__CONN_USB_IF0_E = 0,
        INU_STREAMER__CONN_USB_IF1_E,
        INU_STREAMER__CONN_USB_IF2_E,
        INU_STREAMER__CONN_USB_IF3_E,
        INU_STREAMER__CONN_USB_IF4_E,
        INU_STREAMER__CONN_USB_IF5_E,
        INU_STREAMER__CONN_UART_E,
        INU_STREAMER__CONN_TCP_E,
        INU_STREAMER__CONN_UDP_E,
        INU_STREAMER__CONN_CDC_E,
        INU_STREAMER__CONN_STUB_E,
        INU_STREAMER__CONN_UVC1_E,
        INU_STREAMER__CONN_UVC2_E,
        INU_STREAMER__CONN_UVC3_E,
        INU_STREAMER__CONN_UVC4_E,
        INU_STREAMER__NUM_CONNECTIONS_E
    } inu_streamer__connIdE;
 

    typedef enum
    {
       INU_STREAMER__UVC_REG =0,
       INU_STREAMER__UVC_BOKEH,
       INU_STREAMER__UVC_MAX,
       INU_STREAMER__REGULAR = 0xff
    }inu_streamer__type_e;

    typedef struct
    {
        inu_deviceH  deviceH;
        inu_dataH    dataH;
        inu_graphH   graphH;
        inu_streamer__connIdE dataConnId;
    }inu_streamer_out__initParams;

    typedef struct
    {
        inu_deviceH  deviceH;
        inu_graphH   graphH;
        inu_streamer__connIdE dataConnId;
        inu_streamer__buffWriteCompleteCB          *p_buffWriteCompleteCB;
    }inu_streamer_in__initParams;

    typedef struct
    {
       int          refId;
       unsigned int numBuffers;
       unsigned int bufferSize;
       unsigned int hdrSize;
     }inu_streamer_poolCfg;

    ERRG_codeE inu_streamer__read(inu_streamerH meH, inu_dataH *dataH_p);
    ERRG_codeE inu_streamer__free(inu_streamerH meH, inu_dataH dataH);
    ERRG_codeE inu_streamer__write(inu_streamerH meH, char *buf, int len, inu_data__hdr_t *hdrP, void *cookie);
    void inu_streamer__setBufWriteCB(inu_streamerH meH, inu_streamer__buffWriteCompleteCB writeBufCB);
    ERRG_codeE inu_streamer__getPoolCfg(inu_streamerH meH,inu_streamer_poolCfg *poolCfg);
    inu_streamer__type_e inu_streamer__getStreamType(inu_streamerH meH);
    UINT8 inu_streamer__isUvcActive(inu_streamerH meH);
    void inu_streamer__setResetUvcActive(inu_streamerH meH,unsigned int activeFlag);
    UINT8 inu_streamer__isShtterActive();
    void inu_streamer__setShtterActive(inu_streamerH meH,unsigned int activeFlag);

#ifdef __cplusplus
}
#endif

#endif
