#pragma once
#include "IComponent.h"
#include <vector>

class InteractableComponent : public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<InteractableComponent> Sptr;

	InteractableComponent();
	virtual~InteractableComponent();

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(InteractableComponent);
	virtual nlohmann::json ToJson() const override;
	static InteractableComponent::Sptr FromJson(const nlohmann::json& blob);

public:
	std::vector<std::function<void()>> onInteractionEvent;
	bool isToggled;
};
