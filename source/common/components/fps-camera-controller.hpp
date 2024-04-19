#pragma once

#include "../ecs/component.hpp"
#include "camera-controller.hpp"

#include <glm/glm.hpp> 

namespace our {

    class FpsCameraControllerComponent : public CameraControllerComponent {
    public:
        // The ID of this component type is "FPS Camera Controller"
        static std::string getID() { return "FPS Camera Controller"; }
    };

}