#include "HealthComponent.h"

#include "Utils/ImGuiHelper.h"

void HealthComponent::Awake()
{

}

void HealthComponent::RenderImGui()
{
	LABEL_LEFT(ImGui::SliderFloat, "Health", &currentHealth, 0, maximumHealth);
}

nlohmann::json HealthComponent::ToJson() const
{
	return {
		{ "current", currentHealth },
		{ "maximum", maximumHealth }
	};
}

HealthComponent::HealthComponent() : currentHealth(100), maximumHealth(100)
{ }

HealthComponent::HealthComponent(float maxHealth) : currentHealth(maxHealth), maximumHealth(maxHealth)
{ }

HealthComponent::~HealthComponent() = default;

HealthComponent::Sptr HealthComponent::FromJson(const nlohmann::json & blob)
{
	HealthComponent::Sptr result = std::make_shared<HealthComponent>();
	result->currentHealth = blob["current"];
	result->maximumHealth = blob["maximum"];
	return result;
}

void HealthComponent::DealDamage(float amount)
{
	currentHealth -= amount;

	if (currentHealth <= 0) {
		LOG_WARN("Health <= 0!!");
		// Destroy the game object
	}
}

void HealthComponent::Heal(float amount)
{
	currentHealth += amount;
	if (currentHealth > 100) {
		currentHealth = maximumHealth;
	}
}
