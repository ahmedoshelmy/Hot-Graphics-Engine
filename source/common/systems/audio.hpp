#pragma once

#include "../ecs/world.hpp"
#include <systems/drawer-opener.hpp>

namespace our {

    class AudioSystem {
    public:
        audio_wrapper::MiniAudioWrapper audioPlayer;
        double currentTime = 0;
        double lastPlayedTime = 0;
        double lastDuration ;
        void update(World *world, float deltaTime, const std::string &audioName, const float &duration) {
            currentTime += deltaTime;
            if (!audioName.empty()) {
                audioPlayer.playSong(audioName.c_str());
                lastDuration = duration;
                lastPlayedTime = currentTime;
            }
            if (currentTime - lastPlayedTime >= lastDuration) {
                audioPlayer.stopSong();
            }
        }
    };

}   
