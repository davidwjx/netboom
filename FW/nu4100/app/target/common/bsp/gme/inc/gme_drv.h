#ifndef _GME_H_
#define _GME_H_

#include "inu_types.h"
#include "err_defs.h"
#include "io_pal.h"
#include "iae_drv.h"
#define POWER_MODE_OPT

#define GME_DRVG_GPP_CLK_SRC_OSC   (0)
#define GME_DRVG_GPP_CLK_SRC_PLL   (1)


#define GME_DRVG_INTERNAL_MEM_SIZE_BYTES    (2048)

#define GME_DRVG_MAX_FUSE_U32_OFFSET  (127)

typedef enum
{
   GME_DRVP_MODULE_STATUS_DUMMY_E = 0,
   GME_DRVP_MODULE_STATUS_CLOSE_E = 1,
   GME_DRVP_MODULE_STATUS_OPEN_E  = 2,
   GME_DRVP_MODULE_STATUS_LAST_E
} GME_DRVP_moduleStatusE;

typedef enum
{
  GME_DRVG_HW_UNIT_FCU_E,
  GME_DRVG_HW_UNIT_UART0_E,
  GME_DRVG_HW_UNIT_UART1_E,
  GME_DRVG_HW_UNIT_UART2_E,
  GME_DRVG_HW_UNIT_I2C0_E,
  GME_DRVG_HW_UNIT_I2C1_E,
  GME_DRVG_HW_UNIT_I2C2_E,
  GME_DRVG_HW_UNIT_I2C3_E,
  GME_DRVG_HW_UNIT_I2C4_E,
  GME_DRVG_HW_UNIT_I2C5_E,
  GME_DRVG_HW_UNIT_I2C6_E,
  GME_DRVG_HW_UNIT_GPIO_E,
  GME_DRVG_HW_UNIT_OSC_E,
  GME_DRVG_HW_UNIT_USB_E,
  GME_DRVG_HW_UNIT_GPP_E,
  GME_DRVG_HW_UNIT_IAE_E, //14
  GME_DRVG_HW_UNIT_DPE_E, //15
  GME_DRVG_HW_UNIT_CVA_E, //16
  GME_DRVG_HW_UNIT_PPE_E, //17
  GME_DRVG_HW_UNIT_DMA_0_E, //18
  GME_DRVG_HW_UNIT_DMA_1_E, //19
  GME_DRVG_HW_UNIT_DMA_2_E, //20
  GME_DRVG_HW_UNIT_CIIF_E,
  GME_DRVG_HW_UNIT_CIIF_PAR_0_E,
  GME_DRVG_HW_UNIT_CIIF_PAR_1_E,
  GME_DRVG_HW_UNIT_LRAM_E,
  GME_DRVG_HW_CVRS_CLK_EN_E,
  GME_DRVG_HW_CVLS_CLK_EN_E,
  GME_DRVG_HW_CV0_REF_CLK_EN_E,
  GME_DRVG_HW_CV1_REF_CLK_EN_E,
  GME_DRVG_HW_CV2_REF_CLK_EN_E,
  GME_DRVG_HW_CV3_REF_CLK_EN_E,
  GME_DRVG_HW_I2S_CLK_EN_E,
  GME_DRVG_HW_AUDIO_CLK_EN_E,
  GME_DRVG_HW_AUDIO_EXT_CLK_EN_E,
  GME_DRVG_HW_MIPI_DPHY0_RX_CLK_EN_E,
  GME_DRVG_HW_MIPI_DPHY1_RX_CLK_EN_E,
  GME_DRVG_HW_MIPI_DPHY2_RX_CLK_EN_E,
  GME_DRVG_HW_MIPI_DPHY3_RX_CLK_EN_E,
  GME_DRVG_HW_MIPI_DPHY4_RX_CLK_EN_E,
  GME_DRVG_HW_MIPI_DPHY5_RX_CLK_EN_E,
  GME_DRVG_HW_MIPI_DPHY0_TX_CLK_EN_E,
  GME_DRVG_HW_MIPI_DPHY1_TX_CLK_EN_E,
  GME_DRVG_HW_MIPI_DPHY2_TX_CLK_EN_E,
  GME_DRVG_HW_MIPI_DPHY3_TX_CLK_EN_E,
  GME_DRVG_HW_UNIT_CNN_E,
  GME_DRVG_HW_UNIT_DSP_E,
  GME_DRVG_HW_UNIT_ISP_E,
  GME_DRVG_HW_UNIT_MAX_NUM_E,
  GME_DRVG_HW_UNIT_INVALID_E = 0xFF,
} GME_DRVG_hwUnitE;

typedef enum
{
  GME_DRVG_DBG_24MHZ_CLK_E,
  GME_DRVG_DBG_CPU_PLL_CLK_E,
  GME_DRVG_DBG_SYS_PLL_CLK_E,
  GME_DRVG_DBG_DDR_PLL_CLK_E,
  GME_DRVG_DBG_DSP_PLL_CLK_E,
  GME_DRVG_DBG_CPU_CLK_E,
  GME_DRVG_DBG_SYS_CLK_E,
  GME_DRVG_DBG_USB_SYS_CLK_E,
  GME_DRVG_DBG_DDR_CLK_E,
  GME_DRVG_DBG_DSPA_CLK_E,
  GME_DRVG_DBG_IAE_CLK_E,
  GME_DRVG_DBG_DPE_CLK_E,
  GME_DRVG_DBG_PPE_CLK_E,
  GME_DRVG_DBG_CV_0_CLK_E,
  GME_DRVG_DBG_CV_1_CLK_E,
  GME_DRVG_DBG_UART0_BAUDOUT_N_CLK_E,
  GME_DRVG_DBG_UART1_BAUDOUT_N_CLK_E,
  GME_DRVG_DBG_UART2_BAUDOUT_N_CLK_E,
  GME_DRVG_DBG_RELAXATION_OSC_CLK_E
} GME_DRVG_dbgClockE;

typedef enum {
   GME_DRVG_PERIPHERAL_CLK_SRC_OSC_E = 0,
   GME_DRVG_PERIPHERAL_CLK_SRC_USBPHY_E
} GME_DRVG_clkSrcE;

typedef enum {
   GME_DRVG_POWER_PSS_E = 0,
   GME_DRVG_POWER_IAE_E,
   GME_DRVG_POWER_DPE_E,
   GME_DRVG_POWER_PPE_E,
   GME_DRVG_POWER_DSP_E,
   GME_DRVG_POWER_EVP_E,
   GME_DRVG_POWER_ALL_E = 0x3f
} GME_DRVG_powerModeE;
typedef enum {
   GME_DRVG_PLL_AUDIO_E = 0,
   GME_DRVG_PLL_CPU_E,
   GME_DRVG_PLL_DDR_E,
   GME_DRVG_PLL_DSP_E,
   GME_DRVG_PLL_SYS_E,
   GME_DRVG_PLL_USB2_E,
   GME_DRVG_PLL_USB3_E,
} GME_DRVG_PLLTypesE;

typedef enum
{
   GME_DRVG_GPP_SLOW_CLK_DIV1_E = 0,
   GME_DRVG_GPP_SLOW_CLK_DIV2_E,
   GME_DRVG_GPP_FAST_CLK_DIV_E
} GME_DRVG_gppClkDivE;

typedef struct {
   GME_DRVG_hwUnitE unit;
   UINT16 div;
} GME_DRVG_unitClkDivT;

typedef enum {
   GME_DRVG_LEDS_OE_E = 0,
   GME_DRVG_PWR_ON_EXT_DEVICE_OE_E,
   GME_DRVG_STRAP_OE_E,
   GME_DRVG_DEBUG_CLK_MUX_SEL_E
} GME_DRVP_ioPinE;
typedef enum
{
   GME_DRVG_EXTERNAL_EVENTS_0 = 0,
   GME_DRVG_EXTERNAL_EVENTS_1,
   GME_DRVG_EXTERNAL_EVENTS_2,
   GME_DRVG_EXTERNAL_EVENTS_3,
   GME_DRVG_EXTERNAL_EVENTS_4,
   GME_DRVG_EXTERNAL_EVENTS_5,
   GME_DRVG_EXTERNAL_EVENTS_6,
   GME_DRVG_EXTERNAL_EVENTS_7,
   GME_DRVG_EXTERNAL_EVENTS_8,
   GME_DRVG_EXTERNAL_EVENTS_9,
   GME_DRVG_EXTERNAL_EVENTS_10,
   GME_DRVG_EXTERNAL_EVENTS_11,
   GME_DRVG_EXTERNAL_EVENTS_12,
   GME_DRVG_EXTERNAL_EVENTS_13,
   GME_DRVG_EXTERNAL_EVENTS_14,
   GME_DRVG_EXTERNAL_EVENTS_15
} GME_DRVG_externalEventE;

typedef struct {
   UINT32 val;
   GME_DRVP_ioPinE ioSel;
} GME_DRVP_ioControlParamsT;

typedef struct
{
   UINT32 pll_on   :1,
          bypass   :1,
          refdiv   :6,
          fbdiv    :12,
          unused   :4,
          postdiv1 :3,
          postdiv2 :3,
          dac      :1,
          dsm      :1;
} GME_DRVG_pllControlT;

typedef struct
{
   UINT32 pll_on   :1,
          bypass   :1,
          refdiv   :6,
          fbdiv    :12,
          unused   :4,
          postdiv1 :3,
          postdiv2 :3,
          lock     :1,
          dsm      :1;
 } GME_DRVG_pllStatusT;

typedef struct
{
   UINT32 div_en        :1,
          clk_div       :7,
          clk_src_sel   :5,
          unused        :19;
} GME_DRVG_debug_clkControlT;


typedef struct
{
   GME_DRVG_PLLTypesE pllType;
   GME_DRVG_pllControlT pllControl;
} GME_DRVG_pllObjConfigT;


typedef struct
{
   UINT16   versionId;
   UINT16   revesionId;
} GME_DRVG_versionFieldsT;

typedef union
{
   UINT32                  val;
   GME_DRVG_versionFieldsT feild;
} GME_DRVG_getVersionParamsT;

typedef struct
{
   INU_DEFSG_moduleTypeE      chipPinMuxType;
} GME_DRVG_openParamsT;

typedef struct
{
   UINT16               regAddr;
   UINT16               value;
} GME_DRVG_usbPhyRegParamsT;

typedef enum
{
   GME_DRVG_CIIF_SRC_USB3PLL_E = 0,
   GME_DRVG_CIIF_SRC_AUDIOPLL_E = 1,
   GME_DRVG_CIIF_SRC_DSP_E = 2,
   GME_DRVG_CIIF_SRC_MAX_NUM_E = 3
} GME_DRVG_ciifClkSrcE;

typedef struct
{
   unsigned int num;
   UINT32 val;
} GME_DRVG_saveAndRestoreT;

typedef enum
{
   GME_DRVG_MIPI_DPHY_INSTANCE_0_E = 0,
   GME_DRVG_MIPI_DPHY_INSTANCE_1_E = 1,
   GME_DRVG_MIPI_DPHY_INSTANCE_2_E = 2,
   GME_DRVG_MIPI_DPHY_INSTANCE_3_E = 3,
   GME_DRVG_MIPI_DPHY_INSTANCE_4_E = 4,
   GME_DRVG_MIPI_DPHY_INSTANCE_5_E = 5,
} GME_DRVG_mipiDphyInstanceE;

typedef enum
{
   GME_DRVG_MIPI_DPHY_MODE_SLAVE_E = 0,
   GME_DRVG_MIPI_DPHY_MODE_MASTER_E = 1,
   GME_DRVG_MIPI_DPHY_MODE_LAST_E
} GME_DRVG_dphyModeE;

typedef enum
{
   GME_DRVG_MIPI_DPHY_DIRECTION_TX_E = 0,
   GME_DRVG_MIPI_DPHY_DIRECTION_RX_E = 1,
   GME_DRVG_MIPI_DPHY_DIRECTION_LAST_E
} GME_DRVG_dphyDirectionE;

typedef struct {
   GME_DRVG_mipiDphyInstanceE instance;
   GME_DRVG_dphyModeE mode;
} GME_DRVG_dphyModeCfgT;

typedef struct {
   GME_DRVG_mipiDphyInstanceE instance;
   GME_DRVG_dphyDirectionE direction;
} GME_DRVG_dphyDirectionCfgT;

//////////////////////////////////////////////////////////////////
//connection IOCTL commands
/////////////////////////////////////////////////////////////////
/*typedef enum
{
   GME_DRVG_SHOW_PINMUX_CFG_IOCTL_E = 0,
   GME_DRVG_SET_POWER_ACTIVE_IOCTL_E,
   GME_DRVG_SET_IO_CONTROL_IOCTL_E,
   GME_DRVG_SET_POWER_SUSPEND_IOCTL_E,
   GME_DRVG_GET_SAVE_AND_RESTORE_IOCTL_E,
   GME_DRVG_SET_SAVE_AND_RESTORE_IOCTL_E,
   GME_DRVG_IS_POWER_ON_RESET_IOCTL_E,
   GME_DRVG_IS_WATCHDOG_RESET_IOCTL_E,
   GME_DRVG_SET_POWER_MODE_IOCTL_E,
   GME_DRVG_GET_HW_POWER_MODE_IOCTL_E,
   GME_DRVG_CONFIG_SYS_PLL_IOCTL_E,
   GME_DRVG_LOCK_SYS_PLL_IOCTL_E,
   GME_DRVG_IS_SYS_PLL_ENABLED_IOCTL_E,
   GME_DRVG_IS_HIE_ENABLED_IOCTL_E,
   GME_DRVG_GET_SYS_PLL_CONFIG_IOCTL_E,
   GME_DRVG_SET_GPP_CLOCK_SRC_IOCTL_E,
   GME_DRVG_GET_GPP_CLK_SRC_IOCTL_E,
   GME_DRVG_SET_GPP_CLOC_KDIV_IOCTL_E,
   GME_DRVG_GET_GPP_CLOCK_DIV_IOCTL_E,
   GME_DRVG_SET_PERIPH_CLOCK_SRC_IOCTL_E,
   GME_DRVG_GET_PERIPH_CLOCK_SRC_IOCTL_E,
   GME_DRVG_SET_UNIT_CLOCK_DIV_IOCTL_E,
   GME_DRVG_GET_UNIT_CLOCK_DIV_IOCTL_E,
   GME_DRVG_CHANGE_UNIT_FREQ_IOCTL_E,
   GME_DRVG_DISABLE_CLK_IOCTL_E,
   GME_DRVG_ENABLE_CLK_IOCTL_E,
   GME_DRVG_RESET_UNIT_IOCTL_E,
   GME_DRVG_GET_VERSION_IOCTL_E,
   GME_DRVG_GET_FUSE32_IOCTL_E,
   GME_DRVG_SET_IO_MUX_IOCTL_E,
   GME_DRVG_GET_RESUME_ADDR_IOCTL_E,
   GME_DRVG_SET_RESUME_ADDR_IOCTL_E,
   GME_DRVG_WRITE_REG_IOCTL_E,
   GME_DRVG_READ_REG_IOCTL_E,
   GME_DRVG_RECOVERY_MEM_INIT_IOCTL_E,
   GME_DRVG_RECOVERY_MEM_LOAD_IOCTL_E,
   GME_DRVG_RECOVERY_MEM_STORE_IOCTL_E,
   GME_DRVG_DISABLE_SUSPEND_TIMER_IOCTL_E,
   GME_DRVG_SET_DO_SRC_CHG_ON_SUSPEND_IOCTL_E,
   GME_DRVG_SET_USB_PHY_REG_IOCTL_E,
   GME_DRVG_GET_USB_PHY_REG_IOCTL_E,
   GME_DRVG_SET_CIIF_CLK_SRC_IOCTL_E,
   GME_DRVG_GET_CIIF_CLK_IOCTL_E,
   GME_DRVG_SET_MIPI_MASTER_SLAVE_CFG_IOCTL_E,
   GME_DRVG_SET_MIPI_DIRECTION_CFG_IOCTL_E,
   GME_DRVG_SET_UART_CLK_SRC_IOCTL_E,
   GME_DRVG_NUM_OF_IOCTLS_E
} GME_DRVG_ioctlListE;

typedef enum
{
   GME_DRVG_SHOW_PINMUX_CFG_CMD_E            =  GME_DRVG_SHOW_PINMUX_CFG_IOCTL_E             | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_POWER_ACTIVE_CMD_E           =  GME_DRVG_SET_POWER_ACTIVE_IOCTL_E            | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_IO_CONTROL_CMD_E             =  GME_DRVG_SET_IO_CONTROL_IOCTL_E              | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_POWER_SUSPEND_CMD_E          =  GME_DRVG_SET_POWER_SUSPEND_IOCTL_E           | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_SAVE_AND_RESTORE_CMD_E       =  GME_DRVG_GET_SAVE_AND_RESTORE_IOCTL_E        | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_SAVE_AND_RESTORE_CMD_E       =  GME_DRVG_SET_SAVE_AND_RESTORE_IOCTL_E        | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_IS_POWER_ON_RESET_CMD_E          =  GME_DRVG_IS_POWER_ON_RESET_IOCTL_E           | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_IS_WATCHDOG_RESET_CMD_E          =  GME_DRVG_IS_WATCHDOG_RESET_IOCTL_E           | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_POWER_MODE_CMD_E             =  GME_DRVG_SET_POWER_MODE_IOCTL_E              | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_HW_POWER_MODE_CMD_E          =  GME_DRVG_GET_HW_POWER_MODE_IOCTL_E           | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_CONFIG_SYS_PLL_CMD_E             =  GME_DRVG_CONFIG_SYS_PLL_IOCTL_E              | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_LOCK_SYS_PLL_CMD_E               =  GME_DRVG_LOCK_SYS_PLL_IOCTL_E                | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_IS_SYS_PLL_ENABLED_CMD_E         =  GME_DRVG_IS_SYS_PLL_ENABLED_IOCTL_E          | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_IS_HIE_ENABLED_CMD_E             =  GME_DRVG_IS_HIE_ENABLED_IOCTL_E              | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_SYS_PLL_CONFIG_CMD_E         =  GME_DRVG_GET_SYS_PLL_CONFIG_IOCTL_E          | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_GPP_CLOCK_SRC_CMD_E          =  GME_DRVG_SET_GPP_CLOCK_SRC_IOCTL_E           | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_GPP_CLK_SRC_CMD_E            =  GME_DRVG_GET_GPP_CLK_SRC_IOCTL_E             | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_GPP_CLOC_KDIV_CMD_E          =  GME_DRVG_SET_GPP_CLOC_KDIV_IOCTL_E           | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_GPP_CLOCK_DIV_CMD_E          =  GME_DRVG_GET_GPP_CLOCK_DIV_IOCTL_E           | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_PERIPH_CLOCK_SRC_CMD_E       =  GME_DRVG_SET_PERIPH_CLOCK_SRC_IOCTL_E        | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_PERIPH_CLOCK_SRC_CMD_E       =  GME_DRVG_GET_PERIPH_CLOCK_SRC_IOCTL_E        | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_UNIT_CLOCK_DIV_CMD_E         =  GME_DRVG_SET_UNIT_CLOCK_DIV_IOCTL_E          | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_UNIT_CLOCK_DIV_CMD_E         =  GME_DRVG_GET_UNIT_CLOCK_DIV_IOCTL_E          | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_CHANGE_UNIT_FREQ_CMD_E           =  GME_DRVG_CHANGE_UNIT_FREQ_IOCTL_E            | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_DISABLE_CLK_CMD_E                =  GME_DRVG_DISABLE_CLK_IOCTL_E                 | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_ENABLE_CLK_CMD_E                 =  GME_DRVG_ENABLE_CLK_IOCTL_E                  | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_RESET_UNIT_CMD_E                 =  GME_DRVG_RESET_UNIT_IOCTL_E                  | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_VERSION_CMD_E                =  GME_DRVG_GET_VERSION_IOCTL_E                 | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_FUSE32_CMD_E                 =  GME_DRVG_GET_FUSE32_IOCTL_E                  | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_IO_MUX_CMD_E                 =  GME_DRVG_SET_IO_MUX_IOCTL_E                  | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_RESUME_ADDR_CMD_E            =  GME_DRVG_GET_RESUME_ADDR_IOCTL_E             | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_RESUME_ADDR_CMD_E            =  GME_DRVG_SET_RESUME_ADDR_IOCTL_E             | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_WRITE_REG_CMD_E                  =  GME_DRVG_WRITE_REG_IOCTL_E                   | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_READ_REG_CMD_E                   =  GME_DRVG_READ_REG_IOCTL_E                    | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_RECOVERY_MEM_INIT_CMD_E          =  GME_DRVG_RECOVERY_MEM_INIT_IOCTL_E           | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_RECOVERY_MEM_LOAD_CMD_E          =  GME_DRVG_RECOVERY_MEM_LOAD_IOCTL_E           | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_RECOVERY_MEM_STORE_CMD_E         =  GME_DRVG_RECOVERY_MEM_STORE_IOCTL_E          | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_DISABLE_SUSPEND_TIMER_CMD_E      =  GME_DRVG_DISABLE_SUSPEND_TIMER_IOCTL_E       | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_DO_SRC_CHG_ON_SUSPEND_CMD_E  =  GME_DRVG_SET_DO_SRC_CHG_ON_SUSPEND_IOCTL_E   | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_USB_PHY_REG_CMD_E            =  GME_DRVG_SET_USB_PHY_REG_IOCTL_E             | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_USB_PHY_REG_CMD_E            =  GME_DRVG_GET_USB_PHY_REG_IOCTL_E             | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_CIIF_CLK_SRC_CMD_E           =  GME_DRVG_SET_CIIF_CLK_SRC_IOCTL_E            | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_GET_CIIF_CLK_CMD_E               =  GME_DRVG_GET_CIIF_CLK_IOCTL_E            | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_MIPI_MASTER_SLAVE_CFG_CMD_E  =  GME_DRVG_SET_MIPI_MASTER_SLAVE_CFG_IOCTL_E   | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_MIPI_DIRECTION_CFG_CMD_E     =  GME_DRVG_SET_MIPI_DIRECTION_CFG_IOCTL_E      | IO_CMD_MASK(IO_GME_E),
   GME_DRVG_SET_UART_CLK_SRC_CMD_E           =  GME_DRVG_SET_UART_CLK_SRC_IOCTL_E            | IO_CMD_MASK(IO_GME_E),

} GME_DRVG_ioctlCmdE;*/
typedef struct {
   UINT32   offsetAddress;
   UINT32   val;
} GME_DRVG_writeRegT;

typedef enum
{
   PVT_PROC = 0 ,        //PVT_PROCESS_MEASUREMWENT
   PVT_VOLT = 1 ,        //PVT_VOLTAGE_MEASUREMENT
   PVT_TEMP = 2 ,        //PVT_TEMPRETURE_MEASUREMENT
}GME_DRVG_pvtMeasurementTypeT;

typedef struct
{
   UINT32 on;
   UINT32 bypass;
   UINT32 refdiv;
   UINT32 fbdiv;
   UINT32 postdiv1;
   UINT32 postdiv2;
   UINT32 dacpd;
   UINT32 dsmpd;
   UINT32 frac;
} GME_DRVG_pllConfigT;

typedef enum
{
   FSG_BLOCK        =0,
   TRIGGER_MANAGER = 1,
   
}GME_DRVG_triggerMode;

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE GME_DRVG_changeUnitFreq(GME_DRVG_unitClkDivT * params);
ERRG_codeE GME_DRVG_init(UINT32 memVirtAddr);
float       GME_DRVG_getPvtMeasure( UINT8 sensorNum, GME_DRVG_pvtMeasurementTypeT measureMode );
ERRG_codeE GME_DRVG_enablePvt( void );


typedef enum
{
   GMEG_DPHY_LANE_0 = 0,
   GMEG_DPHY_LANE_1,
   GMEG_DPHY_LANE_CLK,
} GMEG_dphyLaneE;

#define GMEG_MIPI_DPHY_DIR_RX  (1)
#define GMEG_MIPI_DPHY_DIR_TX  (0)
#define GMEG_DPHY_CONFIG_MASTER   (1)
#define GMEG_DPHY_CONFIG_SLAVE    (0)

//Mipi related functions
void GMEG_mipiDphyEnable(INU_DEFSG_mipiInstE inst);
void GMEG_mipiDphyDisable(INU_DEFSG_mipiInstE inst);
void GMEG_mipiDphyTxConfig(INU_DEFSG_mipiInstE inst, unsigned int videoFormat);
UINT32 GMEG_getMipiDphyStatus(INU_DEFSG_mipiInstE inst);
void GMEG_showMipiDphyConfig(INU_DEFSG_mipiInstE inst);

ERRG_codeE GME_DRVG_setPowerMode(GME_DRVG_powerModeE powerMode);
ERRG_codeE GME_DRVG_getHwPowerMode(void);
ERRG_codeE GME_DRVG_ioControl(GME_DRVP_ioControlParamsT *params);

void GMEG_showMipiRegs(void);
unsigned int GME_DRVG_isSysPllEnabled();
int  GME_DRVG_isHieEnabled();
void GME_DRVG_getSysPllConfig(GME_DRVG_pllObjConfigT *pllObjConfig);
void GME_DRVG_setGppClockSrc(int clkSrc);
int  GME_DRVG_getGppClkSrc(void);
void GME_DRVG_setGppClockDiv(GME_DRVG_gppClkDivE sel, UINT16 div);
void GME_DRVG_getGppClockDiv(GME_DRVG_gppClkDivE sel, UINT16 *divP);
unsigned int GME_DRVG_calc_pll(GME_DRVG_PLLTypesE pllType);
/**
 * @brief Modifies the pin muxing for the PPS host pin
 * 
 *
 * @param value This value can be between 0->7
 */
void GME_SetPPSHostPinMuxing(UINT8 value);
int GME_GetPPSHOSTmux();
ERRG_codeE GME_DRVG_setUnitClockDiv(GME_DRVG_unitClkDivT *params);
ERRG_codeE GME_DRVG_getUnitClockDiv(GME_DRVG_unitClkDivT *paramsP);
ERRG_codeE GME_DRVG_disableClk(GME_DRVG_hwUnitE unit);
ERRG_codeE GME_DRVG_enableClk(GME_DRVG_hwUnitE unit);
ERRG_codeE GME_DRVG_resetUnit(GME_DRVG_hwUnitE unit);
ERRG_codeE GME_DRVG_getVersion(GME_DRVG_getVersionParamsT *getVersionParamsP);
ERRG_codeE GME_DRVG_setUartClkSrc(int clkSrc);
/* @brief Changes the IO_CTRL for UART0_SIN to be a GPIO */
void GME_DRVG_switchUART0SinToGPIOIn();
/* @brief Changes the IO_CTRL for UART0_SOUT to be a GPIO */
void GME_DRVG_switchUART0SoutToGPIOIn();
void GME_DRVG_switchWAKEUPToGPIOIn();

void GME_DRVG_setIoMux(GME_DRVG_hwUnitE unit);
void GME_DRVG_setCiifClkSrc(GME_DRVG_ciifClkSrcE src);
GME_DRVG_ciifClkSrcE GME_DRVG_getCiifClkSrc();
ERRG_codeE GME_DRVG_getUsbPhyReg(GME_DRVG_usbPhyRegParamsT *usbPhyRegParamsP);
ERRG_codeE GME_DRVG_setUsbPhyReg(GME_DRVG_usbPhyRegParamsT *usbPhyRegParamsP);
ERRG_codeE GME_DRVG_enable_usb3_pll( void );
ERRG_codeE GME_DRVG_updateCpuPll( void );

ERRG_codeE GME_DRVG_dumpRegs(void);

/**
 * @brief Modifies the trigger mode for fsgCounter to be either managed by the FSG {Plus sensor sync API} or trigger manager
 * 
 * @param fsgCounter FSG Counter to modify
 * @param trigMode Enum describing which mode to operate in
 */
void GME_DRVG_modifyFTRIGMode(IAE_DRVG_fsgCounterNumE fsgCounter,GME_DRVG_triggerMode trigMode);
/*
 * @brief Switches GIO Pins[0:15] to be in GIO mode using the GME IO Muxing registers
 * 
 * @param extEvent GIO pin (Only GIO pins 0->15 have external events enabled)
 */
void GME_DRVG_switchToGPIOForExternalEvents(const GME_DRVG_externalEventE extEvent);
/*
*  @brief Enables all FTRIG output enables, FTRIG3 is by default not enabled.
*/
void GME_DRVG_enableAllFTrigIOOutputEnables();
void GME_DRVG_configAudioPll(GME_DRVG_pllConfigT *pllP);
void GME_DRVG_savePinMuxSetup(void);
void GME_DRVG_restorePinMuxSetup(void);
ERRG_codeE GME_DRVG_writeReg(GME_DRVG_writeRegT *params);
UINT32 GME_DRVG_readReg(UINT32 addrOffset);
void GME_DRVG_getCiifClk(UINT32 *ciifClkHzP);
UINT32 GME_DRVG_readPllFraq(GME_DRVG_PLLTypesE pllType,UINT32 *freq,UINT32 *fraq);
UINT32 GME_DRVG_getSr7(void);
void GME_DRVG_shutterSeq();
//void GME_DRVG_backFromShutterSeq();

void GME_DRVG_iaeCoreReset();
#ifdef DEFSG_EXPERIMENTAL_BLANKING_SLEEP
void gme_enable_iae(void);
void gme_disable_iae(void);
void gme_enable_dpe(void);
void gme_disable_dpe(void);
UINT32 gme_get_sr6(void);


#endif

UINT32 GME_DRVG_getFuse32(UINT16 offset);
UINT8 GME_DRVG_getFuseSecurityControl();

#endif
