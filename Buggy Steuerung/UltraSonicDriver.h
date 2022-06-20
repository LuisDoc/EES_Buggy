#pragma once
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <softPwm.h>
#include <stdlib.h>
#include <chrono>
#define triggerwPi 24
#define echowPi 23

class UltraSonicDriver
{
private:
	//BCM Pins
	int trigger;
	int echo;
	//Hilfsmethoden
	bool configPins();
public:
	/*
	* Konstruktoren
	*/
	//Default Konstruktor mit festgelegten Pins wPi Pins
	UltraSonicDriver() :trigger{ triggerwPi }, echo{ echowPi }{configPins(); }
	//Konstruktor mit variablen Pins
	UltraSonicDriver(int triggerBCMPin, int echoBCMPin) : trigger{ triggerBCMPin }, echo{ echoBCMPin }{configPins(); }
	/*
	* Methoden
	*/
	double measureDistance();
};

