#include "AudioEngine.h"
#include <iostream>
#include <fstream>
#include <cstring>

AudioEngine::AudioEngine() 
    : device(nullptr), context(nullptr), musicSource(0), currentMusicBuffer(0),
      musicPlaying(false), musicVolume(0.5f), sfxVolume(1.0f), initialized(false) {
}

AudioEngine::~AudioEngine() {
    cleanup();
}

bool AudioEngine::initialize() {
    // Open default audio device
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open audio device" << std::endl;
        return false;
    }
    
    // Create context
    context = alcCreateContext(device, nullptr);
    if (!context) {
        std::cerr << "Failed to create audio context" << std::endl;
        alcCloseDevice(device);
        device = nullptr;
        return false;
    }
    
    // Make context current
    if (!alcMakeContextCurrent(context)) {
        std::cerr << "Failed to make context current" << std::endl;
        alcDestroyContext(context);
        alcCloseDevice(device);
        context = nullptr;
        device = nullptr;
        return false;
    }
    
    // Create sound sources
    for (int i = 0; i < MAX_SOURCES; i++) {
        ALuint source = createSource();
        if (source != 0) {
            sources.push_back(source);
        }
    }
    
    // Create music source
    musicSource = createSource();
    
    // Set default listener orientation
    ALfloat listenerOri[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, listenerOri);
    
    initialized = true;
    std::cout << "AudioEngine initialized successfully" << std::endl;
    return true;
}

void AudioEngine::cleanup() {
    if (!initialized) return;
    
    stopAllSounds();
    stopMusic();
    
    // Delete sources
    for (ALuint source : sources) {
        alDeleteSources(1, &source);
    }
    sources.clear();
    
    if (musicSource != 0) {
        alDeleteSources(1, &musicSource);
        musicSource = 0;
    }
    
    // Delete buffers
    for (auto& pair : soundBuffers) {
        alDeleteBuffers(1, &pair.second);
    }
    soundBuffers.clear();
    
    // Cleanup context and device
    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        context = nullptr;
    }
    
    if (device) {
        alcCloseDevice(device);
        device = nullptr;
    }
    
    initialized = false;
}

ALuint AudioEngine::createSource() {
    ALuint source;
    alGenSources(1, &source);
    
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "Failed to create audio source: " << error << std::endl;
        return 0;
    }
    
    // Set default source properties
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 1.0f);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    
    return source;
}

bool AudioEngine::loadWAVFile(const std::string& filename, ALuint& buffer) {
    // Simple WAV file loader (supports basic PCM WAV files)
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open audio file: " << filename << std::endl;
        return false;
    }
    
    // Read RIFF header
    char riff[4];
    file.read(riff, 4);
    if (std::strncmp(riff, "RIFF", 4) != 0) {
        std::cerr << "Invalid WAV file (no RIFF): " << filename << std::endl;
        return false;
    }
    
    uint32_t fileSize;
    file.read(reinterpret_cast<char*>(&fileSize), 4);
    
    char wave[4];
    file.read(wave, 4);
    if (std::strncmp(wave, "WAVE", 4) != 0) {
        std::cerr << "Invalid WAV file (no WAVE): " << filename << std::endl;
        return false;
    }
    
    // Find fmt chunk
    char fmt[4];
    file.read(fmt, 4);
    if (std::strncmp(fmt, "fmt ", 4) != 0) {
        std::cerr << "Invalid WAV file (no fmt): " << filename << std::endl;
        return false;
    }
    
    uint32_t fmtSize;
    file.read(reinterpret_cast<char*>(&fmtSize), 4);
    
    uint16_t audioFormat;
    file.read(reinterpret_cast<char*>(&audioFormat), 2);
    
    uint16_t numChannels;
    file.read(reinterpret_cast<char*>(&numChannels), 2);
    
    uint32_t sampleRate;
    file.read(reinterpret_cast<char*>(&sampleRate), 4);
    
    uint32_t byteRate;
    file.read(reinterpret_cast<char*>(&byteRate), 4);
    
    uint16_t blockAlign;
    file.read(reinterpret_cast<char*>(&blockAlign), 2);
    
    uint16_t bitsPerSample;
    file.read(reinterpret_cast<char*>(&bitsPerSample), 2);
    
    // Skip to data chunk
    file.seekg(36, std::ios::beg);
    
    char data[4];
    file.read(data, 4);
    if (std::strncmp(data, "data", 4) != 0) {
        std::cerr << "Invalid WAV file (no data): " << filename << std::endl;
        return false;
    }
    
    uint32_t dataSize;
    file.read(reinterpret_cast<char*>(&dataSize), 4);
    
    // Read audio data
    std::vector<char> audioData(dataSize);
    file.read(audioData.data(), dataSize);
    file.close();
    
    // Determine OpenAL format
    ALenum format;
    if (numChannels == 1) {
        format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else {
        format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }
    
    // Create OpenAL buffer
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, audioData.data(), dataSize, sampleRate);
    
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "Failed to load audio data: " << error << std::endl;
        return false;
    }
    
    return true;
}

bool AudioEngine::loadSound(const std::string& name, const std::string& filename) {
    if (!initialized) return false;
    
    // Check if already loaded
    if (soundBuffers.find(name) != soundBuffers.end()) {
        std::cout << "Sound already loaded: " << name << std::endl;
        return true;
    }
    
    ALuint buffer;
    if (!loadWAVFile(filename, buffer)) {
        // If WAV loading fails, create a silent buffer as fallback
        std::cerr << "Warning: Could not load " << filename << ", using silent buffer" << std::endl;
        
        // Create a small silent buffer
        alGenBuffers(1, &buffer);
        short silence[1024] = {0};
        alBufferData(buffer, AL_FORMAT_MONO16, silence, sizeof(silence), 22050);
    }
    
    soundBuffers[name] = buffer;
    std::cout << "Loaded sound: " << name << " from " << filename << std::endl;
    return true;
}

void AudioEngine::unloadSound(const std::string& name) {
    auto it = soundBuffers.find(name);
    if (it != soundBuffers.end()) {
        alDeleteBuffers(1, &it->second);
        soundBuffers.erase(it);
    }
}

ALuint AudioEngine::getAvailableSource() {
    for (ALuint source : sources) {
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            return source;
        }
    }
    return 0; // No available source
}

void AudioEngine::playSound(const std::string& name, float volume, float pitch, bool loop) {
    if (!initialized) return;
    
    auto it = soundBuffers.find(name);
    if (it == soundBuffers.end()) {
        std::cerr << "Sound not found: " << name << std::endl;
        return;
    }
    
    ALuint source = getAvailableSource();
    if (source == 0) {
        std::cerr << "No available audio source" << std::endl;
        return;
    }
    
    alSourcei(source, AL_BUFFER, it->second);
    alSourcef(source, AL_GAIN, volume * sfxVolume);
    alSourcef(source, AL_PITCH, pitch);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcePlay(source);
}

void AudioEngine::playSound3D(const std::string& name, float x, float y, float z, float volume) {
    if (!initialized) return;
    
    auto it = soundBuffers.find(name);
    if (it == soundBuffers.end()) {
        std::cerr << "Sound not found: " << name << std::endl;
        return;
    }
    
    ALuint source = getAvailableSource();
    if (source == 0) {
        std::cerr << "No available audio source" << std::endl;
        return;
    }
    
    alSourcei(source, AL_BUFFER, it->second);
    alSource3f(source, AL_POSITION, x, y, z);
    alSourcef(source, AL_GAIN, volume * sfxVolume);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcePlay(source);
}

void AudioEngine::stopAllSounds() {
    for (ALuint source : sources) {
        alSourceStop(source);
    }
}

void AudioEngine::playMusic(const std::string& filename, float volume, bool loop) {
    if (!initialized) return;
    
    stopMusic();
    
    ALuint buffer;
    if (!loadWAVFile(filename, buffer)) {
        std::cerr << "Failed to load music: " << filename << std::endl;
        return;
    }
    
    currentMusicBuffer = buffer;
    alSourcei(musicSource, AL_BUFFER, currentMusicBuffer);
    alSourcef(musicSource, AL_GAIN, volume);
    alSourcei(musicSource, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcePlay(musicSource);
    
    musicPlaying = true;
    musicVolume = volume;
}

void AudioEngine::stopMusic() {
    if (musicSource != 0) {
        alSourceStop(musicSource);
        musicPlaying = false;
    }
    
    if (currentMusicBuffer != 0) {
        alDeleteBuffers(1, &currentMusicBuffer);
        currentMusicBuffer = 0;
    }
}

void AudioEngine::pauseMusic() {
    if (musicSource != 0 && musicPlaying) {
        alSourcePause(musicSource);
    }
}

void AudioEngine::resumeMusic() {
    if (musicSource != 0 && musicPlaying) {
        alSourcePlay(musicSource);
    }
}

void AudioEngine::setMusicVolume(float volume) {
    musicVolume = volume;
    if (musicSource != 0) {
        alSourcef(musicSource, AL_GAIN, volume);
    }
}

void AudioEngine::setListenerPosition(float x, float y, float z) {
    alListener3f(AL_POSITION, x, y, z);
}

void AudioEngine::setListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ) {
    ALfloat ori[] = { atX, atY, atZ, upX, upY, upZ };
    alListenerfv(AL_ORIENTATION, ori);
}

void AudioEngine::setListenerVelocity(float x, float y, float z) {
    alListener3f(AL_VELOCITY, x, y, z);
}

void AudioEngine::setSFXVolume(float volume) {
    sfxVolume = volume;
}

void AudioEngine::update() {
    // Check if music finished and should loop
    if (musicPlaying && musicSource != 0) {
        ALint state;
        alGetSourcei(musicSource, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            musicPlaying = false;
        }
    }
}
