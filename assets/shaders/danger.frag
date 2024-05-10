#version 330 core

out vec4 frag_color;

uniform vec4 tint;
uniform float time;
uniform float remainTime;
void main(){
    //TODO: (Req 7) Modify the following line to compute the fragment color
    // by multiplying the tint with the vertex color
    // frag_color = vec4(1.0);
    frag_color = vec4(tint.xyz, 0.2 * sin(time*remainTime));
}