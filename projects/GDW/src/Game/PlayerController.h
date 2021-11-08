#pragma once

#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/Input.h"

#include "NOU/CMeshRenderer.h"

#include "../Editor/Debug.h"

#include "Physics/Physics.h"

using namespace nou;
using namespace OMG;

namespace GAME
{
	class PlayerController
	{
	public:
		bool isShadow = false;

	protected:
		float cameraLerp = 1.0f;
		glm::vec3 cameraOffset = glm::vec3(0.0f, 3.0f, 5.0f);

	public:
		PlayerController();
		PlayerController(Entity& body1, Entity& body2, Entity& cam);

		void Update(float deltaTime);
		void Render();

	protected:
		Entity* m_body;
		Entity* m_shadow;
		Entity* m_camera;

	protected:
		void HandleInput(float deltaTime);
		void UpdateCamera(float deltaTime);

		// Templated LERP function // TEMPORARY.  MOVE TO SEPERATE FILE
		template<typename T>
		T Lerp(const T& p0, const T& p1, float t)
		{
			return (1.0f - t) * p0 + t * p1;
		}		
	};
}