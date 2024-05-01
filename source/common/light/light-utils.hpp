#pragma once

#include "../shader/shader.hpp"
#include "../components/light-component.hpp"
#include <vector>
namespace our::light_utils {
    /*
        take shader of lit material and set all lights parameters to shader
    */
    void setLightParameters(ShaderProgram *shader, std::vector<LightCommand> lights) {
        shader->set("num_lights", (int)lights.size());

        for(int i = 0;i < lights.size();i++) {
            LightComponent* light = lights[i].light;
            
            shader->set("lights[" + std::to_string(i) + "].position",               lights[i].position);
            shader->set("lights[" + std::to_string(i) + "].direction",              lights[i].direction);
            shader->set("lights[" + std::to_string(i) + "].type",                   lights[i].type);
            shader->set("lights[" + std::to_string(i) + "].color",                  light->color);
            shader->set("lights[" + std::to_string(i) + "].outerCutOff",            glm::cos(glm::radians(light->outerCutOff)));
            shader->set("lights[" + std::to_string(i) + "].cutOff",                 glm::cos(glm::radians(light->cutOff)));
            shader->set("lights[" + std::to_string(i) + "].attenuationConstant",    light->attenuationConstant);
            shader->set("lights[" + std::to_string(i) + "].attenuationLinear",      light->attenuationLinear);
            shader->set("lights[" + std::to_string(i) + "].attenuationQuadratic",   light->attenuationQuadratic);
        }

    }
}