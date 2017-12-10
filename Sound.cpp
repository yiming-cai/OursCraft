#include "Sound.h"

void Sound::updateListener()
{
	glm::vec3 at = camera->camera_lookAt - camera->camera_pos;
	glm::vec3 up = camera->camera_up;
	glm::vec3 pos = camera->camera_pos;
	ALfloat listenerOri[] = {at[0],at[1],at[2],up[0],up[1],up[2]};

	alListener3f(AL_POSITION, pos[0], pos[1], pos[2]);
	alListener3f(AL_VELOCITY, 0.0f, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);
}

ALuint Sound::generateSource(glm::vec3 position)
{
	ALuint source;

	glm::vec3 curr_pos = position;

	alGenSources((ALuint)1, &source);
	alSourcef(source, AL_PITCH, 1.0f);
	alSourcef(source, AL_GAIN, 1.0f);
	alSource3f(source, AL_POSITION, curr_pos[0], curr_pos[1], curr_pos[2]);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, AL_FALSE);
	allGeneratedSources.insert(source);
	bindedSources[source] = 0;

	// check for errors
	checkError("Error generating a source");

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

void Sound::updateSourceGain(ALuint source, float gain)
{
	if (allGeneratedSources.find(source) == allGeneratedSources.end())
	{
		std::cerr << "This source does not exist!" << std::endl;
		return;
	}
	alSourcef(source, AL_POSITION, gain);
}

void Sound::updateSourcePitch(ALuint source, float pitch)
{
	if (allGeneratedSources.find(source) == allGeneratedSources.end())
	{
		std::cerr << "This source does not exist!" << std::endl;
		return;
	}
	alSourcef(source, AL_POSITION, pitch);
}

void Sound::setSourceLooping(ALuint source, bool shouldLoop)
{
	if (allGeneratedSources.find(source) == allGeneratedSources.end())
	{
		std::cerr << "This source does not exist!" << std::endl;
		return;
	}
	alSourcei(source, AL_LOOPING, (shouldLoop? 1 : 0) );
}

ALuint Sound::generateBuffer(std::string filepath)
{
	// set max buffer len to 30mb
	const static int MAX_BUFFER_LEN = 1024 * 1024 * 30;

	SoundAttributes curr;
	auto it = soundBuffers.find(filepath);
	if (it == soundBuffers.end())
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

		ALenum format = 0;
		if (file.channels() == 2)
		{
			if (file.format() && SF_FORMAT_PCM_16 == SF_FORMAT_PCM_16)
			{
				format = AL_FORMAT_STEREO16;
				std::cerr << "Reading in STEREO 16bit sound file" << std::endl;
			}
			else if (file.format() && SF_FORMAT_PCM_S8 == SF_FORMAT_PCM_S8)
			{
				format = AL_FORMAT_STEREO8;
				std::cerr << "Reading in STEREO 8bit sound file" << std::endl;
			}
		}
		else if (file.channels() == 1)
		{
			if (file.format() && SF_FORMAT_PCM_16 == SF_FORMAT_PCM_16)
			{
				format = AL_FORMAT_MONO16;
				std::cerr << "Reading in MONO 16bit sound file" << std::endl;
			}
			else if (file.format() && SF_FORMAT_PCM_S8 == SF_FORMAT_PCM_S8)
			{
				std::cerr << "Reading in MONO 8bit sound file" << std::endl;
				format = AL_FORMAT_MONO8;
			}
		}
		if (format == 0)
		{
			std::cerr << "Does not support " << file.channels() << " channels with sample size that are not 8 or 16bits" << std::endl;
			return 0;
		}

		short *data = new short[MAX_BUFFER_LEN];
		int size = file.read(data, MAX_BUFFER_LEN);

		curr = { data, size, file.samplerate(), format };
		soundBuffers[filepath] = curr;
		std::cerr << "Generating new sound data!" << std::endl;
	}
	else
	{
		curr = (*it).second;
		std::cerr << "Reusing old sound data" << std::endl;
	}

	ALuint buffer;

	alGenBuffers((ALuint)1, &buffer);
	if (checkError("Error generating a new buffer data")) return NULL;

	alBufferData(buffer, curr.format, curr.data, curr.size, curr.frequency);
	std::cerr << "Buffered a file " << filepath << " of size " << curr.size << std::endl;
	if (checkError("Error buffering data")) return NULL;

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
	if (checkError("Error binding source to buffer")) { return; }
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
	if (checkError("Error detecting if sound is playing")) return false;
	
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
	if (checkError("Error pausing")) return;
	while (source_state == AL_PLAYING) {
		alGetSourcei(source, AL_SOURCE_STATE, &source_state);
		if (checkError("Error pausing")) return;
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

	const ALCchar *default_device = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	device = alcOpenDevice( default_device );
	if (!device)
	{
		std::cerr << "Bad audio input!" << std::endl;
	}
	else
	{
		std::cerr << "Playing default device: " << default_device << std::endl;
	}

	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context))
	{
		checkError("Error creating context");
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

	for (auto it : soundBuffers)
	{
		delete[] it.second.data;
	}

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

bool Sound::checkError(std::string comment)
{
	ALCenum error;

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		//std::cerr << alutGetErrorString(error) << std::endl;
		std::cerr << "checkError() returned an error: " << comment << std::endl;
		return true;
	}
	return false;
}


