
#include "knob-component.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void KnobComponent::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;
        closedPosition = data.value("closedPosition", closedPosition);
        openPosition = data.value("openPosition", openPosition);
        closedRotation = glm::radians(data.value("closedRotation", closedRotation));
        openRotation = glm::radians(data.value("openRotation", openRotation));
        key = data.value("key","-");
    }
}


