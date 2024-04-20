
//unsigned char pixel_list[] = {
//
//#include "C:\CPEN391\VGA\pixels.txt"
//};
#include <stdio.h>
#include "sys/alt_stdio.h"
#include <math.h>
#include <stdlib.h>
#include <system.h>
#include <string.h>
#include <sys/alt_irq.h>
#include <altera_avalon_timer_regs.h>
#include <altera_up_avalon_rs232.h>
#include <altera_up_avalon_rs232_regs.h>
#include <priv/alt_legacy_irq.h>
#include <altera_avalon_pio_regs.h>
#include <time.h>
#include "Fonts.h"

#define width 100
#define height 100
#define MAX_ROWS width*height


alt_u8 data[MAX_ROWS]={0};

volatile unsigned *vga_addr = (volatile unsigned *) 0x00004000; /* VGA adapter base address */

void vga_plot(unsigned x, unsigned y, unsigned long colour)
{
    *vga_addr = (y << 24) + (x << 16) + colour;
}

/*wifi command for testing*/
void wifiCommand(int mode,alt_up_rs232_dev *rs232){
	char* mes;
	//mode 1: simple light LED command test
	if(mode == 1)mes = "gpio.write(3, gpio.LOW)\n";
	//mode 2: blinking LED test
	else if(mode == 2) mes = "dofile(\"script1.lua\")\n";
	//mode 3:  connect wifi test
	else if(mode == 3) mes = "dofile(\"test_connect.lua\")\n";
	//mode 3: simpleGetrequest test
	else if(mode == 4) mes = "dofile(\"test_httpGET_uart40.lua\")\n";
	//mode 4: test uart write
	else if(mode == 5) mes = "dofile(\"wifi.lua\")\n";
	//mode 5: upload data
	else if(mode == 6) mes = "get_image(\"192.168.1.69\",\"requestImage\",\"0\",\"0\")\n";
	else mes = '\0';
	alt_up_rs232_disable_read_interrupt(rs232);
	for(int i=0; i < strlen(mes); i++){
		alt_up_rs232_write_data(rs232, mes[i]);
		alt_printf("%c",mes[i]);
	}
}

void read_image_array(alt_up_rs232_dev *rs232,char* host,char *uri,int longitude,int latitude){
	/* clear buffer */
	char receive;
	alt_u8 parity_error;
	while(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
			alt_up_rs232_read_data(rs232, &receive, &parity_error);
//			printf("%c",receive);
	}

	/* send command */
//	wifiCommand(3,rs232);
//	usleep(10000);
	wifiCommand(5,rs232);
	usleep(10000);
	char *str= "get_image(\"206.12.44.228\",\"requestImage\",\"0\",\"0\")\n";
	sprintf(str, "get_image(\"%s\",\"%s\",\"%d\",\"%d\")\n", host,uri,longitude,latitude);
	alt_up_rs232_disable_read_interrupt(rs232);
	for(int i=0; i < strlen(str); i++){
		alt_up_rs232_write_data(rs232, str[i]);
		alt_printf("%c",str[i]);
	}

	/* read response */
	int start = 0;
	alt_up_rs232_enable_read_interrupt(rs232);

	int pos=0;
	int count = 0;
//	char str[MAX_ROWS];
	while(!start){
		if(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
			alt_up_rs232_read_data(rs232, &receive, &parity_error);
//			printf("%c",receive);
		}
		if(receive == '[') {
			start = 1;
			alt_up_rs232_read_data(rs232, &receive, &parity_error); //skip a "\n" after "["
//			str[pos++]=receive;
		}
	}
	while(count < MAX_ROWS){
		if(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
			alt_up_rs232_read_data(rs232, &receive, &parity_error);
//			printf("%c",receive);
//			if(receive != '\n'){
				count++;
				data[pos++]=(alt_u16)receive;
//				printf("%02x",(alt_u8)receive);
//				printf("%02x",(alt_u8)receive);
//				printf("%d\n",count);
				if(count%2000==0){
					printf("%d\n",count);
				}
		}
//		if(count > 100) printf("%d\n",alt_up_rs232_get_used_space_in_read_FIFO(rs232));
	}
//	for(int x = 0; x < MAX_ROWS;x++){
//			printf("%2x\n", data[x]);
//	}
	printf("\n");
	printf("end read array...\n");
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
int sign(int x){
	if (x < 0){
		return -1;
	}
	else if (x == 0){
		return 0;
	}
	else {
		return 1;
	}
}
void drawLine(int x1, int y1, int x2, int y2, int Colour)
{
	int x = x1;
	int y = y1;
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int s1 = sign(x2 - x1);
	int s2 = sign(y2 - y1);
	int i, temp, interchange = 0, error ;
	// if x1=x2 and y1=y2 then it is a line of zero length so we are done
	if(dx == 0 && dy == 0){
		return ;
	}
	// must be a complex line so use Bresenhams algorithm
	else {
		// swap delta x and delta y depending upon slop of line
		if(dy > dx) {
			temp = dx ;
			dx = dy ;
			dy = temp ;
			interchange = 1 ;
		}
		// initialise the error term to compensate for non-zero intercept
		error = (dy << 1) - dx ; // error = (2 * dy) - dx
		// main loop
		for(i = 1; i <= dx; i++) {
			vga_plot(x, y, Colour);
			while(error >= 0) {
				if(interchange == 1){
					x += s1 ;
				}
				else{
					y += s2 ;
				}
				error -= (dx << 1) ; // error = error – (dx * 2)
			}
			if(interchange == 1){
				y += s2 ;
			}
			else{
				x += s1 ;
			}
			error += (dy << 1) ; // error = error + (dy * 2)
		}
	}
}

void drawBox(int x, int y, int w, int h, int colour){
	int x_end = x + w;
	int y_end = y + h;
	drawLine(x,y, x_end, y, colour);
	drawLine(x_end,y, x_end,y_end, colour);
	drawLine(x_end,y_end, x,y_end, colour);
	drawLine(x,y_end, x,y, colour);
}
void drawCharacter(int x, int y, int fontcolour, int backgroundcolour, int c, int Erase)
{
// using register variables (as opposed to stack based ones) may make execution faster
// depends on compiler and CPU
	register int row, column, theX = x, theY = y ;
	register int pixels ;
	register char theColour = fontcolour  ;
	register int BitMask, theC = c ;
	// if x,y coord off edge of screen don't bother
    if(((short)(x) > (short)(width-1)) || ((short)(y) > (short)(height-1))){
        return ;
    }


    // if printable character subtract hex 20
	if(((short)(theC) >= (short)(' ')) && ((short)(theC) <= (short)('~'))) {
		theC = theC - 0x20 ;
		for(row = 0; (char)(row) < (char)(7); row ++)	{

			// get the bit pattern for row 0 of the character from the software font
			pixels = Font5x7[theC][row] ;
			BitMask = 16 ;

			for(column = 0; (char)(column) < (char)(5); column ++)	{
				// if a pixel in the character display it
				if((pixels & BitMask))
					vga_plot(theX+column, theY+row, theColour) ;

				else {
					if(Erase == 1)
						// if pixel is part of background (not part of character)
						// erase the background to value of variable BackGroundColour
						vga_plot(theX+column, theY+row, backgroundcolour) ;
				}
				BitMask = BitMask >> 1 ;
			}
		}
	}
}

int main()
{
	alt_putstr("Hello from Nios II!\n");
	for (int i = 0; i < MAX_ROWS; i++){
		data[i] = 0;
	}
	/* open device */
	struct alt_up_rs232_dev *wifi_rs232;
	wifi_rs232 = alt_up_rs232_open_dev("/dev/rs232");
	if (wifi_rs232 == NULL){ alt_printf("Error: could not open RS232 UART\n");}
	else {alt_printf("Opened RS232 UART device\n");}


	for(int i = 0; i < 160; i++){
		for(int j = 0; j < 120; j++){
			vga_plot(i,j,0x0);	// initialize screen to black

		}
	}

//	drawBox(40,50, 73, 17, 0xff);
//	int x = 45;
//	int y = 55;
//	char *str = "HELLO WORLD";
//	char *p = str;
//
//	for (char c = *p; c != '\0'; c = *++p){
//	  printf("%c\n", c);
//	  if (x > width - 5){
//		  y = y + 8;
//		  x = 10;
//	  }
//	  drawCharacter(x,y,0xff,0x0,c, 0);
//	  x = x + 6;
//	  usleep(1000);
//	}

	read_image_array(wifi_rs232,"206.12.42.186","requestImage",0,0);
	int i = 0;
	for (unsigned y = 0; y < height; y++){
		for (unsigned x =0; x < width; x++){
			vga_plot(x,y, data[i]);
			i++;
			usleep(100);
		}
	}

	printf("DONE \n ");
    return 0;
}




//	unsigned colour[] = {0xed1c24, 0xffffff, 0x22b14c, 0x000000, 0xffaec9, 0x00a2e8, 0xffc90e, 0xa349a4, 0xc3c3c3, 0xc8bfe7, 0xfff200, 0xb97a57};




