#include "AudioSource.h"

#include "Audio/AudioManager.h"
#include "Gameplay/GameObject.h"

AudioSource::AudioSource()
	: strTargetEvent(""), m_Position(glm::vec3(0)), playOnAwake(false)
{ }

void AudioSource::Init(AudioSettings settings)
{
	m_Position = GetGameObject()->GetPosition();
	playOnAwake = settings.PlayOnAwake;
}

void AudioSource::Play()
{
	if (strTargetEvent == "") return;

	AudioEngine::Instance().GetEvent(strTargetEvent).SetPosition(m_Position);
	AudioEngine::Instance().GetEvent(strTargetEvent).Play();
}

void AudioSource::Play(std::string name)
{
	LoadEvent(name);
	Play();
}

void AudioSource::Stop(bool bImmediate)
{
	if (strTargetEvent == "") return;

	if (bImmediate) AudioEngine::Instance().GetEvent(strTargetEvent).StopImmediately();
	else AudioEngine::Instance().GetEvent(strTargetEvent).Stop();
}

bool AudioSource::IsPlaying()
{
	return (strTargetEvent == "") ? AudioEngine::Instance().GetEvent(strTargetEvent).IsPlaying() : false;
}

void AudioSource::Awake()
{
	m_Position = GetGameObject()->GetPosition();
	
	if (strTargetEvent == "") return;

	if (playOnAwake)
	{
		Play();
	}
}

void AudioSource::Update(float deltaTime)
{ }

void AudioSource::RenderImGui()
{
	ImGui::Text("Audio Source");

	if (ImGui::BeginCombo("Source", strTargetEvent.c_str(), 0))
	{
		if (ImGui::Selectable("", strTargetEvent.c_str())) {
			strTargetEvent = "";
		}
		
		for (auto& it : AudioEngine::Instance().GetEventsMap()) {
			bool is_selected = (strTargetEvent == it.first);
			if (ImGui::Selectable(it.first.c_str(), is_selected)) {
				// Stop Current Source
				if (strTargetEvent != "") {
					Stop(true);
				}
				
				// Set New Source
				strTargetEvent = it.first;
				
				if (playOnAwake) {
					Play();
				}
			}

			if (strTargetEvent.c_str()) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
	
	ImGui::Checkbox("Play On Awake?", &playOnAwake);

	// If there's no source, don't render any of these settings
	if (strTargetEvent == "") return;
	
	float newVol = AudioEngine::Instance().GetEvent(strTargetEvent).GetVolume();
	ImGui::InputFloat("Volume", &newVol, 0.1f, 0.25f, 2);
	if (volume != newVol) {
		volume = newVol;
		AudioEngine::Instance().GetEvent(strTargetEvent).SetVolume(volume);
	}
	
}

nlohmann::json AudioSource::ToJson() const
{
	return {
		{"play_on_awake", playOnAwake },
	};
}

AudioSource::Sptr AudioSource::FromJson(const nlohmann::json& blob)
{
	AudioSource::Sptr result = std::make_shared<AudioSource>();
	result->playOnAwake = blob["is_enabled"];

	return result;
}