#ifndef _INU_PP_API_H_
#define _INU_PP_API_H_

#include "inu2.h"

typedef void* inu_ppH;

typedef enum
{
   INU_PP_PINHOLE = 0,
   INU_PP_EQUIDISTANCE = 31,
   INU_PP_MAX_NUM_MODELS,
}INU_PP__modelE;

typedef struct
{
   int sensorGroup;
   float baseline;
   float fx;
   float fy;
   float cxL;
   float cxR;
   float cy;
   int maxDepthMm;
   int voxel_leaf_x_size;
   int voxel_leaf_y_size;
   int voxel_leaf_z_size;
   int vgf_flag;
   int flip_x;
   int flip_y;
   int flip_z;
   INU_PP__modelE model;
}inu_pp__parameter_list_t;

typedef struct
{
   inu_graphH  graphH;
   inu_deviceH deviceH;
   inu_pp__parameter_list_t paramsList;
}inu_pp__initParams;

typedef enum
{
   INU_PP_SPLIT,
   INU_PP_CROP,
   INU_PP_CSC,
   INU_PP_SCALE,   
   INU_PP_REFORMAT,   
   INU_PP_VERTICAL_BINNING,
   INU_PP_ED2D,
   INU_PP_ED2PLY,
   INU_PP_DEPTH2PLY,
   INU_PP_VGF,
   INU_PP_MAX_NUM_ACTIONS,
}INU_PP__actionE;

ERRG_codeE inu_pp__new(inu_ppH *meH, inu_pp__initParams *initParamsP);
void inu_pp__delete(inu_ppH meH);
ERRG_codeE inu_pp__updateParams(inu_ppH meH, inu_pp__parameter_list_t *cfgParamsP);

#endif


