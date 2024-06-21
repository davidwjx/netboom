#ifndef _MIPI_H_
#define _MIPI_H_

#include "mipi_drv.h"

#define IAE_APB_BASE_ADDRESS     (MIPI_DRVG_getIaeVirtBase())
/* CSI Rx Controllers */
#define DPHY_0_BASE_ADDRESS      (IAE_APB_BASE_ADDRESS+0x8000)
#define DPHY_1_BASE_ADDRESS      (IAE_APB_BASE_ADDRESS+0x9000)
#define DPHY_2_BASE_ADDRESS      (IAE_APB_BASE_ADDRESS+0xA000)
#define DPHY_3_BASE_ADDRESS      (IAE_APB_BASE_ADDRESS+0xB000)
#define DPHY_4_BASE_ADDRESS      (IAE_APB_BASE_ADDRESS+0xC000)
#define DPHY_5_BASE_ADDRESS      (IAE_APB_BASE_ADDRESS+0xD000)

#define PPE_APB_BASE_ADDRESS     (MIPI_DRVG_getPpeVirtBase())
/* CSI Tx Controllers */
#define TX_0_BASE_ADDRESS        (PPE_APB_BASE_ADDRESS+0xE000)
#define TX_1_BASE_ADDRESS        (PPE_APB_BASE_ADDRESS+0xE100)
#define TX_2_BASE_ADDRESS        (PPE_APB_BASE_ADDRESS+0xE200)
/* DSI Tx Controllers */
#define DSI_0_BASE_ADDRESS       (PPE_APB_BASE_ADDRESS+0xE400)
#define DSI_1_BASE_ADDRESS       (PPE_APB_BASE_ADDRESS+0xE800)
#define DSI_2_BASE_ADDRESS       (PPE_APB_BASE_ADDRESS+0xEC00)
#define DSI_3_BASE_ADDRESS       (PPE_APB_BASE_ADDRESS+0xEE00)


#include "mipi_csi2_dw.h"
#include "mipi_csi2_tx_svt.h"
#include "gme_drv.h"
#include "iae_drv.h"

#endif //_MIPI_H_
