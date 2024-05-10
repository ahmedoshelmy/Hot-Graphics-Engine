#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <queue>
#include <glad/gl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../shader/shader.hpp"
#include "../application.hpp"

namespace our {
    struct Character {
        unsigned int TextureID; // ID handle of the glyph texture
        glm::ivec2   Size;      // Size of glyph
        glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

    struct TextRenderCommand {
        float initialTime;
        float duration ;
        std::string text;
        float x, y, scale; 
        glm::vec3 color; 
        int text_align_x, text_align_y;
    };

    // The System is responsible for opening drawers where keys might be hidden
    class TextRenderer {
        double currentTime ;
        glm::ivec2 windowSize;

        std::map<GLchar, Character> Characters;
        
        GLuint VAO, VBO;
        ShaderProgram* textShader;
        
        std::queue<TextRenderCommand > textCommands;
    public:
        //* DEBUG HOLE
        static float x, y, scale; 
        static glm::vec3 color; 
        static int text_align_x, text_align_y;

        void initialize(glm::ivec2 windowSize);

        void render(double deltaTime) {
            currentTime += deltaTime;
            renderText("Welcome To Locked Away", 25.0f, 100.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), 0, 1);
            checkTextCommands();
        }

        void showGUI();

        void renderText(std::string text, float x, float y, float scale, glm::vec3 color, int text_align_x,
                                     int text_align_y);

        void addTextCommand(std::string text, double seconds, float x = TextRenderer::x, float y = TextRenderer::y, float scale = TextRenderer::scale, 
                                    glm::vec3 color = TextRenderer::color, int text_align_x = TextRenderer::text_align_x, int text_align_y = TextRenderer::text_align_y);

        void checkTextCommands();

        void destroy() {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            delete textShader;
        }

    };

}

