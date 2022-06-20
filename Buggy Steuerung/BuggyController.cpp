#include "BuggyController.h"


/*
* Basic Constructor will initialise all Variables and initialise all hardware devices and used Gpios
* For safety, first Operation will check for possible Collisions with Objects 
*/
BuggyController::BuggyController() {
	//Initialise Values
	collisionDetected = true; //Buggy should not drive without first Check
	currentSpeed[2] = { 0 };
	currentDirection[2] = {0};
	//Sensors and Hardware initialisiation
	this->audioPlayer = new AudioPlayer;
	this->UltraSonicSensor = new UltraSonicDriver(usTriggerPin, usEchoPin);
	this->i2cMPU6050 = MPU6050::init();
	//Configure M1 and M4
	this->i2c = MotorhatDriver::init();
	if (!i2c) {
		system("echo I2C Device not initialized.");
		return;
	}
	MotorhatDriver::initMotor(this->i2c, M1);
	MotorhatDriver::initMotor(this->i2c, M4);
	//Configure GPIOS
	pinMode(brakePin, OUTPUT);
	//Start with Possible Collision Check
	this->collisionCheck(); //Buggy needs to check, wether he can start driving or not before first drive command
}

/*
* Check for Collision with UltraSonic Sensor
* Method will set global flag and return boolean, so 'run' method can start with collision handling
*/
bool BuggyController::collisionCheck() {
	//Compare Distance to Threshold | Disable drive when there is no instance uf Sensor
	if (this->UltraSonicSensor->measureDistance() < collisionDistanceThreshold || !this->UltraSonicSensor) {
		this->collisionDetected = true;
		return true;
	}
	else {
		this->collisionDetected = false;
		return false;
	}
	
}

/*
* Wait till collisionCheck returns false
* Check frequently
*/
void BuggyController::collisionHandle() {
	//activate Brake Light
	digitalWrite(brakePin, HIGH);

	//Busy wait till there is no more Collision
	while (this->collisionCheck()) {
		delay(collisionHandleDelay);
	}
	//deactivate Brake Light
	digitalWrite(brakePin, LOW);
}

/*
* Method uses AudioPlayer to play a track/sound from 3.5mm Jack
*/
void BuggyController::collisionAudio(){
	//audioPlayer->play();
}

/*
* Changes drive parameters for speed and driving direction
* new settings will be saved in two arrays, so the buggy can use those values, to get back running, after a collision warning stopped it.
*/
void BuggyController::driveConfig(int speedLeft, int speedRight, int directionLeft, int directionRight) {
	//Backup speed and direction values
	currentSpeed[0] = speedLeft;
	currentSpeed[1] = speedRight;
	currentDirection[0] = directionLeft;
	currentDirection[1] = directionRight;
	//Without an instance of the sensor, buggy should not be allowed to drive
	if (!this->UltraSonicSensor) 
		return;
	//Configure speed and direction registers
	MotorhatDriver::setSpeed(i2c, M1, speedLeft);
	MotorhatDriver::setSpeed(i2c, M4, speedRight);
	MotorhatDriver::runMotor(i2c, M1, directionLeft);
	MotorhatDriver::runMotor(i2c, M4, directionRight);
}

/*
* Method will run Buggy for a given duration of time
* Buggy will aim to reach given angle_goal
*/
void BuggyController::run(int angle_goal, int millis) {
	
	int drive_till = micros() + millis * 1000;
	
	while (micros() < drive_till) {
		//Collision Detection
		if (this->collisionCheck()) {
			//Stop Buggy
			this->driveRelease();
			//Run Threads for Collision Handle and Audio Player - Wait for Threads to finish
			std::thread cHandle(&BuggyController::collisionHandle, this);
			std::thread cAudio (&BuggyController::collisionAudio, this);
			cHandle.join();
			std::cout << "Collision Handle done " << std::endl;
			cAudio.join();
			std::cout << "AudioPlayer done" << std::endl;
			//Resume Drive
			this->driveConfig(currentSpeed[0], currentSpeed[1], currentDirection[0], currentDirection[1]);
		}
		//Deviation Check 
		correctDrive(angle_goal);

		//Wait for collisionCheckPeriode Microseconds
		delayMicroseconds(microDelay);
	}
}
/*
* Method will run Buggy until given angle_goal is reached
*/
void BuggyController::run(int angle_goal) {

	MPU6050::getMotions(i2cMPU6050); //Calculate current Z angle
	int current = MPU6050::angle_z; //Get Current Z angle
	
	//While Angle goal is not equal to current angle --> Run
	while (current > (angle_goal + AngleThreshold) || current < (angle_goal - AngleThreshold)) {
		//Read Position from Last update
		current = MPU6050::angle_z;
		//Collision Detection
		if (this->collisionCheck()) {
			//Stop Buggy
			this->driveRelease();
			//Run Threads for Collision Handle and Audio Player - Wait for Threads to finish
			std::thread cHandle(&BuggyController::collisionHandle, this);
			std::thread cAudio(&BuggyController::collisionAudio, this);
			cHandle.join();
			std::cout << "Collision Handle done " << std::endl;
			cAudio.join();
			std::cout << "AudioPlayer done" << std::endl;
			//Resume Drive
			this->driveConfig(currentSpeed[0], currentSpeed[1], currentDirection[0], currentDirection[1]);
		}
		//Deviation Check 
		correctDrive(angle_goal);
	}

}

/*
* Method will check if Buggy is still on course to reach angle_goal.
* Deviations will cause change in speed parameters
*/
void BuggyController::correctDrive(int angle_goal) {
	MPU6050::getMotions(i2cMPU6050);
	double angle = MPU6050::angle_z; //Current Degree
	double deltaAngle = angle_goal - angle;

	//Calculate random values for Engine Settings ==> Generate Log-Data for KI
	int slow = 75 + (std::rand() % (125 - 75 + 1));
	int fast = 125 + (std::rand() % (175 - 125 + 1));

	 //Save Base_path in variable
	std::string path = (std::experimental::filesystem::current_path().u8string());
	//Open File
	std::fstream log("log.txt", std::fstream::app);
	//Goal Angle | Current Angle | Current Speed L | Current Speed R | Next Speed L | Next Speed Right 
	if (log) {
		//Write new Row with new Data except result angle, which will be added in next iteration
		log << angle_goal << ", " << angle << ", " << currentSpeed[0] << ", " << currentSpeed[1] << ", " << slow << ", " << fast <<"\n";
		std::cout << angle_goal << ", " << angle << ", " << currentSpeed[0] << ", " << currentSpeed[1] << ", " << slow << ", " << fast << "\n";
	}
	else {
		std::cout << "Logfile cannot be opend" << std::endl;
	}
	log.close();
	

	//Deviation from Threshold
	if (deltaAngle > AngleThreshold) {
		//Korrektur nach Links
		if(currentDirection[0] == MOTOR_FORWARD)
			driveConfig(slow, fast, currentDirection[0], currentDirection[1]);
		else
			driveConfig(fast, slow, currentDirection[0], currentDirection[1]);
		
	}
	else if(deltaAngle < (AngleThreshold *(-1))) {
		//Korrektur nach Rechts
		if (currentDirection[0] == MOTOR_FORWARD)
			driveConfig(fast, slow, currentDirection[0], currentDirection[1]);
		else
			driveConfig(slow, fast, currentDirection[0], currentDirection[1]);
	}
}

/*
* Method stops buggy
*/
void BuggyController::driveRelease() {
	//Configure Speed Settings
	MotorhatDriver::setSpeed(i2c, M1, 0);
	MotorhatDriver::setSpeed(i2c, M4, 0);
	//Configure Direction Settings
	MotorhatDriver::runMotor(i2c, M1, MOTOR_RELEASE);
	MotorhatDriver::runMotor(i2c, M4, MOTOR_RELEASE);
}

/*
* Move Buggy in given direction for specific period of time
*/
void BuggyController::move(int angle, int direction, int duration) {
	std::cout << "Drive " << angle << "° until " << duration << " milliseconds" << std::endl;
	
	int angle_goal = MPU6050::angle_z + angle;
	driveConfig(150, 150, direction, direction);
	run(angle_goal, duration);
	driveRelease();
}

/*
* Move Buggy in given direction until angle is reached
*/
void BuggyController::move(int angle, int direction) {
	std::cout << "Drive until " << angle << "° reached" << std::endl;
	
	int angle_goal = MPU6050::angle_z + angle;
	driveConfig(150, 150, direction, direction);
	run(angle_goal);
	driveRelease();
}