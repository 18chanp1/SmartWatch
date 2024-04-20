#include <system.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/times.h>
#include "sys/alt_stdio.h"
#include <sys/alt_irq.h>
#include <altera_avalon_pio_regs.h>
#include <priv/alt_legacy_irq.h>
#include "sys/alt_sys_wrappers.h"
#include "altera_up_avalon_video_dma_controller.h"
#include "altera_up_avalon_video_rgb_resampler.h"
#include "altera_up_avalon_rs232.h"
#include "altera_up_avalon_rs232_regs.h"
#include <i2c_opencores_regs.h>
#include <i2c_opencores.h>
#include <altera_avalon_timer_regs.h>
#include <altera_avalon_timer.h>

#define READ_FIFO_EMPTY 0x0

#define WIDTH 120
#define HEIGHT 200
#define MAX_ROWS WIDTH*HEIGHT
#define ip_addr1 "206.12.41.249\0"
const char* de1 = "123\0";

#define BUT_WIDTH 20
#define BUT_HEIGHT 20
#define BOX_WIDTH 40
#define BOX_HEIGHT 16

#define STEP_LENGTH 0.8 //in meters
int color_text_background = 0x0;
int color_display_background= 0x0;
alt_u8 data[MAX_ROWS];

volatile int state = 0;
volatile int change = 0;

int workout_id = 0;

int wp_button[4];
int rp_download_button[4];
int rp_pad_buttonx[3];
int rp_pad_buttony[3];

int rp_button[4];

int fp_button_u[2];
int fp_button_re[2];

float g = 4096 / 9.81;


float xAccel = 0;
float yAccel = 0;
float zAccel = 0;


float previousVector = 0;
float currentVector = 0;


alt_16 _xAccel = 0;
alt_16 _yAccel = 0;
alt_16 _zAccel = 0;

volatile float seconds = 0;
float ms = 0;
alt_u8 above = 0;
alt_32 count = 0;
float steps = 0;
float total_steps = 0;
float final_duration = 0;
volatile float speed = 0;
volatile float total_distance = 0;
volatile float total_duration = 0;
int hadtouched = 0;
float steps_this_second = 0;
float steps_past[5] = {0, 0, 0, 0, 0};
alt_u8 pos_step_array;

/*                              TOUCH SCREEN FUNCTIONS                                       */
alt_up_rs232_dev* touch_screen_dev;
alt_up_rs232_dev* init_touchscreen();
alt_u8 get_touch(alt_up_rs232_dev* screen, alt_u16* xPos, alt_u16* yPos);
alt_u8 draw_touch(alt_up_rs232_dev* screen, alt_up_video_dma_dev* pixel_dma_dev, unsigned int color);
alt_u8 can_read_device(alt_up_rs232_dev* dev);
alt_u8 read_char(alt_up_rs232_dev* dev);
alt_u8 read_device(alt_up_rs232_dev* dev, alt_u8* buffer, alt_u16 buflen);
void init_ts_interrupt( void );
static void ts_isr( void * context, alt_u32 id);

/*                              WIFI MODULE FUNCTIONS                                        */
alt_up_rs232_dev* wifi_rs232;
alt_up_rs232_dev* init_wifi();
void read_image_array(alt_up_rs232_dev *rs232,char* host,char *uri,int id);
void upload_workout_data(alt_up_rs232_dev *rs232,char* host,char* id, float workoutDistance, float workoutTime);
//void read_image_array(alt_up_rs232_dev *rs232,char* requestStr);

/*                              VGA FUNCTIONS                                                */
void welcomePage(alt_up_video_dma_dev* pixel_dev, alt_up_video_dma_dev* char_dev); //0
void finishPage(alt_up_video_dma_dev* pixel_dev, alt_up_video_dma_dev* char_dev);  //3
void downloadPage(alt_up_video_dma_dev* pixel_dev, alt_up_video_dma_dev* char_dev); //1
void runningPage(alt_up_video_dma_dev* pixel_dev, alt_up_video_dma_dev* char_dev); //2
void showId(alt_up_video_dma_dev* char_dev, char* str);
void drawString(alt_up_video_dma_dev* char_dev, unsigned int x, unsigned int y, char* str);
void drawRunning(alt_up_video_dma_dev* char_dev, int x, int y, char* init, float s);

alt_up_video_dma_dev* pixel_dev;
alt_up_video_dma_dev* char_dev;
void init_accl_interrupt( void );
static void accl_isr(void * context, alt_u32 id);



int main(void) {
	/* open wifi device */
	wifi_rs232 = init_wifi();

	touch_screen_dev = init_touchscreen();
	init_ts_interrupt();

	/* Initialize pixel controller */
	pixel_dev = alt_up_video_dma_open_dev("/dev/VGA_Subsystem_VGA_Pixel_DMA");
	if (pixel_dev == NULL)
		alt_printf("Error: could not open VGA's DMA controller device\n");
	else
		alt_printf("Opened VGA's DMA controller device\n");
    alt_up_video_dma_screen_clear(pixel_dev, 0);


	char_dev = alt_up_video_dma_open_dev("/dev/VGA_Subsystem_Char_Buf_Subsystem_Char_Buf_DMA");
	if (char_dev == NULL) {
		alt_printf(
			"Error: could not open character buffer's DMA controller device\n");
	} else
		alt_printf("Opened character buffer's DMA controller device\n");


	alt_up_video_dma_ctrl_set_bb_addr(char_dev, VGA_SUBSYSTEM_CHAR_BUF_SUBSYSTEM_ONCHIP_SRAM_BASE);
	alt_up_video_dma_ctrl_swap_buffers(char_dev);

	while (alt_up_video_dma_ctrl_check_swap_status(char_dev));
	alt_up_video_dma_ctrl_set_bb_addr(
		char_dev, VGA_SUBSYSTEM_CHAR_BUF_SUBSYSTEM_ONCHIP_SRAM_BASE);


	if (pixel_dev->data_width == 1) {
		color_text_background	= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_8BIT(color_text_background);
		color_display_background = ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_8BIT(color_display_background);
	}
	else if (pixel_dev->data_width == 2) {
		color_text_background	= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_16BIT(color_text_background);
		color_display_background = ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_16BIT(color_display_background);
	}
	welcomePage(pixel_dev, char_dev);


	//init touchscreen button location
	state = 0;
	wp_button[0] = 140;
	wp_button[1] = 172;
	wp_button[2] = 40;
	wp_button[3] = 16;

	rp_pad_buttonx[0] = 190;
	rp_pad_buttonx[1] = 230;
	rp_pad_buttonx[2] = 271;
	rp_pad_buttony[0] = 20;
	rp_pad_buttony[1] = 80;
	rp_pad_buttony[2] = 140;
	rp_download_button[0]=61;
	rp_download_button[1]=200;
	rp_download_button[2]=42;
	rp_download_button[3]=16;

	rp_button[0] = 60;
	rp_button[1] = 182;

	fp_button_u[0] = 60;
	fp_button_u[1] = 200;

	fp_button_re[0] = 220;
	fp_button_re[1] = 200;

	 I2C_init(I2C_OPENCORES_BASE, ALT_CPU_FREQ, 400*1000);

   printf("Worked?: %lu\n", I2C_start(I2C_OPENCORES_BASE, 0x68, 0));
   printf("Worked?: %lu\n", I2C_write(I2C_OPENCORES_BASE, 0x75, 0));
   printf("Worked?: %lu\n", I2C_start(I2C_OPENCORES_BASE, 0x68, 1));

   alt_u32 name = I2C_read(I2C_OPENCORES_BASE, 1);

   if (name == 0x68) {
		init_accl_interrupt();
   }

	while(1){
		if (state == 2){
			if (speed == 0){
				drawString(char_dev, 5,10, "Speed (m/s): 0    \0");
			}
			else {
				drawRunning(char_dev, 5,10, "Speed (m/s): \0", speed);
			}
			if (steps == 0) {
				drawString(char_dev, 5,15, "Steps: 0    \0");
			} else {
				drawRunning(char_dev, 5,15, "Steps: \0", steps);
			}
			if (seconds == 0){
				drawString(char_dev, 5,20, "Duration (s): 0          \0");
			}
			else{
				drawRunning(char_dev, 5,20, "Duration (s): \0", seconds + ms / 1000.0f);
			}
			if (total_distance != 0 && total_duration != 0){
				float avg_speed = total_distance / total_duration;
//				printf("Avg_Speed: %f\n", avg_speed);
				drawRunning(char_dev, 5,30, "Average Speed (m/s): \0", avg_speed);
			}
			else{
				drawString(char_dev, 5,30, "Average Speed (m/s): 0      \0");
			}

			if (seconds > 5 && steps_past[0] == 0 && steps_past[1] == 0 && steps_past[2] == 0 && steps_past[3] == 0 && steps_past[4] == 0) {

				IOWR_ALTERA_AVALON_PIO_DATA(BUZZER_BASE, 0xff);
			} else {
				IOWR_ALTERA_AVALON_PIO_DATA(BUZZER_BASE, 0x0);
			}
		}
		else if (state == 3) {
			IOWR_ALTERA_AVALON_PIO_DATA(BUZZER_BASE, 0x0);
			finishPage(pixel_dev,char_dev);
			state = 4;
		}
	}
	return 0;
}

alt_u8 get_button_touched(alt_u16 x,alt_u16 y,alt_u16 x_0,alt_u16 y_0,alt_u16 width,alt_u16 height){
	if(x >=x_0 && x <= x_0+width && y >= y_0 && y<= y_0+height){
		return 1;
	}else{
		return 0;
	}
}
//---------------------------------------------------INTERRUPT HANDLER--------------------------------------------------

void init_ts_interrupt(void){
	/* register TS_ISR */
	alt_irq_register(TOUCHSCREEN_RS232_IRQ, 0x0, (void *)ts_isr);
}

/*
 * Touch screen interrupt handler
 */
static void ts_isr( void * context, alt_u32 id){
	alt_u8 down;
	alt_u16 x;
	alt_u16 y;
	down = get_touch(touch_screen_dev,&x,&y);
	x= x*320/4096;
	y= y*240/4096;
	/*welcome page*/
	if(state == 0){
		hadtouched = 0;
		if(get_button_touched(x,y,wp_button[0],wp_button[1],wp_button[2],wp_button[3])){
//			printf("button touched!\n");
			state = 1;
			downloadPage(pixel_dev, char_dev);
			showId(char_dev, "Id entered: *\0");
		}
	}
	/*download page*/
	else if(state ==1){
		if(get_button_touched(x,y,rp_download_button[0],rp_download_button[1],rp_download_button[2],rp_download_button[3])){
			printf("button touched!ready to download\n");
			showId(char_dev, "DOWNLOADING...\0");
			read_image_array(wifi_rs232,ip_addr1,"requestImage",workout_id);
			showId(char_dev, "FINISH DOWNLOAD !!!\0");
			seconds = 0;
			steps = 0;
			total_distance = 0;
			total_duration = 0;
			for (int i = 0; i < 5; i++){
				steps_past[i] =0 ;
			}
			state = 2;
			runningPage(pixel_dev, char_dev);

//			alt_irq_enable(TOUCHSCREEN_RS232_IRQ);
		}
		else if(get_button_touched(x,y,rp_pad_buttonx[0],rp_pad_buttony[2],BUT_WIDTH,BUT_HEIGHT)){
			workout_id = 1;
//			printf("button touched!id = %d\n",workout_id);
			showId(char_dev, "Id entered: 1\0");
		}

		else if(get_button_touched(x,y,rp_pad_buttonx[1],rp_pad_buttony[2],BUT_WIDTH,BUT_HEIGHT)){
			workout_id = 2;
//			printf("button touched!id = %d\n",workout_id);
			showId(char_dev, "Id entered: 2\0");
		}
		else if(get_button_touched(x,y,rp_pad_buttonx[2],rp_pad_buttony[2],BUT_WIDTH,BUT_HEIGHT)){
			workout_id = 3;
//			printf("button touched!id = %d\n",workout_id);
			showId(char_dev, "Id entered: 3\0");
		}else if(get_button_touched(x,y,rp_pad_buttonx[0],rp_pad_buttony[1],BUT_WIDTH,BUT_HEIGHT)){
			workout_id = 4;
//			printf("button touched!id = %d\n",workout_id);
			showId(char_dev, "Id entered: 4\0");
		}else if(get_button_touched(x,y,rp_pad_buttonx[1],rp_pad_buttony[1],BUT_WIDTH,BUT_HEIGHT)){
			workout_id = 5;
//			printf("button touched!id = %d\n",workout_id);
			showId(char_dev, "Id entered: 5\0");
		}else if(get_button_touched(x,y,rp_pad_buttonx[2],rp_pad_buttony[1],BUT_WIDTH,BUT_HEIGHT)){
			workout_id = 6;
//			printf("button touched!id = %d\n",workout_id);
			showId(char_dev, "Id entered: 6\0");
		}else if(get_button_touched(x,y,rp_pad_buttonx[0],rp_pad_buttony[0],BUT_WIDTH,BUT_HEIGHT)){
			workout_id = 7;
//			printf("button touched!id = %d\n",workout_id);
			showId(char_dev, "Id entered: 7\0");
		}else if(get_button_touched(x,y,rp_pad_buttonx[1],rp_pad_buttony[0],BUT_WIDTH,BUT_HEIGHT)){
			workout_id = 8;
//			printf("button touched8!id = %d\n",workout_id);
			showId(char_dev, "Id entered: 8\0");
		}else if(get_button_touched(x,y,rp_pad_buttonx[2],rp_pad_buttony[0],BUT_WIDTH,BUT_HEIGHT)){
			workout_id = 9;
//			printf("button touched!id = %d\n",workout_id);
			showId(char_dev, "Id entered: 9\0");
		}else if (get_button_touched(x,y,190,200, BUT_WIDTH, BUT_HEIGHT)){
			workout_id = 0;
//			printf("button touched8!id = %d\n",workout_id);
			showId(char_dev, "Id entered: 0\0");
		}else if (get_button_touched(x,y,271,200, BUT_WIDTH, BUT_HEIGHT)){
			workout_id = -1;
//			printf("DEL button touched!\n");
			showId(char_dev, "Id entered: *\0");

		}
	}
//	/*running page*/
	else if(state ==2){
		if(get_button_touched(x,y,rp_button[0],rp_button[1],BOX_WIDTH,BOX_HEIGHT)){
//			printf("END RUN: button touched!\n");
			state = 3;
			finishPage(pixel_dev, char_dev);
		}
	}
	/* Finish Page*/
	else if (state >= 3){
		//upload
		if(get_button_touched(x,y, fp_button_u[0],fp_button_u[1],BOX_WIDTH, BOX_HEIGHT)){
//			printf("Upload button touched\n");
			if (!hadtouched){
				hadtouched = 1;
			   char id_str[2];
			   id_str[0] = (char)(48 + workout_id);
			   id_str[1] = '\0';
			   printf("%f\n", total_distance);
			   printf("%f\n", final_duration);
				upload_workout_data(wifi_rs232,ip_addr1, id_str, total_distance,final_duration);
				drawString(char_dev,12,46, "------DONE!------\0");
			}
		}
		//restart
		else if (get_button_touched(x,y, fp_button_re[0],fp_button_re[1],BOX_WIDTH, BOX_HEIGHT)){
			state = 0;
			welcomePage(pixel_dev, char_dev);
		}
	}
}

void init_accl_interrupt( void ){
   I2C_start(I2C_OPENCORES_BASE, 0x68, 0);
   I2C_write(I2C_OPENCORES_BASE, 0x6B, 0);
   I2C_write(I2C_OPENCORES_BASE, 0x0, 1);


   I2C_start(I2C_OPENCORES_BASE, 0x68, 0);
   I2C_write(I2C_OPENCORES_BASE, 0x1C, 0);
   I2C_write(I2C_OPENCORES_BASE, 0x18, 1);



   I2C_start(I2C_OPENCORES_BASE, 0x68, 0);
   I2C_write(I2C_OPENCORES_BASE, 0x1B, 0);
   I2C_write(I2C_OPENCORES_BASE, 0x0, 1);

    alt_irq_register(I2C_TIMER_IRQ, 0x0, (void*) accl_isr);
    IOWR_ALTERA_AVALON_TIMER_CONTROL(I2C_TIMER_BASE, ALTERA_AVALON_TIMER_CONTROL_CONT_MSK
                                               | ALTERA_AVALON_TIMER_CONTROL_START_MSK
                                               | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK);
}

/*
 * Accelerometer ISR
 */
static void accl_isr( void * context, alt_u32 id ) {
//	printf("ACCL ISR\n");
	IOWR_ALTERA_AVALON_TIMER_STATUS(I2C_TIMER_BASE, 0);
	I2C_start(I2C_OPENCORES_BASE, 0x68, 0);
	I2C_write(I2C_OPENCORES_BASE, 0x3B, 0);
	I2C_start(I2C_OPENCORES_BASE, 0x68, 1);

	_xAccel = (I2C_read(I2C_OPENCORES_BASE, 0)<< 8 | I2C_read(I2C_OPENCORES_BASE, 0));
	_yAccel = (I2C_read(I2C_OPENCORES_BASE, 0)<< 8 | I2C_read(I2C_OPENCORES_BASE, 0));
	_zAccel = (I2C_read(I2C_OPENCORES_BASE, 0)<< 8 | I2C_read(I2C_OPENCORES_BASE, 1));

	if(_xAccel >> 15) {
		xAccel = - ((float) ~_xAccel);
	} else {
		xAccel =  _xAccel;
	}
	if(_yAccel >> 15) {
		yAccel = - ((float) ~_yAccel);
	} else {
		yAccel = (float) _yAccel;
	}
	if(_zAccel >> 15) {
		zAccel = - ((float) ~_zAccel);
	} else {
		zAccel = (float) _zAccel;
	}

	previousVector = currentVector;
	currentVector = sqrt((xAccel * xAccel) + (yAccel * yAccel) + (zAccel * zAccel));
	if (currentVector - previousVector > 270 && !above) {
		above++;
		steps++;
		steps_this_second++;
	}
	if (above) {
		if (count < 20) {
			count++;
		} else {
			count = 0;
			above = 0;
		}
	}
	ms += 10;
	if (ms >= 1000){
		seconds++;
		steps_past[pos_step_array] = steps_this_second;
		pos_step_array++;
		if (pos_step_array >= 5) {
			pos_step_array = 0;
		}
		speed = ((steps_past[0]+steps_past[1]+steps_past[2]+steps_past[3]+steps_past[4])*STEP_LENGTH)/5;
		total_distance += steps_this_second * STEP_LENGTH;
		total_duration = seconds;
		total_steps += steps_this_second;
		steps_this_second = 0;
		ms = 0;
	}
}

//------------------------------------------------------HELPER FUNCTIONS--------------------------------------------------------------------
/*
 * HELPER FUNCTIONS
 */

/*RGB 8 bits to 16 bits converter*/
unsigned rgb8bitTo16bit(unsigned colour){
	unsigned red, green, blue;
	unsigned color8;
	red = (colour >> 5);
	red = red << 2;

	green = (colour >> 2) & (0b000111);
	green = green << 3;

	blue = colour & (0b00000011);
	blue = blue << 3;

	color8 = red << 11 | green << 5 | blue;
	return color8;
}
void drawRunning(alt_up_video_dma_dev* char_dev, int x, int y, char* init, float s){
	char buf[100];
	gcvt(s,5,buf);
	char str[50];
	strcpy(str, init);
	strcat(str, buf);
	drawString(char_dev, x, y,str);
}

//-----------------------------------------------VGA DRAWING FUNCTIONS--------------------------------------------------------------------
/*
 *  Clear the screen
 *  Parameters:
 *            pixel_dev:    pixel controller device handle
 *            char_dev :    character buffer controller device handle
 */
void clearScreen(alt_up_video_dma_dev* pixel_dev, alt_up_video_dma_dev* char_dev){
	alt_up_video_dma_screen_clear(char_dev, 0);
	alt_up_video_dma_screen_clear(pixel_dev, 0);
}


/*
 *  Draw a box starting from position (x, y)
 *  Parameters:
 *            pixel_dev		:   pixel controller device handle
 *            x, y			: 	starting position (top-left corner of the resulting box)
 *            color			: 	color of the parameters of the box (should be 8bits color (rrrgggbb))
 *            width, height	: 	width and height of the resulting box
 */
void drawBox(alt_up_video_dma_dev* pixel_dev, unsigned int x, unsigned int y, unsigned int width, unsigned int height, int colour){
	alt_up_video_dma_draw_box(pixel_dev, rgb8bitTo16bit(colour), x, y, x + width, y + height, 0, 0);
}

/*
 *  Draw a line starting from position (x1, y1) to (x2,y2)
 *  Parameters:
 *            pixel_dev	:   pixel controller device handle
 *            x1, y1	: 	starting position
 *            x2, y2	: 	ending position
 *            color		: 	color of the resulting line (should be 8 bits color (rrrgggbb))
 */
void drawLine(alt_up_video_dma_dev* pixel_dev, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int colour){
	alt_up_video_dma_draw_line(pixel_dev, rgb8bitTo16bit(colour), x1, y1, x2, y2, 0);
}

/*
 *  Draw an image starting from position (x_top, y_top) from a array pixel (image[])
 *  Parameters:
 *            pixel_dev   	:   pixel controller device handle
 *            x_top, y_top	: 	starting position (top-left corner of the resulting image)
 *            image[]     	:	array of color of pixels of the image (color should be 8bits color (rrrgggbb))
 *            width, height	: 	width and height of the resulting image
 */
void drawImage(alt_up_video_dma_dev* pixel_dev, alt_u8 image[], unsigned x_top, unsigned y_top,  alt_u16 width, alt_u16 height) {
	alt_u16 xStep = 1;
	alt_u16 yStep = 1;

	if (width > pixel_dev->x_resolution) {
		xStep = width / pixel_dev->x_resolution;
	}
	if (height > pixel_dev->y_resolution) {
		yStep = height / pixel_dev->y_resolution;
	}
	alt_u8 bitWidth = 1;
	if (pixel_dev->data_width == 2) {
		bitWidth = 2;
	}
	int i = 0;
	unsigned int color;
	for (alt_u16 y = y_top; y*yStep < height + y_top; y++) {
		for (alt_u16 x = x_top; x*xStep < width + x_top; x++) {
			color = rgb8bitTo16bit(image[i]);
			alt_up_video_dma_draw(pixel_dev, color,
					x, y, 0);
			i++;
		}
	}
}

/*
 *  Write a string starting from position (x, y) with white color
 *  Parameters:
 *            char_dev:    character buffer controller device handle
 *            x, y    :    starting position
 *            str     :    the string that needs to be written
 */
void drawString(alt_up_video_dma_dev* char_dev, unsigned int x, unsigned int y, char* str){
	if (alt_up_video_dma_draw_string(char_dev, str, x, y, 0) == -1){
		printf("ERROR printing str\n");
	}
}

//-----------------------------------------------TOUCH SCREEN'S VIEWS--------------------------------------------------------------------
void drawId(alt_up_video_dma_dev* char_dev){
	drawString(char_dev,0, 0, "DE1 Id: 123");
}
void drawGrid(alt_up_video_dma_dev* pixel_dev){
	drawLine(pixel_dev, 160,0, 160,239,0xff);
	drawLine(pixel_dev,80,0, 80,239,0xff);
	drawLine(pixel_dev,240,0, 240,239,0xff);

	drawLine(pixel_dev,0,60, 319,60,0xff);
	drawLine(pixel_dev,0,120, 319,120,0xff);
	drawLine(pixel_dev,0,180, 319,180,0xff);
}

void welcomePage(alt_up_video_dma_dev* pixel_dev, alt_up_video_dma_dev* char_dev){
	clearScreen(pixel_dev,char_dev);
	drawId(char_dev);
	drawString(char_dev,32, 14, "Fitness Tracker\0");
	drawString(char_dev,32, 14, "Fitness Tracker\0");
	drawString(char_dev,29, 20, "Group 29: HeartBreaker\0");
	drawString(char_dev,34, 26, "UBC CPEN 391\0");

	drawBox(pixel_dev, 140,172, 40, 16, 0xff);
	drawString(char_dev,38,45, "Start\0");
}

void finishPage(alt_up_video_dma_dev* pixel_dev, alt_up_video_dma_dev* char_dev){
	clearScreen(pixel_dev,char_dev);
	alt_up_video_dma_screen_clear(char_dev, 0);
	drawId(char_dev);

	if (total_duration == 0 || total_distance == 0){
		drawString(char_dev, 5,10 , "Average Speed (m/s): 0     ");
	}
	else{
		float avg = total_distance / total_duration;
		drawRunning(char_dev , 5, 10, "Average Speed (m/s): \0",avg);
	}
	drawRunning(char_dev, 5,15, "Total Distance (m): \0", total_distance);

	final_duration = seconds + ms / 1000.0f;
	drawRunning(char_dev, 5, 20, "Duration (s): \0", final_duration);
	drawRunning(char_dev,5, 30, "Total Steps: \0", steps);

	drawBox(pixel_dev,135,20,165,100,0xff);
	drawString(char_dev, 35, 10, "Instructions\0");
	drawString(char_dev, 35, 15, "1. Press Upload to upload the current\0");
	drawString(char_dev, 35, 20, "   workout data to website\0");
	drawString(char_dev, 35, 25, "2. Press Restart to start new workout\0");


	drawBox(pixel_dev,60,200, 40, 16, 0xff);
	drawString(char_dev,17,52, "Upload\0");

	drawBox(pixel_dev,220,200, 40, 16, 0xff);
	drawString(char_dev,57,52, "Restart\0");
}

void downloadPage(alt_up_video_dma_dev* pixel_dev, alt_up_video_dma_dev* char_dev){
	clearScreen(pixel_dev,char_dev);
	drawId(char_dev);
	drawString(char_dev,2,10,"Instructions\0");
	drawString(char_dev,2,15,"1. Plan your workout route on the website\0");
	drawString(char_dev,2,20,"2. Enter the Workout Id received\0");
	drawString(char_dev,2,25,"   from the website using the keypad\0");
	drawString(char_dev,2,30,"3. Press Download button to download\0");
	drawString(char_dev,2,35,"   the planned route\0");

	drawBox(pixel_dev,61,200, 42, 16, 0xff);
	drawString(char_dev,17,52, "Download\0");

	//Draw number pad
	drawBox(pixel_dev,190,20, 20, 20, 0xff);
	drawString(char_dev,50,7, "7\0");

	drawBox(pixel_dev,230,20, 20, 20, 0xff);
	drawString(char_dev,60,7, "8\0");

	drawBox(pixel_dev,271,20, 20, 20, 0xff);
	drawString(char_dev,70,7, "9\0");

	drawBox(pixel_dev,190,80, 20, 20, 0xff);
	drawString(char_dev,50,22, "4\0");
	drawBox(pixel_dev,230,80, 20, 20, 0xff);
	drawString(char_dev,60,22, "5\0");
	drawBox(pixel_dev,271,80, 20, 20, 0xff);
	drawString(char_dev,70,22, "6\0");

	drawBox(pixel_dev,190,140, 20, 20, 0xff);
	drawString(char_dev,50,37, "1\0");

	drawBox(pixel_dev,230,140, 20, 20, 0xff);
	drawString(char_dev, 60,37, "2\0");

	drawBox(pixel_dev,271,140, 20, 20, 0xff);
	drawString(char_dev, 70,37, "3\0");

	drawBox(pixel_dev,190,200, 20, 20, 0xff);
	drawString(char_dev, 50,52, "0\0");

	drawBox(pixel_dev,271,200, 20, 20, 0xff);
	drawString(char_dev,69,52, "DEL\0");
}

void showId(alt_up_video_dma_dev* char_dev, char* str){
	drawString(char_dev,13,45, str);
}

void runningPage(alt_up_video_dma_dev* pixel_dev, alt_up_video_dma_dev* char_dev){
	clearScreen(pixel_dev,char_dev);
	drawId(char_dev);
	drawString(char_dev, 5,10, "Speed (m/s): \0");
	drawString(char_dev, 5,15, "Steps: \0");
	drawString(char_dev, 5,20, "Duration (s): \0");
	drawString(char_dev, 5, 30, "Average Speed (m/s): \0");

	drawBox(pixel_dev,60,185, 40, 16, 0xff);
	drawString(char_dev, 17,48, "END RUN\0");
	drawImage(pixel_dev, data, 180,20,120,200);

}

//-----------------------------------------------WIFI MODULE--------------------------------------------------------------------

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
	else if(mode == 5) mes = "dofile(\"wifi.lua\")\n\n";
	else mes = '\0';
	alt_up_rs232_disable_read_interrupt(rs232);
	for(int i=0; i < strlen(mes); i++){
		alt_up_rs232_write_data(rs232, mes[i]);
	}
}
/*
 *  Initializes wifi module
 *  Return: the device handle
 */
alt_up_rs232_dev* init_wifi() {
	alt_up_rs232_dev* wifi_rs232 = alt_up_rs232_open_dev(WIFI_RS232_NAME);
	if (wifi_rs232 == NULL){ alt_printf("Error: could not open RS232 UART\n");}
	else {
		char receive;
		alt_u8 parity_error;
		alt_up_rs232_enable_read_interrupt(wifi_rs232);
		while(alt_up_rs232_get_used_space_in_read_FIFO(wifi_rs232) > 0){
			alt_up_rs232_read_data(wifi_rs232, &receive, &parity_error);
//			printf("%c",receive);
		}
		alt_printf("Opened RS232 UART device\n");
		alt_up_rs232_enable_read_interrupt(wifi_rs232);
		while(alt_up_rs232_get_used_space_in_read_FIFO(wifi_rs232) > 0){
			alt_up_rs232_read_data(wifi_rs232, &receive, &parity_error);
//			printf("%c",receive);
		}
	}
    return wifi_rs232;
}

void read_image_array(alt_up_rs232_dev *rs232,char* host,char *uri,int id){
	/* clear buffer */
	char receive;
	alt_u8 parity_error;
	alt_up_rs232_enable_read_interrupt(rs232);
	while(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
			alt_up_rs232_read_data(rs232, &receive, &parity_error);
	}
	/* send command */
    int host_len = strlen(host);
    int uri_len = strlen(uri);

    char id_str[1];
    id_str[0] = (char)(48 + id);

    char* splitter = "\",\"\0";
    int splitter_len = strlen(splitter);

    char* end = "\")\n\0";
    int end_len = strlen(end);

    char* header = "get_image(\"\0";
    int header_len = strlen(header);

    int request_len = header_len + host_len + splitter_len + uri_len +
    				splitter_len + 1 + end_len;

    char request[request_len];
    strcpy(request, header);

    strcat(request,host);
    strcat(request, splitter);
    strcat(request,uri);
    strcat(request, splitter);
    strcat(request, id_str);
    strcat(request, end);

	alt_up_rs232_disable_read_interrupt(rs232);

	int i = 0;
	while (i < strlen(request)){
		if (alt_up_rs232_get_available_space_in_write_FIFO(rs232) > 0){
			alt_up_rs232_write_data(rs232, request[i]);
			i++;
		}
	}
//	for(int i=0; i < strlen(request); i++){
//		alt_up_rs232_write_data(rs232, request[i]);
//		usleep(100000);
//	}

	/* read response */
	int start = 0;
	alt_up_rs232_enable_read_interrupt(rs232);

	int pos=0;
	int count = 0;
	showId(char_dev, "SENDING REQUEST...\0");
	while(!start ){
		i++;
		if(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
			alt_up_rs232_read_data(rs232, &receive, &parity_error);
//			printf("%c", receive);
		}

		if(receive == '[') {
			start = 1;
			alt_up_rs232_read_data(rs232, &receive, &parity_error); //skip a "\n" after "["
		}
	}
	printf("%s\n", request);
	showId(char_dev, "READING DATA...\0");
	while(count < MAX_ROWS){
		if(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
			alt_up_rs232_read_data(rs232, &receive, &parity_error);
			count++;
			data[pos++]=(alt_u16)receive;
			if(count%4800==0){
				char percent[3];
				percent[2] = '\0';
				int i = 1;
				int temp = (int)count/240;
				int remainder = 0;
				while (temp > 0){
					remainder = temp % 10;
					percent[i] = (char)(48+remainder);
					temp = temp/10;
					i--;
				}
				char* str = "DOWNLOADING \0";
				char* start = ".....\0";
				char* end = "%....\0";
				int len = strlen(start) + strlen(str) + strlen(percent) + strlen(end);

				char msg[len];
				strcpy(msg,start);
				strcat(msg,str);
				strcat(msg,percent);
				strcat(msg,end);
				drawString(char_dev, 10,45, msg);
			}
		}
	}
	alt_up_rs232_enable_read_interrupt(rs232);
	while(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
		alt_up_rs232_read_data(rs232, &receive, &parity_error);
	}
	alt_up_rs232_disable_read_interrupt(rs232);
}


void upload_workout_data(alt_up_rs232_dev *rs232,char* host,char* id, float workoutDistance, float workoutTime){
	/* clear buffer */
	char receive;
	alt_u8 parity_error;
	alt_up_rs232_enable_read_interrupt(rs232);
	while(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
			alt_up_rs232_read_data(rs232, &receive, &parity_error);
	}

	drawString(char_dev,12,46, "....Uploading....\0");
	const char* de1Serial = de1;
	int host_len = strlen(host);
    int id_len = strlen(id);
    int serial_len = strlen(de1Serial);

    char distance_buf[10];
    gcvt(workoutDistance, 5, distance_buf);
    int distance_len = strlen(distance_buf);

    char time_buf[10];
    gcvt(workoutTime, 5, time_buf);

    int time_len = strlen(time_buf);

    char* splitter = "\",\"\0";
    int splitter_len = strlen(splitter);

    char* end = "\")\n\0";
    int end_len = strlen(end);

    char* header = "upload_data(\"\0";
    int header_len = strlen(header);

    int request_len = header_len + host_len + splitter_len + id_len +
    				splitter_len +  distance_len + splitter_len + serial_len + splitter_len + time_len + end_len;

    char request[request_len];
    strcpy(request, header);

    strcat(request,host);
    strcat(request, splitter);
    strcat(request, id);
    strcat(request, splitter);
    strcat(request,distance_buf);
    strcat(request, splitter);
    strcat(request, de1Serial);
    strcat(request, splitter);
    strcat(request, time_buf);
    strcat(request, end);
	printf("%s\n", request);
	alt_up_rs232_disable_read_interrupt(rs232);
	for(int i=0; i < strlen(request); i++){
		alt_up_rs232_write_data(rs232, request[i]);
	}
	usleep(100000);
	alt_up_rs232_enable_read_interrupt(rs232);
	while(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
		alt_up_rs232_read_data(rs232, &receive, &parity_error);
	}
	alt_up_rs232_disable_read_interrupt(rs232);
}

//-----------------------------------------------TOUCH SCREEN MODULE--------------------------------------------------------------------
/*
 *  Initializes the Touchscreen serial connection, and sends a 'touch_enable' command to it
 *  Return: the device handle
 */
alt_up_rs232_dev* init_touchscreen() {
  alt_up_rs232_dev* touchscreen = alt_up_rs232_open_dev(TOUCHSCREEN_RS232_NAME);
  if (touchscreen == NULL) {
    alt_printf("Error: could not open touchscreen UART\n");
  } else {
    alt_printf("Opened Touchscreen UART device\n");
    alt_up_rs232_write_data(touchscreen, 0x55);
    alt_up_rs232_write_data(touchscreen, 0x01);
    alt_up_rs232_write_data(touchscreen, 0x12);
    alt_printf("Sent Touch Enable\n");
    alt_up_rs232_enable_read_interrupt(touchscreen);
  }
  	alt_u8 receive;
  	alt_u8 parity_error;

	while(alt_up_rs232_get_used_space_in_read_FIFO(touchscreen) > 0){
		alt_up_rs232_read_data(touchscreen, &receive, &parity_error);
//		printf("%c",receive);
	}
  return touchscreen;
}

/*
 *  Reads serial data into a buffer, line by line
 *  Parameters:
 *            dev:    device handle
 *            buffer: buffer to write to
 *            buflen: length of buffer
 *  Return:   1 if successful read; 0 if can't read
 */
alt_u8 read_device(alt_up_rs232_dev* dev, alt_u8* buffer, alt_u16 buflen) {
  alt_u16 pos = 0;
  alt_u16 read_FIFO_used;
  alt_u8 p_error;
  alt_u8 data;

  read_FIFO_used = alt_up_rs232_get_used_space_in_read_FIFO(dev);
  if ((read_FIFO_used > READ_FIFO_EMPTY) && (pos < buflen)) {
    alt_up_rs232_read_data(dev, &data, &p_error);

    read_FIFO_used = alt_up_rs232_get_used_space_in_read_FIFO(dev);
    *buffer = data;
    pos++;

    while ((pos < (buflen-1)) && (buffer[pos-1] != 0x0A)) {
        while (read_FIFO_used == READ_FIFO_EMPTY) {
          read_FIFO_used = alt_up_rs232_get_used_space_in_read_FIFO(dev);
        }
        alt_up_rs232_read_data(dev, &data, &p_error);
        *(buffer+pos) = data;
        pos++;
        read_FIFO_used = alt_up_rs232_get_used_space_in_read_FIFO(dev);
      }
    *(buffer+pos) = '\0';
    return -1;
  }
  return 0;
}

/*
 *  Reads the touchscreen serial connection and parses the data into
 *  simple x and y cooridinates that can be used directly as integers
 *  Parameters:
 *            screen: the touchscreen handle
 *            xPos:   a variable into which the x coordinate will be stored
 *            yPos:   a variable into which the y coordinate will be stored
 *  Return:   An integer representing the type of touch
 *              1:  Pressing down
 *              0:  Stop Pressing
 *             -1:  Can't read touchscreen
 */
alt_u8 get_touch(alt_up_rs232_dev* screen, alt_u16* xPos, alt_u16* yPos) {
  alt_u8 looking = 1;
  alt_u8 temp;
  *xPos = 0;
  *yPos = 0;
  alt_u8 penDown = 0;
  alt_u8 count = 0;
  alt_u8 temp_str[4];

  while(looking){
	  temp = read_char(screen);
	  if ((temp == 0x80) || (temp == 0x81)) {
		if (temp == 0x81) { // So we can return whether its a down or up
		  penDown = 1;
		}
		looking = 0;
	  }
  }
  while(count <4){
	  temp = read_char(screen);
	  if(temp != -1){
		  temp_str[count]=temp;
		  count++;
	  }
  }

  *xPos += temp_str[0];
  *xPos = *xPos | (temp_str[1] << 7);
  *yPos += temp_str[2];
  *yPos = *yPos | (temp_str[3] << 7);
  return penDown;
}


/*
 * 	Draws touches from the touchscreen, onto the vga display
 * 	Parameters:
 * 			screen: 		the touchscreen device
 * 			pixel_dma_dev:	the vga display device
 * 			color:			which color to display
 * 	Return:
 * 		1 on success
 * 		0 on failure
 */
alt_u8 draw_touch(alt_up_rs232_dev* screen, alt_up_video_dma_dev * pixel_dma_dev, unsigned int color) {
	alt_u16 x;
	alt_u16 y;
	if (can_read_device(screen)) {
	  get_touch(screen, &x, &y);
	  x = x * pixel_dma_dev->x_resolution / 4096;
	  y = y * pixel_dma_dev->y_resolution / 4096;
	  return (alt_up_video_dma_draw(pixel_dma_dev, color, x, y, 0) == 0);
	}
	return 0;
}

/*
 *  Checks whether the serial connection has data ready to be read
 *  Parameters:
 *        dev:  device handle
 *  Return: 1 if able to be read, 0 if not
 */
alt_u8 can_read_device(alt_up_rs232_dev* dev) {
  alt_u16 read_FIFO_used;
  read_FIFO_used = alt_up_rs232_get_used_space_in_read_FIFO(dev);
    if (read_FIFO_used <= READ_FIFO_EMPTY) {
      ALT_USLEEP(1000000);
      read_FIFO_used = alt_up_rs232_get_used_space_in_read_FIFO(dev);
      if (read_FIFO_used <= READ_FIFO_EMPTY) {
        return 0;
      }
    }
    return 1;
}

/*
 *  Reads a single character from serial connection buffer
 *  Parameters:
 *         dev: device handle
 *  Return: next character from serial connection buffer
 *          -1 on error
 */
alt_u8 read_char(alt_up_rs232_dev* dev) {
  alt_u8 p_error;
  alt_u8 data;
  if (can_read_device(dev)) {
    alt_up_rs232_read_data(dev, &data, &p_error);
    return data;
  }

  return -1;
}

//-----------------------------------------------------------END OF FILE--------------------------------------------------------------
