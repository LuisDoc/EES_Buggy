#pragma once
#include "UltraSonicDriver.h"
#include "AudioPlayer.h"
#include "MotorhatDriver.h"
#include "MPU6050.h"
#include <thread>
#include <future>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <experimental/filesystem>

/*
*	Pin-Defines
*/
#define usTriggerPin 24 //BCM Pin 19
#define usEchoPin 23 //BCM Pin 16
#define brakePin 22 //BCM Pin 6

#define M1 1
#define M2 2
#define M3 3
#define M4 4

#define MOTOR_FORWARD   1
#define MOTOR_BACK      2
#define MOTOR_BRAKE     3
#define MOTOR_RELEASE   4



/*
* Configurate via Defines
*/
#define microDelay 10
#define collisionHandleDelay 500
#define collisionDistanceThreshold 25
#define AngleThreshold 1





class BuggyController
{
private:
	//
	//	Variablen
	//
	int i2c;
	int i2cMPU6050;
	bool collisionDetected; //Buggy not allowed to drive, if value is true
	int currentSpeed[2]; //Backup current Speed Settings
	int currentDirection[2]; //Backup current Drive-Direction Settings
	std::string logfile_name = "correction_log.txt";
	//
	//	sensors & hardware controller
	//
	UltraSonicDriver* UltraSonicSensor;
	AudioPlayer* audioPlayer;
	//
	//	intern methods
	//
	void driveConfig(int speedLeft, int speedRight, int directionLeft, int directionRight); //Set Values to Speed and Direction of M1 and M4
	void run(int angle_goal, int micros); //Run's the buggy until time_goal is reached in angle_goal's direction
	void run(int angle_goal); //Run's the buggy until angle_goal is reached
	void correctDrive(int angle_goal);
	bool collisionCheck(); //When called, buggy will check for collision
	void collisionHandle(); //Busy wait until there is no more collision
	void collisionAudio(); //Method plays Audio Signal
	
public:
	//
	//	Constructor
	//
	BuggyController();
	//
	//	public methods
	//
	void move(int angle, int direction, int duration); //Move until time goal reached
	void move(int angle, int direction); //Move until angle reached

	void driveRelease();
};

