#include "UltraSonicDriver.h"

//Method configurates pins for input or output
bool UltraSonicDriver::configPins() {
	//Check if pin variables are set
	if (!this->echo && !this->trigger)
		return false; //No Values for Pins set
	
	//Confugurate WiringPI Setup--> Use WpI Pin Numbers
	wiringPiSetup();
	//Configurate Pin with WiringPi Library
	pinMode(this->trigger, OUTPUT);
	pinMode(this->echo, INPUT);
	//Return Success
	return true;
}

double UltraSonicDriver::measureDistance() {
	double pulse_start = 0;
	double pulse_end = 0;
	double pulse_duration = 0;
	double distance = 0;
	
	//Set trigger low
	digitalWrite(this->trigger, LOW);
	//Let sensor settle
	delayMicroseconds(200);
	
	//Send Sound Signal for 10 Microseconds
	digitalWrite(this->trigger, HIGH);
	delayMicroseconds(10);
	digitalWrite(this->trigger, LOW);

	

	//Calculate pulse duration
	while (digitalRead(this->echo) == 0) {
		pulse_start = micros(); //Aktuelle Zeit in Mikrosekunden
	}
	while (digitalRead(this->echo) == 1) {
		pulse_end = micros(); //Aktuelle Zeit in Mikrosekunden
	}
	//17150 = 1/2 Schallgeschwindigkeit in cm/s
	return ((pulse_end / 1000000) - (pulse_start / 1000000)) * 17150;
}