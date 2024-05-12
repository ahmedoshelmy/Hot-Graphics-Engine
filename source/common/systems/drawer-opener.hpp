#pragma once

#include "../ecs/world.hpp"
#include "application.hpp"
#include "components/knob-component.hpp"
#include "forward-renderer.hpp"
#include "../systems/text-renderer.hpp"
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
        update(World *world, Application *app, TextRenderer *renderer, double deltaTime) {
            // For each entity in the world
            currentTime += deltaTime;
            Entity *entity = world->getEntity(app->pickedObject);
            if (!entity)return;
            auto *knobComponent = entity->getComponent<KnobComponent>();
            if (!knobComponent) return;

            
            renderer->addTextCommand("Right Click to Open it", 0.05, 1500, 100, 0.6, glm::vec3(0.96, 1.0, 0.78), -1, 1);
            
            // if (Entity *crosshair = world->getEntity("CrossHair"); crosshair) {
            //     auto crossComponent = crosshair->getComponent<MeshRendererComponent>();
            //     auto crossMaterial = dynamic_cast<TintedMaterial *>(crossComponent->material);
            //         crossMaterial->tint = glm::vec4(0.45, 0.9, 0.5, 0.5);
            //     else
            //         crossMaterial->tint = glm::vec4(0.45, 0.4, 0.5, 0.5);
            // }
            // Here there should be delay between opening and closing (Toggling effect) To make it more realistic
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_RIGHT) && prevTime + 2 < currentTime) {
                auto &position = entity->localTransform.position;
                auto &rotation = entity->localTransform.rotation;
                if (knobComponent->open) {
                    position.x -= 2;
                    position.z -= 2;
                    rotation.y += glm::pi<float>() / 2.0;
                    knobComponent->open ^= 1;
                    prevTime = currentTime;
                } else if (knobComponent->key == app->inHandItem) {
                    position.x += 2;
                    position.z += 2;
                    rotation.y -= glm::pi<float>() / 2.0;
                    knobComponent->open ^= 1;
                    prevTime = currentTime;
                    if (entity->name == "door8" || entity->name == "door6") {
                        app->song = {"assets/music/Terror Transition.mp3", 8};
                    } else if (entity->name == "door3" || entity->name == "door4") {
                        app->song = {"assets/music/JUMPSCARE.mp3", 8};
                    } else if (entity->name == "door5" || entity->name == "door3") {
                        app->song = {"assets/music/Scary scream.mp3", 8};
                    } else {
                        app->song = {"assets/music/HORROR DOOR OPENING SOUND EFFECT.mp3", 1.5};
                    }

                }
            }
        }

    };

}
