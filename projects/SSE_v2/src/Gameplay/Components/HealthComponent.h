#pragma once
#include "IComponent.h"

class HealthComponent : public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<HealthComponent> Sptr;

	HealthComponent();
	HealthComponent(float maxHealth);
	virtual ~HealthComponent();

	virtual void Awake() override;


public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(HealthComponent);
	virtual nlohmann::json ToJson() const override;
	static HealthComponent::Sptr FromJson(const nlohmann::json& blob);

protected:
	// variables
	float currentHealth, maximumHealth;


public:
	void DealDamage(float amount);
	void Heal(float amount);

	inline float GetCurrentHealth() { return currentHealth; }
};