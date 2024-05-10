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
    };

    // The System is responsible for opening drawers where keys might be hidden
    class TextRenderer {
        double currentTime ;
        glm::ivec2 windowSize;

        std::map<GLchar, Character> Characters;
        
        GLuint VAO, VBO;
        ShaderProgram* textShader;
        
        std::queue<TextRenderCommand > textCommands;
        //* DEBUG HOLE
        float x = 25.0f, y = 100.0f, scale = 0.2f; 
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f); 
        int text_align_x = 0, text_align_y = 0;

    public:
        void initialize(glm::ivec2 windowSize) {
            this->windowSize = windowSize;
            // Text Renderer Initialization
            textShader = new ShaderProgram();
            textShader->attach("assets/shaders/text/text.vert", GL_VERTEX_SHADER);
            textShader->attach("assets/shaders/text/text.frag", GL_FRAGMENT_SHADER);
            textShader->link();
            textShader->use();
            glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(windowSize.x), 0.0f,
                                            static_cast<float>(windowSize.y));
            textShader->set("projection", projection);

            // FreeType Initialization
            FT_Library ft;
            if (FT_Init_FreeType(&ft)) {
                std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            }


            // Select the font family
            FT_Face face;
            if (FT_New_Face(ft, "assets/fonts/ReggaeOne-Regular.ttf", 0, &face)) {
                std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
            }

            // Set the font size 
            // The second param is the width when set to 0 it is set automatically
            // The third param is the height of the font
            FT_Set_Pixel_Sizes(face, 0, 48);

            // Disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // Generate textures of the characters using the specified font

            for (unsigned char c = 0; c < 128; c++) {
                if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                    std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        GL_RED,
                        face->glyph->bitmap.width,
                        face->glyph->bitmap.rows,
                        0,
                        GL_RED,
                        GL_UNSIGNED_BYTE,
                        face->glyph->bitmap.buffer
                );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                Character character = {
                        texture,
                        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                        static_cast<unsigned int>(face->glyph->advance.x)
                };
                Characters.insert(std::pair<char, Character>(c, character));
            }

            // Destroy FreeType once we're finished

            glBindTexture(GL_TEXTURE_2D, 0);
            FT_Done_Face(face);
            FT_Done_FreeType(ft);

            currentTime = 0;

        }

        void render(double deltaTime) {
            currentTime += deltaTime;
            renderText("Welcome To Locked Away", 25.0f, 100.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), 0, 1);
            // renderText("You Picked Up A Key", 25.0f, 100.0f, 0.2f, glm::vec3(1.0f, 1.0f, 1.0f), 0, 0);
            checkTextCommands();
        }

        void showGUI() {
            ImGui::Begin("Text Renderer Details");
            ImGui::InputFloat("x", &x, 1, 2, 3);
            ImGui::InputFloat("y", &y, 1, 2, 3);
            ImGui::InputFloat("scale", &scale, 0.1, 0.2, 3);
            ImGui::InputInt("align-x", &text_align_x, 1, 2, 3);
            ImGui::InputInt("align-y", &text_align_y, 1, 2, 3);
            ImGui::ColorPicker3("Text Color", glm::value_ptr(color));
            ImGui::End();
        }

        void renderText(std::string text, float x, float y, float scale, glm::vec3 color, int text_align_x,
                                     int text_align_y) {
            // Text Rendering Setup
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            // Setting the buffer and vertex array
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            // Choose the text color

            textShader->use();
            textShader->set("text_color", color);
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(VAO);

            // Center the text

            if (text_align_x == 0) {
                float text_width = 0;
                for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
                    Character ch = Characters[*c];
                    text_width += (ch.Advance >> 6) * scale;
                }
                x = (windowSize.x / 2) - (text_width / 2);
            }

            if (text_align_y == 0) {
                float text_height = 0;
                for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
                    Character ch = Characters[*c];
                    text_height = ch.Size.y;
                }
                y = (windowSize.y / 2) - (text_height / 2);
            }

            // Iterate through all characters
            std::string::const_iterator c;
            float xpos = x;
            float ypos = y;
            for (c = text.begin(); c != text.end(); c++) {
                Character ch = Characters[*c];

                xpos += ch.Bearing.x * scale;
                ypos -= (ch.Size.y - ch.Bearing.y) * scale;

                float w = ch.Size.x * scale;
                float h = ch.Size.y * scale;
                // Update VBO for each character
                float vertices[6][4] = {
                        {xpos,     ypos + h, 0.0f, 0.0f},
                        {xpos,     ypos,     0.0f, 1.0f},
                        {xpos + w, ypos,     1.0f, 1.0f},

                        {xpos,     ypos + h, 0.0f, 0.0f},
                        {xpos + w, ypos,     1.0f, 1.0f},
                        {xpos + w, ypos + h, 1.0f, 0.0f}
                };
                // Render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                // Update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                // Render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                xpos += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
            }
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);

        }

        void renderText(std::string text, double seconds) {
            TextRenderCommand textRenderCommand;
            textRenderCommand.duration = seconds;
            textRenderCommand.text = text;
            textRenderCommand.initialTime = currentTime;
            std::cout<<"RENDER TEXT";
            textCommands.push(textRenderCommand);
        }

        void checkTextCommands() {
            int sz = textCommands.size();
            for (int i = 0; i < sz; ++i) {
                auto textCommand = textCommands.front();
                textCommands.pop();
                if (textCommand.duration + textCommand.initialTime < currentTime)continue;
                textCommands.push(textCommand);
                renderText(textCommand.text, x, y, scale, color, text_align_x, text_align_y);
            }
        }

    };

}
