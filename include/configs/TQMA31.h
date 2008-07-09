/*
 * Copyright (C) 2008, Jens Gehrlein <sew_s@tqs.de>
 *
 * Configuration settings for the TQMA31 board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*******************************************************************************
 * CPU
 ******************************************************************************/

#include <asm/arch/mx31-regs.h>

/* This is an ARM1136 core */
#define CONFIG_ARM1136			1

/* in an i.MX31 processor */
#define CONFIG_MX31			1

#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* Enable passing of ATAGs */
#define CONFIG_CMDLINE_TAG		1
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_INITRD_TAG		1


/*******************************************************************************
 * Clocks
 ******************************************************************************/

 /* Frequency on CKIH in Hz. This is the MCU PLL input clock on TQMA31. */
#define CONFIG_MX31_HCLK_FREQ		26000000

/* Desired CPU core frequency (see PLL settings in lowlevel_init.S) */
#define CONFIG_MX31_CPU_399_MHZ
#undef CONFIG_MX31_CPU_532_MHZ

/* Timer tick */
#define CFG_HZ				32768

/*
 * Alternative clock input for MCU PLL.
 * Unused on TQMA31, but must be defined to avoid compiler errors.
 */
#define CONFIG_MX31_CLK32		32000


/*******************************************************************************
 * RAM
 ******************************************************************************/

#define CONFIG_NR_DRAM_BANKS		1

/*
 * PHYS_SDRAM_1 and PHYS_SDRAM_1_SIZE are passed to the Linux kernel.
 * The size can be overridden by the kernel boot parameter "mem=".
 */
#define PHYS_SDRAM_1			CSD0_BASE
#define PHYS_SDRAM_1_SIZE		(256 * 1024 * 1024)

/* Default values for command mtest */
#define CFG_MEMTEST_START		PHYS_SDRAM_1
#define CFG_MEMTEST_END			(PHYS_SDRAM_1_SIZE - 0x100000)


/*******************************************************************************
 * NOR-Flash
 ******************************************************************************/

#define CFG_FLASH_BASE			CS0_BASE

/* Max number of memory banks */
#define CFG_MAX_FLASH_BANKS		5

/*
 * Bank start addresses.
 * 4 x 32 MiB is the maximum for user and U-Boot code. Please adapt the list
 * as well as CFG_MAX_FLASH_BANKS to your TQM. Also consider dual die chips
 * or single die chips, e.g. one dual die chip represents two banks.
 * The fifth bank is a separate chip for the FPGA configuration. It's
 * controlled by chipselect 1.
 */
#define CFG_FLASH_BANKS_LIST		{CFG_FLASH_BASE,			\
					CFG_FLASH_BASE + 32 * 1024 * 1024,	\
					CFG_FLASH_BASE + 64 * 1024 * 1024,	\
					CFG_FLASH_BASE + 96 * 1024 * 1024,	\
					CS1_BASE}

/* Max number of sectors on one chip */
#define CFG_MAX_FLASH_SECT		259

/* Monitor at beginning of flash */
#define CFG_MONITOR_BASE		CFG_FLASH_BASE

/* Reserved flash space for U-Boot image in bytes */
#define CFG_MONITOR_LEN			(128 * 1024)

/* Flash memory is CFI compliant */
#define CFG_FLASH_CFI			1

/* Use drivers/cfi_flash.c */
#define CFG_FLASH_CFI_DRIVER		1

/* Use buffered writes (~10x faster) */
#define CFG_FLASH_USE_BUFFER_WRITE	1

/* Use hardware sector protection */
#define CFG_FLASH_PROTECTION		1


/*******************************************************************************
 * Serial
 ******************************************************************************/

/* Configure processor internal UART */
#define CONFIG_MX31_UART		1

/* Configure UART1 */
#define CFG_MX31_UART1			1

/* Use UART1 for console */
#define CONFIG_CONS_INDEX		1

/* Default baudrate */
#define CONFIG_BAUDRATE			115200

/* Possible baudrates */
#define CFG_BAUDRATE_TABLE		{9600, 19200, 38400, 57600, 115200}


/*******************************************************************************
 * Ethernet
 ******************************************************************************/

#define CONFIG_DRIVER_SMC911X		1
#define CONFIG_DRIVER_SMC911X_32_BIT	1
#define CONFIG_DRIVER_SMC911X_BASE	CS4_BASE


/*******************************************************************************
 * I2C
 ******************************************************************************/

/* Use the processor internal controller */
#define CONFIG_HARD_I2C
#define CONFIG_I2C_MXC

/* Configure I2C1 */
#define CFG_I2C_MX31_PORT1

#define CFG_I2C_SPEED 			100000
#define CFG_I2C_SLAVE 			0

/* Don't probe these addrs */
#define CFG_I2C_NOPROBES		{0x00}

/* I2C EEPROM, configuration for onboard EEPROM */
#define CFG_I2C_EEPROM_ADDR		0x50
#define CFG_I2C_EEPROM_ADDR_LEN		2

/* 32 bytes per write */
#define CFG_EEPROM_PAGE_WRITE_BITS	5

#define CFG_EEPROM_PAGE_WRITE_ENABLE

/* 10ms +/- 20% */
#define CFG_EEPROM_PAGE_WRITE_DELAY_MS	12

/* I2C SYSMON (LM75) */
#define CONFIG_DTT_LM75			1	/* ON Semi's LM75 */
#define CONFIG_DTT_SENSORS		{0}	/* Sensor addresses */
#define CFG_DTT_MAX_TEMP		70
#define CFG_DTT_LOW_TEMP		-30
#define CFG_DTT_HYSTERESIS		3


/*******************************************************************************
 * SPI
 ******************************************************************************/

/* Use the processor internal controller */
#define CONFIG_HARD_SPI			1
#define CONFIG_MXC_SPI			1

/* 0=CSPI1, 1=CSPI2, 2=CSPI3 */
#define CONFIG_DEFAULT_SPI_BUS		1

/* SCLK polarity: high; chipselect: active high */
#define CONFIG_DEFAULT_SPI_MODE		(SPI_MODE_2 | SPI_CS_HIGH)

/* Add PMIC RTC driver */
#define CONFIG_RTC_MC13783		1


/*******************************************************************************
 * Commands
 ******************************************************************************/

#include <config_cmd_default.h>

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_I2C
#define CONFIG_CMD_DTT
#define CONFIG_CMD_EEPROM
#define CONFIG_CMD_SPI
#define CONFIG_CMD_DATE


/*******************************************************************************
 * Environment
 ******************************************************************************/

#define CONFIG_BOOTDELAY		4

/*
 * Default value for env. variable "loadaddr".
 * Don't use arithmetic here, because it will be converted
 * into a string (see common/environment.c)
 */
#define CONFIG_LOADADDR			0x80800000

/* Default load address */
#define CFG_LOAD_ADDR			CONFIG_LOADADDR

/* Allow to modify environment variables "serial#" and "ethaddr" */
#define CONFIG_ENV_OVERWRITE

#define	CFG_ENV_IS_IN_FLASH		1
#define CFG_ENV_OFFSET			0x20000
#define CFG_ENV_ADDR			(CFG_FLASH_BASE + CFG_ENV_OFFSET)
#define CFG_ENV_SECT_SIZE		0x20000
#define CFG_ENV_SIZE			CFG_ENV_SECT_SIZE

/* Address and size of redundant environment sector */
#define CFG_ENV_OFFSET_REDUND		(CFG_ENV_OFFSET + CFG_ENV_SECT_SIZE)
#define CFG_ENV_SIZE_REDUND		CFG_ENV_SECT_SIZE

/*
 * Kernel parameter "jtag=on" is Freescale-specific extension of the
 * mx31ads BSP. Use it to enable serial console output in Linux.
 */
#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"netdev=eth0\0"							\
	"hostname=tqma31\0"						\
	"uboot_addr=0xa0000000\0"					\
	"uboot=tqma31/u-boot.bin\0"					\
	"kernel=tqma31/uImage\0"					\
	"nfsroot=/opt/eldk/arm\0"					\
	"bootargs_base=setenv bootargs "				\
		"jtag=on "						\
		"console=ttymxc0,115200\0"				\
	"bootargs_nfs=setenv bootargs ${bootargs} "			\
		"root=/dev/nfs rw "					\
		"ethaddr=${ethaddr} "					\
		"ip=${ipaddr}:${serverip}:${gatewayip}:"		\
			"${netmask}:${hostname}:${netdev}:off "		\
		"panic=1 "						\
		"nfsroot=${serverip}:${nfsroot}\0"			\
	"bootcmd=run bootcmd_net\0"					\
	"bootcmd_net=run bootargs_base bootargs_nfs; "			\
		"tftpboot ${loadaddr} ${kernel}; bootm\0"		\
	"prg_uboot=tftpboot ${loadaddr} ${uboot}; "			\
		"protect off ${uboot_addr} 0xa001ffff; "		\
		"erase ${uboot_addr} 0xa001ffff; "			\
		"cp.b ${loadaddr} ${uboot_addr} ${filesize}; "		\
		"setenv filesize; saveenv\0"


/*******************************************************************************
 * JFFS2 partitions
 ******************************************************************************/

#undef CONFIG_JFFS2_CMDLINE
#define CONFIG_JFFS2_DEV		"nor0"


/*******************************************************************************
 * Miscellaneous
 ******************************************************************************/

#define BOARD_LATE_INIT

/* Total available space for malloc in bytes */
#define CFG_MALLOC_LEN			(CFG_ENV_SIZE + 128 * 1024)

/* Regular stack size in bytes */
#define CONFIG_STACKSIZE		(128 * 1024)

/* Size in bytes reserved for initial global data */
#define CFG_GBL_DATA_SIZE		128

/* Console I/O buffer size in bytes */
#define CFG_CBSIZE			256

/* Print buffer size in bytes */
#define CFG_PBSIZE			(CFG_CBSIZE + sizeof(CFG_PROMPT) + 16)

/* Max number of command arguments */
#define CFG_MAXARGS			16

/* Boot argument buffer size in bytes */
#define CFG_BARGSIZE			CFG_CBSIZE

#define CFG_LONGHELP
#define CFG_PROMPT			"=> "

/* Add command line history and command line edit possibility */
#define CONFIG_CMDLINE_EDITING

#define CONFIG_ARP_TIMEOUT		200UL

#endif /* __CONFIG_H */
