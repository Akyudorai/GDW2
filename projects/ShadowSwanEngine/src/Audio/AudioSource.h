#pragma once

#include "fmod.hpp"
#include <string>
#include <GLM/glm.hpp>
#include <vector>

#include "AudioResource.h"
#include "Gameplay/Components/IComponent.h"

//class AudioManager;
//class AudioResource;

struct AudioSettings
{
	bool Is3D = false;
	bool IsLooping = false;
	bool IsStream = false;
};

class AudioSource : public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<AudioSource> Sptr;

	AudioSource();

	//IComponent
	virtual void Awake();
	virtual void Update(float deltaTime);
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static AudioSource::Sptr FromJson(const nlohmann::json& data);
	MAKE_TYPENAME(AudioSource);

public:
	void Init();
	void Play();
	void Play(std::string source, AudioSettings* settings = nullptr);
	void Stop();

public:
	bool IsPlaying = false;
	AudioResource::Sptr m_Resource;
	AudioSettings m_Settings;

	bool playOnAwake;
	FMOD::Channel* m_Channel;
	glm::vec3 m_Position;
	float volume = 1.0f;

private:
	//friend AudioManager;
	
};