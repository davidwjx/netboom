
#define PHYR_BASE_ADDR 0x0C000000
#define ICCM_BASE_ADDR 0x0C140000
#define DCCM_BASE_ADDR 0x0C150000

#define ddrphy_reg_wr(_addr_, _val_) 		((*(volatile unsigned int *)(_addr_)) = (_val_))

#define dwc_ddrphy_apb_wr(reg, data)		ddrphy_reg_wr(PHYR_BASE_ADDR + (reg*4), (data))
#define dwc_ddrphy_apb_wr_i(reg, data)		ddrphy_reg_wr(ICCM_BASE_ADDR + (reg*4), (data))
#define dwc_ddrphy_apb_wr_d(reg, data)		ddrphy_reg_wr(DCCM_BASE_ADDR + (reg*4), (data))

