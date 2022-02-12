#pragma once

#include "Gameplay/Scene.h"
#include "../UI/GameInterfaceManager.h"
#include "Gameplay/RoomLoader.h"

namespace Gameplay
{
	class SceneManager
	{
	public:
		static enum class Scenes {
			Sandbox, LevelOne, LevelTwo, LevelThree, BossLevel, MainMenu
		};

		static void Initialize(GLFWwindow* window);

		static void LoadScene(Scenes sceneIndex, bool playOnLoad);

		static void Update(float deltaTime);
		static void Draw();

		static Scene::Sptr GetCurrentScene() {
			return currentScene;
		}
		
	public:
		static GameInterfaceManager GameInterface;
		static RoomLoader roomLoader;

	protected:
		
		static Scene::Sptr currentScene;
		static GLFWwindow* windowRef;
		

	};
}