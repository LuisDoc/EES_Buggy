#pragma once
#include "UltraSonicDriver.h"
#include "AudioPlayer.h"
#include "MotorhatDriver.h"
#include <thread>
#include <future>
#include <iostream>
#include <stdlib.h>

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

/*
* Configurate via Defines
*/
#define ccperiod 5
#define collisionDistanceThreshold 25





class BuggyController
{
private:
	//
	//	Variablen
	//
	int i2c;
	bool collisionDetected; //Buggy not allowed to drive, if value is true
	int currentSpeed[2]; //Backup current Speed Settings
	int currentDirection[2]; //Backup current Drive-Direction Settings
	//
	//	sensors & hardware controller
	//
	UltraSonicDriver* UltraSonicSensor;
	AudioPlayer* audioPlayer;
	//
	//	intern methods
	//
	void drive(int speedLeft, int speedRight, int directionLeft, int directionRight); //Buggy will Drive with given Parameters
	void driveDelay(int milli); //Implements an Delay with cyclic collision checks
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
	void turnLeftFWD(int milli);
	void turnRightFWD(int milli);
	void driveForward(int milli);
	void driveReverse(int milli);
	void driveRelease();
};

