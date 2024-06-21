    /******************************************************************************\
    |* Copyright (c) 2021 by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")       *|
    |* All Rights Reserved.                                                       *|
    |*                                                                            *|
    |* The material in this file is confidential and contains trade secrets of    *|
    |* of VeriSilicon.  This is proprietary information owned or licensed by      *|
    |* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
    |* transmitted, or used in any way for any purpose, without the express       *|
    |* written permission of VeriSilicon.                                         *|
    |*                                                                            *|
    \******************************************************************************/

    #ifndef _CAMERA_DEVICE_MODULE_IDS_EXT_
    #define _CAMERA_DEVICE_MODULE_IDS_EXT_

    #include <unordered_map>
    #include <string>

    #define ISP_AE_CFG_GET                            "isp_ae_cfg_get"
    #define ISP_AE_CFG_SET                            "isp_ae_cfg_set"
    #define ISP_AE_ECM_GET                            "isp_ae_ecm_get"
    #define ISP_AE_ECM_SET                            "isp_ae_ecm_set"
    #define ISP_AE_ENABLE_GET                         "isp_ae_enable_get"
    #define ISP_AE_ENABLE_SET                         "isp_ae_enable_set"
    #define ISP_AE_RESET                              "isp_ae_reset"
    #define ISP_AE_SETPOINT_SET                       "isp_ae_setpoint_set"
    #define ISP_AE_DAMPOVER_SET                       "isp_ae_dampover_set"
    #define ISP_AE_DAMPUNDER_SET                      "isp_ae_dampunder_set"
    #define ISP_AE_TOLERANCE_SET                      "isp_ae_tolerance_set"
    #define ISP_AE_GENERATION_SET                     "isp_ae_generation_set"

    #define ISP_EC_STATUS_SET                         "isp_ec_status_set"
    #define ISP_EC_EXPOSURETIME_SET                   "isp_ec_exposuretime_set"
    #define ISP_EC_GAIN_SET                           "isp_ec_gain_set"

    #define ISP_AF_AVI_GET                            "isp_af_avi_get"
    #define ISP_AF_CFG_GET                            "isp_af_cfg_get"
    #define ISP_AF_CFG_SET                            "isp_af_cfg_set"
    #define ISP_AF_ENABLE_GET                         "isp_af_enable_get"
    #define ISP_AF_ENABLE_SET                         "isp_af_enable_set"

    #define ISP_AVS_CFG_GET                           "isp_avs_cfg_get"
    #define ISP_AVS_CFG_SET                           "isp_avs_cfg_set"
    #define ISP_AVS_ENABLE_GET                        "isp_avs_enable_get"
    #define ISP_AVS_ENABLE_SET                        "isp_avs_enable_set"
    #define ISP_AVS_STATUS_GET                        "isp_avs_status_get"

    #define ISP_AWB_CFG_GET                           "isp_awb_cfg_get"
    #define ISP_AWB_CFG_SET                           "isp_awb_cfg_set"
    #define ISP_AWB_ENABLE_GET                        "isp_awb_enable_get"
    #define ISP_AWB_ENABLE_SET                        "isp_awb_enable_set"
    #define ISP_AWB_ILLUMPRO_GET                      "isp_awb_illumpro_get"
    #define ISP_AWB_ILLUMPRO_SET                      "isp_awb_illumpro_set"
    #define ISP_AWB_RESET                             "isp_awb_reset"
    #define ISP_AWB_STATUS_GET                        "isp_awb_status_get"
    #define ISP_AWB_MEAS_WIN_SET                      "isp_awb_meas_win_set"
    #define ISP_AWB_MODE_SET                          "isp_awb_mode_set"
    #define ISP_AWB_DAMP_SET                          "isp_awb_damp_set"

    #define ISP_BLS_CFG_GET                           "isp_bls_cfg_get"
    #define ISP_BLS_CFG_SET                           "isp_bls_cfg_set"

    #define ISP_CAC_ENABLE_GET                        "isp_cac_enable_get"
    #define ISP_CAC_ENABLE_SET                        "isp_cac_enable_set"
    #define ISP_CAC_STATUS                            "isp_cac_status"

    #define ISP_CNR_CFG_GET                           "isp_cnr_cfg_get"
    #define ISP_CNR_CFG_SET                           "isp_cnr_cfg_set"
    #define ISP_CNR_ENABLE_GET                        "isp_cnr_enable_get"
    #define ISP_CNR_ENABLE_SET                        "isp_cnr_enable_set"

    #define ISP_CPROC_CFG_GET                         "isp_cproc_cfg_get"
    #define ISP_CPROC_CFG_SET                         "isp_cproc_cfg_set"
    #define ISP_CPROC_ENABLE_GET                      "isp_cproc_enable_get"
    #define ISP_CPROC_ENABLE_SET                      "isp_cproc_enable_set"
    #define ISP_CPROC_LUMAIN_SET                      "isp_cproc_lumain_set"
    #define ISP_CPROC_LUMAOUT_SET                     "isp_cproc_lumaout_set"
    #define ISP_CPROC_CHROMAOUT_SET                   "isp_cproc_chromaout_set"
    #define ISP_CPROC_BRIGHTNESS_SET                  "isp_cproc_brightness_set"
    #define ISP_CPROC_CONTRAST_SET                    "isp_cproc_contrast_set"
    #define ISP_CPROC_SATURATION_SET                  "isp_cproc_saturation_set"
    #define ISP_CPROC_HUE_SET                         "isp_cproc_hue_set"

    #define ISP_DEMOSAIC_CFG_GET                      "isp_demosaic_cfg_get"
    #define ISP_DEMOSAIC_CFG_SET                      "isp_demosaic_cfg_set"
    #define ISP_DEMOSAIC_ENABLE_GET                   "isp_demosaic_enable_get"
    #define ISP_DEMOSAIC_ENABLE_SET                   "isp_demosaic_enable_set"
    #define ISP_DEMOSAIC_RESET                        "isp_demosaic_reset"
    #define ISP_DEMOSAIC_THRESHOLD_SET                "isp_demosaic_cfg_set"
    #define ISP_DEMOSAIC_GLOBAL_INTERPTHRESHOLD_SET   "isp_demosaic_global_interpolationthreshold_set"
    #define ISP_DEMOSAIC_GLOBAL_DENOISESTRENGTH_SET   "isp_demosaic_global_denoisestrength_set"
    #define ISP_DEMOSAIC_GLOBAL_SHARPENSIZE_SET       "isp_demosaic_global_sharpensize_set"
    #define ISP_DEMOSAIC_DEMOIRE_ENABLE_SET           "isp_demosaic_demoire_enable_set"
    #define ISP_DEMOSAIC_DEMOIRE_AREATHRESHOLD_SET    "isp_demosaic_demoire_areathreshold_set"
    #define ISP_DEMOSAIC_DEMOIRE_SATURATION_SET       "isp_demosaic_demoire_saturation_set"
    #define ISP_DEMOSAIC_DEMOIRE_THRESHOLD_SET        "isp_demosaic_demoire_threshold_set"
    #define ISP_DEMOSAIC_DEMOIRE_EDGE_SET             "isp_demosaic_demoire_edge_set"
    #define ISP_DEMOSAIC_DEPURPLE_ENABLE_SET          "isp_demosaic_depurple_enable_set"
    #define ISP_DEMOSAIC_DEPURPLE_CBCRMODE_SET        "isp_demosaic_depurple_cbcrmode_set"
    #define ISP_DEMOSAIC_DEPURPLE_SATURATION_SET      "isp_demosaic_depurple_saturation_set"
    #define ISP_DEMOSAIC_DEPURPLE_THRESHOLD_SET       "isp_demosaic_depurple_threshold_set"
    #define ISP_DEMOSAIC_SHARPEN_ENABLE_SET           "isp_demosaic_sharpen_enable_set"
    #define ISP_DEMOSAIC_SHARPEN_CLIP_SET             "isp_demosaic_sharpen_clip_set"
    #define ISP_DEMOSAIC_SHARPEN_FACTOR_SET           "isp_demosaic_sharpen_factor_set"
    #define ISP_DEMOSAIC_SHARPEN_THRESHOLD_SET        "isp_demosaic_sharpen_threshold_set"
    #define ISP_DEMOSAIC_SHARPEN_RATIO_SET            "isp_demosaic_sharpen_ratio_set"
    #define ISP_DEMOSAIC_SHARPEN_FILTER_SET           "isp_demosaic_sharpen_filter_set"
    #define ISP_DEMOSAIC_SHARPENLINE_ENABLE_SET       "isp_demosaic_sharpenline_enable_set"
    #define ISP_DEMOSAIC_SHARPENLINE_RATIO_SET        "isp_demosaic_sharpenline_ratio_set"
    #define ISP_DEMOSAIC_SHARPENLINE_SHIFT1_SET       "isp_demosaic_sharpenline_shift1_set"
    #define ISP_DEMOSAIC_SHARPENLINE_SHIFT2_SET       "isp_demosaic_sharpenline_shift2_set"
    #define ISP_DEMOSAIC_SHARPENLINE_STRENGTH_SET     "isp_demosaic_sharpenline_strength_set"
    #define ISP_DEMOSAIC_SHARPENLINE_T1_SET           "isp_demosaic_sharpenline_t1_set"
    #define ISP_DEMOSAIC_SKIN_ENABLE_SET              "isp_demosaic_skin_enable_set"
    #define ISP_DEMOSAIC_SKIN_CB_THRESHOLD_SET        "isp_demosaic_skin_cb_threshold_set"
    #define ISP_DEMOSAIC_SKIN_CR_THRESHOLD_SET        "isp_demosaic_skin_cr_threshold_set"
    #define ISP_DEMOSAIC_SKIN_Y_THRESHOLD_SET         "isp_demosaic_skin_y_threshold_set"

    #define ISP_DEVICE_CALIBRATIONSAVE                "isp_device_calibrationsave"
    #define ISP_DEVICE_CALIBRATIONLOADANDINIT         "isp_device_calibrationloadandinit"
    #define ISP_DEVICE_CALIBRATIONLOAD                "isp_device_calibrationload"
    #define ISP_DEVICE_INITENGINEOPERATION            "isp_device_initengineoperation"
    #define ISP_DEVICE_RESOLUTION_SET_SENSOR_ISP      "isp_device_resolution_set_sensor_isp"
    #define ISP_DEVICE_CAMERA_CONNECT                 "isp_device_camera_connect"

    #define ISP_DEVICE_CAPTUREDMA                     "isp_device_capturedma"
    #define ISP_DEVICE_CAPTURESENSOR                  "isp_device_capturesensor"
    #define ISP_DEVICE_INPUTINFO                      "isp_device_inputinfo"
    #define ISP_DEVICE_INPUTSWITCH                    "isp_device_inputswitch"
    #define ISP_DEVICE_PREVIEW                        "isp_device_preview"
    #define ISP_DEVICE_BITSTREAMID                    "isp_device_bitstreamid"
    #define ISP_DEVICE_CAMERIC_ID                     "isp_device_cameric_id"
    #define ISP_DEVICE_INPUT_CONNECT                  "isp_device_input_connect"
    #define ISP_DEVICE_INPUT_DISCONNECT               "isp_device_input_disconnect"
    #define ISP_DEVICE_RESET                          "isp_device_reset"
    #define ISP_DEVICE_RESOLUTION_GET                 "isp_device_resolution_get"
    #define ISP_DEVICE_RESOLUTION_SET                 "isp_device_resolution_set"
    #define ISP_DEVICE_STREAMING_START                "isp_device_streaming_start"
    #define ISP_DEVICE_STREAMING_STOP                 "isp_device_streaming_stop"
    #define ISP_DEVICE_CAMERA_DISCONNECT              "isp_device_camera_disconnect"
    #define ISP_DEVICE_CAMERA_RESET                   "isp_device_camera_reset"
    #define ISP_DEVICE_PREVIEW_START                  "isp_device_preview_start"
    #define ISP_DEVICE_PREVIEW_STOP                   "isp_device_preview_stop"
    #define ISP_DEVICE_GET_HALHANDLE                  "isp_device_get_halhandle"
    #define ISP_DEVICE_GET_METADATA                   "isp_device_get_metadata"
    #define ISP_DEVICE_OUTPUT_IMAGE_FORMAT            "isp_device_output_image_format"
    #define ISP_DEVICE_STANDBY_ENABLE_SET             "isp_device_standby_enable_set"

    #define ISP_2DNR_CFG_GET                          "isp_2dnr_cfg_get"
    #define ISP_2DNR_CFG_SET                          "isp_2dnr_cfg_set"
    #define ISP_2DNR_ENABLE_GET                       "isp_2dnr_enable_get"
    #define ISP_2DNR_ENABLE_SET                       "isp_2dnr_enable_set"
    #define ISP_2DNR_RESET                            "isp_2dnr_reset"
    #define ISP_2DNR_STATUS_GET                       "isp_2dnr_status_get"
    #define ISP_2DNR_TABLE_GET                        "isp_2dnr_table_get"
    #define ISP_2DNR_TABLE_SET                        "isp_2dnr_table_set"
    #define ISP_2DNR_GENERATION_SET                   "isp_2dnr_generation_set"
    #define ISP_2DNR_AUTOMODE_SET                     "isp_2dnr_automode_set"
    #define ISP_2DNR_AUTOLEVEL_SET                    "isp_2dnr_autolevel_set"
    #define ISP_2DNR_DENOISEPRESTRENGTH_SET           "isp_2dnr_denoise_prestrength_set"
    #define ISP_2DNR_DENOISESTRENGTH_SET              "isp_2dnr_denoise_strength_set"
    #define ISP_2DNR_SIGMA_SET                        "isp_2dnr_sigma_set"
    #define ISP_2DNR_BLENDOPACITYSTATIC_SET           "isp_2dnr_blendopacity_static_set"
    #define ISP_2DNR_BLENDOPACITYMOVING_SET           "isp_2dnr_blendopacity_moving_set"

    #define ISP_3DNR_CFG_GET                          "isp_3dnr_cfg_get"
    #define ISP_3DNR_CFG_SET                          "isp_3dnr_cfg_set"
    #define ISP_3DNR_ENABLE_GET                       "isp_3dnr_enable_get"
    #define ISP_3DNR_ENABLE_SET                       "isp_3dnr_enable_set"
    #define ISP_3DNR_RESET                            "isp_3dnr_reset"
    #define ISP_3DNR_STATUS_GET                       "isp_3dnr_status_get"
    #define ISP_3DNR_TABLE_GET                        "isp_3dnr_table_get"
    #define ISP_3DNR_TABLE_SET                        "isp_3dnr_table_set"
    #define ISP_3DNR_GENERATION_SET                   "isp_3dnr_generation_set"
    #define ISP_3DNR_AUTOMODE_SET                     "isp_3dnr_automode_set"
    #define ISP_3DNR_AUTOLEVEL_SET                    "isp_3dnr_autolevel_set"
    #define ISP_3DNR_DELTAFACTOR_SET                  "isp_3dnr_deltafactor_set"
    #define ISP_3DNR_MOTIONFACTOR_SET                 "isp_3dnr_motionfactor_set"
    #define ISP_3DNR_STRENGTH_SET                     "isp_3dnr_strength_set"
    #define ISP_3DNR_TNR_ENABLE_SET                   "isp_3dnr_tnr_enable_set"
    #define ISP_3DNR_TNR_FILTERLENGTH_SET             "isp_3dnr_tnr_filterlength_set"
    #define ISP_3DNR_TNR_FILTERLENGTH2_SET            "isp_3dnr_tnr_filterlength2_set"
    #define ISP_3DNR_TNR_MOTIONSLOPE_SET              "isp_3dnr_tnr_motionslope_set"
    #define ISP_3DNR_TNR_NOISELEVEL_SET               "isp_3dnr_tnr_noiselevel_set"
    #define ISP_3DNR_TNR_SADWEIGHT_SET                "isp_3dnr_tnr_sadweight_set"

    #define ISP_DPCC_ENABLE_GET                       "isp_dpcc_enable_get"
    #define ISP_DPCC_ENABLE_SET                       "isp_dpcc_enable_set"

    #define ISP_DPF_CFG_GET                           "isp_dpf_cfg_get"
    #define ISP_DPF_CFG_SET                           "isp_dpf_cfg_set"
    #define ISP_DPF_ENABLE_GET                        "isp_dpf_enable_get"
    #define ISP_DPF_ENABLE_SET                        "isp_dpf_enable_set"

    #define ISP_EE_CFG_GET                            "isp_ee_cfg_get"
    #define ISP_EE_CFG_SET                            "isp_ee_cfg_set"
    #define ISP_EE_ENABLE_GET                         "isp_ee_enable_get"
    #define ISP_EE_ENABLE_SET                         "isp_ee_enable_set"
    #define ISP_EE_RESET                              "isp_ee_reset"
    #define ISP_EE_STATUS_GET                         "isp_ee_status_get"
    #define ISP_EE_TABLE_GET                          "isp_ee_table_get"
    #define ISP_EE_TABLE_SET                          "isp_ee_table_set"
    #define ISP_EE_AUTOMODE_SET                       "isp_ee_automode_set"
    #define ISP_EE_STRENGTH_SET                       "isp_ee_strength_set"
    #define ISP_EE_YUPGAIN_SET                        "isp_ee_yupgain_set"
    #define ISP_EE_YDOWNGAIN_SET                      "isp_ee_ydowngain_set"
    #define ISP_EE_UVGAIN_SET                         "isp_ee_uvgain_set"
    #define ISP_EE_EDGEGAIN_SET                       "isp_ee_edgegain_set"

    #define ISP_FILESYSTEM_REMOVE                     "isp_filesystem_remove"
    #define ISP_FILESYSTEM_LIST                       "isp_filesystem_list"

    #define ISP_FILTER_CFG_GET                        "isp_filter_cfg_get"
    #define ISP_FILTER_CFG_SET                        "isp_filter_cfg_set"
    #define ISP_FILTER_ENABLE_GET                     "isp_filter_enable_get"
    #define ISP_FILTER_ENABLE_SET                     "isp_filter_enable_set"
    #define ISP_FILTER_STATUS_GET                     "isp_filter_status_get"
    #define ISP_FILTER_TABLE_GET                      "isp_filter_table_get"
    #define ISP_FILTER_TABLE_SET                      "isp_filter_table_set"

    #define ISP_GC_CFG_GET                            "isp_gc_cfg_get"
    #define ISP_GC_CFG_SET                            "isp_gc_cfg_set"
    #define ISP_GC_ENABLE_GET                         "isp_gc_enable_get"
    #define ISP_GC_ENABLE_SET                         "isp_gc_enable_set"
    #define ISP_GC_USERCURVE_SET                      "isp_gc_usercurve_set"
    #define ISP_GC_STANDCURVE_SET                     "isp_gc_standurve_set"

    #define ISP_HDR_CFG_GET                           "isp_hdr_cfg_get"
    #define ISP_HDR_CFG_SET                           "isp_hdr_cfg_set"
    #define ISP_HDR_ENABLE_GET                        "isp_hdr_enable_get"
    #define ISP_HDR_ENABLE_SET                        "isp_hdr_enable_set"
    #define ISP_HDR_RESET                             "isp_hdr_reset"
    #define ISP_HDR_STATUS                            "isp_hdr_status"
    #define ISP_HDR_GENERATION_SET                    "isp_hdr_generation_set"
    #define ISP_HDR_EXPOSURERATIO_SET                 "isp_hdr_exposureratio_set"
    #define ISP_HDR_EXTENSIONBIT_SET                  "isp_hdr_extensionbit_set"
    #define ISP_HDR_TRANSRANGE_SET                    "isp_hdr_transrange_set"

    #define ISP_IE_CFG_GET                            "isp_ie_cfg_get"
    #define ISP_IE_CFG_SET                            "isp_ie_cfg_set"
    #define ISP_IE_ENABLE_GET                         "isp_ie_enable_get"
    #define ISP_IE_ENABLE_SET                         "isp_ie_enable_set"

    #define ISP_LSC_CFG_GET                           "isp_lsc_cfg_get"
    #define ISP_LSC_CFG_SET                           "isp_lsc_cfg_set"
    #define ISP_LSC_ENABLE_GET                        "isp_lsc_enable_get"
    #define ISP_LSC_ENABLE_SET                        "isp_lsc_enable_set"
    #define ISP_LSC_STATUS_GET                        "isp_lsc_status_get"

    #define ISP_REG_DESCRIPTION                       "isp_reg_description"
    #define ISP_REG_GET                               "isp_reg_get"
    #define ISP_REG_SET                               "isp_reg_set"

    #define ISP_SENSOR_CAPS                           "isp_sensor_caps"
    #define ISP_SENSOR_CFG_GET                        "isp_sensor_cfg_get"
    #define ISP_SENSOR_CFG_SET                        "isp_sensor_cfg_set"
    #define ISP_SENSOR_NAME_GET                       "isp_sensor_name_get"
    #define ISP_SENSOR_REVISION_GET                   "isp_sensor_revision_get"
    #define ISP_SENSOR_OPEN                           "isp_sensor_open"
    #define ISP_SENSOR_IS_CONNECTED                   "isp_sensor_is_connected"
    #define ISP_SENSOR_IS_TEST_PATTERN                "isp_sensor_is_test_pattern"
    #define ISP_SENSOR_DRV_CHANGE                     "isp_sensor_drv_change"
    #define ISP_SENSOR_DRV_LIST                       "isp_sensor_drv_list"
    #define ISP_SENSOR_INFO                           "isp_sensor_info"
    #define ISP_SENSOR_RESOLUTION_LIST_GET            "isp_sensor_resolution_list_get"
    #define ISP_SENSOR_GET_RESOLUTION_NAME            "isp_sensor_get_resolution_name"
    #define ISP_SENSOR_REG_DESCRIPTION                "isp_sensor_reg_description"
    #define ISP_SENSOR_REG_DUMP2FILE                  "isp_sensor_reg_dump2file"
    #define ISP_SENSOR_REG_GET                        "isp_sensor_reg_get"
    #define ISP_SENSOR_REG_SET                        "isp_sensor_reg_set"
    #define ISP_SENSOR_REG_TABLE                      "isp_sensor_reg_table"
    #define ISP_SENSOR_RESOLUTION_SET                 "isp_sensor_resolution_set"
    #define ISP_SENSOR_TESTPATTERN_EN_SET             "isp_sensor_testpattern_en_set"
    #define ISP_SENSOR_CALIB_INSTALL                  "isp_sensor_calib_install"
    #define ISP_SENSOR_CALIB_UNINSTALL                "isp_sensor_calib_uninstall"
    #define ISP_SENSOR_RESOLUTION_SUP_GET             "isp_sensor_resolution_sup_get"
    #define ISP_SENSOR_FPS_GET                        "isp_sensor_fps_get"
    #define ISP_SENSOR_FPS_SET                        "isp_sensor_fps_set"
    #define ISP_SENSOR_QUERY                          "isp_sensor_query"
    #define ISP_SENSOR_MODE_GET                       "isp_sensor_mode_get"
    #define ISP_SENSOR_MODE_SET                       "isp_sensor_mode_set"
    #define ISP_SENSOR_PRE_LOAD_LIB                   "isp_sensor_pre_load_lib"
    #define ISP_SENSOR_RES_W_GET                      "isp_sensor_res_w_get"
    #define ISP_SENSOR_RES_H_GET                      "isp_sensor_res_h_get"
    #define ISP_SENSOR_MODE_LOCK                      "isp_sensor_mode_lock"
    #define ISP_SENSOR_STATUS                         "isp_sensor_status"

    #define ISP_SIMP_CFG_GET                          "isp_simp_cfg_get"
    #define ISP_SIMP_CFG_SET                          "isp_simp_cfg_set"
    #define ISP_SIMP_ENABLE_GET                       "isp_simp_enable_get"
    #define ISP_SIMP_ENABLE_SET                       "isp_simp_enable_set"

    #define ISP_WB_CFG_GET                            "isp_wb_cfg_get"
    #define ISP_WB_CFG_SET                            "isp_wb_cfg_set"
    #define ISP_WB_GAIN_SET                           "isp_wb_gain_set"
    #define ISP_WB_CCOFFSET_SET                       "isp_wb_ccoffset_set"
    #define ISP_WB_CCMATRIX_SET                       "isp_wb_ccmatrix_set"

    #define ISP_WDR_CFG_GET                           "isp_wdr_cfg_get"
    #define ISP_WDR_CFG_SET                           "isp_wdr_cfg_set"
    #define ISP_WDR_ENABLE_GET                        "isp_wdr_enable_get"
    #define ISP_WDR_ENABLE_SET                        "isp_wdr_enable_set"
    #define ISP_WDR_RESET                             "isp_wdr_reset"
    #define ISP_WDR_STATUS_GET                        "isp_wdr_status_get"
    #define ISP_WDR_TABLE_GET                         "isp_wdr_table_get"
    #define ISP_WDR_TABLE_SET                         "isp_wdr_table_set"
    #define ISP_WDR_GENERATION_SET                    "isp_wdr_generation_set"
    #define ISP_WDR_GENERATION_GET                    "isp_wdr_generation_get"
    #define ISP_WDR_STRENGTH_SET                      "isp_wdr_strength_set"
    #define ISP_WDR_STRENGTH_RANGE_SET                "isp_wdr_strength_range_set"
    #define ISP_WDR_GSTRENGTH_SET                     "isp_wdr_gstrength_set"
    #define ISP_WDR_AUTO_MODE_SET                     "isp_wdr_auto_mode_set"
    #define ISP_WDR_AUTO_LEVEL_SET                    "isp_wdr_auto_level_set"
    #define ISP_WDR_GAIN_MAX_SET                      "isp_wdr_gain_max_set"
    #define ISP_WDR_FLAT_STRENGTH_SET                 "isp_wdr_flat_strength_set"
    #define ISP_WDR_FLAT_THRESHOLD_SET                "isp_wdr_flat_threshold_set"
    #define ISP_WDR_CONTRAST_SET                      "isp_wdr_contrast_set"

    #define ISP_PIPELINE_WARM_UP                      "isp_pipeline_warm_up"
    #define ISP_PIPELINE_SMP_MODE                     "isp_pipeline_smp_mode"
    #define ISP_PIPELINE_CFG_DWE                      "isp_pipeline_cfg_dwe"
    #define ISP_PIPELINE_CFG_TESTPATTEN_EN            "isp_pipeline_cfg_testpatten_en"
    #define ISP_PIPELINE_RESOLUTION_IS_OUT            "isp_pipeline_resolution_is_out"
    #define ISP_PIPELINE_RESOLUTION_MP_OUT            "isp_pipeline_resolution_mp_out"
    #define ISP_PIPELINE_MP_OUT_IMAGE_FORMAT          "isp_pipeline_mp_out_image_format"
    #define ISP_PIPELINE_QUERY                        "isp_pipeline_query"
    #define ISP_PIPELINE_CFG_STATUS                   "isp_pipeline_cfg_status"

    #define ISP_GE_CFG_SET                            "isp_ge_cfg_set"
    #define ISP_GE_CFG_GET                            "isp_ge_cfg_get"
    #define ISP_GE_ENABLE_SET                         "isp_ge_enable_set"
    #define ISP_GE_ENABLE_GET                         "isp_ge_enable_get"

    #define ISP_RGBGAMMA_CFG_SET                      "isp_rgbgamma_cfg_set"
    #define ISP_RGBGAMMA_CFG_GET                      "isp_rgbgamma_cfg_get"
    #define ISP_RGBGAMMA_ENABLE_SET                   "isp_rgbgamma_enable_set"
    #define ISP_RGBGAMMA_ENABLE_GET                   "isp_rgbgamma_enable_get"

    #define ISP_DCI_CFG_SET                           "isp_dci_cfg_set"
    #define ISP_DCI_CFG_GET                           "isp_dci_cfg_get"
    #define ISP_DCI_ENABLE_SET                        "isp_dci_enable_set"
    #define ISP_DCI_ENABLE_GET                        "isp_dci_enable_get"
    #define ISP_DCI_STATUS_GET                        "isp_dci_status_get"
    #define ISP_DCI_TABLE_SET                         "isp_dci_table_set"
    #define ISP_DCI_TABLE_GET                         "isp_dci_table_get"
    #define ISP_DCI_AUTOMODE_SET                      "isp_dci_automode_set"
    #define ISP_DCI_MODE_SET                          "isp_dci_mode_set"
    #define ISP_DCI_EXPONENTHIGH_SET                  "isp_dci_exponenthigh_set"
    #define ISP_DCI_EXPONENTLOW_SET                   "isp_dci_exponentlow_set"
    #define ISP_DCI_EXPONENTMID_SET                   "isp_dci_exponentmid_set"
    #define ISP_DCI_HISTSCALE_SET                     "isp_dci_histscale_set"
    #define ISP_DCI_HISTSTEP_SET                      "isp_dci_histstep_set"
    #define ISP_DCI_HISTBASE_SET                      "isp_dci_histbase_set"
    #define ISP_DCI_HISTSTRENGTH_SET                  "isp_dci_histstrength_set"
    #define ISP_DCI_HISTDAMP_SET                      "isp_dci_histdamp_set"
    #define ISP_DCI_POINTSTART_SET                    "isp_dci_pointstart_set"
    #define ISP_DCI_POINTEND_SET                      "isp_dci_pointend_set"
    #define ISP_DCI_POINTINFLECTION_SET               "isp_dci_pointinflection_set"
    #define ISP_DCI_POINTINFLECTION2_SET              "isp_dci_pointinflection2_set"

    #define ISP_AHDR_CFG_SET                          "isp_ahdr_cfg_set"
    #define ISP_AHDR_CFG_GET                          "isp_ahdr_cfg_get"
    #define ISP_AHDR_ENABLE_SET                       "isp_ahdr_enable_set"
    #define ISP_AHDR_ENABLE_GET                       "isp_ahdr_enable_get"
    #define ISP_AHDR_TABLE_SET                        "isp_ahdr_table_set"
    #define ISP_AHDR_TABLE_GET                        "isp_ahdr_table_get"
    #define ISP_AHDR_RESET                            "isp_ahdr_reset"
    #define ISP_AHDR_AUTOMODE_SET                     "isp_ahdr_automode_set"
    #define ISP_AHDR_C_SET                            "isp_ahdr_c_set"
    #define ISP_AHDR_C2_SET                           "isp_ahdr_c2_set"
    #define ISP_AHDR_CEIL_SET                         "isp_ahdr_ceil_set"
    #define ISP_AHDR_OBJECTFECTOR_SET                 "isp_ahdr_objectfector_set"
    #define ISP_AHDR_SETPOINT2_SET                    "isp_ahdr_setpoint2_set"
    #define ISP_AHDR_TOLERANCE_SET                    "isp_ahdr_tolerance_set"
    #define ISP_AHDR_RATIOMAX_SET                     "isp_ahdr_ratiomax_set"
    #define ISP_AHDR_RATIOMIN_SET                     "isp_ahdr_ratiomin_set"
    #define ISP_AHDR_GENERATION_GET                   "isp_ahdr_generation_get"

    #define ISP_CA_CFG_SET                            "isp_ca_cfg_set"
    #define ISP_CA_CFG_GET                            "isp_ca_cfg_get"
    #define ISP_CA_ENABLE_SET                         "isp_ca_enable_set"
    #define ISP_CA_ENABLE_GET                         "isp_ca_enable_get"
    #define ISP_CA_TABLE_SET                          "isp_ca_table_set"
    #define ISP_CA_TABLE_GET                          "isp_ca_table_get"
    #define ISP_CA_AUTOMODE_SET                       "isp_ca_automode_set"
    #define ISP_CA_MODE_SET                           "isp_ca_mode_set"
    #define ISP_CA_CURVE_SET                          "isp_ca_curve_set"
    #define ISP_CA_SINFLECTION_SET                    "isp_ca_sinflection_set"
    #define ISP_CA_SEXPONENT_SET                      "isp_ca_sexponent_set"
    #define ISP_CA_PARABOLICFACTOR_SET                "isp_ca_parabolicfactor_set"



    #define ISP_DG_CFG_SET                            "isp_dg_cfg_set"
    #define ISP_DG_CFG_GET                            "isp_dg_cfg_get"
    #define ISP_DG_ENABLE_SET                         "isp_dg_enable_set"
    #define ISP_DG_ENABLE_GET                         "isp_dg_enable_get"


static std::unordered_map<std::string, int> isp_ioctl_ids_map = {
    { ISP_AE_CFG_GET,                               ISPCORE_MODULE_AE_CFG_GET },
    { ISP_AE_CFG_SET,                               ISPCORE_MODULE_AE_CFG_SET },
    { ISP_AE_ECM_GET,                               ISPCORE_MODULE_AE_ECM_GET },
    { ISP_AE_ECM_SET,                               ISPCORE_MODULE_AE_ECM_SET },
    { ISP_AE_ENABLE_GET,                            ISPCORE_MODULE_AE_ENABLE_GET },
    { ISP_AE_ENABLE_SET,                            ISPCORE_MODULE_AE_ENABLE_SET },
    { ISP_AE_RESET,                                 ISPCORE_MODULE_AE_RESET },
    { ISP_AE_SETPOINT_SET,                          ISPCORE_MODULE_AE_SETPOINT_SET },
    { ISP_AE_DAMPOVER_SET,                          ISPCORE_MODULE_AE_DAMPOVER_SET },
    { ISP_AE_DAMPUNDER_SET,                         ISPCORE_MODULE_AE_DAMPUNDER_SET },
    { ISP_AE_TOLERANCE_SET,                         ISPCORE_MODULE_AE_TOLERANCE_SET },
    { ISP_AE_GENERATION_SET,                        ISPCORE_MODULE_AE_GENERATION_SET },

    { ISP_EC_EXPOSURETIME_SET,                      ISPCORE_MODULE_SENSOR_EXPOSURETIME_SET },
    { ISP_EC_GAIN_SET,                              ISPCORE_MODULE_SENSOR_GAIN_SET },

    { ISP_AF_AVI_GET,                               ISPCORE_MODULE_AF_AVI_GET },
    { ISP_AF_CFG_GET,                               ISPCORE_MODULE_AF_CFG_GET },
    { ISP_AF_CFG_SET,                               ISPCORE_MODULE_AF_CFG_SET },
    { ISP_AF_ENABLE_GET,                            ISPCORE_MODULE_AF_ENABLE_GET },
    { ISP_AF_ENABLE_SET,                            ISPCORE_MODULE_AF_ENABLE_SET },

    { ISP_AVS_CFG_GET,                              ISPCORE_MODULE_AVS_CFG_GET },
    { ISP_AVS_CFG_SET,                              ISPCORE_MODULE_AVS_CFG_SET },
    { ISP_AVS_ENABLE_GET,                           ISPCORE_MODULE_AVS_ENABLE_GET },
    { ISP_AVS_ENABLE_SET,                           ISPCORE_MODULE_AVS_ENABLE_SET },
    { ISP_AVS_STATUS_GET,                           ISPCORE_MODULE_AVS_STATUS_GET },

    { ISP_AWB_CFG_GET,                              ISPCORE_MODULE_AWB_CFG_GET },
    { ISP_AWB_CFG_SET,                              ISPCORE_MODULE_AWB_CFG_SET },
    { ISP_AWB_ENABLE_GET,                           ISPCORE_MODULE_AWB_ENABLE_GET },
    { ISP_AWB_ENABLE_SET,                           ISPCORE_MODULE_AWB_ENABLE_SET },
    { ISP_AWB_ILLUMPRO_GET,                         ISPCORE_MODULE_AWB_ILLUMPRO_GET },
    { ISP_AWB_ILLUMPRO_SET,                         ISPCORE_MODULE_AWB_ILLUMPRO_SET },
    { ISP_AWB_RESET,                                ISPCORE_MODULE_AWB_RESET },
    { ISP_AWB_STATUS_GET,                           ISPCORE_MODULE_AWB_STATUS_GET },
    { ISP_AWB_MEAS_WIN_SET,                         ISPCORE_MODULE_AWB_MEAS_WIN_SET },
    { ISP_AWB_MODE_SET,                             ISPCORE_MODULE_AWB_MODE_SET },
    { ISP_AWB_DAMP_SET,                             ISPCORE_MODULE_AWB_DAMP_SET },

    { ISP_BLS_CFG_GET,                              ISPCORE_MODULE_BLS_CFG_GET },
    { ISP_BLS_CFG_SET,                              ISPCORE_MODULE_BLS_CFG_SET },

    { ISP_CAC_ENABLE_GET,                           ISPCORE_MODULE_CAC_ENABLE_GET },
    { ISP_CAC_ENABLE_SET,                           ISPCORE_MODULE_CAC_ENABLE_SET },
    { ISP_CAC_STATUS,                               ISPCORE_MODULE_CAC_STATUS },

    { ISP_CNR_CFG_GET,                              ISPCORE_MODULE_CNR_CFG_GET },
    { ISP_CNR_CFG_SET,                              ISPCORE_MODULE_CNR_CFG_SET },
    { ISP_CNR_ENABLE_GET,                           ISPCORE_MODULE_CNR_ENABLE_GET },
    { ISP_CNR_ENABLE_SET,                           ISPCORE_MODULE_CNR_ENABLE_SET },

    { ISP_CPROC_CFG_GET,                            ISPCORE_MODULE_CPROC_CFG_GET },
    { ISP_CPROC_CFG_SET,                            ISPCORE_MODULE_CPROC_CFG_SET },
    { ISP_CPROC_ENABLE_GET,                         ISPCORE_MODULE_CPROC_ENABLE_GET },
    { ISP_CPROC_ENABLE_SET,                         ISPCORE_MODULE_CPROC_ENABLE_SET },
    { ISP_CPROC_LUMAIN_SET,                         ISPCORE_MODULE_CPROC_LUMAIN_SET },
    { ISP_CPROC_LUMAOUT_SET,                        ISPCORE_MODULE_CPROC_LUMAOUT_SET },
    { ISP_CPROC_CHROMAOUT_SET,                      ISPCORE_MODULE_CPROC_CHROMAOUT_SET },
    { ISP_CPROC_BRIGHTNESS_SET,                     ISPCORE_MODULE_CPROC_BRIGHTNESS_SET },
    { ISP_CPROC_CONTRAST_SET,                       ISPCORE_MODULE_CPROC_CONTRAST_SET },
    { ISP_CPROC_SATURATION_SET,                     ISPCORE_MODULE_CPROC_SATURATION_SET },
    { ISP_CPROC_HUE_SET,                            ISPCORE_MODULE_CPROC_HUE_SET },

    { ISP_DEMOSAIC_CFG_GET,                         ISPCORE_MODULE_DEMOSAIC_CFG_GET },
    { ISP_DEMOSAIC_CFG_SET,                         ISPCORE_MODULE_DEMOSAIC_CFG_SET },
    { ISP_DEMOSAIC_ENABLE_GET,                      ISPCORE_MODULE_DEMOSAIC_ENABLE_GET },
    { ISP_DEMOSAIC_ENABLE_SET,                      ISPCORE_MODULE_DEMOSAIC_ENABLE_SET },
    { ISP_DEMOSAIC_RESET,                           ISPCORE_MODULE_DEMOSAIC_RESET },
    { ISP_DEMOSAIC_THRESHOLD_SET,                   ISPCORE_MODULE_DEMOSAIC_THRESHLOD_SET },
    { ISP_DEMOSAIC_GLOBAL_INTERPTHRESHOLD_SET,      ISPCORE_MODULE_DEMOSAIC_GLOBAL_INTERPTHRESHOLD_SET },
    { ISP_DEMOSAIC_GLOBAL_DENOISESTRENGTH_SET,      ISPCORE_MODULE_DEMOSAIC_GLOBAL_DENOISESTRENGTH_SET },
    { ISP_DEMOSAIC_GLOBAL_SHARPENSIZE_SET,          ISPCORE_MODULE_DEMOSAIC_GLOBAL_SHARPENSIZE_SET },
    { ISP_DEMOSAIC_DEMOIRE_ENABLE_SET,              ISPCORE_MODULE_DEMOSAIC_DEMOIRE_ENABLE_SET },
    { ISP_DEMOSAIC_DEMOIRE_AREATHRESHOLD_SET,       ISPCORE_MODULE_DEMOSAIC_DEMOIRE_AREATHRESHOLD_SET },
    { ISP_DEMOSAIC_DEMOIRE_SATURATION_SET,          ISPCORE_MODULE_DEMOSAIC_DEMOIRE_SATURATION_SET },
    { ISP_DEMOSAIC_DEMOIRE_THRESHOLD_SET,           ISPCORE_MODULE_DEMOSAIC_DEMOIRE_THRESHOLD_SET },
    { ISP_DEMOSAIC_DEMOIRE_EDGE_SET,                ISPCORE_MODULE_DEMOSAIC_DEMOIRE_EDGE_SET },
    { ISP_DEMOSAIC_DEPURPLE_ENABLE_SET,             ISPCORE_MODULE_DEMOSAIC_DEPURPLE_ENABLE_SET },
    { ISP_DEMOSAIC_DEPURPLE_CBCRMODE_SET,           ISPCORE_MODULE_DEMOSAIC_DEPURPLE_CBCRMODE_SET },
    { ISP_DEMOSAIC_DEPURPLE_SATURATION_SET,         ISPCORE_MODULE_DEMOSAIC_DEPURPLE_SATURATION_SET },
    { ISP_DEMOSAIC_DEPURPLE_THRESHOLD_SET,          ISPCORE_MODULE_DEMOSAIC_DEPURPLE_THRESHOLD_SET },
    { ISP_DEMOSAIC_SHARPEN_ENABLE_SET,              ISPCORE_MODULE_DEMOSAIC_SHARPEN_ENABLE_SET },
    { ISP_DEMOSAIC_SHARPEN_CLIP_SET,                ISPCORE_MODULE_DEMOSAIC_SHARPEN_CLIP_SET },
    { ISP_DEMOSAIC_SHARPEN_FACTOR_SET,              ISPCORE_MODULE_DEMOSAIC_SHARPEN_FACTOR_SET },
    { ISP_DEMOSAIC_SHARPEN_THRESHOLD_SET,           ISPCORE_MODULE_DEMOSAIC_SHARPEN_THRESHOLD_SET },
    { ISP_DEMOSAIC_SHARPEN_RATIO_SET,               ISPCORE_MODULE_DEMOSAIC_SHARPEN_RATIO_SET },
    { ISP_DEMOSAIC_SHARPEN_FILTER_SET,              ISPCORE_MODULE_DEMOSAIC_SHARPEN_FILTER_SET },
    { ISP_DEMOSAIC_SHARPENLINE_ENABLE_SET,          ISPCORE_MODULE_DEMOSAIC_SHARPENLINE_ENABLE_SET },
    { ISP_DEMOSAIC_SHARPENLINE_RATIO_SET,           ISPCORE_MODULE_DEMOSAIC_SHARPENLINE_RATIO_SET },
    { ISP_DEMOSAIC_SHARPENLINE_SHIFT1_SET,          ISPCORE_MODULE_DEMOSAIC_SHARPENLINE_SHIFT1_SET },
    { ISP_DEMOSAIC_SHARPENLINE_SHIFT2_SET,          ISPCORE_MODULE_DEMOSAIC_SHARPENLINE_SHIFT2_SET },
    { ISP_DEMOSAIC_SHARPENLINE_STRENGTH_SET,        ISPCORE_MODULE_DEMOSAIC_SHARPENLINE_STRENGTH_SET },
    { ISP_DEMOSAIC_SHARPENLINE_T1_SET,              ISPCORE_MODULE_DEMOSAIC_SHARPENLINE_T1_SET },
    { ISP_DEMOSAIC_SKIN_ENABLE_SET,                 ISPCORE_MODULE_DEMOSAIC_SKIN_ENABLE_SET },
    { ISP_DEMOSAIC_SKIN_CB_THRESHOLD_SET,           ISPCORE_MODULE_DEMOSAIC_SKIN_CB_THRESHOLD_SET },
    { ISP_DEMOSAIC_SKIN_CR_THRESHOLD_SET,           ISPCORE_MODULE_DEMOSAIC_SKIN_CR_THRESHOLD_SET },
    { ISP_DEMOSAIC_SKIN_Y_THRESHOLD_SET,            ISPCORE_MODULE_DEMOSAIC_SKIN_Y_THRESHOLD_SET },

    { ISP_DEVICE_CALIBRATIONSAVE,                   ISPCORE_MODULE_DEVICE_CALIBRATIONSAVE },
    { ISP_DEVICE_CALIBRATIONLOADANDINIT,            ISPCORE_MODULE_DEVICE_CALIBRATIONLOADANDINIT },
    { ISP_DEVICE_CALIBRATIONLOAD,                   ISPCORE_MODULE_DEVICE_CALIBRATIONLOAD },
    { ISP_DEVICE_INITENGINEOPERATION,               ISPCORE_MODULE_DEVICE_INITENGINEOPERATION },
    { ISP_DEVICE_RESOLUTION_SET_SENSOR_ISP,         ISPCORE_MODULE_DEVICE_RESOLUTION_SET_SENSOR_ISP },
    { ISP_DEVICE_CAMERA_CONNECT,                    ISPCORE_MODULE_DEVICE_CAMERA_CONNECT },

    { ISP_DEVICE_CAPTUREDMA,                        ISPCORE_MODULE_DEVICE_CAPTUREDMA },
    { ISP_DEVICE_CAPTURESENSOR,                     ISPCORE_MODULE_DEVICE_CAPTURESENSOR },
    { ISP_DEVICE_INPUTINFO,                         ISPCORE_MODULE_DEVICE_INPUTINFO },
    { ISP_DEVICE_INPUTSWITCH,                       ISPCORE_MODULE_DEVICE_INPUTSWITCH },
    { ISP_DEVICE_PREVIEW,                           ISPCORE_MODULE_DEVICE_PREVIEW },
    { ISP_DEVICE_BITSTREAMID,                       ISPCORE_MODULE_DEVICE_BITSTREAMID },
    { ISP_DEVICE_CAMERIC_ID,                        ISPCORE_MODULE_DEVICE_CAMERIC_ID },
    { ISP_DEVICE_INPUT_CONNECT,                     ISPCORE_MODULE_DEVICE_INPUT_CONNECT },
    { ISP_DEVICE_INPUT_DISCONNECT,                  ISPCORE_MODULE_DEVICE_INPUT_DISCONNECT },
    { ISP_DEVICE_RESET,                             ISPCORE_MODULE_DEVICE_RESET },
    { ISP_DEVICE_RESOLUTION_GET,                    ISPCORE_MODULE_DEVICE_RESOLUTION_GET },
    { ISP_DEVICE_RESOLUTION_SET,                    ISPCORE_MODULE_DEVICE_RESOLUTION_SET },
    { ISP_DEVICE_STREAMING_START,                   ISPCORE_MODULE_DEVICE_STREAMING_START },
    { ISP_DEVICE_STREAMING_STOP,                    ISPCORE_MODULE_DEVICE_STREAMING_STOP },
    { ISP_DEVICE_CAMERA_DISCONNECT,                 ISPCORE_MODULE_DEVICE_CAMERA_DISCONNECT },
    { ISP_DEVICE_CAMERA_RESET,                      ISPCORE_MODULE_DEVICE_CAMERA_RESET },
    { ISP_DEVICE_PREVIEW_START,                     ISPCORE_MODULE_DEVICE_PREVIEW_START },
    { ISP_DEVICE_PREVIEW_STOP,                      ISPCORE_MODULE_DEVICE_PREVIEW_STOP },
    { ISP_DEVICE_GET_HALHANDLE,                     ISPCORE_MODULE_DEVICE_GET_HALHANDLE },
    { ISP_DEVICE_GET_METADATA,                      ISPCORE_MODULE_DEVICE_GET_METADATA },
    { ISP_DEVICE_OUTPUT_IMAGE_FORMAT,               ISPCORE_MODULE_DEVICE_OUTPUT_IMAGE_FORMAT },
    { ISP_DEVICE_STANDBY_ENABLE_SET,                ISPCORE_MODULE_DEVICE_STANDBY_ENABLE_SET },

    { ISP_2DNR_CFG_GET,                             ISPCORE_MODULE_2DNR_CFG_GET },
    { ISP_2DNR_CFG_SET,                             ISPCORE_MODULE_2DNR_CFG_SET },
    { ISP_2DNR_ENABLE_GET,                          ISPCORE_MODULE_2DNR_ENABLE_GET },
    { ISP_2DNR_ENABLE_SET,                          ISPCORE_MODULE_2DNR_ENABLE_SET },
    { ISP_2DNR_RESET,                               ISPCORE_MODULE_2DNR_RESET },
    { ISP_2DNR_STATUS_GET,                          ISPCORE_MODULE_2DNR_STATUS_GET },
    { ISP_2DNR_TABLE_GET,                           ISPCORE_MODULE_2DNR_TABLE_GET },
    { ISP_2DNR_TABLE_SET,                           ISPCORE_MODULE_2DNR_TABLE_SET },
    { ISP_2DNR_GENERATION_SET,                      ISPCORE_MODULE_2DNR_GENERATION_SET },
    { ISP_2DNR_AUTOMODE_SET,                        ISPCORE_MODULE_2DNR_AUTOMODE_SET },
    { ISP_2DNR_AUTOLEVEL_SET,                       ISPCORE_MODULE_2DNR_AUTOLEVEL_SET },
    { ISP_2DNR_DENOISEPRESTRENGTH_SET,              ISPCORE_MODULE_2DNR_DENOISEPRESTRENGTH_SET },
    { ISP_2DNR_DENOISESTRENGTH_SET,                 ISPCORE_MODULE_2DNR_DENOISESTRENGTH_SET },
    { ISP_2DNR_SIGMA_SET,                           ISPCORE_MODULE_2DNR_SIGMA_SET },
    { ISP_2DNR_BLENDOPACITYSTATIC_SET,              ISPCORE_MODULE_2DNR_BLENDOPACITYSTATIC_SET },
    { ISP_2DNR_BLENDOPACITYMOVING_SET,              ISPCORE_MODULE_2DNR_BLENDOPACITYMOVING_SET },

    { ISP_3DNR_CFG_GET,                             ISPCORE_MODULE_3DNR_CFG_GET },
    { ISP_3DNR_CFG_SET,                             ISPCORE_MODULE_3DNR_CFG_SET },
    { ISP_3DNR_ENABLE_GET,                          ISPCORE_MODULE_3DNR_ENABLE_GET },
    { ISP_3DNR_ENABLE_SET,                          ISPCORE_MODULE_3DNR_ENABLE_SET },
    { ISP_3DNR_RESET,                               ISPCORE_MODULE_3DNR_RESET },
    { ISP_3DNR_STATUS_GET,                          ISPCORE_MODULE_3DNR_STATUS_GET },
    { ISP_3DNR_TABLE_GET,                           ISPCORE_MODULE_3DNR_TABLE_GET },
    { ISP_3DNR_TABLE_SET,                           ISPCORE_MODULE_3DNR_TABLE_SET },
    { ISP_3DNR_GENERATION_SET,                      ISPCORE_MODULE_3DNR_GENERATION_SET },
    { ISP_3DNR_AUTOMODE_SET,                        ISPCORE_MODULE_3DNR_AUTOMODE_SET },
    { ISP_3DNR_AUTOLEVEL_SET,                       ISPCORE_MODULE_3DNR_AUTOLEVEL_SET },
    { ISP_3DNR_DELTAFACTOR_SET,                     ISPCORE_MODULE_3DNR_DELTAFACTOR_SET },
    { ISP_3DNR_MOTIONFACTOR_SET,                    ISPCORE_MODULE_3DNR_MOTIONFACTOR_SET },
    { ISP_3DNR_STRENGTH_SET,                        ISPCORE_MODULE_3DNR_STRENGTH_SET },
    { ISP_3DNR_TNR_ENABLE_SET,                      ISPCORE_MODULE_3DNR_TNR_ENABLE_SET },
    { ISP_3DNR_TNR_FILTERLENGTH_SET,                ISPCORE_MODULE_3DNR_TNR_FILTERLENGTH_SET  },
    { ISP_3DNR_TNR_FILTERLENGTH2_SET,               ISPCORE_MODULE_3DNR_TNR_FILTERLENGTH2_SET },
    { ISP_3DNR_TNR_MOTIONSLOPE_SET,                 ISPCORE_MODULE_3DNR_TNR_MOTIONSLOPE_SET },
    { ISP_3DNR_TNR_NOISELEVEL_SET,                  ISPCORE_MODULE_3DNR_TNR_NOISELEVEL_SET },
    { ISP_3DNR_TNR_SADWEIGHT_SET,                   ISPCORE_MODULE_3DNR_TNR_SADWEIGHT_SET },

    { ISP_DPCC_ENABLE_GET,                          ISPCORE_MODULE_DPCC_ENABLE_GET },
    { ISP_DPCC_ENABLE_SET,                          ISPCORE_MODULE_DPCC_ENABLE_SET },

    { ISP_DPF_CFG_GET,                              ISPCORE_MODULE_DPF_CFG_GET },
    { ISP_DPF_CFG_SET,                              ISPCORE_MODULE_DPF_CFG_SET },
    { ISP_DPF_ENABLE_GET,                           ISPCORE_MODULE_DPF_ENABLE_GET },
    { ISP_DPF_ENABLE_SET,                           ISPCORE_MODULE_DPF_ENABLE_SET },

    { ISP_EE_CFG_GET,                               ISPCORE_MODULE_EE_CFG_GET },
    { ISP_EE_CFG_SET,                               ISPCORE_MODULE_EE_CFG_SET },
    { ISP_EE_ENABLE_GET,                            ISPCORE_MODULE_EE_ENABLE_GET },
    { ISP_EE_ENABLE_SET,                            ISPCORE_MODULE_EE_ENABLE_SET },
    { ISP_EE_RESET,                                 ISPCORE_MODULE_EE_RESET },
    { ISP_EE_STATUS_GET,                            ISPCORE_MODULE_EE_STATUS_GET },
    { ISP_EE_TABLE_GET,                             ISPCORE_MODULE_EE_TABLE_GET },
    { ISP_EE_TABLE_SET,                             ISPCORE_MODULE_EE_TABLE_SET },
    { ISP_EE_AUTOMODE_SET,                          ISPCORE_MODULE_EE_AUTOMODE_SET },
    { ISP_EE_STRENGTH_SET,                          ISPCORE_MODULE_EE_STRENGTH_SET },
    { ISP_EE_YUPGAIN_SET,                           ISPCORE_MODULE_EE_YUPGAIN_SET },
    { ISP_EE_YDOWNGAIN_SET,                         ISPCORE_MODULE_EE_YDOWNGAIN_SET },
    { ISP_EE_UVGAIN_SET,                            ISPCORE_MODULE_EE_UVGAIN_SET },
    { ISP_EE_EDGEGAIN_SET,                          ISPCORE_MODULE_EE_EDGEGAIN_SET },


    { ISP_FILESYSTEM_REMOVE,                        ISPCORE_MODULE_FILESYSTEM_REMOVE },
    { ISP_FILESYSTEM_LIST,                          ISPCORE_MODULE_FILESYSTEM_LIST },

    { ISP_FILTER_CFG_GET,                           ISPCORE_MODULE_FILTER_CFG_GET },
    { ISP_FILTER_CFG_SET,                           ISPCORE_MODULE_FILTER_CFG_SET },
    { ISP_FILTER_ENABLE_GET,                        ISPCORE_MODULE_FILTER_ENABLE_GET },
    { ISP_FILTER_ENABLE_SET,                        ISPCORE_MODULE_FILTER_ENABLE_SET },
    { ISP_FILTER_STATUS_GET,                        ISPCORE_MODULE_FILTER_STATUS_GET },
    { ISP_FILTER_TABLE_GET,                         ISPCORE_MODULE_FILTER_TABLE_GET },
    { ISP_FILTER_TABLE_SET,                         ISPCORE_MODULE_FILTER_TABLE_SET },

    { ISP_GC_CFG_GET,                               ISPCORE_MODULE_GC_CFG_GET },
    { ISP_GC_CFG_SET,                               ISPCORE_MODULE_GC_CFG_SET },
    { ISP_GC_ENABLE_GET,                            ISPCORE_MODULE_GC_ENABLE_GET },
    { ISP_GC_ENABLE_SET,                            ISPCORE_MODULE_GC_ENABLE_SET },
    { ISP_GC_USERCURVE_SET,                         ISPCORE_MODULE_GC_CFG_SET },
    { ISP_GC_STANDCURVE_SET,                        ISPCORE_MODULE_GC_CFG_SET },

    { ISP_HDR_CFG_GET,                              ISPCORE_MODULE_HDR_CFG_GET },
    { ISP_HDR_CFG_SET,                              ISPCORE_MODULE_HDR_CFG_SET },
    { ISP_HDR_ENABLE_GET,                           ISPCORE_MODULE_HDR_ENABLE_GET },
    { ISP_HDR_ENABLE_SET,                           ISPCORE_MODULE_HDR_ENABLE_SET },
    { ISP_HDR_RESET,                                ISPCORE_MODULE_HDR_RESET },
    { ISP_HDR_STATUS,                               ISPCORE_MODULE_HDR_STATUS },
    { ISP_HDR_GENERATION_SET,                       ISPCORE_MODULE_HDR_GENERATION_SET },
    { ISP_HDR_EXPOSURERATIO_SET,                    ISPCORE_MODULE_HDR_EXPOSURERATIO_SET },
    { ISP_HDR_EXTENSIONBIT_SET,                     ISPCORE_MODULE_HDR_EXTENSIONBIT_SET },
    { ISP_HDR_TRANSRANGE_SET,                       ISPCORE_MODULE_HDR_TRANSRANGE_SET },

    { ISP_IE_CFG_GET,                               ISPCORE_MODULE_IE_CFG_GET },
    { ISP_IE_CFG_SET,                               ISPCORE_MODULE_IE_CFG_SET },
    { ISP_IE_ENABLE_GET,                            ISPCORE_MODULE_IE_ENABLE_GET },
    { ISP_IE_ENABLE_SET,                            ISPCORE_MODULE_IE_ENABLE_SET },

    { ISP_LSC_CFG_GET,                              ISPCORE_MODULE_LSC_CFG_GET },
    { ISP_LSC_CFG_SET,                              ISPCORE_MODULE_LSC_CFG_SET },
    { ISP_LSC_ENABLE_GET,                           ISPCORE_MODULE_LSC_ENABLE_GET },
    { ISP_LSC_ENABLE_SET,                           ISPCORE_MODULE_LSC_ENABLE_SET },
    { ISP_LSC_STATUS_GET,                           ISPCORE_MODULE_LSC_STATUS_GET },

    { ISP_REG_DESCRIPTION,                          ISPCORE_MODULE_REG_DESCRIPTION },
    { ISP_REG_GET,                                  ISPCORE_MODULE_REG_GET },
    { ISP_REG_SET,                                  ISPCORE_MODULE_REG_SET },

    { ISP_SENSOR_CAPS,                              ISPCORE_MODULE_SENSOR_CAPS },
    { ISP_SENSOR_CFG_GET,                           ISPCORE_MODULE_SENSOR_CFG_GET },
    { ISP_SENSOR_CFG_SET,                           ISPCORE_MODULE_SENSOR_CFG_SET },
    { ISP_SENSOR_NAME_GET,                          ISPCORE_MODULE_SENSOR_NAME_GET },
    { ISP_SENSOR_REVISION_GET,                      ISPCORE_MODULE_SENSOR_REVISION_GET },
    { ISP_SENSOR_OPEN,                              ISPCORE_MODULE_SENSOR_OPEN },
    { ISP_SENSOR_IS_CONNECTED,                      ISPCORE_MODULE_SENSOR_IS_CONNECTED },
    { ISP_SENSOR_DRV_CHANGE,                        ISPCORE_MODULE_SENSOR_DRV_CHANGE },
    { ISP_SENSOR_DRV_LIST,                          ISPCORE_MODULE_SENSOR_DRV_LIST },
    { ISP_SENSOR_INFO,                              ISPCORE_MODULE_SENSOR_INFO },
    { ISP_SENSOR_REG_DESCRIPTION,                   ISPCORE_MODULE_SENSOR_REG_DESCRIPTION },
    { ISP_SENSOR_REG_DUMP2FILE,                     ISPCORE_MODULE_SENSOR_REG_DUMP2FILE },
    { ISP_SENSOR_REG_GET,                           ISPCORE_MODULE_SENSOR_REG_GET },
    { ISP_SENSOR_REG_SET,                           ISPCORE_MODULE_SENSOR_REG_SET },
    { ISP_SENSOR_REG_TABLE,                         ISPCORE_MODULE_SENSOR_REG_TABLE },
    { ISP_SENSOR_CALIB_INSTALL,                     ISPCORE_MODULE_SENSOR_CALIB_INSTALL },
    { ISP_SENSOR_CALIB_UNINSTALL,                   ISPCORE_MODULE_SENSOR_CALIB_UNINSTALL },
    { ISP_SENSOR_FPS_GET,                           ISPCORE_MODULE_SENSOR_FPS_GET },
    { ISP_SENSOR_FPS_SET,                           ISPCORE_MODULE_SENSOR_FPS_SET },
    { ISP_SENSOR_QUERY,                             ISPCORE_MODULE_SENSOR_QUERY },
    { ISP_SENSOR_MODE_GET,                          ISPCORE_MODULE_SENSOR_MODE_GET },
    { ISP_SENSOR_MODE_SET,                          ISPCORE_MODULE_SENSOR_MODE_SET },
    { ISP_SENSOR_PRE_LOAD_LIB,                      ISPCORE_MODULE_SENSOR_PRE_LOAD_LIB },
    { ISP_SENSOR_MODE_LOCK,                         ISPCORE_MODULE_SENSOR_MODE_LOCK },
    { ISP_SENSOR_STATUS,                            ISPCORE_MODULE_SENSOR_STATUS },

    { ISP_SIMP_CFG_GET,                             ISPCORE_MODULE_SIMP_CFG_GET },
    { ISP_SIMP_CFG_SET,                             ISPCORE_MODULE_SIMP_CFG_SET },
    { ISP_SIMP_ENABLE_GET,                          ISPCORE_MODULE_SIMP_ENABLE_GET },
    { ISP_SIMP_ENABLE_SET,                          ISPCORE_MODULE_SIMP_ENABLE_SET },

    { ISP_WB_CFG_GET,                               ISPCORE_MODULE_WB_CFG_GET },
    { ISP_WB_CFG_SET,                               ISPCORE_MODULE_WB_CFG_SET },
    { ISP_WB_GAIN_SET,                              ISPCORE_MODULE_WB_GAIN_SET },
    { ISP_WB_CCOFFSET_SET,                          ISPCORE_MODULE_WB_CCOFFSET_SET },
    { ISP_WB_CCMATRIX_SET,                          ISPCORE_MODULE_WB_CCMATRIX_SET },

    { ISP_WDR_CFG_GET,                              ISPCORE_MODULE_WDR_CFG_GET },
    { ISP_WDR_CFG_SET,                              ISPCORE_MODULE_WDR_CFG_SET },
    { ISP_WDR_ENABLE_GET,                           ISPCORE_MODULE_WDR_ENABLE_GET },
    { ISP_WDR_ENABLE_SET,                           ISPCORE_MODULE_WDR_ENABLE_SET },
    { ISP_WDR_RESET,                                ISPCORE_MODULE_WDR_RESET },
    { ISP_WDR_STATUS_GET,                           ISPCORE_MODULE_WDR_STATUS_GET },
    { ISP_WDR_TABLE_GET,                            ISPCORE_MODULE_WDR_TABLE_GET },
    { ISP_WDR_TABLE_SET,                            ISPCORE_MODULE_WDR_TABLE_SET },
    { ISP_WDR_GENERATION_SET,                       ISPCORE_MODULE_WDR_GENERATION_SET },
    { ISP_WDR_STRENGTH_SET,                         ISPCORE_MODULE_WDR_STRENGTH_SET },
    { ISP_WDR_STRENGTH_RANGE_SET,                   ISPCORE_MODULE_WDR_STRENGTH_RANGE_SET },
    { ISP_WDR_GSTRENGTH_SET,                        ISPCORE_MODULE_WDR_GSTRENGTH_SET },
    { ISP_WDR_AUTO_MODE_SET,                        ISPCORE_MODULE_WDR_AUTO_MODE_SET },
    { ISP_WDR_AUTO_LEVEL_SET,                       ISPCORE_MODULE_WDR_AUTO_LEVEL_SET },
    { ISP_WDR_GAIN_MAX_SET,                         ISPCORE_MODULE_WDR_GAIN_MAX_SET },
    { ISP_WDR_FLAT_STRENGTH_SET,                    ISPCORE_MODULE_WDR_FLAT_STRENGTH_SET },
    { ISP_WDR_FLAT_THRESHOLD_SET,                   ISPCORE_MODULE_WDR_FLAT_THRESHOLD_SET },
    { ISP_WDR_CONTRAST_SET,                         ISPCORE_MODULE_WDR_CONTRAST_SET },

    { ISP_PIPELINE_WARM_UP,                         ISPCORE_MODULE_PIPELINE_WARM_UP },
    { ISP_PIPELINE_SMP_MODE,                        ISPCORE_MODULE_PIPELINE_SMP_MODE },
    { ISP_PIPELINE_CFG_DWE,                         ISPCORE_MODULE_PIPELINE_CFG_DWE },
    { ISP_PIPELINE_CFG_TESTPATTEN_EN,               ISPCORE_MODULE_PIPELINE_CFG_TESTPATTEN_EN },
    { ISP_PIPELINE_RESOLUTION_IS_OUT,               ISPCORE_MODULE_PIPELINE_RESOLUTION_IS_OUT },
    { ISP_PIPELINE_RESOLUTION_MP_OUT,               ISPCORE_MODULE_PIPELINE_RESOLUTION_MP_OUT },
    { ISP_PIPELINE_MP_OUT_IMAGE_FORMAT,             ISPCORE_MODULE_PIPELINE_MP_OUT_IMAGE_FORMAT },
    { ISP_PIPELINE_QUERY,                           ISPCORE_MODULE_PIPELINE_QUERY },
    { ISP_PIPELINE_CFG_STATUS,                      ISPCORE_MODULE_PIPELINE_CFG_STATUS },

    { ISP_GE_CFG_SET,                               ISPCORE_MODULE_GE_CFG_SET },
    { ISP_GE_CFG_GET,                               ISPCORE_MODULE_GE_CFG_GET },
    { ISP_GE_ENABLE_SET,                            ISPCORE_MODULE_GE_ENABLE_SET },
    { ISP_GE_ENABLE_GET,                            ISPCORE_MODULE_GE_ENABLE_GET },

    { ISP_RGBGAMMA_CFG_SET,                         ISPCORE_MODULE_RGBGAMMA_CFG_SET },
    { ISP_RGBGAMMA_CFG_GET,                         ISPCORE_MODULE_RGBGAMMA_CFG_GET },
    { ISP_RGBGAMMA_ENABLE_SET,                      ISPCORE_MODULE_RGBGAMMA_ENABLE_SET },
    { ISP_RGBGAMMA_ENABLE_GET,                      ISPCORE_MODULE_RGBGAMMA_ENABLE_GET },

    { ISP_DCI_CFG_SET,                              ISPCORE_MODULE_DCI_CFG_SET },
    { ISP_DCI_CFG_GET,                              ISPCORE_MODULE_DCI_CFG_GET },
    { ISP_DCI_ENABLE_SET,                           ISPCORE_MODULE_DCI_ENABLE_SET },
    { ISP_DCI_ENABLE_GET,                           ISPCORE_MODULE_DCI_ENABLE_GET },
    { ISP_DCI_STATUS_GET,                           ISPCORE_MODULE_DCI_STATUS_GET },
    { ISP_DCI_TABLE_SET,                            ISPCORE_MODULE_DCI_TABLE_SET },
    { ISP_DCI_TABLE_GET,                            ISPCORE_MODULE_DCI_TABLE_GET },
    { ISP_DCI_AUTOMODE_SET,                         ISPCORE_MODULE_DCI_AUTOMODE_SET },
    { ISP_DCI_MODE_SET,                             ISPCORE_MODULE_DCI_MODE_SET },
    { ISP_DCI_EXPONENTHIGH_SET,                     ISPCORE_MODULE_DCI_EXPONENTHIGH_SET },
    { ISP_DCI_EXPONENTLOW_SET,                      ISPCORE_MODULE_DCI_EXPONENTLOW_SET },
    { ISP_DCI_EXPONENTMID_SET,                      ISPCORE_MODULE_DCI_EXPONENTMID_SET },
    { ISP_DCI_HISTSCALE_SET,                        ISPCORE_MODULE_DCI_HISTSCALE_SET },
    { ISP_DCI_HISTSTEP_SET,                         ISPCORE_MODULE_DCI_HISTSTEP_SET },
    { ISP_DCI_HISTBASE_SET,                         ISPCORE_MODULE_DCI_HISTBASE_SET },
    { ISP_DCI_HISTSTRENGTH_SET,                     ISPCORE_MODULE_DCI_HISTSTRENGTH_SET },
    { ISP_DCI_HISTDAMP_SET,                         ISPCORE_MODULE_DCI_HISTDAMP_SET },
    { ISP_DCI_POINTSTART_SET,                       ISPCORE_MODULE_DCI_POINTSTART_SET },
    { ISP_DCI_POINTEND_SET,                         ISPCORE_MODULE_DCI_POINTEND_SET },
    { ISP_DCI_POINTINFLECTION_SET,                  ISPCORE_MODULE_DCI_POINTINFLECTION_SET },
    { ISP_DCI_POINTINFLECTION2_SET,                 ISPCORE_MODULE_DCI_POINTINFLECTION2_SET },

    { ISP_AHDR_CFG_SET,                             ISPCORE_MODULE_AHDR_CFG_SET },
    { ISP_AHDR_CFG_GET,                             ISPCORE_MODULE_AHDR_CFG_GET },
    { ISP_AHDR_ENABLE_SET,                          ISPCORE_MODULE_AHDR_ENABLE_SET },
    { ISP_AHDR_ENABLE_GET,                          ISPCORE_MODULE_AHDR_ENABLE_GET },
    { ISP_AHDR_TABLE_SET,                           ISPCORE_MODULE_AHDR_TABLE_SET },
    { ISP_AHDR_TABLE_GET,                           ISPCORE_MODULE_AHDR_TABLE_GET },
    { ISP_AHDR_RESET,                               ISPCORE_MODULE_AHDR_RESET },
    { ISP_AHDR_AUTOMODE_SET,                        ISPCORE_MODULE_AHDR_AUTOMODE_SET },
    { ISP_AHDR_C_SET,                               ISPCORE_MODULE_AHDR_C_SET },
    { ISP_AHDR_C2_SET,                              ISPCORE_MODULE_AHDR_C2_SET },
    { ISP_AHDR_CEIL_SET,                            ISPCORE_MODULE_AHDR_CEIL_SET },
    { ISP_AHDR_OBJECTFECTOR_SET,                    ISPCORE_MODULE_AHDR_OBJECTFECTOR_SET },
    { ISP_AHDR_SETPOINT2_SET,                       ISPCORE_MODULE_AHDR_SETPOINT2_SET },
    { ISP_AHDR_TOLERANCE_SET,                       ISPCORE_MODULE_AHDR_TOLERANCE_SET },
    { ISP_AHDR_RATIOMAX_SET,                        ISPCORE_MODULE_AHDR_RATIOMAX_SET },
    { ISP_AHDR_RATIOMIN_SET,                        ISPCORE_MODULE_AHDR_RATIOMIN_SET },
    { ISP_AHDR_GENERATION_GET,                      ISPCORE_MODULE_AHDR_GENERATION_GET },

    { ISP_CA_CFG_SET,                               ISPCORE_MODULE_CA_CFG_SET },
    { ISP_CA_CFG_GET,                               ISPCORE_MODULE_CA_CFG_GET },
    { ISP_CA_ENABLE_SET,                            ISPCORE_MODULE_CA_ENABLE_SET },
    { ISP_CA_ENABLE_GET,                            ISPCORE_MODULE_CA_ENABLE_GET },
    { ISP_CA_TABLE_SET,                             ISPCORE_MODULE_CA_TABLE_SET },
    { ISP_CA_TABLE_GET,                             ISPCORE_MODULE_CA_TABLE_GET },
    { ISP_CA_AUTOMODE_SET,                          ISPCORE_MODULE_CA_AUTOMODE_SET },
    { ISP_CA_MODE_SET,                              ISPCORE_MODULE_CA_MODE_SET },
    { ISP_CA_CURVE_SET,                             ISPCORE_MODULE_CA_CURVE_SET },
    { ISP_CA_SINFLECTION_SET,                       ISPCORE_MODULE_CA_SINFLECTION_SET },
    { ISP_CA_SEXPONENT_SET,                         ISPCORE_MODULE_CA_SEXPONENT_SET },
    { ISP_CA_PARABOLICFACTOR_SET,                   ISPCORE_MODULE_CA_PARABOLICFACTOR_SET },

    { ISP_DG_CFG_SET,                               ISPCORE_MODULE_DG_CFG_SET },
    { ISP_DG_CFG_GET,                               ISPCORE_MODULE_DG_CFG_GET },
    { ISP_DG_ENABLE_SET,                            ISPCORE_MODULE_DG_ENABLE_SET },
    { ISP_DG_ENABLE_GET,                            ISPCORE_MODULE_DG_ENABLE_GET },

};

#endif
