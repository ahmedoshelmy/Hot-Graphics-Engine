#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../light/light-utils.hpp"
#include<glm/gtc/matrix_access.hpp>
#include<glm/gtc/matrix_inverse.inl>
#include<glm/gtc/matrix_transform.hpp>
#include"physics.hpp"
#include <glad/gl.h>
// Text Rendering

#include <iostream>
#include <utility>

namespace our {

    void ForwardRenderer::initialize(glm::ivec2 windowSize, Application *app, const nlohmann::json &config) {
        // First, we store the window size for later use
        this->windowSize = windowSize;
        this->app = app;
        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky")) {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            //TODO: (Req 10) Pick the correct pipeline state to draw the sky
            // Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            // We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
            skyPipelineState.depthTesting.enabled = true;

            skyPipelineState.faceCulling.enabled = false;
            skyPipelineState.faceCulling.culledFace = GL_FRONT;

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky 
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material 
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a sky texture in the configuration
        if (config.contains("skyCube")) {
            auto skyCubeData = config["skyCube"];
            // if(!data.is_object())
            // First, we load the mesh a cube which will be used to draw the sky
            this->skyCube = mesh_utils::loadOBJ("assets/models/cube.obj");

            // We can draw the sky using the shader used to draw cube textured objects
            ShaderProgram *skyCubeShader = new ShaderProgram();
            skyCubeShader->attach("assets/shaders/textured3d.vert", GL_VERTEX_SHADER);
            skyCubeShader->attach("assets/shaders/textured3d.frag", GL_FRAGMENT_SHADER);
            skyCubeShader->link();

            // Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            // We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyCubePipelineState{};
            skyCubePipelineState.depthTesting.enabled = true;
            skyCubePipelineState.depthTesting.function = GL_LEQUAL;

            skyCubePipelineState.faceCulling.enabled = false;
            skyCubePipelineState.faceCulling.culledFace = GL_FRONT;

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTexturesFile[6];

            skyTexturesFile[0] = skyCubeData.value("left", "");
            skyTexturesFile[1] = skyCubeData.value("right", "");
            skyTexturesFile[2] = skyCubeData.value("top", "");
            skyTexturesFile[3] = skyCubeData.value("bottom", "");
            skyTexturesFile[4] = skyCubeData.value("back", "");
            skyTexturesFile[5] = skyCubeData.value("front", "");
            
            Texture3D *skyTexture = texture_utils::loadCube(skyTexturesFile);
            // Setup a sampler for the sky 
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // Combine all the aforementioned objects (except the mesh) into a material 
            this->skyMaterialCube = new Textured3DMaterial();
            this->skyMaterialCube->shader = skyCubeShader;
            this->skyMaterialCube->texture = skyTexture;
            this->skyMaterialCube->sampler = skySampler;
            this->skyMaterialCube->pipelineState = skyCubePipelineState;
            this->skyMaterialCube->transparent = false;
        }


        // Then we check if there is a postprocessing shader in the configuration
        if(config.contains("postprocess")){
            //Create a framebuffer
            glGenFramebuffers(1, &postprocessFrameBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
            //Create a color and a depth texture and attach them to the framebuffer
            // Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            // The depth format can be (Depth component with 24 bits).
            colorTarget = our::texture_utils::empty(windowSize, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(),
                                   0);

            //TODO: add stensil
            depthTarget = our::texture_utils::empty(windowSize, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            }
            // //Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
            postprocessMaterial->pipelineState.depthTesting.enabled = false;
        }

        

        this->initCastingBuffer();

        currentTime = 0;
    }


    void ForwardRenderer::destroy() {
        // Delete all objects related to the sky
        if (skyMaterial) {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        if(skyMaterialCube) {
            delete skyCube;
            delete skyMaterialCube->shader;
            delete skyMaterialCube->texture;
            delete skyMaterialCube->sampler;
            delete skyMaterialCube;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial) {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
        glDeleteFramebuffers(1, &castingFBO);
        glDeleteVertexArrays(1, &castingVAO);
        delete primitiveCastingTarget;
        delete depthCastingTarget;
        delete castingMaterial->sampler;
        delete castingMaterial->shader;
        delete castingMaterial;

    }

    void ForwardRenderer::render(World *world, std::string &pickedItem, double deltaTime) {
        currentTime += deltaTime;
        mp[0] = "NON-WORLD";
        picked_item = "NON-WORLD";
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        lightSources.clear();
        for (auto entity: world->getEntities()) {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera) camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer) {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                command.name = meshRenderer->getOwner()->name;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent) {
                    transparentCommands.push_back(command);
                } else {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }

            if (auto lightRenderer = entity->getComponent<LightComponent>(); lightRenderer) {
                LightCommand command;
                command.type = lightRenderer->type;
                command.position = lightRenderer->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0f);
                command.direction = lightRenderer->getOwner()->getLocalToWorldMatrix() * glm::vec4(lightRenderer->direction, 0.0);
                command.light = lightRenderer;
                lightSources.push_back(command);
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if(camera == nullptr) return;
        //Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        // HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        glm::vec3 cameraPosition = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, 0.0f, 1.0f);
        glm::vec3 cameraForward = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, -1.0f, 0.0f);
        
        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand& first, const RenderCommand& second){
            float distanceFirst = glm::dot(cameraForward, first.center);
            float distanceSecond = glm::dot(cameraForward, second.center);

            return distanceFirst > distanceSecond;
        });

        //Get the camera ViewProjection matrix and store it in VP
        glm::mat4 VP =  camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();
        //Set the OpenGL viewport using viewportStart and viewportSize
        glViewport(0, 0, windowSize[0], windowSize[1]);


        //Set the clear color to black and the clear depth to 1
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0);
        //Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        // ============================= picking phase =====================
        // bind casting frame buffer
        pickingPhaseRenderer(camera);
        // read pixel that camera point on (since camera always point to center)
        PixelInfo pixel = readPixel(windowSize.x / 2, windowSize.y / 2);
        // get picked entity name
        picked_item = mp[pixel.ObjectID];
        pickedItem = picked_item;
        // ============================= renderer phase =====================
        rendererPhaseRenderer(camera);
        // ============================= debugging ImGUI phase =====================
        // if left click on entity
        if(app->getMouse().justPressed(GLFW_MOUSE_BUTTON_2) ) {
            // read mouse coords
            // since opengl read y from (0_top) where opposite happen in renderer (0_bottom) we need to reverse directions
            glm::vec2 mouse_coord = app->getMouse().getMousePosition();
            mouse_coord.y = windowSize.y - mouse_coord.y;
            // read pixel info
            PixelInfo mouse_pixel = readPixel(mouse_coord.x, mouse_coord.y);
            // get selected item with previouse
            prevSelectedItem = selectedItem;
            selectedItem = mp[mouse_pixel.ObjectID];
            
        } 
    }

    void ForwardRenderer::showGUI(World *world) {
        Entity* player = world->getEntity("player");
        CameraComponent* camera = player->getComponent<CameraComponent>();
        // for show other widgets
        bool showDemoWindow = true;
        ImGui::ShowDemoWindow(&showDemoWindow);
        ImGui::Begin("Entities");
        // loop on all entities and call it's gui
        for (auto entity: world->getEntities()) {
            if(selectedItem == entity->name && prevSelectedItem == selectedItem) {
                ImGui::SetNextTreeNodeOpen(false);
                selectedItem = "NON-WORLD";
            }
            else if(selectedItem == entity->name)
                ImGui::SetNextTreeNodeOpen(true);
            entity->showGUI(camera);
        }
        
        ImGui::End();
    }

   
    void ForwardRenderer::pickingPhaseRenderer(CameraComponent *camera) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, castingFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();

        // loop as it is and draw id to pixel
        unsigned int gObjectIndex = 1;
        //TODO: add alpha so the complete transparent not seen
        for(auto command : opaqueCommands) {
            castingMaterial->setup();
            mp[gObjectIndex] = command.name;
            castingMaterial->shader->set("gObjectIndex", gObjectIndex++);
            castingMaterial->shader->set("transform", VP * command.localToWorld);
            command.mesh->draw();
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *) 0);

        }

        // for(auto command : transparentCommands) {
        //     castingMaterial->setup();
        //     castingMaterial->shader->set("gObjectIndex", gObjectIndex++);
        //     castingMaterial->shader->set("transform", VP * command.localToWorld);
        //     command.mesh->draw();
        // }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void ForwardRenderer::rendererPhaseRenderer(CameraComponent *camera) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if(postprocessMaterial){
            //bind the framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, postprocessFrameBuffer);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();
        glm::vec3 cameraForward = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, -1.0, 0.0);
        // getting mouse coordinates

        for (auto command: opaqueCommands) {
            command.material->setup();
            // if (command.name == picked_item) command.material->shader->set("tint", glm::vec4(0.0, 1.0, 0.0, 0.2));
            // else command.material->shader->set("tint", glm::vec4(1.0, 1.0, 1.0, 1.0));
            if (dynamic_cast<LitMaterial *>(command.material) != nullptr) {
                light_utils::setLightParameters(command.material->shader, lightSources);
                command.material->shader->set("cameraForward", cameraForward);
                command.material->shader->set("u_Model", command.localToWorld);
                command.material->shader->set("u_View", camera->getViewMatrix());
                command.material->shader->set("u_Projection", camera->getProjectionMatrix(windowSize));
            } else {
                command.material->shader->set("transform", VP * command.localToWorld);
            }
            command.mesh->draw();
        }


        // If there is a sky material, draw the sky
        if (this->skyMaterial) {
            //V = setup the sky material
            skyMaterial->setup();
            //Get the camera position
            glm::vec3 cameraPosition = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0);
            //V = Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position)
            glm::mat4 skyModel(1.0f);
            skyModel = glm::translate(skyModel, cameraPosition);
            //V = We want the sky to be drawn behind everything (in NDC space, z=1)
            // We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            glm::mat4 alwaysBehindTransform = glm::mat4(
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 1.0f
            );
            //V = set the "transform" uniform
            this->skyMaterial->shader->set("transform", alwaysBehindTransform * VP * skyModel);
            //V = draw the sky sphere
            this->skySphere->draw();
        }

        if(this->skyMaterialCube) {
            skyMaterialCube->setup();

            //remove translation from view matrix
            // glm::mat4 view =   glm::mat4(glm::mat3(camera->getViewMatrix()));
            // glm::mat4 projection =  camera->getProjectionMatrix(windowSize);

            glm::vec3 cameraPosition = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0);
            //V = Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position)
            glm::mat4 skyModel(1.0f);
            skyModel = glm::translate(skyModel, cameraPosition);
            //We want the sky to be drawn behind everything (in NDC space, z=1)
            // We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            //* it's more easy to do it in vertex shader (just select .xyww) as it will always make it behind
            //V = set the "transform" uniform
            this->skyMaterialCube->shader->set("transform", VP * skyModel);
            //V = draw the sky sphere
            this->skyCube->draw();
        }
        
        //Draw all the transparent commands
        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto command: transparentCommands) {
            command.material->setup();
            command.material->shader->set("transform", VP * command.localToWorld);
            command.mesh->draw();
        }
        // Text Rendering
//        renderText("Welcome To Locked Away", 25.0f, 100.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), 0, 1);
        // renderText("You Picked Up A Key", 25.0f, 100.0f, 0.2f, glm::vec3(1.0f, 1.0f, 1.0f), 0, 0);
        // checkTextCommands();

        if (postprocessMaterial) {
            //TODO: (Req 11) Return to the default framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //Setup the postprocess material and draw the fullscreen triangle
            postprocessMaterial->setup();
            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }


    }

    PixelInfo ForwardRenderer::readPixel(unsigned int x, unsigned int y)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, castingFBO);

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        PixelInfo Pixel;
        glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);

        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        return Pixel;
    }


}