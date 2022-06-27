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
	previous_goal_angle = 0;
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
		std::cout << "!!! Possible Collision Detected !!!\n";
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
	
	
	
	//Save Current Drive Speeds
	int left_speed = currentSpeed[0];
	int right_speed = currentSpeed[1];
	int backup_left = left_speed;
	int backup_right = right_speed;

	//Check if Course Correction is needed
	if (deltaAngle > AngleThreshold || deltaAngle < (AngleThreshold * (-1))) {
		//Calculate new Speeds for Left and Right
		calculateSpeeds(left_speed, right_speed, deltaAngle);
	}
	else {
		//Reset Values
		left_speed = default_speed;
		right_speed = default_speed;
	}
	//Adjust Drive Parameters
	driveConfig(left_speed, right_speed, currentDirection[0], currentDirection[1]);
		
	/*
	* Write to Logfile and console
	*/
	
	std::cout << "Left: " << left_speed << " Right: " << right_speed << " ||| ";
	std::cout << "Current Angle: " << angle << "=> Goal: " << angle_goal << std::endl;

	//Open File
	std::fstream log("log.txt", std::fstream::app);
	//Goal Angle | Current Angle | Current Speed L | Current Speed R | Next Speed L | Next Speed Right 
	if (log) {
		//Write new Row with new Data except result angle, which will be added in next iteration
		log << angle_goal << ", " << angle << ", " << backup_left << ", " << backup_right << ", " << left_speed << ", " << right_speed << "\n";
	}
	else {
		std::cout << "Logfile cannot be opend" << std::endl;
	}
	log.close();
}

/*
* Method calculates drive Speeds for the slower and the faster 
*/
void BuggyController::calculateSpeeds(int& left, int& right, int delta_angle) {
	/*
	* Calculate two RPM Speeds
	*/

	int slow = default_speed - abs(delta_angle) * 6;
	int fast = default_speed + abs(delta_angle) * 6;

	//Check if Speeds exceed
	if (slow <= driveMin)
		slow = driveMin;
	if (fast >= driveMax)
		fast = driveMax;

	/*
	* Decide which Tire needs to be faster or slower
	*/

	//Correct to left handside
	if (delta_angle > AngleThreshold) {
		if (currentDirection[0] == MOTOR_FORWARD) {
			left = slow;
			right = fast;
		}
		else {
			left = fast;
			right = slow;
		}
	}
	//Correct to right handside
	else if (delta_angle < (AngleThreshold * (-1))) {
		if (currentDirection[0] == MOTOR_FORWARD) {
			left = fast;
			right = slow;
		}
		else{
			left = slow;
			right = fast;
		}
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
	std::cout << "\n\n\nDrive " << angle << " Degree until " << duration << " milliseconds" << std::endl << std::endl;
	
	int angle_goal = previous_goal_angle + angle;
	previous_goal_angle = angle_goal;

	driveConfig(default_speed, default_speed, direction, direction);
	run(angle_goal, duration);
	driveRelease();
	delay(150); 
}

/*
* Move Buggy in given direction until angle is reached
*/
void BuggyController::move(int angle, int direction) {
	std::cout << "\n\n\nDrive until " << angle << "Degree reached" << std::endl << std::endl;

	int angle_goal = previous_goal_angle + angle;
	previous_goal_angle = angle_goal;

	driveConfig(default_speed, default_speed, direction, direction);
	run(angle_goal);
	driveRelease();
	delay(150);
}

/*
* Fun Mode
*/
void BuggyController::fun(int duration) {
	driveConfig(100, 100, MOTOR_FORWARD, MOTOR_BACK);
	run(-720);
	driveRelease();
}

/*
* Control Buggy live from Terminal
*/
void BuggyController::TerminalControl() {
	char input;

	bool quit = false;

	std::cout << "Press W, A, S, D to move the Buggy " << std::endl;

	while(!quit){
		//Check Break condition
		std::cout << "Waiting for Order: ? " << std::endl;
		std::cin.get(input);
		//Listen to input
		
		switch (input) {
		case 'w': this->move(0, MOTOR_FORWARD, 5000); break;
		case 'a': this->move(90, MOTOR_FORWARD); break;
		case 's': this->move(0, MOTOR_BACK, 5000); break;
		case 'd': this->move(-90, MOTOR_FORWARD); break;
		default: std::cout << "invalid input, try again" << std::endl;
		}
	}
}