#pragma once


// FMOD
#include "fmod_studio.hpp"
#include "fmod_studio.h"
#include "fmod_errors.h"

// Standard Library
#include <string>
#include <unordered_map>
#include <vector>
#include <math.h>
#include <iostream>

// GLM
#include <GLM/glm.hpp>

using namespace FMOD;

// AUDIO OBJECT
// ==================================
class AudioObject
{
public:
	virtual ~AudioObject() {}

protected:
	int ErrorCheck(FMOD_RESULT result);

	float dbToVolume(float db);
	float VolumeToDb(float volume);
	FMOD_VECTOR VectorToFmod(const glm::vec3& vec);
	glm::vec3 FmodToVector(const FMOD_VECTOR& vec);
};

// AUDIO BUS
// ==================================
class AudioBus : public AudioObject
{
	friend class AudioEngine;

public:

	// Get and Set Paused
	bool CheckPaused();
	void SetPaused(const bool& pause);

	// Get and Set Volume
	float GetVolume();
	void SetVolume(const float& newVolume);

	// Get and Set Mute
	bool CheckMuted();
	void SetMute(const bool& mute);

	// Stops all events
	void StopAllEvents(const bool& fade = false);

private:
	// AudioEngine class uses this to create bus objects
	AudioBus(FMOD::Studio::Bus* bus);

	// Don't want copies, should only grab refs from audio engine.
	AudioBus(AudioBus const&) = delete;
	void operator=(AudioBus const&) = delete;

private:
	FMOD::Studio::Bus* m_Bus;
};

// AUDIO LISTENER
// ==================================
class AudioListener : public AudioObject
{
	friend class AudioEngine;

public:

	// Get and Set Position
	void SetPosition(const glm::vec3& pos);
	glm::vec3 GetPosition();

	// Get and Set Velocity
	void SetVelocity(const glm::vec3& vel);
	glm::vec3 GetVelocity();

	// Get and Set Forward Direction
	void SetForward(const glm::vec3& forward);
	glm::vec3 GetForward();

	// Get and Set Up Direction
	void SetUp(const glm::vec3& up);
	glm::vec3 GetUp();

private:

	// Only AudioEngine can create a listener
	// Get a ref from AudioEngine::GetListener()
	AudioListener() {}
	AudioListener(AudioListener const&) = delete;
	void operator=(AudioListener const&) = delete;

private:

	// Ref to FMOD System
	FMOD::Studio::System* m_StudioSystem;

	// Save the most recent changes
	FMOD_3D_ATTRIBUTES m_Attributes;
	FMOD_VECTOR m_AttenuationPosition;

	// Basic ID, first listener is 0
	int m_ID;
	void SetID(const int& id);
};

// AUDIO EVENT
// ==================================
class AudioEvent : public AudioObject
{
	friend class AudioEngine;

public:
	~AudioEvent();

	// Will only play if event is not currently playing
	void Play();

	// Restarts the event
	void Restart();

	// Allows AHDSR modulators to complete their release, and DSP effect tails to play out.
	void Stop();

	// Stops the event instance immediately.
	void StopImmediately();

	// Checks if event is playing
	bool IsPlaying();

	// Get and Set Paused
	bool CheckPaused();
	void SetPaused(const bool& pause);

	// Parameters
	void SetParameter(const char* name, const float& value, const bool& ignoreSeekSpeed = false);
	float GetParameterValue(const char* name);

	// Get and Set Position
	void SetPosition(const glm::vec3& pos);
	glm::vec3 GetPosition();

	// Get and Set Volume
	void SetVolume(const float& newVolume);
	float GetVolume();

private:

	// AudioEngine class uses this to create Event objects
	AudioEvent(FMOD::Studio::EventInstance* eventInstance);

	// Don't want copies, should only grab refs from Audio Engine
	AudioEvent(AudioEvent const&) = delete;
	void operator=(AudioEvent const&) = delete;

private:

	FMOD_3D_ATTRIBUTES m_Attributes;
	FMOD::Studio::EventInstance* m_EventInstance;
};

// AUDIO ENGINE
// ==================================
class AudioEngine : public AudioObject
{
	friend class AudioEvent;

public:

	// Singleton /////////////////////

	static AudioEngine& Instance();

	AudioEngine(AudioEngine const&) = delete;
	void operator=(AudioEngine const&) = delete;

	//////////////////////////////////

	void Init();
	void Update();
	void Shutdown();

	// Banks
	void LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);
	void LoadBankWithString(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);

	// Listener
	AudioListener& GetListener();

	// Events
	AudioEvent& CreateSoundEvent(const std::string& eventName, const std::string& GUID);
	AudioEvent& GetEvent(const std::string& eventName);
	inline std::unordered_map<std::string, AudioEvent*> GetEventsMap() { return m_EventMap; }

	// Global Params
	void SetGlobalParam(const char* name, const float& value, const bool& ignoreSeekSpeed = false);
	float GetGlobalParamValue(const char* name);

	// Bus
	void LoadBus(const std::string& busName, const std::string& GUID);
	AudioBus& GetBus(const std::string& busName);

	void PauseEverything();
	void UnPauseEverything();
	void StopAllSounds();

private:
	AudioEngine() {}

private:

	// FMOD Systems
	FMOD::Studio::System* m_StudioSystem;
	FMOD::System* m_System;

	// Listener
	AudioListener m_Listener;

	// Banks
	std::unordered_map<std::string, FMOD::Studio::Bank*> m_BankMap;

	// Events
	std::unordered_map<std::string, AudioEvent*> m_EventMap;

	// Bus
	std::unordered_map<std::string, AudioBus*> m_BusMap;
};