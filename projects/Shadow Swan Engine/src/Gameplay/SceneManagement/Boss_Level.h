#pragma once

#include "Gameplay/Scene.h"

using namespace Gameplay;

class Boss_Level
{
public:
	Boss_Level() {};
	static Scene::Sptr Load(GLFWwindow* window);
	static bool PlayOnLoad;
};