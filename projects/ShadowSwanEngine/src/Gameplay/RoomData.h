#pragma once

#include "GameObject.h"

namespace Gameplay
{
	struct RoomData
	{
		std::string RoomName;
		std::vector<GameObject::Sptr> ObjectsInRoom;

		glm::vec3 cameraPos;
	};
}