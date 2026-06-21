/*
 * INA226 voltage/current monitor
 */

#include "ina226.h"

static int read_reg16(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint16_t *out)
{
    uint8_t buf[2];
    int err = i2c_rx(i2c, addr, reg, buf, 2);
    if(!err)
        *out = ((uint16_t)buf[0] << 8) | buf[1];
    return err;
}

/* Bus voltage register: 1.25 mV/LSB = 5/4 mV/LSB */
int32_t ina226_read_bus_mv(I2C_TypeDef *i2c, uint8_t addr)
{
    uint16_t raw;
    if(read_reg16(i2c, addr, INA226_REG_BUS_V, &raw))
        return -1;
    return (int32_t)((uint32_t)raw * 5 / 4);
}

/* Shunt voltage register: 2.5 uV/LSB = 5/2 uV/LSB, signed */
int32_t ina226_read_shunt_uv(I2C_TypeDef *i2c, uint8_t addr)
{
    uint16_t raw;
    if(read_reg16(i2c, addr, INA226_REG_SHUNT_V, &raw))
        return -1;
    return (int32_t)(int16_t)raw * 5 / 2;
}

static int write_reg16(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint16_t val)
{
    uint8_t buf[3] = {reg, (uint8_t)(val >> 8) & 0xff, (uint8_t)(val & 0xff)}; //MSB, LSB
    return i2c_tx(i2c, addr, buf, 3);
}

void ina226_init(I2C_TypeDef *i2c, uint8_t addr)
{
    i2c_init(I2C1);
    write_reg16(i2c, addr, INA226_REG_CALIB, INA226_CAL);
}

/* Current_LSB * raw (signed) */
int32_t ina226_read_current_ua(I2C_TypeDef *i2c, uint8_t addr)
{
    uint16_t raw;
    if(read_reg16(i2c, addr, INA226_REG_CURRENT, &raw))
        return -1;
    return (int32_t)(int16_t)raw * INA226_CURRENT_LSB_UA;
}

/* Power_LSB = 25 * Current_LSB */
int32_t ina226_read_power_uw(I2C_TypeDef *i2c, uint8_t addr)
{
    uint16_t raw;
    if(read_reg16(i2c, addr, INA226_REG_POWER, &raw))
        return -1;
    return (int32_t)raw * 25 * INA226_CURRENT_LSB_UA;
}