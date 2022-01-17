#include "PlayerController.h"

namespace GAME
{
	PlayerController::PlayerController()
	{
		m_body = nullptr;
		m_shadow = nullptr;
		m_camera = nullptr;
	}

	PlayerController::PlayerController(Entity& body1, Entity& body2, Entity& cam)
	{
		m_body = &body1;
		m_shadow = &body2;
		m_camera = &cam;
	}

	void PlayerController::Update(float deltaTime)
	{	
		HandleInput(deltaTime);
		UpdateCamera(deltaTime);
		m_body->Get<PhysicsObject>().Update(deltaTime);
		m_shadow->Get<PhysicsObject>().Update(deltaTime);
	}

	void PlayerController::Render()
	{
		m_body->transform.RecomputeGlobal();
		m_shadow->transform.RecomputeGlobal();

		m_body->Get<CMeshRenderer>().Draw();
		m_shadow->Get<CMeshRenderer>().Draw();
	}

	void PlayerController::HandleInput(float deltaTime)
	{
		m_body->Get<PhysicsObject>().velocity = glm::vec3(0);
		m_shadow->Get<PhysicsObject>().velocity = glm::vec3(0);

		if (Input::GetKey(GLFW_KEY_W)) 
		{	
			// Move Forward
			if (!isShadow) {
				m_body->Get<PhysicsObject>().velocity.z = -1;
				//m_body->transform.m_pos.z -= deltaTime;
				m_body->transform.m_rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
							
			else {
				m_shadow->Get<PhysicsObject>().velocity.z = -1;
				//m_shadow->transform.m_pos.z -= deltaTime;
				m_shadow->transform.m_rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}					
		} 

		if (Input::GetKey(GLFW_KEY_S)) 
		{	
			// Move Backward
			if (!isShadow) {
				m_body->Get<PhysicsObject>().velocity.z = 1;
				//m_body->transform.m_pos.z += deltaTime;
				m_body->transform.m_rotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
				
			else {
				m_shadow->Get<PhysicsObject>().velocity.z = 1;
				//m_shadow->transform.m_pos.z += deltaTime;
				m_shadow->transform.m_rotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
				
		}

		if (Input::GetKey(GLFW_KEY_A)) 
		{	
			// Move Left
			if (!isShadow) {
				m_body->Get<PhysicsObject>().velocity.x = -1;
				//m_body->transform.m_pos.x -= deltaTime;
				m_body->transform.m_rotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
				
			else {
				m_shadow->Get<PhysicsObject>().velocity.x = -1;
				//m_shadow->transform.m_pos.x -= deltaTime;
				m_shadow->transform.m_rotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
						
		}

		if (Input::GetKey(GLFW_KEY_D)) 
		{
			// Move Right
			if (!isShadow) {
				m_body->Get<PhysicsObject>().velocity.x = 1;
				//m_body->transform.m_pos.x += deltaTime;
				m_body->transform.m_rotation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
				
			else {
				m_shadow->Get<PhysicsObject>().velocity.x = 1;
				//m_shadow->transform.m_pos.x += deltaTime;
				m_shadow->transform.m_rotation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
				
		}

		if (Input::GetKeyDown(GLFW_KEY_Q)) 
		{
			// Swap
			isShadow = !isShadow;
			cameraLerp = 0;
		}

		if (Input::GetKeyDown(GLFW_KEY_E)) {
			// Ability 
			Debug::Log("Ability");
		}

		if (Input::GetKeyDown(GLFW_KEY_R)) {
			// Recall
			Debug::Log("Recall Shadow");
		}
	}

	void PlayerController::UpdateCamera(float deltaTime)
	{
		if (cameraLerp < 1.0f)
			cameraLerp += deltaTime * 0.1f;
		else if (cameraLerp > 1.0f)
			cameraLerp = 1.0f;

		m_camera->transform.m_pos = Lerp(m_camera->transform.m_pos, ((isShadow) ? m_shadow->transform.m_pos : m_body->transform.m_pos) + cameraOffset, cameraLerp);	
		m_camera->Get<CCamera>().Update();
	}
}