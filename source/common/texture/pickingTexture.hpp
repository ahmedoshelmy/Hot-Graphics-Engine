#pragma once
#include <glad/gl.h>

class PickingTexture {

    public:
        // hold framebuffer object id
        GLuint FBO, pickingTexture, depthTexture;
        PickingTexture();
        ~PickingTexture();

};