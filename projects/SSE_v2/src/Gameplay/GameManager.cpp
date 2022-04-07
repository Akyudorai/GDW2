#include "GameManager.h"

#include "Application/Application.h"
#include "Gameplay/InputEngine.h"

#include "Application/Layers/GameSceneLayer.h"
#include "Application/Layers/MenuSceneLayer.h"

namespace Gameplay
{
	GameInterfaceManager GameManager::GameInterface = GameInterfaceManager();

	void GameManager::Update(float deltaTime)
	{
		// If the player presses the Numpad 7, switch to Game Scene.
		if (InputEngine::GetKeyState(GLFW_KEY_KP_7) == ButtonState::Pressed)
		{
			Application& app = Application::Get();

			// Unload Menu Scene
			app.GetLayer<MenuSceneLayer>()->UnloadScene();
			isMainMenu = false;

			// Load Game Scene						
			Reset();

		}

		// If the player presses the Numpad 8 key, switch to Menu Scene.
		if (InputEngine::GetKeyState(GLFW_KEY_KP_8) == ButtonState::Pressed)
		{
			Application& app = Application::Get();

			// Unload Game Scene
			app.GetLayer<GameSceneLayer>()->UnloadScene();

			// Load Menu Scene			
			app.GetLayer<MenuSceneLayer>()->LoadScene();
			isMainMenu = true;

		}

		// If the player presses the Numpad 9 key when game is over, it restarts the game.
		if (InputEngine::GetKeyState(GLFW_KEY_KP_9) == ButtonState::Pressed)
		{
			if (IsGameOver()) {
				Reset();
			}			
		}

		// If the game is over, don't do anything below this line
		if (isGameOver || isMainMenu) return;

		// Increment Game Timer
		if (!isPaused) {
			gameTime += deltaTime;
		}

		// If the player presses the Tab key, it pauses the game.
		if (InputEngine::GetKeyState(GLFW_KEY_TAB) == ButtonState::Pressed)
		{
			SetPaused(!IsPaused());

			if (GameInterface.m_PauseMenuPanel != nullptr) {				
				GameInterface.TogglePausePanel(IsPaused());
			}
		}

		// REMOVE ONCE UI FIXES ARE DONE
		// If the player presses the Numpad 1 key, it causes a win event.
		if (InputEngine::GetKeyState(GLFW_KEY_KP_1) == ButtonState::Pressed)
		{
			if (GameInterface.m_WinPanel != nullptr) {
				SetPaused(true);
				GameInterface.ToggleWinPanel(true);				
			}

			if (GameInterface.m_GameUserInterface != nullptr)
			{
				GameInterface.ToggleGameUserInterface(false);
			}
		}

		// REMOVE ONCE UI FIXES ARE DONE
		// If the player presses the Numpad 1 key, it causes a loss event.
		if (InputEngine::GetKeyState(GLFW_KEY_KP_2) == ButtonState::Pressed)
		{
			if (GameInterface.m_PauseMenuPanel != nullptr) {
				SetPaused(true);
				GameInterface.ToggleLosePanel(true);
			}

			if (GameInterface.m_GameUserInterface != nullptr)
			{
				GameInterface.ToggleGameUserInterface(false);
			}
		}

		// REMOVE ONCE UI FIXES ARE DONE
		// Clear all UI except for Game UI
		if (InputEngine::GetKeyState(GLFW_KEY_KP_0) == ButtonState::Pressed)
		{
			SetPaused(false);
			SetGameOver(false);
			
			if (GameInterface.m_GameUserInterface != nullptr)
			{
				GameInterface.ToggleGameUserInterface(true);
			}

			if (GameInterface.m_PauseMenuPanel != nullptr) {				
				GameInterface.ToggleLosePanel(false);
			}

			if (GameInterface.m_WinPanel != nullptr) {				
				GameInterface.ToggleWinPanel(false);
			}

			if (GameInterface.m_PauseMenuPanel != nullptr) {				
				GameInterface.TogglePausePanel(false);
			}
		}
	}

	void GameManager::Reset()
	{
		Application& app = Application::Get();

		// Unload the Level
		app.LoadScene(std::make_shared<Gameplay::Scene>());

		// Initialize Game Variables
		hasKey = false;
		gameTime = 0.0f;
		SetPaused(false);
		SetGameOver(false);

		// Reload the Level
		app.GetLayer<GameSceneLayer>()->LoadScene();

		// Initialize all UI
		if (GameInterface.m_GameUserInterface != nullptr) {
			GameInterface.ToggleGameUserInterface(true);
		}

		if (GameInterface.m_PauseMenuPanel != nullptr) {
			GameInterface.ToggleLosePanel(false);
		}

		if (GameInterface.m_WinPanel != nullptr) {
			GameInterface.ToggleWinPanel(false);
		}

		if (GameInterface.m_PauseMenuPanel != nullptr) {
			GameInterface.TogglePausePanel(false);
		}
	}
}