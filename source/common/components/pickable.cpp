#include "pickable.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void PickableComponent::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;
        inventoryMousePosition = data.value("inventoryMousePosition", inventoryMousePosition);
        inventoryPosition = data.value("inventoryPosition", inventoryPosition);
        inventoryRotation = data.value("inventoryRotation", inventoryRotation);
        inventoryScale = data.value("inventoryScale", inventoryScale);
        handPosition = data.value("handPosition", handPosition);
        handRotation = data.value("handRotation", handRotation);
        handScale = data.value("handScale", handScale);
    }
}

