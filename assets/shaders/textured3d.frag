#version 330 core

uniform samplerCube tex;
in vec3 texDir;
out vec4 frag_color;

void main(){
    //TODO: (Req 7) Modify the following line to compute the fragment color
    // by multiplying the tint with the vertex color and with the texture color 
    // frag_color = vec4(1.0);
    frag_color =  texture(tex, texDir);
}