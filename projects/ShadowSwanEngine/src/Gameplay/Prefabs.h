#pragma once

#include "GameObject.h"


namespace Gameplay
{
	class Prefabs
	{
	public:
		static enum class Prefab {
			Wall, Ground, Crate, PresurePlate, Switch, Elevator, Turret, Key, KeyDoor, SpikeTrap
		};

		static GameObject::Sptr Instantiate(Prefab id);
	};
}
