#include "MPU6050.h"

//Globale Variable
int16_t MPU6050::x_off;
int16_t MPU6050::y_off;
int16_t MPU6050::z_off;
uint8_t MPU6050::gyroPrecision;

int MPU6050::currentTime;
int MPU6050::previousTime;

double MPU6050::angle_x;
double MPU6050::angle_y;
double MPU6050::angle_z;


//Initialise MPU6050 for Gyroscope Usage
int MPU6050::init() {
	
	//Initialise Variables

	MPU6050::currentTime = micros();
	MPU6050::previousTime = currentTime;
	MPU6050::gyroPrecision = 0; 

	MPU6050::angle_x = 0;
	MPU6050::angle_y = 0;
	MPU6050::angle_z = 0;


	//Setup I2C device
	int i2c = wiringPiI2CSetup(MPU6050_I2CAdress);

	//Reset
	wiringPiI2CWriteReg8(i2c, MPU6050_RA_PWR_MGMT_1, 0x00);
	//Clock Source
	MPU6050::setClockSource(i2c, MPU6050_CLOCK_PLL_XGYRO);
	//FullScale Gyro Range
	MPU6050::gyroPrecision = MPU6050_GYRO_FS_250;
	MPU6050::setGyroRange(i2c, MPU6050::gyroPrecision);
	
	//Set Sleep Enabled auf false
	MPU6050::setSleepBit(i2c, false);
	//Calibrate Offsets
	calibrateOffsets(i2c);


	return i2c;
}

//
//	Data Read Methods
//

//Calculate current Angles -> Save in global Variables and Return in Array
int* MPU6050::getMotions(int i2c) {
	
	int result[3] = { 0 };

	MPU6050::previousTime = MPU6050::currentTime;
	MPU6050::currentTime = micros(); //Aktuelle Zeit in Mikrosekunden
	double elapsedTime = 0;
	
	//Determine which lsb_sensitivity is needed
	double lsb_sensitivity = 1;
	switch (gyroPrecision) {
	case MPU6050_GYRO_FS_250: lsb_sensitivity = 131.0;  break;
	case MPU6050_GYRO_FS_500: lsb_sensitivity = 65.5;  break;
	case MPU6050_GYRO_FS_1000: lsb_sensitivity = 32.8; break;
	case MPU6050_GYRO_FS_2000: lsb_sensitivity = 16.4;  break;
	default: 1;
	}

	int16_t x_m = 0, y_m = 0, z_m = 0;
	//Daten auslesen mit I2C vom Register MPU6050_RA_ACCEL_XOUT_H ==> X Werte
	// X-Achse
	int x_h = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_XOUT_H);
	int x_l = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_XOUT_L);
	x_m = (x_h << 8 | x_l) - MPU6050::x_off;
	// Y-Achse
	int y_h = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_YOUT_H);
	int y_l = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_YOUT_L);
	y_m = (y_h << 8 | y_l) - MPU6050::y_off;
	// Z-Achse
	int z_h = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_ZOUT_H);
	int z_l = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_ZOUT_L);
	z_m = (z_h << 8 | z_l) - MPU6050::z_off;

	elapsedTime = (double)((double)MPU6050::currentTime - (double)MPU6050::previousTime) / 1000000;

	//Calculate Angles and Save in global variables
	MPU6050::angle_x = MPU6050::angle_x + ((x_m / lsb_sensitivity)) * elapsedTime;
	MPU6050::angle_y = MPU6050::angle_y + ((y_m / lsb_sensitivity)) * elapsedTime;
	MPU6050::angle_z = MPU6050::angle_z + ((z_m / lsb_sensitivity)) * elapsedTime;
	//Return Angles in Array
	result[0] = MPU6050::angle_x;
	result[1] = MPU6050::angle_y;
	result[2] = MPU6050::angle_z;
	return result;
}

//
//	Configuration Methods
// 

//Configure Clock
void MPU6050::setClockSource(int i2c, uint8_t source) {
	//Aktuellen Registerwert lesen
	int oldmode = wiringPiI2CReadReg8(i2c, MPU6050_RA_PWR_MGMT_1);
	//Unteren 3-Bits anpassen für Clk Select
	int newmode = oldmode;
	newmode &= ~0b111; //Clock Select Bits löschen
	newmode |= source;
	//Gesamten Registerwert schreiben
	wiringPiI2CWriteReg8(i2c, MPU6050_RA_PWR_MGMT_1, newmode);
}

//Configure Offets
void MPU6050::calibrateOffsets(int i2c) {
	int n = 40;
	//Calculate best offset values
	int x_sum = 0, y_sum = 0, z_sum = 0;
	int16_t x_val = 0, y_val = 0, z_val = 0;
	for (int i = 0; i < n; i++) {
		int16_t x_m = 0, y_m = 0, z_m = 0;
		//Daten auslesen mit I2C vom Register MPU6050_RA_ACCEL_XOUT_H ==> X Werte
		// X-Achse
		signed int x_h = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_XOUT_H);
		signed int x_l = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_XOUT_L);
		x_m = (x_h << 8 | x_l);
		// Y-Achse
		int y_h = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_YOUT_H);
		int y_l = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_YOUT_L);
		y_m = (y_h << 8 | y_l);
		// Z-Achse
		int z_h = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_ZOUT_H);
		int z_l = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_ZOUT_L);
		z_m = (z_h << 8 | z_l);
		//int16_t Variablen werden benötigt für korrekte Berechnung von signed Ergebnissen
		x_sum += x_m;
		y_sum += y_m;
		z_sum += z_m;
	}

	//Set Offset Values
	MPU6050::x_off = x_sum / n;
	MPU6050::y_off = y_sum / n;
	MPU6050::z_off = z_sum / n;
}

//Gyroscope Precision Range
void MPU6050::setGyroRange(int i2c, uint8_t range){
	//Read Current Settings
	int oldmode = wiringPiI2CReadReg8(i2c, MPU6050_RA_GYRO_CONFIG);
	int newmode = oldmode;
	//Changes only on Bit 4 and 3
	newmode &= ~(11000); //Reset Bits
	//Set new FS Configuration
	newmode |= range << 3;
	//Write to register
	wiringPiI2CWriteReg8(i2c, MPU6050_RA_GYRO_CONFIG, newmode);
}

//Activate or Deactivate Sleep mode
void MPU6050::setSleepBit(int i2c, bool enabled){
	//Read Current Settings
	int oldmode = wiringPiI2CReadReg8(i2c, MPU6050_RA_PWR_MGMT_1);
	int newmode = oldmode;
	//Reset Bit
	newmode &= ~(1 << 6);
	//If enable, then set sleep bit 1, else set sleep bit 0
	enabled ? newmode |= (1 << 6) : newmode &= ~(1 << 6);
}

