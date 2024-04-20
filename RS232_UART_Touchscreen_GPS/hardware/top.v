module top(input CLOCK_50, input [3:0] KEY, inout [35:0] GPIO_1, inout [35:0] GPIO_0, output [9:0] LEDR);

    test u0(
		.clk_clk(CLOCK_50),
		.gps_rs232_RXD(GPIO_1[0]),
		.gps_rs232_TXD(GPIO_1[1]),
		.reset_reset_n(1'b1),
		.spi_0_external_MISO(GPIO_1[4]),
		.spi_0_external_MOSI(GPIO_1[5]),
		.spi_0_external_SCLK(GPIO_1[6]),
		.spi_0_external_SS_n(GPIO_1[7]),
		.touchscreen_rs232_RXD(GPIO_1[2]),
		.touchscreen_rs232_TXD(GPIO_1[3]),
		.i2c_scl(GPIO_1[8]),
		.i2c_sda(GPIO_1[9])
	);
	
//	module test (
//		input  wire  clk_clk,               //               clk.clk
//		input  wire  gps_rs232_RXD,         //         gps_rs232.RXD
//		output wire  gps_rs232_TXD,         //                  .TXD
//		inout  wire  i2c_scl,               //               i2c.scl
//		inout  wire  i2c_sda,               //                  .sda
//		input  wire  reset_reset_n,         //             reset.reset_n
//		input  wire  spi_0_external_MISO,   //    spi_0_external.MISO
//		output wire  spi_0_external_MOSI,   //                  .MOSI
//		output wire  spi_0_external_SCLK,   //                  .SCLK
//		output wire  spi_0_external_SS_n,   //                  .SS_n
//		input  wire  touchscreen_rs232_RXD, // touchscreen_rs232.RXD
//		output wire  touchscreen_rs232_TXD  //                  .TXD
//	);


	reg LED_ON;
	reg [31:0] count;
	assign LEDR = {9{LED_ON}};
	assign GPIO_0[7] = 0;
	
	always@(posedge CLOCK_50) begin
		if (count >= 32'd250000000) begin
			LED_ON <= 1'b1;
		end else begin
			LED_ON <= 1'b0;
		end
		
		if (count >= 32'd500000000) begin
			count <= 32'd0;
		end else begin
			count <= count + 32'd1;
		end
	end

endmodule
