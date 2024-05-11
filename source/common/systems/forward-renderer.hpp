#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/light-component.hpp"
#include "../components/mesh-renderer.hpp"
#include "../texture/texture-utils.hpp"
#include "../asset-loader.hpp"
#include "../application.hpp"

#include <glad/gl.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
namespace our
{

    
    // The render command stores command that tells the renderer that it should draw
    // the given mesh at the given localToWorld matrix using the given material
    // The renderer will fill this struct using the mesh renderer components
    struct RenderCommand {
        glm::mat4 localToWorld;
        glm::vec3 center;
        Mesh* mesh;
        Material* material;
        std::string name;
    };
    struct LightCommand {
        glm::vec3 position;
        glm::vec3 direction;
        LightType type;
        LightComponent* light;
    };
    

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer {
         double currentTime ;

        // These window size will be used on multiple occasions (setting the viewport, computing the aspect ratio, etc.)
        glm::ivec2 windowSize;
        // These are two vectors in which we will store the opaque and the transparent commands.
        // We define them here (instead of being local to the "render" function) as an optimization to prevent reallocating them every frame
        std::vector<RenderCommand> opaqueCommands;
        std::vector<RenderCommand> transparentCommands;
        std::vector<LightCommand> lightSources;
        // Objects used for rendering a skybox
        Mesh* skySphere;
        TexturedMaterial* skyMaterial;
        // Objects used for rendering a skycube
        Mesh* skyCube;
        Textured3DMaterial* skyMaterialCube;
        // Objects used for testing
        Mesh* whiteBoxMesh;
        TintedMaterial* whiteBoxMaterial;
        bool showWhiteBox = false;
        // Objects used for Postprocessing
        GLuint postprocessFrameBuffer, postProcessVertexArray;
        Texture2D *colorTarget, *depthTarget;
        TexturedMaterial* postprocessMaterial;

        // Lighting
        float darkness_factor;

        // need application for picking objects
        Application* app;
        
    public:
        // Initialize the renderer including the sky and the Postprocessing objects.
        // windowSize is the width & height of the window (in pixels).
        void initialize(glm::ivec2 windowSize, Application* app, const nlohmann::json& config);
        // Clean up the renderer
        void destroy();
        // This function should be called every frame to draw the given world
        void render(World* world, double);

        void showGUI(World* world);

        void rendererPhaseRenderer(CameraComponent *camera) ;

    private:
       
    };
}


