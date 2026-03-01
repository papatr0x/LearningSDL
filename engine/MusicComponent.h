// Created by Patricio Palma on 09-03-26.

#ifndef LETSLEARNSDL_MUSICCOMPONENT_H
#define LETSLEARNSDL_MUSICCOMPONENT_H

#include "Component.h"
#include "AudioManager.h"
#include <string>

// Controls background music from within a scene's object hierarchy.
// If autoPlay is true, music starts automatically on onStart().
// Gain is a float multiplier: 1.0f = 100%, 0.5f = 50%.
// fadeInMs: duration of the fade-in in milliseconds (0 = no fade).
//
// Note: does NOT stop music on destruction — the scene that needs to stop
// music on unload should call stop() from Scene::unload(), or rely on the
// next scene's playMusic() which stops the current track automatically.
class MusicComponent : public Component {
public:
    MusicComponent(Object* owner, std::string id, std::string musicPath,
                   int loops = -1, bool autoPlay = true, int fadeInMs = 0, float gain = 1.0f)
        : Component(owner, std::move(id))
        , musicPath(std::move(musicPath))
        , loops(loops)
        , autoPlay(autoPlay)
        , fadeInMs(fadeInMs)
        , gain(gain) {}

    void onStart() noexcept override {
        if (autoPlay) play();
    }

    void play() {
        AudioManager::instance().playMusic(musicPath, loops, fadeInMs);
        AudioManager::instance().setMusicGain(gain);
    }

    // fadeOutMs: fade-out in milliseconds (0 = immediate stop).
    void stop(const int fadeOutMs = 0) { AudioManager::instance().stopMusic(fadeOutMs); }
    void pause()                       { AudioManager::instance().pauseMusic(); }
    void resume()                      { AudioManager::instance().resumeMusic(); }
    void setGain(const float g)        { gain = g; AudioManager::instance().setMusicGain(g); }

    void setMusicPath(std::string path) { musicPath = std::move(path); }

private:
    std::string musicPath;
    int         loops;
    bool        autoPlay;
    int         fadeInMs;
    float       gain;
};

#endif //LETSLEARNSDL_MUSICCOMPONENT_H
