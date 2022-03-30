#pragma once

#include "fmod.hpp"
#include <string>
#include <GLM/glm.hpp>
#include <vector>

#include "Gameplay/Components/IComponent.h"

struct AudioSettings
{
	bool PlayOnAwake;
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
	void Init(AudioSettings settings);
	void Play();
	void Play(std::string name);
	void Stop(bool bImmediate);

	inline void LoadEvent(std::string eventName) { strTargetEvent = eventName; }
	bool IsPlaying();

public:
	glm::vec3 m_Position;
	std::string strTargetEvent;

	bool playOnAwake;
	float volume = 1.0f;



};
