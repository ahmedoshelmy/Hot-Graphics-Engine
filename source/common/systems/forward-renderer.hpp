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
    struct PixelInfo {
        unsigned int ObjectID = 0;
        unsigned int DrawID = 0;
        unsigned int PrimID = 0;

        void Print()
        {
            printf("Object %d draw %d prim %d\n", ObjectID, DrawID, PrimID);
        }
    };
    struct TextRenderCommand {
        float initialTime;
        float duration ;
        std::string text;
    };

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer {
         std::queue<TextRenderCommand > textCommands;
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
        // Objects used for testing
        Mesh* whiteBoxMesh;
        TintedMaterial* whiteBoxMaterial;
        bool showWhiteBox = false;
        // Objects used for Postprocessing
        GLuint postprocessFrameBuffer, postProcessVertexArray;
        Texture2D *colorTarget, *depthTarget;
        TexturedMaterial* postprocessMaterial;
        // Objects for ray casting
        GLuint castingFBO, castingVAO;
        Texture2D* primitiveCastingTarget, *depthCastingTarget;
        TexturedMaterial* castingMaterial;
        std::map<unsigned int, std::string> mp;
        std::string picked_item;
        // need application for picking objects
        Application* app;
    public:
        // Initialize the renderer including the sky and the Postprocessing objects.
        // windowSize is the width & height of the window (in pixels).
        void initialize(glm::ivec2 windowSize, Application* app, const nlohmann::json& config);
        // Clean up the renderer
        void destroy();
        // This function should be called every frame to draw the given world
        void render(World* world,std::string &,double);

        void showGUI(World* world);

        void pickingComponent(World* world, CameraComponent* camera, glm::ivec2 windowSize);

        void initCastingBuffer() {
            glGenFramebuffers(1, &castingFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, castingFBO);

            primitiveCastingTarget = texture_utils::empty(windowSize, GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, primitiveCastingTarget->getOpenGLName(), 0);

            depthCastingTarget = our::texture_utils::empty(windowSize, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthCastingTarget->getOpenGLName(), 0);
            // check status
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "ERROR:: CASTING FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            }

            //Unbind the framebuffer and texture just to be safe
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Create a sampler to use for sampling the scene texture in the post processing shader
            glGenVertexArrays(1, &castingVAO);

            Sampler* castingSampler = new Sampler();
            castingSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            castingSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            castingSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            castingSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram* castingShader = new ShaderProgram();
            castingShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            castingShader->attach("assets/shaders/picking.frag", GL_FRAGMENT_SHADER);
            castingShader->link();

            // Create a post processing material
            castingMaterial = new TexturedMaterial();
            castingMaterial->shader = castingShader;
            castingMaterial->texture = primitiveCastingTarget;
            castingMaterial->sampler = castingSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            // castingMaterial->pipelineState.depthMask = false;
            // castingMaterial->pipelineState.depthTesting.enabled = false;

        }
        void pickingPhaseRenderer(CameraComponent *camera) ;
        void rendererPhaseRenderer(CameraComponent *camera) ;
        PixelInfo readPixel(unsigned int x, unsigned int y);
        void renderText(std::string text, double seconds);

    private:
        struct Character {
            unsigned int TextureID; // ID handle of the glyph texture
            glm::ivec2   Size;      // Size of glyph
            glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
            unsigned int Advance;   // Horizontal offset to advance to next glyph
        };

        // Objects used for text rendering

        std::map<GLchar, Character> Characters;
        GLuint VAO, VBO;
        ShaderProgram* textShader;
        void renderText(std::string text, float x, float y, float scale, glm::vec3 color, int text_align_x, int text_align_y);
        void checkTextCommands();
    };

}