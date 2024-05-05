
#include "ecs/component.hpp"

namespace our {
    class GroundOrStairsComponent : public Component {
    public:
        // The ID of this component type is "Light Component"
        static std::string getID() { return "Ground or Stairs Component"; }

        virtual void deserialize(const nlohmann::json& data) {}

    };
}


