#pragma once

#include "Gameplay/Scene.h"
#include "../UI/GameInterfaceManager.h"

namespace Gameplay
{
	class SceneManager
	{
	public:
		static enum class Scenes {
			Sandbox, LevelOne, LevelTwo, LevelThree, BossLevel
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

	protected:
		static Scene::Sptr currentScene;
		static GLFWwindow* windowRef;
		

	};
}