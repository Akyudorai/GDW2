#pragma once

#include "Gameplay/Scene.h"

using namespace Gameplay;

class Level_One
{
public:
	Level_One() {};
	static Scene::Sptr Load(GLFWwindow* window);
	static bool PlayOnLoad;
};