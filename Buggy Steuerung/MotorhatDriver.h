#pragma once
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <softPwm.h>
#include <stdlib.h>

namespace MotorhatDriver {
	#define ADAFRUIT_MOTORHAT       0x60

	//PINS
	#define PWM_M1_PWM      8
	#define PWM_M1_IN2      9
	#define PWM_M1_IN1      10
	#define PWM_M2_PWM      13
	#define PWM_M2_IN2      12
	#define PWM_M2_IN1      11
	#define PWM_M3_PWM      2
	#define PWM_M3_IN2      3
	#define PWM_M3_IN1      4
	#define PWM_M4_PWM      7
	#define PWM_M4_IN2      6
	#define PWM_M4_IN1      5

	#define PWM_FREQUENCY   1600.0
	#define PWM_PRESCALE    0xFE

	#define PWM_MODE1       0x00
	#define PWM_MODE2       0x01
	#define PWM_LED0_ON_L   0x06
	#define PWM_LED0_ON_H   0x07
	#define PWM_LED0_OFF_L  0x08
	#define PWM_LED0_OFF_H  0x09

	#define PWM_RESTART     0x80
	#define PWM_SLEEP       0x10
	#define PWM_ALLCALL     0x01
	#define PWM_INVRT       0x10
	#define PWM_OUTDRV      0x04

	#define PWM_ALL_LED_ON_L        0xFA
	#define PWM_ALL_LED_ON_H        0xFB
	#define PWM_ALL_LED_OFF_L       0xFC
	#define PWM_ALL_LED_OFF_H       0xFD

	#define MOTOR_FORWARD   1
	#define MOTOR_BACK      2
	#define MOTOR_BRAKE     3
	#define MOTOR_RELEASE   4

	/*
		Funktionen
	*/

	void setAllPWM(int i2c, int on, int off); //Write value son and off to registers for all pins
	void setPWM(int i2c, int pin, int on, int off); //Write values on and off to registers for specific pin
	void setPin(int i2c, int pin, int value); //Set Value to Pin ==> Used for Direction Pick
	void runMotor(int i2c, int motor, int command);  //Pick Motor direction (forwards/backwards/release)
	void setSpeed(int i2c, int motor, int speed); //Set Motor speed
	int init(); //Init Driver
	void initMotor(int i2c, int motor); //Init Motor
}
