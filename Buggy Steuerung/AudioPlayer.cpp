#include "AudioPlayer.h"

//Basic-Constructor
AudioPlayer::AudioPlayer() {
	//Save Base_path addr
	this->base_path = std::experimental::filesystem::current_path(); //Save Base_path in variable
	//Seeding Random Numbers Generator
	srand(time(NULL));
}


void AudioPlayer::play() { //Public Starter-Method for multiple private play Methods

	playNiluBots();
}

void AudioPlayer::playNiluBots() {
	std::vector<std::string> audio_list; //list of all audio files in following directory
	//Get all files in Directory
	for (const auto& entry : std::experimental::filesystem::directory_iterator(this->base_path + this->audio_path))
		audio_list.push_back(entry.path());
	//Generate random number
	int ran = (std::rand() % audio_list.size());
	//Play Audio to Jack
	std::string command = "mpg321 -g 100 " + audio_list[ran]; //PLay random audio file
	std::cout << "Max: " << audio_list.size() << " und random number: " << ran << std::endl;
	system(command.c_str());
}