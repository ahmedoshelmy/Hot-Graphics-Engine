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
            // move around
            CameraComponent* camera = nullptr;
            FpsCameraControllerComponent* controller = nullptr;
            
            camera = CameraControllerSystem::moveAround(world, deltaTime, controller);
            if(!camera) return;
            //FPS Movement won't exit xz plane
            camera->getOwner()->localTransform.position.y = 0;
        }
    };
}