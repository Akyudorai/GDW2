#pragma once

#include <map>
#include <string>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

class Prefabs
{
public: // Singleton Format
	static Prefabs& Instance() { static Prefabs instance; return instance; }

protected: // Protected Funcs
	Prefabs() {};

public: // Public Funcs

	// Loads a Prefab to be generated
	static Gameplay::GameObject::Sptr Load(Gameplay::Scene::Sptr scene, std::string name, glm::vec3 position = glm::vec3(0));

};