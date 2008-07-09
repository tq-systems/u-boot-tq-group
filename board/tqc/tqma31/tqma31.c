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
#include <spi.h>
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

	/* FPGA configuration flash on CS1 */
	__REG (CSCR_U(1)) = CSCR_U_CNC_3 | CSCR_U_WSC_13 | CSCR_U_EDC_4;
	__REG (CSCR_A(1)) = CSCR_A_RWA_2 | CSCR_A_RWN_2 | CSCR_A_LBN_2_MUM0;
	__REG (CSCR_L(1)) = CSCR_L_OEA_10 | CSCR_L_EBWA_3 | CSCR_L_EBWN_3 \
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

	/* Pins for IPU */
	mx31_pad_ctl (PAD_CTL_FPSHIFT, PAD_CTL_IPP_PUE_PULL \
					| PAD_CTL_IPP_PUS_100K_DN \
					| PAD_CTL_IPP_HYS_STD \
					| PAD_CTL_IPP_ODE_STD \
					| PAD_CTL_IPP_SRE_FAST);
}

static int adjust_voltages (void)
{
	u32 reg;
	u32 val;
	static struct spi_slave *slave = NULL;

	slave = spi_setup_slave(1, 0, 1000000,
		SPI_MODE_2 | SPI_CS_HIGH);
	if (!slave)
		return -1;

	if (spi_claim_bus(slave))
		return -1;

	/* Set PMIC arbitration switchers */
	val = 0x000020;
	reg = 0x14000000 | val | 0x80000000;
	if (spi_xfer(slave, 32, (uchar *)&reg, (uchar *)&val,
		SPI_XFER_BEGIN | SPI_XFER_END))
		return -1;

	/* Set PMIC regulator enable to 0x0 */
	val = 0x000000;
	reg = 0x20000000 | val | 0x80000000;
	if (spi_xfer(slave, 32, (uchar *)&reg, (uchar *)&val,
		SPI_XFER_BEGIN | SPI_XFER_END))
		return -1;

	/*
	 * Set PMIC regulator setting 0
	 *	VRFDIG = 1,8V / reset value = 1,875V
	 *	VGEN   = 1,8V / reset value = 1,5V
	 *	VDIG   = 1,3V / reset value = 1,5V
	 */
	val = 0x63cdc;
	reg = 0x3c000000 | val | 0x80000000;
	if (spi_xfer(slave, 32, (uchar *)&reg, (uchar *)&val,
		SPI_XFER_BEGIN | SPI_XFER_END))
		return -1;

	/*
	 * Set PMIC regulator mode 0 to 0x24924
	 *	VAUDIO	= on
	 *	VIOH	= off
	 *	VIOLO	= on
	 *	VDIG	= on
	 *	VGEN	= on
	 *	VRFDIG	= on
	 *	VRFREF	= on
	 *	VRFCP	= on
	 */
	val = 0x249241;
	reg = 0x40000000 | val | 0x80000000;
	if (spi_xfer(slave, 32, (uchar *)&reg, (uchar *)&val,
		SPI_XFER_BEGIN | SPI_XFER_END))
		return -1;

	/*
	 * Set PMIC regulator mode 1 to 0x0
	 *	VSIM	= off
	 *	VESIM	= off
	 *	VCAM	= off
	 *	VRFBG	= off
	 *	VVIB	= off
	 *	VRF1	= off
	 *	VRF2	= off
	 *	VMMC1	= off
	 *	VMMC2	= off
	 */
	val = 0x0;
	reg = 0x42000000 | val | 0x80000000;
	if (spi_xfer(slave, 32, (uchar *)&reg, (uchar *)&val,
		SPI_XFER_BEGIN | SPI_XFER_END))
		return -1;

	/*
	 * FIXME: switcher settings 0 and 1.
	 * Kernel hangs when unpacking itself.
	 * Workaround:
	 * Up to now CPU can operate at maximum allowed voltage 1.6 V.
	 */
	/*
	 * Set PMIC switcher setting 0
	 *	SW1A     = 1,2V / reset value = 1,6V
	 *	SW1ADVS  = 1,6V / reset value = 1,6V
	 *	SW1ASTBY = 1,2V / reset value = 1,6V
	 */
	/*
	val = 0xc70c;
	reg = 0x30000000 | val | 0x80000000;
	if (spi_xfer(slave, 32, (uchar *)&reg, (uchar *)&val,
		SPI_XFER_BEGIN | SPI_XFER_END))
		return -1;
	*/

	/*
	 * Set PMIC switcher setting 1
	 *	SW1A     = 1,2V / reset value = 1,6V
	 *	SW1ADVS  = 1,6V / reset value = 1,6V
	 *	SW1ASTBY = 1,2V / reset value = 1,6V
	 */
	/*
	val = 0xc70c;
	reg = 0x32000000 | val | 0x80000000;
	if (spi_xfer(slave, 32, (uchar *)&reg, (uchar *)&val,
		SPI_XFER_BEGIN | SPI_XFER_END))
		return -1;
	*/

	/*
	 * Set PMIC switcher setting 5
	 *	SW5 = 5V
	 *	SW5 = off
	 */
	val = 0x021605;
	reg = 0x3A000000 | val | 0x80000000;
	if (spi_xfer(slave, 32, (uchar *)&reg, (uchar *)&val,
		SPI_XFER_BEGIN | SPI_XFER_END))
		return -1;

	spi_release_bus(slave);
	spi_free_slave(slave);

	return 0;
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

int board_late_init(void)
{
	/*
	 * Must call this function in late init stage, because the SPI driver,
	 * required by this function, uses malloc(). The malloc space has not
	 * been setup in the board_init() stage.
	 */
	if (adjust_voltages ()) {
		printf ("Adjusting voltages failed!\n");
		return -1;
	}
	return 0;
}

