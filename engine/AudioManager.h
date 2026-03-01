// Created by Patricio Palma on 09-03-26.

#ifndef LETSLEARNSDL_AUDIOMANAGER_H
#define LETSLEARNSDL_AUDIOMANAGER_H

#include <SDL3_mixer/SDL_mixer.h>
#include <string>
#include <memory>
#include <unordered_map>

// Persistent singleton (lifetime = engine lifetime, like TextureManager/FontManager).
//
// Model: MIX_Mixer owns MIX_Track* and MIX_Audio*.
//   - Music: one dedicated MIX_Track* that loops.
//   - Fire-and-forget SFX: MIX_PlayAudio() (no control after play).
//   - Controllable SFX: AudioComponent creates its own MIX_Track via createTrack().
//
// Gain is a float multiplier: 1.0f = 100%, 0.5f = 50%.
class AudioManager {
public:
    AudioManager(const AudioManager&)            = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    static AudioManager& instance();

    bool init();
    // Must be called after all MIX_Track* owned by components are destroyed
    // (i.e. after activeScene.reset() in GameEngine::shutdown()).
    void unloadAll();

    // --- Background music (one looping track) ---
    // fadeInMs: duration of the fade-in in milliseconds (0 = no fade).
    void playMusic(const std::string& path, int loops = -1, int fadeInMs = 0);
    // fadeOutMs: duration of fade-out in milliseconds (0 = immediate stop).
    void stopMusic(int fadeOutMs = 0);
    void pauseMusic();
    void resumeMusic();
    void setMusicGain(float gain);

    // --- Fire-and-forget SFX (no control after play) ---
    void playSound(const std::string& path);

    // Creates a new child track owned by the caller. Must be destroyed with
    // MIX_DestroyTrack() before AudioManager::unloadAll() is called.
    MIX_Track* createTrack();

    MIX_Audio* getOrLoadAudio(const std::string& path);

private:
    AudioManager() = default;

    struct AudioDeleter { void operator()(MIX_Audio* a) const { MIX_DestroyAudio(a); } };
    using AudioPtr = std::unique_ptr<MIX_Audio, AudioDeleter>;

    MIX_Mixer* mixer{};
    MIX_Track* musicTrack{};
    std::unordered_map<std::string, AudioPtr> audioCache;
};

#endif //LETSLEARNSDL_AUDIOMANAGER_H
