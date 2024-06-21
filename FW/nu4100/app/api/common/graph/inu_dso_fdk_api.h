#ifndef _INU_DSO_FDK_API_H_
#define _INU_DSO_FDK_API_H_

#include "inu2.h"

#define INU_DSO_FDK__MAX_NAME_STRING_LEN (100)

typedef void* inu_dso_fdkH;

/* Callback invoked by the plugin for handling datas */
typedef void (inu_dso_fdk__operate_complete_t) (inu_dso_fdkH);
typedef void (inu_dso_fdk__new_data_t)         (inu_dso_fdkH, inu_dataH);
typedef void (inu_dso_fdk__done_data_t)        (inu_dso_fdkH, inu_dataH);

typedef struct
{
	inu_dso_fdk__operate_complete_t 	*operate_complete_cb_p;
	inu_dso_fdk__new_data_t				*new_data_cb_p;
	inu_dso_fdk__done_data_t				*done_data_cb_p;
}inu_dso_fdk__callbacks_p_t;

/* interface of FDK plugin */
typedef void (inu_dso_fdk__ctor_func_t)       (inu_dso_fdkH meH, inu_dso_fdk__callbacks_p_t callbacks);
typedef void (inu_dso_fdk__dtor_func_t)       (inu_dso_fdkH meH);
typedef void (inu_dso_fdk__setState_func_t)   (inu_dso_fdkH meH, inu_function__stateParam state);
typedef void (inu_dso_fdk__operate_func_t)    (inu_dso_fdkH meH, inu_dataH dataInputs[]);
typedef void (inu_dso_fdk__rxData_func_t)     (inu_dso_fdkH meH, unsigned char *bufP, unsigned int len);
typedef void (inu_dso_fdk__txDataDone_func_t) (inu_dso_fdkH meH, unsigned char *bufP);

typedef struct
{
	inu_deviceH deviceH;
	char        name[INU_DSO_FDK__MAX_NAME_STRING_LEN];
}inu_dso_fdk__initParams;

ERRG_codeE inu_dso_fdk__new(inu_dso_fdkH *meH, inu_dso_fdk__initParams *initParamsP);
void inu_dso_fdk__delete(inu_dso_fdkH meH);
void inu_dso_fdk__operate_complete(inu_dso_fdkH meH, inu_dataH dataH);

#endif