#include "light-component.hpp"
#include "../asset-loader.hpp"
#include <deserialize-utils.hpp>

#include<glm/glm.hpp>
namespace our {
    // Receives the mesh & material from the AssetLoader by the names given in the json object
    void LightComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
       
        std::string type_string = data.value("lightType", "");
        name = data.value("name", type_string);

        if(type_string == "directional") type = LightType::DIRECTIONAL;
        else if(type_string == "point") type = LightType::POINT;
        else if(type_string == "spot") type = LightType::SPOT;
        else if(type_string == "flash") type = LightType::FLASH;
        else type = LightType::DIRECTIONAL;

        color = data.value("color", glm::vec3(0));
        direction = data.value("direction", glm::vec3(0));
        cutOff = data.value("cutOff", 0.0f);
        outerCutOff = data.value("outerCutOff", 0.0f);
        attenuationConstant = data.value("attenuationConstant", 1.0f);
        attenuationLinear = data.value("attenuationLinear", 0.007f);
        attenuationQuadratic = data.value("attenuationQuadratic", 0.0014f);


        debug = data.value("debug", false);
    }
}