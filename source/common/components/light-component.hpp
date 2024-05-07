#pragma once

#include "../ecs/component.hpp"
#include "../mesh/mesh.hpp"
#include "../material/material.hpp"
#include "../asset-loader.hpp"

namespace our {
    enum LightType {
        DIRECTIONAL,
        POINT,
        SPOT,
        FLASH
    };
    // This component denotes that any renderer should draw the given mesh using the given material at the transformation of the owning entity.
    class LightComponent : public Component {
    public:
        std::string name;
        LightType type; 
        glm::vec3 color; 
        glm::vec3 direction;            // spot light where direction point to 
        float cutOff;                   // spot cutoff angle in degrees
        float outerCutOff;              // spot outer cutoff angle in degrees
        
        float attenuationConstant;     // The constant of the attenuation
        float attenuationLinear;       // The linear of the attenuation
        float attenuationQuadratic;    // The quadratic of the attenuation

        bool debug;

        // The ID of this component type is "Light Component"
        static std::string getID() { return "Light Component"; }

        // Receives the mesh & material from the AssetLoader by the names given in the json object
        void deserialize(const nlohmann::json& data) override;

    };

}