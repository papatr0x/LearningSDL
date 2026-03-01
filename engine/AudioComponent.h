// Created by Patricio Palma on 09-03-26.

#ifndef LETSLEARNSDL_AUDIOCOMPONENT_H
#define LETSLEARNSDL_AUDIOCOMPONENT_H

#include "Component.h"
#include "AudioManager.h"
#include <string>

// Plays a sound effect attached to an Object.
// Each AudioComponent owns one MIX_Track* created lazily on first play().
// Gain is a float multiplier: 1.0f = 100%, 0.5f = 50%.
class AudioComponent : public Component {
public:
    AudioComponent(Object* owner, std::string id, std::string soundPath, float gain = 1.0f)
        : Component(owner, std::move(id))
        , soundPath(std::move(soundPath))
        , gain(gain) {}

    ~AudioComponent() override {
        if (track) {
            MIX_StopTrack(track, 0);
            MIX_DestroyTrack(track);
        }
    }

    // Plays the sound. loops=0 plays once, -1 loops forever.
    void play(const int loops = 0) {
        if (!ensureTrack()) return;
        MIX_Audio* audio = AudioManager::instance().getOrLoadAudio(soundPath);
        if (!audio) return;
        MIX_SetTrackAudio(track, audio);
        MIX_SetTrackLoops(track, loops);
        MIX_PlayTrack(track, 0);
    }

    // fadeOutMs: fade-out in milliseconds (0 = immediate stop).
    void stop(const int fadeOutMs = 0) {
        if (track) MIX_StopTrack(track, fadeOutMs);
    }

    // gain: 1.0f = 100%, 0.5f = 50%
    void setGain(const float g) {
        gain = g;
        if (track) MIX_SetTrackGain(track, gain);
    }

    void setSoundPath(std::string path) { soundPath = std::move(path); }

private:
    // Returns true if the track is ready to use.
    bool ensureTrack() {
        if (track) return true;
        track = AudioManager::instance().createTrack();
        if (track) MIX_SetTrackGain(track, gain);
        return track != nullptr;
    }

    std::string soundPath;
    float       gain;
    MIX_Track*  track{};
};

#endif //LETSLEARNSDL_AUDIOCOMPONENT_H
