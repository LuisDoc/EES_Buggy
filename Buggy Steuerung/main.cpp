/// Includes
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <softPwm.h>
#include <signal.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "MotorhatDriver.h"
#include "BuggyController.h"
#include "UltraSonicDriver.h"
#include "MPU6050.h"
#include "AudioPlayer.h"

/*
* Global Vars
*/
// Objects of Controll-Classes
BuggyController bc;
UltraSonicDriver us;
AudioPlayer ap;


//Stop Programm when STRG + C pressed
void signalHandler(int signum);

int main(void)
{
	// Csignal für Abbruch über STRG-C
	signal(SIGINT, signalHandler);
	//Notify Programm start on console
	std::cout << "Programm started" << std::endl;

	//Programm Code :: Start

	//Drive Square
	for (int i = 0; i < 16; i++) {
		bc.move(0, MOTOR_FORWARD, 2000);
		bc.move(-90, MOTOR_FORWARD);
	}
	

	//Programm Code :: Stop
	bc.driveRelease();
	return EXIT_SUCCESS;
}



/// Interrupt Routine for STRG-C
void signalHandler(int signum)
{
	std::cout << std::endl << "Programm aborted: CTRL + C pressed" << std::endl;
	// Beenden Sie hier bitte alle Verbindung zu den Sensoren etc.
	bc.driveRelease();
	exit(signum);
}