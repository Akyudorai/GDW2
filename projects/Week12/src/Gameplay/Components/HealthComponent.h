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
	
	float GetCurrentHealth() {

		return currentHealth;

		/*if (dp > 0) {
			float val = (int(currentHealth * std::pow(10, dp) + 0.5f));
			return (float)val / std::pow(10, dp);
		}
		else {
			return std::ceil(currentHealth);
		}*/
		
	}
};