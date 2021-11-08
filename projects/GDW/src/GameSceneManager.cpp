#include "GameSceneManager.h"

#include "NOU/App.h"
#include "NOU/Input.h"

#include "Editor/Debug.h"
#include "Editor/Resources.h"
#include "Scene/Level_1.h"
#include "Scene/Level_2.h"
#include "Scene/Level_3.h"
#include "Scene/Dev_Stage.h"

using namespace GAME;
using namespace nou;

std::unique_ptr<GameSceneManager> GameSceneManager::instance(nullptr);

GameSceneManager::GameSceneManager() : currentScene(nullptr), isRunning(false)
{
	frameRate = 60;
}

GameSceneManager::~GameSceneManager()
{
	isRunning = false;
}

GameSceneManager* GameSceneManager::GetInstance()
{
	if (instance.get() == nullptr) {
		instance.reset(new GameSceneManager());
	}

	return instance.get();
}

bool GameSceneManager::Initialize()
{
	currentScene = new Level_1();
	if (currentScene == nullptr) {
		Debug::LogError("FATAL ERROR: Failed to initalize the Scene.");
		return false;
	}
	if (currentScene->OnCreate() == false) return false;

	return true;
}

void GameSceneManager::Run()
{
	// Create window and set clear color
	App::Init("Game", 800, 600);
	App::SetClearColor(glm::vec4(0.0f, 0.27f, 0.4f, 1.0f));

	// Initialize ImGui
	App::InitImgui();

	// Load in our model/texture resources
	Resources::GetInstance().LoadResources();

	App::Tick();
	isRunning = Initialize();
	
	while (isRunning && !App::IsClosing()) {

		App::FrameStart();
		float deltaTime = App::GetDeltaTime();

		HandleEvents();
		Update(deltaTime);
		Render();

		//App::StartImgui();	
		//Editor::GetInstance().Render();
		//App::EndImgui();	

		App::SwapBuffers();
	}

	App::Cleanup();

	return;
}

void GameSceneManager::HandleEvents()
{
	// Load Level 1
	if (Input::GetKeyDown(GLFW_KEY_F1)) {
		if (currentScene) delete currentScene;
		currentScene = new Level_1();
		currentScene->OnCreate();		
	}

	// Load Level 2
	if (Input::GetKeyDown(GLFW_KEY_F2)) {
		if (currentScene) delete currentScene;
		currentScene = new Level_2();
		currentScene->OnCreate();
	}

	// Load Level 3
	if (Input::GetKeyDown(GLFW_KEY_F3)) {
		if (currentScene) delete currentScene;
		currentScene = new Level_3();
		currentScene->OnCreate();
	}

	// Load Development Stage
	if (Input::GetKeyDown(GLFW_KEY_F4)) {
		if (currentScene) delete currentScene;
		currentScene = new Dev_Stage();
		currentScene->OnCreate();
	}

	if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
		isRunning = false;
	}

}

void GameSceneManager::Update(const float deltaTime)
{
	assert(currentScene);
	currentScene->Update(deltaTime);
}

void GameSceneManager::Render() const
{
	assert(currentScene);
	currentScene->Render();
}


