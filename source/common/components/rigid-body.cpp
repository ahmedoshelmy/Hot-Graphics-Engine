#include "rigid-body.hpp"
#include "../deserialize-utils.hpp"
#include "../ecs/world.hpp"


namespace our {
    void RigidBodyComponent::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;
    }

    RigidBodyComponent::~RigidBodyComponent() {

    }
}