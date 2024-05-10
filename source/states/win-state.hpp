#pragma once
#include <material/material.hpp>
#include <texture/texture-utils.hpp>
#include <mesh/mesh.hpp>
#include <mesh/mesh-utils.hpp>


class WinState : public our::State {
    our::Mesh* rectangle;
    our::TexturedMaterial* menuMaterial;
    float time;

    void onInitialize() override {
        rectangle = our::mesh_utils::rectangle();

        menuMaterial = new our::TexturedMaterial();
        // Here, we load the shader that will be used to draw the background
        menuMaterial->shader = new our::ShaderProgram();
        menuMaterial->shader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
        menuMaterial->shader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
        menuMaterial->shader->link();
        // Then we load the menu texture
        menuMaterial->texture = our::texture_utils::loadImage("assets/textures/win_state.jpg");
        // Initially, the menu material will be black, then it will fade in
        menuMaterial->tint = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        menuMaterial->pipelineState.depthTesting.enabled = false;

        time = 0;
    }

    void onDraw(double deltaTime) override {
        auto& keyboard = getApp()->getKeyboard();
        // show keyboard input
        if(keyboard.justPressed(GLFW_KEY_P)){
            getApp()->changeState("play");
        } else if(keyboard.justPressed(GLFW_KEY_M)) {
            getApp()->changeState("menu");
        } else if(keyboard.justPressed(GLFW_KEY_ESCAPE) || keyboard.justPressed(GLFW_KEY_E)) {
            getApp()->close();
        }

        // draw texture
        glm::ivec2 size = getApp()->getFrameBufferSize();
        // Make sure the viewport covers the whole size of the framebuffer.
        glViewport(0, 0, size.x, size.y);

        glm::mat4 VP = glm::ortho(0.0f, (float)size.x, (float)size.y, 0.0f, 1.0f, -1.0f);
        glm::mat4 M = glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

        // First, we apply the fading effect.
        time += (float)deltaTime;
        menuMaterial->tint = glm::vec4(glm::smoothstep(0.00f, 2.00f, time));

        menuMaterial->setup();
        menuMaterial->shader->set("transform", VP*M);
        rectangle->draw();
    }

    void onDestroy() override {

    }
};