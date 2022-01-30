#pragma once
#include "IComponent.h"

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
	std::function<void()> onInteractionEvent;
	bool isToggled;
};
