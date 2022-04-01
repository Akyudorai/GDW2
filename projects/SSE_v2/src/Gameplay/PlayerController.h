#pragma once
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Light.h"
#include "Gameplay/Components/HealthComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"

#include "Gameplay/Components/GUI/GuiText.h"

#include "NOU/CMeshRenderer.h"
#include "NOU/Input.h"

#include "Components/IComponent.h"

class PlayerController : public Gameplay::IComponent
{
// IComponent
// =================================================================
public:
	typedef std::shared_ptr<PlayerController> Sptr;

	PlayerController();
	virtual ~PlayerController();

public:
	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	MAKE_TYPENAME(PlayerController);
	virtual nlohmann::json ToJson() const override;
	static PlayerController::Sptr FromJson(const nlohmann::json& blob);

// =================================================================

public:
	bool isShadow = false;
	bool shadowIsExtended = false;
	float movSpeed = 15.0f;

protected:
	Gameplay::GameObject::Sptr m_body = nullptr;
	Gameplay::GameObject::Sptr m_shadow = nullptr;
	Gameplay::GameObject::Sptr m_camera = nullptr;
	Gameplay::Physics::TriggerVolume::Sptr m_interaction = nullptr;
	//	Light::Sptr m_light = nullptr;

	float cameraLerpT = 1.0f;	
	glm::vec3 cameraOffset = glm::vec3(0.0f, -7.0f, 15.0f);
	glm::vec3 cameraRotation = glm::vec3(30.0f, 0.0f, 0.0f);
	//	glm::vec3 lightOffset = glm::vec3(0.0f, 0.0f, 10.0f);

private:
	bool gameStarted = false;

	float lerpT = 0.0f;
	glm::vec3 targetEuler = glm::vec3(0.0f, 0.0f, 0.0f);

public:
	inline void SetCharacterBody(Gameplay::GameObject::Sptr body) { m_body = body; };
	inline Gameplay::GameObject::Sptr GetCharacterBody() { return m_body; }
	inline void SetCharacterShadow(Gameplay::GameObject::Sptr shadow) { m_shadow = shadow; };
	inline Gameplay::GameObject::Sptr GetCharacterShadow() { return m_shadow; }
	inline void SetCamera(Gameplay::GameObject::Sptr camera) { m_camera = camera; }
	inline Gameplay::GameObject::Sptr GetCamera() { return m_camera; }
	inline void SetInteractionCollider(Gameplay::Physics::TriggerVolume::Sptr ic) { m_interaction = ic;  };
	inline Gameplay::Physics::TriggerVolume::Sptr GetInteractionCollider() { return m_interaction; }

protected:
	void HandleInput(float deltaTime);
	void HandleCamera(float deltaTime);

	// Lerp should be moved to some utility class somewhere
	template<typename T>
	T Lerp(const T& p0, const T& p1, float t) {
		return (1.0f - t) * p0 + t * p1;
	}
};


//class PlayerController
//{
//public:
//	enum Component
//	{
//		Body, Shadow, Camera, InteractionBox,
//		Light, BodyHealthUI, ShadowHealthUI
//	};
//
//public:
//	bool isShadow = false;
//	bool shadowIsExtended = false;
//
//public:
//	PlayerController();
//	~PlayerController();
//	void Initialize(
//		Gameplay::GameObject& body, Gameplay::GameObject& shadow,
//		Gameplay::GameObject& camera, Light::Sptr light,
//		Gameplay::Physics::TriggerVolume& interaction);
//	void Update(float deltaTime);
//
//protected:
//
//	// All our references that our Player Controller manipulates.
//	Gameplay::GameObject* m_body = nullptr;
//	Gameplay::GameObject* m_shadow = nullptr;
//	Gameplay::GameObject* m_camera = nullptr;
//	Light::Sptr m_light = nullptr;
//	Gameplay::Physics::TriggerVolume* m_interaction = nullptr;
//
//	float cameraLerpT = 1.0f;
//	float movSpeed = 15.0f;
//	glm::vec3 cameraOffset = glm::vec3(0.0f, -7.0f, 15.0f);
//	glm::vec3 cameraRotation = glm::vec3(30.0f, 0.0f, 0.0f);
//	glm::vec3 lightOffset = glm::vec3(0.0f, 0.0f, 10.0f);
//
//private:
//	bool gameStarted = false;
//	GameManager* gmRef;
//
//	float lerpT = 0.0f;
//	glm::vec3 targetEuler = glm::vec3(0.0f, 0.0f, 0.0f);
//
//protected:
//	void HandleInput(float deltaTime);
//	void HandleCamera(float deltaTime);
//
//Al
//
//	template<typename T>
//	T Lerp(const T& p0, const T& p1, float t) {
//		return (1.0f - t) * p0 + t * p1;
//	}
//};