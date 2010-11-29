/*
 *  lis3lv02d.h - ST LIS3LV02DL accelerometer driver
 * 
 * The actual chip is STMicroelectronics LIS3LV02DL or LIS3LV02DQ that seems to
 * be connected via SPI. There exists also several similar chips (such as LIS302DL or
 * LIS3L02DQ) but not in the HP laptops and they have slightly different registers.
 * They can also be connected via I2C.
 */

#define LIS3LV02DL_ID	0x3A /* Also the LIS3LV02DQ */
#define LIS302DL_ID		0x3B /* Also the LIS202DL! */

enum lis3lv02d_reg {
	WHO_AM_I	= 0x0F,
	OFFSET_X	= 0x16,
	OFFSET_Y	= 0x17,
	OFFSET_Z	= 0x18,
	GAIN_X		= 0x19,
	GAIN_Y		= 0x1A,
	GAIN_Z		= 0x1B,
	CTRL_REG1	= 0x20,
	CTRL_REG2	= 0x21,
	CTRL_REG3	= 0x22,
	HP_FILTER_RESET	= 0x23,
	STATUS_REG	= 0x27,
	OUTX_L		= 0x28,
	OUTX_H		= 0x29,
	OUTY_L		= 0x2A,
	OUTY_H		= 0x2B,
	OUTZ_L		= 0x2C,
	OUTZ_H		= 0x2D,
	/*LIS3LV02DL_ID */
	FF_WU_CFG	= 0x30,
	FF_WU_SRC	= 0x31,
	FF_WU_ACK	= 0x32,
	FF_WU_THS_L	= 0x34,
	FF_WU_THS_H	= 0x35,
	FF_WU_DURATION	= 0x36,
	DD_CFG		= 0x38,
	DD_SRC		= 0x39,
	DD_ACK		= 0x3A,
	DD_THSI_L	= 0x3C,
	DD_THSI_H	= 0x3D,
	DD_THSE_L	= 0x3E,
	DD_THSE_H	= 0x3F,
	/*LIS302DL_ID */
	FF_WU_CFG1 = 0x30,
	FF_WU_SRC1 = 0x31,
	FF_WU_THS1 = 0x32,
	FF_WU_DURATION1 = 0x33,
	FF_WU_CFG2 = 0x34,
	FF_WU_SRC2 = 0x35,
	FF_WU_THS2 = 0x36,
	FF_WU_DURATION2 = 0x37,
	CLICK_CFG = 0x38,
	CLICK_SRC = 0x39,
	CLICK_THSY_X = 0x3B,
	CLICK_THSZ = 0x3C,
	CLICK_TIMELIMIT = 0x3D,
	CLICK_LANTENCY = 0x3E,
	CLICK_WINDOW = 0x3F,
};

enum lis3lv02d_ctrl1 {
	CTRL1_Xen	= 0x01,
	CTRL1_Yen	= 0x02,
	CTRL1_Zen	= 0x04,
	CTRL1_ST	= 0x08,
	CTRL1_DF0	= 0x10,
	CTRL1_DF1	= 0x20,
	CTRL1_PD0	= 0x40,
	CTRL1_PD1	= 0x80,
};
enum lis3lv02d_ctrl2 {
	CTRL2_DAS	= 0x01,
	CTRL2_SIM	= 0x02,
	CTRL2_DRDY	= 0x04,
	CTRL2_IEN	= 0x08,
	CTRL2_BOOT	= 0x10,
	CTRL2_BLE	= 0x20,
	CTRL2_BDU	= 0x40, /* Block Data Update */
	CTRL2_FS	= 0x80, /* Full Scale selection */
};


enum lis3lv02d_ctrl3 {
	CTRL3_CFS0	= 0x01,
	CTRL3_CFS1	= 0x02,
	CTRL3_FDS	= 0x10,
	CTRL3_HPFF	= 0x20,
	CTRL3_HPDD	= 0x40,
	CTRL3_ECK	= 0x80,
};

enum lis3lv02d_status_reg {
	STATUS_XDA	= 0x01,
	STATUS_YDA	= 0x02,
	STATUS_ZDA	= 0x04,
	STATUS_XYZDA	= 0x08,
	STATUS_XOR	= 0x10,
	STATUS_YOR	= 0x20,
	STATUS_ZOR	= 0x40,
	STATUS_XYZOR	= 0x80,
};

enum lis3lv02d_ff_wu_cfg {
	FF_WU_CFG_XLIE	= 0x01,
	FF_WU_CFG_XHIE	= 0x02,
	FF_WU_CFG_YLIE	= 0x04,
	FF_WU_CFG_YHIE	= 0x08,
	FF_WU_CFG_ZLIE	= 0x10,
	FF_WU_CFG_ZHIE	= 0x20,
	FF_WU_CFG_LIR	= 0x40,
	FF_WU_CFG_AOI	= 0x80,
};

enum lis3lv02d_ff_wu_src {
	FF_WU_SRC_XL	= 0x01,
	FF_WU_SRC_XH	= 0x02,
	FF_WU_SRC_YL	= 0x04,
	FF_WU_SRC_YH	= 0x08,
	FF_WU_SRC_ZL	= 0x10,
	FF_WU_SRC_ZH	= 0x20,
	FF_WU_SRC_IA	= 0x40,
};

enum lis3lv02d_dd_cfg {
	DD_CFG_XLIE	= 0x01,
	DD_CFG_XHIE	= 0x02,
	DD_CFG_YLIE	= 0x04,
	DD_CFG_YHIE	= 0x08,
	DD_CFG_ZLIE	= 0x10,
	DD_CFG_ZHIE	= 0x20,
	DD_CFG_LIR	= 0x40,
	DD_CFG_IEND	= 0x80,
};

enum lis3lv02d_dd_src {
	DD_SRC_XL	= 0x01,
	DD_SRC_XH	= 0x02,
	DD_SRC_YL	= 0x04,
	DD_SRC_YH	= 0x08,
	DD_SRC_ZL	= 0x10,
	DD_SRC_ZH	= 0x20,
	DD_SRC_IA	= 0x40,
};

#define LIS302DL_DRV_NAME "st-lis302dl"
#define LIS302DL_SIZE 0x40

#if defined(CONFIG_MACH_MEIZU_M8_3G) ||	defined(CONFIG_MACH_SMDK6410)	
  #define INT1_PIN  S5PC1XX_GPH1(0)
  #define INT2_PIN  S5PC1XX_GPH3(5)
  #define GPIO_FUN_INT  S3C_GPIO_SFN(2)
#endif 

#if defined(CONFIG_MACH_MEIZU_M8_3G) ||	defined(CONFIG_MACH_SMDK6410)
  #define INT1_PIN  S3C64XX_GPL(13)
  #define INT2_PIN  S3C64XX_GPM(0)
  #define GPIO_FUN_INT  S3C_GPIO_SFN(3)
#endif

static struct workqueue_struct *lis302dl_wq;

struct lis302dl_data {
	uint16_t addr;
	unsigned int irq1;
	unsigned int irq2;
	struct work_struct  work1;
	struct work_struct  work2;
	uint32_t flags;
	int (*power)(int on);
	struct i2c_client *client;
	unsigned char x;
	unsigned char y;
	unsigned char z;
};

