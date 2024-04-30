#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

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
    glm::vec3   LightMaterial::directionalLightDir = glm::vec3(-15.0f, 15.0f, -25.0f);
    glm::vec3   LightMaterial::ambientDirLight = glm::vec3(0.0f, 0.0f, 0.0f), 
                LightMaterial::diffuseDirLight = glm::vec3(0.0f, 0.0f, 0.0f), 
                LightMaterial::specDirLight    = glm::vec3(0.2f, 0.2f, 0.2f);
    // -------------------- spot light -----------------
    glm::vec3   LightMaterial::ambientSpotLight = glm::vec3( 0.3f, 0.3f, 0.3f), 
                LightMaterial::diffuseSpotLight = glm::vec3(0.1f, 0.2f, 0.2f), 
                LightMaterial::specSpotLight    = glm::vec3(.2f, .2f, .2f);
    float LightMaterial::cutOff = 15.0f, LightMaterial::outerCutOff = 20.0f;
    float LightMaterial::spot_constant = 1.0f, LightMaterial::spot_linear = 0.014f, LightMaterial::spot_quadratic = 0.0007f;

    void LightMaterial::setup() const {
        Material::setup();
        shader->set("material.albedo", 0);
        shader->set("material.specular", 1);
        // shader->set("material.emission", 2);
        shader->set("material.shininess", shininess); 
        // ==============================================================================
        shader->set("isNormalMap", isNormalMap);
        if(isNormalMap) {
            shader->set("normalMap", 2);
        }
        shader->set("u_colorMaskTexture", 3);
        // ==============================================================================
        shader->set("dirLight.direction", directionalLightDir);
        shader->set("dirLight.ambient", ambientDirLight);
        shader->set("dirLight.diffuse", diffuseDirLight);
        shader->set("dirLight.specular", specDirLight);
        // ==============================================================================
        // shader->set("spotLight.position", camera.GetPosition());
        // shader->set("spotLight.direction", camera.GetFront());
        
        shader->set("spotLight.ambient", ambientSpotLight);
        shader->set("spotLight.diffuse", diffuseSpotLight);
        shader->set("spotLight.specular", specSpotLight);

        shader->set("spotLight.cutOff", glm::cos(glm::radians(cutOff)));
        shader->set("spotLight.outerCutOff", glm::cos(glm::radians(outerCutOff)));

        shader->set("spotLight.constant", spot_constant);
        shader->set("spotLight.linear", spot_linear);
        shader->set("spotLight.quadratic", spot_quadratic);
        // ==============================================================================
        // for(int i = 0;i < 4;i++) {
        //     shader->set("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);

        //     shader->set("pointLights[" + std::to_string(i) + "].ambient", pointLightColors[i] * 0.1f);
        //     shader->set("pointLights[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
        //     shader->set("pointLights[" + std::to_string(i) + "].specular", pointLightColors[i]);

        //     shader->set("pointLights[" + std::to_string(i) + "].constant", point_constant);
        //     shader->set("pointLights[" + std::to_string(i) + "].linear", point_linear);
        //     shader->set("pointLights[" + std::to_string(i) + "].quadratic", point_quadratic);
        // }
    

        glActiveTexture(GL_TEXTURE0);
        diffuseTexture->bind();
        sampler->bind(0);
        glActiveTexture(GL_TEXTURE1);
        specularTexture->bind();
        sampler->bind(1);
        if(isNormalMap) {
            glActiveTexture(GL_TEXTURE2);
            normalMap->bind();
            sampler->bind(2);
        }
        glActiveTexture(GL_TEXTURE3);
        maskTexture->bind();
        sampler->bind(3);
        // glActiveTexture(GL_TEXTURE2);
        // emission_texture->bind();
        

    }

    // This function read the material data from a json object
    void LightMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        // alphaThreshold = data.value("alphaThreshold", 0.0f);
        diffuseTexture = AssetLoader<Texture2D>::get(data.value("diffuseTexture", ""));
        specularTexture = AssetLoader<Texture2D>::get(data.value("specularTexture", ""));
        normalMap = AssetLoader<Texture2D>::get(data.value( "normalMap", ""));
        maskTexture = AssetLoader<Texture2D>::get(data.value( "colorMask", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));

        isNormalMap = data.value("isNormalMap", isNormalMap);
    }

}