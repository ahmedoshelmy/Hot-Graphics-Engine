
#include "../ecs/component.hpp"

//TODO: This will represent an object that physics rules apply to including (Collision).
// Probably most of the entities will contain this component

namespace our {
    enum class RigidBodyShape {
        Sphere,
        Box
    };
    class RigidBodyComponent : public Component {

    private:
        RigidBodyShape rigidBodyShape;
        int length ;
    public:
        static std::string getID() { return "RigidBody"; }
        int getLength();
        void deserialize(const nlohmann::json &data) override;

        ~RigidBodyComponent();
    };
}

