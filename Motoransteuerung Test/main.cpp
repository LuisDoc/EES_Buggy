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

void signalHandler(int signum);

// Buggy fährt ein Rechteck ab
void square(BuggyController bc) {
	for (int i = 0; i < 16; i++) {
		bc.driveForward(1000);
		bc.turnRightFWD(970);
	}
	
}
// Buggy fährt einen Slalom
void slalom(BuggyController bc) {
	bc.driveForward(1000);
	bc.turnRightFWD(970);
	bc.driveForward(500);
	bc.turnLeftFWD(1300);
	bc.driveForward(1000);
	bc.turnRightFWD(970);
	bc.driveForward(500);
}

//Globale Variablen
BuggyController bc;
UltraSonicDriver us;

int main(void)
{
	// Csignal für Abbruch über STRG-C
	signal(SIGINT, signalHandler);

	std::cout << "Programm started" << std::endl;
	//Start programm code
	delay(2000);
	bc.driveForward(5000);

	//End programm code

	bc.driveRelease();
	return EXIT_SUCCESS;
}



/// Interrupt Routine for STRG-C
void signalHandler(int signum)
{	
	std::cout << std::endl <<  "Programm aborted: CTRL + C pressed" << std::endl;
	// Beenden Sie hier bitte alle Verbindung zu den Sensoren etc.
	bc.driveRelease();
	exit(signum);
}