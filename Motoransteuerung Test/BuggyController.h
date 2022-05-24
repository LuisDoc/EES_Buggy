#pragma once
#include "MotorhatDriver.h"
#include <stdlib.h>

#define M1 1
#define M2 2
#define M3 3
#define M4 4


class BuggyController
{
private:
	int i2c;
public:
	BuggyController();
	void turnLeftFWD(int mili);
	void turnRightFWD(int mili);
	void driveForward(int mili);
	void driveReverse(int mili);
	void driveRelease();
};

