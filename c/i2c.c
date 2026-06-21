/*
 * i2c.c - i2c port driver
 * 06-23-20 E. Brombaugh
 */

#include <stdio.h>
#include "printf.h"
#include "clkcnt.h"
#include "i2c.h"
#include "up5k_riscv.h"

/* control reg bits */
#define I2C_CCR_EN 0x80

#define I2C_CMD_STA    0x80
#define I2C_CMD_STO    0x40
#define I2C_CMD_RD     0x20
#define I2C_CMD_WR     0x10
#define I2C_CMD_ACK    0x08
#define I2C_CMD_CKSDIS 0x04

#define I2C_SR_TIP   0x80
#define I2C_SR_BUSY  0x40
#define I2C_SR_RARC  0x20
//#define I2C_SR_SRW 0x10 
//#define I2C_SR_ARBL 0x08 
#define I2C_SR_TRRDY 0x04 
#define I2C_SR_TROE 0x02 
//#define I2C_SR_HGC 0x01

static int8_t i2c_wait_done(I2C_TypeDef *s)
{
    uint32_t timeout = 100000;

    /* Wait until transfer starts */
    while(timeout--) {
        if(s->I2CSR & I2C_SR_TIP)
            break;
    }

    if(!timeout)
        return 3;

    timeout = 100000;

    /* Wait until transfer finishes */
    while(timeout--) {
        if(!(s->I2CSR & I2C_SR_TIP))
            return 0;
    }

    return 3;
}

static void i2c_stop(I2C_TypeDef *s)
{
    s->I2CCMDR = I2C_CMD_STO | I2C_CMD_CKSDIS;
}

void i2c_init(I2C_TypeDef *s) { 
    
#if 1 
    s->I2CCR1 = I2C_CCR_EN | 12; // enable I2C 
    s->I2CBRMSB = 0; // high 2 bits - resets I2C core 
    s->I2CBRLSB = 30; // low 8 bits - (12MHz/100kHz)/4 = 30 
#else 

    s->reserved0 = 0xff; // 0
    s->reserved1 = 0xff; // 1 
    s->reserved2 = 0xff; // 2 
    s->I2CSADDR = 0xff; // 3 
    s->reserved4 = 0xff; // 4 
    s->reserved5 = 0xff; // 5 
    s->I2CIRQ = 0xff; // 6 
    s->I2CIRQEN = 0xff; // 7 
    s->I2CCR1 = 0xff; // 8 
    s->I2CCMDR = 0xff; // 9 
    s->I2CBRLSB = 0xff; // A 
    s->I2CBRMSB = 0xff; // B 
    s->I2CSR = 0xff; // C 
    s->I2CTXDR = 0xff; // D 
    s->I2CRXDR = 0xff; // E 
    s->I2CGCDR = 0xff; // F 
    
#endif 
}

int8_t i2c_tx(I2C_TypeDef *s, uint8_t addr, uint8_t *data, uint8_t sz)
{
    uint8_t i;
    uint8_t stat;

    s->I2CTXDR = addr << 1;
    s->I2CCMDR = I2C_CMD_STA | I2C_CMD_WR | I2C_CMD_CKSDIS;

    if(i2c_wait_done(s)) {
        i2c_stop(s);
        return 3;
    }

    stat = s->I2CSR;
    if(stat & I2C_SR_RARC) {
        i2c_stop(s);
        return 1;
    }

    for(i = 0; i < sz; i++) {
        s->I2CTXDR = data[i];
        s->I2CCMDR = I2C_CMD_WR | I2C_CMD_CKSDIS;

        if(i2c_wait_done(s)) {
            i2c_stop(s);
            return 3;
        }

        stat = s->I2CSR;
        if(stat & I2C_SR_RARC) {
            i2c_stop(s);
            return 1;
        }
    }

    i2c_stop(s);
    return 0;
}

int8_t i2c_rx(I2C_TypeDef *s, uint8_t addr, uint8_t reg, uint8_t *data, uint8_t sz)
{
    uint8_t i;
    uint32_t timeout;

    s->I2CTXDR = addr << 1;
    s->I2CCMDR = I2C_CMD_STA | I2C_CMD_WR | I2C_CMD_CKSDIS;

    if(i2c_wait_done(s)) {
        i2c_stop(s);
        return 3;
    }

    if(s->I2CSR & I2C_SR_RARC) {
        i2c_stop(s);
        return 1;
    }

    s->I2CTXDR = reg;
    s->I2CCMDR = I2C_CMD_WR | I2C_CMD_CKSDIS;

    if(i2c_wait_done(s)) {
        i2c_stop(s);
        return 3;
    }

    if(s->I2CSR & I2C_SR_RARC) {
        i2c_stop(s);
        return 1;
    }

    s->I2CTXDR = (addr << 1) | 1;
    s->I2CCMDR = I2C_CMD_STA | I2C_CMD_WR | I2C_CMD_CKSDIS;

    if(i2c_wait_done(s)) {
        i2c_stop(s);
        return 3;
    }

    if(s->I2CSR & I2C_SR_RARC) {
        i2c_stop(s);
        return 1;
    }

    for(i = 0; i < sz; i++) {
        uint8_t cmd = I2C_CMD_RD | I2C_CMD_CKSDIS;

        if(i == sz - 1)
            cmd |= I2C_CMD_ACK | I2C_CMD_STO;

        s->I2CCMDR = cmd;

        timeout = 100000;
        while(timeout--) {
            if(!(s->I2CSR & I2C_SR_TRRDY))
                break;
        }
        if(!timeout)
            return 3;

        if(i2c_wait_done(s))
            return 3;

        timeout = 100000;
        while(timeout--) {
            if(s->I2CSR & I2C_SR_TRRDY)
                break;
        }
        if(!timeout)
            return 3;

        data[i] = s->I2CRXDR;
    }

    return 0;
}