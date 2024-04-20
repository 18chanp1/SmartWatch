#include <stdio.h>
#include <system.h>
#include <i2c_opencores_regs.h>
#include <i2c_opencores.h>
#include <altera_avalon_timer_regs.h>
#include <altera_avalon_timer.h>
#include <priv/alt_legacy_irq.h>
#include "sys/alt_sys_wrappers.h"
#include <sys/alt_irq.h>
#include <math.h>

float g = 4096 / 9.81;


float xAccel = 0;
float yAccel = 0;
float zAccel = 0;


float previousVector = 0;
float currentVector = 0;


alt_16 _xAccel = 0;
alt_16 _yAccel = 0;
alt_16 _zAccel = 0;

alt_u8 above = 0;
alt_32 count = 0;
alt_32 steps = 0;
static void timer_isr(void * context, alt_u32 id);

void init_timer_interrupt( void );

int main()
{
  printf("Testing Accelerometer\n");

  I2C_init(I2C_OPENCORES_0_BASE, ALT_CPU_FREQ, 400*1000);

   printf("Worked?: %lu\n", I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0));
   printf("Worked?: %lu\n", I2C_write(I2C_OPENCORES_0_BASE, 0x75, 0));
   printf("Worked?: %lu\n", I2C_start(I2C_OPENCORES_0_BASE, 0x68, 1));

   alt_u32 name = I2C_read(I2C_OPENCORES_0_BASE, 1);

   if (name == 0x68) {
	   printf("NAME IS RIGHT\n");
	   init_timer_interrupt();

	   while (1) {
		   ALT_USLEEP(10000);
		   alt_irq_disable(MPU_TIMER_IRQ);
		   printf("STEPS: %d\n", steps);
		   alt_irq_enable(MPU_TIMER_IRQ);
	   }
   }
  return 0;
}

static void timer_isr( void * context, alt_u32 id ) {
	IOWR_ALTERA_AVALON_TIMER_STATUS(MPU_TIMER_BASE, 0);
	I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0);
	I2C_write(I2C_OPENCORES_0_BASE, 0x3B, 0);
	I2C_start(I2C_OPENCORES_0_BASE, 0x68, 1);

	_xAccel = (I2C_read(I2C_OPENCORES_0_BASE, 0)<< 8 | I2C_read(I2C_OPENCORES_0_BASE, 0));
	_yAccel = (I2C_read(I2C_OPENCORES_0_BASE, 0)<< 8 | I2C_read(I2C_OPENCORES_0_BASE, 0));
	_zAccel = (I2C_read(I2C_OPENCORES_0_BASE, 0)<< 8 | I2C_read(I2C_OPENCORES_0_BASE, 1));

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

	if (currentVector - previousVector > 1000 && !above) {
		above++;
		steps++;
	}
	if (above) {
		if (count < 20) {
			count++;
		} else {
			count = 0;
			above = 0;
		}
	}
}

void init_timer_interrupt( void ){


   I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0);
   I2C_write(I2C_OPENCORES_0_BASE, 0x6B, 0);
   I2C_write(I2C_OPENCORES_0_BASE, 0x0, 1);


   I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0);
   I2C_write(I2C_OPENCORES_0_BASE, 0x1C, 0);
   I2C_write(I2C_OPENCORES_0_BASE, 0x18, 1);



   I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0);
   I2C_write(I2C_OPENCORES_0_BASE, 0x1B, 0);
   I2C_write(I2C_OPENCORES_0_BASE, 0x0, 1);

    alt_irq_register(MPU_TIMER_IRQ, 0x0, (void*) timer_isr);
    IOWR_ALTERA_AVALON_TIMER_CONTROL(MPU_TIMER_BASE, ALTERA_AVALON_TIMER_CONTROL_CONT_MSK
                                               | ALTERA_AVALON_TIMER_CONTROL_START_MSK
                                               | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK);
}
