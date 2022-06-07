#pragma once
#include "MotorhatDriver.h"
#include <iostream>
#include "UltraSonicDriver.h"
#include <stdlib.h>

/*
*	Pin-Defines
*/
#define usTriggerPin 24
#define usEchoPin 23

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
	//Variablen
	int i2c;
	int collisionCheckPeriode; //Buggy will check each ccperiod[ms] for collisions
	int collisionThreshold; //Buggy will stop, if distance is lower then collisionDistanceThreshold[cm]
	bool collisionDetected; //Buggy not allowed to drive, if value is true
	int currentSpeed[2];
	int currentDirection[2];
	//Sensoren
	UltraSonicDriver* UltraSonicSensor;
	//Private Methoden
	void drive(int speedLeft, int speedRight, int directionLeft, int directionRight); //Buggy will Drive with given Parameters
	bool collisionCheck(); //When called, buggy will check for collision
	void driveDelay(int milli);
public:
	BuggyController();
	void turnLeftFWD(int milli);
	void turnRightFWD(int milli);
	void driveForward(int milli);
	void driveReverse(int milli);
	void driveRelease();
};

