module vga_avalon(input logic clk, input logic reset_n,
                  input logic [3:0] address,
                  input logic read, output logic [31:0] readdata,
                  input logic write, input logic [31:0] writedata,
                  output logic [7:0] vga_red, output logic [7:0] vga_green, output logic [7:0] vga_blue,
                  output logic vga_hsync, output logic vga_vsync, output logic vga_clk);
    // signals for vga
    logic VGA_HS;
	logic VGA_VS;
	logic VGA_BLANK;
	logic VGA_SYNC;
	logic VGA_CLK;
    logic plot;
    logic [9:0] VGA_R;
    logic [9:0] VGA_G;
    logic [9:0] VGA_B;
    logic [6:0] y_val;
    logic [7:0] x_val;
    logic [7:0] brightness;

    // assign vga_r,g,b module outputs to be low 8 bits of VGA_R,G,B
    assign vga_red = VGA_R[7:0];
    assign vga_green = VGA_G[7:0];
    assign vga_blue = VGA_B[7:0];

    
    // assign x, y and brightness to corresponding section of writedata
    assign y_val      = writedata[31:24];
    assign x_val      = writedata[23:16];
    assign brightness =   writedata[7:0];


    // module instantiation
    vga_adapter #( .RESOLUTION("320x240"), .MONOCHROME("FALSE"), .BITS_PER_COLOUR_CHANNEL(3) ) 
    vga(.resetn(reset_n), 
        .clock(clk), 
        .colour(brightness),
        .x(x_val), 
        .y(y_val), 
        .plot(plot),
        .VGA_R(VGA_R), 
        .VGA_G(VGA_G), 
        .VGA_B(VGA_B),
        .VGA_HS(vga_hsync),
		.VGA_VS(vga_vsync),
		.VGA_CLK(vga_clk),
        .*);


    always@(*)begin
        plot = ((write === 1'b1)&(address === 4'b0)&(x_val < 8'd160)&(y_val < 7'd120)) ? 1'b1 : 1'b0;
    end

endmodule: vga_avalon

// ~~~ Notes and Instructions ~~~

/*The module you are to write will follow a very simple write-only interface. 
  Whenever address offset 0 is written and the coordinates are within screen boundaries,
  you should send exactly one pixel plot event to the VGA core. (If the coordinates are outside of the screen,
  the write should be ignored.)

  The write request consists of a single 32-bit word with address offset 0, with the following bit encoding:

  word	bits	meaning
  0	    30..24	y coordinate (7 bits)
  0	    23..16	x coordinate (8 bits)
  0	    7..0	brightness (0=black, 255=white)

  As usual, the top-left corner corresponds to coordinates (0,0).
  Your Avalon module should ignore read requests as well as writes to locations other than offset 0.*/