#ifndef _INU_PROJRCTOR_API_H_
#define _INU_PROJRCTOR_API_H_

#include "inu2.h"

typedef void* inu_projectorH;

typedef enum
{
   INU_SENSOR__TYPE_PATTERN = 0,
   INU_SENSOR__TYPE_FLOOD,
   INU_SENSOR__TYPE_PATTERN_FLOOD,
}inu_projector__type_e;

typedef enum
{
   INU_PROJECTOR__ID_0 = 0,
   INU_PROJECTOR__ID_1,
   INU_PROJECTOR__ID_2,
   INU_PROJECTOR__MAX_NUM_ID,
}inu_projector__id_e;


typedef enum
{
   INU_PROJECTOR__STATE_OFF = 0,
   INU_PROJECTOR__STATE_LOW,
   INU_PROJECTOR__STATE_HIGH
}inu_projector__state_e;

typedef enum
{
   INU_PROJECTOR__MODEL_INUITIVE = 0,
   INU_PROJECTOR__MODEL_AMS,
}inu_projector__model_e;

typedef struct
{
   inu_projector__id_e               id;
   inu_projector__model_e         model;
   inu_projector__type_e          type;
   inu_projector__state_e         state;
}inu_projector__parameter_list_t;


#endif
