#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/fps-camera-controller.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>
#include <material/material.hpp>
#include "mesh/mesh-utils.hpp"

// This state shows how to use the ECS framework and deserialization.

class Playstate: public our::State {
    //* DEBUG Hole
    bool showGUI = true;

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraControllerFree;
    our::FpsCameraControllerSystem cameraControllerFps;
    our::MovementSystem movementSystem;
    bool showDemoWindow = false;


    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        cameraControllerFree.enter(getApp());
        cameraControllerFps.enter(getApp());
        // initalize lighting constants
        // Then we initialize the renderer

        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        cameraControllerFree.update(&world, (float)deltaTime);
        cameraControllerFps.update(&world, (float)deltaTime);
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);

        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

        if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
    }

    void onImmediateGui(){ 
        if(showGUI) {
            
            ImGui::Text("Directional Light");
            ImGui::SliderFloat3("direction", glm::value_ptr(our::LightMaterial::directionalLightDir), -25, 25, "%.3f", 0 );
            ImGui::SliderFloat3("ambient directional light", glm::value_ptr(our::LightMaterial::ambientDirLight), 0.0, 1.0, "%.3f", 0 );
            ImGui::SliderFloat3("diffuse directional light", glm::value_ptr(our::LightMaterial::diffuseDirLight), 0.0, 1.0, "%.3f", 0 );
            ImGui::SliderFloat3("specular directional light", glm::value_ptr(our::LightMaterial::specDirLight), 0.0, 1.0, "%.3f", 0 );
            ImGui::Text("Spot Light");
            ImGui::SliderFloat3("ambient spot light", glm::value_ptr(our::LightMaterial::ambientSpotLight), 0.0, 1.0, "%.3f", 0 );
            ImGui::SliderFloat3("diffuse spot light", glm::value_ptr(our::LightMaterial::diffuseSpotLight), 0.0, 1.0, "%.3f", 0 );
            ImGui::SliderFloat3("specular spot light", glm::value_ptr(our::LightMaterial::specSpotLight), 0.0, 1.0, "%.3f", 0 );

            ImGui::SliderFloat("spot light cutoff", &our::LightMaterial::cutOff, 0.0, 180.f, "%.2f", 0 );
            ImGui::SliderFloat("spot light outer cutoff", &our::LightMaterial::outerCutOff, 0.0, 180.f, "%.2f", 0 );

            ImGui::SliderFloat("spot light linear", &our::LightMaterial::spot_linear, 0.0, 1.f, "%.4f", 0 );
            ImGui::SliderFloat("spot light quadratic", &our::LightMaterial::spot_quadratic, 0.0, 1.f, "%.4f", 0 );
            ImGui::Text("Point Light");
            // ImGui::SliderFloat("edge1", &edge1, 0.01f, 1.f, "%.2f", 0 );
        }
    }      

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraControllerFree.exit();
        cameraControllerFps.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};