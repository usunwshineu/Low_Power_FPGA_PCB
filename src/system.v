/*
 * system.v - top-level system for picorv32
 * 06-30-19 E. Brombaugh
 */

`default_nettype none

module system(
	input	clk_12,			// clock, reset
			reset,
	
	input	RX,				// serial
	output	TX,
	
	inout	spi0_mosi,		// SPI core 0
			spi0_miso,
			spi0_sclk,
			spi0_cs0,
	
	inout	spi1_mosi,		// SPI core 1
			//spi1_miso,
			spi1_sclk,
			spi1_cs0,
	
	inout	i2c0_sda,		// I2C core 0
			i2c0_scl,

	inout	i2c1_sda,		// I2C core 1
			i2c1_scl,

	input 	[2:0] buttons,

	output reg [31:0] gp_out
);
	// CPU
	wire        mem_valid; //CPU requests access
	wire        mem_instr; 
	wire        mem_ready; //access finished
	wire [31:0] mem_addr; //address requested by CPU
	reg  [31:0] mem_rdata; //data returned to CPU
	wire [31:0] mem_wdata; //data written by CPU
	wire [ 3:0] mem_wstrb; //write byte enables
	picorv32 #(
		.PROGADDR_RESET(32'h 0000_0000),	// start or ROM
		.STACKADDR(32'h 1001_0000),			// end of SPRAM
		.BARREL_SHIFTER(1),
		.COMPRESSED_ISA(0),
		.ENABLE_COUNTERS(0),
		//.ENABLE_MUL(1),
		.ENABLE_FAST_MUL(1),
		.ENABLE_DIV(1),
		.ENABLE_IRQ(0),
		.ENABLE_IRQ_QREGS(0),
		.CATCH_MISALIGN(0),
		.CATCH_ILLINSN(0)
	) cpu_I (
		.clk       (clk_12),
		.resetn    (~reset),
		.mem_valid (mem_valid),
		.mem_instr (mem_instr),
		.mem_ready (mem_ready),
		.mem_addr  (mem_addr),
		.mem_wdata (mem_wdata),
		.mem_wstrb (mem_wstrb),
		.mem_rdata (mem_rdata)
	);
	
	// Address decode
	wire rom_sel = (mem_addr[31:28]==4'h0)&mem_valid ? 1'b1 : 1'b0;
	wire ram_sel = (mem_addr[31:28]==4'h1)&mem_valid ? 1'b1 : 1'b0;
	wire gpo_sel = (mem_addr[31:28]==4'h2)&mem_valid ? 1'b1 : 1'b0;
	wire ser_sel = (mem_addr[31:28]==4'h3)&mem_valid ? 1'b1 : 1'b0;
	wire wbb_sel = (mem_addr[31:28]==4'h4)&mem_valid ? 1'b1 : 1'b0;
	wire cnt_sel = (mem_addr[31:28]==4'h5)&mem_valid ? 1'b1 : 1'b0;
	wire btn_sel = (mem_addr[31:28]==4'h6)&mem_valid ? 1'b1 : 1'b0;
	
	// 2k x 32 ROM
	reg [31:0] rom[2047:0], rom_do;
	initial
        $readmemh("rom.hex",rom);		
	always @(posedge clk_12)
		rom_do <= rom[mem_addr[12:2]];
	
	// RAM, byte addressable
	wire [31:0] ram_do;
	spram_16kx32 uram(
		.clk(clk_12),
		.sel(ram_sel),
		.we(mem_wstrb),
		.addr(mem_addr[15:0]),
		.wdat(mem_wdata),
		.rdat(ram_do)
	);
	
	// GPIO
	always @(posedge clk_12)
		if(gpo_sel)
		begin
			if(mem_wstrb[0])
				gp_out[7:0] <= mem_wdata[7:0];
			if(mem_wstrb[1])
				gp_out[15:8] <= mem_wdata[15:8];
			if(mem_wstrb[2])
				gp_out[23:16] <= mem_wdata[23:16];
			if(mem_wstrb[3])
				gp_out[31:24] <= mem_wdata[31:24];
		end
	
	// Serial
	wire [7:0] ser_do;
	acia uacia(
		.clk(clk_12),			// system clock
		.rst(reset),			// system reset
		.cs(ser_sel),			// chip select
		.we(mem_wstrb[0]),		// write enable
		.rs(mem_addr[2]),		// address
		.rx(RX),				// serial receive
		.din(mem_wdata[7:0]),	// data bus input
		.dout(ser_do),			// data bus output
		.tx(TX),				// serial transmit
		.irq()					// interrupt request
	);
	
	// 256B Wishbone bus master and SB IP cores @ F100-F1FF
	wire [7:0] wbb_do;
	wire wbb_rdy;
	wb_bus uwbb(
		.clk(clk_12),			// system clock
		.rst(reset),			// system reset
		.cs(wbb_sel),			// chip select
		.we(mem_wstrb[0]),		// write enable
		.addr(mem_addr[9:2]),	// address
		.din(mem_wdata[7:0]),	// data bus input
		.dout(wbb_do),			// data bus output
		.rdy(wbb_rdy),			// bus ready
		.spi0_mosi(spi0_mosi),	// spi core 0 mosi
		.spi0_miso(spi0_miso),	// spi core 0 miso
		.spi0_sclk(spi0_sclk),	// spi core 0 sclk
		.spi0_cs0(spi0_cs0),	// spi core 0 cs
		.spi1_mosi(spi1_mosi),	// spi core 1 mosi
		//.spi1_miso(spi1_miso),	// spi core 1 miso
		.spi1_sclk(spi1_sclk),	// spi core 1 sclk
		.spi1_cs0(spi1_cs0),	// spi core 1 cs
		.i2c0_sda(i2c0_sda),	// i2c core 0 data
		.i2c0_scl(i2c0_scl),	// i2c core 0 clk
		.i2c1_sda(i2c1_sda),	// i2c core 1 data
		.i2c1_scl(i2c1_scl)		// i2c core 1 clk
	);
	
	// Resettable clock counter
	reg [31:0] cnt;
	always @(posedge clk_12)
		if(cnt_sel & |mem_wstrb)
		begin
			if(mem_wstrb[0])
				cnt[7:0] <= mem_wdata[7:0];
			if(mem_wstrb[1])
				cnt[15:8] <= mem_wdata[15:8];
			if(mem_wstrb[2])
				cnt[23:16] <= mem_wdata[23:16];
			if(mem_wstrb[3])
				cnt[31:24] <= mem_wdata[31:24];
		end
		else
			cnt <= cnt + 32'd1;
	
	// Read Mux
	always @(*)
		casex({btn_sel,cnt_sel,wbb_sel,ser_sel,gpo_sel,ram_sel,rom_sel})
			7'b0000001: mem_rdata = rom_do;
			7'b000001x: mem_rdata = ram_do;
			7'b00001xx: mem_rdata = gp_out;
			7'b0001xxx: mem_rdata = {{24{1'b0}},ser_do};
			7'b001xxxx: mem_rdata = {{24{1'b0}},wbb_do};
			7'b01xxxxx: mem_rdata = cnt;
			7'b1xxxxxx: mem_rdata = {29'b0, buttons};
			default: mem_rdata = 32'd0;
		endcase
	
	// ready flag
	reg mem_rdy;
	always @(posedge clk_12)
		if(reset)
			mem_rdy <= 1'b0;
		else
			mem_rdy <= (btn_sel|cnt_sel|ser_sel|gpo_sel|ram_sel|rom_sel) & ~mem_rdy;
	assign mem_ready = wbb_rdy | mem_rdy;

endmodule

