#include "PlayerController.h"

#include "Scene.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/InteractableComponent.h"


PlayerController::PlayerController() :
	m_body(nullptr), m_shadow(nullptr), m_camera(nullptr), m_light(nullptr), m_interaction(nullptr),
	m_bodyHealthText(nullptr), m_shadowHealthText(nullptr), m_pauseMenu(nullptr)
{ }

PlayerController::~PlayerController()
{ }

void PlayerController::Initialize(
	Gameplay::GameObject& body, Gameplay::GameObject& shadow, 
	Gameplay::GameObject& camera, Gameplay::Light& light,
	Gameplay::Physics::TriggerVolume& interaction)
{
	m_body = &body;
	m_shadow = &shadow;
	m_camera = &camera;
	m_light = &light;
	m_interaction = &interaction;
}

void PlayerController::Update(float deltaTime)
{
	// If the player presses the Tab key, it pauses the game.  See Scene.cpp at line 
	if (glfwGetKey(m_camera->GetScene()->Window, GLFW_KEY_TAB)) {
		// Pause
		if (m_pauseMenu != nullptr) {
			m_pauseMenu->IsActive = !m_pauseMenu->IsActive;
			Gameplay::Scene::IsPaused = !Gameplay::Scene::IsPaused;
		}
	}

	// If we're paused, we dont want our player to be able to do anything else after this point
	if (Gameplay::Scene::IsPaused) return;

	HandleInput(deltaTime);
	HandleCamera(deltaTime);	

	// Change the Health Text on screen if we have a reference to it
	if (m_bodyHealthText != nullptr && m_body != nullptr) {	
		m_bodyHealthText->SetText(("Body: " + std::to_string(m_body->Get<HealthComponent>()->GetCurrentHealth())));
	}

	// Change the Health Text on screen if we have a reference to it
	if (m_shadowHealthText != nullptr && m_shadow != nullptr) {
		m_shadowHealthText->SetText(("Shadow: " + std::to_string(m_shadow->Get<HealthComponent>()->GetCurrentHealth())));
	}
}

void PlayerController::HandleInput(float deltaTime)
{	
	GLFWwindow* windowRef = m_camera->GetScene()->Window;

	glm::vec3 motion = glm::vec3(0);	
	if (glfwGetKey(windowRef, GLFW_KEY_W)) { motion += glm::vec3(0, 0.5, 0); }
	if (glfwGetKey(windowRef, GLFW_KEY_S)) { motion -= glm::vec3(0, 0.5, 0); }
	if (glfwGetKey(windowRef, GLFW_KEY_A)) { motion -= glm::vec3(0.5, 0, 0); }
	if (glfwGetKey(windowRef, GLFW_KEY_D)) { motion += glm::vec3(0.5, 0, 0); }

	if (motion != glm::vec3(0)) {
		if (!isShadow) {
			m_body->SetPosition(m_body->GetPosition() + motion * movSpeed * deltaTime);
			m_body->LookAt(m_body->GetPosition() + motion);
			if (glm::distance(m_body->GetPosition(), m_shadow->GetPosition()) >= 21.0f) {						
				m_shadow->Get<RenderComponent>()->IsEnabled = false;
				m_shadow->Get<Gameplay::Physics::RigidBody>()->IsEnabled = false;
			}
		}
		else if (isShadow && glm::distance(m_shadow->GetPosition() + motion, m_body->GetPosition()) <= 20.0f) {
			m_shadow->SetPosition(m_shadow->GetPosition() + motion * movSpeed * deltaTime);
			m_shadow->LookAt(m_shadow->GetPosition() + motion);
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
			m_shadow->Get<Gameplay::Physics::RigidBody>()->SetCollisionGroup(0x02);			// SHADOW COLLISION
			m_shadow->Get<Gameplay::Physics::RigidBody>()->SetCollisionMask(0xFFFFFFFE);	// SHADOW COLLISION
			shadowIsExtended = true;
		}

		if (!isShadow && glm::distance(m_body->GetPosition(), m_shadow->GetPosition()) <= 5.0f)
		{
			m_shadow->Get<RenderComponent>()->IsEnabled = false;	
			m_shadow->Get<Gameplay::Physics::RigidBody>()->IsEnabled = false;
			m_shadow->Get<Gameplay::Physics::RigidBody>()->SetCollisionGroup(0x03);			// NO COLLISION
			m_shadow->Get<Gameplay::Physics::RigidBody>()->SetCollisionMask(0xFFFFFFFD);	// NO COLLISION
			shadowIsExtended = false;
		}
	}

	if (glfwGetKey(windowRef, GLFW_KEY_E)) {
		// Ability 				
		for (auto& object : m_interaction->_currentCollisions) {			
			bool interactable = object.lock()->GetGameObject()->Has<InteractableComponent>();
			if (interactable && object.lock()->GetGameObject()->Get<InteractableComponent>()->onInteractionEvent) {				
				object.lock()->GetGameObject()->Get<InteractableComponent>()->onInteractionEvent();
			}
		}
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
			m_shadow->Get<Gameplay::Physics::RigidBody>()->SetCollisionGroup(0x03);			// NO COLLISION
			m_shadow->Get<Gameplay::Physics::RigidBody>()->SetCollisionMask(0xFFFFFFFD);	// NO COLLISION
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

//template<typename T>
//void PlayerController::SetComponent(Component pcComponent, T* ref)
//{
//	switch (pcComponent) 
//	{
//	case PlayerControllerComponent::Body:		
//		/*if (std::is_same_v<T, Gameplay::GameObject>) {
//			m_body = &ref;
//		}
//		else {
//			LOG_ERROR("PlayerController::SetComponent -- Type Mismatch")
//		}*/		
//		break;
//	case PlayerControllerComponent::Shadow:
//		/*if (std::is_same_v< T, Gameplay::GameObject>) {
//			m_body = &ref;
//		}
//		else {
//			LOG_ERROR("PlayerController::SetComponent -- Type Mismatch")
//		}*/
//		break;
//	case PlayerControllerComponent::Camera:
//		/*if (std::is_same_v<T, Gameplay::GameObject>) {
//			m_body = &ref;
//		}
//		else {
//			LOG_ERROR("PlayerController::SetComponent -- Type Mismatch")
//		}*/
//		break;
//	case PlayerControllerComponent::InteractionBox:
//		/*if (std::is_same_v < T, Gameplay::Physics::TriggerVolume) {
//			m_body = &ref;
//		}
//		else {
//			LOG_ERROR("PlayerController::SetComponent -- Type Mismatch")
//		}*/
//		break;
//	case PlayerControllerComponent::Light:
//		/*if (std::is_same_v<T, Gameplay::Light>) {
//			m_body = &ref;
//		}
//		else {
//			LOG_ERROR("PlayerController::SetComponent -- Type Mismatch")
//		}*/
//		break;
//	case PlayerControllerComponent::BodyHealthUI:
//		/*if (std::is_same_v<T, GuiText>) {
//			m_body = &ref;
//		}
//		else {
//			LOG_ERROR("PlayerController::SetComponent -- Type Mismatch")
//		}*/
//		break;
//	case PlayerControllerComponent::ShadowHealthUI:
//		/*if (std::is_same_v<T, GuiText>) {
//			m_body = &ref;
//		}
//		else {
//			LOG_ERROR("PlayerController::SetComponent -- Type Mismatch")
//		}*/
//		break;
//
//	default:
//		LOG_ERROR("NO SUCH COMPONENT IN PLAYERCONTROLLER.H");
//		break;
//	}
//}