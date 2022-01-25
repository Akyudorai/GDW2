#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"

TriggerVolumeEnterBehaviour::TriggerVolumeEnterBehaviour() :
	IComponent()
{ }
TriggerVolumeEnterBehaviour::~TriggerVolumeEnterBehaviour() = default;

void TriggerVolumeEnterBehaviour::Awake()
{
	_playerInTrigger = false;
}

void TriggerVolumeEnterBehaviour::Update(float dt)
{
	if (_playerInTrigger && onTriggerStayEvent) {
		onTriggerStayEvent();
	}
}

void TriggerVolumeEnterBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	//LOG_INFO("Body has entered our trigger volume: {}", body->GetGameObject()->Name);
	
	if (onTriggerEnterEvent) onTriggerEnterEvent();
	_playerInTrigger = true;
}

void TriggerVolumeEnterBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	//LOG_INFO("Body has left our trigger volume: {}", body->GetGameObject()->Name);
	if (onTriggerExitEvent) onTriggerExitEvent();
	_playerInTrigger = false;
}

void TriggerVolumeEnterBehaviour::RenderImGui() { }

nlohmann::json TriggerVolumeEnterBehaviour::ToJson() const {
	return { };
}

TriggerVolumeEnterBehaviour::Sptr TriggerVolumeEnterBehaviour::FromJson(const nlohmann::json& blob) {
	TriggerVolumeEnterBehaviour::Sptr result = std::make_shared<TriggerVolumeEnterBehaviour>();
	return result;
}
