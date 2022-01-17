#include "SceneManager.h"

#include "Sandbox_Level.h"
#include "Level_One.h"
#include "Level_Two.h"
#include "Level_Three.h"
#include "Boss_Level.h"


namespace Gameplay
{
	Scene::Sptr SceneManager::currentScene = nullptr;
	GLFWwindow* SceneManager::windowRef = nullptr;
	GameInterfaceManager SceneManager::GameInterface = GameInterfaceManager();

	void SceneManager::Initialize(GLFWwindow* window)
	{
		windowRef = window;
		GameInterface = GameInterfaceManager();
	}

	void SceneManager::LoadScene(Scenes sceneIndex, bool playOnLoad)
	{
		if (currentScene != nullptr) {
			currentScene = nullptr;
			GameInterface.Clear();
		}
		
		currentScene = std::make_shared<Scene>();

		switch (sceneIndex)
		{
			default:
			case Scenes::Sandbox:				
				Sandbox_Level::Load(windowRef);
				playOnLoad = Sandbox_Level::PlayOnLoad;
				break;
			case Scenes::LevelOne:
				Level_One::Load(windowRef);
				playOnLoad = Level_One::PlayOnLoad;
				break;
			case Scenes::LevelTwo:
				Level_Two::Load(windowRef);
				playOnLoad = Level_Two::PlayOnLoad;
				break;
			case Scenes::LevelThree:
				Level_Three::Load(windowRef);
				playOnLoad = Level_Three::PlayOnLoad;
				break;
			case Scenes::BossLevel:
				Boss_Level::Load(windowRef);
				playOnLoad = Boss_Level::PlayOnLoad;
				break;
		}

		currentScene->IsPlaying = true;
		currentScene->IsPaused = !playOnLoad;
	}

	void SceneManager::Update(float deltaTime)
	{
		if (!currentScene->IsPlaying) {
			return;
		}

		currentScene->DoPhysics(deltaTime);
		currentScene->Update(deltaTime);

		// Switch Scene to Sandbox
		if (glfwGetKey(windowRef, GLFW_KEY_F5) == GLFW_PRESS) {
			LoadScene(Scenes::Sandbox, true);
		}

		// Switch Scene to Level One
		if (glfwGetKey(windowRef, GLFW_KEY_F1) == GLFW_PRESS) {
			LoadScene(Scenes::LevelOne, true);
		}

		// Switch Scene to Level Two
		if (glfwGetKey(windowRef, GLFW_KEY_F2) == GLFW_PRESS) {
			LoadScene(Scenes::LevelTwo, true);
		}

		// Switch Scene to Level Three
		if (glfwGetKey(windowRef, GLFW_KEY_F3) == GLFW_PRESS) {
			LoadScene(Scenes::LevelThree, true);
		}

		// Switch Scene to Boss Level
		if (glfwGetKey(windowRef, GLFW_KEY_F4) == GLFW_PRESS) {
			LoadScene(Scenes::BossLevel, true);
		}

	}

	void SceneManager::Draw()
	{
		if (!currentScene->IsPlaying) {
			return;
		}
	}
}