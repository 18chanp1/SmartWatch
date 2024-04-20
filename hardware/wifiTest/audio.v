module audio(
input CLOCK_50,
 input [3:0] KEY, 
 inout [35:0] GPIO_0,
 inout [35:0] GPIO_1,
 input [9:0] SW,
 
 // SDRAM
output 		[12: 0]	DRAM_ADDR,
output		[ 1: 0]	DRAM_BA,
output					DRAM_CAS_N,
output					DRAM_CKE,
output					DRAM_CLK,
output					DRAM_CS_N,
inout			[15: 0]	DRAM_DQ,
output					DRAM_LDQM,
output					DRAM_RAS_N,
output					DRAM_UDQM,
output					DRAM_WE_N
 );

    ComponentTest u0(
		.clk_clk(CLOCK_50),
		.reset_reset_n(KEY[0]), 
		.rs232_external_interface_RXD(GPIO_1[27]), 
		.rs232_external_interface_TXD(GPIO_1[26]),
		.pio_0_external_connection_export(GPIO_1[28]),
		.sw_external_connection_export(SW),
		.key1_external_connection_export(~KEY),
		
		.sdram_addr(DRAM_ADDR),                            //                          sdram.addr
		.sdram_ba(DRAM_BA),                              //                               .ba
		.sdram_cas_n(DRAM_CAS_N),                           //                               .cas_n
		.sdram_cke(DRAM_CKE),                             //                               .cke
		.sdram_cs_n(DRAM_CS_N),                            //                               .cs_n
		.sdram_dq(DRAM_DQ),                              //                               .dq
		.sdram_dqm({DRAM_UDQM,DRAM_LDQM}),                             //                               .dqm
		.sdram_ras_n(DRAM_RAS_N),                           //                               .ras_n
		.sdram_we_n(DRAM_WE_N)
	);
	
	
endmodule