#include "sys/alt_stdio.h"
#include "altera_up_avalon_rs232.h"
#include "altera_up_avalon_rs232_regs.h"
#include "sys/alt_sys_wrappers.h"
#include <malloc.h>
#include <sys/times.h>
#include <system.h>

#define RLEDs ((volatile long *) 0xFF200000)
#define READ_FIFO_EMPTY 0x0

alt_up_rs232_dev* init_gps();
alt_up_rs232_dev* init_touchscreen();

alt_u8 get_touch(alt_up_rs232_dev* screen, alt_u16* xPos, alt_u16* yPos);

alt_u8 get_location(alt_up_rs232_dev* gps, alt_32* time, alt_32* latitude, alt_32* longitude);

alt_u8 can_read_device(alt_up_rs232_dev* dev);
alt_u8 read_char(alt_up_rs232_dev* dev);
alt_u8 read_device(alt_up_rs232_dev* dev, alt_u8* buffer, alt_u16 buflen);



int main()
{ 
  alt_putstr("Hello from Nios II!\n");
  *RLEDs = 0x0ff;

  alt_up_rs232_dev* gps_dev = init_gps();
  alt_up_rs232_dev* touchscreen_dev = init_touchscreen();
  
  alt_32 time;
  alt_32 latitude;
  alt_32 longitude;
  alt_u16 x;
  alt_u16 y;
  alt_u8 down;


  while (1) 	{
	  if (can_read_device(gps_dev)) {
      get_location(gps_dev, &time, &latitude, &longitude);
      alt_printf("Time: %x, Lat: %x, Long: %x\n", time, latitude, longitude);
    }
  ALT_USLEEP(1000);
    if (can_read_device(touchscreen_dev)) {
      down = get_touch(touchscreen_dev, &x, &y);
      if (down < 0) {
        alt_printf("Don't see a touch!\n");
      } else {
        alt_printf("X: %x, Y: %x, and DOWN: %x\n", x, y, down);
      }
    }
  }

  return 0; 
}

/*
 *  Initializes the GPS serial connection
 *  Return: the device handle
 */
alt_up_rs232_dev* init_gps() {
  alt_up_rs232_dev* gps = alt_up_rs232_open_dev(RS232_0_NAME);
  if (gps == NULL) {
    alt_printf("Error: could not open GPS UART\n");
  } else {
    alt_printf("Opened GPS UART device\n");
    alt_up_rs232_enable_read_interrupt(gps);
  }
  return gps;
}

/*
 *  Initializes the Touchscreen serial connection, and sends a 'touch_enable' command to it
 *  Return: the device handle
 */
alt_up_rs232_dev* init_touchscreen() {
  alt_up_rs232_dev* touchscreen = alt_up_rs232_open_dev(RS232_1_NAME);
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
 *  Reads the GPS serial connections and parses the data into simple integers that hold
 *  the latitude, longitude, and time of the reading
 *  Parameters:
 *          gps:  the gps handle
 *          time: variable that will store the time in UTC (hhmmss sss)
 *          latitude: variable that latitude will be stored in (ddmm.mmmm) (negative value means South)
 *          longitude: variable that longitude will be stored in (ddmm.mmmm) (negative value means West)
 * Return:  -1 for read error, 0 otherwise
 * */
alt_u8 get_location(alt_up_rs232_dev* gps, alt_32* time, alt_32* latitude, alt_32* longitude) {
  alt_u8 looking = 1;
  alt_u8 temp;
  *time = 0;
  *latitude = 0;
  *longitude = 0;
  
  while (looking) {
    temp = read_char(gps);
    if (temp == 'G') {  // Read until we get 'GGA'
      temp = read_char(gps);
      if (temp == 'G') {
        temp = read_char(gps);
        if (temp == 'A') {
          looking = 0;
          temp = read_char(gps);
          while (temp != ',') {  // Read until we reach UTC
            if (temp == -1) {
              return -1;
            }
            temp = read_char(gps);
          }

          // Reached UTC
          
          temp = read_char(gps);
          if (temp == ',') { // Incase the time is empty
            return -1;
          }

          while (temp != '.') { // Until we get to the decimal point
            if (temp == -1) {
              return -1;
            }
            *time *= 10;
            *time += (temp - '0');
            temp = read_char(gps);
          }
            
          temp = read_char(gps);
          while (temp != ',') { // Until we reach Latitude
            if (temp == -1) {
              return -1;
            }
            *time *= 10;
            *time += (temp - '0');
            temp = read_char(gps);
          }

          // Reached Latitude

          temp = read_char(gps);
          if (temp == ',') { // Incase the latitude is empty
            return -1;
          }

          while (temp != '.') { // Until we get to the decimal point
            if (temp == -1) {
              return -1;
            }
            *latitude *= 10;
            *latitude += (temp - '0');
            temp = read_char(gps);
          }
            
          temp = read_char(gps);
          while (temp != ',') { // Until we reach NS
            if (temp == -1) {
              return -1;
            }
            *latitude *= 10;
            *latitude += (temp - '0');
            temp = read_char(gps);
          }

          // Reached NS

          temp = read_char(gps);
          if (temp == ',') { // Incase the NS is empty
            return -1;
          }

          if (temp == 'S') {
            *latitude *= -1;  // Negative means southern latitude
          }

          temp = read_char(gps); // Should always be ','
          if (temp == -1) {
            return -1;
          }

          // Reached Longitude

          temp = read_char(gps);
          while (temp != '.') { // Until we get to the decimal point
            if (temp == -1) {
              return -1;
            }
            *longitude *= 10;
            *longitude += (temp - '0');
            temp = read_char(gps);
          }
            
          temp = read_char(gps);
          while (temp != ',') { // Until we reach EW
            if (temp == -1) {
              return -1;
            }
            *longitude *= 10;
            *longitude += (temp - '0');
            temp = read_char(gps);
          }

          // Reached EW

          temp = read_char(gps);
          if (temp == ',') { // Incase the EW is empty
            return -1;
          }

          if (temp == 'W') {
            *longitude *= -1;  // Negative means western longitude
          }
        }
      }
    } else if (temp == -1) {
      return -1;
    }
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
