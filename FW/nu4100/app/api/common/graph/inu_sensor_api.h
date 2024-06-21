#ifndef _INU_SENSOR_API_H_
#define _INU_SENSOR_API_H_

#include "inu2.h"

typedef void* inu_sensorH;

typedef enum
{
   INU_SENSOR__ID_0 = 0,
   INU_SENSOR__ID_1,
   INU_SENSOR__ID_2,
   INU_SENSOR__ID_3,
   INU_SENSOR__ID_4,
   INU_SENSOR__ID_5,
   INU_SENSOR__MAX_NUM_ID,
}inu_sensor__id_e;

typedef enum
{
   INU_SENSOR__ROLE_LEFT = 0,
   INU_SENSOR__ROLE_RIGHT,
   INU_SENSOR__ROLE_COLOR
}inu_sensor__role_e;

typedef enum
{
   INU_SENSOR__FUNCTION_MASTER = 0,
   INU_SENSOR__FUNCTION_SLAVE,
   INU_SENSOR__FUNCTION_SINGLE,
   INU_SENSOR__FUNCTION_INVALID,
}inu_sensor__function_e;

typedef enum
{
   INU_SENSOR__POWER_HZ_50 = 0,
   INU_SENSOR__POWER_HZ_60,
   INU_SENSOR__POWER_HZ_INVALID,
}inu_sensor__power_hz_e;

typedef enum
{
   INU_SENSOR__EXP_MODE_MANUAL = 0,
   INU_SENSOR__EXP_MODE_AUTO,
   INU_SENSOR__EXP_MODE_INVALID,
}inu_sensor__exp_mode_e;

typedef enum
{
   INU_SENSOR__TRIGGER_SRC_DISABLE_E = 0,
   INU_SENSOR__TRIGGER_SRC_SW_TIMER1_E,
   INU_SENSOR__TRIGGER_SRC_SW_TIMER2_E,
   INU_SENSOR__TRIGGER_SRC_SW_TIMER3_E,
   INU_SENSOR__TRIGGER_SRC_HW_TIMER_E,
   INU_SENSOR__TRIGGER_SRC_SLU0,
   INU_SENSOR__TRIGGER_SRC_SLU1,
   INU_SENSOR__TRIGGER_SRC_SLU2,
   INU_SENSOR__TRIGGER_SRC_SLU3,
   INU_SENSOR__TRIGGER_SRC_SLU4,
   INU_SENSOR__TRIGGER_SRC_SLU5,
}inu_sensor__trigger_src_e;

typedef enum
{
   INU_SENSOR__ORIENTATION_ORIGINAL = 0,
   INU_SENSOR__ORIENTATION_MIRROR,
   INU_SENSOR__ORIENTATION_FLIP,
   INU_SENSOR__ORIENTATION_MIRROR_FLIP,
   INU_SENSOR__ORIENTATION_INVALID,
}inu_sensor__orientation_e;

typedef enum
{
   INU_SENSOR___MODEL_NONE_E = 0,
   INU_SENSOR___MODEL_AMS_CGSS130_E = 130,
   INU_SENSOR___MODEL_AR_134_E = 134,
   INU_SENSOR___MODEL_AR_135_E = 135,
   INU_SENSOR___MODEL_AR_135X_E = 136,
   INU_SENSOR___MODEL_APTINA_1040_E = 1040,
   INU_SENSOR___MODEL_OV_7251_E = 7251,
   INU_SENSOR___MODEL_OV_2685_E = 2685,
   INU_SENSOR___MODEL_OV_9282_E = 9282,
   INU_SENSOR___MODEL_OV_5675_E = 5675,
   INU_SENSOR___MODEL_OV_8856_E = 8856,
   INU_SENSOR___MODEL_OV_4689_E = 4689,
   INU_SENSOR___MODEL_CGS_132_E  = 132,
   INU_SENSOR___MODEL_OV_9782_E = 9782,
   INU_SENSOR___MODEL_CGS_031_E = 031
}inu_sensor__model_e;

typedef enum
{
   INU_SENSORS__LEN_TYPE_WIDE = 0,
   INU_SENSORS__LEN_TYPE_NARROW = 1,
   INU_SENSORS__LEN_TYPE_MAX70 = 2,
   INU_SENSORS__LEN_TYPE_MAX45 = 3,
}inu_sensor__len_type_e;

typedef enum
{
   INU_SENSOR__CONTEXT_A        = 0,
   INU_SENSOR__CONTEXT_B           ,
   INU_SENSOR__NUM_CONTEXTS
} inu_sensor__sensorContext_e;

typedef enum
{
   INU_SENSORS__OP_MODE_BINNING_E = 1,
   INU_SENSORS__OP_MODE_VERTICAL_BINNING_E = 2,
   INU_SENSORS__OP_MODE_FULL_E = 3,
   INU_SENSORS__OP_MODE_FULL_HD_E = 4,
   INU_SENSORS__OP_MODE_USER_DEF_E = 5,
   INU_SENSORS__OP_MODE_UNKNOWN_E = 6,
   INU_SENSORS__OP_MODE_NUM_MODES = 7,
}inu_sensor__op_mode_e;

typedef enum
{
   INU_SENSOR__TABLE_STEREO = 0,
   INU_SENSOR__TABLE_MONO
}inu_sensor__table_type_e;

typedef enum
{
	INU_SENSOR__PROJECTOR_INST_0 = 0,
	INU_SENSOR__PROJECTOR_INST_1,
	INU_SENSOR__PROJECTOR_INST_2,
	INU_SENSOR__MAX_NUM_PROJECTORS,
}inu_sensor__projector_inst_e;

typedef enum
{
	INU_SENSOR__FOCUS_MODE_MANUAL = 0,
	INU_SENSOR__FOCUS_MODE_INFINITY,
	INU_SENSOR__FOCUS_MODE_MACRO,
	INU_SENSOR__FOCUS_NUM_OF_MODES,
}inu_sensor__focusMode_e;

typedef struct
{
   unsigned int width;
   unsigned int height;
}inu_sensor__resolution_t;


typedef struct
{
   float                      analog;
   float                      digital;
}inu_sensor__gain_t;


typedef struct
{
   inu_sensor__id_e           id;
   inu_sensor__role_e         role;
   inu_sensor__op_mode_e      op_mode;
   inu_sensor__table_type_e   tableType;
   inu_sensor__function_e     function;
   UINT32                     fps;
   inu_sensor__power_hz_e     power_hz;
   UINT32                     defaultExp;
   UINT32                     totalExpTimePerSec;
   UINT32                     expTimePerDutyCycle;
   inu_sensor__trigger_src_e  trigger_src;
   UINT32                     trigger_delay;
   inu_sensor__orientation_e  orientation;
   UINT32                     triggerExtGpio;
   UINT32                     vertical_offset;
   inu_sensor__gain_t         gain;
   UINT32                     i2cNum;
   UINT32                     power_gpio_master;
   UINT32                     power_gpio_slave;
   UINT32                     sensor_clk_src;
   UINT32                     sensor_clk_divider;
   UINT32                     fsin_gpio;
   UINT32                     exp_mode;
   UINT32                     strobeNum;
   UINT32                     groupId;
   UINT32                     sensor_width;
   UINT32                     sensor_height;
   //sensor detection params
   inu_sensor__model_e        model;
   inu_sensor__len_type_e     lenType;
   inu_sensor__projector_inst_e	projector_inst;

}inu_sensor__parameter_list_t;

typedef struct
{
   UINT32                      exposure; //[Us]
   inu_sensor__gain_t          gain;
   UINT32                      fps;
   inu_sensor__sensorContext_e context;
   INT32                      temperature;
}inu_sensor__runtimeCfg_t;

typedef struct
{
   INT32                       dac;
   INT32                       min;
   INT32                       max;
   UINT32                      chipId;
   inu_sensor__sensorContext_e context;
}inu_sensor__getFocusParams_t;

typedef struct
{
   inu_sensor__focusMode_e     mode;
   INT32                       dac;
   inu_sensor__sensorContext_e context;
}inu_sensor__setFocusParams_t;

typedef struct
{
   UINT32                       startX;
   UINT32                       startY;
}inu_sensor__setCropParams_t;

UINT32      inu_sensor__getId(inu_sensorH meH);
ERRG_codeE  inu_sensor__setRuntimeCfg( inu_sensorH meH, inu_sensor__runtimeCfg_t *cfgP );
ERRG_codeE  inu_sensor__getRuntimeCfg( inu_sensorH meH, inu_sensor__runtimeCfg_t *cfgP );
ERRG_codeE  inu_sensor__getFocusParams( inu_sensorH meH, inu_sensor__getFocusParams_t *paramsP );
ERRG_codeE  inu_sensor__setFocusParams( inu_sensorH meH, inu_sensor__setFocusParams_t *paramsP );
ERRG_codeE  inu_sensor__setCropParams( inu_sensorH meH, inu_sensor__setCropParams_t *paramsP );

#endif
