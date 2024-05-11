#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/fps-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/light.hpp>
#include <systems/text-renderer.hpp>
#include <asset-loader.hpp>
#include <material/material.hpp>
#include "mesh/mesh-utils.hpp"
#include "systems/drawer-opener.hpp"
#include "systems/locked-away.hpp"
#include "systems/audio.hpp"
#include <systems/physics.hpp>
#include <systems/picking.hpp>
#include <systems/clock-controller.hpp>


// This state shows how to use the ECS framework and deserialization.

class Playstate: public our::State {
    //* DEBUG Hole
    bool showGUI = true;
    our::World world;
    our::ForwardRenderer renderer;
    our::TextRenderer textRenderer;
    our::ClockController clockController;
    our::FreeCameraControllerSystem cameraControllerFree;
    our::FpsCameraControllerSystem cameraControllerFps;
    our::MovementSystem movementSystem;
    our::LightSystem lightSystem;
    our::PhysicsSystem physicsSystem;
    our::PickingSystem pickingSystem;
    our::DrawerOpenerSystem drawerOpenerSystem;
    our::LockedAwaySystem lockedAwaySystem;
    our::AudioSystem audioSystem;
    bool showDemoWindow = false;
    std::string inHandItem;
    std::string song;
    float songDuration ;
    our::GameState gameState;
    audio_wrapper::MiniAudioWrapper audioPlayer;


    void onInitialize() override {
        gameState = our::GameState::PLAY;
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
        // initalize physics
        physicsSystem.initialize(&world, getApp()->getWindowSize());
        // initalize lighting constants
        // Then we initialize the renderer

        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, getApp(), config["renderer"]);
        textRenderer.initialize(getApp()->getWindowSize());
        clockController.initialize(getApp()->getWindowSize());
    }

    void onDraw(double deltaTime) override {
        songDuration = 0 , song = "";
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        cameraControllerFree.update(&world, (float)deltaTime);
        cameraControllerFps.update(&world, (float)deltaTime);

        drawerOpenerSystem.update(&world, getApp(), pickingSystem.pickedObject, inHandItem, song, songDuration ,&renderer,(float)deltaTime);
        lockedAwaySystem.update(&world, deltaTime, &gameState, &renderer);
        // And finally we use the renderer system to draw the scene
        physicsSystem.update(&world, getApp(), (float)deltaTime);



        pickingSystem.pickedObject = physicsSystem.cameraSelected;
        pickingSystem.mousePickedObject = physicsSystem.mouseSelected;
        std::cout << "Camera: " << pickingSystem.pickedObject << " Mouse: " << pickingSystem.mousePickedObject << "\n";
        pickingSystem.update(&world, getApp(), inHandItem, &textRenderer, song, songDuration,deltaTime);
        renderer.render(&world, deltaTime);

        // And finally we use the renderer system to draw the scene
        clockController.render(&textRenderer, deltaTime);
        textRenderer.render(deltaTime);
        audioSystem.update(&world,deltaTime,song,songDuration);

        

        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

//        if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
//            // If the escape  key is pressed in this frame, go to the play state
//            getApp()->changeState("menu");
//        }

        if (clockController.getCurrentTime() >= clockController.getEndGameInterval()) {
            getApp()->changeState("lose");
        }
        if (gameState == our::GameState::WIN) {
            getApp()->changeState("win");
        }
    }

    void onImmediateGui(){
        physicsSystem.showGUI(&world);
        lightSystem.showGUI(&world);
        renderer.showGUI(&world);
        lockedAwaySystem.showGUI(&world);
        textRenderer.showGUI();
    }

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // destroy text renderer
        textRenderer.destroy();
        // destroy clock controller
        clockController.destroy();
        physicsSystem.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraControllerFree.exit();
        cameraControllerFps.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }

    
};