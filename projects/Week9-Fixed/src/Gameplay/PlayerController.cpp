#include "PlayerController.h"

#include "Scene.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"


PlayerController::PlayerController() : 
	m_body(nullptr), m_shadow(nullptr), m_camera(nullptr), m_light(nullptr)
{ }

PlayerController::~PlayerController()
{ }

void PlayerController::Initialize(
	Gameplay::GameObject& body, Gameplay::GameObject& shadow, 
	Gameplay::GameObject& camera, Gameplay::Light& light)
{
	m_body = &body;
	m_shadow = &shadow;
	m_camera = &camera;
	m_light = &light;
}

void PlayerController::Update(float deltaTime)
{
	HandleInput(deltaTime);
	HandleCamera(deltaTime);	
}

void PlayerController::HandleInput(float deltaTime)
{	
	GLFWwindow* windowRef = m_camera->GetScene()->Window;

	glm::vec3 motion = glm::vec3(0);	
	if (glfwGetKey(windowRef, GLFW_KEY_W)) { motion += glm::vec3(0, 1, 0); }
	if (glfwGetKey(windowRef, GLFW_KEY_S)) { motion -= glm::vec3(0, 1, 0); }
	if (glfwGetKey(windowRef, GLFW_KEY_A)) { motion -= glm::vec3(1, 0, 0); }
	if (glfwGetKey(windowRef, GLFW_KEY_D)) { motion += glm::vec3(1, 0, 0); }

	if (motion != glm::vec3(0)) {
		if (!isShadow) {
			m_body->SetPosition(m_body->GetPosition() + motion * movSpeed * deltaTime);
			if (glm::distance(m_body->GetPosition(), m_shadow->GetPosition()) >= 21.0f) {						
				m_shadow->Get<RenderComponent>()->IsEnabled = false;
				m_shadow->Get<Gameplay::Physics::RigidBody>()->IsEnabled = false;
			}
		}
		else if (isShadow && glm::distance(m_shadow->GetPosition() + motion, m_body->GetPosition()) <= 20.0f) {
			m_shadow->SetPosition(m_shadow->GetPosition() + motion * movSpeed * deltaTime);
		}
	}

	if (glfwGetKey(windowRef, GLFW_KEY_Q))
	{		
		// Extend Shadow		
		isShadow = !isShadow;
		cameraLerpT = 0;

		m_light->Range = (isShadow) ? 50.0f : 200.0f;
		//m_light->Range = Lerp(m_light->Range, (isShadow) ? 50.0f : 200.0f, cameraLerpT);

		if (isShadow && !m_shadow->Get<RenderComponent>()->IsEnabled) {
			m_shadow->SetPosition(m_body->GetPosition());
			m_shadow->Get<RenderComponent>()->IsEnabled = true;	
			m_shadow->Get<Gameplay::Physics::RigidBody>()->IsEnabled = true;
			shadowIsExtended = true;
		}

		if (!isShadow && glm::distance(m_body->GetPosition(), m_shadow->GetPosition()) <= 5.0f)
		{
			m_shadow->Get<RenderComponent>()->IsEnabled = false;	
			m_shadow->Get<Gameplay::Physics::RigidBody>()->IsEnabled = false;
			shadowIsExtended = false;
		}
	}

	if (glfwGetKey(windowRef, GLFW_KEY_E)) {
		// Ability 
		LOG_INFO("Ability Activated");		
	}

	if (glfwGetKey(windowRef, GLFW_KEY_F)) {
		// Shadow Swap
		if (shadowIsExtended) {
			glm::vec3 temp = m_body->GetPosition();
			m_body->SetPosition(m_shadow->GetPosition());
			m_shadow->SetPosition(temp);
		}
		
	}

	if (glfwGetKey(windowRef, GLFW_KEY_R)) {
		// Recall
		if (!isShadow) {
			m_shadow->Get<RenderComponent>()->IsEnabled = false;
			m_shadow->Get<Gameplay::Physics::RigidBody>()->IsEnabled = false;
			shadowIsExtended = false;
		}		
	}
}

void PlayerController::HandleCamera(float deltaTime)
{
	if (cameraLerpT < 1.0f)
		cameraLerpT += deltaTime * 0.1f;
	else if (cameraLerpT > 1.0f)
		cameraLerpT = 1.0f;

	m_camera->SetPosition(Lerp(m_camera->GetPosition(), ((isShadow) ? m_shadow->GetPosition() : m_body->GetPosition()) + cameraOffset, cameraLerpT));	
	m_camera->SetRotation(cameraRotation);

	m_light->Position = Lerp(m_light->Position, ((isShadow) ? m_shadow->GetPosition() : m_body->GetPosition()) + lightOffset, cameraLerpT);
	m_camera->GetScene()->SetupShaderAndLights();
}