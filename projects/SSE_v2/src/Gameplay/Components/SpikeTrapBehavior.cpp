#include "SpikeTrapBehavior.h"
#include "Utils/ImGuiHelper.h"

#include "Gameplay/GameObject.h"
#include "../Physics/TriggerVolume.h"
#include "AnimatorComponent.h"
#include "AudioSource.h"

#include "Audio/AudioManager.h"

using namespace Gameplay;

void SpikeTrapBehavior::RenderImGui()
{ }

nlohmann::json SpikeTrapBehavior::ToJson() const
{
	return {};
}

SpikeTrapBehavior::SpikeTrapBehavior()
{ }

SpikeTrapBehavior::~SpikeTrapBehavior() = default;

SpikeTrapBehavior::Sptr SpikeTrapBehavior::FromJson(const nlohmann::json & blob)
{
	SpikeTrapBehavior::Sptr result = std::make_shared<SpikeTrapBehavior>();
	return result;
}

void SpikeTrapBehavior::Initialize(float spikeRate, float rateOffset, bool disabled)
{
	attackRate = spikeRate;
	currentTimer = spikeRate + rateOffset;
	isExtended = false;
	isDisabled = disabled;
}

void SpikeTrapBehavior::Update(float deltaTime)
{
	// Do nothing if disabled
	if (isDisabled) return;

	// Count Down Timer
	if (currentTimer > 0.0f)
	{
		currentTimer -= deltaTime;
	}

	// When Timer Hits Zero
	else if (currentTimer <= 0.0f)
	{
		// Retract Spikes
		if (isExtended)
		{			
			trigger->IsEnabled = false;
			currentTimer = attackRate;
			isExtended = false;

			animator->SetReverse(true);
			//animator->Play();			
		}

		// Extend Spikes
		else
		{
			this->GetGameObject()->Get<AudioSource>()->Play("Spikes");
			trigger->IsEnabled = true;
			currentTimer = attackRate;
			isExtended = true;

			animator->SetReverse(false);
			//animator->Play();
		}
	}
}