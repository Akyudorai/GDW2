#include "AudioSource.h"

#include "AudioManager.h"
#include "Utils/ResourceManagement/Resources.h"
#include "Gameplay/GameObject.h"


AudioSource::AudioSource()
	: m_Resource(nullptr), m_Settings(AudioSettings()), m_Channel(nullptr)
{ 
	m_Position = glm::vec3(0.0f);
}

void AudioSource::Init()
{
	m_Position = GetGameObject()->GetPosition();	
}

void AudioSource::Play()
{
	if (m_Resource == nullptr) return;

	LOG_INFO("Playing Source");

	IsPlaying = true;
	AudioManager::instance().PlaySource(this);
}

void AudioSource::Stop()
{
	if (m_Resource == nullptr) return;

	LOG_INFO("Stopping Source");

	IsPlaying = false;
	AudioManager::instance().StopSource(this);
}

void AudioSource::Awake()
{	
	m_Position = GetGameObject()->GetPosition();
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

	ImGui::Checkbox("Play On Awake?", &playOnAwake);

	if (ImGui::Checkbox("Is 3D?", &m_Settings.Is3D))
	{
		AudioManager::SetMode(m_Resource->Sound, m_Settings);
	}

	if (ImGui::Checkbox("Looping?", &m_Settings.IsLooping))
	{
		AudioManager::SetMode(m_Resource->Sound, m_Settings);
	}

	if (ImGui::Checkbox("Is Stream?", &m_Settings.IsStream))
	{
		AudioManager::SetMode(m_Resource->Sound, m_Settings);
	}

	static const char* current_source = (m_Resource == nullptr) ? "" : m_Resource->Name.c_str();
	if (ImGui::BeginCombo("Source", current_source, 0))
	{
		if (ImGui::Selectable("", current_source)) {
			current_source = "";

			if (IsPlaying) Stop();

			// Remove the audio resource
			m_Resource = nullptr;
		}

		std::map<std::string, AudioResource::Sptr>::iterator it;
		for (auto& it = Resources::GetAudioMap()->begin(); it != Resources::GetAudioMap()->end(); ++it)
		{
			bool is_selected = (current_source == it->first.c_str());
			if (ImGui::Selectable(it->first.c_str(), is_selected)) {
				current_source = it->first.c_str();

				if (IsPlaying) Stop();

				// attach the new audio resource
				m_Resource = it->second;
				m_Resource->Sound = AudioManager::LoadSound(m_Resource, m_Settings);

				if (playOnAwake) {
					Play();
				}
			}

			if (current_source)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

nlohmann::json AudioSource::ToJson() const
{
	return { 
		{"play_on_awake", playOnAwake },
		{"resource", (m_Resource != nullptr) ? m_Resource->Name : "NULL" },
		{"is_3d", m_Settings.Is3D },
		{"is_looping", m_Settings.IsLooping },
		{"is_stream", m_Settings.IsStream }
	};
}

AudioSource::Sptr AudioSource::FromJson(const nlohmann::json& blob)
{
	AudioSource::Sptr result = std::make_shared<AudioSource>();
	result->playOnAwake = blob["is_enabled"];
	result->m_Settings.Is3D = blob["is_3d"];
	result->m_Settings.IsLooping = blob["is_looping"];
	result->m_Settings.IsStream = blob["is_stream"];
	result->m_Resource = Resources::GetSound(blob["resource"]);
	result->m_Resource->Sound = AudioManager::LoadSound(result->m_Resource, result->m_Settings);

	return result;
}