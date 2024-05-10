
#include "../ecs/component.hpp"
#include "../asset-loader.hpp"
#include <glm/glm.hpp>

//This is for entities that can be opened like doors and Drawers
namespace our {

    class KnobComponent : public Component {
    public:
        bool open = false;
        glm::vec3 openPosition;
        glm::vec3 closedPosition;
        glm::vec3 openRotation;
        glm::vec3 closedRotation;
        std::string key;

        static std::string getID() { return "Knob"; }

        // Receives the mesh & material from the AssetLoader by the names given in the json object
        void deserialize(const nlohmann::json &data) override;

    };

}
