#pragma once

#include "../ecs/world.hpp"
#include "application.hpp"
#include "forward-renderer.hpp"
#include <iostream>

namespace our {
    enum class GameState {
        LOSE,
        WIN,
        PLAY
    };

    // The System is responsible for opening drawers where keys might be hidden
    class LockedAwaySystem {
        double currentTime = 0;
        double endGameInterval = 20; // 2 minutes
        float x, y;
    public:
        // This should be called every frame to update all entities containing a CollisionComponent.
        void update(World *world, double deltaTime, GameState *gameState, ForwardRenderer *renderer) {
            currentTime += deltaTime;
            if (currentTime >= endGameInterval) {
                *gameState = GameState::LOSE;
            }
        }

        void showGUI(World *world) {

            ImGui::Begin("Timer");
            ImGui::InputFloat("X Position of Text", &x, 0.1, 0.2, 0);
            ImGui::InputFloat("Y Position of Text", &y, 0.1, 0.2, 0);
            ImGui::End();
        }

    };

}
