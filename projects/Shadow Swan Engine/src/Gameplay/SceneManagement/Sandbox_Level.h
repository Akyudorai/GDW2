#pragma once

#include "Gameplay/SceneManagement/SceneData.h"
#include "Gameplay/Scene.h"

class Sandbox_Level
{
public:
	Sandbox_Level() {};
	static Scene::Sptr Load(GLFWwindow* window);
	static bool PlayOnLoad;
};