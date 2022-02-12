#pragma once

#include "GameObject.h"
#include "RoomData.h"

namespace Gameplay
{
	enum class CameraMode {
		FollowTarget, FollowRoom
	};

	class CameraManager
	{

	public:
		CameraManager() {}

		void Initialize(GameObject::Sptr camera);
		void Update(float deltaTime);

		void SetCameraMode(CameraMode mode) {
			m_mode = mode;
		}

		CameraMode GetCameraMode() {
			return m_mode;
		}

		void SetRoomData(RoomData* data) {
			m_roomData = data;
		}
		void SetTarget(GameObject* target) {
			m_target = target;
		}

	protected:		
		GameObject::Sptr m_camera;
		
		CameraMode m_mode;
		GameObject* m_target;
		RoomData* m_roomData;

		float cameraLerpT = 1.0f;
		glm::vec3 cameraOffset = glm::vec3(0, -7, 15);
		glm::vec3 cameraRotation = glm::vec3(30, 0, 0);	

		/* MOVE TO UTILITY CLASS */
		template<typename T>
		T Lerp(const T& p0, const T& p1, float t) {
			return (1.0f - t) * p0 + t * p1;
		}
	};
}
