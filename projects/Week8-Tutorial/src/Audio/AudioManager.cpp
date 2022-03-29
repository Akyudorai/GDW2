#include "AudioManager.h"

#include "Logging.h"

#include "fmod_studio.hpp"
#include "fmod_errors.h"


#pragma region Audio Object

FMOD_VECTOR AudioObject::VectorToFmod(const glm::vec3& vec)
{
    FMOD_VECTOR fVec;
    fVec.x = vec.x;
    fVec.y = vec.y;
    fVec.z = vec.z;
    return fVec;
}

glm::vec3 AudioObject::FmodToVector(const FMOD_VECTOR& vec)
{
    glm::vec3 gVec;
    gVec.x = vec.x;
    gVec.y = vec.y;
    gVec.z = vec.z;
    return gVec;
}

float AudioObject::dbToVolume(float db)
{
    return powf(10.0f, 0.05f * db);
}

float AudioObject::VolumeToDb(float vol)
{
    return 20.0f * log10f(vol);
}

int AudioObject::ErrorCheck(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        LOG_ERROR("FMOD ERROR: " + std::string(FMOD_ErrorString(result)));
        return 1;
    }

    return 0;
}

#pragma endregion

#pragma region Audio Bus

AudioBus::AudioBus(FMOD::Studio::Bus* bus)
    : m_Bus(bus)
{ }

bool AudioBus::CheckPaused()
{
    bool paused;
    ErrorCheck(m_Bus->getPaused(&paused));
    return paused;
}

void AudioBus::SetPaused(const bool& pause)
{
    ErrorCheck(m_Bus->setPaused(pause));
}

float AudioBus::GetVolume()
{
    float volume;
    ErrorCheck(m_Bus->getVolume(&volume));
    return volume;
}

void AudioBus::SetVolume(const float& newVolume)
{
    ErrorCheck(m_Bus->setVolume(newVolume));
}

bool AudioBus::CheckMuted()
{
    bool muted;
    ErrorCheck(m_Bus->getMute(&muted));
    return muted;
}

void AudioBus::SetMute(const bool& mute)
{
    ErrorCheck(m_Bus->setMute(mute));
}

void AudioBus::StopAllEvents(const bool& fade)
{
    if (fade) 
    {
        ErrorCheck(m_Bus->stopAllEvents(FMOD_STUDIO_STOP_ALLOWFADEOUT));
    }

    else
    {
        ErrorCheck(m_Bus->stopAllEvents(FMOD_STUDIO_STOP_IMMEDIATE));
    }
}

#pragma endregion

#pragma region Audio Listener

void AudioListener::SetPosition(const glm::vec3& pos)
{
    // Convert glm vec to fmod vec
    FMOD_VECTOR newPos = VectorToFmod(pos);

    // Update our member data
    m_AttenuationPosition = newPos;
    m_Attributes.position = newPos;

    // Update in fmod studio
    ErrorCheck(m_StudioSystem->setListenerAttributes(m_ID, &m_Attributes, &m_AttenuationPosition));
}

glm::vec3 AudioListener::GetPosition()
{
    return FmodToVector(m_Attributes.position);
}

void AudioListener::SetVelocity(const glm::vec3& vel)
{
    // Convert glm vec to fmod vec
    FMOD_VECTOR newVel = VectorToFmod(vel);

    // Update our member data
    m_Attributes.velocity = newVel;

    // Update in fmod studio
    ErrorCheck(m_StudioSystem->setListenerAttributes(m_ID, &m_Attributes, &m_AttenuationPosition));
}

glm::vec3 AudioListener::GetVelocity()
{
    return FmodToVector(m_Attributes.velocity);
}

void AudioListener::SetForward(const glm::vec3& forward)
{
    // Convert glm vec to fmod vec
    FMOD_VECTOR newForward = VectorToFmod(forward);

    // Update our member data
    m_Attributes.forward = newForward;

    // Update in fmod studio
    ErrorCheck(m_StudioSystem->setListenerAttributes(m_ID, &m_Attributes, &m_AttenuationPosition));
}

glm::vec3 AudioListener::GetForward()
{
    return FmodToVector(m_Attributes.forward);
}

void AudioListener::SetUp(const glm::vec3& up)
{
    // Convert glm vec to fmod vec
    FMOD_VECTOR newUp = VectorToFmod(up);

    // Update our member data
    m_Attributes.up = newUp;

    // Update in fmod studio
    ErrorCheck(m_StudioSystem->setListenerAttributes(m_ID, &m_Attributes, &m_AttenuationPosition));
}

glm::vec3 AudioListener::GetUp()
{
    return FmodToVector(m_Attributes.up);
}

void AudioListener::SetID(const int& id)
{
    m_ID = id;
}

#pragma endregion

#pragma region Audio Event

AudioEvent::AudioEvent(FMOD::Studio::EventInstance* eventInstance)
    : m_EventInstance(eventInstance)
{ }

AudioEvent::~AudioEvent()
{ }

void AudioEvent::Play()
{
    if (IsPlaying()) return;

    // Start the event
    ErrorCheck(m_EventInstance->start());
}

void AudioEvent::Restart()
{
    // Start the event
    ErrorCheck(m_EventInstance->start());
}

void AudioEvent::Stop()
{
    ErrorCheck(m_EventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT));
    
}

void AudioEvent::StopImmediately()
{
    ErrorCheck(m_EventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE));
}

bool AudioEvent::IsPlaying()
{
    // Check if already playing
    FMOD_STUDIO_PLAYBACK_STATE state;
    ErrorCheck(m_EventInstance->getPlaybackState(&state));

    if (state == FMOD_STUDIO_PLAYBACK_PLAYING) return true;
    return false;
}

bool AudioEvent::CheckPaused()
{
    bool paused;
    ErrorCheck(m_EventInstance->getPaused(&paused));
    return paused;
}

void AudioEvent::SetPaused(const bool& pause)
{
    if (IsPlaying()) ErrorCheck(m_EventInstance->setPaused(pause));
}

void AudioEvent::SetParameter(const char* name, const float& value, const bool& ignoreSeekSpeed)
{
    ErrorCheck(m_EventInstance->setParameterByName(name, value, ignoreSeekSpeed));
}

float AudioEvent::GetParameterValue(const char* name)
{
    // Make float for output
    float value;

    // Put value into float
    ErrorCheck(m_EventInstance->getParameterByName(name, &value));

    // Return float
    return value;
}

void AudioEvent::SetPosition(const glm::vec3& pos)
{
    // Update our attributes
    m_Attributes.position = VectorToFmod(pos);

    // Send update to fmod
    ErrorCheck(m_EventInstance->set3DAttributes(&m_Attributes));
}

glm::vec3 AudioEvent::GetPosition()
{
    return FmodToVector(m_Attributes.position);
}

float AudioEvent::GetVolume()
{
    float volume;
    ErrorCheck(m_EventInstance->getVolume(&volume));
    return volume;
}

void AudioEvent::SetVolume(const float& newVolume)
{
    ErrorCheck(m_EventInstance->setVolume(newVolume));
}

#pragma endregion

#pragma region Audio Engine

AudioEngine& AudioEngine::Instance()
{
    static AudioEngine instance;
    return instance;
}

void AudioEngine::Init()
{
    // Initialize the audio systems in FMOD
    ErrorCheck(FMOD::Studio::System::create(&m_StudioSystem));
    ErrorCheck(m_StudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

    // Save ref to core system
    ErrorCheck(m_StudioSystem->getCoreSystem(&m_System));

    // Set one listener, makes its ID 0
    ErrorCheck(m_StudioSystem->setNumListeners(1));
    m_Listener.SetID(0);
    m_Listener.m_StudioSystem = m_StudioSystem;

    // Get the attributes
    ErrorCheck(m_StudioSystem->getListenerAttributes(0, &m_Listener.m_Attributes, &m_Listener.m_AttenuationPosition));
}

void AudioEngine::Update()
{
    // Update FMOD System
    ErrorCheck(m_StudioSystem->update());
}

void AudioEngine::Shutdown()
{
    // Delete Events
    for (auto event : m_EventMap)
    {
        delete event.second;
    }
    m_EventMap.clear();

    // Delete busses
    for (auto bus : m_BusMap)
    {
        delete bus.second;
    }   
    m_BusMap.clear();

    AudioEngine::ErrorCheck(m_StudioSystem->unloadAll());
    AudioEngine::ErrorCheck(m_StudioSystem->release());
}

void AudioEngine::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    // Make sure the bank isn't already loaded
    auto bank = m_BankMap.find(bankName);
    if (bank != m_BankMap.end())
        return;

    // Create new bank
    FMOD::Studio::Bank* newBank;

    // Load the bank with FMOD
    ErrorCheck(m_StudioSystem->loadBankFile(("audio/banks/" + bankName + ".bank").c_str(), flags, &newBank));

    // Store the bank in our map so we can use it later
    if (newBank)
    {
        m_BankMap[bankName] = newBank;
    }
}

void AudioEngine::LoadBankWithString(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    LoadBank(strBankName, flags);
    LoadBank(strBankName + ".strings", flags);
}

AudioListener& AudioEngine::GetListener()
{
    return m_Listener;
}

AudioEvent& AudioEngine::CreateSoundEvent(const std::string& eventName, const std::string& GUID)
{
    // Get find event in file
    FMOD::Studio::EventDescription* eventDescription = NULL;
    ErrorCheck(m_StudioSystem->getEvent(GUID.c_str(), &eventDescription));

    // If the event exists
    if (eventDescription)
    {
        // USe the event description to make and return an instance
        FMOD::Studio::EventInstance* newFmodEvent = nullptr;
        ErrorCheck(eventDescription->createInstance(&newFmodEvent));

        // Create an audio event
        AudioEvent* newAudioEvent = new AudioEvent(newFmodEvent);

        // Make sure multiple events with the same name aren't created
        if (m_EventMap.find(eventName) != m_EventMap.end()) {
            LOG_ASSERT(m_EventMap.find(eventName) != m_EventMap.end(), "Event with that name already exists!");
        }

        // Add the audio event to our map
        m_EventMap[eventName] = newAudioEvent;

        return *newAudioEvent;
    }
}

AudioEvent& AudioEngine::GetEvent(const std::string& eventName)
{
    // If the event exists
    if (m_EventMap.find(eventName) != m_EventMap.end())
    {
        return *m_EventMap.at(eventName);
    }
    else
    {
        LOG_ERROR("No such event exists.");
    }
}

void AudioEngine::LoadBus(const std::string& busName, const std::string& GUID)
{
    // Find the bus in fmod
    FMOD::Studio::Bus* fmodBus = nullptr;
    ErrorCheck(m_StudioSystem->getBus(GUID.c_str(), &fmodBus));

    // Make sure we're not adding a duplicate name
    if (m_BusMap.find(busName) != m_BusMap.end()) {
        LOG_ASSERT(m_BusMap.find(busName) != m_BusMap.end(), "Bus with that name already exists!");
    }

    // Create a new Audio Bus
    AudioBus* newBus = new AudioBus(fmodBus);

    // Add the bus to our map
    m_BusMap[busName] = newBus;
}

AudioBus& AudioEngine::GetBus(const std::string& busName)
{
    // If the bus exists
    if (m_BusMap.find(busName) != m_BusMap.end())
    {
        return *m_BusMap.at(busName);
    }

    else
    {
        LOG_ERROR("No such bus exists.");
    }
}

void AudioEngine::PauseEverything()
{
    for (auto bus : m_BusMap)
    {
        bus.second->SetPaused(true);
    }
}

void AudioEngine::UnPauseEverything()
{
    for (auto bus : m_BusMap)
    {
        bus.second->SetPaused(false);
    }
}

void AudioEngine::StopAllSounds()
{
    for (auto bus : m_BusMap)
    {
        bus.second->StopAllEvents();
    }
}

void AudioEngine::SetGlobalParam(const char* name, const float& value, const bool& ignoreSeekSpeed)
{
    ErrorCheck(m_StudioSystem->setParameterByName(name, value, ignoreSeekSpeed));
}

float AudioEngine::GetGlobalParamValue(const char* name)
{
    // Make float for output
    float value;

    // Put value in float
    ErrorCheck(m_StudioSystem->getParameterByName(name, &value));

    return value;
}

#pragma endregion

/* OLD ENGINE

bool AudioManager::Init()
{
    // Initialize FMOD Studio System
    FMOD:Studio::System::create(&instance().fmStudio);
    instance().fmStudio->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, NULL);
    
    instance().fmStudio->getCoreSystem(&instance().fmSystem);

    // Initialize FMOD System
    //AudioManager::ErrorCheck(System_Create(&instance().fmSystem), __LINE__);
    //AudioManager::ErrorCheck(instance().fmSystem->init(32, FMOD_INIT_NORMAL, 0), __LINE__);
    

    // Loading Bank
    AudioManager::ErrorCheck(instance().fmStudio->loadBankFile("audio/Master.bank", FMOD_STUDIO_LOAD_BANK_NONBLOCKING, &instance().masterBank), __LINE__);
    if (!instance().masterBank) { LOG_ERROR("####### Failed to load a master bank #######"); }
    
    AudioManager::ErrorCheck(instance().fmStudio->loadBankFile("audio/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NONBLOCKING, &instance().masterStringBank), __LINE__);
    if (!instance().masterStringBank) { LOG_ERROR("####### Failed to load a master string bank #######"); }

    AudioManager::ErrorCheck(instance().fmStudio->loadBankFile("audio/SFX.bank", FMOD_STUDIO_LOAD_BANK_NONBLOCKING, &instance().soundBank), __LINE__);
    if (!instance().soundBank) { LOG_ERROR("####### Failed to load a SFX bank #######"); }

 
    FMOD::Studio::EventDescription* arr[512];
    int arrCount = 0;
    AudioManager::ErrorCheck(instance().soundBank->getEventCount(&arrCount), __LINE__);
    LOG_INFO("Sound Bank Arr Count: " + std::to_string(arrCount));
    AudioManager::ErrorCheck(instance().soundBank->getEventList(&*arr, 512, &arrCount), __LINE__);
    LOG_INFO("Sound Bank Event Count: " + std::to_string(arrCount));


    AudioManager::ErrorCheck(instance().masterBank->getEventCount(&arrCount), __LINE__);
    LOG_INFO("Master Bank Arr Count: " + std::to_string(arrCount));
    AudioManager::ErrorCheck(instance().masterBank->getEventList(&*arr, 512, &arrCount), __LINE__);
    LOG_INFO("Master Bank Event Count: " + std::to_string(arrCount));

    AudioManager::ErrorCheck(instance().masterStringBank->getEventCount(&arrCount), __LINE__);
    LOG_INFO("Master String Bank Arr Count: " + std::to_string(arrCount));
    AudioManager::ErrorCheck(instance().masterStringBank->getEventList(&*arr, 512, &arrCount), __LINE__);
    LOG_INFO("Master String Bank Event Count: " + std::to_string(arrCount));
    

    

    FMOD::Studio::EventDescription* eventDescription;
    instance().fmStudio->getEvent("event:/DiscreteParameters", &eventDescription);

    if (!eventDescription)
    {
        LOG_ERROR("Failed to load event description");
    }


    // Loading Event
    //LoadEvent("event:/DiscreteParameters");

    // Load Param
    float result;
    GetEventParameter("DiscreteParamters", "Swap1", &result);
    LOG_INFO("Paramater::Swap1 result = " + std::to_string(result));

    return true;
}

void AudioManager::Update()
{
    instance().fmSystem->update();
    instance().fmStudio->update();
}

void AudioManager::Shutdown()
{
    instance().fmStudio->unloadAll();
    instance().fmStudio->release();

    instance().fmSystem->close();
    instance().fmSystem->release();
}

int AudioManager::ErrorCheck(FMOD_RESULT result, int line)
{
    if (result != FMOD_OK)
    {
        LOG_ERROR(std::string(FMOD_ErrorString(result)) + " line " + std::to_string(line));
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
    return nullptr;
}

Sound* AudioManager::LoadSound(std::string path, AudioSettings settings)
{
    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= settings.Is3D ? FMOD_3D : FMOD_2D;
    eMode |= settings.IsLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= settings.IsStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    Sound* result = nullptr;

    AudioManager::ErrorCheck(instance().fmSystem->createSound(path.c_str(), eMode, nullptr, &result), __LINE__);
    if (result)
    {
        return result;
    }


    LOG_ERROR("Sound failed to load.")
        return nullptr;
}

FMOD::Studio::Bank* AudioManager::LoadBank(const std::string& filename, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    FMOD::Studio::Bank* bank;
    AudioManager::ErrorCheck(instance().fmStudio->loadBankFile(filename.c_str(), flags, &bank), __LINE__);
    if (instance().soundBank) {
        LOG_INFO("LoadBank::SUCCESS");
        return bank;
        
    }
    else {
        LOG_ERROR("LoadBank::FAILED");
        return nullptr;
    }

}

void AudioManager::LoadEvent(const std::string& eventName)
{
    FMOD::Studio::EventDescription* eventDescription = NULL;

    FMOD::Studio::EventDescription* edArr[512];
    int arrCount = 0;
    instance().soundBank->getEventCount(&arrCount);

    LOG_INFO("List Count:" + std::to_string(arrCount));

    instance().soundBank->getEventList(&*edArr, 512, &arrCount);
    
    LOG_INFO("List Info Count: " + std::to_string(arrCount));


    AudioManager::ErrorCheck(instance().fmStudio->getEvent("event:/DiscreteParameters", &eventDescription), __LINE__);

    if (eventDescription)
    {
        FMOD::Studio::EventInstance* eventInstance = NULL;
        AudioManager::ErrorCheck(eventDescription->createInstance(&eventInstance), __LINE__);

        if (eventInstance)
        {
            instance().Events.emplace(eventName, eventInstance);
        }
        else
        {
            LOG_ERROR("LoadEvent::Event Instance not created.");
        }        
    }
    else
    {
        LOG_ERROR("LoadEvent::Event Description not found!");
    }
}

void AudioManager::PlayEvent(const std::string& eventName)
{
    auto tFound = instance().Events.find(eventName);
    if (tFound != instance().Events.end()) {
        AudioManager::ErrorCheck(tFound->second->start(), __LINE__);
        LOG_INFO("PlayEvent::SUCCESS!");
    }
    else
    {
        LOG_ERROR("PlayEvent::EVENT NOT FOUND")
    }
}

void AudioManager::StopEvent(const std::string& eventName, bool bImmediate)
{
    auto tFound = instance().Events.find(eventName);
    if (tFound != instance().Events.end()) {
        
        FMOD_STUDIO_STOP_MODE eMode;
        eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
        AudioManager::ErrorCheck(tFound->second->stop(eMode), __LINE__);
        LOG_INFO("StopEvent::SUCCESS!");
    }
}

void AudioManager::GetEventParameter(const std::string& eventName, const std::string& paramName, float* param)
{
    auto tFound = instance().Events.find(eventName);
    if (tFound != instance().Events.end()) {
        AudioManager::ErrorCheck(tFound->second->getParameterByName(paramName.c_str(), param), __LINE__);
        LOG_INFO("GetEventParameter::SUCCESS");
    }
    else
    {
        LOG_ERROR("GetEventParameter::FAILED");
    }
}

void AudioManager::SetEventParameter(const std::string& eventName, const std::string& paramName, float value)
{

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

void AudioManager::Play(std::string path)
{
    Sound* sound = LoadSound(path, { false, false, false });
    Channel* channel;
    
    AudioManager::ErrorCheck(fmSystem->playSound(sound, nullptr, true, &channel), __LINE__);
    if (channel)
    {
        FMOD_MODE currMode;
        sound->getMode(&currMode);
        
        AudioManager::ErrorCheck(channel->setVolume(dbToVolume(1.0f)), __LINE__);
        AudioManager::ErrorCheck(channel->setPaused(false), __LINE__);
    }

}

void AudioManager::PlaySource(AudioSource* source, float fVolume)
{
    CurrentSounds.push_back(source);

    FMOD::ChannelGroup* masterGroup;
    FMOD::DSP* dsp_echo;
    FMOD::DSP* dsp_lowPass;
    FMOD::DSP* dsp_highPass;
    FMOD::DSP* dsp_flange;

    fmSystem->getMasterChannelGroup(&masterGroup);
    
    // Create effects to play with
    fmSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp_echo);
    fmSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &dsp_lowPass);
    fmSystem->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &dsp_highPass);
    fmSystem->createDSPByType(FMOD_DSP_TYPE_FLANGE, &dsp_flange);

    // Add them to master channel group
    masterGroup->addDSP(0, dsp_echo);
    masterGroup->addDSP(0, dsp_lowPass);
    masterGroup->addDSP(0, dsp_highPass);
    masterGroup->addDSP(0, dsp_flange);

    // By default, bbypass all effects.  This means let the original signal go therough without processing.  
    // It will sound 'dry' until effects are enabled by the user
    dsp_echo->setBypass(false);
    dsp_lowPass->setBypass(true);
    dsp_highPass->setBypass(true);
    dsp_flange->setBypass(true);

    


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

void AudioManager::ClearSounds()
{
    std::vector<AudioSource*>::iterator it;
    for (auto it = CurrentSounds.begin(); it != CurrentSounds.end(); ++it)
    {
        (*it)->m_Channel->stop();
    }

    CurrentSounds.clear();
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

*/