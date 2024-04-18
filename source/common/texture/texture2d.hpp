#pragma once

#include <glad/gl.h>

namespace our {

    // This class defined an OpenGL texture which will be used as a GL_TEXTURE_2D
    class Texture2D {
        // The OpenGL object name of this texture 
        GLuint name = 0;
    public:
        // This constructor creates an OpenGL texture and saves its object name in the member variable "name" 
        Texture2D() {
            //TODO: (Req 5) Complete this function
            // This tells opengl to generate 1 texture and store its name in the member variable "name"
            glGenTextures(1, &name);
        };

        // This deconstructor deletes the underlying OpenGL texture
        ~Texture2D() { 
            //TODO: (Req 5) Complete this function
            // This tells opengl to delete 1 texture with the name stored in the member variable "name"
            glDeleteTextures(1, &name);
        }

        // Get the internal OpenGL name of the texture which is useful for use with framebuffers
        GLuint getOpenGLName() {
            return name;
        }

        // This method binds this texture to GL_TEXTURE_2D
        void bind() const {
            //TODO: (Req 5) Complete this function
            // This tells opengl to bind the texture with the name stored in the member variable "name" to GL_TEXTURE_2D 
            // so that any subsequent texture operations will affect this texture
            glBindTexture(GL_TEXTURE_2D, name);
        }

        // This static method ensures that no texture is bound to GL_TEXTURE_2D
        static void unbind(){
            //TODO: (Req 5) Complete this function
            // This tells opengl to unbind any texture from GL_TEXTURE_2D
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;
    };
    
}