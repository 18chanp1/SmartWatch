

module ComputerSystem (
	// Clock pins
	CLOCK_50,
	CLOCK2_50,

	// SDRAM
	DRAM_ADDR,
	DRAM_BA,
	DRAM_CAS_N,
	DRAM_CKE,
	DRAM_CLK,
	DRAM_CS_N,
	DRAM_DQ,
	DRAM_LDQM,
	DRAM_RAS_N,
	DRAM_UDQM,
	DRAM_WE_N,

	// 40-Pin Headers
	GPIO_0,
	GPIO_1,
	
	// Seven Segment Displays
	HEX0,
	HEX1,
	HEX2,
	HEX3,
	HEX4,
	HEX5,

	// Pushbuttons
	KEY,

	// LEDs
	LEDR,

	// Slider Switches
	SW,

	// VGA
	VGA_B,
	VGA_BLANK_N,
	VGA_CLK,
	VGA_G,
	VGA_HS,
	VGA_R,
	VGA_SYNC_N,
	VGA_VS,
);

//=======================================================
//  PARAMETER declarations
//=======================================================


//=======================================================
//  PORT declarations
//=======================================================

// Clock pins
input						CLOCK_50;
input						CLOCK2_50;


// SDRAM
output 		[12: 0]	DRAM_ADDR;
output		[ 1: 0]	DRAM_BA;
output					DRAM_CAS_N;
output					DRAM_CKE;
output					DRAM_CLK;
output					DRAM_CS_N;
inout			[15: 0]	DRAM_DQ;
output					DRAM_LDQM;
output					DRAM_RAS_N;
output					DRAM_UDQM;
output					DRAM_WE_N;

// 40-pin headers
inout			[35: 0]	GPIO_0;
inout			[35: 0]	GPIO_1;

// Seven Segment Displays
output		[ 6: 0]	HEX0;
output		[ 6: 0]	HEX1;
output		[ 6: 0]	HEX2;
output		[ 6: 0]	HEX3;
output		[ 6: 0]	HEX4;
output		[ 6: 0]	HEX5;


// Pushbuttons
input			[ 3: 0]	KEY;

// LEDs
output		[ 9: 0]	LEDR;

// Slider Switches
input			[ 9: 0]	SW;


// VGA
output		[ 7: 0]	VGA_B;
output					VGA_BLANK_N;
output					VGA_CLK;
output		[ 7: 0]	VGA_G;
output					VGA_HS;
output		[ 7: 0]	VGA_R;
output					VGA_SYNC_N;
output					VGA_VS;

//=======================================================
//  REG/WIRE declarations
//=======================================================

wire			[31: 0]	hex3_hex0;
wire			[15: 0]	hex5_hex4;

assign HEX0 = ~hex3_hex0[ 6: 0];
assign HEX1 = ~hex3_hex0[14: 8];
assign HEX2 = ~hex3_hex0[22:16];
assign HEX3 = ~hex3_hex0[30:24];
assign HEX4 = ~hex5_hex4[ 6: 0];
assign HEX5 = ~hex5_hex4[14: 8];

//=======================================================
//  Structural coding
//=======================================================

ComputerSystem_HW Design (
	// Global signals
	.system_pll_ref_clk_clk					(CLOCK_50),
	.system_pll_ref_reset_reset			(1'b0),
	.video_pll_ref_clk_clk					(CLOCK2_50),
	.video_pll_ref_reset_reset				(1'b0),

	// Slider Switches
	.slider_switches_export					(SW),

	// Pushbuttons
	.pushbuttons_export						(~KEY[3:0]),


	// LEDs
	.leds_export								(LEDR),
	
	// Seven Segs
	.hex3_hex0_export							(hex3_hex0),
	.hex5_hex4_export							(hex5_hex4),

	// VGA Subsystem
	.vga_CLK										(VGA_CLK),
	.vga_BLANK									(VGA_BLANK_N),
	.vga_SYNC									(VGA_SYNC_N),
	.vga_HS										(VGA_HS),
	.vga_VS										(VGA_VS),
	.vga_R										(VGA_R),
	.vga_G										(VGA_G),
	.vga_B										(VGA_B),

	// SDRAM
	.sdram_clk_clk								(DRAM_CLK),
	.sdram_addr									(DRAM_ADDR),
	.sdram_ba									(DRAM_BA),
	.sdram_cas_n								(DRAM_CAS_N),
	.sdram_cke									(DRAM_CKE),
	.sdram_cs_n									(DRAM_CS_N),
	.sdram_dq									(DRAM_DQ),
	.sdram_dqm									({DRAM_UDQM,DRAM_LDQM}),
	.sdram_ras_n								(DRAM_RAS_N),
	.sdram_we_n									(DRAM_WE_N),
	
	//	Touchscreen RS232
	.touchscreen_RXD						(GPIO_1[0]),
	.touchscreen_TXD						(GPIO_1[1]),
	
	// Wifi RS232
	.wifi_RXD								(GPIO_1[2]),
	.wifi_TXD								(GPIO_1[3]),
	
	//	Accel SPI
	.accel_MISO								(GPIO_0[32]),
	.accel_MOSI								(GPIO_0[16]),
	.accel_SCLK								(GPIO_0[14]),
	.accel_SS_n								(GPIO_0[31]),
	
	.i2c_scl									(GPIO_1[8]),
	.i2c_sda									(GPIO_1[9])
	);


endmodule
