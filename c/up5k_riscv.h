/*
 * up5k_riscv.h - hardware definitions for up5k_riscv
 * 07-01-19 E. Brombaugh
 */

#ifndef __up5k_riscv__
#define __up5k_riscv__

#include <stdint.h>

// 32-bit parallel out
#define gp_out (*(volatile uint32_t *)0x20000000)
#define button_addr (*(volatile uint32_t *)0x60000000) 

// 32-bit clock counter
#define clkcnt_reg (*(volatile uint32_t *)0x50000000)

// ACIA serial
#define acia_ctlstat (*(volatile uint8_t *)0x30000000)
#define acia_data (*(volatile uint8_t *)0x30000004)

// SPI cores @ BUS_ADDR74 = 0b0000 and 0b0010
#define SPI0_BASE 0x40000000
#define SPI1_BASE 0x40000080

typedef struct
{
	uint32_t reserved0[8];
	volatile uint8_t SPICR0; // starts at 0x20; timing and CS delays
	uint8_t reserved1[3];
	volatile uint8_t SPICR1; // enable/ disable SPI operation
	uint8_t reserved2[3];
	volatile uint8_t SPICR2; // M/S selection, CPOL, CPHA, CS behaviour
	uint8_t reserved3[3];
	volatile uint8_t SPIBR; // SPI clock speed
	uint8_t reserved4[3];
	volatile uint8_t SPISR; // status register: busy, complete etc
	uint8_t reserved5[3];
	volatile uint8_t SPITXDR; // byte to send
	uint8_t reserved6[3];
	volatile uint8_t SPIRXDR; // received byte
	uint8_t reserved7[3];
	volatile uint8_t SPICSR; // control CS outputs
	uint8_t reserved8[3];
} SPI_TypeDef;

#define SPI0 ((SPI_TypeDef *)SPI0_BASE)
#define SPI1 ((SPI_TypeDef *)SPI1_BASE)

// I2C cores @ BUS_ADDR74 = 0b0001 and 0b0011
#define I2C0_BASE 0x40000040
#define I2C1_BASE 0x400000C0

typedef struct
{
	uint32_t reserved0;		   // 0 -> register numbers
	uint32_t reserved1;		   // 1
	uint32_t reserved2;		   // 2
	volatile uint8_t I2CSADDR; // 3 -> slave address
	uint8_t reserved3[3];
	uint32_t reserved4;		 // 4
	uint32_t reserved5;		 // 5
	volatile uint8_t I2CIRQ; // 6 -> interrupt status flags
	uint8_t reserved6[3];
	volatile uint8_t I2CIRQEN; // 7 -> enables interrupt source
	uint8_t reserved7[3];
	volatile uint8_t I2CCR1; // 8 -> enables controller and operating modes
	uint8_t reserved8[3];
	volatile uint8_t I2CCMDR; // 9 -> generates START, STOP, R/W operations
	uint8_t reserved9[3];
	volatile uint8_t I2CBRLSB; // A -> lower 8 bits of clk divider
	uint8_t reservedA[3];
	volatile uint8_t I2CBRMSB; // B -> upper 8 bits of clk divider
	uint8_t reservedB[3];
	volatile uint8_t I2CSR; // C -> status reg: ACK, busy etc
	uint8_t reservedC[3];
	volatile uint8_t I2CTXDR; // D -> byte to transmit
	uint8_t reservedD[3];
	volatile uint8_t I2CRXDR; // E -> byte received
	uint8_t reservedE[3];
	volatile uint8_t I2CGCDR; // F -> general call data register for broadcasting commands
	uint8_t reservedF[3];
} I2C_TypeDef;

#define I2C0 ((I2C_TypeDef *)I2C0_BASE)
#define I2C1 ((I2C_TypeDef *)I2C1_BASE)

#endif