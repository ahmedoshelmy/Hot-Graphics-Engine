
#include "../ecs/component.hpp"
// TODO: This will be the component of the main character and will have its data including scoring and other

namespace our {
    class PlayerComponent : public Component {

    public:

        static std::string getID() { return "Player"; }

        void deserialize(const nlohmann::json &data) override;
    };
}

