#pragma once
#include "Scene/Scene.h"
#include <memory>


namespace GAME 
{
	class GameSceneManager {
	private:
		GameSceneManager();
		~GameSceneManager();
	public:
		GameSceneManager(const GameSceneManager&) = delete;
		GameSceneManager(GameSceneManager&&) = delete;
		GameSceneManager& operator=(const GameSceneManager&) = delete;
		GameSceneManager& operator=(GameSceneManager&&) = delete;

	private:
		Scene* currentScene;

		static std::unique_ptr<GameSceneManager> instance;
		friend std::default_delete<GameSceneManager>;

	private:
		bool isRunning;
		unsigned int frameRate;

	public:
		static GameSceneManager* GetInstance();
		void Run();
		bool Initialize();
		void Update(const float deltaTime);
		void Render() const;
		void HandleEvents();
	};
}
