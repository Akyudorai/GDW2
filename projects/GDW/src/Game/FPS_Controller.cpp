
#include "FPS_Controller.h"

namespace GAME 
{
	FPS_Controller::FPS_Controller()
	{
		m_owner = nullptr;
		m_camera = nullptr;
	}

	FPS_Controller::FPS_Controller(Entity& owner, CCamera& camera)
	{
		m_owner = &owner;
		m_camera = &camera;
	}

	void FPS_Controller::UpdateMotion(float deltaTime)
	{
		if (Input::GetKey(GLFW_KEY_W)) {
			// Move Forward
			m_owner->transform.m_pos.z -= deltaTime;			
		}

		if (Input::GetKey(GLFW_KEY_S)) {
			// Move Backward
			m_owner->transform.m_pos.z += deltaTime;
		}

		if (Input::GetKey(GLFW_KEY_A)) {
			// Move Left
			m_owner->transform.m_pos.x -= deltaTime;
		}

		if (Input::GetKey(GLFW_KEY_D)) {
			// Move Right
			m_owner->transform.m_pos.x += deltaTime;
		}
	}

	void FPS_Controller::UpdateCamera(float deltaTime)
	{
		if (Input::GetKey(GLFW_KEY_UP)) {
			// Tilt Up
			m_owner->transform.m_rotation += glm::quat(0, deltaTime, 0, 0);
		}

		if (Input::GetKey(GLFW_KEY_DOWN)) {
			m_owner->transform.m_rotation -= glm::quat(0, deltaTime, 0, 0);
		}

		if (Input::GetKey(GLFW_KEY_LEFT)) {
			m_owner->transform.m_rotation += glm::quat(0, 0, deltaTime, 0);
		}

		if (Input::GetKey(GLFW_KEY_RIGHT)) {
			m_owner->transform.m_rotation -= glm::quat(0, 0, deltaTime, 0);
		}
	}


}