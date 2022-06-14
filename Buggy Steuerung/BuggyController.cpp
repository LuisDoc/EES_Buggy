#include "BuggyController.h"

//
//
//	Constructor
//
//

//Basic Constructor
BuggyController::BuggyController() {
	
	//
	//	Initialise Vars
	//
	collisionDetected = true; //Buggy should not drive without first Check
	currentSpeed[2] = { 0 };
	currentDirection[2] = {0};
	
	//
	// Sensoren und Hardware Controller
	// 
	this->audioPlayer = new AudioPlayer;
	this->UltraSonicSensor = new UltraSonicDriver(usTriggerPin, usEchoPin);

	//
	//	Configure M1 and M4
	//
	this->i2c = MotorhatDriver::init();
	if (!i2c) {
		system("echo I2C Device not initialized.");
		return;
	}
	MotorhatDriver::initMotor(this->i2c, M1);
	MotorhatDriver::initMotor(this->i2c, M4);
	
	//
	//	Configure GPIOS
	//
	pinMode(brakePin, OUTPUT);
	
	//
	//	Method calls
	//
	this->collisionCheck(); //Buggy needs to check, wether he can start driving or not before first drive command
}

//
//
//	Collision Detection
//
//

//Method makes use of Ultra Sonic Sensor Driver to check for upcoming collisions
bool BuggyController::collisionCheck() {
	//
	//	Compare Distance to threshold, which is set by define || if there is no instance of Sensor, buggy should not be allowed to drive
	//
	if (this->UltraSonicSensor->measureDistance() < collisionDistanceThreshold || !this->UltraSonicSensor) {
		this->collisionDetected = true;
		return true;
	}
	else {
		this->collisionDetected = false;
		return false;
	}
	
}

//Method handles Collision by simply waiting until collisionCheck returns false
void BuggyController::collisionHandle() {
	//
	//	Busy wait until there is no more Collision
	//
	while (this->collisionCheck()) {
		delay(ccperiod / 2);//Check for Collision 2* faster then usual
	}
}

//Method plays Audio signal, when stopping because of collision
void BuggyController::collisionAudio(){
	std::cout << "Collision Audio beginning" << std::endl;
	audioPlayer->play();
	std::cout << "Collision Audio done " << std::endl;
}

//
//
//	Control Buggy Movement
//
//

//Private Method that actualy changes driving parameters
void BuggyController::drive(int speedLeft, int speedRight, int directionLeft, int directionRight) {
	//
	//	Save new Speed and Direction Configurations in Case Buggy needs to stop, because of Ultra Sonic Sensor alert
	//
	currentSpeed[0] = speedLeft;
	currentSpeed[1] = speedRight;
	currentDirection[0] = directionLeft;
	currentDirection[1] = directionRight;

	
	if (!this->UltraSonicSensor) //Without an instance of the sensor, buggy should not be allowed to drive
		return;
	
	//
	//	Configure Speed and Direction of M1 and M4
	//
	MotorhatDriver::setSpeed(i2c, M1, speedLeft);
	MotorhatDriver::setSpeed(i2c, M4, speedRight);
	MotorhatDriver::runMotor(i2c, M1, directionLeft);
	MotorhatDriver::runMotor(i2c, M4, directionRight);
}

//Delay which includes cyclic collision checks
void BuggyController::driveDelay(int milli) {
	//
	//	Calculate Intervals in which Collision Check has to be done
	//
	int n_perioden = milli / ccperiod;
	int periode = 0;
	while (periode++ < n_perioden) {
		//
		//	When Collision Detected stop buggy and wait, until object disappeared from sensors
		//
		if (this->collisionCheck()) {
			//
			//	Stop the buggy and activate Brake Light
			//
			this->driveRelease(); //Stop Buggy
			digitalWrite(brakePin, HIGH); //Activate Brake Light
			
			//
			//	Run threads for handling and audio signals
			//
			std::thread cHandle(&BuggyController::collisionHandle, this);
			std::thread cAudio (&BuggyController::collisionAudio, this);
			cHandle.join();
			cAudio.join();
			//
			//	Resume Drive and deactivate Brake Light
			//
			digitalWrite(brakePin, LOW);
			std::cout << "Collision Handle done " << std::endl;
			this->drive(currentSpeed[0], currentSpeed[1], currentDirection[0], currentDirection[1]);
		}
		//Wait for collisionCheckPeriode milliseconds
		delay(ccperiod);
	}

}

//Public methods that can be called by user, which determined values for drive method
void BuggyController::turnLeftFWD(int milli) {
	std::cout << "Left Turn: " << milli << " milliseconds" << std::endl;
	//
	//	Turn Left for milli x milliseconds, then stop the buggy
	//
	drive(50, 127, MOTOR_FORWARD, MOTOR_FORWARD);
	driveDelay(milli);
	driveRelease();
}
void BuggyController::turnRightFWD(int milli) {
	std::cout << "Right Turn: " << milli << " milliseconds" << std::endl;
	//
	//	Turn Right for milli x milliseconds, then stop the buggy
	//
	drive(127, 50, MOTOR_FORWARD, MOTOR_FORWARD);
	driveDelay(milli);
	driveRelease();
}
void BuggyController::driveForward(int milli) {
	std::cout << "Forwarddrive: " << milli << " milliseconds" << std::endl;
	//
	//	Drive Reverse for milli x milliseconds, then stop the buggy
	//
	drive(110, 127, MOTOR_FORWARD, MOTOR_FORWARD);
	driveDelay(milli);
	driveRelease();
}
void BuggyController::driveReverse(int milli) {
	std::cout << "Reversedrive: " << milli << " milliseconds" << std::endl;
	//
	//	Drive Reverse for milli x milliseconds, then stop the buggy
	//
	drive(110, 127, MOTOR_BACK, MOTOR_BACK);
	driveDelay(milli);
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
