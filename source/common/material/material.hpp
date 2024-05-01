#pragma once

#include "pipeline-state.hpp"
#include "../texture/texture2d.hpp"
#include "../texture/sampler.hpp"
#include "../shader/shader.hpp"

#include <glm/vec4.hpp>
#include <json/json.hpp>

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

namespace our {

    // This is the base class for all the materials
    // It contains the 3 essential components required by any material
    // 1- The pipeline state when drawing objects using this material
    // 2- The shader program used to draw objects using this material
    // 3- Whether this material is transparent or not
    // Materials that send uniforms to the shader should inherit from the is material and add the required uniforms
    class Material {
    public:
        PipelineState pipelineState;
        ShaderProgram* shader;
        bool transparent;
        
        // This function does 2 things: setup the pipeline state and set the shader program to be used
        virtual void setup() const;
        // This function read a material from a json object
        virtual void deserialize(const nlohmann::json& data);
    };

    // This material adds a uniform for a tint (a color that will be sent to the shader)
    // An example where this material can be used is when the whole object has only color which defined by tint
    class TintedMaterial : public Material {
    public:
        glm::vec4 tint;

        void setup() const override;
        void deserialize(const nlohmann::json& data) override;
    };

    // This material adds two uniforms (besides the tint from Tinted Material)
    // The uniforms are:
    // - "tex" which is a Sampler2D. "texture" and "sampler" will be bound to it.
    // - "alphaThreshold" which defined the alpha limit below which the pixel should be discarded
    // An example where this material can be used is when the object has a texture
    class TexturedMaterial : public TintedMaterial {
    public:
        Texture2D* texture;
        Sampler* sampler;
        float alphaThreshold;

        void setup() const override;
        void deserialize(const nlohmann::json& data) override;
    };


    class LightMaterial : public Material {
    public:
        Texture2D* diffuseTexture, *specularTexture, *normalMap, *maskTexture;
        Sampler* sampler;
        bool isNormalMap = false;
        float shininess = 32.0f;
        // -------------------- directional light -----------------
        static glm::vec3 directionalLightDir;
        static glm::vec3 ambientDirLight, diffuseDirLight, specDirLight;
        // -------------------- spot light -----------------
        static glm::vec3 ambientSpotLight, diffuseSpotLight, specSpotLight;
        static float cutOff, outerCutOff;
        static float spot_constant, spot_linear, spot_quadratic;
        // -------------------- 4 point lights -----------------
        // positions of the point lights
        // TODO: adding spotlight
        glm::vec3 pointLightPositions[4] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
        };

        glm::vec3 pointLightColors[4] = {
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.3f, 0.1f, 0.1f)
        };
        float point_constant = 1.0f, point_linear = 0.14f, point_quadratic = 0.07f;

        void setup() const override;
        void deserialize(const nlohmann::json& data) override;
    };

    class LitMaterial : public Material {
    public:
        Texture2D* albedoMap, *colorMaskTexture, *normalMap, *r_ao_m_Map;
        Sampler* sampler;
        static const int MX_LIGHTS = 1;
        // -------------------- 4 point lights + 1 directional light + 1 flashlight -----------------
        // positions of the point lights

        glm::vec3 lightPositions[MX_LIGHTS] = {
            glm::vec3(-2.0f, -2.0f, -2.0f),
            // glm::vec3( 0.7f,  0.2f,  2.0f),
            // glm::vec3( 2.3f, -3.3f, -4.0f),
            // glm::vec3(-4.0f,  2.0f, -12.0f),
            // glm::vec3( 0.0f,  0.0f, -3.0f)
        };

        glm::vec3 lightColors[MX_LIGHTS] = {
            glm::vec3(0.1f, 0.1f, 0.1f),
            // glm::vec3(0.1f, 0.1f, 0.1f),
            // glm::vec3(0.1f, 0.1f, 0.1f),
            // glm::vec3(0.1f, 0.1f, 0.1f),
            // glm::vec3(0.3f, 0.1f, 0.1f)
        };
        int lightTypes[MX_LIGHTS] = {
            DIRECTIONAL_LIGHT,
            // POINT_LIGHT,
            // POINT_LIGHT,
            // POINT_LIGHT,
            // POINT_LIGHT
        };
        float cutOff = 5.0f, outerCutOff = 15.0f;

        void setup() const override;
        void deserialize(const nlohmann::json& data) override;
    };

    // This function returns a new material instance based on the given type
    inline Material* createMaterialFromType(const std::string& type){
        if(type == "tinted"){
            return new TintedMaterial();
        } else if(type == "textured"){
            return new TexturedMaterial();
        } else if(type == "light"){
            return new LitMaterial();
        } else {
            return new Material();
        }
    }

}