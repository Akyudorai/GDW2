#pragma once

#include "Gameplay/Scene.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Light.h"

using namespace Gameplay;

class SceneData
{
protected:
	std::vector<GameObject::Sptr>		GameObjects;
	std::vector<Light>					Lights;

public:
	 virtual Scene::Sptr Load(std::string name);
};