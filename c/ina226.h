/*
 * INA226 voltage/current monitor
 */

/*
 * 20 mOhm shunt, Current_LSB = 10 uA
 * CAL = 0.00512 / (10e-6 * 0.02) = 25600
 * Power_LSB = 25 * 10 uA = 250 uW/LSB
 */

#ifndef __ina226__
#define __ina226__

#include <stdint.h>
#include "i2c.h"

#define INA226_REG_CONFIG       0x00
#define INA226_REG_SHUNT_V      0x01  /* signed 16-bit, 2.5 uV/LSB */
#define INA226_REG_BUS_V        0x02  /* unsigned 16-bit, 1.25 mV/LSB */
#define INA226_REG_POWER        0x03
#define INA226_REG_CURRENT      0x04
#define INA226_REG_CALIB        0x05
#define INA226_CAL              28000 //27,947 actually
#define INA226_CURRENT_LSB_UA   10 //9.16 actually

void    ina226_init(I2C_TypeDef *i2c, uint8_t addr);
int32_t ina226_read_bus_mv(I2C_TypeDef *i2c, uint8_t addr);
int32_t ina226_read_shunt_uv(I2C_TypeDef *i2c, uint8_t addr);
int32_t ina226_read_current_ua(I2C_TypeDef *i2c, uint8_t addr);
int32_t ina226_read_power_uw(I2C_TypeDef *i2c, uint8_t addr);

#endif
