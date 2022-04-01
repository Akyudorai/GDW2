#pragma once
#include "IComponent.h"
#include "../Physics/ICollider.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/GameObject.h"

class SpikeTrapBehavior : public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<SpikeTrapBehavior> Sptr;

	SpikeTrapBehavior();
	virtual ~SpikeTrapBehavior();

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(SpikeTrapBehavior);
	virtual nlohmann::json ToJson() const override;
	static SpikeTrapBehavior::Sptr FromJson(const nlohmann::json& blob);

public:
	void Initialize(float spikeRate, float rateOffset, bool disabled);
	void SetAnimator(Gameplay::AnimatorComponent::Sptr animatorRef) { animator = animatorRef; animator->SetSpeed(4); }
	void SetTrigger(Gameplay::Physics::TriggerVolume::Sptr triggerRef) { trigger = triggerRef; }
	void Update(float deltaTime);

	inline void SetTarget(Gameplay::GameObject::Sptr object) { target = object; }
	inline Gameplay::GameObject::Sptr GetTarget() { return target; }

private:
	float currentTimer = 0.0f;
	Gameplay::GameObject::Sptr target;


	Gameplay::AnimatorComponent::Sptr animator = nullptr;
	Gameplay::Physics::TriggerVolume::Sptr trigger = nullptr;
	float attackRate = 3.0f;
	bool isExtended = false;
	bool isDisabled = false;
};