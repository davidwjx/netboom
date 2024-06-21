#ifndef _INU_CDNN_DATA_API_H_
#define _INU_CDNN_DATA_API_H_

#define CDNNG_MAX_BLOB_SIZE         (10*1024*1024)
#define CDNNG_MAX_TAILS             (30)

typedef void* inu_cdnn_dataH;
#pragma pack(push, 1)
/***************************************************************************
inu_cdnn_data__tailHeaderT - This structure describes the output buffer of a network
****************************************************************************/
typedef struct
{
    int fracBits;       // In case of using fixed point: the scale factor in which the fixed point data is represented. 
                        // Output x(float) is represented as x(float) = scale_factor * x(fixed) 
} inu_cdnn_data__tailCevaFieldsT;

typedef struct 
{
    float scale;                      // Fixed point values shoule be divided by this scale
    int allocCount;                   // Relevant for Bboxes output - Number of allocated bboxes
    int validCount;                   // Relevant for Bboxes output - Number of valid bboxes
    int bboxScale;                    // Relevant for Bboxes output - Use short with scale instead of float
    int confidenceScale;              // Relevant for Bboxes output - Use short with scale instead of float
} inu_cdnn_data__tailSynopsysFieldsT;

typedef struct 
{
    // Fields relevant for ceva
    inu_cdnn_data__tailCevaFieldsT tailCevaFields;
    // Fields relevant for Synopsys
    inu_cdnn_data__tailSynopsysFieldsT tailSynopsysFields;
} inu_cdnn_data__tailFieldsT;


/***************************************************************************
inu_cdnn_data__tailHeaderT - This structure describes the output buffer of a network
****************************************************************************/
typedef struct
{
   int elementSize;
   int elementCount;
   int width;
   int height;
   int inputs;
   inu_cdnn_data__tailFieldsT cnnFields;
}inu_cdnn_data__tailHeaderT;

/***************************************************************************
inu_cdnn_data__hdr_t - is the header at the beginning of the result buffer
from CEVA to Host.
numOfTails is the number of concatenated inu_cdnn_data__tailHeaderT, comming right after this struct.

totOutputSize - total results size
frameId - webcam frame number
numOfTails - number of tails (blobs)
tailHeader[CDNNG_MAX_TAILS] - tail header for each tail (blob)
offsetToBlob[CDNNG_MAX_TAILS]    - offset to tail (blob), assuming tail 0 offset is 0
- assuming that tail 0 starting righ after this buffer
****************************************************************************/

typedef struct
{
   inu_data__hdr_t           dataHdr;       //header (timestamp & index)
   UINT32 totOutputSize;
   UINT32 frameId;
   UINT32 numOfTails;
   int network_id;
   inu_load_network_cdnnEngineTypeE engineType;
   inu_cdnn_data__tailHeaderT tailHeader[CDNNG_MAX_TAILS];
   UINT32 offsetToBlob[CDNNG_MAX_TAILS];
} inu_cdnn_data__hdr_t;

typedef struct
{
   inu_data__initParams    dataInitParams;
}inu_cdnn_data__initParams;
#pragma pack(pop)

#endif
