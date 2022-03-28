#pragma once
#include "Gameplay/GameObject.h"
#include "Gameplay/Light.h"
#include "Gameplay/Components/UpdatedBoxCollider.h"
#include "Gameplay/Components/HealthComponent.h"

#include "Gameplay/Components/GUI/GuiText.h"

#include "NOU/CMeshRenderer.h"
#include "NOU/Input.h"



class PlayerController
{
public:
	enum Component
	{
		Body, Shadow, Camera, InteractionBox,
		Light, BodyHealthUI, ShadowHealthUI
	};

public:
	bool isShadow = false;
	bool shadowIsExtended = false;
	float playerSpeed = 0.5f;

public:
	PlayerController();
	~PlayerController();
	void Initialize(
		Gameplay::GameObject& body, Gameplay::GameObject& shadow, 
		Gameplay::GameObject& camera, Gameplay::Light& light, 
		Gameplay::Physics::TriggerVolume& interaction);
	void Update(float deltaTime);	

	/*template<typename T>
	void SetComponent(Component pcComponent, T* ref);*/
	/*void SetBodyHealthUI(GuiText& text) {
		m_bodyHealthText = &text;
	}

	void SetShadowHealthUI(GuiText& text) {
		m_shadowHealthText = &text;
	}

	void SetPauseMenu(Gameplay::GameObject& object) {
		m_pauseMenu = &object;		
	}

	void SetGuideCanvas(Gameplay::GameObject& object) {
		m_guideCanvas = &object;
	}

	void SetMainMenuCanvas(Gameplay::GameObject& object) {
		m_mainCanvas = &object;
	}

	void SetLoseCanvas(Gameplay::GameObject& object) {
		m_loseCanvas = &object;
	}*/

protected:

	// All our references that our Player Controller manipulates.
	Gameplay::GameObject* m_body = nullptr;
	Gameplay::GameObject* m_shadow = nullptr;
	Gameplay::GameObject* m_camera = nullptr;
	Gameplay::Light* m_light = nullptr;
	Gameplay::Physics::TriggerVolume* m_interaction = nullptr;
	/*GuiText* m_bodyHealthText = nullptr;
	GuiText* m_shadowHealthText = nullptr;
	Gameplay::GameObject* m_pauseMenu = nullptr;
	Gameplay::GameObject* m_guideCanvas = nullptr;
	Gameplay::GameObject* m_mainCanvas = nullptr;
	Gameplay::GameObject* m_loseCanvas = nullptr;*/

	float cameraLerpT = 1.0f;	
	float movSpeed = 15.0f;
	glm::vec3 cameraOffset = glm::vec3(0.0f, -7.0f, 15.0f);
	glm::vec3 cameraRotation = glm::vec3(30.0f, 0.0f, 0.0f);
	glm::vec3 lightOffset = glm::vec3(0.0f, 0.0f, 10.0f);	

private:	
	bool gameStarted = false;

	float lerpT = 0.0f;
	glm::vec3 targetEuler = glm::vec3(0.0f, 0.0f, 0.0f);

protected:
	void HandleInput(float deltaTime);
	void HandleCamera(float deltaTime);



	template<typename T>
	T Lerp(const T& p0, const T& p1, float t) {
		return (1.0f - t) * p0 + t * p1;
	}
};