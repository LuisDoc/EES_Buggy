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
	#define MPU6050_RA_GYRO_CONFIG		0x1B //Register for Gyroscope Configuration
	//Gyroscope Precision Range
	#define MPU6050_GYRO_FS_250         0x00 //Highest Precision
	#define MPU6050_GYRO_FS_500         0x01
	#define MPU6050_GYRO_FS_1000        0x02
	#define MPU6050_GYRO_FS_2000        0x03 //Lowest Precision
	//Gyroscope Offset Registers
	#define MPU6050_RA_XG_OFFS_TC       0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
	#define MPU6050_RA_YG_OFFS_TC       0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
	#define MPU6050_RA_ZG_OFFS_TC       0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
	//Gyroscope Output Registers
	#define MPU6050_RA_GYRO_XOUT_H      0x43 //Output Register for X's High Bits
	#define MPU6050_RA_GYRO_XOUT_L      0x44 //Output Register for X's Low Bits
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

	extern int16_t x_off; //Offset Value for X 
	extern int16_t y_off; //Offset Value for Y
	extern int16_t z_off; //Offset Value for Z

	extern uint8_t gyroPrecision;
	
	extern int currentTime;
	extern int previousTime;

	//Public Access to current angles
	extern double angle_x;
	extern double angle_y;
	extern double angle_z;
	
	//Methoden
	int init(); //Initialise MPU6050 for Gyroscope Usage
	void calibrateOffsets(int i2c); //Calculate offsets
	int* getMotions(int i2c); //Calculate current angles - Returns pointer on Array with current angles
	void setClockSource(int i2c, uint8_t source); //Change clock source
	void setGyroRange(int i2c, uint8_t range); //Set Precision of Gyroscope
	void setSleepBit(int i2c, bool enabled); //Activate or deactivate sleep Mode
}
