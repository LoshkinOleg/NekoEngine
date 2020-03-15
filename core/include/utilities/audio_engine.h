#ifndef _AUDIO_ENGINE_H_	//Prevents multiple definition of the objects
#define _AUDIO_ENGINE_H_

#include <mathematics/vector.h>
#include <string_view>

template<typename Sound, typename Channel, typename  Event, typename Bank, typename LoadBankFlag, typename LoadEventFlag, typename LoadSoundFlag, typename PlaySoundFlag, typename PlayEventFlag>
class AudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();

    void Update();

    //Loading
    void LoadBank(_STRING_VIEW_ bankName, LoadBankFlag flag);
    void LoadEvent(_STRING_VIEW_ eventName, LoadEventFlag flag);
    void LoadSound(_STRING_VIEW_ soundName, LoadSoundFlag flag);
    void UnloadSound(_STRING_VIEW_ soundName);

    //Playing/Stopping
    void PlaySound(_STRING_VIEW_ soundName, PlaySoundFlag flag);
    void PlayEvent(_STRING_VIEW_ eventName, PlayEventFlag flag);
    void StopChannel(int channelID);
    void StopEvent(_STRING_VIEW_ eventName);
    void StopAllChannels();

    bool IsPlaying(Channel channelID);
    bool IsEventPlaying(_STRING_VIEW_ eventName);
	
private:
    typedef std::map<std::string, Sound*> SoundMap;
    typedef std::map<std::string, Channel*> ChannelMap;
    typedef std::map<std::string, Event*> EventMap;
    typedef std::map<std::string, Bank*> BankMap;

    SoundMap soundMap;
    ChannelMap channelMap;
    EventMap eventMap;
    BankMap bankMap;
};

/*#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <iostream>
#include <map>
#include <string>
#include <vector>

//Vector3 struct to place sound in 3D space
struct Vector3		//TODO: Change with Neko Vector3s
{
    float x;
    float y;
    float z;
};

//It will do calls to the Implementation struct to start, stop and update FMOD
class AudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();
    void Update();

    void LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void LoadEvent(const std::string& eventName);
    void LoadSound(const std::string& soundName, bool is3d = true, bool isLooping = false, bool isStream = false);
    void UnLoadSound(const std::string& soundName);
    void Set3dListenerAndOrientation(const Vector3& position = Vector3{ 0, 0, 0 }, float volumeDB = 0.0f);
    int PlaySound(const std::string& soundName, const Vector3& position = Vector3{ 0, 0, 0 }, float volumeDB = 0.0f);
    void PlayEvent(const std::string& eventName);
    void StopChannel(int channelID);
    void StopEvent(const std::string& eventName, bool stopImmediate = false);
    void GetEventParameter(const std::string& eventName, const std::string& eventParameter, float* parameter);
    void SetEventParameter(const std::string& eventName, const std::string& parameterName, float eventValue);
    void StopAllChannels();
    void SetChannel3dPosition(int channelID, const Vector3& position);
    void SetChannelVolume(int channelID, float volumeDB);
    bool IsPlaying(int channelID);
    bool IsEventPlaying(const std::string& eventName);
    float DbToVolume(float dB);
    float VolumeTodB(float volume);
    FMOD_VECTOR VectorToFmod(const Vector3& position);
private:
    FMOD::Studio::System* fmodStudioSystem;
    FMOD::System* fmodSystem;
    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    typedef std::map<int, FMOD::Channel*> ChannelMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
    typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

    SoundMap soundMap;
    ChannelMap channelMap;
    EventMap eventMap;
    BankMap bankMap;
    int nextChannelID;
};*/
#endif
