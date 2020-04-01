#include <AudioEngine.h>

AudioEngine::AudioEngine()
{
    FMOD::Studio::System::create(&fmodStudioSystem);
    fmodStudioSystem->initialize(MAX_CHANNELS, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL);
    fmodStudioSystem->getCoreSystem(&fmodSystem);
}

AudioEngine::~AudioEngine()
{
    fmodStudioSystem->unloadAll();
    fmodStudioSystem->release();
}

void AudioEngine::Update()
{
    std::vector<ChannelMap::iterator> stoppedChannels;
    //Erases stopped channelsMap
    for (auto it = channelMap.begin(), itEnd = channelMap.end(); it != itEnd; ++it)
    {
        bool isPlaying = false;
        it->second->isPlaying(&isPlaying);
        if (!isPlaying)
        {
            stoppedChannels.push_back(it);
        }
    }
    for (auto& it : stoppedChannels)
    {
        channelMap.erase(it);
    }
    fmodStudioSystem->update();
}

//Load Bank
void AudioEngine::LoadBank(std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)   //Banks stores all the soundsMap and information for each event
{
    const auto it = bankMap.find(bankName);
    if (it != bankMap.end())
        return;
    FMOD::Studio::Bank* bank;
    fmodStudioSystem->loadBankFile(bankName.c_str(), flags, &bank);
    if (bank) {
        bankMap[bankName] = bank;
    }
}

void AudioEngine::LoadEvent(std::string& eventName) {
    const auto it = eventMap.find(eventName);
    if (it != eventMap.end())
        return;
    FMOD::Studio::EventDescription* eventDescription = nullptr;
    AudioEngine::fmodStudioSystem->getEvent(eventName.c_str(), &eventDescription);
    if (eventDescription) {
        FMOD::Studio::EventInstance* eventInstance = nullptr;
        eventDescription->createInstance(&eventInstance);
        if (eventInstance) {
            eventMap[eventName] = eventInstance;
        }
    }
}

//Loads a sound with name, if he's in 3 Dimensions, looping or streaming
void AudioEngine::LoadSound(std::string &soundName, bool is3d, bool isLooping, bool isStream)
{
    const auto it = soundMap.find(soundName);
    if (it != soundMap.end())
        return;

    FMOD_MODE mode = FMOD_DEFAULT;
    mode |= is3d ? FMOD_3D : FMOD_2D;
    mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    fmodSystem->createSound(soundName.c_str(), mode, nullptr, &pSound);
    if (pSound) {
        soundMap[soundName] = pSound;
    }

}

//Unload soundsMap by name
void AudioEngine::UnLoadSound(std::string &soundName)
{
    auto foundIt = soundMap.find(soundName);
    if (foundIt == soundMap.end())
        return;

    foundIt->second->release();
    soundMap.erase(foundIt);
}

//Plays the selected sound
void AudioEngine::PlaySound(std::string& soundName, const Vector3& v3Position, float volumeDb)
{
    const ChannelId channelId = nextChannelID++;  //Creates a new channel ID
    auto it = soundMap.find(soundName);  //Search Sound
    if (it == soundMap.end())
    {
        LoadSound(soundName);
        it = soundMap.find(soundName);
        if (it == soundMap.end())
        {
            return;
        }
    }
    FMOD::Channel* channel = nullptr;
    fmodSystem->playSound(it->second, nullptr, true, &channel);
    if (channel)
    {
        FMOD_MODE currMode;
        it->second->getMode(&currMode);
        if (currMode & FMOD_3D) {
            FMOD_VECTOR fmodPosition = Vec3ToFmod(v3Position);
            channel->set3DAttributes(&fmodPosition, nullptr);
        }
        channel->setVolume(DbToVolume(volumeDb));
        channel->setPaused(false);
        channelMap[channelId] = channel;
    }
}

void AudioEngine::PlayEvent(std::string& eventName) {
    auto it = eventMap.find(eventName);
    if (it == eventMap.end()) {
        LoadEvent(eventName);
        it = eventMap.find(eventName);
        if (it == eventMap.end())
            return;
    }
    it->second->start();
}

void AudioEngine::StopChannel(int channelID)
{
    const auto it = channelMap.find(channelID);
	if (it == channelMap.end())
	{
        return;
	}
    it->second->stop();
}

void AudioEngine::StopEvent(std::string& eventName, bool isImmediate) {
    const auto it = eventMap.find(eventName);
    if (it == eventMap.end())
        return;

    FMOD_STUDIO_STOP_MODE stopMode;
    stopMode = isImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    it->second->stop(stopMode);
}


float AudioEngine::GetEventParameter(std::string& eventName, std::string& parameterName) {
    const auto it = eventMap.find(eventName);
    if (it == eventMap.end())
        return;

    float value = 0;
    it->second->getParameterByName(parameterName.c_str(), &value);

	return value;
}

void AudioEngine::SetEventParameter(std::string& eventName, std::string& parameterName, float parameterValue) {
    const auto it = eventMap.find(eventName);
    if (it == eventMap.end())
        return;

    it->second->setParameterByName(parameterName.c_str(), parameterValue);
}

void AudioEngine::StopAllChannels()
{
	for (const auto channel : channelMap)
	{
        StopChannel(channel.first);
	}
}

//Sets the channel position in 3D
void AudioEngine::SetChannel3dPosition(unsigned int channelID, const Vector3& position)
{
    auto it = channelMap.find(channelID);
    if (it == channelMap.end())
        return;

    FMOD_VECTOR fmodPosition = Vec3ToFmod(position);
    it->second->set3DAttributes(&fmodPosition, nullptr);
}

//Sets the channel volume
void AudioEngine::SetChannelVolume(unsigned int channelId, float volumeDB)
{
    const auto it = channelMap.find(channelId);
    if (it == channelMap.end())
        return;

    it->second->setVolume(volumeDB);
}

bool AudioEngine::IsPlaying(unsigned int channelID)
{
    const auto it = channelMap.find(channelID);
	if (it == channelMap.end())
	{
        return false;
	}

    auto isPlaying = false;

    it->second->isPlaying(&isPlaying);

    return isPlaying;
}

bool AudioEngine::IsEventPlaying(std::string& eventName) {
    auto it = eventMap.find(eventName);
    if (it == eventMap.end())
        return false;

    FMOD_STUDIO_PLAYBACK_STATE* state = nullptr;
    if (it->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
        return true;
    }
    return false;
}

void AudioEngine::SetListener(const Vector3& position, float volumeDB)
{
    FMOD_3D_ATTRIBUTES attributes;
    attributes.position = Vec3ToFmod(position);
    fmodStudioSystem->setListenerAttributes(0, &attributes);
}

FMOD_VECTOR AudioEngine::Vec3ToFmod(const Vector3& position)
{
    FMOD_VECTOR fmodVector;
    fmodVector.x = position.x;
    fmodVector.y = position.y;
    fmodVector.z = position.z;
    return fmodVector;
}