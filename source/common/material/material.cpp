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
    void Material::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;

        if (data.contains("pipelineState")) {
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
    void TintedMaterial::deserialize(const nlohmann::json &data) {
        Material::deserialize(data);
        if (!data.is_object()) return;
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
        if (sampler)
            sampler->bind(0);
        shader->set("tex", 0);

    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json &data) {
        TintedMaterial::deserialize(data);
        if (!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    void Textured3DMaterial::setup() const {
        Material::setup();
        glActiveTexture(GL_TEXTURE0);
        texture->bind();
        if (sampler)
            sampler->bind(0);
        shader->set("tex", 0);
    }


    void LitMaterial::setup() const {
        Material::setup();
        shader->set("material.albedoMap", 0);
        shader->set("material.colorMaskTexture", 1);
        shader->set("material.r_ao_m_Map", 2);
        shader->set("material.IOR", IOR);
        shader->set("material.emissive", 4);
        if (emissiveMap)shader->set("material.enableEmissive", true);
        else shader->set("material.enableEmissive", false);
        if (colorMaskTexture) {
            shader->set("material.enableColorMasking", true);
            shader->set("material.color1", colorMasking[0]);
            shader->set("material.color2", colorMasking[1]);
            shader->set("material.color3", colorMasking[2]);
            shader->set("material.color4", colorMasking[3]);
        }
        else {
            shader->set("material.enableColorMasking", false);
        }
        
        if(normalMap) {
            shader->set("material.enableNormalTexture", true);
            shader->set("material.normalMap", 3);
        } else {
            shader->set("material.enableNormalTexture", false);
        }

        glActiveTexture(GL_TEXTURE0);
        albedoMap->bind();
        sampler->bind(0);

        if (colorMaskTexture) {
            glActiveTexture(GL_TEXTURE1);
            colorMaskTexture->bind();
            sampler->bind(1);
        }


        glActiveTexture(GL_TEXTURE2);
        r_ao_m_Map->bind();
        sampler->bind(2);

        if(normalMap){
            glActiveTexture(GL_TEXTURE3);
            normalMap->bind();
            sampler->bind(3);
        }

        if (emissiveMap) {
            glActiveTexture(GL_TEXTURE4);
            emissiveMap->bind();
            sampler->bind(4);
        }


    }


    void LitMaterial::deserialize(const nlohmann::json &data) {
        Material::deserialize(data);
        if (!data.is_object()) return;
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));

        albedoMap = AssetLoader<Texture2D>::get(data.value("albedoMap", ""));
        colorMaskTexture = AssetLoader<Texture2D>::get(data.value("colorMaskTexture", ""));
        normalMap = AssetLoader<Texture2D>::get(data.value("normalMap", ""));
        r_ao_m_Map = AssetLoader<Texture2D>::get(data.value("r_ao_m_Map", ""));
        emissiveMap = AssetLoader<Texture2D>::get(data.value("emissiveMap", ""));
        colorMasking[0] = data.value("color1", glm::vec3(1.0, 1.0, 1.0));
        colorMasking[1] = data.value("color2", glm::vec3(1.0, 1.0, 1.0));
        colorMasking[2] = data.value("color3", glm::vec3(1.0, 1.0, 1.0));
        colorMasking[3] = data.value("color4", glm::vec3(1.0, 1.0, 1.0));
        IOR = data.value("IOR", 0.03);


    }


}