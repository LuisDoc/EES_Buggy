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
// Ultraschallsensor Test Methode
void testUltraschallSensor(UltraSonicDriver us) {
	std::cout << "Abstand: " << us.measureDistance() << std::endl;
}

int main(void)
{
	// Csignal für Abbruch über STRG-C
	signal(SIGINT, signalHandler);
	//Notify Programm start on console
	std::cout << "Programm started" << std::endl;


	do{
		//Start Programm Code here
		
		bc.driveForward(3000);
		delay(1000);
		
		//End Programm Code here
	} while (0);

	//Stop Buggy if programm is done
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