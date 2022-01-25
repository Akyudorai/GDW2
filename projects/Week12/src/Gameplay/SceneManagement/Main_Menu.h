#pragma once

#include "Gameplay/Scene.h"

using namespace Gameplay;

class Main_Menu
{
public:
	Main_Menu() {};
	static Scene::Sptr Load(GLFWwindow* window);
	static bool PlayOnLoad;
};