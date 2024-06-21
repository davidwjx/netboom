#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_


#ifdef __cplusplus
extern "C" {
#endif

#define CALIB_SENSOR_OPERATION_MODE                "SensorOperatingMode"
#define CALIB_SENSOR_ROLE                          "Role"
#define CALIB_SENSOR_VERTICAL_OFFSET               "SensorVerticalOffset"
#define CALIB_SENSOR_GAIN_OFFSET                   "SensorGainOffset"
#define CALIB_GAIN_OFFSET_PARTNER                  "GainOffsetPartner"
#define CALIB_VERTICAL_CROP_OFFSET                 "VerticalCropOffset"
#define CALIB_HORIZONTAL_CROP_OFFSET               "HorizontalCropOffset"
#define CALIB_SLU_OUTPUT_VERTICAL_SIZE             "SluOutputVerticalSize"
#define CALIB_SLU_OUTPUT_HORIZONTAL_SIZE           "SluOutputHorizontalSize"
#define CALIB_IB_VERTICAL_LUT_OFFSET               "IbVerticalLutOffset"
#define CALIB_IB_HORIZONTAL_LUT_OFFSET             "IbHorizontalLutOffset"
#define CALIB_IB_OFFSET                            "IbOffset"
#define CALIB_LEFT_IB_OFFSET                       "LeftIbOffset"
#define CALIB_RIGHT_IB_OFFSET                      "RightIbOffset"
#define CALIB_IB_OFFSET                            "IbOffset"
#define CALIB_IB_OFFSET_R                          "IbOffsetR"
#define CALIB_IB_OFFSET_GR                         "IbOffsetGR"
#define CALIB_IB_OFFSET_GB                         "IbOffsetGB"
#define CALIB_IB_OFFSET_B                          "IbOffsetB"
#define CALIB_DSR_VERTICAL_LUT_OFFSET              "DsrVerticalLutOffset"
#define CALIB_DSR_HORIZONTAL_LUT_OFFSET            "DsrHorizontalLutOffset"
#define CALIB_DSR_OUTPUT_VERTICAL_SIZE             "DsrOutputVerticalSize"
#define CALIB_DSR_OUTPUT_HORIZONTAL_SIZE           "DsrOutputHorizontalSize"
#define CALIB_DSR_VERTICAL_BLOCK_SIZE              "DsrVerticalBlockSize"
#define CALIB_DSR_HORIZONTAL_BLOCK_SIZE            "DsrHorizontalBlockSize"
#define CALIB_DSR_LUT_HORIZONTAL_SIZE              "DsrLutHorizontalSize"
#define CALIB_DSR_HORIZONTAL_DELTA_SIZE            "DsrHorizontalDeltaSize"
#define CALIB_DSR_HORIZONTAL_DELTA_FRACTIONAL_BITS "DsrHorizontalDeltaFractionalBits"
#define CALIB_DSR_VERTICAL_DELTA_SIZE              "DsrVerticalDeltaSize"
#define CALIB_DSR_VERTICAL_DELTA_FRACTIONAL_BITS   "DsrVerticalDeltaFractionalBits"
#define CALIB_IB_VERTICAL_BLOCK_SIZE               "IbVerticalBlockSize"
#define CALIB_IB_HORIZONTAL_BLOCK_SIZE             "IbHorizontalBlockSize"
#define CALIB_IB_LUT_HORIZONTAL_SIZE               "IbLutHorizontalSize"
#define CALIB_IB_FIXED_POINT_FRACTIONAL_BITS       "IbFixedPointFractionalBits"

#define CALIB_SENSOR_LEFT  "LeftSensorParameters"
#define CALIB_SENSOR_RIGHT "RightSensorParameters"
#define CALIB_SENSOR_START_NAME  "sensor_"

#define CALIB_MISC         "Misc"

#define CALIB_MODE_BIN          "Binning"
#define CALIB_MODE_VERTICAL_BIN "VerticalBinning"
#define CALIB_MODE_FULL         "Full"
#define CALIB_MODE_VERTIVAL_BINNING "VerticalBinning"
#define CALIB_YML_MAX_STR_SIZE                 32

#define CALIB_ROLE_LEFT         "Left"
#define CALIB_ROLE_RIGHT        "Right"

   typedef enum
   {
      CALIB_MODE_BIN_E = 0,
      CALIB_MODE_FULL_E,
      CALIB_MODE_USER_DEFINE_E,
      CALIB_MODE_VERTICAL_BINNING_E,
      CALIB_MODE_UXGA_E,
      CALIB_MODE_UNKNOWN,
      CALIB_NUM_OP_MODES_E
   } CALIB_sensorOperatingModeE;

   typedef enum
   {
      CALIB_ROLE_LEFT_E = 0,
      CALIB_ROLE_RIGHT_E,
      CALIB_ROLE_UNKNOWN_E
   } CALIB_sensorRoleE;

   typedef enum
   {
      CALIB_LUT_MODE_BIN_E = 0,
      CALIB_LUT_MODE_FULL_E,
      CALIB_LUT_MODE_VERTICAL_BINNING_E,
      CALIB_NUM_LUT_MODES_E
   } CALIB_sensorLutModeE;

   typedef enum
   {
      CALIB_LUT_TEMPER_MODE_0_E = 0,
      CALIB_NUM_TEMPER_LUT_MODES_E
   } CALIB_sensorTemperModeE;

   typedef enum
   {
      CALIB_SECTION_SENSOR_0_PARAMETERS_E = 0,
      CALIB_SECTION_SENSOR_1_PARAMETERS_E,
      CALIB_SECTION_SENSOR_2_PARAMETERS_E,
      CALIB_SECTION_SENSOR_3_PARAMETERS_E,
      CALIB_SECTION_SENSOR_4_PARAMETERS_E,
      CALIB_SECTION_SENSOR_5_PARAMETERS_E,
      CALIB_SECTION_SENSOR_6_PARAMETERS_E,
      CALIB_SECTION_SENSOR_7_PARAMETERS_E,
      CALIB_SECTION_MISC_E,
      CALIB_SECTION_UNKNOWN_E
   } CALIB_sectionNumE;

   typedef enum
   {
      CALIB_YML_SECTION_SENSOR_0_E,
      CALIB_YML_SECTION_SENSOR_1_E,
      CALIB_YML_SECTION_SENSOR_2_E,
      CALIB_YML_SECTION_SENSOR_3_E,
      CALIB_YML_SECTION_SENSOR_4_E,
      CALIB_YML_SECTION_SENSOR_5_E,
      CALIB_YML_SECTION_SENSOR_6_E,
      CALIB_YML_SECTION_SENSOR_7_E,
      CALIB_YML_SECTION_BASELINE_0_E,
      CALIB_YML_SECTION_BASELINE_1_E,
      CALIB_YML_SECTION_BASELINE_2_E,
      CALIB_YML_SECTION_BASELINE_3_E,
      CALIB_YML_MAX_SECTION_E
    } CALIB_ymlSectionE;

   typedef enum
   {
       CALIB_YML_SENSOR_HOR_PIXEL_SIZE_E =0,
       CALIB_YML_SENSOR_HOR_RES_E,
       CALIB_YML_SENSOR_VER_PIXEL_SIZE_E,
       CALIB_YML_SENSOR_VER_RES_E,
       CALIB_YML_SENSOR_MAX_PARAMS_E

   }CALIB_yml_sensorParamsE;

   typedef enum
   {
       CALIB_YML_T_E =0,
       CALIB_YML_OM_E,
       CALIB_YML_ALPHAC_E,
       CALIB_YML_CC_E,
       CALIB_YML_FC_E,
       CALIB_YML_KC_E,
       CALIB_YML_MODEL_E,
       CALIB_YML_HOR_PIX_SIZE_E,
       CALIB_YML_HOR_RES_E,
       CALIB_YML_VER_PIX_SIZE_E,
       CALIB_YML_VER_RES_E,
       CALIB_YML_NUMPARAMS_E
   }CALIB_yml_paramsE;

   typedef enum
   {
       CALIB_YML_REAL_E =0,
       CALIB_YML_VIRTUAL_E,
       CALIB_YML_MAX_REALVIRT_E
   }CALIB_yml_realVirtE;

   typedef enum
   {
       CALIB_YML_IMAGERS_E =0,
       CALIB_YML_VAL_E,
       CALIB_YML_BASELINE_NUMPARAMS_E
   }CALIB_yml_baseline_paramsE;

   typedef enum
   {
       CALIB_YML_type_float_E =0,
       CALIB_YML_type_str_E
    }CALIB_yml_params_typesE;

   typedef struct
   {
     void                       *paramList;
     unsigned int               paramListSize;
    }Calib_YmlParamsT;

    typedef struct
    {
      char                       *string;
      CALIB_yml_params_typesE    type;
     }Calib_YmlParmsDscrsT;

   typedef struct
   {
      UINT8                       sectionNum;
      UINT8                       calibMode;
      Calib_YmlParamsT            calibYmlParams[CALIB_YML_MAX_REALVIRT_E][CALIB_YML_NUMPARAMS_E];
    }Calib_YmlSensorParamsT;

   typedef struct
   {
      UINT8                       sectionNum;
      UINT8                       calibMode;
      Calib_YmlParamsT            calibYmlBaselineParams[CALIB_YML_BASELINE_NUMPARAMS_E];
    }Calib_YmlBLParamsT;

   typedef union
   {
       Calib_YmlSensorParamsT     sensorParams;
       Calib_YmlBLParamsT         baselineParams;
   }Calib_YmlSectionParamsT;

   typedef struct
   {
      CALIB_sectionNumE          sectionNum;
      CALIB_sensorOperatingModeE SensorOperatingMode;
      CALIB_sensorRoleE          SensorRole;
      UINT32                     sensorVerticalOffset;
      INT32                      sensorGainOffset;
      UINT32                     gainOffsetPartner;
      UINT32                     verticalCropOffset;
      UINT32                     horizontalCropOffset;
      UINT32                     sluOutputVerticalSize;
      UINT32                     sluOutputHorizontalSize;
      UINT32                     ibVerticalLutOffset;
      UINT32                     ibHorizontalLutOffset;
      UINT32                     ibOffset;
      UINT32                     ibOffsetR;
      UINT32                     ibOffsetGR;
      UINT32                     ibOffsetGB;
      UINT32                     ibOffsetB;
      UINT32                     dsrVerticalLutOffset;
      UINT32                     dsrHorizontalLutOffset;
      UINT32                     dsrOutputVerticalSize;
      UINT32                     dsrOutputHorizontalSize;
      UINT32                     dsrVerticalBlockSize;
      UINT32                     dsrHorizontalBlockSize;
      UINT32                     dsrLutHorizontalSize;
      UINT32                     dsrHorizontalDeltaSize;
      UINT32                     dsrHorizontalDeltaFractionalBits;
      UINT32                     dsrVerticalDeltaSize;
      UINT32                     dsrVerticalDeltaFractionalBits;
      UINT32                     ibVerticalBlockSize;
      UINT32                     ibHorizontalBlockSize;
      UINT32                     ibLutHorizontalSize;
      UINT32                     ibFixedPointFractionalBits;
   } CALIB_sectionDataT;

   typedef struct
   {
      float fc;
      float d2dFactor;
   }CALIB_ymlDataT;

   typedef struct
   {
       UINT32 calib;
       UINT32 sensorId;
   }
   CALIB_iauTosensor;

   ERRG_codeE CALIBG_getCalibData(char* path[], CALIB_sectionDataT** sectionsDataTbl, int *NumOfSections);
   void CALIBG_freeCalibArr(CALIB_sectionDataT* sectionsDataTbl);
   char *CALIB_getSensorModeStr(CALIB_sensorLutModeE calibMode);
   void CALIBG_ymlGetSectionsDB(CALIB_sensorLutModeE LutMode,CALIB_ymlSectionE section,Calib_YmlSectionParamsT **sectionStart);
   BOOLEAN CALIBG_ymlGetSectionsBLValue(CALIB_sensorLutModeE LutMode,UINT32 sensor0,UINT32 sensor1,float *value);
   BOOLEAN CALIBG_ymlGetSectionFcValue(CALIB_sensorLutModeE LutMode,CALIB_ymlSectionE section,float *fx,float *fy);
   BOOLEAN CALIBG_ymlGetSectionCcValue(CALIB_sensorLutModeE LutMode,CALIB_ymlSectionE section,float *cx,float *cy);
#ifdef __cplusplus
}
#endif

#endif 

