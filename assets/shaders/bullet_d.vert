#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 u_View;
uniform mat4 u_Projection;
out Varyings {
    vec4 color;
} vs_out;


void main(){
    //TODO: (Req 7) Change the next line to apply the transformation matrix
    gl_Position = u_Projection*u_View*vec4(position, 1.0);
    vs_out.color = color;
}