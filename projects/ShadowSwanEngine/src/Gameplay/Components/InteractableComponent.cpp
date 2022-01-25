#include "InteractableComponent.h"
#include "Utils/ImGuiHelper.h"

void InteractableComponent::RenderImGui()
{ }

nlohmann::json InteractableComponent::ToJson() const
{
	return {};
}

InteractableComponent::InteractableComponent()
{ }

InteractableComponent::~InteractableComponent() = default;

InteractableComponent::Sptr InteractableComponent::FromJson(const nlohmann::json & blob)
{
	InteractableComponent::Sptr result = std::make_shared<InteractableComponent>();
	return result;
}