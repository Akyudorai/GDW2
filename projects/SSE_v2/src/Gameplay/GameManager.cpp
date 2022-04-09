#include "GameManager.h"

#include "Application/Application.h"
#include "Gameplay/InputEngine.h"

#include "Application/Layers/GameSceneLayer.h"
#include "Application/Layers/MenuSceneLayer.h"

#include <fstream>
#include <iostream>

namespace Gameplay
{
	GameInterfaceManager GameManager::GameInterface = GameInterfaceManager();
	Leaderboard GameManager::leaderboard = Leaderboard();

	Leaderboard::Leaderboard() 
	{ 
		HighScores = std::vector<Highscore>();
		HighScores.push_back(Highscore(4));
		HighScores.push_back(Highscore(7));
		HighScores.push_back(Highscore(10));
		HighScores.push_back(Highscore(3));
		HighScores.push_back(Highscore(0));
		HighScores.push_back(Highscore(5));
		HighScores.push_back(Highscore(17));
		HighScores.push_back(Highscore(12));
		HighScores.push_back(Highscore(2));
		HighScores.push_back(Highscore(1));
	}

	void Leaderboard::Push(Highscore score)
	{
		// Check if Score Exceeds the lowest possible score on leaderboard
		if (score.CompletionTime < HighScores.at(9).CompletionTime && HighScores.at(9).CompletionTime != 0 ||
			HighScores.at(9).CompletionTime == 0)
		{
			// Add it to the list
			HighScores.at(9) = score;

			// Sort the List
			Sort();			
		}
		else
		{
			LOG_INFO("Score not sufficient to be placed in leaderboard.");
		}
	}

	// Data is always HighScores
	void Leaderboard::Sort()
	{
		// Go through the list and organize in order of shortest time to greatest.
		int length = HighScores.size();
		int smallest = 0;
		for (int i = 0; i < length-1; i++)
		{
			// Smallest should always be first in the list
			smallest = i;

			// Loop through positions 1 to 9
			for (int j = i + 1; j < length; j++)
			{
				// Check if the Completion Time of next position is less than the smallest position
				if (HighScores.at(j).CompletionTime < HighScores.at(smallest).CompletionTime && HighScores.at(j).CompletionTime != 0 ||
					HighScores.at(smallest).CompletionTime == 0) {
					smallest = j;
				}
			}
			
			// If the current position is not the smallest
			if (i != smallest) {
				// Swap their positions  
				Highscore tmp = HighScores.at(i);
				HighScores.at(i) = HighScores.at(smallest);
				HighScores.at(smallest) = tmp;
			}
		}

		LOG_INFO("Leaderboard Sorted");
	}	


	void Leaderboard::Debug()
	{
		std::string result = "Leaderboard\n";
		for (int i = 0; i < HighScores.size(); i++)
		{			
			result += std::to_string(i + 1);
			result += " : ";

			if (HighScores.at(i).CompletionTime != 0) {
				result += std::to_string(HighScores.at(i).CompletionTime);
			}
			else {
				result += "-";
			}
			
			result += "\n";
		}

		LOG_INFO(result);
	}

	void GameManager::Update(float deltaTime)
	{
		if (InputEngine::GetKeyState(GLFW_KEY_KP_5) == ButtonState::Pressed)
		{
			leaderboard.Debug();
		}

		if (InputEngine::GetKeyState(GLFW_KEY_KP_6) == ButtonState::Pressed)
		{
			leaderboard.Sort();
		}

		if (InputEngine::GetKeyState(GLFW_KEY_KP_4) == ButtonState::Pressed)
		{
			leaderboard.Push(Highscore(7));
		}

		// If the player presses the Numpad 7, switch to Game Scene.
		if (InputEngine::GetKeyState(GLFW_KEY_KP_7) == ButtonState::Pressed)
		{
			Application& app = Application::Get();

			// Initialize Game Variables
			hasKey = false;
			gameTime = 0.0f;
			SetPaused(false);
			SetGameOver(false);
			pc = nullptr;

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

			// Initialize Game Variables
			hasKey = false;
			gameTime = 0.0f;
			SetPaused(false);
			SetGameOver(false);
			pc = nullptr;

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

		// Initialize Game Variables
		hasKey = false;
		gameTime = 0.0f;
		SetPaused(false);
		SetGameOver(false);
		pc = nullptr;

		// Unload the Level
		app.LoadScene(std::make_shared<Gameplay::Scene>());

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