#pragma once

#include "../ecs/world.hpp"
#include "application.hpp"
#include "components/knob-component.hpp"
#include "forward-renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>

namespace our {

    // The System is responsible for opening drawers where keys might be hidden
    class DrawerOpenerSystem {
        double currentTime = 0;
        double prevTime = 0;
    public:

        // This should be called every frame to update all entities containing a CollisionComponent.
        void
        update(World *world, Application *app, const std::string &pickedObject, const std::string &objectInHand,
               std::string & songName, float & songDuration,  ForwardRenderer *renderer,
               double deltaTime) {
            // For each entity in the world
            currentTime += deltaTime;
            Entity *entity = world->getEntity(pickedObject);
            if (!entity)return;
            auto *knobComponent = entity->getComponent<KnobComponent>();
            if (!knobComponent) return;
            // Here there should be delay between opening and closing (Toggling effect) To make it more realistic
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_RIGHT) && prevTime + 2 < currentTime) {
                auto &position = entity->localTransform.position;
                auto &rotation = entity->localTransform.rotation;
                if (knobComponent->open) {
                    position.x -= 2;
                    position.z -= 2;
                    rotation.y += glm::pi<float>() / 2.0;
//                    rotation = knobComponent->closedRotation;
                    knobComponent->open ^= 1;
                    prevTime = currentTime;
                } else if (knobComponent->key == objectInHand) {
                    position.x += 2;
                    position.z += 2;
                    rotation.y -= glm::pi<float>() / 2.0;
//                    rotation = knobComponent->openRotation;
                    knobComponent->open ^= 1;
                    prevTime = currentTime;
                    songDuration = 1.5;
                    songName ="assets/music/HORROR DOOR OPENING SOUND EFFECT.mp3";
                }
            }
        }

    };

}
