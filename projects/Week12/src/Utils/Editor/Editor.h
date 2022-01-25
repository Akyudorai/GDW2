#pragma once

#include "Gameplay/Scene.h"



class Editor
{
protected:
	BulletDebugMode physicsDebugMode = BulletDebugMode::None;
	nlohmann::json editorSceneState;
	std::string scenePath = "scene.json";
	GLFWwindow* window;
	float playbackSpeed = 1.0f;

public:
	void Initialize(GLFWwindow* windowRef);
	void Update(float deltaTime);
	void Draw(float deltaTime);

protected:
	bool DrawSaveLoadImGui(Gameplay::Scene::Sptr& scene, std::string& path);
	bool DrawLightImGui(const Gameplay::Scene::Sptr& scene, const char* title, int ix);
};