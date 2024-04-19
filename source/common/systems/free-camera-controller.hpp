#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/free-camera-controller.hpp"

#include "camera-controller.hpp"

namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/free-camera-controller.hpp"
    class FreeCameraControllerSystem : public CameraControllerSystem { 
    public:
        virtual void update(World* world, float deltaTime) {
            // move around
            CameraComponent* camera = nullptr;
            FreeCameraControllerComponent* controller = nullptr;
            
            camera = CameraControllerSystem::moveAround(world, deltaTime, controller);
        }
    };

}
