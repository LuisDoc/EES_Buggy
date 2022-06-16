#pragma once
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <cstdint>
#include <iostream>

/*
	1. Offsets einstellen

*/

namespace MPU6050 {
	#define INT_MIN -32768
	#define INT_MAX 32767


	//I2C Address
	#define MPU6050_I2CAdress			0x68
	//Configure Register
	#define MPU6050_RA_PWR_MGMT_1		0x6B
	#define MPU6050_RA_GYRO_CONFIG		0x1B
	#define MPU6050_RA_ACCEL_CONFIG		0x1C
	//Gyroscope Precision Range
	#define MPU6050_GYRO_FS_250         0x00
	#define MPU6050_GYRO_FS_500         0x01
	#define MPU6050_GYRO_FS_1000        0x02
	#define MPU6050_GYRO_FS_2000        0x03
	//Gyroscope Offset Registers
	#define MPU6050_RA_XG_OFFS_TC       0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
	#define MPU6050_RA_YG_OFFS_TC       0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
	#define MPU6050_RA_ZG_OFFS_TC       0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
	//Acceleration Precision Range
	#define MPU6050_ACCEL_FS_2          0x00
	#define MPU6050_ACCEL_FS_4          0x01
	#define MPU6050_ACCEL_FS_8          0x02
	#define MPU6050_ACCEL_FS_16         0x03

	//Output Register
	//Acceleration
	#define MPU6050_RA_ACCEL_XOUT_H     0x3B
	#define MPU6050_RA_ACCEL_XOUT_L     0x3C
	#define MPU6050_RA_ACCEL_YOUT_H     0x3D
	#define MPU6050_RA_ACCEL_YOUT_L     0x3E
	#define MPU6050_RA_ACCEL_ZOUT_H     0x3F
	#define MPU6050_RA_ACCEL_ZOUT_L     0x40
	//Temperature
	#define MPU6050_RA_TEMP_OUT_H       0x41
	#define MPU6050_RA_TEMP_OUT_L       0x42
	//Gyro	
	#define MPU6050_RA_GYRO_XOUT_H      0x43
	#define MPU6050_RA_GYRO_XOUT_L      0x44
	#define MPU6050_RA_GYRO_YOUT_H      0x45
	#define MPU6050_RA_GYRO_YOUT_L      0x46
	#define MPU6050_RA_GYRO_ZOUT_H      0x47
	#define MPU6050_RA_GYRO_ZOUT_L      0x48
	//Clock Sources
	#define MPU6050_CLOCK_INTERNAL          0x00
	#define MPU6050_CLOCK_PLL_XGYRO         0x01
	#define MPU6050_CLOCK_PLL_YGYRO         0x02
	#define MPU6050_CLOCK_PLL_ZGYRO         0x03
	#define MPU6050_CLOCK_PLL_EXT32K        0x04 
	#define MPU6050_CLOCK_PLL_EXT19M        0x05
	#define MPU6050_CLOCK_KEEP_RESET        0x07
	
	//Variablen
	extern int16_t x_off;
	extern int16_t y_off;
	extern int16_t z_off;

	extern uint8_t gyroPrecision;
	
	extern int currentTime;
	extern int previousTime;

	extern double angle_x;
	extern double angle_y;
	extern double angle_z;
	
	//Methoden
	int init();
	void calibrateOffsets(int i2c);
	void getMotions(int i2c);
	void setClockSource(int i2c, uint8_t source);
	void setGyroRange(int i2c, uint8_t range);
	void setGyroOffsets(int i2c, int16_t x_off, int16_t y_off, int16_t z_off);
	void setAccelRange(int i2c, uint8_t range);
	void setSleepBit(int i2c, bool enabled);
	void integrate(int i2c);
}
