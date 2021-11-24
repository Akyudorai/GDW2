#pragma once
#include "Gameplay/GameObject.h"
#include "Gameplay/Light.h"
#include "Gameplay/Components/UpdatedBoxCollider.h"
#include "Gameplay/Components/HealthComponent.h"

#include "NOU/CMeshRenderer.h"
#include "NOU/Input.h"


class PlayerController
{
public:
	bool isShadow = false;
	bool shadowIsExtended = false;

public:
	PlayerController();
	~PlayerController();
	void Initialize(
		Gameplay::GameObject& body, Gameplay::GameObject& shadow, 
		Gameplay::GameObject& camera, Gameplay::Light& light);
	void Update(float deltaTime);	

protected:
	Gameplay::GameObject* m_body = nullptr;
	Gameplay::GameObject* m_shadow = nullptr;
	Gameplay::GameObject* m_camera = nullptr;
	Gameplay::Light* m_light = nullptr;

	float cameraLerpT = 1.0f;	
	float movSpeed = 15.0f;
	glm::vec3 cameraOffset = glm::vec3(0.0f, -7.0f, 15.0f);
	glm::vec3 cameraRotation = glm::vec3(30.0f, 0.0f, 0.0f);
	glm::vec3 lightOffset = glm::vec3(0.0f, 0.0f, 10.0f);	

protected:
	void HandleInput(float deltaTime);
	void HandleCamera(float deltaTime);

	template<typename T>
	T Lerp(const T& p0, const T& p1, float t) {
		return (1.0f - t) * p0 + t * p1;
	}
};