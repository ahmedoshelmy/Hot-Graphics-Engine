#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

out Varyings {
    vec4 color;
    vec4 position;  
} vs_out;

uniform mat4 transform;

void main(){
    //TODO: (Req 7) Change the next line to apply the transformation matrix
    vec4 out_position = transform * vec4(position, 1.0);
    gl_Position = out_position;
    vs_out.color = color;
}