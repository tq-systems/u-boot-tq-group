/*
 *
 * (c) 2007 Pengutronix, Sascha Hauer <s.hauer@pengutronix.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __ASM_ARCH_MX31_REGS_H
#define __ASM_ARCH_MX31_REGS_H

#define __REG(x)     (*((volatile u32 *)(x)))
#define __REG16(x)   (*((volatile u16 *)(x)))
#define __REG8(x)    (*((volatile u8 *)(x)))

#define CCM_BASE	0x53f80000
#define CCM_CCMR	(CCM_BASE + 0x00)
#define CCM_PDR0	(CCM_BASE + 0x04)
#define CCM_PDR1	(CCM_BASE + 0x08)
#define CCM_RCSR	(CCM_BASE + 0x0c)
#define CCM_MPCTL	(CCM_BASE + 0x10)
#define CCM_UPCTL	(CCM_BASE + 0x10)
#define CCM_SPCTL	(CCM_BASE + 0x18)
#define CCM_COSR	(CCM_BASE + 0x1C)
#define CCM_CGR0	(CCM_BASE + 0x20)
#define CCM_CGR1	(CCM_BASE + 0x24)
#define CCM_CGR2	(CCM_BASE + 0x28)

#define CCMR_MDS	(1 << 7)
#define CCMR_SBYCS	(1 << 4)
#define CCMR_MPE	(1 << 3)
#define CCMR_PRCS_MASK	(3 << 1)
#define CCMR_FPM	(1 << 1)
#define CCMR_CKIH	(2 << 1)

#define PDR0_CSI_PODF(x)	(((x) & 0x1ff) << 23)
#define PDR0_PER_PODF(x)	(((x) & 0x1f) << 16)
#define PDR0_HSP_PODF(x)	(((x) & 0x7) << 11)
#define PDR0_NFC_PODF(x)	(((x) & 0x7) << 8)
#define PDR0_IPG_PODF(x)	(((x) & 0x3) << 6)
#define PDR0_MAX_PODF(x)	(((x) & 0x7) << 3)
#define PDR0_MCU_PODF(x)	((x) & 0x7)

#define PLL_PD(x)		(((x) & 0xf) << 26)
#define PLL_MFD(x)		(((x) & 0x3ff) << 16)
#define PLL_MFI(x)		(((x) & 0xf) << 10)
#define PLL_MFN(x)		(((x) & 0x3ff) << 0)

#define ESDCTL_BASE	0xB8001000
#define ESDCTL0		(ESDCTL_BASE + 0x00)
#define ESDCFG0		(ESDCTL_BASE + 0x04)
#define ESDCTL1		(ESDCTL_BASE + 0x08)
#define ESDCFG1		(ESDCTL_BASE + 0x0C)
#define ESDMISC		(ESDCTL_BASE + 0x10)
#define ESDCDLY1	(ESDCTL_BASE + 0x20)
#define ESDCDLY2	(ESDCTL_BASE + 0x24)
#define ESDCDLY3	(ESDCTL_BASE + 0x28)
#define ESDCDLY4	(ESDCTL_BASE + 0x2C)
#define ESDCDLY5	(ESDCTL_BASE + 0x30)
#define ESDCDLYL	(ESDCTL_BASE + 0x34)

#define WEIM_BASE	0xb8002000
#define CSCR_U(x)	(WEIM_BASE + (x) * 0x10)
#define CSCR_L(x)	(WEIM_BASE + 4 + (x) * 0x10)
#define CSCR_A(x)	(WEIM_BASE + 8 + (x) * 0x10)

#define CSCR_U_CNC_1		0x00004000
#define CSCR_U_CNC_3		0x0000C000
#define CSCR_U_WSC_9		0x00000900
#define CSCR_U_WSC_12		0x00000C00
#define CSCR_U_WSC_22		0x00001600
#define CSCR_U_EDC_4		0x00000004
#define CSCR_U_EDC_13		0x0000000D

#define CSCR_L_OEA_2		0x20000000
#define CSCR_L_OEA_4		0x40000000
#define CSCR_L_OEA_10		0xA0000000
#define CSCR_L_OEN_15		0x0F000000
#define CSCR_L_EBWA_3		0x00300000
#define CSCR_L_EBWN_3		0x00030000
#define CSCR_L_CSA_4		0x00004000
#define CSCR_L_CSN_15		0x000000F0
#define CSCR_L_EBC_ONLY_WA	0x00000800
#define CSCR_L_DSZ_16_MUM0	0x00000500
#define CSCR_L_CSEN		0x00000001

#define CSCR_A_RWA_2		0x00200000
#define CSCR_A_RWN_2		0x00020000
#define CSCR_A_RWN_15		0x000F0000
#define CSCR_A_LBN_2_MUM0	0x00000800

#define WCR			(WEIM_BASE + 0x60)
#define WCR_BCM_1		0x00000004
#define WCR_BCM_0		0x00000000

#define IOMUXC_BASE	0x43FAC000
#define IOMUXC_GPR	(IOMUXC_BASE + 0x8)
#define IOMUXC_SW_MUX_CTL(x)	(IOMUXC_BASE + 0xc + (x) * 4)
#define IOMUXC_SW_PAD_CTL(x)	(IOMUXC_BASE + 0x154 + (x) * 4)

#define IPU_BASE		0x53fc0000
#define IPU_CONF		IPU_BASE

#define IPU_CONF_PXL_ENDIAN	(1<<8)
#define IPU_CONF_DU_EN		(1<<7)
#define IPU_CONF_DI_EN		(1<<6)
#define IPU_CONF_ADC_EN		(1<<5)
#define IPU_CONF_SDC_EN		(1<<4)
#define IPU_CONF_PF_EN		(1<<3)
#define IPU_CONF_ROT_EN		(1<<2)
#define IPU_CONF_IC_EN		(1<<1)
#define IPU_CONF_SCI_EN		(1<<0)

#define WDOG_BASE		0x53FDC000

/*
 * Signal Multiplexing (IOMUX)
 */

/* bits in the SW_MUX_CTL registers */
#define MUX_CTL_OUT_GPIO_DR	(0 << 4)
#define MUX_CTL_OUT_FUNC	(1 << 4)
#define MUX_CTL_OUT_ALT1	(2 << 4)
#define MUX_CTL_OUT_ALT2	(3 << 4)
#define MUX_CTL_OUT_ALT3	(4 << 4)
#define MUX_CTL_OUT_ALT4	(5 << 4)
#define MUX_CTL_OUT_ALT5	(6 << 4)
#define MUX_CTL_OUT_ALT6	(7 << 4)
#define MUX_CTL_IN_NONE		(0 << 0)
#define MUX_CTL_IN_GPIO		(1 << 0)
#define MUX_CTL_IN_FUNC		(2 << 0)
#define MUX_CTL_IN_ALT1		(4 << 0)
#define MUX_CTL_IN_ALT2		(8 << 0)

#define MUX_CTL_FUNC		(MUX_CTL_OUT_FUNC | MUX_CTL_IN_FUNC)
#define MUX_CTL_ALT1		(MUX_CTL_OUT_ALT1 | MUX_CTL_IN_ALT1)
#define MUX_CTL_ALT2		(MUX_CTL_OUT_ALT2 | MUX_CTL_IN_ALT2)
#define MUX_CTL_GPIO		(MUX_CTL_OUT_GPIO_DR | MUX_CTL_IN_GPIO)

/* Register offsets based on IOMUXC_BASE */
/* 0x00 .. 0x7b */
#define MUX_CTL_RTS1		0x7c
#define MUX_CTL_CTS1		0x7d
#define MUX_CTL_DTR_DCE1	0x7e
#define MUX_CTL_DSR_DCE1	0x7f
#define MUX_CTL_CSPI2_SCLK	0x80
#define MUX_CTL_CSPI2_SPI_RDY	0x81
#define MUX_CTL_RXD1		0x82
#define MUX_CTL_TXD1		0x83
#define MUX_CTL_CSPI2_MISO	0x84
#define MUX_CTL_CSPI2_SS0	0x85
#define MUX_CTL_CSPI2_SS1	0x86
#define MUX_CTL_CSPI2_SS2	0x87
#define MUX_CTL_CSPI2_MOSI	0x8b

#define MUX_CTL_I2C_CLK		0xa2
#define MUX_CTL_I2C_DAT		0xa3

/* The modes a specific pin can be in
 * these macros can be used in mx31_gpio_mux() and have the form
 * MUX_[contact name]__[pin function]
 */
#define MUX_RXD1__UART1_RXD_MUX	((MUX_CTL_FUNC << 8) | MUX_CTL_RXD1)
#define MUX_TXD1__UART1_TXD_MUX	((MUX_CTL_FUNC << 8) | MUX_CTL_TXD1)
#define MUX_RTS1__UART1_RTS_B	((MUX_CTL_FUNC << 8) | MUX_CTL_RTS1)
#define MUX_CTS1__UART1_CTS_B	((MUX_CTL_FUNC << 8) | MUX_CTL_CTS1)

#define MUX_CSPI2_MOSI__I2C2_SCL ((MUX_CTL_ALT1 << 8) | MUX_CTL_CSPI2_MOSI)
#define MUX_CSPI2_MISO__I2C2_SDA ((MUX_CTL_ALT1 << 8) | MUX_CTL_CSPI2_MISO)

#define MUX_CSPI2_MOSI__CSPI2_MOSI ((MUX_CTL_FUNC << 8) | MUX_CTL_CSPI2_MOSI)
#define MUX_CSPI2_MISO__CSPI2_MISO ((MUX_CTL_FUNC << 8) | MUX_CTL_CSPI2_MISO)
#define MUX_CSPI2_SS0__CSPI2_SS0_B ((MUX_CTL_FUNC << 8) | MUX_CTL_CSPI2_SS0)
#define MUX_CSPI2_SCLK__CSPI2_CLK  ((MUX_CTL_FUNC << 8) | MUX_CTL_CSPI2_SCLK)

#define MUX_I2C_CLK__I2C1_SCL	((MUX_CTL_FUNC << 8) | MUX_CTL_I2C_CLK)
#define MUX_I2C_DAT__I2C1_SDA	((MUX_CTL_FUNC << 8) | MUX_CTL_I2C_DAT)

/* bits in the SW_PAD_CTL registers */
#define PAD_CTL_LOOPBACK_DIS		(0 << 9)
#define PAD_CTL_LOOPBACK_ENA		(1 << 9)
#define PAD_CTL_IPP_PUE_DIS		(0 << 7)
#define PAD_CTL_IPP_PUE_KEEPER		(2 << 7)
#define PAD_CTL_IPP_PUE_PULL		(3 << 7)
#define PAD_CTL_IPP_PUS_100K_DN		(0 << 5)
#define PAD_CTL_IPP_PUS_100K_UP		(1 << 5)
#define PAD_CTL_IPP_HYS_STD		(0 << 4)
#define PAD_CTL_IPP_HYS_SCHMITT		(1 << 4)
#define PAD_CTL_IPP_ODE_STD		(0 << 3)
#define PAD_CTL_IPP_ODE_OD		(1 << 3)
#define PAD_CTL_IPP_DSE_STD		(0 << 1)
#define PAD_CTL_IPP_DSE_HIGH		(1 << 1)
#define PAD_CTL_IPP_DSE_MAX		(2 << 1)
#define PAD_CTL_IPP_SRE_SLOW		(0 << 0)
#define PAD_CTL_IPP_SRE_FAST		(1 << 0)

/* bit fields in the SW_PAD_CTL registers, offsets based on IOMUXC_BASE */
#define	PAD_CTL_IO1_SHIFT		0
#define	PAD_CTL_IO2_SHIFT		10
#define	PAD_CTL_IO3_SHIFT		20
#define PAD_CTL_FPSHIFT			((0x184 << 8) | PAD_CTL_IO1_SHIFT)
#define PAD_CTL_CSPI2_SS2		((0x1F4 << 8) | PAD_CTL_IO3_SHIFT)
#define PAD_CTL_CSPI2_SCLK		((0x1F4 << 8) | PAD_CTL_IO2_SHIFT)
#define PAD_CTL_CSPI2_SPI_RDY		((0x1F4 << 8) | PAD_CTL_IO1_SHIFT)
#define PAD_CTL_CSPI2_MISO		((0x1F8 << 8) | PAD_CTL_IO3_SHIFT)
#define PAD_CTL_CSPI2_SS0		((0x1F8 << 8) | PAD_CTL_IO2_SHIFT)
#define PAD_CTL_CSPI2_SS1		((0x1F8 << 8) | PAD_CTL_IO1_SHIFT)
#define PAD_CTL_CSPI2_MOSI		((0x1FC << 8) | PAD_CTL_IO1_SHIFT)
#define PAD_CTL_I2C_CLK			((0x21C << 8) | PAD_CTL_IO2_SHIFT)
#define PAD_CTL_I2C_DAT			((0x21C << 8) | PAD_CTL_IO1_SHIFT)


/*
 * Memory regions and CS
 */
#define IPU_MEM_BASE	0x70000000
#define CSD0_BASE	0x80000000
#define CSD1_BASE	0x90000000
#define CS0_BASE	0xA0000000
#define CS1_BASE	0xA8000000
#define CS2_BASE	0xB0000000
#define CS3_BASE	0xB2000000
#define CS4_BASE	0xB4000000
#define CS4_PSRAM_BASE	0xB5000000
#define CS5_BASE	0xB6000000
#define PCMCIA_MEM_BASE	0xC0000000

#endif /* __ASM_ARCH_MX31_REGS_H */
