

#include "../ecs/component.hpp"
#include <glm/glm.hpp>
// TODO: This will be the component of the main character and will have its data including scoring and other

namespace our {
    enum class PickedObjectState {
        UNPICKED,
        PICKED,
        INVENTORY
    };
    class PickableComponent : public Component {

    public:
        glm::vec2 inventoryMousePosition  = {0, 0};
        glm::vec3 inventoryScale  = {0, 0, 0};
        glm::vec3 inventoryRotation  = {0, 0, 0};
        glm::vec3 inventoryPosition  = {0, 0, 0};
        glm::vec3 handScale  = {0, 0, 0};
        glm::vec3 handRotation  = {0, 0, 0};
        glm::vec3 handPosition  = {0, 0, 0};
        PickedObjectState pickedObjectState = PickedObjectState::UNPICKED;

        static std::string getID() { return "Pickable"; }

        void deserialize(const nlohmann::json &data) override;
    };
}