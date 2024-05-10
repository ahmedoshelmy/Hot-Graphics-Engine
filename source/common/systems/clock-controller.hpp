#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>
#include "../material/material.hpp"
#include "../mesh/mesh.hpp"
#include "../application.hpp"
#include "../mesh/mesh-utils.hpp"
namespace our {

    // The System is responsible for opening drawers where keys might be hidden
    class ClockController {
        glm::ivec2 windowSize;
    
        TintedMaterial * dangerMaterial;
        Mesh* rectangle;
        glm::mat4 transform; // contrain transform matrix of shader

        double currentTime;
        double endSoonInterval = 0.2*60; 
        double endGameInterval = 5*60; 

    public:
        void initialize(glm::ivec2 windowSize) {
            this->windowSize = windowSize;
            this->currentTime = 0.0;
            // create material of 
            dangerMaterial = new TintedMaterial();
            dangerMaterial->shader = new ShaderProgram();
            dangerMaterial->shader->attach("assets/shaders/tinted.vert", GL_VERTEX_SHADER);
            dangerMaterial->shader->attach("assets/shaders/danger.frag", GL_FRAGMENT_SHADER);
            dangerMaterial->shader->link();
            // tinted with red
            dangerMaterial->tint = glm::vec4(0.5, 0.0, 0.0, 0.0);
            // close depth testing as it appear above all
            dangerMaterial->pipelineState.depthTesting.enabled = false;
            // enable blending for transparency
            dangerMaterial->pipelineState.blending.enabled = true;
            // create mesh rectangle that take whole screen
            rectangle = mesh_utils::rectangle();

            // transform matrix that show rectangle and make it always centered & take full screen 
            transform = glm::mat4(1.0);
            transform = glm::scale(transform, glm::vec3(2));
            transform = glm::translate(transform,  glm::vec3(-0.5, -0.5, 0.0));
        }

        void render(TextRenderer* textRenderer, double deltaTime) {
            currentTime += deltaTime;
            // show clock
            if(endGameInterval > currentTime + 0.05)
                showClock(textRenderer);
            // if time enter the danger time show flash of red
            if(endGameInterval - currentTime <= endSoonInterval) {
                // renderer rectangle
                dangerMaterial->setup();
                dangerMaterial->shader->set("transform", transform);
                dangerMaterial->shader->set("time", (float)currentTime);
                // used to increase frequence
                float frequencyPulse =  (endSoonInterval - (endGameInterval - currentTime )) / endSoonInterval ;
                frequencyPulse = glm::mix(2.5f, 5.0f, frequencyPulse);
                dangerMaterial->shader->set("frequencyPulse", frequencyPulse);
                
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

        void destroy() {
            delete rectangle;
            delete dangerMaterial->shader;
            delete dangerMaterial;
        }
    };

}

