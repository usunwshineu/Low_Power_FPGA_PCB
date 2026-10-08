# ICE40UP5K LOW POWER FPGA BOARD 

## PCB design

A custom low-power FPGA development board based on the Lattice iCE40UP5K FPGA. 
The board was designed to provide FPGA processing capabilities together with 
power monitoring and common communication interfaces.

The board contains:
* ICE40UP5K FPGA
* FTDI USB interface
* 12MHz clock (used for both the FPGA and FTDI)
* INA226 sensors for measuring power consumption
* I2C and SPI mode 0 PMODs
* SPI Flash memory
* I/O pins
* a reset button
* 3 buttons
* 4 LEDs and 1 RGB LED
  
For different SPI modes you can check <a href="https://0x04.net/~mwk/sbdocs/ice40/FPGA-TN-02010-1-8-iCE40-I2C-and-SPI-Hardened-IP-User-Guide.pdf" target="this document">.

The final assembled board:
<img width="1200" height="1159" alt="Assembled" src="https://github.com/user-attachments/assets/65e52ea7-b1c1-4008-889b-031e828e067f" />

## PCB programming 

I checked the functionality of the board by running some simple verilog and I used icestorm for creating the bitstream. 
You can check more about Project IceStorm here <a href="https://0x04.net/~mwk/sbdocs/ice40/FPGA-TN-02010-1-8-iCE40-I2C-and-SPI-Hardened-IP-User-Guide.pdf](https://prjicestorm.readthedocs.io/en/latest/overview.html#what-is-project-icestorm">.

The commands I used were: 
* yosys -p "synth_ice40 -top top -json module.json" module.v -> for synthesizing the verilog design
* nextpnr-ice40 --up5k --package sg48 --json module.json --pcf pins.pcf --asc module.asc -> place-and-route
* icepack module.asc module.bin -> converting the result into binary bitstream
* iceprog module.bin -> programming the SPI Flash

Replace module.v, module.json, module.asc, module.bin, and pins.pcf with the corresponding filenames for your design.

## Adding a microprocessor

I used this particular RISC V system, which was built specifically for iCE40UP5k FPGAs: <a href="https://github.com/emeb/up5k_riscv/blame/master/README.md">.
I modified the icestorm/up5k_riscv.pcf file with my own pins. Any other modified files were added to this repository.
In main.c file I wrote a code to read the power consumption (using I2C communication with the INA226 sensors) and display it on an OLED (SPI communication). 

This provides a complete demonstration of the board's:

* RISC V processing system
* I2C communication
* SPI communication
* Power monitoring
* OLED display interface
