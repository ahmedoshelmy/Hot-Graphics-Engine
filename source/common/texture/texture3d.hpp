#pragma once

#include <glad/gl.h>

namespace our {

    // This class defined an OpenGL texture which will be used as a GL_TEXTURE_2D
    class Texture3D {
        // The OpenGL object name of this texture 
        GLuint name = 0;
    public:
        // This constructor creates an OpenGL texture and saves its object name in the member variable "name" 
        Texture3D() {
            // This tells opengl to generate 1 texture and store its name in the member variable "name"
            glGenTextures(1, &name);
        };

        // This deconstructor deletes the underlying OpenGL texture
        ~Texture3D() { 
            // This tells opengl to delete 1 texture with the name stored in the member variable "name"
            glDeleteTextures(1, &name);
        }

        // Get the internal OpenGL name of the texture which is useful for use with framebuffers
        GLuint getOpenGLName() {
            return name;
        }

        // This method binds this texture to GL_TEXTURE_CUBE_MAP
        void bind() const {
            // This tells opengl to bind the texture with the name stored in the member variable "name" to GL_TEXTURE_CUBE_MAP 
            // so that any subsequent texture operations will affect this texture
            glBindTexture(GL_TEXTURE_CUBE_MAP, name);
        }

        // This static method ensures that no texture is bound to GL_TEXTURE_CUBE_MAP
        static void unbind(){
            // This tells opengl to unbind any texture from GL_TEXTURE_CUBE_MAP
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
        // remove copy constructor and assignment operation
        Texture3D(const Texture2D&) = delete;
        Texture3D& operator=(const Texture3D&) = delete;
    };
    
}