from machine import Pin, I2C
from time import sleep 
import struct

i2c = I2C(0, sda=Pin(4), scl=Pin(5), freq=100000)

INA226_ADDR1 = 0x40
INA226_ADDR2 = 0x41
R_SHUNT = 0.02 #in Ohms

REG_SHUNT_VOLTAGE = 0x01 #registers from DS
REG_BUS_VOLTAGE = 0x02

def read_s16(addr, reg):
    data = i2c.readfrom_mem(addr, reg, 2)
    return struct.unpack(">h", data)[0] #convert to decimal

def read_u16(addr, reg):
    data = i2c.readfrom_mem(addr, reg, 2)
    return struct.unpack(">H", data)[0]

while True:

    shunt_samples1 = []
    bus_samples1 = []

    shunt_samples2 = []
    bus_samples2 = []

    for _ in range(100):
        raw_shunt = read_s16(INA226_ADDR1, REG_SHUNT_VOLTAGE)
        raw_bus = read_u16(INA226_ADDR1, REG_BUS_VOLTAGE)

        shunt_samples1.append(raw_shunt)
        bus_samples1.append(raw_bus)

        raw_shunt = read_s16(INA226_ADDR2, REG_SHUNT_VOLTAGE)
        raw_bus = read_u16(INA226_ADDR2, REG_BUS_VOLTAGE)

        shunt_samples2.append(raw_shunt)
        bus_samples2.append(raw_bus)


    avg_shunt1 = sum(shunt_samples1) / len(shunt_samples1)
    avg_bus1 = sum(bus_samples1) / len(bus_samples1)
    avg_shunt2 = sum(shunt_samples2) / len(shunt_samples2)
    avg_bus2 = sum(bus_samples2) / len(bus_samples2)

    bus_v1 = avg_bus1 * 1.25e-3
    shunt_v1 = avg_shunt1 * 2.5e-6
    
    bus_v2 = avg_bus2 * 1.25e-3
    shunt_v2 = avg_shunt2 * 2.5e-6

    current_a1 = shunt_v1 / R_SHUNT
    power_w1 = bus_v1 * current_a1

    current_a2 = shunt_v2 / R_SHUNT
    power_w2 = bus_v2 * current_a2

    print("Bus voltage1:", bus_v1 , "V")
    print("Shunt voltage1:", shunt_v1, "V")
    print("Current1:", current_a1 * 1000, "mA")
    print("Power1:", power_w1 * 1000, "mW")
    print()

    print("Bus voltage2:", bus_v2 , "V")
    print("Shunt voltage2:", shunt_v2, "V")
    print("Current2:", current_a2 * 1000, "mA")
    print("Power2:", power_w2 * 1000, "mW")
    print()
    print("Total power:", (power_w2 + power_w1) * 1000 )
    print("**************")
    print()

    sleep(1)