#include "SceneManager.h"
#include "Level_One.h"
#include "Main_Menu.h"

#include "../InputManagement/InputHandler.h"
#include "Audio/AudioManager.h"

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
			//AudioManager::instance().ClearSounds();
			AudioEngine::Instance().StopAllSounds();

			if (AudioEngine::Instance().GetEvent("Test").IsPlaying())
				AudioEngine::Instance().GetEvent("Test").StopImmediately();
		}
		
		currentScene = std::make_shared<Scene>();		

		switch (sceneIndex)
		{
			default:
			case Scenes::LevelOne:
				Level_One::Load(windowRef);
				playOnLoad = Level_One::PlayOnLoad;
				break;
			case Scenes::MainMenu:
				Main_Menu::Load(windowRef);
				playOnLoad = Main_Menu::PlayOnLoad;
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


		//Switch to Level 1
		if (InputHandler::GetKeyDown(GLFW_KEY_F1)) {
			LoadScene(Scenes::LevelOne, true);
		}

		// Switch Scene to Main Menu
		if (InputHandler::GetKeyDown(GLFW_KEY_F2)) {
			LoadScene(Scenes::MainMenu, true);
		}

	}

	void SceneManager::Draw()
	{
		if (!currentScene->IsPlaying) {
			return;
		}
	}
}