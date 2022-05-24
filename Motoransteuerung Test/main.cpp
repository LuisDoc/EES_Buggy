/// Includes
#include <signal.h>
#include <stdlib.h>
#include "MotorhatDriver.h"
#include "BuggyController.h"

void signalHandler(int signum);

/* Buggy fährt ein Rechteck ab */
void square(BuggyController bc) {
	for (int i = 0; i < 16; i++) {
		bc.driveForward(1000);
		bc.turnRightFWD(970);
	}
	
}
//360° Links Umdrehung := 3225
//360° Rechts Umdrehung := 2675
/* Buggy fährt einen Slalom*/
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

int main(void)
{
	// Csignal für Abbruch über STRG-C
	signal(SIGINT, signalHandler);
	
	//square(bc);
	square(bc);

	bc.driveRelease();
	return EXIT_SUCCESS;
}



/// Interrupt Routine for STRG-C
void signalHandler(int signum)
{
	// Beenden Sie hier bitte alle Verbindung zu den Sensoren etc.
	bc.driveRelease();
	//
	exit(signum);
}