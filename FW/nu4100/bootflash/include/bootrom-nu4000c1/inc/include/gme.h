#ifndef _GME_H_
#define _GME_H_ 

#define GMEG_CPU_CLK_SRC_OSC   (0)
#define GMEG_CPU_CLK_SRC_PLL   (1)

#define GMEG_SYS_CLK_SRC_OSC   (0)
#define GMEG_SYS_CLK_SRC_PLL   (1)

#define GMEG_UART_CLK_SRC_OSC  (0)
#define GMEG_UART_CLK_SRC_PLL  (1)

#define GMEG_LRAM_CLK_SRC_OSC  (0)
#define GMEG_LRAM_CLK_SRC_PLL  (1)


#define GMEG_USBPHY_LANE0_RX_OVRD_IN_HI_ADDR     (0x1006)
#define GMEG_USBPHY_LANE1_RX_OVRD_IN_HI_ADDR     (0x1106)
#define GMEG_USBPHY_RX_OVRD_IN_HI_VAL            (0x0A80)

typedef enum
{
  GMEG_HW_UNIT_UART0_E,
  GMEG_HW_UNIT_UART1_E,
  GMEG_HW_UNIT_TIMERS_E,
  GMEG_HW_UNIT_SPI_E,
  GMEG_HW_UNIT_USB_E,
  GMEG_HW_UNIT_USB_SYS_E,
  GMEG_HW_UNIT_GPP_E,
  GMEG_HW_UNIT_SYS_E,
  GMEG_HW_UNIT_LRAM_E,
} GMEG_hwUnitE;

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
} GMEG_pllConfigT;

typedef struct
{
   UINT32 on;
   UINT32 bypass;
   UINT32 refdiv;
   UINT32 fbdiv;
   UINT32 postdiv1;
   UINT32 postdiv2;
   UINT32 lock;
} GMEG_pllStatusT;

//GME status
int GMEG_isPowerOnReset(void);

void GMEG_setSuspendMode(int mode);
void GMEG_resetPowerState(void);

//PLLs
void GMEG_configSysPll(const GMEG_pllConfigT *configP);
void GMEG_lockSysPll(void);
void GMEG_getSysPllStatus(GMEG_pllStatusT *configP);

void GMEG_configCpuPll(const GMEG_pllConfigT *configP);
void GMEG_lockCpuPll(void);
void GMEG_getCpuPllStatus(GMEG_pllStatusT *configP);

void GMEG_configDspPll(const GMEG_pllConfigT *configP);
void GMEG_lockDspPll(void);
void GMEG_getDspPllStatus(GMEG_pllStatusT *configP);

void GMEG_configPllLockCnt(unsigned int select, unsigned int lockTime);

int GMEG_isSysPllEnabled();
int GMEG_isCpuPllEnabled();
int GMEG_isDspPllEnabled();
int GMEG_isHieEnabled();

void GMEG_setGppClockSrc(UINT16 src);
void GMEG_setGppClockDiv(UINT16 div);

void GMEG_setSysClockSrc(UINT16 src);
void GMEG_setSysClockDiv(UINT16 div);

void GMEG_setLramClockSrc(UINT16 clkSrc);

void GMEG_setUsbClockDiv(UINT16 div);

void GMEG_setUartClockSrc(GMEG_hwUnitE uart, UINT16 clkSrc);
void GMEG_setUartClockDiv(GMEG_hwUnitE uart, UINT16 div);

void GMEG_setFcuClockDiv(UINT16 div);

void GMEG_changeUnitFreq(GMEG_hwUnitE unit);

//Clock gating
int GMEG_disableClk(GMEG_hwUnitE unit);
int GMEG_enableClk(GMEG_hwUnitE unit);

//Version and ID read
UINT32 GMEG_getVersion(void);

//Save and Restore Values
UINT32 GMEG_getResumeAddr(void);
void GMEG_setResumeAddr(UINT32 addr);

//Strap-pin values
int GMEG_getBootMode(void);
int GMEG_isUartDebug(void);
int GMEG_isTypecInvert(void);
//Test mode
int GMEG_isTestMode(void);

//General-purpose read/write GME register
void GMEG_writeReg(UINT32 addrOffset, UINT32 val);
UINT32 GMEG_readReg(UINT32 addrOffset);
void GMEG_writeSaveAndRestoreReg(UINT32 id, UINT32 val);
UINT32 GMEG_readSaveAndRestoreReg(UINT32 id);

//Misc functions
void GMEG_disableSuspendTimer(void);
void GMEG_setDoSrcChgOnSuspend(BOOL doChange);
void GMEG_phyBridgeWaitForAckFall( void );
void GMEG_phyBridgeWaitForAckRise( void );
void GMEG_phyBridgeCapAddr(UINT16 phyRegAddr);
void GMEG_setUsbPhyReg( UINT16 regAddr, UINT16 value );
void GMEG_initUsbPhyParam(void);
void GMEG_setUartMux(GMEG_hwUnitE unit);

#endif
