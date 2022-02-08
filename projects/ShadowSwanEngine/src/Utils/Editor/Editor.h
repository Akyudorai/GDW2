#pragma once

#include "Gameplay/Scene.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/GameObject.h"


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
	void DrawOldGuiEditor(float deltaTime);
	void DrawNewGuiEditor(float deltaTime);
	bool DrawSaveLoadImGui(Gameplay::Scene::Sptr& scene, std::string& path);
	bool DrawLightImGui(const Gameplay::Scene::Sptr& scene, const char* title, int ix);

public:
	static bool IsPlaying;

protected:
	
	bool sceneViewOpen = true;
	glm::vec3 cameraMotion = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	float cameraSpeed = 5.0f;
	int selectedObjectIndex = 0;
	static Gameplay::GameObject::Sptr selectedObject;;
	Gameplay::Camera::Sptr m_camera;	
};