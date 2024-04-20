#include <stdio.h>
#include <system.h>
#include <i2c_opencores_regs.h>
#include <i2c_opencores.h>
#include "sys/alt_sys_wrappers.h"

int main()
{
  printf("Nios is awake!\n");

  printf("Testing Accelerometer\n");

  I2C_init(I2C_OPENCORES_0_BASE, ALT_CPU_FREQ, 400*1000);

   printf("Worked?: %d\n", I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0));

   printf("Worked?: %lu\n", I2C_write(I2C_OPENCORES_0_BASE, 0x75, 0));

   printf("Worked?: %d\n", I2C_start(I2C_OPENCORES_0_BASE, 0x68, 1));

   alt_u32 name = I2C_read(I2C_OPENCORES_0_BASE, 1);

   if (name == 0x68) {
	   printf("Name is correct - %x\n", name);
	   alt_u32 accel;
	   alt_u32 test;
	   alt_u32 working;

	   I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0);
	   I2C_write(I2C_OPENCORES_0_BASE, 0x6B, 0);
	   I2C_write(I2C_OPENCORES_0_BASE, 0x0, 1);


	   I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0);
	   I2C_write(I2C_OPENCORES_0_BASE, 0x1C, 0);
	   I2C_write(I2C_OPENCORES_0_BASE, 0x10, 1);


	   working = I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0);
	   if (working) {
	   	   		   printf("NOT WORKING\n");
	   	   	   }
	   working = I2C_write(I2C_OPENCORES_0_BASE, 0x1C, 0);
	   if (working) {
	   	   		   printf("NOT WORKING\n");
	   	   	   }
	   working = I2C_start(I2C_OPENCORES_0_BASE, 0x68, 1);
	   printf("TESTING: ACCEL CONFIG = %x\n", I2C_read(I2C_OPENCORES_0_BASE, 1));


	   for (alt_u32 i = 0x0D; i <= 0x75; i++) {
		   I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0);
		   I2C_write(I2C_OPENCORES_0_BASE, i, 0);
		   I2C_start(I2C_OPENCORES_0_BASE, 0x68, 1);
		   test = I2C_read(I2C_OPENCORES_0_BASE, 1);

		   printf("Register #%x is %x\n", i, test);
	   }

	   while (1) {
		   ALT_USLEEP(100000);

		   I2C_start(I2C_OPENCORES_0_BASE, 0x68, 0);
		   I2C_write(I2C_OPENCORES_0_BASE, 0x3B, 0);
		   I2C_start(I2C_OPENCORES_0_BASE, 0x68, 1);

		   printf("Accel X is %x\n", (I2C_read(I2C_OPENCORES_0_BASE, 0)<< 8 | I2C_read(I2C_OPENCORES_0_BASE, 0)));
	   }
   }
  return 0;
}
