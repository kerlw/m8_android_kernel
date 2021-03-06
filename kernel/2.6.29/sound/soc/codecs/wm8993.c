/*
 * wm8993.c -- WM8993 ALSA SoC audio driver
 *
 * Copyright 2009 Wolfson Microelectronics plc
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/tlv.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/wm8993.h>

#include "wm8993.h"

#include <plat/s3c6410.h>

static u16 wm8993_reg_defaults[WM8993_REGISTER_COUNT] = {
    0x8993,     /* R0   - Software Reset */
    0x0000,     /* R1   - Power Management (1) */
    0x6000,     /* R2   - Power Management (2) */
    0x0000,     /* R3   - Power Management (3) */
    0x4050,     /* R4   - Audio Interface (1) */
    0x4000,     /* R5   - Audio Interface (2) */
    0x01C8,     /* R6   - Clocking 1 */
    0x0000,     /* R7   - Clocking 2 */
    0x0000,     /* R8   - Audio Interface (3) */
    0x0040,     /* R9   - Audio Interface (4) */
    0x0004,     /* R10  - DAC CTRL */
    0x00C0,     /* R11  - Left DAC Digital Volume */
    0x00C0,     /* R12  - Right DAC Digital Volume */
    0x0000,     /* R13  - Digital Side Tone */
    0x0300,     /* R14  - ADC CTRL */
    0x00C0,     /* R15  - Left ADC Digital Volume */
    0x00C0,     /* R16  - Right ADC Digital Volume */
    0x0000,     /* R17 */
    0x0000,     /* R18  - GPIO CTRL 1 */
    0x0010,     /* R19  - GPIO1 */
    0x0000,     /* R20  - IRQ_DEBOUNCE */
    0x0000,     /* R21 */
    0x8000,     /* R22  - GPIOCTRL 2 */
    0x0800,     /* R23  - GPIO_POL */
    0x008B,     /* R24  - Left Line Input 1&2 Volume */
    0x008B,     /* R25  - Left Line Input 3&4 Volume */
    0x008B,     /* R26  - Right Line Input 1&2 Volume */
    0x008B,     /* R27  - Right Line Input 3&4 Volume */
    0x006D,     /* R28  - Left Output Volume */
    0x006D,     /* R29  - Right Output Volume */
    0x0066,     /* R30  - Line Outputs Volume */
    0x0020,     /* R31  - HPOUT2 Volume */
    0x0079,     /* R32  - Left OPGA Volume */
    0x0079,     /* R33  - Right OPGA Volume */
    0x0003,     /* R34  - SPKMIXL Attenuation */
    0x0003,     /* R35  - SPKMIXR Attenuation */
    0x0011,     /* R36  - SPKOUT Mixers */
    0x0100,     /* R37  - SPKOUT Boost */
    0x0079,     /* R38  - Speaker Volume Left */
    0x0079,     /* R39  - Speaker Volume Right */
    0x0000,     /* R40  - Input Mixer2 */
    0x0000,     /* R41  - Input Mixer3 */
    0x0000,     /* R42  - Input Mixer4 */
    0x0000,     /* R43  - Input Mixer5 */
    0x0000,     /* R44  - Input Mixer6 */
    0x0000,     /* R45  - Output Mixer1 */
    0x0000,     /* R46  - Output Mixer2 */
    0x0000,     /* R47  - Output Mixer3 */
    0x0000,     /* R48  - Output Mixer4 */
    0x0000,     /* R49  - Output Mixer5 */
    0x0000,     /* R50  - Output Mixer6 */
    0x0000,     /* R51  - HPOUT2 Mixer */
    0x0000,     /* R52  - Line Mixer1 */
    0x0000,     /* R53  - Line Mixer2 */
    0x0000,     /* R54  - Speaker Mixer */
    0x0000,     /* R55  - Additional Control */
    0x0000,     /* R56  - AntiPOP1 */
    0x0000,     /* R57  - AntiPOP2 */
    0x0000,     /* R58  - MICBIAS */
    0x0000,     /* R59 */
    0x0000,     /* R60  - FLL Control 1 */
    0x0000,     /* R61  - FLL Control 2 */
    0x0000,     /* R62  - FLL Control 3 */
    0x2EE0,     /* R63  - FLL Control 4 */
    0x0002,     /* R64  - FLL Control 5 */
    0x2287,     /* R65  - Clocking 3 */
    0x025F,     /* R66  - Clocking 4 */
    0x0000,     /* R67  - MW Slave Control */
    0x0000,     /* R68 */
    0x0002,     /* R69  - Bus Control 1 */
    0x0000,     /* R70  - Write Sequencer 0 */
    0x0000,     /* R71  - Write Sequencer 1 */
    0x0000,     /* R72  - Write Sequencer 2 */
    0x0000,     /* R73  - Write Sequencer 3 */
    0x0000,     /* R74  - Write Sequencer 4 */
    0x0000,     /* R75  - Write Sequencer 5 */
    0x1F25,     /* R76  - Charge Pump 1 */
    0x0000,     /* R77 */
    0x0000,     /* R78 */
    0x0000,     /* R79 */
    0x0000,     /* R80 */
    0x0000,     /* R81  - Class W 0 */
    0x0000,     /* R82 */
    0x0000,     /* R83 */
    0x0000,     /* R84  - DC Servo 0 */
    0x054A,     /* R85  - DC Servo 1 */
    0x0000,     /* R86 */
    0x0000,     /* R87  - DC Servo 3 */
    0x0000,     /* R88  - DC Servo Readback 0 */
    0x0000,     /* R89  - DC Servo Readback 1 */
    0x0000,     /* R90  - DC Servo Readback 2 */
    0x0000,     /* R91 */
    0x0000,     /* R92 */
    0x0000,     /* R93 */
    0x0000,     /* R94 */
    0x0000,     /* R95 */
    0x0100,     /* R96  - Analogue HP 0 */
    0x0000,     /* R97 */
    0x0000,     /* R98  - EQ1 */
    0x000C,     /* R99  - EQ2 */
    0x000C,     /* R100 - EQ3 */
    0x000C,     /* R101 - EQ4 */
    0x000C,     /* R102 - EQ5 */
    0x000C,     /* R103 - EQ6 */
    0x0FCA,     /* R104 - EQ7 */
    0x0400,     /* R105 - EQ8 */
    0x00D8,     /* R106 - EQ9 */
    0x1EB5,     /* R107 - EQ10 */
    0xF145,     /* R108 - EQ11 */
    0x0B75,     /* R109 - EQ12 */
    0x01C5,     /* R110 - EQ13 */
    0x1C58,     /* R111 - EQ14 */
    0xF373,     /* R112 - EQ15 */
    0x0A54,     /* R113 - EQ16 */
    0x0558,     /* R114 - EQ17 */
    0x168E,     /* R115 - EQ18 */
    0xF829,     /* R116 - EQ19 */
    0x07AD,     /* R117 - EQ20 */
    0x1103,     /* R118 - EQ21 */
    0x0564,     /* R119 - EQ22 */
    0x0559,     /* R120 - EQ23 */
    0x4000,     /* R121 - EQ24 */
    0x0000,     /* R122 - Digital Pulls */
    0x0F08,     /* R123 - DRC Control 1 */
    0x0000,     /* R124 - DRC Control 2 */
    0x0080,     /* R125 - DRC Control 3 */
    0x0000,     /* R126 - DRC Control 4 */
};

static struct {
    int ratio;
    int clk_sys_rate;
} clk_sys_rates[] = {
    { 64,   0 },
    { 128,  1 },
    { 192,  2 },
    { 256,  3 },
    { 384,  4 },
    { 512,  5 },
    { 768,  6 },
    { 1024, 7 },
    { 1408, 8 },
    { 1536, 9 },
};

static struct {
    int rate;
    int sample_rate;
} sample_rates[] = {
    { 8000,  0  },
    { 11025, 1  },
    { 12000, 1  },
    { 16000, 2  },
    { 22050, 3  },
    { 24000, 3  },
    { 32000, 4  },
    { 44100, 5  },
    { 48000, 5  },
};

static struct {
    int div; /* *10 due to .5s */
    int bclk_div;
} bclk_divs[] = {
    { 10,  0  },
    { 15,  1  },
    { 20,  2  },
    { 30,  3  },
    { 40,  4  },
    { 55,  5  },
    { 60,  6  },
    { 80,  7  },
    { 110, 8  },
    { 120, 9  },
    { 160, 10 },
    { 220, 11 },
    { 240, 12 },
    { 320, 13 },
    { 440, 14 },
    { 480, 15 },
};

struct wm8993_priv {
    u16 reg_cache[WM8993_REGISTER_COUNT];
    struct wm8993_platform_data pdata;
    struct snd_soc_codec codec;
    int master;
    int sysclk_source;
    unsigned int mclk_rate;
    unsigned int sysclk_rate;
    unsigned int fs;
    unsigned int bclk;
    int class_w_users;
    unsigned int fll_fref;
    unsigned int fll_fout;
};

static unsigned int wm8993_read_hw(struct snd_soc_codec *codec, u8 reg)
{
    struct i2c_msg xfer[2];
    u16 data;
    int ret;
    struct i2c_client *i2c = codec->control_data;

    /* Write register */
    xfer[0].addr = i2c->addr;
    xfer[0].flags = 0;
    xfer[0].len = 1;
    xfer[0].buf = &reg;

    /* Read data */
    xfer[1].addr = i2c->addr;
    xfer[1].flags = I2C_M_RD;
    xfer[1].len = 2;
    xfer[1].buf = (u8 *)&data;

    ret = i2c_transfer(i2c->adapter, xfer, 2);
    if (ret != 2) {
        printk("Failed to read 0x%x: %d\n", reg, ret);
        return 0;
    }

    return (data >> 8) | ((data & 0xff) << 8);
}

static int wm8993_volatile(unsigned int reg)
{
    switch (reg) {
    case WM8993_SOFTWARE_RESET:
    case WM8993_DC_SERVO_0:
    case WM8993_DC_SERVO_READBACK_0:
    case WM8993_DC_SERVO_READBACK_1:
    case WM8993_DC_SERVO_READBACK_2:
        return 1;
    default:
        return 0;
    }
}

static unsigned int wm8993_read(struct snd_soc_codec *codec,
                unsigned int reg)
{
    u16 *reg_cache = codec->reg_cache;

    BUG_ON(reg > WM8993_MAX_REGISTER);

    if (wm8993_volatile(reg))
        return wm8993_read_hw(codec, reg);
    else
        return reg_cache[reg];
}

static int wm8993_write(struct snd_soc_codec *codec, unsigned int reg,
            unsigned int value)
{
    u16 *reg_cache = codec->reg_cache;
    u8 data[3];
    int ret;

    BUG_ON(reg > WM8993_MAX_REGISTER);

    /* data is
     *   D15..D9 WM8993 register offset
     *   D8...D0 register data
     */
    data[0] = reg;
    data[1] = value >> 8;
    data[2] = value & 0x00ff;

    if (!wm8993_volatile(reg))
        reg_cache[reg] = value;

    ret = codec->hw_write(codec->control_data, data, 3);
    //printk("wm8993:i2c_write reg:%X value:%X ret:%d\n", reg, value, ret);

    if (ret == 3)
        return 0;
    if (ret < 0)
        return ret;
    return -EIO;
}

struct _fll_div {
    u16 fll_fratio;
    u16 fll_outdiv;
    u16 fll_clk_ref_div;
    u16 n;
    u16 k;
};

/* The size in bits of the FLL divide multiplied by 10
 * to allow rounding later */
#define FIXED_FLL_SIZE ((1 << 16) * 10)

static struct {
    unsigned int min;
    unsigned int max;
    u16 fll_fratio;
    int ratio;
} fll_fratios[] = {
    {       0,    64000, 4, 16 },
    {   64000,   128000, 3,  8 },
    {  128000,   256000, 2,  4 },
    {  256000,  1000000, 1,  2 },
    { 1000000, 13500000, 0,  1 },
};

static int fll_factors(struct _fll_div *fll_div, unsigned int Fref,
               unsigned int Fout)
{
    u64 Kpart;
    unsigned int K, Ndiv, Nmod, target;
    unsigned int div;
    int i;

    /* Fref must be <=13.5MHz */
    div = 1;
    while ((Fref / div) > 13500000) {
        div *= 2;

        if (div > 8) {
            printk("Can't scale %dMHz input down to <=13.5MHz\n",
                   Fref);
            return -EINVAL;
        }
    }

    printk("Fref=%u Fout=%u\n", Fref, Fout);

    /* Apply the division for our remaining calculations */
    Fref /= div;

    /* Fvco should be 90-100MHz; don't check the upper bound */
    div = 0;
    target = Fout * 2;
    while (target < 90000000) {
        div++;
        target *= 2;
        if (div > 7) {
            printk("Unable to find FLL_OUTDIV for Fout=%uHz\n",
                   Fout);
            return -EINVAL;
        }
    }
    fll_div->fll_outdiv = div;

    printk("Fvco=%dHz\n", target);

    /* Find an appropraite FLL_FRATIO and factor it out of the target */
    for (i = 0; i < ARRAY_SIZE(fll_fratios); i++) {
        if (fll_fratios[i].min <= Fref && Fref <= fll_fratios[i].max) {
            fll_div->fll_fratio = fll_fratios[i].fll_fratio;
            target /= fll_fratios[i].ratio;
            break;
        }
    }
    if (i == ARRAY_SIZE(fll_fratios)) {
        printk("Unable to find FLL_FRATIO for Fref=%uHz\n", Fref);
        return -EINVAL;
    }

    /* Now, calculate N.K */
    Ndiv = target / Fref;

    fll_div->n = Ndiv;
    Nmod = target % Fref;
    printk("Nmod=%d\n", Nmod);

    /* Calculate fractional part - scale up so we can round. */
    Kpart = FIXED_FLL_SIZE * (long long)Nmod;

    do_div(Kpart, Fref);

    K = Kpart & 0xFFFFFFFF;

    if ((K % 10) >= 5)
        K += 5;

    /* Move down to proper range now rounding is done */
    fll_div->k = K / 10;

    printk("N=%x K=%x FLL_FRATIO=%x FLL_OUTDIV=%x FLL_CLK_REF_DIV=%x\n",
         fll_div->n, fll_div->k,
         fll_div->fll_fratio, fll_div->fll_outdiv,
         fll_div->fll_clk_ref_div);

    return 0;
}

static int wm8993_set_fll(struct snd_soc_dai *dai, int fll_id,
              unsigned int Fref, unsigned int Fout)
{
    struct snd_soc_codec *codec = dai->codec;
    struct wm8993_priv *wm8993 = codec->private_data;
    u16 reg1, reg4, reg5;
    struct _fll_div fll_div;
    int ret;

    /* Any change? */
    if (Fref == wm8993->fll_fref && Fout == wm8993->fll_fout)
        return 0;

    /* Disable the FLL */
    if (Fout == 0) {
        printk("FLL disabled\n");
        wm8993->fll_fref = 0;
        wm8993->fll_fout = 0;

        reg1 = wm8993_read(codec, WM8993_FLL_CONTROL_1);
        reg1 &= ~WM8993_FLL_ENA;
        //printk("wm8993_write: WM8993_FLL_CONTROL_1 at %d", __LINE__);
        wm8993_write(codec, WM8993_FLL_CONTROL_1, reg1);

        return 0;
    }

    ret = fll_factors(&fll_div, Fref, Fout);
    if (ret != 0)
        return ret;

    reg5 = wm8993_read(codec, WM8993_FLL_CONTROL_5);
    reg5 &= ~WM8993_FLL_CLK_SRC_MASK;

    switch (fll_id) {
    case WM8993_FLL_MCLK:
        break;

    case WM8993_FLL_LRCLK:
        reg5 |= 1;
        break;

    case WM8993_FLL_BCLK:
        reg5 |= 2;
        break;

    default:
        printk("Unknown FLL ID %d\n", fll_id);
        return -EINVAL;
    }

    /* Any FLL configuration change requires that the FLL be
     * disabled first. */
    reg1 = wm8993_read(codec, WM8993_FLL_CONTROL_1);
    reg1 &= ~WM8993_FLL_ENA;
    //printk("wm8993_write: WM8993_FLL_CONTROL_1 at %d", __LINE__);
    wm8993_write(codec, WM8993_FLL_CONTROL_1, reg1);

    /* Apply the configuration */
    if (fll_div.k)
        reg1 |= WM8993_FLL_FRAC_MASK;
    else
        reg1 &= ~WM8993_FLL_FRAC_MASK;
    //printk("wm8993_write: WM8993_FLL_CONTROL_1 at %d", __LINE__);
    wm8993_write(codec, WM8993_FLL_CONTROL_1, reg1);

	//printk("wm8993_write: WM8993_FLL_CONTROL_2 at %d", __LINE__);
    wm8993_write(codec, WM8993_FLL_CONTROL_2,
             (fll_div.fll_outdiv << WM8993_FLL_OUTDIV_SHIFT) |
             (fll_div.fll_fratio << WM8993_FLL_FRATIO_SHIFT));
    //printk("wm8993_write: WM8993_FLL_CONTROL_3 at %d", __LINE__);
    wm8993_write(codec, WM8993_FLL_CONTROL_3, fll_div.k);

    reg4 = wm8993_read(codec, WM8993_FLL_CONTROL_4);
    reg4 &= ~WM8993_FLL_N_MASK;
    reg4 |= fll_div.n << WM8993_FLL_N_SHIFT;
    //printk("wm8993_write: WM8993_FLL_CONTROL_4 at %d", __LINE__);
    wm8993_write(codec, WM8993_FLL_CONTROL_4, reg4);

    reg5 &= ~WM8993_FLL_CLK_REF_DIV_MASK;
    reg5 |= fll_div.fll_clk_ref_div << WM8993_FLL_CLK_REF_DIV_SHIFT;
    //printk("wm8993_write: WM8993_FLL_CONTROL_5 at %d", __LINE__);
    wm8993_write(codec, WM8993_FLL_CONTROL_5, reg5);

    /* Enable the FLL */
    //printk("wm8993_write: WM8993_FLL_CONTROL_1 at %d", __LINE__);
    wm8993_write(codec, WM8993_FLL_CONTROL_1, reg1 | WM8993_FLL_ENA);

    //printk("FLL enabled at %dHz->%dHz\n", Fref, Fout);

    wm8993->fll_fref = Fref;
    wm8993->fll_fout = Fout;

    return 0;
}

static int configure_clock(struct snd_soc_codec *codec)
{
    struct wm8993_priv *wm8993 = codec->private_data;
    unsigned int reg;

    /* This should be done on init() for bypass paths */
    switch (wm8993->sysclk_source) {
    case WM8993_SYSCLK_MCLK:
        printk("Using %dHz MCLK\n", wm8993->mclk_rate);

        reg = wm8993_read(codec, WM8993_CLOCKING_2);
        reg &= ~WM8993_SYSCLK_SRC;
        if (wm8993->mclk_rate > 13500000) {
            reg |= WM8993_MCLK_DIV;
            wm8993->sysclk_rate = wm8993->mclk_rate / 2;
        } else {
            reg &= ~WM8993_MCLK_DIV;
            wm8993->sysclk_rate = wm8993->mclk_rate;
        }
        reg &= ~WM8993_MCLK_DIV;
        reg &= ~(WM8993_MCLK_DIV | WM8993_SYSCLK_SRC);
        //printk("wm8993_write: WM8993_CLOCKING_2 at %d", __LINE__);
        wm8993_write(codec, WM8993_CLOCKING_2, reg);
        break;

    case WM8993_SYSCLK_FLL:
        printk("Using %dHz FLL clock\n",
            wm8993->fll_fout);

        reg = wm8993_read(codec, WM8993_CLOCKING_2);
        reg |= WM8993_SYSCLK_SRC;
        if (wm8993->fll_fout > 13500000) {
            reg |= WM8993_MCLK_DIV;
            wm8993->sysclk_rate = wm8993->fll_fout / 2;
        } else {
            reg &= ~WM8993_MCLK_DIV;
            wm8993->sysclk_rate = wm8993->fll_fout;
        }
        //printk("wm8993_write: WM8993_CLOCKING_2 at %d", __LINE__);
        wm8993_write(codec, WM8993_CLOCKING_2, reg);
        break;

    default:
        printk("System clock not configured\n");
        return -EINVAL;
    }

    printk("CLK_SYS is %dHz\n", wm8993->sysclk_rate);

    return 0;
}

static void wait_for_dc_servo(struct snd_soc_codec *codec, int mask)
{
    unsigned int reg;
    int count = 0;

    printk("Waiting for DC servo...\n");
    do {
        count++;
        msleep(1);
        reg = wm8993_read(codec, WM8993_DC_SERVO_READBACK_0);
        printk("DC servo status: %x\n", reg);
    } while ((reg & WM8993_DCS_CAL_COMPLETE_MASK)
         != WM8993_DCS_CAL_COMPLETE_MASK && count < 1000);

    if ((reg & WM8993_DCS_CAL_COMPLETE_MASK)
        != WM8993_DCS_CAL_COMPLETE_MASK)
        printk("Timed out waiting for DC Servo\n");
}

static const DECLARE_TLV_DB_SCALE(inpga_tlv, -1650, 150, 0);
static const DECLARE_TLV_DB_SCALE(inmix_sw_tlv, 0, 3000, 0);
static const DECLARE_TLV_DB_SCALE(inmix_tlv, -1500, 300, 1);
static const DECLARE_TLV_DB_SCALE(sidetone_tlv, -3600, 300, 0);
static const DECLARE_TLV_DB_SCALE(drc_comp_threash, -4500, 75, 0);
static const DECLARE_TLV_DB_SCALE(drc_comp_amp, -2250, 75, 0);
static const DECLARE_TLV_DB_SCALE(drc_min_tlv, -1800, 600, 0);
static const unsigned int drc_max_tlv[] = {
    TLV_DB_RANGE_HEAD(4),
    0, 2, TLV_DB_SCALE_ITEM(1200, 600, 0),
    3, 3, TLV_DB_SCALE_ITEM(3600, 0, 0),
};
static const DECLARE_TLV_DB_SCALE(drc_qr_tlv, 1200, 600, 0);
static const DECLARE_TLV_DB_SCALE(drc_startup_tlv, -1800, 300, 0);
static const DECLARE_TLV_DB_SCALE(eq_tlv, -1200, 100, 0);
static const DECLARE_TLV_DB_SCALE(digital_tlv, -7200, 75, 1);
static const DECLARE_TLV_DB_SCALE(dac_boost_tlv, 0, 600, 0);
static const DECLARE_TLV_DB_SCALE(earpiece_tlv, -600, 600, 0);
static const DECLARE_TLV_DB_SCALE(outmix_tlv, -2100, 300, 0);
static const DECLARE_TLV_DB_SCALE(spkmix_tlv, -300, 300, 0);
static const DECLARE_TLV_DB_SCALE(spkmixout_tlv, -1800, 600, 1);
static const DECLARE_TLV_DB_SCALE(outpga_tlv, -5700, 100, 0);
static const unsigned int spkboost_tlv[] = {
    TLV_DB_RANGE_HEAD(7),
    0, 6, TLV_DB_SCALE_ITEM(0, 150, 0),
    7, 7, TLV_DB_SCALE_ITEM(1200, 0, 0),
};
static const DECLARE_TLV_DB_SCALE(line_tlv, -600, 600, 0);

static const char *speaker_ref_text[] = {
    "SPKVDD/2",
    "VMID",
};

static const struct soc_enum speaker_ref =
    SOC_ENUM_SINGLE(WM8993_SPEAKER_MIXER, 8, 2, speaker_ref_text);

static const char *speaker_mode_text[] = {
    "Class D",
    "Class AB",
};

static const struct soc_enum speaker_mode =
    SOC_ENUM_SINGLE(WM8993_SPKMIXR_ATTENUATION, 8, 2, speaker_mode_text);

static const char *dac_deemph_text[] = {
    "None",
    "32kHz",
    "44.1kHz",
    "48kHz",
};

static const struct soc_enum dac_deemph =
    SOC_ENUM_SINGLE(WM8993_DAC_CTRL, 4, 4, dac_deemph_text);

static const char *adc_hpf_text[] = {
    "Hi-Fi",
    "Voice 1",
    "Voice 2",
    "Voice 3",
};

static const struct soc_enum adc_hpf =
    SOC_ENUM_SINGLE(WM8993_ADC_CTRL, 5, 4, adc_hpf_text);

static const char *drc_path_text[] = {
    "ADC",
    "DAC"
};

static const struct soc_enum drc_path =
    SOC_ENUM_SINGLE(WM8993_DRC_CONTROL_1, 14, 2, drc_path_text);

static const char *drc_r0_text[] = {
    "1",
    "1/2",
    "1/4",
    "1/8",
    "1/16",
    "0",
};

static const struct soc_enum drc_r0 =
    SOC_ENUM_SINGLE(WM8993_DRC_CONTROL_3, 8, 6, drc_r0_text);

static const char *drc_r1_text[] = {
    "1",
    "1/2",
    "1/4",
    "1/8",
    "0",
};

static const struct soc_enum drc_r1 =
    SOC_ENUM_SINGLE(WM8993_DRC_CONTROL_4, 13, 5, drc_r1_text);

static const char *drc_attack_text[] = {
    "Reserved",
    "181us",
    "363us",
    "726us",
    "1.45ms",
    "2.9ms",
    "5.8ms",
    "11.6ms",
    "23.2ms",
    "46.4ms",
    "92.8ms",
    "185.6ms",
};

static const struct soc_enum drc_attack =
    SOC_ENUM_SINGLE(WM8993_DRC_CONTROL_2, 12, 12, drc_attack_text);

static const char *drc_decay_text[] = {
    "186ms",
    "372ms",
    "743ms",
    "1.49s",
    "2.97ms",
    "5.94ms",
    "11.89ms",
    "23.78ms",
    "47.56ms",
};

static const struct soc_enum drc_decay =
    SOC_ENUM_SINGLE(WM8993_DRC_CONTROL_2, 8, 9, drc_decay_text);

static const char *drc_ff_text[] = {
    "5 samples",
    "9 samples",
};

static const struct soc_enum drc_ff =
    SOC_ENUM_SINGLE(WM8993_DRC_CONTROL_3, 7, 2, drc_ff_text);

static const char *drc_qr_rate_text[] = {
    "0.725ms",
    "1.45ms",
    "5.8ms",
};

static const struct soc_enum drc_qr_rate =
    SOC_ENUM_SINGLE(WM8993_DRC_CONTROL_3, 0, 3, drc_qr_rate_text);

static const char *drc_smooth_text[] = {
    "Low",
    "Medium",
    "High",
};

static const struct soc_enum drc_smooth =
    SOC_ENUM_SINGLE(WM8993_DRC_CONTROL_1, 4, 3, drc_smooth_text);


/*
 * Update the DC servo calibration on gain changes
 */
static int wm8993_put_dc_servo(struct snd_kcontrol *kcontrol,
                  struct snd_ctl_elem_value *ucontrol)
{
    struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
    int ret;

    ret = snd_soc_put_volsw_2r(kcontrol, ucontrol);

    /* Only need to do this if the outputs are active */
    if (wm8993_read(codec, WM8993_POWER_MANAGEMENT_1)
        & (WM8993_HPOUT1L_ENA | WM8993_HPOUT1R_ENA))
        snd_soc_update_bits(codec,
                    WM8993_DC_SERVO_0,
                    WM8993_DCS_TRIG_SINGLE_0 |
                    WM8993_DCS_TRIG_SINGLE_1,
                    WM8993_DCS_TRIG_SINGLE_0 |
                    WM8993_DCS_TRIG_SINGLE_1);

    return ret;
}

static const struct snd_kcontrol_new wm8993_snd_controls[] = {
SOC_SINGLE_TLV("IN1L Volume", WM8993_LEFT_LINE_INPUT_1_2_VOLUME, 0, 31, 0,
           inpga_tlv),
SOC_SINGLE("IN1L Switch", WM8993_LEFT_LINE_INPUT_1_2_VOLUME, 7, 1, 1),
SOC_SINGLE("IN1L ZC Switch", WM8993_LEFT_LINE_INPUT_1_2_VOLUME, 7, 1, 0),

SOC_SINGLE_TLV("IN1R Volume", WM8993_RIGHT_LINE_INPUT_1_2_VOLUME, 0, 31, 0,
           inpga_tlv),
SOC_SINGLE("IN1R Switch", WM8993_RIGHT_LINE_INPUT_1_2_VOLUME, 7, 1, 1),
SOC_SINGLE("IN1R ZC Switch", WM8993_RIGHT_LINE_INPUT_1_2_VOLUME, 7, 1, 0),


SOC_SINGLE_TLV("IN2L Volume", WM8993_LEFT_LINE_INPUT_3_4_VOLUME, 0, 31, 0,
           inpga_tlv),
SOC_SINGLE("IN2L Switch", WM8993_LEFT_LINE_INPUT_3_4_VOLUME, 7, 1, 1),
SOC_SINGLE("IN2L ZC Switch", WM8993_LEFT_LINE_INPUT_3_4_VOLUME, 7, 1, 0),

SOC_SINGLE_TLV("IN2R Volume", WM8993_RIGHT_LINE_INPUT_3_4_VOLUME, 0, 31, 0,
           inpga_tlv),
SOC_SINGLE("IN2R Switch", WM8993_RIGHT_LINE_INPUT_3_4_VOLUME, 7, 1, 1),
SOC_SINGLE("IN2R ZC Switch", WM8993_RIGHT_LINE_INPUT_3_4_VOLUME, 7, 1, 0),

SOC_SINGLE_TLV("MIXINL IN2L Volume", WM8993_INPUT_MIXER3, 7, 1, 0,
           inmix_sw_tlv),
SOC_SINGLE_TLV("MIXINL IN1L Volume", WM8993_INPUT_MIXER3, 4, 1, 0,
           inmix_sw_tlv),
SOC_SINGLE_TLV("MIXINL Output Record Volume", WM8993_INPUT_MIXER3, 0, 7, 0,
           inmix_tlv),
SOC_SINGLE_TLV("MIXINL IN1LP Volume", WM8993_INPUT_MIXER5, 6, 7, 0, inmix_tlv),
SOC_SINGLE_TLV("MIXINL Direct Voice Volume", WM8993_INPUT_MIXER5, 0, 6, 0,
           inmix_tlv),

SOC_SINGLE_TLV("MIXINR IN2R Volume", WM8993_INPUT_MIXER4, 7, 1, 0,
           inmix_sw_tlv),
SOC_SINGLE_TLV("MIXINR IN1R Volume", WM8993_INPUT_MIXER4, 4, 1, 0,
           inmix_sw_tlv),
SOC_SINGLE_TLV("MIXINR Output Record Volume", WM8993_INPUT_MIXER4, 0, 7, 0,
           inmix_tlv),
SOC_SINGLE_TLV("MIXINR IN1RP Volume", WM8993_INPUT_MIXER6, 6, 7, 0, inmix_tlv),
SOC_SINGLE_TLV("MIXINR Direct Voice Volume", WM8993_INPUT_MIXER6, 0, 6, 0,
           inmix_tlv),

SOC_DOUBLE_TLV("Digital Sidetone Volume", WM8993_DIGITAL_SIDE_TONE,
           5, 9, 12, 0, sidetone_tlv),

SOC_SINGLE("DRC Switch", WM8993_DRC_CONTROL_1, 15, 1, 0),
SOC_ENUM("DRC Path", drc_path),
SOC_SINGLE_TLV("DRC Compressor Threashold Volume", WM8993_DRC_CONTROL_2,
           2, 60, 1, drc_comp_threash),
SOC_SINGLE_TLV("DRC Compressor Amplitude Volume", WM8993_DRC_CONTROL_3,
           11, 30, 1, drc_comp_amp),
SOC_ENUM("DRC R0", drc_r0),
SOC_ENUM("DRC R1", drc_r1),
SOC_SINGLE_TLV("DRC Minimum Volume", WM8993_DRC_CONTROL_1, 2, 3, 1,
           drc_min_tlv),
SOC_SINGLE_TLV("DRC Maximum Volume", WM8993_DRC_CONTROL_1, 0, 3, 0,
           drc_max_tlv),
SOC_ENUM("DRC Attack Rate", drc_attack),
SOC_ENUM("DRC Decay Rate", drc_decay),
SOC_ENUM("DRC FF Delay", drc_ff),
SOC_SINGLE("DRC Anti-clip Switch", WM8993_DRC_CONTROL_1, 9, 1, 0),
SOC_SINGLE("DRC Quick Release Switch", WM8993_DRC_CONTROL_1, 10, 1, 0),
SOC_SINGLE_TLV("DRC Quick Release Volume", WM8993_DRC_CONTROL_3, 2, 3, 0,
           drc_qr_tlv),
SOC_ENUM("DRC Quick Release Rate", drc_qr_rate),
SOC_SINGLE("DRC Smoothing Switch", WM8993_DRC_CONTROL_1, 11, 1, 0),
SOC_SINGLE("DRC Smoothing Hysteresis Switch", WM8993_DRC_CONTROL_1, 8, 1, 0),
SOC_ENUM("DRC Smoothing Hysteresis Threashold", drc_smooth),
SOC_SINGLE_TLV("DRC Startup Volume", WM8993_DRC_CONTROL_4, 8, 18, 0,
           drc_startup_tlv),

SOC_SINGLE("EQ Switch", WM8993_EQ1, 0, 1, 0),

SOC_DOUBLE_R_TLV("Capture Volume", WM8993_LEFT_ADC_DIGITAL_VOLUME,
         WM8993_RIGHT_ADC_DIGITAL_VOLUME, 1, 96, 0, digital_tlv),
SOC_SINGLE("ADC High Pass Filter Switch", WM8993_ADC_CTRL, 8, 1, 0),
SOC_ENUM("ADC High Pass Filter Mode", adc_hpf),

SOC_DOUBLE_R_TLV("Playback Volume", WM8993_LEFT_DAC_DIGITAL_VOLUME,
         WM8993_RIGHT_DAC_DIGITAL_VOLUME, 1, 96, 0, digital_tlv),
SOC_SINGLE_TLV("Playback Boost Volume", WM8993_AUDIO_INTERFACE_2, 10, 3, 0,
           dac_boost_tlv),
SOC_ENUM("DAC Deemphasis", dac_deemph),

SOC_SINGLE_TLV("Left Output Mixer IN2RN Volume", WM8993_OUTPUT_MIXER5, 6, 7, 1,
           outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer IN2LN Volume", WM8993_OUTPUT_MIXER3, 6, 7, 1,
           outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer IN2LP Volume", WM8993_OUTPUT_MIXER3, 9, 7, 1,
           outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer IN1L Volume", WM8993_OUTPUT_MIXER3, 0, 7, 1,
           outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer IN1R Volume", WM8993_OUTPUT_MIXER3, 3, 7, 1,
           outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer Right Input Volume",
           WM8993_OUTPUT_MIXER5, 3, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer Left Input Volume",
           WM8993_OUTPUT_MIXER5, 0, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer DAC Volume", WM8993_OUTPUT_MIXER5, 9, 7, 1,
           outmix_tlv),

SOC_SINGLE_TLV("Right Output Mixer IN2LN Volume",
           WM8993_OUTPUT_MIXER6, 6, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer IN2RN Volume",
           WM8993_OUTPUT_MIXER4, 6, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer IN1L Volume",
           WM8993_OUTPUT_MIXER4, 3, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer IN1R Volume",
           WM8993_OUTPUT_MIXER4, 0, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer IN2RP Volume",
           WM8993_OUTPUT_MIXER4, 9, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer Left Input Volume",
           WM8993_OUTPUT_MIXER6, 3, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer Right Input Volume",
           WM8993_OUTPUT_MIXER6, 6, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer DAC Volume",
           WM8993_OUTPUT_MIXER6, 9, 7, 1, outmix_tlv),

SOC_DOUBLE_R_TLV("Output Volume", WM8993_LEFT_OPGA_VOLUME,
         WM8993_RIGHT_OPGA_VOLUME, 0, 63, 0, outpga_tlv),
SOC_DOUBLE_R("Output Switch", WM8993_LEFT_OPGA_VOLUME,
         WM8993_RIGHT_OPGA_VOLUME, 6, 1, 0),
SOC_DOUBLE_R("Output ZC Switch", WM8993_LEFT_OPGA_VOLUME,
         WM8993_RIGHT_OPGA_VOLUME, 7, 1, 0),

SOC_SINGLE("Earpiece Switch", WM8993_HPOUT2_VOLUME, 5, 1, 1),
SOC_SINGLE_TLV("Earpiece Volume", WM8993_HPOUT2_VOLUME, 4, 1, 1, earpiece_tlv),

SOC_SINGLE_TLV("SPKL Input Volume", WM8993_SPKMIXL_ATTENUATION,
           5, 1, 1, spkmix_tlv),
SOC_SINGLE_TLV("SPKL IN1LP Volume", WM8993_SPKMIXL_ATTENUATION,
           4, 1, 1, spkmix_tlv),
SOC_SINGLE_TLV("SPKL Output Volume", WM8993_SPKMIXL_ATTENUATION,
           3, 1, 1, spkmix_tlv),
SOC_SINGLE_TLV("SPKL DAC Volume", WM8993_SPKMIXL_ATTENUATION,
           2, 1, 1, spkmix_tlv),

SOC_SINGLE_TLV("SPKR Input Volume", WM8993_SPKMIXR_ATTENUATION,
           5, 1, 1, spkmix_tlv),
SOC_SINGLE_TLV("SPKR IN1RP Volume", WM8993_SPKMIXR_ATTENUATION,
           4, 1, 1, spkmix_tlv),
SOC_SINGLE_TLV("SPKR Output Volume", WM8993_SPKMIXR_ATTENUATION,
           3, 1, 1, spkmix_tlv),
SOC_SINGLE_TLV("SPKR DAC Volume", WM8993_SPKMIXR_ATTENUATION,
           2, 1, 1, spkmix_tlv),

SOC_DOUBLE_R_TLV("Speaker Mixer Volume",
         WM8993_SPKMIXL_ATTENUATION, WM8993_SPKMIXR_ATTENUATION,
         0, 3, 1, spkmixout_tlv),
SOC_DOUBLE_R_TLV("Speaker Volume",
         WM8993_SPEAKER_VOLUME_LEFT, WM8993_SPEAKER_VOLUME_RIGHT,
         0, 63, 0, outpga_tlv),
SOC_DOUBLE_R("Speaker Switch",
         WM8993_SPEAKER_VOLUME_LEFT, WM8993_SPEAKER_VOLUME_RIGHT,
         6, 1, 0),
SOC_DOUBLE_R("Speaker ZC Switch",
         WM8993_SPEAKER_VOLUME_LEFT, WM8993_SPEAKER_VOLUME_RIGHT,
         7, 1, 0),
SOC_DOUBLE_TLV("Speaker Boost Volume", WM8993_SPKOUT_BOOST, 0, 3, 7, 0,
           spkboost_tlv),
SOC_ENUM("Speaker Reference", speaker_ref),
SOC_ENUM("Speaker Mode", speaker_mode),

{
    .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = "Headphone Volume",
    .access = SNDRV_CTL_ELEM_ACCESS_TLV_READ |
         SNDRV_CTL_ELEM_ACCESS_READWRITE,
    .tlv.p = outpga_tlv,
    .info = snd_soc_info_volsw_2r,
    .get = snd_soc_get_volsw_2r, .put = wm8993_put_dc_servo,
    .private_value = (unsigned long)&(struct soc_mixer_control) {
        .reg = WM8993_LEFT_OUTPUT_VOLUME,
        .rreg = WM8993_RIGHT_OUTPUT_VOLUME,
        .shift = 0, .max = 63
    },
},
SOC_DOUBLE_R("Headphone Switch", WM8993_LEFT_OUTPUT_VOLUME,
         WM8993_RIGHT_OUTPUT_VOLUME, 6, 1, 0),
SOC_DOUBLE_R("Headphone ZC Switch", WM8993_LEFT_OUTPUT_VOLUME,
         WM8993_RIGHT_OUTPUT_VOLUME, 7, 1, 0),

SOC_SINGLE("LINEOUT1N Switch", WM8993_LINE_OUTPUTS_VOLUME, 6, 1, 1),
SOC_SINGLE("LINEOUT1P Switch", WM8993_LINE_OUTPUTS_VOLUME, 5, 1, 1),
SOC_SINGLE_TLV("LINEOUT1 Volume", WM8993_LINE_OUTPUTS_VOLUME, 4, 1, 1,
           line_tlv),

SOC_SINGLE("LINEOUT2N Switch", WM8993_LINE_OUTPUTS_VOLUME, 2, 1, 1),
SOC_SINGLE("LINEOUT2P Switch", WM8993_LINE_OUTPUTS_VOLUME, 1, 1, 1),
SOC_SINGLE_TLV("LINEOUT2 Volume", WM8993_LINE_OUTPUTS_VOLUME, 0, 1, 1,
           line_tlv),
};

static const struct snd_kcontrol_new wm8993_eq_controls[] = {
SOC_SINGLE_TLV("EQ1 Volume", WM8993_EQ2, 0, 24, 0, eq_tlv),
SOC_SINGLE_TLV("EQ2 Volume", WM8993_EQ3, 0, 24, 0, eq_tlv),
SOC_SINGLE_TLV("EQ3 Volume", WM8993_EQ4, 0, 24, 0, eq_tlv),
SOC_SINGLE_TLV("EQ4 Volume", WM8993_EQ5, 0, 24, 0, eq_tlv),
SOC_SINGLE_TLV("EQ5 Volume", WM8993_EQ6, 0, 24, 0, eq_tlv),
};

static int wm8993_earpiece_event(struct snd_soc_dapm_widget *w,
                 struct snd_kcontrol *control, int event)
{
    struct snd_soc_codec *codec = w->codec;
    u16 reg = wm8993_read(codec, WM8993_ANTIPOP1) & ~WM8993_HPOUT2_IN_ENA;

    switch (event) {
    case SND_SOC_DAPM_PRE_PMU:
        reg |= WM8993_HPOUT2_IN_ENA;
    	//printk("wm8993_write: WM8993_ANTIPOP1 at %d", __LINE__);
        wm8993_write(codec, WM8993_ANTIPOP1, reg);
        udelay(50);
        break;

    case SND_SOC_DAPM_POST_PMD:
    	//printk("wm8993_write: WM8993_ANTIPOP1 at %d", __LINE__);
        wm8993_write(codec, WM8993_ANTIPOP1, reg);
        break;

    default:
        BUG();
        break;
    }

    return 0;
}

static int clk_sys_event(struct snd_soc_dapm_widget *w,
             struct snd_kcontrol *kcontrol, int event)
{
    struct snd_soc_codec *codec = w->codec;

    switch (event) {
    case SND_SOC_DAPM_PRE_PMU:
        return configure_clock(codec);

    case SND_SOC_DAPM_POST_PMD:
        break;
    }

    return 0;
}

/*
 * When used with DAC outputs only the WM8993 charge pump supports
 * operation in class W mode, providing very low power consumption
 * when used with digital sources.  Enable and disable this mode
 * automatically depending on the mixer configuration.
 *
 * Currently the only supported paths are the direct DAC->headphone
 * paths (which provide minimum power consumption anyway).
 */
static int wm8993_class_w_put(struct snd_kcontrol *kcontrol,
                  struct snd_ctl_elem_value *ucontrol)
{
    struct snd_soc_dapm_widget *widget = snd_kcontrol_chip(kcontrol);
    struct snd_soc_codec *codec = widget->codec;
    struct wm8993_priv *wm8993 = codec->private_data;
    int ret;

    /* Turn it off if we're using the main output mixer */
    if (ucontrol->value.integer.value[0] == 0) {
        if (wm8993->class_w_users == 0) {
            printk("Disabling Class W\n");
            snd_soc_update_bits(codec, WM8993_CLASS_W_0,
                        WM8993_CP_DYN_FREQ |
                        WM8993_CP_DYN_V,
                        0);
        }
        wm8993->class_w_users++;
    }

    /* Implement the change */
    ret = snd_soc_dapm_put_enum_double(kcontrol, ucontrol);

    /* Enable it if we're using the direct DAC path */
    if (ucontrol->value.integer.value[0] == 1) {
        if (wm8993->class_w_users == 1) {
            printk("Enabling Class W\n");
            snd_soc_update_bits(codec, WM8993_CLASS_W_0,
                        WM8993_CP_DYN_FREQ |
                        WM8993_CP_DYN_V,
                        WM8993_CP_DYN_FREQ |
                        WM8993_CP_DYN_V);
        }
        wm8993->class_w_users--;
    }

    printk("Indirect DAC use count now %d\n",
        wm8993->class_w_users);

    return ret;
}

#define SOC_DAPM_ENUM_W(xname, xenum) \
{       .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
    .info = snd_soc_info_enum_double, \
    .get = snd_soc_dapm_get_enum_double, \
    .put = wm8993_class_w_put, \
    .private_value = (unsigned long)&xenum }

static int hp_event(struct snd_soc_dapm_widget *w,
            struct snd_kcontrol *kcontrol, int event)
{
    struct snd_soc_codec *codec = w->codec;
    unsigned int reg = wm8993_read(codec, WM8993_ANALOGUE_HP_0);

    switch (event) {
    case SND_SOC_DAPM_POST_PMU:
        snd_soc_update_bits(codec, WM8993_CHARGE_PUMP_1,
                    WM8993_CP_ENA, WM8993_CP_ENA);

        msleep(5);

        snd_soc_update_bits(codec, WM8993_POWER_MANAGEMENT_1,
                    WM8993_HPOUT1L_ENA | WM8993_HPOUT1R_ENA,
                    WM8993_HPOUT1L_ENA | WM8993_HPOUT1R_ENA);

        reg |= WM8993_HPOUT1L_DLY | WM8993_HPOUT1R_DLY;
        //printk("wm8993_write: WM8993_ANALOGUE_HP_0 at %d", __LINE__);
        wm8993_write(codec, WM8993_ANALOGUE_HP_0, reg);

        /* Start the DC servo */
        snd_soc_update_bits(codec, WM8993_DC_SERVO_0,
                    WM8993_DCS_ENA_CHAN_0 |
                    WM8993_DCS_ENA_CHAN_1 |
                    WM8993_DCS_TRIG_STARTUP_1 |
                    WM8993_DCS_TRIG_STARTUP_0,
                    WM8993_DCS_ENA_CHAN_0 |
                    WM8993_DCS_ENA_CHAN_1 |
                    WM8993_DCS_TRIG_STARTUP_1 |
                    WM8993_DCS_TRIG_STARTUP_0);
        wait_for_dc_servo(codec, WM8993_DCS_TRIG_STARTUP_0 |
                  WM8993_DCS_TRIG_STARTUP_1);
        snd_soc_update_bits(codec, WM8993_DC_SERVO_1,
                    WM8993_DCS_TIMER_PERIOD_01_MASK, 0xa);

        reg |= WM8993_HPOUT1R_OUTP | WM8993_HPOUT1R_RMV_SHORT |
            WM8993_HPOUT1L_OUTP | WM8993_HPOUT1L_RMV_SHORT;
        //printk("wm8993_write: WM8993_ANALOGUE_HP_0 at %d", __LINE__);
        wm8993_write(codec, WM8993_ANALOGUE_HP_0, reg);
        break;

    case SND_SOC_DAPM_PRE_PMD:
        reg &= ~(WM8993_HPOUT1L_RMV_SHORT |
             WM8993_HPOUT1L_DLY |
             WM8993_HPOUT1L_OUTP |
             WM8993_HPOUT1R_RMV_SHORT |
             WM8993_HPOUT1R_DLY |
             WM8993_HPOUT1R_OUTP);

        snd_soc_update_bits(codec, WM8993_DC_SERVO_1,
                    WM8993_DCS_TIMER_PERIOD_01_MASK, 0);
        snd_soc_update_bits(codec, WM8993_DC_SERVO_0,
                    WM8993_DCS_ENA_CHAN_0 |
                    WM8993_DCS_ENA_CHAN_1, 0);

		//printk("wm8993_write: WM8993_ANALOGUE_HP_0 at %d", __LINE__);
        wm8993_write(codec, WM8993_ANALOGUE_HP_0, reg);
        snd_soc_update_bits(codec, WM8993_POWER_MANAGEMENT_1,
                    WM8993_HPOUT1L_ENA | WM8993_HPOUT1R_ENA,
                    0);

        snd_soc_update_bits(codec, WM8993_CHARGE_PUMP_1,
                    WM8993_CP_ENA, 0);
        break;
    }

    return 0;
}

static const struct snd_kcontrol_new in1l_pga[] = {
SOC_DAPM_SINGLE("IN1LP Switch", WM8993_INPUT_MIXER2, 5, 1, 0),
SOC_DAPM_SINGLE("IN1LN Switch", WM8993_INPUT_MIXER2, 4, 1, 0),
};

static const struct snd_kcontrol_new in1r_pga[] = {
SOC_DAPM_SINGLE("IN1RP Switch", WM8993_INPUT_MIXER2, 1, 1, 0),
SOC_DAPM_SINGLE("IN1RN Switch", WM8993_INPUT_MIXER2, 0, 1, 0),
};

static const struct snd_kcontrol_new in2l_pga[] = {
SOC_DAPM_SINGLE("IN2LP Switch", WM8993_INPUT_MIXER2, 7, 1, 0),
SOC_DAPM_SINGLE("IN2LN Switch", WM8993_INPUT_MIXER2, 6, 1, 0),
};

static const struct snd_kcontrol_new in2r_pga[] = {
SOC_DAPM_SINGLE("IN2RP Switch", WM8993_INPUT_MIXER2, 3, 1, 0),
SOC_DAPM_SINGLE("IN2RN Switch", WM8993_INPUT_MIXER2, 2, 1, 0),
};

static const struct snd_kcontrol_new mixinl[] = {
SOC_DAPM_SINGLE("IN2L Switch", WM8993_INPUT_MIXER3, 8, 1, 0),
SOC_DAPM_SINGLE("IN1L Switch", WM8993_INPUT_MIXER3, 5, 1, 0),
};

static const struct snd_kcontrol_new mixinr[] = {
SOC_DAPM_SINGLE("IN2R Switch", WM8993_INPUT_MIXER4, 8, 1, 0),
SOC_DAPM_SINGLE("IN1R Switch", WM8993_INPUT_MIXER4, 5, 1, 0),
};

static const struct snd_kcontrol_new left_output_mixer[] = {
SOC_DAPM_SINGLE("Right Input Switch", WM8993_OUTPUT_MIXER1, 7, 1, 0),
SOC_DAPM_SINGLE("Left Input Switch", WM8993_OUTPUT_MIXER1, 6, 1, 0),
SOC_DAPM_SINGLE("IN2RN Switch", WM8993_OUTPUT_MIXER1, 5, 1, 0),
SOC_DAPM_SINGLE("IN2LN Switch", WM8993_OUTPUT_MIXER1, 4, 1, 0),
SOC_DAPM_SINGLE("IN2LP Switch", WM8993_OUTPUT_MIXER1, 1, 1, 0),
SOC_DAPM_SINGLE("IN1R Switch", WM8993_OUTPUT_MIXER1, 3, 1, 0),
SOC_DAPM_SINGLE("IN1L Switch", WM8993_OUTPUT_MIXER1, 2, 1, 0),
SOC_DAPM_SINGLE("DAC Switch", WM8993_OUTPUT_MIXER1, 0, 1, 0),
};

static const struct snd_kcontrol_new right_output_mixer[] = {
SOC_DAPM_SINGLE("Left Input Switch", WM8993_OUTPUT_MIXER2, 7, 1, 0),
SOC_DAPM_SINGLE("Right Input Switch", WM8993_OUTPUT_MIXER2, 6, 1, 0),
SOC_DAPM_SINGLE("IN2LN Switch", WM8993_OUTPUT_MIXER2, 5, 1, 0),
SOC_DAPM_SINGLE("IN2RN Switch", WM8993_OUTPUT_MIXER2, 4, 1, 0),
SOC_DAPM_SINGLE("IN1L Switch", WM8993_OUTPUT_MIXER2, 3, 1, 0),
SOC_DAPM_SINGLE("IN1R Switch", WM8993_OUTPUT_MIXER2, 2, 1, 0),
SOC_DAPM_SINGLE("IN2RP Switch", WM8993_OUTPUT_MIXER2, 1, 1, 0),
SOC_DAPM_SINGLE("DAC Switch", WM8993_OUTPUT_MIXER2, 0, 1, 0),
};

static const struct snd_kcontrol_new earpiece_mixer[] = {
SOC_DAPM_SINGLE("Direct Voice Switch", WM8993_HPOUT2_MIXER, 5, 1, 0),
SOC_DAPM_SINGLE("Left Output Switch", WM8993_HPOUT2_MIXER, 4, 1, 0),
SOC_DAPM_SINGLE("Right Output Switch", WM8993_HPOUT2_MIXER, 3, 1, 0),
};

static const struct snd_kcontrol_new left_speaker_mixer[] = {
SOC_DAPM_SINGLE("Input Switch", WM8993_SPEAKER_MIXER, 7, 1, 0),
SOC_DAPM_SINGLE("IN1LP Switch", WM8993_SPEAKER_MIXER, 5, 1, 0),
SOC_DAPM_SINGLE("Output Switch", WM8993_SPEAKER_MIXER, 3, 1, 0),
SOC_DAPM_SINGLE("DAC Switch", WM8993_SPEAKER_MIXER, 6, 1, 0),
};

static const struct snd_kcontrol_new right_speaker_mixer[] = {
SOC_DAPM_SINGLE("Input Switch", WM8993_SPEAKER_MIXER, 6, 1, 0),
SOC_DAPM_SINGLE("IN1RP Switch", WM8993_SPEAKER_MIXER, 4, 1, 0),
SOC_DAPM_SINGLE("Output Switch", WM8993_SPEAKER_MIXER, 2, 1, 0),
SOC_DAPM_SINGLE("DAC Switch", WM8993_SPEAKER_MIXER, 0, 1, 0),
};

static const struct snd_kcontrol_new left_speaker_boost[] = {
SOC_DAPM_SINGLE("Direct Voice Switch", WM8993_SPKOUT_MIXERS, 5, 1, 0),
SOC_DAPM_SINGLE("SPKL Switch", WM8993_SPKOUT_MIXERS, 4, 1, 0),
SOC_DAPM_SINGLE("SPKR Switch", WM8993_SPKOUT_MIXERS, 3, 1, 0),
};

static const struct snd_kcontrol_new right_speaker_boost[] = {
SOC_DAPM_SINGLE("Direct Voice Switch", WM8993_SPKOUT_MIXERS, 2, 1, 0),
SOC_DAPM_SINGLE("SPKL Switch", WM8993_SPKOUT_MIXERS, 1, 1, 0),
SOC_DAPM_SINGLE("SPKR Switch", WM8993_SPKOUT_MIXERS, 0, 1, 0),
};

static const char *hp_mux_text[] = {
    "Mixer",
    "DAC",
};

static const struct soc_enum hpl_enum =
    SOC_ENUM_SINGLE(WM8993_OUTPUT_MIXER1, 8, 2, hp_mux_text);

static const struct snd_kcontrol_new hpl_mux =
    SOC_DAPM_ENUM_W("Left Headphone Mux", hpl_enum);

static const struct soc_enum hpr_enum =
    SOC_ENUM_SINGLE(WM8993_OUTPUT_MIXER2, 8, 2, hp_mux_text);

static const struct snd_kcontrol_new hpr_mux =
    SOC_DAPM_ENUM_W("Right Headphone Mux", hpr_enum);

static const struct snd_kcontrol_new line1_mix[] = {
SOC_DAPM_SINGLE("IN1R Switch", WM8993_LINE_MIXER1, 2, 1, 0),
SOC_DAPM_SINGLE("IN1L Switch", WM8993_LINE_MIXER1, 1, 1, 0),
SOC_DAPM_SINGLE("Output Switch", WM8993_LINE_MIXER1, 0, 1, 0),
};

static const struct snd_kcontrol_new line1n_mix[] = {
SOC_DAPM_SINGLE("Left Output Switch", WM8993_LINE_MIXER1, 6, 1, 0),
SOC_DAPM_SINGLE("Right Output Switch", WM8993_LINE_MIXER1, 5, 1, 0),
};

static const struct snd_kcontrol_new line1p_mix[] = {
SOC_DAPM_SINGLE("Left Output Switch", WM8993_LINE_MIXER1, 0, 1, 0),
};

static const struct snd_kcontrol_new line2_mix[] = {
SOC_DAPM_SINGLE("IN2R Switch", WM8993_LINE_MIXER2, 2, 1, 0),
SOC_DAPM_SINGLE("IN2L Switch", WM8993_LINE_MIXER2, 1, 1, 0),
SOC_DAPM_SINGLE("Output Switch", WM8993_LINE_MIXER2, 0, 1, 0),
};

static const struct snd_kcontrol_new line2n_mix[] = {
SOC_DAPM_SINGLE("Left Output Switch", WM8993_LINE_MIXER2, 6, 1, 0),
SOC_DAPM_SINGLE("Right Output Switch", WM8993_LINE_MIXER2, 5, 1, 0),
};

static const struct snd_kcontrol_new line2p_mix[] = {
SOC_DAPM_SINGLE("Right Output Switch", WM8993_LINE_MIXER2, 0, 1, 0),
};

static const struct snd_soc_dapm_widget wm8993_dapm_widgets[] = {
SND_SOC_DAPM_INPUT("IN1LN"),
SND_SOC_DAPM_INPUT("IN1LP"),
SND_SOC_DAPM_INPUT("IN2LN"),
SND_SOC_DAPM_INPUT("IN2LP/VXRN"),
SND_SOC_DAPM_INPUT("IN1RN"),
SND_SOC_DAPM_INPUT("IN1RP"),
SND_SOC_DAPM_INPUT("IN2RN"),
SND_SOC_DAPM_INPUT("IN2RP/VXRP"),

SND_SOC_DAPM_SUPPLY("CLK_SYS", WM8993_BUS_CONTROL_1, 1, 0, clk_sys_event,
            SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
SND_SOC_DAPM_SUPPLY("TOCLK", WM8993_CLOCKING_1, 14, 0, NULL, 0),
SND_SOC_DAPM_SUPPLY("CLK_DSP", WM8993_CLOCKING_3, 0, 0, NULL, 0),

SND_SOC_DAPM_MICBIAS("MICBIAS2", WM8993_POWER_MANAGEMENT_1, 5, 0),
SND_SOC_DAPM_MICBIAS("MICBIAS1", WM8993_POWER_MANAGEMENT_1, 4, 0),

SND_SOC_DAPM_MIXER("IN1L PGA", WM8993_POWER_MANAGEMENT_2, 6, 0,
           in1l_pga, ARRAY_SIZE(in1l_pga)),
SND_SOC_DAPM_MIXER("IN1R PGA", WM8993_POWER_MANAGEMENT_2, 4, 0,
           in1r_pga, ARRAY_SIZE(in1r_pga)),

SND_SOC_DAPM_MIXER("IN2L PGA", WM8993_POWER_MANAGEMENT_2, 7, 0,
           in2l_pga, ARRAY_SIZE(in2l_pga)),
SND_SOC_DAPM_MIXER("IN2R PGA", WM8993_POWER_MANAGEMENT_2, 5, 0,
           in2r_pga, ARRAY_SIZE(in2r_pga)),

/* Dummy widgets to represent differential paths */
SND_SOC_DAPM_PGA("Direct Voice", SND_SOC_NOPM, 0, 0, NULL, 0),

SND_SOC_DAPM_MIXER("MIXINL", WM8993_POWER_MANAGEMENT_2, 9, 0,
           mixinl, ARRAY_SIZE(mixinl)),
SND_SOC_DAPM_MIXER("MIXINR", WM8993_POWER_MANAGEMENT_2, 8, 0,
           mixinr, ARRAY_SIZE(mixinr)),

SND_SOC_DAPM_ADC("ADCL", "Capture", WM8993_POWER_MANAGEMENT_2, 1, 0),
SND_SOC_DAPM_ADC("ADCR", "Capture", WM8993_POWER_MANAGEMENT_2, 0, 0),

SND_SOC_DAPM_DAC("DACL", "Playback", WM8993_POWER_MANAGEMENT_3, 1, 0),
SND_SOC_DAPM_DAC("DACR", "Playback", WM8993_POWER_MANAGEMENT_3, 0, 0),

SND_SOC_DAPM_MIXER("Left Output Mixer", WM8993_POWER_MANAGEMENT_3, 5, 0,
           left_output_mixer, ARRAY_SIZE(left_output_mixer)),
SND_SOC_DAPM_MIXER("Right Output Mixer", WM8993_POWER_MANAGEMENT_3, 4, 0,
           right_output_mixer, ARRAY_SIZE(right_output_mixer)),

SND_SOC_DAPM_PGA("Left Output PGA", WM8993_POWER_MANAGEMENT_3, 7, 0, NULL, 0),
SND_SOC_DAPM_PGA("Right Output PGA", WM8993_POWER_MANAGEMENT_3, 6, 0, NULL, 0),

SND_SOC_DAPM_MIXER("Earpiece Mixer", SND_SOC_NOPM, 0, 0,
           earpiece_mixer, ARRAY_SIZE(earpiece_mixer)),
SND_SOC_DAPM_PGA_E("Earpiece Driver", WM8993_POWER_MANAGEMENT_1, 11, 0,
           NULL, 0, wm8993_earpiece_event,
           SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

SND_SOC_DAPM_MIXER("SPKL", WM8993_POWER_MANAGEMENT_3, 8, 0,
           left_speaker_mixer, ARRAY_SIZE(left_speaker_mixer)),
SND_SOC_DAPM_MIXER("SPKR", WM8993_POWER_MANAGEMENT_3, 9, 0,
           right_speaker_mixer, ARRAY_SIZE(right_speaker_mixer)),

SND_SOC_DAPM_MIXER("SPKL Boost", SND_SOC_NOPM, 0, 0,
           left_speaker_boost, ARRAY_SIZE(left_speaker_boost)),
SND_SOC_DAPM_MIXER("SPKR Boost", SND_SOC_NOPM, 0, 0,
           right_speaker_boost, ARRAY_SIZE(right_speaker_boost)),

SND_SOC_DAPM_PGA("SPKL Driver", WM8993_POWER_MANAGEMENT_1, 12, 0,
         NULL, 0),
SND_SOC_DAPM_PGA("SPKR Driver", WM8993_POWER_MANAGEMENT_1, 13, 0,
         NULL, 0),

SND_SOC_DAPM_MUX("Left Headphone Mux", SND_SOC_NOPM, 0, 0, &hpl_mux),
SND_SOC_DAPM_MUX("Right Headphone Mux", SND_SOC_NOPM, 0, 0, &hpr_mux),
SND_SOC_DAPM_PGA_E("Headphone PGA", SND_SOC_NOPM, 0, 0,
           NULL, 0,
           hp_event, SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),

SND_SOC_DAPM_MIXER("LINEOUT1 Mixer", SND_SOC_NOPM, 0, 0,
           line1_mix, ARRAY_SIZE(line1_mix)),
SND_SOC_DAPM_MIXER("LINEOUT2 Mixer", SND_SOC_NOPM, 0, 0,
           line2_mix, ARRAY_SIZE(line2_mix)),

SND_SOC_DAPM_MIXER("LINEOUT1N Mixer", SND_SOC_NOPM, 0, 0,
           line1n_mix, ARRAY_SIZE(line1n_mix)),
SND_SOC_DAPM_MIXER("LINEOUT1P Mixer", SND_SOC_NOPM, 0, 0,
           line1p_mix, ARRAY_SIZE(line1p_mix)),
SND_SOC_DAPM_MIXER("LINEOUT2N Mixer", SND_SOC_NOPM, 0, 0,
           line2n_mix, ARRAY_SIZE(line2n_mix)),
SND_SOC_DAPM_MIXER("LINEOUT2P Mixer", SND_SOC_NOPM, 0, 0,
           line2p_mix, ARRAY_SIZE(line2p_mix)),

SND_SOC_DAPM_PGA("LINEOUT1N Driver", WM8993_POWER_MANAGEMENT_3, 13, 0,
         NULL, 0),
SND_SOC_DAPM_PGA("LINEOUT1P Driver", WM8993_POWER_MANAGEMENT_3, 12, 0,
         NULL, 0),
SND_SOC_DAPM_PGA("LINEOUT2N Driver", WM8993_POWER_MANAGEMENT_3, 11, 0,
         NULL, 0),
SND_SOC_DAPM_PGA("LINEOUT2P Driver", WM8993_POWER_MANAGEMENT_3, 10, 0,
         NULL, 0),

SND_SOC_DAPM_OUTPUT("SPKOUTLP"),
SND_SOC_DAPM_OUTPUT("SPKOUTLN"),
SND_SOC_DAPM_OUTPUT("SPKOUTRP"),
SND_SOC_DAPM_OUTPUT("SPKOUTRN"),
SND_SOC_DAPM_OUTPUT("HPOUT1L"),
SND_SOC_DAPM_OUTPUT("HPOUT1R"),
SND_SOC_DAPM_OUTPUT("HPOUT2P"),
SND_SOC_DAPM_OUTPUT("HPOUT2N"),
SND_SOC_DAPM_OUTPUT("LINEOUT1P"),
SND_SOC_DAPM_OUTPUT("LINEOUT1N"),
SND_SOC_DAPM_OUTPUT("LINEOUT2P"),
SND_SOC_DAPM_OUTPUT("LINEOUT2N"),
};

static const struct snd_soc_dapm_route routes[] = {
    { "IN1L PGA", "IN1LP Switch", "IN1LP" },
    { "IN1L PGA", "IN1LN Switch", "IN1LN" },

    { "IN1R PGA", "IN1RP Switch", "IN1RP" },
    { "IN1R PGA", "IN1RN Switch", "IN1RN" },

    { "IN2L PGA", "IN2LP Switch", "IN2LP/VXRN" },
    { "IN2L PGA", "IN2LN Switch", "IN2LN" },

    { "IN2R PGA", "IN2RP Switch", "IN2RP/VXRP" },
    { "IN2R PGA", "IN2RN Switch", "IN2RN" },

    { "Direct Voice", NULL, "IN2LP/VXRN" },
    { "Direct Voice", NULL, "IN2RP/VXRP" },

    { "MIXINL", "IN1L Switch", "IN1L PGA" },
    { "MIXINL", "IN2L Switch", "IN2L PGA" },
    { "MIXINL", NULL, "Direct Voice" },
    { "MIXINL", NULL, "IN1LP" },
    { "MIXINL", NULL, "Left Output Mixer" },

    { "MIXINR", "IN1R Switch", "IN1R PGA" },
    { "MIXINR", "IN2R Switch", "IN2R PGA" },
    { "MIXINR", NULL, "Direct Voice" },
    { "MIXINR", NULL, "IN1RP" },
    { "MIXINR", NULL, "Right Output Mixer" },

    { "ADCL", NULL, "MIXINL" },
    { "ADCL", NULL, "CLK_SYS" },
    { "ADCL", NULL, "CLK_DSP" },
    { "ADCR", NULL, "MIXINR" },
    { "ADCR", NULL, "CLK_SYS" },
    { "ADCR", NULL, "CLK_DSP" },

    { "DACL", NULL, "CLK_SYS" },
    { "DACL", NULL, "CLK_DSP" },
    { "DACR", NULL, "CLK_SYS" },
    { "DACR", NULL, "CLK_DSP" },

    { "Left Output Mixer", "Left Input Switch", "MIXINL" },
    { "Left Output Mixer", "Right Input Switch", "MIXINR" },
    { "Left Output Mixer", "IN2RN Switch", "IN2RN" },
    { "Left Output Mixer", "IN2LN Switch", "IN2LN" },
    { "Left Output Mixer", "IN2LP Switch", "IN2LP/VXRN" },
    { "Left Output Mixer", "IN1L Switch", "IN1L PGA" },
    { "Left Output Mixer", "IN1R Switch", "IN1R PGA" },
    { "Left Output Mixer", "DAC Switch", "DACL" },

    { "Right Output Mixer", "Left Input Switch", "MIXINL" },
    { "Right Output Mixer", "Right Input Switch", "MIXINR" },
    { "Right Output Mixer", "IN2LN Switch", "IN2LN" },
    { "Right Output Mixer", "IN2RN Switch", "IN2RN" },
    { "Right Output Mixer", "IN2RP Switch", "IN2RP/VXRP" },
    { "Right Output Mixer", "IN1L Switch", "IN1L PGA" },
    { "Right Output Mixer", "IN1R Switch", "IN1R PGA" },
    { "Right Output Mixer", "DAC Switch", "DACR" },

    { "Left Output PGA", NULL, "Left Output Mixer" },
    { "Left Output PGA", NULL, "CLK_SYS" },
    { "Left Output PGA", NULL, "TOCLK" },

    { "Right Output PGA", NULL, "Right Output Mixer" },
    { "Right Output PGA", NULL, "CLK_SYS" },
    { "Right Output PGA", NULL, "TOCLK" },

    { "Earpiece Mixer", "Direct Voice Switch", "Direct Voice" },
    { "Earpiece Mixer", "Left Output Switch", "Left Output PGA" },
    { "Earpiece Mixer", "Right Output Switch", "Right Output PGA" },

    { "Earpiece Driver", NULL, "Earpiece Mixer" },
    { "HPOUT2N", NULL, "Earpiece Driver" },
    { "HPOUT2P", NULL, "Earpiece Driver" },

    { "SPKL", "Input Switch", "MIXINL" },
    { "SPKL", "IN1LP Switch", "IN1LP" },
    { "SPKL", "Output Switch", "Left Output Mixer" },
    { "SPKL", "DAC Switch", "DACL" },
    { "SPKL", NULL, "CLK_SYS" },
    { "SPKL", NULL, "TOCLK" },

    { "SPKR", "Input Switch", "MIXINR" },
    { "SPKR", "IN1RP Switch", "IN1RP" },
    { "SPKR", "Output Switch", "Right Output Mixer" },
    { "SPKR", "DAC Switch", "DACR" },
    { "SPKR", NULL, "CLK_SYS" },
    { "SPKR", NULL, "TOCLK" },

    { "SPKL Boost", "Direct Voice Switch", "Direct Voice" },
    { "SPKL Boost", "SPKL Switch", "SPKL" },
    { "SPKL Boost", "SPKR Switch", "SPKR" },

    { "SPKR Boost", "Direct Voice Switch", "Direct Voice" },
    { "SPKR Boost", "SPKR Switch", "SPKR" },
    { "SPKR Boost", "SPKL Switch", "SPKL" },

    { "SPKL Driver", NULL, "SPKL Boost" },
    { "SPKL Driver", NULL, "CLK_SYS" },

    { "SPKR Driver", NULL, "SPKR Boost" },
    { "SPKR Driver", NULL, "CLK_SYS" },

    { "SPKOUTLP", NULL, "SPKL Driver" },
    { "SPKOUTLN", NULL, "SPKL Driver" },
    { "SPKOUTRP", NULL, "SPKR Driver" },
    { "SPKOUTRN", NULL, "SPKR Driver" },

    { "Left Headphone Mux", "DAC", "DACL" },
    { "Left Headphone Mux", "Mixer", "Left Output Mixer" },
    { "Right Headphone Mux", "DAC", "DACR" },
    { "Right Headphone Mux", "Mixer", "Right Output Mixer" },

    { "Headphone PGA", NULL, "Left Headphone Mux" },
    { "Headphone PGA", NULL, "Right Headphone Mux" },
    { "Headphone PGA", NULL, "CLK_SYS" },
    { "Headphone PGA", NULL, "TOCLK" },

    { "HPOUT1L", NULL, "Headphone PGA" },
    { "HPOUT1R", NULL, "Headphone PGA" },

    { "LINEOUT1N", NULL, "LINEOUT1N Driver" },
    { "LINEOUT1P", NULL, "LINEOUT1P Driver" },
    { "LINEOUT2N", NULL, "LINEOUT2N Driver" },
    { "LINEOUT2P", NULL, "LINEOUT2P Driver" },
};

static const struct snd_soc_dapm_route lineout1_diff_routes[] = {
    { "LINEOUT1 Mixer", "IN1L Switch", "IN1L PGA" },
    { "LINEOUT1 Mixer", "IN1R Switch", "IN1R PGA" },
    { "LINEOUT1 Mixer", "Output Switch", "Left Output Mixer" },

    { "LINEOUT1N Driver", NULL, "LINEOUT1 Mixer" },
    { "LINEOUT1P Driver", NULL, "LINEOUT1 Mixer" },
};

static const struct snd_soc_dapm_route lineout1_se_routes[] = {
    { "LINEOUT1N Mixer", "Left Output Switch", "Left Output Mixer" },
    { "LINEOUT1N Mixer", "Right Output Switch", "Left Output Mixer" },

    { "LINEOUT1P Mixer", "Left Output Switch", "Left Output Mixer" },

    { "LINEOUT1N Driver", NULL, "LINEOUT1N Mixer" },
    { "LINEOUT1P Driver", NULL, "LINEOUT1P Mixer" },
};

static const struct snd_soc_dapm_route lineout2_diff_routes[] = {
    { "LINEOUT2 Mixer", "IN2L Switch", "IN2L PGA" },
    { "LINEOUT2 Mixer", "IN2R Switch", "IN2R PGA" },
    { "LINEOUT2 Mixer", "Output Switch", "Right Output Mixer" },

    { "LINEOUT2N Driver", NULL, "LINEOUT2 Mixer" },
    { "LINEOUT2P Driver", NULL, "LINEOUT2 Mixer" },
};

static const struct snd_soc_dapm_route lineout2_se_routes[] = {
    { "LINEOUT2N Mixer", "Left Output Switch", "Left Output Mixer" },
    { "LINEOUT2N Mixer", "Right Output Switch", "Left Output Mixer" },

    { "LINEOUT2P Mixer", "Right Output Switch", "Right Output Mixer" },

    { "LINEOUT2N Driver", NULL, "LINEOUT2N Mixer" },
    { "LINEOUT2P Driver", NULL, "LINEOUT2P Mixer" },
};

static int wm8993_set_bias_level(struct snd_soc_codec *codec,
                 enum snd_soc_bias_level level)
{
    struct wm8993_priv *wm8993 = codec->private_data;

    switch (level) {
    case SND_SOC_BIAS_ON:
    case SND_SOC_BIAS_PREPARE:
        /* VMID=2*40k */
        snd_soc_update_bits(codec, WM8993_POWER_MANAGEMENT_1,
                    WM8993_VMID_SEL_MASK, 0x2);
        snd_soc_update_bits(codec, WM8993_POWER_MANAGEMENT_2,
                    WM8993_TSHUT_ENA, WM8993_TSHUT_ENA);
        break;

    case SND_SOC_BIAS_STANDBY:
        if (codec->bias_level == SND_SOC_BIAS_OFF) {
            /* Bring up VMID with fast soft start */
            snd_soc_update_bits(codec, WM8993_ANTIPOP2,
                        WM8993_STARTUP_BIAS_ENA |
                        WM8993_VMID_BUF_ENA |
                        WM8993_VMID_RAMP_MASK |
                        WM8993_BIAS_SRC,
                        WM8993_STARTUP_BIAS_ENA |
                        WM8993_VMID_BUF_ENA |
                        WM8993_VMID_RAMP_MASK |
                        WM8993_BIAS_SRC);

            /* If either line output is single ended we
             * need the VMID buffer */
            if (!wm8993->pdata.lineout1_diff ||
                !wm8993->pdata.lineout2_diff)
                snd_soc_update_bits(codec, WM8993_ANTIPOP1,
                         WM8993_LINEOUT_VMID_BUF_ENA,
                         WM8993_LINEOUT_VMID_BUF_ENA);

            /* VMID=2*40k */
            snd_soc_update_bits(codec, WM8993_POWER_MANAGEMENT_1,
                        WM8993_VMID_SEL_MASK |
                        WM8993_BIAS_ENA,
                        WM8993_BIAS_ENA | 0x2);
            msleep(32);

            /* Switch to normal bias */
            snd_soc_update_bits(codec, WM8993_ANTIPOP2,
                        WM8993_BIAS_SRC |
                        WM8993_STARTUP_BIAS_ENA, 0);
        }

        /* VMID=2*240k */
        snd_soc_update_bits(codec, WM8993_POWER_MANAGEMENT_1,
                    WM8993_VMID_SEL_MASK, 0x4);

        snd_soc_update_bits(codec, WM8993_POWER_MANAGEMENT_2,
                    WM8993_TSHUT_ENA, 0);
        break;

    case SND_SOC_BIAS_OFF:
        snd_soc_update_bits(codec, WM8993_ANTIPOP1,
                    WM8993_LINEOUT_VMID_BUF_ENA, 0);

        snd_soc_update_bits(codec, WM8993_POWER_MANAGEMENT_1,
                    WM8993_VMID_SEL_MASK | WM8993_BIAS_ENA,
                    0);
        break;
    }

    codec->bias_level = level;

    return 0;
}

static int wm8993_set_sysclk(struct snd_soc_dai *codec_dai,
                 int clk_id, unsigned int freq, int dir)
{
    struct snd_soc_codec *codec = codec_dai->codec;
    struct wm8993_priv *wm8993 = codec->private_data;

    switch (clk_id) {
    case WM8993_SYSCLK_MCLK:
        wm8993->mclk_rate = freq;
    case WM8993_SYSCLK_FLL:
        wm8993->sysclk_source = clk_id;
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

static int wm8993_set_dai_fmt(struct snd_soc_dai *dai,
                  unsigned int fmt)
{
    struct snd_soc_codec *codec = dai->codec;
    struct wm8993_priv *wm8993 = codec->private_data;
    unsigned int aif1 = wm8993_read(codec, WM8993_AUDIO_INTERFACE_1);
    unsigned int aif4 = wm8993_read(codec, WM8993_AUDIO_INTERFACE_4);

    aif1 &= ~(WM8993_BCLK_DIR | WM8993_AIF_BCLK_INV |
          WM8993_AIF_LRCLK_INV | WM8993_AIF_FMT_MASK);
    aif4 &= ~WM8993_LRCLK_DIR;

    switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
    case SND_SOC_DAIFMT_CBS_CFS:
        wm8993->master = 0;
        break;
    case SND_SOC_DAIFMT_CBS_CFM:
        aif4 |= WM8993_LRCLK_DIR;
        wm8993->master = 1;
        break;
    case SND_SOC_DAIFMT_CBM_CFS:
        aif1 |= WM8993_BCLK_DIR;
        wm8993->master = 1;
        break;
    case SND_SOC_DAIFMT_CBM_CFM:
        aif1 |= WM8993_BCLK_DIR;
        aif4 |= WM8993_LRCLK_DIR;
        wm8993->master = 1;
        break;
    default:
        return -EINVAL;
    }

    switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
    case SND_SOC_DAIFMT_DSP_B:
        aif1 |= WM8993_AIF_LRCLK_INV;
    case SND_SOC_DAIFMT_DSP_A:
        aif1 |= 0x18;
        break;
    case SND_SOC_DAIFMT_I2S:
        aif1 |= 0x10;
        break;
    case SND_SOC_DAIFMT_RIGHT_J:
        break;
    case SND_SOC_DAIFMT_LEFT_J:
        aif1 |= 0x8;
        break;
    default:
        return -EINVAL;
    }

    switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
    case SND_SOC_DAIFMT_DSP_A:
    case SND_SOC_DAIFMT_DSP_B:
        /* frame inversion not valid for DSP modes */
        switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
        case SND_SOC_DAIFMT_NB_NF:
            break;
        case SND_SOC_DAIFMT_IB_NF:
            aif1 |= WM8993_AIF_BCLK_INV;
            break;
        default:
            return -EINVAL;
        }
        break;

    case SND_SOC_DAIFMT_I2S:
    case SND_SOC_DAIFMT_RIGHT_J:
    case SND_SOC_DAIFMT_LEFT_J:
        switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
        case SND_SOC_DAIFMT_NB_NF:
            break;
        case SND_SOC_DAIFMT_IB_IF:
            aif1 |= WM8993_AIF_BCLK_INV | WM8993_AIF_LRCLK_INV;
            break;
        case SND_SOC_DAIFMT_IB_NF:
            aif1 |= WM8993_AIF_BCLK_INV;
            break;
        case SND_SOC_DAIFMT_NB_IF:
            aif1 |= WM8993_AIF_LRCLK_INV;
            break;
        default:
            return -EINVAL;
        }
        break;
    default:
        return -EINVAL;
    }

	//printk("wm8993_write: WM8993_AUDIO_INTERFACE_1 at %d", __LINE__);
    wm8993_write(codec, WM8993_AUDIO_INTERFACE_1, aif1);
    //printk("wm8993_write: WM8993_AUDIO_INTERFACE_4 at %d", __LINE__);
    wm8993_write(codec, WM8993_AUDIO_INTERFACE_4, aif4);

    return 0;
}

static int wm8993_hw_params(struct snd_pcm_substream *substream,
                struct snd_pcm_hw_params *params,
                struct snd_soc_dai *dai)
{
    struct snd_soc_codec *codec = dai->codec;
    struct wm8993_priv *wm8993 = codec->private_data;
    int ret, i, best, best_val, cur_val;
    unsigned int clocking1, clocking3, aif1, aif4;

    clocking1 = wm8993_read(codec, WM8993_CLOCKING_1);
    clocking1 &= ~WM8993_BCLK_DIV_MASK;

    clocking3 = wm8993_read(codec, WM8993_CLOCKING_3);
    clocking3 &= ~(WM8993_CLK_SYS_RATE_MASK | WM8993_SAMPLE_RATE_MASK);

    aif1 = wm8993_read(codec, WM8993_AUDIO_INTERFACE_1);
    aif1 &= ~WM8993_AIF_WL_MASK;

    aif4 = wm8993_read(codec, WM8993_AUDIO_INTERFACE_4);
    aif4 &= ~WM8993_LRCLK_RATE_MASK;

    /* What BCLK do we need? */
    wm8993->fs = params_rate(params);
    wm8993->bclk = 2 * wm8993->fs;
    switch (params_format(params)) {
    case SNDRV_PCM_FORMAT_S16_LE:
        wm8993->bclk *= 16;
        break;
    case SNDRV_PCM_FORMAT_S20_3LE:
        wm8993->bclk *= 20;
        aif1 |= 0x8;
        break;
    case SNDRV_PCM_FORMAT_S24_LE:
        wm8993->bclk *= 24;
        aif1 |= 0x10;
        break;
    case SNDRV_PCM_FORMAT_S32_LE:
        wm8993->bclk *= 32;
        aif1 |= 0x18;
        break;
    default:
        return -EINVAL;
    }

    printk("Target BCLK is %dHz\n", wm8993->bclk);

    ret = configure_clock(codec);
    if (ret != 0)
        return ret;

    /* Select nearest CLK_SYS_RATE */
    best = 0;
    best_val = abs((wm8993->sysclk_rate / clk_sys_rates[0].ratio)
               - wm8993->fs);
    for (i = 1; i < ARRAY_SIZE(clk_sys_rates); i++) {
        cur_val = abs((wm8993->sysclk_rate /
                   clk_sys_rates[i].ratio) - wm8993->fs);;
        if (cur_val < best_val) {
            best = i;
            best_val = cur_val;
        }
    }
    printk("Selected CLK_SYS_RATIO of %d\n",
        clk_sys_rates[best].ratio);
    clocking3 |= (clk_sys_rates[best].clk_sys_rate
              << WM8993_CLK_SYS_RATE_SHIFT);

    /* SAMPLE_RATE */
    best = 0;
    best_val = abs(wm8993->fs - sample_rates[0].rate);
    for (i = 1; i < ARRAY_SIZE(sample_rates); i++) {
        /* Closest match */
        cur_val = abs(wm8993->fs - sample_rates[i].rate);
        if (cur_val < best_val) {
            best = i;
            best_val = cur_val;
        }
    }
    printk("Selected SAMPLE_RATE of %dHz\n",
        sample_rates[best].rate);
    clocking3 |= (sample_rates[i].sample_rate << WM8993_SAMPLE_RATE_SHIFT);

    /* BCLK_DIV */
    best = 0;
    best_val = INT_MAX;
    for (i = 0; i < ARRAY_SIZE(bclk_divs); i++) {
        cur_val = ((wm8993->sysclk_rate * 10) / bclk_divs[i].div)
            - wm8993->bclk;
        if (cur_val < 0) /* Table is sorted */
            break;
        if (cur_val < best_val) {
            best = i;
            best_val = cur_val;
        }
    }
    wm8993->bclk = (wm8993->sysclk_rate * 10) / bclk_divs[best].div;
    printk("Selected BCLK_DIV of %d for %dHz BCLK\n",
        bclk_divs[best].div, wm8993->bclk);
    clocking1 |= bclk_divs[best].bclk_div << WM8993_BCLK_DIV_SHIFT;

    /* LRCLK is a simple fraction of BCLK */
    printk("LRCLK_RATE is %d\n", wm8993->bclk / wm8993->fs);
    aif4 |= wm8993->bclk / wm8993->fs;

	//printk("wm8993_write: WM8993_CLOCKING_1 at %d", __LINE__);
    wm8993_write(codec, WM8993_CLOCKING_1, clocking1);
    //printk("wm8993_write: WM8993_CLOCKING_3 at %d", __LINE__);
    wm8993_write(codec, WM8993_CLOCKING_3, clocking3);
    //printk("wm8993_write: WM8993_AUDIO_INTERFACE_1 at %d", __LINE__);
    wm8993_write(codec, WM8993_AUDIO_INTERFACE_1, aif1);
    //printk("wm8993_write: WM8993_AUDIO_INTERFACE_4 at %d", __LINE__);
    wm8993_write(codec, WM8993_AUDIO_INTERFACE_4, aif4);

    /* ReTune Mobile? */
    if (wm8993->pdata.num_retune_configs) {
        u16 eq1 = wm8993_read(codec, WM8993_EQ1);
        struct wm8993_retune_mobile_setting *s;

        best = 0;
        best_val = abs(wm8993->pdata.retune_configs[0].rate
                   - wm8993->fs);
        for (i = 0; i < wm8993->pdata.num_retune_configs; i++) {
            cur_val = abs(wm8993->pdata.retune_configs[i].rate
                      - wm8993->fs);
            if (cur_val < best_val) {
                best_val = cur_val;
                best = i;
            }
        }
        s = &wm8993->pdata.retune_configs[best];

        printk("ReTune Mobile %s tuned for %dHz\n",
            s->name, s->rate);

        /* Disable EQ while we reconfigure */
        snd_soc_update_bits(codec, WM8993_EQ1, WM8993_EQ_ENA, 0);

        for (i = 1; i < ARRAY_SIZE(s->config); i++) {
        	//printk("wm8993_write: WM8993_EQ%d at %d", i+1, __LINE__);
            wm8993_write(codec, WM8993_EQ1 + i, s->config[i]);
        }

        snd_soc_update_bits(codec, WM8993_EQ1, WM8993_EQ_ENA, eq1);
    }

    return 0;
}

static int wm8993_digital_mute(struct snd_soc_dai *codec_dai, int mute)
{
    struct snd_soc_codec *codec = codec_dai->codec;
    unsigned int reg;

    reg = wm8993_read(codec, WM8993_DAC_CTRL);

    if (mute)
        reg |= WM8993_DAC_MUTE;
    else
        reg &= ~WM8993_DAC_MUTE;

	//printk("wm8993_write: WM8993_DAC_CTRL at %d", __LINE__);
    wm8993_write(codec, WM8993_DAC_CTRL, reg);

    return 0;
}

static struct snd_soc_dai_ops wm8993_ops = {
    .set_sysclk = wm8993_set_sysclk,
    .set_fmt = wm8993_set_dai_fmt,
    .hw_params = wm8993_hw_params,
    .digital_mute = wm8993_digital_mute,
    .set_pll = wm8993_set_fll,
};

#define WM8993_RATES SNDRV_PCM_RATE_8000_48000

#define WM8993_FORMATS (SNDRV_PCM_FMTBIT_S16_LE |\
            SNDRV_PCM_FMTBIT_S20_3LE |\
            SNDRV_PCM_FMTBIT_S24_LE |\
            SNDRV_PCM_FMTBIT_S32_LE)

struct snd_soc_dai wm8993_dai = {
    .name = "WM8993",
    .playback = {
        .stream_name = "Playback",
        .channels_min = 1,
        .channels_max = 2,
        .rates = WM8993_RATES,
        .formats = WM8993_FORMATS,
    },
    .capture = {
         .stream_name = "Capture",
         .channels_min = 1,
         .channels_max = 2,
         .rates = WM8993_RATES,
         .formats = WM8993_FORMATS,
     },
    .ops = &wm8993_ops,
    .symmetric_rates = 1,
};
EXPORT_SYMBOL_GPL(wm8993_dai);

static struct snd_soc_codec *wm8993_codec;

static int wm8993_probe(struct platform_device *pdev)
{
    struct snd_soc_device *socdev = platform_get_drvdata(pdev);
    struct snd_soc_codec *codec;
    struct wm8993_priv *wm8993;
    unsigned int reg;
    
    int ret = 0;

    if (!wm8993_codec) {
        printk("I2C device not yet probed\n");
        goto err;
    }

    socdev->card->codec = wm8993_codec;
    codec = wm8993_codec;
    wm8993 = codec->private_data;

    ret = snd_soc_new_pcms(socdev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1);
    if (ret < 0) {
        printk("failed to create pcms\n");
        goto err;
    }

    snd_soc_add_controls(codec, wm8993_snd_controls,
                 ARRAY_SIZE(wm8993_snd_controls));
    if (wm8993->pdata.num_retune_configs != 0) {
        printk("Using ReTune Mobile\n");
    } else {
        printk("No ReTune Mobile, using normal EQ\n");
        snd_soc_add_controls(codec, wm8993_eq_controls,
                     ARRAY_SIZE(wm8993_eq_controls));
    }

    snd_soc_dapm_new_controls(codec, wm8993_dapm_widgets,
                  ARRAY_SIZE(wm8993_dapm_widgets));

    snd_soc_dapm_add_routes(codec, routes, ARRAY_SIZE(routes));

    if (wm8993->pdata.lineout1_diff)
        snd_soc_dapm_add_routes(codec,
                    lineout1_diff_routes,
                    ARRAY_SIZE(lineout1_diff_routes));
    else
        snd_soc_dapm_add_routes(codec,
                    lineout1_se_routes,
                    ARRAY_SIZE(lineout1_se_routes));

    if (wm8993->pdata.lineout2_diff)
        snd_soc_dapm_add_routes(codec,
                    lineout2_diff_routes,
                    ARRAY_SIZE(lineout2_diff_routes));
    else
        snd_soc_dapm_add_routes(codec,
                    lineout2_se_routes,
                    ARRAY_SIZE(lineout2_se_routes));

    snd_soc_dapm_new_widgets(codec);
    
    return ret;

card_err:
    snd_soc_free_pcms(socdev);
    snd_soc_dapm_free(socdev);
err:
    return ret;
}

static int wm8993_remove(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;
	free_irq(IRQ_EINT(12), codec);
	
    snd_soc_free_pcms(socdev);
    snd_soc_dapm_free(socdev);

    return 0;
}

struct snd_soc_codec_device soc_codec_dev_wm8993 = {
    .probe =    wm8993_probe,
    .remove =       wm8993_remove,
};
EXPORT_SYMBOL_GPL(soc_codec_dev_wm8993);

static int wm8993_i2c_probe(struct i2c_client *i2c,
                const struct i2c_device_id *id)
{
    struct wm8993_priv *wm8993;
    struct snd_soc_codec *codec;
    unsigned int val;
    int ret;

    if (wm8993_codec) {
        printk("A WM8993 is already registered\n");
        return -EINVAL;
    }

    wm8993 = kzalloc(sizeof(struct wm8993_priv), GFP_KERNEL);
    if (wm8993 == NULL)
        return -ENOMEM;

    codec = &wm8993->codec;
    if (i2c->dev.platform_data)
        memcpy(&wm8993->pdata, i2c->dev.platform_data,
               sizeof(wm8993->pdata));

    mutex_init(&codec->mutex);
    INIT_LIST_HEAD(&codec->dapm_widgets);
    INIT_LIST_HEAD(&codec->dapm_paths);

    codec->name = "WM8993";
    codec->read = wm8993_read;
    codec->write = wm8993_write;
    codec->hw_write = (hw_write_t)i2c_master_send;
    codec->reg_cache = wm8993->reg_cache;
    codec->reg_cache_size = ARRAY_SIZE(wm8993->reg_cache);
    codec->bias_level = SND_SOC_BIAS_OFF;
    codec->set_bias_level = wm8993_set_bias_level;
    codec->dai = &wm8993_dai;
    codec->num_dai = 1;
    codec->private_data = wm8993;

    memcpy(wm8993->reg_cache, wm8993_reg_defaults,
           sizeof(wm8993->reg_cache));

    i2c_set_clientdata(i2c, wm8993);
    codec->control_data = i2c;
    wm8993_codec = codec;

    codec->dev = &i2c->dev;

    val = wm8993_read_hw(codec, WM8993_SOFTWARE_RESET);
    if (val != wm8993_reg_defaults[WM8993_SOFTWARE_RESET]) {
        printk("Invalid ID register value %x\n", val);
        ret = -EINVAL;
        goto err;
    }

	//printk("wm8993_write: WM8993_SOFTWARE_RESET at %d", __LINE__);
    ret = wm8993_write(codec, WM8993_SOFTWARE_RESET, 0xffff);
    if (ret != 0)
        goto err;

    /* By default we're using the output mixers */
    wm8993->class_w_users = 2;

    /* Latch volume update bits and default ZC on */
    snd_soc_update_bits(codec, WM8993_LEFT_LINE_INPUT_1_2_VOLUME,
                WM8993_IN1_VU, WM8993_IN1_VU);
    snd_soc_update_bits(codec, WM8993_RIGHT_LINE_INPUT_1_2_VOLUME,
                WM8993_IN1_VU, WM8993_IN1_VU);
    snd_soc_update_bits(codec, WM8993_LEFT_LINE_INPUT_3_4_VOLUME,
                WM8993_IN2_VU, WM8993_IN2_VU);
    snd_soc_update_bits(codec, WM8993_RIGHT_LINE_INPUT_3_4_VOLUME,
                WM8993_IN2_VU, WM8993_IN2_VU);

    snd_soc_update_bits(codec, WM8993_SPEAKER_VOLUME_RIGHT,
                WM8993_SPKOUT_VU, WM8993_SPKOUT_VU);

    snd_soc_update_bits(codec, WM8993_LEFT_OUTPUT_VOLUME,
                WM8993_HPOUT1L_ZC, WM8993_HPOUT1L_ZC);
    snd_soc_update_bits(codec, WM8993_RIGHT_OUTPUT_VOLUME,
                WM8993_HPOUT1_VU | WM8993_HPOUT1R_ZC,
                WM8993_HPOUT1_VU | WM8993_HPOUT1R_ZC);

    snd_soc_update_bits(codec, WM8993_LEFT_OPGA_VOLUME,
                WM8993_MIXOUTL_ZC, WM8993_MIXOUTL_ZC);
    snd_soc_update_bits(codec, WM8993_RIGHT_OPGA_VOLUME,
                WM8993_MIXOUTR_ZC | WM8993_MIXOUT_VU,
                WM8993_MIXOUTR_ZC | WM8993_MIXOUT_VU);

    snd_soc_update_bits(codec, WM8993_RIGHT_DAC_DIGITAL_VOLUME,
                WM8993_DAC_VU, WM8993_DAC_VU);
    snd_soc_update_bits(codec, WM8993_RIGHT_ADC_DIGITAL_VOLUME,
                WM8993_ADC_VU, WM8993_ADC_VU);

    /* Manualy manage the HPOUT sequencing for independent stereo
     * control. */
    snd_soc_update_bits(codec, WM8993_ANALOGUE_HP_0,
                WM8993_HPOUT1_AUTO_PU, 0);

    /* Use automatic clock configuration */
    snd_soc_update_bits(codec, WM8993_CLOCKING_4, WM8993_SR_MODE, 0);

    if (!wm8993->pdata.lineout1_diff)
        snd_soc_update_bits(codec, WM8993_LINE_MIXER1,
                    WM8993_LINEOUT1_MODE,
                    WM8993_LINEOUT1_MODE);
    if (!wm8993->pdata.lineout2_diff)
        snd_soc_update_bits(codec, WM8993_LINE_MIXER2,
                    WM8993_LINEOUT2_MODE,
                    WM8993_LINEOUT2_MODE);

    if (wm8993->pdata.lineout1fb)
        snd_soc_update_bits(codec, WM8993_ADDITIONAL_CONTROL,
                    WM8993_LINEOUT1_FB, WM8993_LINEOUT1_FB);

    if (wm8993->pdata.lineout2fb)
        snd_soc_update_bits(codec, WM8993_ADDITIONAL_CONTROL,
                    WM8993_LINEOUT2_FB, WM8993_LINEOUT2_FB);

    /* Apply the microphone bias/detection configuration - the
     * platform data is directly applicable to the register. */
    snd_soc_update_bits(codec, WM8993_MICBIAS,
                WM8993_JD_SCTHR_MASK | WM8993_JD_THR_MASK |
                WM8993_MICB1_LVL | WM8993_MICB2_LVL,
                wm8993->pdata.jd_scthr << WM8993_JD_SCTHR_SHIFT |
                wm8993->pdata.jd_thr << WM8993_JD_THR_SHIFT |
                wm8993->pdata.micbias1_lvl |
                wm8993->pdata.micbias1_lvl << 1);

    ret = wm8993_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
    if (ret != 0)
        goto err;

    wm8993_dai.dev = codec->dev;

    ret = snd_soc_register_dai(&wm8993_dai);
    if (ret != 0)
        goto err_bias;

    ret = snd_soc_register_codec(codec);

    return 0;

err_bias:
    wm8993_set_bias_level(codec, SND_SOC_BIAS_OFF);
err:
    wm8993_codec = NULL;
    kfree(wm8993);
    return ret;
}

static int wm8993_i2c_remove(struct i2c_client *client)
{
    struct wm8993_priv *wm8993 = i2c_get_clientdata(client);

    snd_soc_unregister_codec(&wm8993->codec);
    snd_soc_unregister_dai(&wm8993_dai);

    wm8993_set_bias_level(&wm8993->codec, SND_SOC_BIAS_OFF);
    kfree(wm8993);

    return 0;
}

static const struct i2c_device_id wm8993_i2c_id[] = {
    { "wm8993", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, wm8993_i2c_id);

static struct i2c_driver wm8993_i2c_driver = {
    .driver = {
        .name = "WM8993",
        .owner = THIS_MODULE,
    },
    .probe = wm8993_i2c_probe,
    .remove = wm8993_i2c_remove,
    .id_table = wm8993_i2c_id,
};


static int __init wm8993_modinit(void)
{
    int ret;

	if (!m8_checkse())
		return 0;
	
    ret = i2c_add_driver(&wm8993_i2c_driver);
    if (ret != 0)
        pr_err("WM8993: Unable to register I2C driver: %d\n", ret);

    return ret;
}
module_init(wm8993_modinit);

static void __exit wm8993_exit(void)
{
    i2c_del_driver(&wm8993_i2c_driver);
}
module_exit(wm8993_exit);


MODULE_DESCRIPTION("ASoC WM8993 driver");
MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");
MODULE_LICENSE("GPL");
