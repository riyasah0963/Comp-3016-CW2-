#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>

// Audio engine using OpenAL for 3D spatial audio
class AudioEngine {
private:
    ALCdevice* device;
    ALCcontext* context;
    
    // Sound buffers
    std::map<std::string, ALuint> soundBuffers;
    
    // Sound sources (for playing multiple sounds simultaneously)
    std::vector<ALuint> sources;
    const int MAX_SOURCES = 32;
    
    // Background music
    ALuint musicSource;
    ALuint currentMusicBuffer;
    bool musicPlaying;
    float musicVolume;
    float sfxVolume;
    
    bool initialized;
    
    // Helper functions
    ALuint createSource();
    bool loadWAVFile(const std::string& filename, ALuint& buffer);
    ALuint getAvailableSource();
    
public:
    AudioEngine();
    ~AudioEngine();
    
    bool initialize();
    void cleanup();
    
    // Sound loading
    bool loadSound(const std::string& name, const std::string& filename);
    void unloadSound(const std::string& name);
    
    // Sound playback
    void playSound(const std::string& name, float volume = 1.0f, float pitch = 1.0f, bool loop = false);
    void playSound3D(const std::string& name, float x, float y, float z, float volume = 1.0f);
    void stopSound(const std::string& name);
    void stopAllSounds();
    
    // Music playback
    void playMusic(const std::string& filename, float volume = 0.5f, bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(float volume);
    
    // Listener (player) position for 3D audio
    void setListenerPosition(float x, float y, float z);
    void setListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ);
    void setListenerVelocity(float x, float y, float z);
    
    // Volume control
    void setMasterVolume(float volume);
    void setSFXVolume(float volume);
    float getMusicVolume() const { return musicVolume; }
    float getSFXVolume() const { return sfxVolume; }
    
    // Update (call every frame)
    void update();
    
    bool isInitialized() const { return initialized; }
};
