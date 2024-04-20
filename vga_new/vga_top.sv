module vga_top(input logic CLOCK_50, input logic [3:0] KEY, // KEY[3] is async active-low reset
             input logic [9:0] SW, output logic [9:0] LEDR,
             output logic [7:0] VGA_R, output logic [7:0] VGA_G, output logic [7:0] VGA_B,
             output logic VGA_HS, output logic VGA_VS, output logic VGA_CLK,
             output logic DRAM_CLK, output logic DRAM_CKE,
             output logic DRAM_CAS_N, output logic DRAM_RAS_N, output logic DRAM_WE_N,
             output logic [12:0] DRAM_ADDR, output logic [1:0] DRAM_BA, output logic DRAM_CS_N,
             inout logic [15:0] DRAM_DQ, output logic DRAM_UDQM, output logic DRAM_LDQM,
             output logic [6:0] HEX0, output logic [6:0] HEX1, output logic [6:0] HEX2,
             output logic [6:0] HEX3, output logic [6:0] HEX4, output logic [6:0] HEX5,
				 inout logic [35:0] GPIO_0, inout logic [35:0] GPIO_1);
    assign HEX1 = 7'b1111111;
    assign HEX2 = 7'b1111111;
    assign HEX3 = 7'b1111111;
    assign HEX4 = 7'b1111111;
    assign HEX5 = 7'b1111111;
    assign LEDR[8:0] = 9'b000000000;
    
    vga_top_hw u0(.clk_clk(CLOCK_50), 
					 .reset_reset_n(KEY[0]),
					 .locked_pll_export(LEDR[9]),
					 .vga_out_vga_red(VGA_R),
					 .vga_out_vga_green(VGA_G),
					 .vga_out_vga_blue(VGA_B),
					 .vga_out_vga_hsync(VGA_HS),
					 .vga_out_vga_vsync(VGA_VS),
					 .vga_out_vga_clk(VGA_CLK),
					 .sdram_clk_clk(DRAM_CLK),
					 .sdram_out_addr(DRAM_ADDR),
					 .sdram_out_ba(DRAM_BA),
					 .sdram_out_cas_n(DRAM_CAS_N),
					 .sdram_out_cke(DRAM_CKE),
					 .sdram_out_cs_n(DRAM_CS_N),
					 .sdram_out_dq(DRAM_DQ),
					 .sdram_out_dqm({DRAM_UDQM, DRAM_LDQM}),
					 .sdram_out_ras_n(DRAM_RAS_N),
					 .sdram_out_we_n(DRAM_WE_N),
					 .hex_out_export(HEX0),
					 .buzzer_out_export (GPIO_1[28]), 
					 .sw_out_export     (SW),     
					 .key_out_export    (~KEY),    
					 .rs232_out_RXD     (GPIO_1[27]),     
					 .rs232_out_TXD     (GPIO_1[26])     
					);
endmodule: vga_top
