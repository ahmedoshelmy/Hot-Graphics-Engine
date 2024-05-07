#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/fps-camera-controller.hpp"

#include "camera-controller.hpp"
namespace our
{
    class FpsCameraControllerSystem : public CameraControllerSystem {
    public:
        virtual void update(World* world, float deltaTime) {
            // entity hold camera
            Entity* player = world->getEntity("player");
            if(!player) return;
            FpsCameraControllerComponent* controller = player->getComponent<FpsCameraControllerComponent>();
            // if no FPS camera controller nothing we can do
            if(!controller) return;
            // main camera component
            CameraComponent* camera = player->getComponent<CameraComponent>();
            if(!camera) return;
            // store y before any change
            float yBefore = camera->getOwner()->localTransform.position.y ;
            // move around using keys (inherited from the main class (CameraController)) 
            camera = CameraControllerSystem::moveAround(world, deltaTime, controller);
            //FPS Movement won't exit xz plane
            camera->getOwner()->localTransform.position.y = yBefore;
        }
    };
}