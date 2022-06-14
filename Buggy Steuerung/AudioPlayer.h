#pragma once
#include <stdlib.h>
#include <iostream>
#include <string>
#include <experimental/filesystem>

#define egzon_audio_upper_limit 5
#define egzon_audio_lower_start 1


/*
* Treiber-Klasse für 3.5mm Audio-Jack am Raspberry Pi 4B
*/
class AudioPlayer
{
private:
	std::string audio_path = "/sources/audio/";
	std::string base_path = "";

	/* Private Methoden */
	void playNiluBots();

public:
	void play();
	AudioPlayer();
};

