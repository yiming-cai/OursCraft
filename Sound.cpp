#include "Sound.h"

void Sound::updateListener()
{
	glm::vec3 at = camera->camera_lookAt - camera->camera_pos;
	glm::vec3 up = camera->camera_up;
	glm::vec3 pos = camera->camera_pos;
	ALfloat listenerOri[] = {at[0],at[1],at[2],up[0],up[1],up[2]};

	alListener3f(AL_POSITION, pos[0], pos[1], pos[2]);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);
}

ALuint Sound::generateSource(glm::vec3 position)
{
	ALuint source;

	glm::vec3 curr_pos = position;
	// check for errors

	alGenSources((ALuint)1, &source);
	alSourcef(source, AL_PITCH, 1);
	alSourcef(source, AL_GAIN, 1);
	alSource3f(source, AL_POSITION, curr_pos[0], curr_pos[1], curr_pos[2]);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, AL_FALSE);
	allGeneratedSources.insert(source);
	bindedSources[source] = 0;
	return source;
}

void Sound::updateSourcePosition(ALuint source, glm::vec3 curr_pos)
{
	if (allGeneratedSources.find(source) == allGeneratedSources.end())
	{
		std::cerr << "This source does not exist!" << std::endl;
		return;
	}
	alSource3f(source, AL_POSITION, curr_pos[0], curr_pos[1], curr_pos[2]);
}

void Sound::updateSourceVelocity(ALuint source, glm::vec3 velocity)
{
	if (allGeneratedSources.find(source) == allGeneratedSources.end())
	{
		std::cerr << "This source does not exist!" << std::endl;
		return;
	}
	alSource3f(source, AL_VELOCITY, velocity[0], velocity[1], velocity[2]);
}

ALuint Sound::generateBuffer(std::string filepath)
{

	std::vector<char> bytes(filepath.begin(), filepath.end());
	bytes.push_back('\0');
	char *c = &bytes[0];

	SndfileHandle file;
	fprintf(stderr, "Reading file named '%s'\n", c);
	file = SndfileHandle(c);
	printf("Opened file '%s'\n", c);
	printf("    Sample rate : %d\n", file.samplerate());
	printf("    Channels    : %d\n", file.channels());
	printf("    Formats    : %d\n", file.format());
	
	ALuint buffer;

	alGenBuffers((ALuint)1, &buffer);
	// check for errors

	//alBufferData(buffer, AL_FORMAT_STEREO16, );
	// check for errors

	//ALuint buffer = alutCreateBufferFromFile(c);
	if (checkError()) return NULL;

	allGeneratedBuffers.insert(buffer);
	return buffer;
}

void Sound::bindSourceToBuffer(ALuint source, ALuint buffer)
{
	if (allGeneratedSources.find(source) == allGeneratedSources.end())
	{
		std::cerr << "This source does not exist!" << std::endl;
		return;
	}
	if (allGeneratedBuffers.find(buffer) == allGeneratedBuffers.end())
	{
		std::cerr << "This buffer does not exist!" << std::endl;
		return;
	}
	alSourcei(source, AL_BUFFER, buffer);
	if (checkError()) { return; }
	bindedSources[source] = buffer;
}

void Sound::playSound(ALuint source)
{
	if (allGeneratedSources.find(source) == allGeneratedSources.end())
	{
		std::cerr << "This source does not exist!" << std::endl;
		return;
	}
	alSourcePlay(source);
}

bool Sound::isSourcePlaying(ALuint source)
{
	if (allGeneratedSources.find(source) == allGeneratedSources.end())
	{
		std::cerr << "This source does not exist!" << std::endl;
		return false;
	}
	ALint source_state;
	alGetSourcei(source, AL_SOURCE_STATE, &source_state);
	if (checkError()) return false;
	
	if (source_state == AL_PLAYING) return true;
	else return false;
}

void Sound::pauseUntilFinishPlaying(ALuint source)
{
	if (allGeneratedSources.find(source) == allGeneratedSources.end())
	{
		std::cerr << "This source does not exist!" << std::endl;
		return;
	}
	ALint source_state;
	alGetSourcei(source, AL_SOURCE_STATE, &source_state);
	if (checkError()) return;
	while (source_state == AL_PLAYING) {
		alGetSourcei(source, AL_SOURCE_STATE, &source_state);
		if (checkError()) return;
	}
}

Sound::Sound(Camera * cam)
{
	camera = cam;
	if (cam != nullptr)
	{
		updateListener();
	}

	ALCboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
	{
		// enumeration not supported
		std::cerr << "AL Enumeration is not supported" << std::endl;
	}

	// reset error stack
	alGetError();

	device = alcOpenDevice(NULL);
	if (!device)
	{
		std::cerr << "Bad audio input!" << std::endl;
	}

	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context))
	{
		checkError();
		return;
	}

	//alutInitWithoutContext(NULL, NULL);
}

// for cleaning up everything
Sound::~Sound()
{
	for (auto source : allGeneratedSources)
	{
		alDeleteSources(1, &source);
	}

	for (auto buffer : allGeneratedBuffers)
	{
		alDeleteBuffers(1, &buffer);
	}

	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	//alutExit();
}

void Sound::list_audio_devices(const ALCchar *devices)
{
	const ALCchar *device = devices, *next = devices + 1;
	size_t len = 0;

	fprintf(stdout, "Devices list:\n");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0') {
		fprintf(stdout, "%s\n", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	fprintf(stdout, "----------\n");
}

bool Sound::checkError()
{
	ALCenum error;

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		//std::cerr << alutGetErrorString(error) << std::endl;
		return true;
	}
	return false;
}


