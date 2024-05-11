#pragma once

#include "../ecs/world.hpp"
#include <systems/drawer-opener.hpp>

namespace our {

    class AudioSystem {
    public:
        audio_wrapper::MiniAudioWrapper audioPlayer;
        double currentTime = 0;
        double lastPlayedTime = 0;
        double lastDuration;

        void update(World *world, float deltaTime, Application *app) {
            currentTime += deltaTime;
            std::string audioName = app->song.first;
            float duration = app->song.second;
            if (!audioName.empty() && currentTime - lastPlayedTime >= 1) {
                audioPlayer.playSong(audioName.c_str(), false);
                lastDuration = duration;
                lastPlayedTime = currentTime;
            }
            if (currentTime - lastPlayedTime >= lastDuration) {
                audioPlayer.stopSong();
            }
            // Resetting the song in order not to play it several times
            app->song = {"", 0};
        }
    };

}   
