#pragma once

#include "Util.h"
#include "Camera.h"
#include "Model.h"
#include <AL/al.h>
#include <AL/alc.h>
//#include <AL/alut.h>
#include <sndfile.h>
#include <sndfile.hh>
#include <iostream>
#include <set>

struct SoundAttributes
{
	short * data;
	int size;
	int frequency;
	ALenum format;
};

class Sound
{
private:

	ALCdevice * device;
	ALCcontext * context;
	Camera * camera;
	std::set<ALuint> allGeneratedSources;
	std::set<ALuint> allGeneratedBuffers;
	std::map<ALuint, ALuint> bindedSources;
	std::map< std::string, SoundAttributes > soundBuffers;

public:

	Sound(Camera * cam);
	~Sound();

	// for testing
	void list_audio_devices(const ALCchar * devices);

	// for checking and printing out any errors
	// return true if there is indeed an error
	bool checkError(std::string comment);

	// for updating sound listener to current camera context
	void updateListener();

	// use this for generating a new sound source (or object)
	ALuint generateSource(glm::vec3 position);

	// use this for updating the location of the source (or object)
	void updateSourcePosition(ALuint source, glm::vec3 position);

	// use this for updating the velocity of the source (or object)
	void updateSourceVelocity(ALuint source, glm::vec3 velocity);

	//void updateSour

	// use this if you want the Sound to loop
	void setSourceLooping(ALuint source, bool shouldLoop);

	// generate a new buffer for the sound file specified by the filepath
	// will return null and print an error message if there's any errors
	ALuint generateBuffer(std::string filepath);

	// this will bind a source to a buffer, to actually play an audio
	void bindSourceToBuffer(ALuint source, ALuint buffer);

	// play the sound in the last binded buffer to this source
	void playSound(ALuint source);

	// check if the current source is still playing the sound
	bool isSourcePlaying(ALuint source);

	void pauseUntilFinishPlaying(ALuint source);
};