#pragma once

#include "Gameplay/Scene.h"

using namespace Gameplay;

class Level_Three
{
public:
	Level_Three() {};
	static Scene::Sptr Load(GLFWwindow* window);
	static bool PlayOnLoad;
};