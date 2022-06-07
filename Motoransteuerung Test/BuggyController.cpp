#include "BuggyController.h"

/*
* Basic Constructor
* Initializes Motorhat Motors M1 and M4
* since we are not using M2 and M3
*/
BuggyController::BuggyController() {
	//Inti Vars
	collisionCheckPeriode = ccperiod; //Buggy will check each ccperiod[ms] for collisions
	collisionThreshold = collisionDistanceThreshold; //Buggy will stop, if distance is lower then collisionDistanceThreshold[cm]
	collisionDetected = true; //Buggy not allowed to drive, if value is true
	currentSpeed[2] = { 0 };
	currentDirection[2] = {0};

	//Init MotorhatDriver
	this->i2c = MotorhatDriver::init();

	if (!i2c) {
		system("echo I2C Device not initialized.");
		return;
	}
	//Init Motoren M1 und M4
	MotorhatDriver::initMotor(this->i2c, M1);
	MotorhatDriver::initMotor(this->i2c, M4);

	/*
	* Sensoren
	*/
	//Create Object with Default Settings
	this->UltraSonicSensor = new UltraSonicDriver(usTriggerPin, usEchoPin);
	//Check for Collisions
	this->collisionCheck();
}

/*
* Method uses Ultra Sonic Sensor to check for possible Collision with Objects
* If Danger detected motors will be stopped
*/
bool BuggyController::collisionCheck() {
	double distance = this->UltraSonicSensor->measureDistance();
	//Compare Actual Distance to threshold || If there is no UltraSonic sensor, a collision will be detected to be on the safe side
	if (distance < this->collisionThreshold || !this->UltraSonicSensor) {
		//Print Information to console
		std::cout << "!!!Collision-Warning: " << distance << " !!!" << std::endl;

		//Set Flag true --> Disable Driving Commands while true
		this->collisionDetected = true;
		return true;
	}
	//If there are no collisions detected --> Reset Flag to false
	this->collisionDetected = false;
	return false;
}

/*
* Method implements an delay, with periodic collisionDetection
* collsionCheckPeriode determines the period length of collision_detection in ms
*/
void BuggyController::driveDelay(int milli) {
	int n_perioden = milli / this->collisionCheckPeriode; //n_perioden *  delay(collisionCheckPeriode) = delay(mili) 
	//After each period, the buggy will check for collision
	int periode = 0;
	while (periode++ < n_perioden) {
		//Check for Collisions
		this->collisionCheck();
		//Check if a collision was found --> Stop Driving
		if (this->collisionDetected) {
			//Buggy anhalten
			this->driveRelease();
			//Wait until there is no more Collision
			while (this->collisionDetected) {
				//Check for Collisions
				this->collisionCheck();
				//Wait for collisionCheckPeriode milliseconds
				delay(collisionCheckPeriode);
			}
			//Continue, collision fixed
			this->drive(currentSpeed[0], currentSpeed[1], currentDirection[0], currentDirection[1]);
		}
		//Wait for collisionCheckPeriode milliseconds
		delay(collisionCheckPeriode);
	}
	
}

/*
* Drive Method actualy controls the Buggy with Methods from Motorhat Namespace
* Buggy won't drive if sensors are not initialized, for security
* Buggy won't drive if there was an Collision detected by Ultra Sonic Sensor
*/
void BuggyController::drive(int speedLeft, int speedRight, int directionLeft, int directionRight) {
	//Safe recent drive settings
	currentSpeed[0] = speedLeft;
	currentSpeed[1] = speedRight;
	currentDirection[0] = directionLeft;
	currentDirection[1] = directionRight;

	//If there is no Instance of UltraSonicSensor, the buggy is not allowed to drive
	if (!this->UltraSonicSensor)
		return;
	//Configure Speed Settings
	MotorhatDriver::setSpeed(i2c, M1, speedLeft);
	MotorhatDriver::setSpeed(i2c, M4, speedRight);
	//Configure Direction Settings
	MotorhatDriver::runMotor(i2c, M1, directionLeft);
	MotorhatDriver::runMotor(i2c, M4, directionRight);
}
/*
* Controlling Methods
* Public Methods that call private Methods in order to control the movement
* Methods determine Turning radius, driving Speed, etc.
*/
void BuggyController::turnLeftFWD(int milli) {
	std::cout << "Turn Left for " << milli << " milliseconds" << std::endl;

	//Drive
	drive(50, 127, MOTOR_FORWARD, MOTOR_FORWARD);
	//Drive for mili x milisecond
	driveDelay(milli);
	//Stop
	driveRelease();
}
void BuggyController::turnRightFWD(int milli) {
	std::cout << "Turn Right for " << milli << " milliseconds" << std::endl;

	//Drive 
	drive(127, 50, MOTOR_FORWARD, MOTOR_FORWARD);
	//Drive for mili x milisecond
	driveDelay(milli);
	//Stop
	driveRelease();
}
void BuggyController::driveForward(int milli) {
	std::cout << "Forwards for " << milli << " milliseconds" << std::endl;

	//Drive 
	drive(110, 127, MOTOR_FORWARD, MOTOR_FORWARD);
	//Drive for mili x milisecond
	driveDelay(milli);
	//Stop
	driveRelease();
}
void BuggyController::driveReverse(int milli) {
	std::cout << "Backwards for " << milli << " milliseconds" << std::endl;

	//Drive 
	drive(110, 127, MOTOR_BACK, MOTOR_BACK);
	//Drive for mili x milisecond
	driveDelay(milli);
	//Stop
	driveRelease();
}
void BuggyController::driveRelease() {
	//Configure Speed Settings
	MotorhatDriver::setSpeed(i2c, M1, 0);
	MotorhatDriver::setSpeed(i2c, M4, 0);
	//Configure Direction Settings
	MotorhatDriver::runMotor(i2c, M1, MOTOR_RELEASE);
	MotorhatDriver::runMotor(i2c, M4, MOTOR_RELEASE);
}
