#include "altera_up_avalon_video_dma_controller.h"
#include "altera_up_avalon_video_rgb_resampler.h"
#include "sys/alt_stdio.h"
#include "altera_up_avalon_rs232.h"
#include "altera_up_avalon_rs232_regs.h"
#include "sys/alt_sys_wrappers.h"
#include <malloc.h>
#include <sys/times.h>
#include <system.h>


#define READ_FIFO_EMPTY 0x0
#define RGB_24BIT_BLACK			0x000000
#define RGB_24BIT_WHITE			0xFFFFFF
#define RGB_24BIT_BLUE			0x0000FF
#define RGB_24BIT_INTEL_BLUE		0x0071C5


alt_up_rs232_dev* init_gps();
alt_up_rs232_dev* init_touchscreen();

alt_u8 get_touch(alt_up_rs232_dev* screen, alt_u16* xPos, alt_u16* yPos);
alt_u8 draw_touch(alt_up_rs232_dev* screen, alt_up_video_dma_dev* pixel_dma_dev, unsigned int color);
alt_u8 write_text(alt_up_video_dma_dev* char_buffer, const char* str, unsigned int x, unsigned int y);
void draw_image(alt_up_video_dma_dev* pixel_dma_dev, alt_u8 image[][3], alt_u16 width, alt_u16 height);

//alt_u8 get_location(alt_up_rs232_dev* gps, alt_32* time, alt_32* latitude, alt_32* longitude);

alt_u8 can_read_device(alt_up_rs232_dev* dev);
alt_u8 read_char(alt_up_rs232_dev* dev);
alt_u8 read_device(alt_up_rs232_dev* dev, alt_u8* buffer, alt_u16 buflen);
/*******************************************************************************
 * This program demonstrates use of the video in the computer system.
 * Draws a blue box on the video display, and places a text string inside the
 * box
 ******************************************************************************/
int main(void) {
    /* The base addresses of devices are listed in the "BSP/system.h" file*/
	alt_up_rs232_dev* touchscreen_dev = init_touchscreen();
//	alt_u16 x;
//	alt_u16 y;
//	alt_u8 down;
    alt_up_video_dma_dev * pixel_dma_dev;
    alt_up_video_dma_dev * char_dma_dev;

    /* create a message to be displayed on the VGA display */
    char text[40] = "TESTING DISPAY\0";
//    char text_top_row[40]    = "Intel FPGA\0";
//    char text_bottom_row[40] = "Computer Systems\0";

    int color_text_background	= RGB_24BIT_INTEL_BLUE;
    int color_display_background = 0xFF00FF;

    /* initialize the pixel buffer HAL */
    pixel_dma_dev = alt_up_video_dma_open_dev("/dev/VGA_Subsystem_VGA_Pixel_DMA");
    if (pixel_dma_dev == NULL)
        alt_printf("Error: could not open VGA's DMA controller device\n");
    else
        alt_printf("Opened VGA's DMA controller device\n");

    /* clear the graphics screen */
    alt_up_video_dma_screen_clear(pixel_dma_dev, 0);

    /* output text message in the middle of the VGA monitor */
    char_dma_dev = alt_up_video_dma_open_dev(
        "/dev/VGA_Subsystem_Char_Buf_Subsystem_Char_Buf_DMA");
    if (char_dma_dev == NULL) {
        alt_printf(
            "Error: could not open character buffer's DMA controller device\n");
        return -1;
    } else
        alt_printf("Opened character buffer's DMA controller device\n");
    /* Make sure the front and back buffers point to base address of the onchip
     * memory */
    alt_up_video_dma_ctrl_set_bb_addr(
        char_dma_dev, VGA_SUBSYSTEM_CHAR_BUF_SUBSYSTEM_ONCHIP_SRAM_BASE);
    alt_up_video_dma_ctrl_swap_buffers(char_dma_dev);
    while (alt_up_video_dma_ctrl_check_swap_status(char_dma_dev))
        ;
    alt_up_video_dma_ctrl_set_bb_addr(
        char_dma_dev, VGA_SUBSYSTEM_CHAR_BUF_SUBSYSTEM_ONCHIP_SRAM_BASE);

    /* update colors */
    if (pixel_dma_dev->data_width == 1) {
        color_text_background	= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_8BIT(color_text_background);
    	color_display_background = ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_8BIT(color_display_background);
    }
    else if (pixel_dma_dev->data_width == 2) {
        color_text_background	= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_16BIT(color_text_background);
    	color_display_background = ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_16BIT(color_display_background);
    }

    alt_up_video_dma_screen_clear(char_dma_dev, 0);

    write_text(char_dma_dev, text, 100, 100);
    alt_up_video_dma_draw_box(pixel_dma_dev, color_text_background, 90, 90, 150, 110, 0, 1);
    alt_up_video_dma_screen_fill(pixel_dma_dev, color_display_background, 0);

//    int char_mid_x = char_dma_dev->x_resolution / 2;
//    int char_mid_y = char_dma_dev->y_resolution / 2;
//    alt_up_video_dma_draw_string(char_dma_dev, text_bottom_row, char_mid_x - 8,
//                                 char_mid_y, 0);

    /* now draw a background box for the text */
//	int x_diff_factor = pixel_dma_dev->x_resolution / char_dma_dev->x_resolution;
//	int y_diff_factor = pixel_dma_dev->y_resolution / char_dma_dev->y_resolution;
//    alt_up_video_dma_draw_box(
//        pixel_dma_dev, color_text_background,
//		(char_mid_x - 9) * x_diff_factor, (char_mid_y - 2) * y_diff_factor,
//        (char_mid_x + 9) * x_diff_factor - 1, (char_mid_y + 2) * y_diff_factor - 1,
//		0, 1);
//    alt_u16 x1;
//    alt_u16 x2;
//    alt_u16 y1;
//    alt_u16 y2;
//    while (!can_read_device(touchscreen_dev));
//    	down = get_touch(touchscreen_dev, &x, &y);
//  	  x2 = x * pixel_dma_dev->x_resolution / 4096;
//  	  y2 = y * pixel_dma_dev->y_resolution / 4096;
    while (1) 	{
//    	ALT_USLEEP(2000);
    	draw_touch(touchscreen_dev, pixel_dma_dev, color_text_background);
//        if (can_read_device(touchscreen_dev)) {
//        	down = get_touch(touchscreen_dev, &x, &y);
//        	x1 = x2;
//        	y1 = y2;
//          x2 = x * pixel_dma_dev->x_resolution / 4096;
//          y2 = y * pixel_dma_dev->y_resolution / 4096;
//          alt_up_video_dma_draw_line(
//                  pixel_dma_dev, color_text_background,
//          		x1, y1, x2, y2,
//          		0);
//          alt_printf("X: %x, Y: %x, and DOWN: %x\n", x, y, down);
//          if (down < 0) {
//            alt_printf("Don't see a touch!\n");
//          } else {
//            alt_printf("X: %x, Y: %x, and DOWN: %x\n", x, y, down);
//          }
//        }
      }

	return 0;
}

/*
 *  Initializes the GPS serial connection
 *  Return: the device handle
 */
//alt_up_rs232_dev* init_gps() {
//  alt_up_rs232_dev* gps = alt_up_rs232_open_dev(RS232_0_NAME);
//  if (gps == NULL) {
//    alt_printf("Error: could not open GPS UART\n");
//  } else {
//    alt_printf("Opened GPS UART device\n");
//    alt_up_rs232_enable_read_interrupt(gps);
//  }
//  return gps;
//}

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

  while (looking) {
    temp = read_char(screen);
    if ((temp == 0x80) || (temp == 0x81)) {
      if (temp == 0x81) { // So we can return whether its a down or up
        penDown = 1;
      }

      temp = read_char(screen); // First part of X (X0 - X6)
      if (temp == -1) {
        return -1;
      }
      *xPos += temp;

      temp = read_char(screen); // Second part of X (X7 - X11)
      if (temp == -1) {
        return -1;
      }
      *xPos = *xPos | (temp << 7);

      temp = read_char(screen); // Same thing as with the X
      if (temp == -1) {
        return -1;
      }
      *yPos += temp;

      temp = read_char(screen); // Same thing as with the X
      if (temp == -1) {
        return -1;
      }
      *yPos = *xPos | (temp << 7);
      return penDown;
    } else if (temp == -1) {
      return -1;
    }
  }
  return -1;
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
 * Writes given string onto screen at given position (x, y)
 * Parameters:
 * 			char_buffer:	the screen buffer that will be drawn on
 * 			str:			the null-terminated string to be written
 * 			x:				desired x position of string
 * 			y:				desired y position of string
 * Return:
 * 			1 on success
 * 			0 on failure
 */
alt_u8 write_text(alt_up_video_dma_dev* char_buffer, const char* str, unsigned int x, unsigned int y) {
	return (alt_up_video_dma_draw_string(char_buffer, str, x, y, 0) == 0);
}

/*
 * Draws a given image (2d dimensional array with an array for each pixel that contains [R, G, B]) onto the given screen
 * Parameters:
 * 			pixel_dma_dev:	screen device
 * 			image[][]:		image array
 * 			width:			width of the image
 * 			height:			height of the image
 *
 */
void draw_image(alt_up_video_dma_dev* screen, alt_u8 image[][3], alt_u16 width, alt_u16 height) {
	alt_u16 xStep = 1;
	alt_u16 yStep = 1;
	alt_u16 tempPixel[3];
	alt_u32 color;
	if (width > screen->x_resolution) {
		xStep = width / screen->x_resolution;
	}
	if (height > screen->y_resolution) {
		yStep = height / screen->y_resolution;
	}

	for (alt_u16 y = 0; y*yStep < height; y++) {
		for (alt_u16 x = 0; x*xStep < width; x++) {
			for (alt_u8 i = 0; i < 3; i++) {
				tempPixel[i] = image[y*yStep*width + x*xStep][i];
			}
			color = (tempPixel[0] << 16) | (tempPixel[1] << 8) | tempPixel[2];
			alt_up_video_dma_draw(screen, color,
					x, y, 0);
		}
	}
}

/*
 *  Reads the GPS serial connections and parses the data into simple integers that hold
 *  the latitude, longitude, and time of the reading
 *  Parameters:
 *          gps:  the gps handle
 *          time: variable that will store the time in UTC (hhmmss sss)
 *          latitude: variable that latitude will be stored in (ddmm.mmmm) (negative value means South)
 *          longitude: variable that longitude will be stored in (ddmm.mmmm) (negative value means West)
 * Return:  -1 for read error, 0 otherwise
 * */
//alt_u8 get_location(alt_up_rs232_dev* gps, alt_32* time, alt_32* latitude, alt_32* longitude) {
//  alt_u8 looking = 1;
//  alt_u8 temp;
//  *time = 0;
//  *latitude = 0;
//  *longitude = 0;
//
//  while (looking) {
//    temp = read_char(gps);
//    if (temp == 'G') {  // Read until we get 'GGA'
//      temp = read_char(gps);
//      if (temp == 'G') {
//        temp = read_char(gps);
//        if (temp == 'A') {
//          looking = 0;
//          temp = read_char(gps);
//          while (temp != ',') {  // Read until we reach UTC
//            if (temp == -1) {
//              return -1;
//            }
//            temp = read_char(gps);
//          }
//
//          // Reached UTC
//
//          temp = read_char(gps);
//          if (temp == ',') { // Incase the time is empty
//            return -1;
//          }
//
//          while (temp != '.') { // Until we get to the decimal point
//            if (temp == -1) {
//              return -1;
//            }
//            *time *= 10;
//            *time += (temp - '0');
//            temp = read_char(gps);
//          }
//
//          temp = read_char(gps);
//          while (temp != ',') { // Until we reach Latitude
//            if (temp == -1) {
//              return -1;
//            }
//            *time *= 10;
//            *time += (temp - '0');
//            temp = read_char(gps);
//          }
//
//          // Reached Latitude
//
//          temp = read_char(gps);
//          if (temp == ',') { // Incase the latitude is empty
//            return -1;
//          }
//
//          while (temp != '.') { // Until we get to the decimal point
//            if (temp == -1) {
//              return -1;
//            }
//            *latitude *= 10;
//            *latitude += (temp - '0');
//            temp = read_char(gps);
//          }
//
//          temp = read_char(gps);
//          while (temp != ',') { // Until we reach NS
//            if (temp == -1) {
//              return -1;
//            }
//            *latitude *= 10;
//            *latitude += (temp - '0');
//            temp = read_char(gps);
//          }
//
//          // Reached NS
//
//          temp = read_char(gps);
//          if (temp == ',') { // Incase the NS is empty
//            return -1;
//          }
//
//          if (temp == 'S') {
//            *latitude *= -1;  // Negative means southern latitude
//          }
//
//          temp = read_char(gps); // Should always be ','
//          if (temp == -1) {
//            return -1;
//          }
//
//          // Reached Longitude
//
//          temp = read_char(gps);
//          while (temp != '.') { // Until we get to the decimal point
//            if (temp == -1) {
//              return -1;
//            }
//            *longitude *= 10;
//            *longitude += (temp - '0');
//            temp = read_char(gps);
//          }
//
//          temp = read_char(gps);
//          while (temp != ',') { // Until we reach EW
//            if (temp == -1) {
//              return -1;
//            }
//            *longitude *= 10;
//            *longitude += (temp - '0');
//            temp = read_char(gps);
//          }
//
//          // Reached EW
//
//          temp = read_char(gps);
//          if (temp == ',') { // Incase the EW is empty
//            return -1;
//          }
//
//          if (temp == 'W') {
//            *longitude *= -1;  // Negative means western longitude
//          }
//        }
//      }
//    } else if (temp == -1) {
//      return -1;
//    }
//  }
//  return 0;
//}

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
