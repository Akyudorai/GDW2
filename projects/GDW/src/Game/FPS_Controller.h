#pragma once

#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/Input.h"

using namespace nou;

namespace GAME
{
	class FPS_Controller
	{
	public:
		FPS_Controller(Entity& owner, CCamera& camera);
		virtual ~FPS_Controller() = default;
		FPS_Controller(FPS_Controller&&) = default;
		FPS_Controller& operator=(FPS_Controller&&) = default;

		void UpdateCamera(float deltaTime);
		void UpdateMotion(float deltaTime);

	protected:
		Entity* m_owner;
		CCamera* m_camera;
		FPS_Controller();
	};
}