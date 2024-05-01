#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"
#include "./components/light-component.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: (Req 7) Write this function
        pipelineState.setup();
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        Material::setup();
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        TintedMaterial::setup();
        shader->set("alphaThreshold", alphaThreshold);
        glActiveTexture(GL_TEXTURE0);
        texture->bind();
        if(sampler)
            sampler->bind(0);
        shader->set("tex", 0);

    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }


    // -------------------- directional light -----------------
    // glm::vec3   LightMaterial::directionalLightDir = glm::vec3(-15.0f, 15.0f, -25.0f);
    // glm::vec3   LightMaterial::ambientDirLight = glm::vec3(0.0f, 0.0f, 0.0f), 
    //             LightMaterial::diffuseDirLight = glm::vec3(0.0f, 0.0f, 0.0f), 
    //             LightMaterial::specDirLight    = glm::vec3(0.2f, 0.2f, 0.2f);
    // // -------------------- spot light -----------------
    // glm::vec3   LightMaterial::ambientSpotLight = glm::vec3( 0.3f, 0.3f, 0.3f), 
    //             LightMaterial::diffuseSpotLight = glm::vec3(0.1f, 0.2f, 0.2f), 
    //             LightMaterial::specSpotLight    = glm::vec3(.2f, .2f, .2f);
    // float LightMaterial::cutOff = 15.0f, LightMaterial::outerCutOff = 20.0f;
    // float LightMaterial::spot_constant = 1.0f, LightMaterial::spot_linear = 0.014f, LightMaterial::spot_quadratic = 1.0f;


    void LitMaterial::setup() const {
        Material::setup();
        shader->set("material.albedoMap", 0);
        shader->set("material.colorMaskTexture", 1);
        shader->set("material.r_ao_m_Map", 2);
        shader->set("material.normalMap", 3);

    //    shader->set("num_lights", (int)lights.size());

    //     for(int i = 0;i < lights.size();i++) {
    //         shader->set("lights[" + std::to_string(i) + "].type",   lights[i].type);
    //         shader->set("lights[" + std::to_string(i) + "].color",  lights[i].color);
    //         shader->set("lights[1].outerCutOff",                    glm::cos(glm::radians(lights[i].outerCutOff)));
    //         shader->set("lights[1].cutOff",                         glm::cos(glm::radians(lights[i].cutOff)));
    //         shader->set("attenuationConstant",                      lights[i].attenuationConstant);
    //         shader->set("attenuationLinear",                        lights[i].attenuationLinear);
    //         shader->set("attenuationQuadratic",                     lights[i].attenuationQuadratic);
    //     }
    

        glActiveTexture(GL_TEXTURE0);
        albedoMap->bind();
        sampler->bind(0); 

        glActiveTexture(GL_TEXTURE1);
        colorMaskTexture->bind();
        sampler->bind(1);


        glActiveTexture(GL_TEXTURE2);
        r_ao_m_Map->bind();
        sampler->bind(2);

        glActiveTexture(GL_TEXTURE3);
        normalMap->bind();
        sampler->bind(3);

    }


    void LitMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));

        albedoMap = AssetLoader<Texture2D>::get(data.value("albedoMap", ""));
        colorMaskTexture = AssetLoader<Texture2D>::get(data.value("colorMaskTexture", ""));
        normalMap = AssetLoader<Texture2D>::get(data.value( "normalMap", ""));
        r_ao_m_Map = AssetLoader<Texture2D>::get(data.value( "r_ao_m_Map", ""));

    }



    

}