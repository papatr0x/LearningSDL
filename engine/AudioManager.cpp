// Created by Patricio Palma on 09-03-26.

#include "AudioManager.h"
#include <iostream>

AudioManager& AudioManager::instance() {
    static AudioManager inst;
    return inst;
}

bool AudioManager::init() {
    if (!MIX_Init()) {
        std::cerr << "MIX_Init failed: " << SDL_GetError() << "\n";
        return false;
    }
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer) {
        std::cerr << "MIX_CreateMixerDevice failed: " << SDL_GetError() << "\n";
        MIX_Quit();
        return false;
    }
    musicTrack = MIX_CreateTrack(mixer);
    if (!musicTrack) {
        std::cerr << "MIX_CreateTrack (music) failed: " << SDL_GetError() << "\n";
        MIX_DestroyMixer(mixer);
        mixer = nullptr;
        MIX_Quit();
        return false;
    }
    std::clog << "AudioManager init succeeded\n";
    return true;
}

void AudioManager::unloadAll() {
    if (musicTrack) {
        MIX_StopTrack(musicTrack, 0);
        MIX_DestroyTrack(musicTrack);
        musicTrack = nullptr;
    }
    audioCache.clear();  // AudioDeleter calls MIX_DestroyAudio on each entry

    if (mixer) {
        MIX_DestroyMixer(mixer);
        mixer = nullptr;
    }
    MIX_Quit();
}

// --- Music ---

void AudioManager::playMusic(const std::string& path, const int loops, const int fadeInMs) {
    MIX_Audio* audio = getOrLoadAudio(path);
    if (!audio || !musicTrack) return;

    MIX_StopTrack(musicTrack, 0);
    MIX_SetTrackAudio(musicTrack, audio);
    MIX_SetTrackLoops(musicTrack, loops);

    SDL_PropertiesID props = 0;
    if (fadeInMs > 0) {
        props = SDL_CreateProperties();
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fadeInMs);
        SDL_SetFloatProperty(props, MIX_PROP_PLAY_FADE_IN_START_GAIN_FLOAT, 0.0f);
    }

    if (!MIX_PlayTrack(musicTrack, props))
        std::cerr << "MIX_PlayTrack (music) failed: " << SDL_GetError() << "\n";

    if (props) SDL_DestroyProperties(props);
}

void AudioManager::stopMusic(const int fadeOutMs) {
    if (!musicTrack) return;
    // MIX_StopTrack takes fade-out in sample frames; convert ms to frames via SDL_AudioSpec if needed.
    // For simplicity we use 0 (immediate) or let the caller pass 0 for now.
    // A full ms-to-frames conversion requires knowing the mixer sample rate.
    MIX_StopTrack(musicTrack, fadeOutMs);
}

void AudioManager::pauseMusic()                    { if (musicTrack) MIX_PauseTrack(musicTrack); }
void AudioManager::resumeMusic()                   { if (musicTrack) MIX_ResumeTrack(musicTrack); }
void AudioManager::setMusicGain(const float gain)  { if (musicTrack) MIX_SetTrackGain(musicTrack, gain); }

// --- Fire-and-forget SFX ---

void AudioManager::playSound(const std::string& path) {
    MIX_Audio* audio = getOrLoadAudio(path);
    if (!audio || !mixer) return;
    if (!MIX_PlayAudio(mixer, audio))
        std::cerr << "MIX_PlayAudio failed [" << path << "]: " << SDL_GetError() << "\n";
}

// --- Track factory ---

MIX_Track* AudioManager::createTrack() {
    if (!mixer) return nullptr;
    MIX_Track* track = MIX_CreateTrack(mixer);
    if (!track)
        std::cerr << "MIX_CreateTrack failed: " << SDL_GetError() << "\n";
    return track;
}

// --- Audio cache ---

MIX_Audio* AudioManager::getOrLoadAudio(const std::string& path) {
    const auto it = audioCache.find(path);
    if (it != audioCache.end()) return it->second.get();

    MIX_Audio* raw = MIX_LoadAudio(mixer, path.c_str(), true);
    if (!raw) {
        std::cerr << "MIX_LoadAudio failed [" << path << "]: " << SDL_GetError() << "\n";
        return nullptr;
    }
    audioCache.emplace(path, AudioPtr{raw});
    return raw;
}
