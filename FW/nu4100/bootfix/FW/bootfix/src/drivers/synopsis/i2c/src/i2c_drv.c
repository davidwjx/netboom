
#include "common.h"
/* Inuitive drivers */
#include "gme.h"
#include "i2c_drv.h"
#include "nu_regs.h"
static void i2c_delay(int t)
{
        volatile unsigned count=0;
        while (count++ < t);
}

static UINT32 I2C_READ(UINT32 off) {
        volatile UINT32 c;

        c = *(UINT32 *) (I2C_BASE + off);
        i2c_delay(100);
        return c;
}

static void I2C_WRITE(UINT32 off, UINT32 data) {
        *(UINT32 *) (I2C_BASE + off) = data;
        i2c_delay(100);
}

/*
 * i2c_wait_for_bb - Waits for bus busy
 *
 * Waits for bus busy
 */
static int i2c_wait_for_bb(void) {
        volatile unsigned long start_time_bb = 0;

        while ((I2C_READ(OFF_IC_STATUS) & IC_STATUS_MA)
                        || !(I2C_READ(OFF_IC_STATUS) & IC_STATUS_TFE)) {
                /* Worst case timeout for 1 byte is kept as 2ms */
                if (++start_time_bb > IC_BB_TIMEO)
                        return 1;
        }
        return 0;
}

static int dw_i2c_enable(int enable) {
        UINT32 ena = enable ? IC_ENABLE_0B : 0;
        int timeout = 1000;

        do {
                I2C_WRITE(OFF_IC_ENABLE, ena);
                if ((I2C_READ(OFF_IC_ENA_STATUS) & IC_ENABLE_0B) == ena)
                        return 0;

                /*
                 * Wait 1000 times the signaling period of the highest I2C
                 * transfer supported by the driver (for 400KHz this is
                 * 25us) as described in the DesignWare I2C databook.
                 */
                i2c_delay(25000);

        } while (timeout--);

        rel_log("timeout in %sabling I2C adapter\n", enable ? "en" : "dis");

        return -1;
}


/*
 * i2c_setaddress - Sets the target slave address
 * @i2c_addr:   target i2c address
 *
 * Sets the target slave address.
 */
static void i2c_setaddress(unsigned int i2c_addr) {
        I2C_WRITE(OFF_IC_TAR, i2c_addr);
}

static int i2c_xfer_init(UINT8 chip, UINT32 addr, int alen)
{
        if (i2c_wait_for_bb()) {
                rel_log("i2c_wait_for_bb - Timeout\n");
                return 1;
        }

        dw_i2c_enable(0);
        i2c_setaddress(chip);
        dw_i2c_enable(1);

        while (alen) {
                alen--;
                /* high byte address going out first */
                I2C_WRITE(OFF_IC_CMD, (addr >> (alen * 8)) & 0xff);
        }
        return 0;
}


/*
 * i2c_flush_rxfifo - Flushes the i2c RX FIFO
 *
 * Flushes the i2c RX FIFO
 */
static void i2c_flush_rxfifo(void)
{

        while (I2C_READ(OFF_IC_STATUS) & IC_STATUS_RFNE)
                I2C_READ(OFF_IC_CMD);
}

static int i2c_xfer_finish()
{
        UINT32 start_stop_det = 0;

        rel_log("i2c_xfer_finish\n");

        while (1) {
                if (I2C_READ(OFF_IC_INTR_STAT) & IC_STOP_DET) {
                        I2C_READ(OFF_CLR_STOP_DET);
                        break;
                } else if (++start_stop_det > I2C_STOPDET_TO) {
                        break;
                }
        }

        if (i2c_wait_for_bb()) {
                rel_log("Timed out waiting for bus\n");
                return 1;
        }

        i2c_flush_rxfifo();

        return 0;
}


/*
 * i2c_read - Read from i2c memory
 * @chip:       target i2c address
 * @addr:       address to read from
 * @alen:
 * @buffer:     buffer for read data
 * @len:        no of bytes to be read
 *
 * Read from i2c memory.
 */
static int dw_i2c_read(UINT8 dev, UINT32 addr, int alen, UINT8 *buffer, int len)
{
        volatile unsigned long start_time_rx;
        unsigned int active = 0;

        if (i2c_xfer_init(dev, addr, alen))
        {
                rel_log("i2c_xfer_init failure\n");
                return 1;
        }

        start_time_rx = 0;
        while (len) {
                if (!active) {
                        /*
                         * Avoid writing to ic_cmd_data multiple times
                         * in case this loop spins too quickly and the
                         * ic_status RFNE bit isn't set after the first
                         * write. Subsequent writes to ic_cmd_data can
                         * trigger spurious i2c transfer.
                         */
                                    if (len == 1) {
                                                    I2C_WRITE(OFF_IC_CMD, IC_CMD | IC_STOP);
                                            }
                                            else
                                                    I2C_WRITE(OFF_IC_CMD, IC_CMD);
                                            active = 1;
                                    }

                                    if (I2C_READ(OFF_IC_STATUS) & IC_STATUS_RFNE) {
                                            *buffer++ = (UINT8) I2C_READ(OFF_IC_CMD);
                                            len--;
                                            start_time_rx = 0;
                                            active = 0;
                                    }
                                    else {
                                            if (start_time_rx++ > 100)
                                                    return  1;
                                    }
                            }
                            return i2c_xfer_finish();
                    }


static void i2c_deinit(void) {
        GME_SW_RESET_0_SW_I2C_0_RESET_N_W(0);   //reset to prevent additional interrupts
        GME_SW_RESET_0_SW_I2C_0_RESET_N_W(1);
}


static int dw_i2c_set_bus_speed(void) {

        unsigned ena;

        /* Get enable setting for restore later */
        ena = I2C_READ(OFF_IC_ENABLE) & IC_ENABLE_0B;

        /* to set speed cltr must be disabled */
        dw_i2c_enable(0);

        /* speed fast low */
        I2C_WRITE(OFF_IC_FS_SCL_LCNT, 0x1df);
        /* speed fast high */
        I2C_WRITE(OFF_IC_FS_SCL_HCNT, 0x10b);

        I2C_WRITE(OFF_IC_SS_SCL_LCNT, 0x5db);
        I2C_WRITE(OFF_IC_SS_SCL_HCNT, 0x507);

        I2C_WRITE(OFF_IC_SDA_HOLD, 1);

        if (ena == IC_ENABLE_0B)
                dw_i2c_enable(1);
        return 0;
}

static int i2c_init(unsigned slaveaddr) {
        int ret;

        GME_CLOCK_ENABLE_I2C_0_CLK_EN_W(1);
        GME_SW_RESET_0_SW_I2C_0_RESET_N_W(0);
        GME_SW_RESET_0_SW_I2C_0_RESET_N_W(1);

        /* Disable i2c */
        ret = dw_i2c_enable(0);
        if (ret)
                return ret;

        I2C_WRITE(OFF_IC_CON, 0x80 | IC_CON_SD | IC_CON_RE | IC_CON_SPD_FS | IC_CON_MM);

        I2C_WRITE(OFF_IC_RX_TL, IC_RX_TL);
        I2C_WRITE(OFF_IC_TX_TL, IC_TX_TL);
        I2C_WRITE(OFF_IC_INTR_MASK, 254);
        I2C_WRITE(OFF_IC_HS_MADDR, 1);

        dw_i2c_set_bus_speed();

        /* Enable i2c */
        ret = dw_i2c_enable(1);
        if (ret)
                return ret;

        return 0;
}

int i2c_read_pmic(void) {
    unsigned char buffer;
    if (i2c_init(0x28)) {
        rel_log("bootfix:I2c Initialization failure\n");
        return 1;
    } else
        rel_log("bootfix:I2c_init: OK\n");

    if (dw_i2c_read(0x28, 0x45, 1, &buffer, 1)) {
        rel_log("bootfix:Failure while reading I2c\n");
        return 1;
    } else
        rel_log("bootfix:dw_i2c_read: buffer read: %x\n", (int ) buffer);

    i2c_deinit();

    rel_log("bootfix:value: %d\n", buffer);
    return (buffer & 0x040);
}
