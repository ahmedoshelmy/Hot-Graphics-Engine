#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>
#include "../material/material.hpp"
#include "../mesh/mesh.hpp"
#include "../application.hpp"

namespace our {

    // The System is responsible for opening drawers where keys might be hidden
    class ClockController {
        glm::ivec2 windowSize;
    
        TintedMaterial * dangerMaterial;
        Mesh* rectangle;
        double currentTime = 0.0;
        double endSoonInterval = 1*60; 
        double endGameInterval = 1.1*60; 

    public:
        void initialize(glm::ivec2 windowSize) {
            this->windowSize = windowSize;

            dangerMaterial = new TintedMaterial();
            dangerMaterial->shader = new ShaderProgram();
            dangerMaterial->shader->attach("assets/shaders/tinted.vert", GL_VERTEX_SHADER);
            dangerMaterial->shader->attach("assets/shaders/danger.frag", GL_FRAGMENT_SHADER);
            dangerMaterial->shader->link();

            dangerMaterial->tint = glm::vec4(0.5, 0.0, 0.0, 0.0);
            dangerMaterial->pipelineState.depthTesting.enabled = false;
            dangerMaterial->pipelineState.blending.enabled = true;

            // Then we create a rectangle whose top-left corner is at the origin and its size is 1x1.
            // Note that the texture coordinates at the origin is (0.0, 1.0) since we will use the 
            // projection matrix to make the origin at the the top-left corner of the screen.
            rectangle = new our::Mesh({
                {{0.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                {{1.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                {{1.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                {{0.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            },{
                0, 1, 2, 2, 3, 0,
            });



        }

        void render(TextRenderer* textRenderer, double deltaTime) {
            currentTime += deltaTime;
            showClock(textRenderer);

            if(endGameInterval - currentTime <= endSoonInterval) {
                // show clock
                // transform matrix that show rectangle
                glm::mat4 transform = glm::mat4(1.0);
                transform = glm::scale(transform, glm::vec3(2));
                transform = glm::translate(transform,  glm::vec3(-0.5, -0.5, 0.0));
                // renderer rectangle
                dangerMaterial->setup();
                dangerMaterial->shader->set("transform", transform);
                dangerMaterial->shader->set("time", (float)currentTime);
                // used to increase frequence
                float remainPercentage =  (endSoonInterval - (endGameInterval - currentTime )) / endSoonInterval ;
                remainPercentage = glm::mix(2.5f, 5.0f, remainPercentage);
                dangerMaterial->shader->set("remainTime", remainPercentage);
                rectangle->draw();
            }
        }

        void showGUI();

        void showClock(TextRenderer* textRenderer) {
            int minutes = (endGameInterval - currentTime) / 60; 
            int seconds = ((int)endGameInterval - (int)currentTime) % 60;
            std::string showTime = "0" + std::to_string(minutes) + " : " + (seconds < 10 ? "0" : "") + std::to_string(seconds);
            textRenderer->addTextCommand(showTime, 0.05, 1620, 1000, 1.2, glm::vec3(0.3, 0.3, 0.3), 1, 1);
        }

        double getCurrentTime() const { return currentTime; }
        double getEndSoonInterval() const { return endSoonInterval; }
        double getEndGameInterval() const { return endGameInterval; }

    };

}

