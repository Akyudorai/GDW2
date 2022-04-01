#include "PlayerController.h"

#include "Scene.h"
#include "InputEngine.h"
#include "Audio/AudioManager.h"
#include "GameManager.h"

// Components
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/InteractableComponent.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/AudioSource.h"

// Utils
#include "Utils/ResourceManager/Resources.h"

// Debugging
#include "Logging.h"

PlayerController::PlayerController() :
	m_body(nullptr), m_shadow(nullptr), m_camera(nullptr), m_interaction(nullptr)//, m_light(nullptr),
{ }

PlayerController::~PlayerController()
{ }


void PlayerController::Update(float deltaTime)
{
	if (m_body == nullptr || m_shadow == nullptr || m_camera == nullptr) return;

	AudioEngine::Instance().GetListener().SetPosition((isShadow) ? m_shadow->GetPosition() : m_body->GetPosition());
	AudioEngine::Instance().GetListener().SetVelocity(glm::vec3(0, 0, 0));
	AudioEngine::Instance().GetListener().SetForward(glm::vec3(0, 0, 1));
	AudioEngine::Instance().GetListener().SetUp(glm::vec3(0, 1, 0));

	if (lerpT < 1.0f)
		lerpT += deltaTime;
	else
		lerpT = 1.0f;

	// Detect for Death
	if (m_body->Get<HealthComponent>()->GetCurrentHealth() <= 0 || m_shadow->Get<HealthComponent>()->GetCurrentHealth() <= 0)
	{
		if (GameManager::GetInstance().GameInterface.m_LosePanel != nullptr)
		{
			GameManager::GetInstance().GameInterface.ToggleLosePanel(true);
			GameManager::GetInstance().SetPaused(true);
		}
	}


	// If the player presses the Tab key, it pauses the game.  See Scene.cpp at line 
	if (InputEngine::GetKeyState(GLFW_KEY_TAB) == ButtonState::Pressed)
	{
		if (GameManager::GetInstance().GameInterface.m_PauseMenuPanel != nullptr) {
			GameManager::GetInstance().SetPaused(!GameManager::GetInstance().IsPaused());
			GameManager::GetInstance().GameInterface.TogglePausePanel(GameManager::GetInstance().IsPaused());
		}
	}


	// If we're paused, we dont want our player to be able to do anything else after this point
	if (GameManager::GetInstance().IsPaused()) return;

	HandleInput(deltaTime);
	HandleCamera(deltaTime);

	// Change the Health Text on screen if we have a reference to it
	if (GameManager::GetInstance().GameInterface.m_bodyHealthDisplay != nullptr && m_body != nullptr)
	{
		GameManager::GetInstance().GameInterface.m_bodyHealthDisplay->SetText(("Body: " + std::to_string(m_body->Get<HealthComponent>()->GetCurrentHealth())));
	}

	if (GameManager::GetInstance().GameInterface.m_shadowHealthDisplay != nullptr && m_body != nullptr)
	{
		GameManager::GetInstance().GameInterface.m_shadowHealthDisplay->SetText(("Shadow: " + std::to_string(m_shadow->Get<HealthComponent>()->GetCurrentHealth())));
	}
}

void PlayerController::HandleInput(float deltaTime)
{
	glm::vec3 motion = glm::vec3(0);

	if (InputEngine::IsKeyDown(GLFW_KEY_W)) { motion += glm::vec3(0, 0.5, 0); }
	if (InputEngine::IsKeyDown(GLFW_KEY_S)) { motion -= glm::vec3(0, 0.5, 0); }
	if (InputEngine::IsKeyDown(GLFW_KEY_A)) { motion -= glm::vec3(0.5, 0, 0); }
	if (InputEngine::IsKeyDown(GLFW_KEY_D)) { motion += glm::vec3(0.5, 0, 0); }

	if (motion != glm::vec3(0)) {
		if (!isShadow) {

			// Change Animation to Walk
			if (m_body->Get<Gameplay::AnimatorComponent>()->currentAnimation != "Walk") {
				m_body->Get<Gameplay::AnimatorComponent>()->SetAnimation("Walk");
			}

			m_body->SetPosition(m_body->GetPosition() + motion * movSpeed * deltaTime);
			m_body->LookAt(m_body->GetPosition() + motion);

			if (!AudioEngine::Instance().GetEvent("Walk").IsPlaying())
			{
				//m_body->Get<AudioSource>()->Play("Walk");
				AudioEngine::Instance().GetEvent("Walk").SetPosition(m_body->GetPosition());
				AudioEngine::Instance().GetEvent("Walk").Play();

			}

			if (glm::distance(m_body->GetPosition(), m_shadow->GetPosition()) >= 21.0f) {
				m_shadow->Get<RenderComponent>()->IsEnabled = false;
				//m_shadow->Get<Gameplay::Physics::RigidBody>()->IsEnabled = false;
			}
		}
		else if (isShadow && glm::distance(m_shadow->GetPosition() + motion, m_body->GetPosition()) <= 20.0f) {
			m_shadow->SetPosition(m_shadow->GetPosition() + motion * movSpeed * deltaTime);
			m_shadow->LookAt(m_shadow->GetPosition() + motion);

			// Change Animation To Walk
			if (m_shadow->Get<Gameplay::AnimatorComponent>()->currentAnimation != "Walk") {
				m_shadow->Get<Gameplay::AnimatorComponent>()->SetAnimation("Walk");
			}

			if (!AudioEngine::Instance().GetEvent("Walk").IsPlaying())
			{
				//m_shadow->Get<AudioSource>()->Play("Walk");
				AudioEngine::Instance().GetEvent("Walk").SetPosition(m_shadow->GetPosition());
				AudioEngine::Instance().GetEvent("Walk").Play();
			}

		}
	}
	else {
		if (!isShadow) {
			// Change animation to Idle
			if (m_body->Get<Gameplay::AnimatorComponent>()->currentAnimation != "Idle") {
				m_body->Get<Gameplay::AnimatorComponent>()->SetAnimation("Idle");
			}
			//m_body->Get<AudioSource>()->Stop();
			AudioEngine::Instance().GetEvent("Walk").Stop();
		}
		else {
			// Change animation to Idle
			if (m_shadow->Get<Gameplay::AnimatorComponent>()->currentAnimation != "Idle") {
				m_shadow->Get<Gameplay::AnimatorComponent>()->SetAnimation("Idle");
			}
			//m_shadow->Get<AudioSource>()->Stop();
			AudioEngine::Instance().GetEvent("Walk").Stop();
		}
	}

	// JUMP
	if (InputEngine::GetKeyState(GLFW_KEY_SPACE) == ButtonState::Pressed)
	{
		if (!isShadow) {
			m_body->Get<Gameplay::Physics::RigidBody>()->ApplyImpulse(glm::vec3(0.0f, 0.0f, 5.5f));
			//AudioManager::instance().Play("audio/Jump.wav");
			AudioEngine::Instance().GetEvent("Jump").SetPosition(m_body->GetPosition());
			AudioEngine::Instance().GetEvent("Jump").Play();
		}
		else {
			m_shadow->Get<Gameplay::Physics::RigidBody>()->ApplyImpulse(glm::vec3(0.0f, 0.0f, 5.5f));
			//AudioManager::instance().Play("audio/Jump.wav");
			AudioEngine::Instance().GetEvent("Jump").SetPosition(m_shadow->GetPosition());
			AudioEngine::Instance().GetEvent("Jump").Play();
		}
	}

	// SHADOW EXTENSION
	if (InputEngine::GetKeyState(GLFW_KEY_Q) == ButtonState::Pressed)
	{
		// Extend Shadow		
		isShadow = !isShadow;
		cameraLerpT = 0;

		//m_light->SetRadius((isShadow) ? 10.0f : 100.0f);

		if (isShadow && !m_shadow->Get<RenderComponent>()->IsEnabled) {
			m_shadow->SetPosition(m_body->GetPosition());
			m_shadow->Get<RenderComponent>()->IsEnabled = true;
			shadowIsExtended = true;
		}

		if (!isShadow && glm::distance(m_body->GetPosition(), m_shadow->GetPosition()) <= 5.0f)
		{
			m_shadow->Get<RenderComponent>()->IsEnabled = false;
			shadowIsExtended = false;
		}
	}

	// INTERACT
	if (InputEngine::GetKeyState(GLFW_KEY_E) == ButtonState::Pressed) {

		// Ability 				
		for (auto& object : m_interaction->GetCollisions()) {
			bool interactable = object.lock()->GetGameObject()->Has<InteractableComponent>();
			
			if (interactable) {

				std::vector<std::function<void()>> interactionEvents = object.lock()->GetGameObject()->Get<InteractableComponent>()->onInteractionEvent;
				if (interactionEvents.size() > 0)
				{
					for (auto& e : interactionEvents) {
						e();
					}
				}
			}
		}

		if (!isShadow) {
			//AudioManager::instance().Play("audio/SwordSwing.mp3");

			// (DISABLED) :: Too loud.  It's overpowering the other SFX.
			//AudioEngine::Instance().GetEvent("Interact").SetPosition(m_body->GetPosition());
			//AudioEngine::Instance().GetEvent("Interact").Play();
		}
	}

	// SHADOW SWAP
	if (InputEngine::GetKeyState(GLFW_KEY_F) == ButtonState::Pressed) {

		// Shadow Swap
		if (shadowIsExtended) {
			glm::vec3 temp = m_body->GetPosition();
			m_body->SetPosition(m_shadow->GetPosition());
			m_shadow->SetPosition(temp);

			AudioEngine::Instance().GetEvent("Swap").SetPosition(m_body->GetPosition());
			AudioEngine::Instance().GetEvent("Swap").Play();
		}
	}

	// SHADOW RECALL
	if (InputEngine::GetKeyState(GLFW_KEY_R) == ButtonState::Pressed) {

		// Recall
		if (!isShadow) {
			m_shadow->Get<RenderComponent>()->IsEnabled = false;
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

	//m_light->Position = Lerp(m_light->Position, ((isShadow) ? m_shadow->GetPosition() : m_body->GetPosition()) + lightOffset, cameraLerpT);
	//m_camera->GetScene()->SetupShaderAndLights();
}

void PlayerController::RenderImGui()
{ }

nlohmann::json PlayerController::ToJson() const
{
	return {};
}

PlayerController::Sptr PlayerController::FromJson(const nlohmann::json& blob)
{
	PlayerController::Sptr result = std::make_shared<PlayerController>();
	return result;
}