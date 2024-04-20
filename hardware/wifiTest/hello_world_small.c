/* 
 * "Small Hello World" example. 
 * 
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example 
 * designs. It requires a STDOUT  device in your system's hardware. 
 *
 * The purpose of this example is to demonstrate the smallest possible Hello 
 * World application, using the Nios II HAL library.  The memory footprint
 * of this hosted application is ~332 bytes by default using the standard 
 * reference design.  For a more fully featured Hello World application
 * example, see the example titled "Hello World".
 *
 * The memory footprint of this example has been reduced by making the
 * following changes to the normal "Hello World" example.
 * Check in the Nios II Software Developers Manual for a more complete 
 * description.
 * 
 * In the SW Application project (small_hello_world):
 *
 *  - In the C/C++ Build page
 * 
 *    - Set the Optimization Level to -Os
 * 
 * In System Library project (small_hello_world_syslib):
 *  - In the C/C++ Build page
 * 
 *    - Set the Optimization Level to -Os
 * 
 *    - Define the preprocessor option ALT_NO_INSTRUCTION_EMULATION 
 *      This removes software exception handling, which means that you cannot 
 *      run code compiled for Nios II cpu with a hardware multiplier on a core 
 *      without a the multiply unit. Check the Nios II Software Developers 
 *      Manual for more details.
 *
 *  - In the System Library page:
 *    - Set Periodic system timer and Timestamp timer to none
 *      This prevents the automatic inclusion of the timer driver.
 *
 *    - Set Max file descriptors to 4
 *      This reduces the size of the file handle pool.
 *
 *    - Check Main function does not exit
 *    - Uncheck Clean exit (flush buffers)
 *      This removes the unneeded call to exit when main returns, since it
 *      won't.
 *
 *    - Check Don't use C++
 *      This builds without the C++ support code.
 *
 *    - Check Small C library
 *      This uses a reduced functionality C library, which lacks  
 *      support for buffering, file IO, floating point and getch(), etc. 
 *      Check the Nios II Software Developers Manual for a complete list.
 *
 *    - Check Reduced device drivers
 *      This uses reduced functionality drivers if they're available. For the
 *      standard design this means you get polled UART and JTAG UART drivers,
 *      no support for the LCD driver and you lose the ability to program 
 *      CFI compliant flash devices.
 *
 *    - Check Access device drivers directly
 *      This bypasses the device file system to access device drivers directly.
 *      This eliminates the space required for the device file system services.
 *      It also provides a HAL version of libc services that access the drivers
 *      directly, further reducing space. Only a limited number of libc
 *      functions are available in this configuration.
 *
 *    - Use ALT versions of stdio routines:
 *
 *           Function                  Description
 *        ===============  =====================================
 *        alt_printf       Only supports %s, %x, and %c ( < 1 Kbyte)
 *        alt_putstr       Smaller overhead than puts with direct drivers
 *                         Note this function doesn't add a newline.
 *        alt_putchar      Smaller overhead than putchar with direct drivers
 *        alt_getchar      Smaller overhead than getchar with direct drivers
 *
 */

#include <stdio.h>
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

#define MAX_ROWS 150*120
alt_u8 data[MAX_ROWS];

/*wifi command for testing*/
void wifiCommand(int mode,alt_up_rs232_dev *rs232){
	char* data;
	//mode 1: simple light LED command test
	if(mode == 1)data = "gpio.write(3, gpio.LOW)\n";
	//mode 2: blinking LED test
	else if(mode == 2) data = "dofile(\"script1.lua\")\n";
	//mode 3:  connect wifi test
	else if(mode == 3) data = "dofile(\"test_connectHome.lua\")\n";
	//mode 3: simpleGetrequest test
	else if(mode == 4) data = "dofile(\"test_httpGET_uart40.lua\")\n";
	//mode 4: test uart write
	else if(mode == 5) data = "dofile(\"wifi.lua\")\n";
	//mode 5: upload data
	else if(mode == 6) data = "get_image(\"192.168.1.69\",\"submitserial\",\"0\",\"0\")\n";
	else data = '\0';
	alt_up_rs232_disable_read_interrupt(rs232);
	for(int i=0; i < strlen(data); i++){
		alt_up_rs232_write_data(rs232, data[i]);
		alt_printf("%c",data[i]);
	}
}

/*global variables for timer_isr*/
int state = 1;
int count_t = 0;
int interval = 1000;

/*timer interrupt register for buzzer
 *buzz for 1000 interval and stop for 1000
 */
static void timer_isr( void * context, alt_u32 id ){
	if(state){
		if(count_t >= interval){
			state = 0;
			count_t = 0;
		}
		count_t++;
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, 0x3ff);
	}else{
		if(count_t >= interval){
			state = 1;
			count_t = 0;
		}
		count_t++;
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, 0);
	}
	/* clear interrupt */
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
}


void init_timer_interrupt( void ){
	/* register ISR */
//	alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, (void *)timer_isr, 0x0, 0x0);
	alt_irq_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, 0x0, (void *)timer_isr);

	/* start timer */
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, ALTERA_AVALON_TIMER_CONTROL_CONT_MSK
			                                   | ALTERA_AVALON_TIMER_CONTROL_START_MSK
											   | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK);
}

/* send HTTP post request to server, read response, and parse into image array
 */
void read_image_array(alt_up_rs232_dev *rs232,char* host,char *uri,int longitude,int latitude){
	/* clear buffer */
	char receive;
	alt_u8 parity_error;
	while(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
			alt_up_rs232_read_data(rs232, &receive, &parity_error);
			printf("%c",receive);
	}

	/* send command */
	wifiCommand(5,rs232);
	usleep(1000);
//	char *data= "get_image(\"192.168.137.172\",\"submitserial\",\"0\",\"0\")\n";
	sprintf(data, "get_image(\"%s\",\"%s\",\"%d\",\"%d\")\n", host,uri,longitude,latitude);
	alt_up_rs232_disable_read_interrupt(rs232);
	for(int i=0; i < strlen(data); i++){
		alt_up_rs232_write_data(rs232, data[i]);
		alt_printf("%c",data[i]);
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
			printf("%c",receive);
		}if(receive == '[') {
			start = 1;
			alt_up_rs232_read_data(rs232, &receive, &parity_error); //skip a "\n" after "["
//						str[pos++]=receive;
		}
	}
	while(count < MAX_ROWS){
		if(alt_up_rs232_get_used_space_in_read_FIFO(rs232) > 0){
			alt_up_rs232_read_data(rs232, &receive, &parity_error);
//			printf("%c",receive);
//			if(receive != '\n'){
				count++;
				data[pos++]=(alt_u8)receive;
//				printf("%02x",(alt_u8)receive);
//				printf("%d\n",count);
				if(count%2000==0)printf("%d\n",count);
//				}
		}
//		if(count > 100) printf("%d\n",alt_up_rs232_get_used_space_in_read_FIFO(rs232));
	}
	printf("\n");
	printf("end read array...\n");
}

void wait_for_key_pressed(){
	alt_u8 prev,cur;
	while(1){
		cur = IORD_ALTERA_AVALON_PIO_DATA(KEY_BASE)>>1 & 0x1;
		if(cur != prev && !cur){
			return;
		}
		prev = cur;
	}
	return;
}

void run_mode(alt_up_rs232_dev *wifi_rs232){
	int state = 0;
		while(1){
			/*default state: welcome line, start button*/
			if(state == 0){
				printf("WELCOME!\n");
				printf("Please press KEY1 to start...\n");
				wait_for_key_pressed();
				state++;

			}
			//get-map state: instructions on switches and button to get map
			//            1. select map on switches(binary), display options if applicable
			//            2. press KEY1 to confirm, and send POST request
			//            3. "loading..." line on screen while waiting
			//            4. once map shown, start button press to start run
			//            5. (if applicable) redo button
			else if(state == 1){
				printf("Please select map using SW, and press KEY1 to confirm\n");
				usleep(1000);
				wait_for_key_pressed();
				printf("You have selected id=%d map\n",IORD_ALTERA_AVALON_PIO_DATA(SW_BASE));

				printf("press KEY1 to retrieve map from server..\n");
				usleep(1000);
				wait_for_key_pressed();
				// send request
				read_image_array(wifi_rs232,"192.168.1.69","submitserial",0,0);
				//display image
				printf("Image displayed\n");
				printf("(go to next state)\n");
				state++;

			}
			//running state: display map and data on screen, end button
			//            1. (if applicable) pause and resume button
			else if(state == 2){
				printf("Running mode..\n");
				printf("press KEY1 to end..\n");
				usleep(1000);
				wait_for_key_pressed();
				printf("(go to next state)\n");
				state++;
			}
			//finish state: display congrat line, map and finish data, Restart button
			else if(state == 3){
				// send request :todo
				usleep(1000);
				read_image_array(wifi_rs232,"192.168.1.69","submitserial",0,0);
				printf("Congrat!! You have finished your Run!\n");
				printf("press KEY1 to restart..\n");
				usleep(1000);
				wait_for_key_pressed();
				printf("(go to next state)\n");
				state=0;
			}
		}

}
int main()
{
	alt_putstr("Hello from Nios II!\n");
	/* open device */
	struct alt_up_rs232_dev *wifi_rs232;
	wifi_rs232 = alt_up_rs232_open_dev("/dev/rs232");
	if (wifi_rs232 == NULL)
	{alt_printf("Error: could not open RS232 UART\n");}
	else
	{alt_printf("Opened RS232 UART device\n");}

//	run_mode(wifi_rs232);
//	get_route_input();
	read_image_array(wifi_rs232,"192.168.1.69","submitserial",0,0);
    return 0;
}


