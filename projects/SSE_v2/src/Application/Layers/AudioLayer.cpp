#include "AudioLayer.h"
#include "Application/Application.h"
#include "Audio/AudioManager.h"

AudioLayer::AudioLayer() :
	ApplicationLayer()
{
	Name = "Audio";
	Overrides = AppLayerFunctions::OnAppLoad | AppLayerFunctions::OnUpdate | AppLayerFunctions::OnAppUnload;
}

AudioLayer::~AudioLayer()
{ }

void AudioLayer::OnAppLoad(const nlohmann::json& config)
{
	Application& app = Application::Get();


}

void AudioLayer::OnUpdate()
{
	Application& app = Application::Get();
	
	AudioEngine::Instance().Update();
}

void AudioLayer::OnAppUnload()
{
	Application& app = Application::Get();

	AudioEngine::Instance().Shutdown();
}