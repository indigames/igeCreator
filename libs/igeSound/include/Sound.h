#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <map>
#include <string>

#ifdef _WIN32
#define PYXIE_EXPORT __declspec(dllexport)
#else
#define PYXIE_EXPORT
#endif

#ifdef NDEBUG
#define LOG_VERBOSE(...)
#define LOG_DEBUG(...)
#define LOG(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#else
#ifdef _ANDROID_
#define LOG_VERBOSE(...) __android_log_print(ANDROID_LOG_VERBOSE, "Sound", __VA_ARGS__);
#define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, "Sound", __VA_ARGS__);
#define LOG(...) __android_log_print(ANDROID_LOG_INFO, "Sound", __VA_ARGS__);
#define LOG_WARN(...) __android_log_print(ANDROID_LOG_WARN, "Sound", __VA_ARGS__);
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, "Sound", __VA_ARGS__);
#else
#define LOG_VERBOSE(...) printf(__VA_ARGS__);
#define LOG_DEBUG(...) printf(__VA_ARGS__);
#define LOG(...) printf(__VA_ARGS__);
#define LOG_WARN(...) printf(__VA_ARGS__);
#define LOG_ERROR(...) printf(__VA_ARGS__);
#endif
#endif

#include "soloud.h"

using namespace SoLoud;

class PYXIE_EXPORT Sound
{
public:
	Sound();
	~Sound();
	void init();
	void release();
	void update();
	int play(const char* filename, bool stream, bool loop, bool is_3d, float x, float y, float z, float volume = -1.0f);
	int playPreset(int sfx_preset, bool is_loop, bool is_3d, float x, float y, float z, float volume = -1.0f);
	void stop(const char* filename);
	void stop(int handle);
	void stopAllSound();
	void load(const char* filename, bool stream);
	void unload(const char* filename);
	void setGlobalVolume(float volume);
	float getGlobalVolume();
	void fadeVolume(int handle, float aTo, float aTime);
	void scheduleStop(int handle, float aTime);
	double getStreamTime(int handle);
	void set3dSourcePosition(int handle, float x, float y, float z);
	void set3dMinMaxDistance(int handle, float min, float max);
	void set3dAttenuation(int handle, int attenuationModel, float rolloffFactor);
	
protected:
	bool isInitialized() { return initialized; }
	void releaseAllSound();

private:
	bool initialized;
	std::map<std::string, AudioSource*> m_audioSourcesDict;
	Soloud *m_soloud;

	static Sound* instance;
public:
	static Sound* Instance()
	{
		if (!instance)
		{
			instance = new Sound();
		}
			
		return instance;
	}
};