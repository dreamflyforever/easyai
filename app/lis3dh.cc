#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <pthread.h>

#define I2C_DEVICE "/dev/i2c-3" //
#define LIS3DHTR_ADDR 0x19      
#define WHO_AM_I     0x0F       // ID reg
#define CTRL_REG1    0x20       
#define OUT_X_L      0x28       // X_L
#define OUT_X_H      0x29       // X_H
#define OUT_Y_L      0x2A       // Y L
#define OUT_Y_H      0x2B       // Y H
#define OUT_Z_L      0x2C       // Z L
#define OUT_Z_H      0x2D       // Z H
int g_is_run;
void i2c_write(int file, uint8_t reg, uint8_t data) {
	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = data;
	if (write(file, buf, 2) != 2) {
		perror("Failed to write to the i2c bus");
	}
}

uint8_t i2c_read_byte(int file, uint8_t reg) {
	uint8_t buf[1];
	if (write(file, &reg, 1) != 1) {
		perror("Failed to write to the i2c bus");
	}
	if (read(file, buf, 1) != 1) {
		perror("Failed to read from the i2c bus");
	}
	return buf[0];
}

void i2c_read_bytes(int file, uint8_t reg, uint8_t *data, size_t length) {
	if (write(file, &reg, 1) != 1) {
		perror("Failed to write to the i2c bus");
	}
	if (read(file, data, length) != length) {
		perror("Failed to read from the i2c bus");
	}
}

void * lis3hd_main(void * arg)
{
	// open i2c device
	int i2c_file = open(I2C_DEVICE, O_RDWR);
	if (i2c_file < 0) {
		perror("Failed to open the i2c bus");
		exit(1);
	}

	// set i2c device
	if (ioctl(i2c_file, I2C_SLAVE, LIS3DHTR_ADDR) < 0) {
		perror("Failed to acquire bus access and/or talk to slave");
		close(i2c_file);
		exit(1);
	}

	// check, ID Reg
	uint8_t who_am_i = i2c_read_byte(i2c_file, WHO_AM_I);
	if (who_am_i != 0x33) {
		fprintf(stderr, "Failed to connect to LIS3DHTR: WHO_AM_I = 0x%02X\n", who_am_i);
		close(i2c_file);
		exit(1);
	}

	// init ctrl reg1, x,y,z 100Hz, 0x57 = 0b01010111
	i2c_write(i2c_file, CTRL_REG1, 0x57); 
	int sum;
	while(1) {

		uint8_t accel_data[6];
		i2c_read_bytes(i2c_file, OUT_X_L | 0x80, accel_data, 6);

		// H << 8 | L
		int16_t accel_x = (accel_data[1] << 8) | accel_data[0];
		int16_t accel_y = (accel_data[3] << 8) | accel_data[2];
		int16_t accel_z = (accel_data[5] << 8) | accel_data[4];

		// norm to g
		float accel_x_g = accel_x / 16384.0;
		float accel_y_g = accel_y / 16384.0;
		float accel_z_g = accel_z / 16384.0;

		sleep(1);
		sum = abs(accel_x ) + abs(accel_y ) + abs(accel_z);
		(sum >= 0.75)? g_is_run = 1: g_is_run = 0;
		printf("accelerometer data: X=%.2f g, Y=%.2f g, Z=%.2f g, sum: %d\n", accel_x_g, accel_y_g, accel_z_g, sum);

	}

	close(i2c_file);
	return 0;
}

int get_device_status()
{
	return g_is_run;
}

int is_device_run()
{
	pthread_t run_task;
	g_is_run = 0;
	int retval = pthread_create(&run_task, NULL, lis3hd_main, NULL);
	return 0;
}
