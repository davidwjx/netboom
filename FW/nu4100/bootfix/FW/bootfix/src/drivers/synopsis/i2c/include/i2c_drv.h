/****************************************************************************
 *
 *   FileName: i2c_drv.h
 *
 *   Author: Giyora Arbel.
 *
 *   Date:
 *
 *   Description: I2C driver wrapper
 *
 ****************************************************************************/
#ifndef I2C_DRV_H
#define I2C_DRV_H

#define I2C_BASE                                0x08050000   /* Bus 0 */

#define OFF_IC_CON                              0
#define OFF_IC_TAR                              0x4
#define OFF_IC_SAR                              0x8
#define OFF_IC_HS_MADDR                         0xc
#define OFF_IC_SS_SCL_HCNT                      0x14
#define OFF_IC_SS_SCL_LCNT                      0x18
#define OFF_IC_FS_SCL_HCNT                      0x1c
#define OFF_IC_FS_SCL_LCNT                      0x20
#define OFF_IC_HS_SCL_HCNT                      0x24
#define OFF_IC_HS_SCL_LCNT                      0x28
#define OFF_IC_INTR_MASK                        0x30
#define OFF_IC_INTR_STAT                        0x34
#define OFF_IC_RX_TL                            0x38
#define OFF_IC_TX_TL                            0x3c
#define OFF_IC_CLR_INT                          0x40
#define OFF_CLR_STOP_DET                        0x60
#define OFF_IC_ENABLE                           0x6c
#define OFF_IC_CMD                                      0x10
#define OFF_IC_STATUS                           0x70
#define OFF_IC_SDA_HOLD                         0x7c
#define OFF_IC_ENA_STATUS                       0x9c

/* i2c enable register definitions */
#define IC_ENABLE_0B            0x0001

/* i2c status register  definitions */
#define IC_STATUS_SA            0x0040
#define IC_STATUS_MA            0x0020
#define IC_STATUS_RFF           0x0010
#define IC_STATUS_RFNE          0x0008
#define IC_STATUS_TFE           0x0004
#define IC_STATUS_TFNF          0x0002
#define IC_STATUS_ACT           0x0001

/* i2c data buffer and command register definitions */
#define IC_CMD                  0x0100
#define IC_STOP                 0x0200

/* fifo threshold register definitions */
#define IC_TL0                  0x00
#define IC_TL1                  0x01
#define IC_TL2                  0x02
#define IC_TL3                  0x03
#define IC_TL4                  0x04
#define IC_TL5                  0x05
#define IC_TL6                  0x06
#define IC_TL7                  0x07
#define IC_RX_TL                IC_TL0
#define IC_TX_TL                IC_TL0

/* i2c interrupt status register definitions */
#define IC_GEN_CALL             0x0800
#define IC_START_DET    0x0400
#define IC_STOP_DET             0x0200
#define IC_ACTIVITY             0x0100
#define IC_RX_DONE              0x0080
#define IC_TX_ABRT              0x0040
#define IC_RD_REQ               0x0020
#define IC_TX_EMPTY             0x0010
#define IC_TX_OVER              0x0008
#define IC_RX_FULL              0x0004
#define IC_RX_OVER              0x0002
#define IC_RX_UNDER             0x0001

/* i2c control register definitions */
#define IC_CON_SD                               0x0040
#define IC_CON_RE                               0x0020
#define IC_CON_10BITADDRMASTER  0x0010
#define IC_CON_10BITADDR_SLAVE  0x0008
#define IC_CON_SPD_MSK                  0x0006
#define IC_CON_SPD_SS                   0x0002
#define IC_CON_SPD_FS                   0x0004
#define IC_CON_SPD_HS                   0x0006
#define IC_CON_MM                               0x0001

#define IC_BB_TIMEO                             200000
#define I2C_BYTE_TO                             200000
#define I2C_STOPDET_TO                  10000


int i2c_read_pmic(void);

#endif //I2C_DRV_H
