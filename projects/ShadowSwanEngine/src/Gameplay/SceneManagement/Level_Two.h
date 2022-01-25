#pragma once

#include "Gameplay/Scene.h"

using namespace Gameplay;

class Level_Two
{
public:
	Level_Two() {};
	static Scene::Sptr Load(GLFWwindow* window);
	static bool PlayOnLoad;
};