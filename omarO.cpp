#include <iostream>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include </usr/include/AL/alut.h>
using namespace std;
int init_sound(ALuint &buffer, ALuint &source)
{
	//MAKE ARRAY OF FILE NAMES for when we use more sounds
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) {
		cout << "ERROR: alutInit()\n";
		return 0;
	}
	//Clear error state.
	alGetError();
	//
	//Setup the listener.
	//Forward and up vectors are used.
	float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListenerf(AL_GAIN, 1.0f);
	//
	//Buffer holds the sound information.
	//ALuint alBuffer;
	//alBuffer = alutCreateBufferFromFile("./sounds/test.wav");
	buffer = alutCreateBufferFromFile("./sounds/test.wav");
	//
	//Source refers to the sound.
	//ALuint alSource;
	//Generate a source, and store it in a buffer.
	//alGenSources(1, &alSource);
	alGenSources(1, &source);
	//alSourcei(alSource, AL_BUFFER, alBuffer);
	alSourcei(source, AL_BUFFER, buffer);
	//Set volume and pitch to normal, no looping of sound.
	alSourcef(source, AL_GAIN, 1.0f);
	alSourcef(source, AL_PITCH, 1.0f);
	alSourcei(source, AL_LOOPING, AL_FALSE);
	if (alGetError() != AL_NO_ERROR) {
		cout << "ERROR: setting source\n";
		return 0;
	}
	return 0;
}
void play_sound(ALuint &source)
{
		alSourcePlay(source);
		//usleep(200000);
}
int clean_sound(ALuint &buffer, ALuint &source)
{

	//Cleanup.
	//First delete the source.
	//alDeleteSources(1, &alSource);
	alDeleteSources(1, &source);
	//Delete the buffer.
	alDeleteBuffers(1, &buffer);
	//Close out OpenAL itself.
	//Get active context.
	ALCcontext *Context = alcGetCurrentContext();
	//Get device for active context.
	ALCdevice *Device = alcGetContextsDevice(Context);
	//Disable context.
	alcMakeContextCurrent(NULL);
	//Release context(s).
	alcDestroyContext(Context);
	//Close device.
	alcCloseDevice(Device);
	return 0;
}
