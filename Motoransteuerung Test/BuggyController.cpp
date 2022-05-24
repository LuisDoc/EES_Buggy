#include "BuggyController.h"

/*
* Basis Konstruktor
* Initialisiert die beiden Motoren M1 und M4
*/
BuggyController::BuggyController() {
	//Init MotorhatDriver
	this->i2c = MotorhatDriver::init();

	if (!i2c) {
		system("echo I2C Device not initialized.");
		return;
	}
	//Init Motoren M1 und M4
	MotorhatDriver::initMotor(this->i2c, M1);
	MotorhatDriver::initMotor(this->i2c, M4);
}

void BuggyController::turnLeftFWD(int mili) {
	//Configure Wheel Speed
	MotorhatDriver::setSpeed(i2c, M1, 100);
	MotorhatDriver::setSpeed(i2c, M4, 255);
	//Activate Forward Drive
	MotorhatDriver::runMotor(i2c, M1, MOTOR_FORWARD);
	MotorhatDriver::runMotor(i2c, M4, MOTOR_FORWARD);
	//Delay
	delay(mili);
	//Stop
	driveRelease();
}
void BuggyController::turnRightFWD(int mili) {
	//Configure Wheel Speed
	MotorhatDriver::setSpeed(i2c, M1, 255);
	MotorhatDriver::setSpeed(i2c, M4, 100);
	//Activate Forward Drive
	MotorhatDriver::runMotor(i2c, M1, MOTOR_FORWARD);
	MotorhatDriver::runMotor(i2c, M4, MOTOR_FORWARD);
	//Delay
	delay(mili);
	//Stop
	driveRelease();
}
void BuggyController::driveForward(int mili){
	//Activate Forward Drive
	MotorhatDriver::runMotor(i2c, M1, MOTOR_FORWARD);
	MotorhatDriver::runMotor(i2c, M4, MOTOR_FORWARD);
	//Anfahren
	for (int i = 0; i < 10; i++) {
		MotorhatDriver::setSpeed(i2c, M1, 2 * i);
		MotorhatDriver::setSpeed(i2c, M4, 2 * i);
	}
	//Configure Wheel Speed
	MotorhatDriver::setSpeed(i2c, M1, 220);
	MotorhatDriver::setSpeed(i2c, M4, 255);
	
	//Delay
	delay(mili);
	//Stop
	driveRelease();
}
void BuggyController::driveReverse(int mili) {
	//Configure Wheel Speed
	MotorhatDriver::setSpeed(i2c, M1, 220);
	MotorhatDriver::setSpeed(i2c, M4, 255);
	//Activate Forward Drive
	MotorhatDriver::runMotor(i2c, M1, MOTOR_BACK);
	MotorhatDriver::runMotor(i2c, M4, MOTOR_BACK);
	//Delay
	delay(mili);
	//Stop
	driveRelease();
}
void BuggyController::driveRelease() {
	//Stop Buggy
	MotorhatDriver::runMotor(i2c, M1, MOTOR_RELEASE);
	MotorhatDriver::runMotor(i2c, M4, MOTOR_RELEASE);
	//Set Speed to zero
	MotorhatDriver::setSpeed(i2c, M1, 0);
	MotorhatDriver::setSpeed(i2c, M4, 0);
}
