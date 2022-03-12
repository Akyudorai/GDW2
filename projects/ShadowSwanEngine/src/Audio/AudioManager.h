#pragma once

#include "fmod.hpp"
#include "fmod_studio.hpp"
#include <GLM/glm.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace FMOD;


#include "AudioSource.h"
#include "AudioResource.h"

class AudioManager
{
public:
	~AudioManager() = default;

	static AudioManager& instance() { static AudioManager audioManager; return audioManager; }
	static bool Init();
	static void Update();
	static void Shutdown();
	static int ErrorCheck(FMOD_RESULT result, int line);

	static Sound* LoadSound(AudioResource::Sptr resource, AudioSettings settings);
	static Sound* LoadSound(std::string path, AudioSettings settings = { false, false, false });
	static void SetMode(Sound* sound, AudioSettings settings);

	static void SetListener(const FMOD_VECTOR* position, const FMOD_VECTOR* velocity, const FMOD_VECTOR* forward, const FMOD_VECTOR* up);

	void Play(std::string path);
	void PlaySource(AudioSource* source, float fVolume = 1.0f);
	void StopSource(AudioSource* source);

	void ClearSounds();

private:
	AudioManager() {}
	float dbToVolume(float db);
	float VolumeToDb(float volume);
	FMOD_VECTOR VectorToFmod(const glm::vec3& vPosition);

private:
	FMOD::System* fmSystem;
	std::vector<AudioSource*> CurrentSounds;
};