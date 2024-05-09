#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/fps-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/light.hpp>
#include <asset-loader.hpp>
#include <material/material.hpp>
#include "mesh/mesh-utils.hpp"
#include "systems/drawer-opener.hpp"
#include <systems/physics.hpp>
#include <systems/picking.hpp>


// This state shows how to use the ECS framework and deserialization.

class WinState : public our::State {


    void onInitialize() override {

    }

    void onDraw(double deltaTime) override {


        // Get a reference to the keyboard object
        auto &keyboard = getApp()->getKeyboard();

        if (keyboard.justPressed(GLFW_KEY_ESCAPE)) {
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
    }

    void onImmediateGui() {

    }

    void onDestroy() override {

    }
};