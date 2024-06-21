#ifndef __OS05A10_REGS_H__
#define __OS05A10_REGS_H__

/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define OS05A10_DRVP_RESET_SLEEP_TIME                                  (3*1000)   // 3ms
#define OS05A10_DRVP_CHANGE_ADDRESS_SLEEP_TIME                         (1*1000)   // 1ms
#define OS05A10_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define OS05A10_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define OS05A10_DRVP_SENSOR_ACCESS_3_BYTE                              (3)
#define OS05A10_DRVP_SENSOR_ACCESS_4_BYTE                              (4)
#define OS05A10_DRVP_REGISTER_ACCESS_SIZE                              (2)
#define OS05A10_DRVP_16_BIT_ACCESS                                     (16)
#define OS05A10_DRVP_8_BIT_ACCESS                                      (8)
#define OS05A10_DRVP_MAX_NUMBER_SENSOR_INSTANCE                        (6)


///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define OS05A10_DRVP_SC_MODE_SELECT_ADDRESS                            (0x0100)
#define OS05A10_DRVP_SC_SOFTWARE_RESET_ADDRESS                         (0x0103)
#define OS05A10_DRVP_SC_SCCB_ID1_ADDRESS                               (0x3035)
#define OS05A10_DRVP_SC_CTRL_2B_ADDRESS                                (0x3036) // SC_SCCB_ID2
        

#define OS05A10_DRVP_SC_CHIP_ID_HIGH_ADDRESS                           (0x300A)
#define OS05A10_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0x300B)
#define OS05A10_DRVP_SC_REG5_ADDRESS                                   (0x3027) //

/* MEC/MGC */
#define OS05A10_DRVP_LONG_EXPO_H_ADDRESS                               (0x3501)
#define OS05A10_DRVP_LONG_EXPO_L_ADDRESS                               (0x3502)
#define OS05A10_DRVP_AEC_MANUAL_ADDRESS                                (0x3503)
#define OS05A10_DRVP_LONG_GAIN_H_ADDRESS                               (0x3508)
#define OS05A10_DRVP_LONG_GAIN_L_ADDRESS                               (0x3509)
#define OS05A10_DRVP_LONG_DIGIGAIN_H_ADDRESS                           (0x350A)
#define OS05A10_DRVP_LONG_DIGIGAIN_L_ADDRESS                           (0x350B)

#define OS05A10_DRVP_AEC_GAIN_1_ADDRESS                                (0x3509) //0-3 fraction  bits 4- 7 gain

#define OS05A10_DRVP_TIMING_X_OUTPUT_SIZE_ADDRESS                      (0x3808)
#define OS05A10_DRVP_TIMING_HTS_LOW_ADDRESS                            (0x380C)
#define OS05A10_DRVP_TIMING_HTS_HIGH_ADDRESS                           (0x380D)
#define OS05A10_DRVP_TIMING_VTS_LOW_ADDRESS                            (0x380E)
#define OS05A10_DRVP_TIMING_VTS_HIGH_ADDRESS                           (0x380F)

#define OS05A10_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS                (0x3820)
#define OS05A10_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS              (0x3821)

#define OS05A10_DRVP_TIMING_REG_2B_ADDRESS                             (0x382B)

#define OS05A10_DRVP_PLL1_PRE_DIVP_ADDRESS                             (0x030c)
#define OS05A10_DRVP_PLL1_PRE_DIV_ADDRESS                              (0x0303)
#define OS05A10_DRVP_PLL1_LOOP_HIGH_ADDRESS                            (0x0304)
#define OS05A10_DRVP_PLL1_LOOP_LOW_ADDRESS                             (0x0305)
#define OS05A10_DRVP_MIPI_PRE_DIVIDER_ADDRESS                          (0x0307)
#define OS05A10_DRVP_MIPI_DIVIDER_ADDRESS                              (0x0306)
#define OS05A10_DRVP_DIG_PCLK_DIV_ADDRESS                              (0x301e)
#define OS05A10_DRVP_PLL2_PRE_DIVP_ADDRESS                             (0x0322)
#define OS05A10_DRVP_PLL2_PRE_DIV_ADDRESS                              (0x0323)
#define OS05A10_DRVP_PLL2_LOOP_HIGH_ADDRESS                            (0x0324)
#define OS05A10_DRVP_PLL2_LOOP_LOW_ADDRESS                             (0x0325)
#define OS05A10_DRVP_PLL2_TC_PRE_DIVIDER_ADDRESS                       (0x0328)
#define OS05A10_DRVP_PLL2_TC_DIVIDER_ADDRESS                           (0x032A)
#define OS05A10_DRVP_DIG_SALCLK_DIV_ADDRESS                            (0x3016)

#define OS05A10_DRVP_GROUP_ACCESS_ADDRESS                              (0x3208)
#define OS05A10_DRVP_GROUP0_PERIOD_ADDRESS                             (0x3209)
#define OS05A10_DRVP_GROUP1_PERIOD_ADDRESS                             (0x320A)
#define OS05A10_DRVP_GROUP_SW_CTRL_ADDRESS                             (0x320D)

/* System control registers */
#define OS05A10_DRVP_IO_PAD_OEN_H_ADDRESS                              (0x3001)
#define OS05A10_DRVP_IO_PAD_OEN_L_ADDRESS                              (0x3002)
#define OS05A10_DRVP_PAD_CTRL_ADDRESS                                  (0x3009)

/* Sensor control registers */
#define OS05A10_DRVP_REG7E_ADDRESS                                     (0x377E)
#define OS05A10_DRVP_REG97_ADDRESS                                     (0x3797)
#define OS05A10_DRVP_REG98_ADDRESS                                     (0x3798)
#define OS05A10_DRVP_REG99_ADDRESS                                     (0x3799)

/* Timing control registers */
#define OS05A10_DRVP_VSYNC_COL_START_H_ADDRESS                         (0x3818)
#define OS05A10_DRVP_VSYNC_COL_START_L_ADDRESS                         (0x3819)
#define OS05A10_DRVP_VSYNC_ROW_START_H_ADDRESS                         (0x381a)
#define OS05A10_DRVP_VSYNC_ROW_START_L_ADDRESS                         (0x381b)
#define OS05A10_DRVP_REG22_ADDRESS                                     (0x3822)
#define OS05A10_DRVP_REG23_ADDRESS                                     (0x3823)
#define OS05A10_DRVP_CS_RST_FSIN_H_ADDRESS                             (0x3824)
#define OS05A10_DRVP_CS_RST_FSIN_L_ADDRESS                             (0x3825)
#define OS05A10_DRVP_R_RST_FSIN_H_ADDRESS                              (0x3826)
#define OS05A10_DRVP_R_RST_FSIN_L_ADDRESS                              (0x3827)
#define OS05A10_DRVP_REG32_ADDRESS                                     (0x3832)
#define OS05A10_DRVP_REG34_ADDRESS                                     (0x3834)
#define OS05A10_DRVP_REG42_ADDRESS                                     (0x3842)

/* Strobe control registers */
#define OS05A10_DRVP_STROBE_CTRL0_ADDRESS                              (0x3b00)
#define OS05A10_DRVP_STROBE_H_ADDRESS                                  (0x3b02)
#define OS05A10_DRVP_STROBE_L_ADDRESS                                  (0x3b03)
#define OS05A10_DRVP_STROBE_CTRL1_ADDRESS                              (0x3b04)
#define OS05A10_DRVP_STROBE_WIDTH_ADDRESS                              (0x3b05)

/* Temperature sensor control */
#define OS05A10_DRVP_TPM_CTRL_12                                       (0x4D12)
#define OS05A10_DRVP_TPM_CTRL_13                                       (0x4D13)
#define OS05A10_DRVP_TPM_CTRL_14                                       (0x4D14)

#define OS05A10_DRVP_AWB_GAIN_B_H_ADDRESS                              (0x5100)
#define OS05A10_DRVP_AWB_GAIN_B_L_ADDRESS                              (0x5101)
#define OS05A10_DRVP_AWB_GAIN_G_H_ADDRESS                              (0x5102)
#define OS05A10_DRVP_AWB_GAIN_G_L_ADDRESS                              (0x5103)
#define OS05A10_DRVP_AWB_GAIN_R_H_ADDRESS                              (0x5104)
#define OS05A10_DRVP_AWB_GAIN_R_L_ADDRESS                              (0x5105)

/* Register "ISP CTRL 1" is used to control the test pattern */
#define OS05A10_DRVP_TEST_PAT_ADDRESS   (0x5081)

///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define OS05A10_DRVP_SC_CHIP_ID                                        (0x530541)
#define OS05A10_DRVP_I2C_DEFAULT_ADDRESS                               (0x42)
#define OS05A10_DRVP_I2C_MASTER_ADDRESS                                (OS05A10_DRVP_I2C_DEFAULT_ADDRESS)
#define OS05A10_DRVP_I2C_SLAVE_ADDRESS                                 (0xA0)
#define OS05A10_DRVP_I2C_GLOBAL_ADDRESS                                (0x42)
#define OS05A10_DRVP_MIN_EXPLINES   (4)
#define TEST_PAT_TYPE_VBAR     (0x0)

#endif /*__OS05A10_REGS_H__*/
