
#include "ecs/component.hpp"

namespace our {
    class TriggerComponent : public Component {
    public:
        // The ID of this component type is "Light Component"
        static std::string getID() { return "Light Component"; }

        // Receives the mesh & material from the AssetLoader by the names given in the json object
        void deserialize(const nlohmann::json &data) override;

    };
}


