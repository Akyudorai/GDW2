#include "CameraManager.h"

namespace Gameplay
{
	void CameraManager::Initialize(GameObject::Sptr camera)
	{
		m_camera = camera;
	}

	void CameraManager::Update(float deltaTime)
	{
		if (m_camera == nullptr) return;

		// Handle Camera Lerp Timer
		if (cameraLerpT < 1.0f)
			cameraLerpT += deltaTime * 0.1f;
		else if (cameraLerpT > 1.0f)
			cameraLerpT = 1.0f;

		// Handle Camera Movement
		switch (m_mode) {
		case CameraMode::FollowTarget:
			LOG_INFO("FOLLOWING TARGET");
			m_camera->SetPosition(Lerp(m_camera->GetPosition(), m_target->GetPosition() + cameraOffset, cameraLerpT));
			break;
		case CameraMode::FollowRoom:
			LOG_INFO("FOLLOWING ROOM");
			m_camera->SetPosition(Lerp(m_camera->GetPosition(), m_roomData->cameraPos + cameraOffset, cameraLerpT));
			break;
		}
		
	}
}