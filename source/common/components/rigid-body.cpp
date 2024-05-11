#include "rigid-body.hpp"
#include "../deserialize-utils.hpp"
#include "../ecs/world.hpp"
#include <iostream>

namespace our {
    void RigidBodyComponent::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;
        std::string shape = data.value("shape", "");
        if (shape == "Sphere") {
            rigidBodyShape = RigidBodyShape::Sphere;
            length = data.value("radius", 0);
        }
        else if (shape == "Box") {
            rigidBodyShape = RigidBodyShape::Box;
            length = data.value("length", 0);
        }

    }

    RigidBodyComponent::~RigidBodyComponent() {

    }

    int RigidBodyComponent::getLength() {
        return length;
    }
}