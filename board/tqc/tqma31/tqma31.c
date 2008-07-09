/*
 * Copyright (C) 2008, Jens Gehrlein <sew_s@tqs.de>
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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/mx31.h>
#include <asm/arch/mx31-regs.h>

DECLARE_GLOBAL_DATA_PTR;

static void setup_chipselects (void)
{
	/* NOR Flash on CS0 */
	__REG (CSCR_U(0)) = CSCR_U_CNC_3 | CSCR_U_WSC_12 | CSCR_U_EDC_4;
	__REG (CSCR_A(0)) = CSCR_A_RWA_2 | CSCR_A_RWN_2 | CSCR_A_LBN_2_MUM0;
	__REG (CSCR_L(0)) = CSCR_L_OEA_10 | CSCR_L_EBWA_3 | CSCR_L_EBWN_3 \
		| CSCR_L_EBC_ONLY_WA | CSCR_L_DSZ_16_MUM0 | CSCR_L_CSEN;

	/* Ethernet controller on CS4 */
	__REG (CSCR_U(4)) = CSCR_U_CNC_1 | CSCR_U_WSC_22 | CSCR_U_EDC_4;
	__REG (CSCR_A(4)) = CSCR_A_RWA_2 | CSCR_A_RWN_15;
	__REG (CSCR_L(4)) = CSCR_L_OEA_2 | CSCR_L_OEN_15 | CSCR_L_CSN_15 \
		| CSCR_L_DSZ_16_MUM0 | CSCR_L_CSEN;
}

static void setup_iomux (void)
{
	/* Pins for UART1 */
	mx31_gpio_mux (MUX_RXD1__UART1_RXD_MUX);
	mx31_gpio_mux (MUX_TXD1__UART1_TXD_MUX);
	mx31_gpio_mux (MUX_RTS1__UART1_RTS_B);
	mx31_gpio_mux (MUX_CTS1__UART1_CTS_B);

	/* Pins for I2C1 */
	mx31_gpio_mux (MUX_I2C_CLK__I2C1_SCL);
	mx31_gpio_mux (MUX_I2C_DAT__I2C1_SDA);
	mx31_pad_ctl (PAD_CTL_I2C_CLK, PAD_CTL_IPP_ODE_OD);
	mx31_pad_ctl (PAD_CTL_I2C_DAT, PAD_CTL_IPP_ODE_OD);

	/* Pins for SPI2 */
	mx31_gpio_mux (MUX_CSPI2_MOSI__CSPI2_MOSI);
	mx31_gpio_mux (MUX_CSPI2_MISO__CSPI2_MISO);
	mx31_gpio_mux (MUX_CSPI2_SS0__CSPI2_SS0_B);
	mx31_gpio_mux (MUX_CSPI2_SCLK__CSPI2_CLK);
	mx31_pad_ctl (PAD_CTL_CSPI2_SS0, 0);
	mx31_pad_ctl (PAD_CTL_CSPI2_SS1, 0);
	mx31_pad_ctl (PAD_CTL_CSPI2_SS2, 0);
	mx31_pad_ctl (PAD_CTL_CSPI2_SCLK, 0);
	mx31_pad_ctl (PAD_CTL_CSPI2_SPI_RDY, 0);
	mx31_pad_ctl (PAD_CTL_CSPI2_MISO, 0);
	mx31_pad_ctl (PAD_CTL_CSPI2_MOSI, 0);
}

int dram_init (void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}

int board_init (void)
{
	/* WEIM burst clock runs all the time */
	__REG (WCR) = WCR_BCM_1;

	setup_chipselects ();
	setup_iomux ();

	/* Machine type ID for ARM linux */
	gd->bd->bi_arch_number = MACH_TYPE_TQMA31;

	/* Address of boot parameters */
	gd->bd->bi_boot_params = 0x80000100;

	/*
	 * Tell other U-Boot entities: relocation done. For instance, needed
	 * to enable command line history.
	 */
	gd->flags |= GD_FLG_RELOC;

	return 0;
}

int checkboard (void)
{
	printf ("Board: TQMA31\n");
	return 0;
}
