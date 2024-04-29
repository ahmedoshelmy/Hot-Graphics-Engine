
#include "../ecs/component.hpp"

//TODO: This will represent an object that physics rules apply to including (Collision).
// Probably most of the entities will contain this component

namespace our {
    class RigidBodyComponent : public Component {

    public:

        static std::string getID() { return "RigidBody"; }

        void deserialize(const nlohmann::json &data) override;

        ~RigidBodyComponent();
    };
}

