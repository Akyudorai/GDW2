#include "GameManagementLayer.h"
#include "Application/Application.h"
#include "Application/Timing.h"
#include "Gameplay/GameManager.h"

GameManagementLayer::GameManagementLayer() :
	ApplicationLayer()
{
	Name = "Game Manager";
	Overrides = AppLayerFunctions::OnAppLoad | AppLayerFunctions::OnUpdate | AppLayerFunctions::OnAppUnload;
}

GameManagementLayer::~GameManagementLayer()
{ }

void GameManagementLayer::OnAppLoad(const nlohmann::json& config)
{
	Application& app = Application::Get();

}

void GameManagementLayer::OnUpdate()
{
	Application& app = Application::Get();
	float dt = Timing::Current().DeltaTime();

	GameManager::GetInstance().Update(dt);
}