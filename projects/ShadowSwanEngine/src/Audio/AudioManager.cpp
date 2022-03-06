#include "AudioManager.h"

#include "Logging.h"

bool AudioManager::Init()
{
    // Initialize FMOD System
    AudioManager::ErrorCheck(System_Create(&instance().fmSystem), __LINE__);

    // Initialize FMOD
    AudioManager::ErrorCheck(instance().fmSystem->init(32, FMOD_INIT_NORMAL, 0), __LINE__);

    return true;
}

void AudioManager::Update()
{
    FMOD_REVERB_PROPERTIES reverb;

    instance().fmSystem->update();

    //AudioManager::ErrorCheck(fmStudioSystem->update());
}

void AudioManager::Shutdown()
{
    instance().fmSystem->close();
    instance().fmSystem->release();
}

int AudioManager::ErrorCheck(FMOD_RESULT result, int line)
{
    if (result != FMOD_OK)
    {
        LOG_ERROR("FMOD ERROR [" + std::to_string(line) + "]" + std::to_string(result));
        return 1;
    }

    return 0;
}

Sound* AudioManager::LoadSound(AudioResource::Sptr resource, AudioSettings settings)
{
    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= settings.Is3D ? FMOD_3D : FMOD_2D;
    eMode |= settings.IsLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= settings.IsStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    Sound* result = nullptr;

    AudioManager::ErrorCheck(instance().fmSystem->createSound(resource->Filepath.c_str(), eMode, nullptr, &result), __LINE__);
    if (result)
    {
        return result;
    }


    LOG_ERROR("Sound failed to load.")    
    return nullptr;;
}

void AudioManager::SetMode(Sound* sound, AudioSettings settings)
{
    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= settings.Is3D ? FMOD_3D : FMOD_2D;
    eMode |= settings.IsLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= settings.IsStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    AudioManager::ErrorCheck(sound->setMode(eMode), __LINE__);
}

void AudioManager::SetListener(const FMOD_VECTOR* position, const FMOD_VECTOR* velocity, const FMOD_VECTOR* forward, const FMOD_VECTOR* up)
{
    instance().fmSystem->set3DListenerAttributes(0, position, velocity, forward, up);
}

void AudioManager::PlaySource(AudioSource* source, float fVolume)
{
    CurrentSounds.push_back(source);

    AudioManager::ErrorCheck(fmSystem->playSound(source->m_Resource->Sound, nullptr, true, &source->m_Channel), __LINE__);
    if (source->m_Channel)
    {
        FMOD_MODE currMode;
        source->m_Resource->Sound->getMode(&currMode);
        if (currMode & FMOD_3D)
        {
            FMOD_VECTOR position = VectorToFmod(source->m_Position);
            AudioManager::ErrorCheck(source->m_Channel->set3DAttributes(&position, nullptr), __LINE__);
        }

        AudioManager::ErrorCheck(source->m_Channel->setVolume(dbToVolume(fVolume)), __LINE__);
        AudioManager::ErrorCheck(source->m_Channel->setPaused(false), __LINE__);
    }
}

void AudioManager::StopSource(AudioSource* source)
{
    std::vector<AudioSource*>::iterator it;
    for (auto it = CurrentSounds.begin(); it != CurrentSounds.end(); ++it)
    {
        if ((*it) == source)
        {
            (*it)->m_Channel->stop();
        }
    }
}

FMOD_VECTOR AudioManager::VectorToFmod(const glm::vec3& vPosition)
{
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}

float AudioManager::dbToVolume(float dB)
{
    return powf(10.0f, 0.05f * dB);
}

float AudioManager::VolumeToDb(float volume)
{
    return 20.0f * log10f(volume);
}