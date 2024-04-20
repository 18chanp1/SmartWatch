
//unsigned char pixel_list[] = {
//
//#include "C:\CPEN391\VGA\pixels.txt"
//};
#include <stdio.h>
#include <math.h>

//unsigned num_pixels = sizeof(pixel_list)/2;
volatile unsigned *vga_addr = (volatile unsigned *) 0x00004000; /* VGA adapter base address */

void vga_plot(unsigned x, unsigned y, unsigned colour)
{
    *vga_addr = (y << 24) + (x << 16) + colour;
}

// ~~~ Notes and Instructions ~~~

/*
  The write request consists of a single 32-bit word with address offset 0, with the following bit encoding:

  word	bits	meaning
  0	    30..24	y coordinate (7 bits)
  0	    23..16	x coordinate (8 bits)
  0	    7..0	bgr (bbgggrrr)
*/
#define RGB_24BIT_TO_8BIT(c)        (((c >> 6) & 0x000000C0) | ((c >> 11) & 0x00000038) | ((c >> 16) & 0x00000007))
unsigned RGB24to8BitConverter(unsigned colour24b){
	unsigned rgb = colour24b;
    unsigned r = (rgb >> 16) & 0xFF;
    unsigned g = (rgb >> 8) & 0xFF;
    unsigned b = rgb & 0xFF;

    unsigned B = b >> 6;
    unsigned G = g >>  5;
    unsigned R = r >> 5;

    unsigned RGB = (B << 6) | ( G << 3 ) | R;
    unsigned colour = RGB;
//	unsigned  mask;
//	mask = 0x00ff00;
//	unsigned green = (colour24b & mask)>> 8;
//	unsigned g = floor(green/32);
//
//	mask = 0xff0000;
//	unsigned red = (colour24b&mask)>>16;
//	unsigned r = floor(red/32);
//
//	mask = 0x0000ff;
//	unsigned blue = colour24b & mask;
//	unsigned b = floor(blue/32);
//	mask = 0x3;
//	b = b & mask;
//
//	unsigned colour = 0;
//	colour = colour + (b << 6) + (g << 3) + r;

	return colour;
}
void toBinary(unsigned colour){
		int a[10], i;
		for (i = 0; colour > 0; i++){

			a[i] = colour % 2;
			colour = colour /2;
		}

		for (i = i-1; i >= 0; i--){
			printf("%d", a[i]);
		}
		printf("\n");
}
int main()
{
	unsigned colour[] = {0xed1c24, 0xffffff, 0x22b14c, 0x000000, 0xffaec9, 0x00a2e8, 0xffc90e, 0xa349a4, 0xc3c3c3, 0xc8bfe7, 0xfff200, 0xb97a57};
	for(int i = 0; i < 160; i++)
	{
		for(int j = 0; j < 120; j++)
		{
			vga_plot(i,j,0);	// initialize screen to black
		}
	}
	for (int i = 0; i < 12; i++){
		for (int x = 0; x < 160; x++){
			vga_plot(x,59+i, RGB24to8BitConverter(colour[i]));

		}

	}

}





//	unsigned char x;
//	unsigned char y;
//	unsigned int colour = 0; // colour is black

	//                    row(y) col(x)
//	unsigned char pixel_arr[130][170] = {{0}};  // initialize pixel array to all zero (ie black)


//for(int i = 0; i < 160; i++)
//{
//	for(int j = 0; j < 120; j++)
//	{
//		vga_plot(i,j,0);	// initialize screen to black
//	}
//}

//	colour = 255; // colour is white
//	for(int k = 1; k < (num_pixels*2); k = k + 2)
//	{
//		x = pixel_list[k-1];
//		y = pixel_list[k];

//		pixel_arr[y][x] = 1;  // set pixel to 1

//		vga_plot(x,y,255);
//	}

	// apply filter to image with 2D array
//	for(int i = 0; i < 160; i++)
//	{
//		for(int j = 0; j < 120; j++)
//		{
//			colour =
//
//			(((pixel_arr[j][i])  * 1) + ((pixel_arr[j][i+1])   * 2) + ((pixel_arr[j][i+2])   *  4) + ((pixel_arr[j][i+3])   * 2) + ((pixel_arr[j][i+4])   * 1) +
//
//		 	((pixel_arr[j+1][i]) * 2) + ((pixel_arr[j+1][i+1]) * 4) + ((pixel_arr[j+1][i+2]) *  8) + ((pixel_arr[j+1][i+3]) * 4) + ((pixel_arr[j+1][i+4]) * 2) +
//
//		 	((pixel_arr[j+2][i]) * 4) + ((pixel_arr[j+2][i+1]) * 8) + ((pixel_arr[j+2][i+2]) * 16) + ((pixel_arr[j+2][i+3]) * 8) + ((pixel_arr[j+2][i+4]) * 4) +
//
//		 	((pixel_arr[j+3][i]) * 2) + ((pixel_arr[j+3][i+1]) * 4) + ((pixel_arr[j+3][i+2]) *  8) + ((pixel_arr[j+3][i+3]) * 4) + ((pixel_arr[j+3][i+4]) * 2) +
//
//		 	((pixel_arr[j+4][i]) * 1) + ((pixel_arr[j+4][i+1]) * 2) + ((pixel_arr[j+4][i+2]) *  4) + ((pixel_arr[j+4][i+3]) * 2) + ((pixel_arr[j+4][i+4]) * 1));
//
//		 	if(colour > 255)  // if the colour is greater than 255, set to 255
//		 	{
//				colour = 255;
//		 	}
//
//			vga_plot(i,j,colour);
//		}
//
//	}

//}

// ~~~ Notes and Instructions ~~~

/*In a separate file main.c, you should #include the contents of vga_plot.c and add the function main().
 In the misc folder you will find a file called pixels.txt, which you should also #include in main.c as the initializing values of an array.
 The contents are a list of (x,y) pixel coordinates for an image. Write a function to draw all of these pixels in white by repeatedly calling vga_plot()
 and fill the background (unlisted pixels) with black. Modify the function to use shades of gray in some clever pattern. For example, each pixel has eight
 immediate neighbours and sixteen secondary neighbours; you can use choose a pixel brightness based on whether its neighbour pixels are set, weighing closer
  pixels more heavily. The weighting scheme below gray values in the range of 0 to 92 for a pixel located at the center (0,0) by consulting pixels +/- 1/2 rows/columns away.:*/

// note vga monitor is 160x120

/*  kernel

	1  2  4  2  1
	2  4  8  4  2
	4  8 16  8  4
	2  4  8  4  2
	1  2  4  2  1

*/

