#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;


uniform mat4 transform;
out vec3 texDir;
void main(){
    //TODO: (Req 7) Change the next line to apply the transformation matrix
    gl_Position = transform*vec4(position, 1.0);
    texDir = position;
}