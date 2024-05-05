
#include "ecs/component.hpp"

namespace our {
    class GroundOrStairsComponent : public Component {
    public:
        float y = 5.0;
        float dy = 0.0;
        // The ID of this component type is "Light Component"
        static std::string getID() { return "Ground or Stairs Component"; }

        virtual void deserialize(const nlohmann::json& data) {
            y   = data.value("y", y);
            dy  = data.value("dy", dy);
        }

    };
}


