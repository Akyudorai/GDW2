#pragma once

#include "GameObject.h"


namespace Gameplay
{
	class Prefabs
	{
	public:
		static enum class Prefab {
			Wall, Crate
		};

		static GameObject::Sptr Instantiate(Prefab id);
	};
}
