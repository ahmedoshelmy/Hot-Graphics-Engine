#version 330 core

in Varyings {
    vec4 color;
} fs_in;

out vec4 frag_color;

uniform vec4 tint;

uniform float edge0, edge1; // Intensity of the highlight
uniform vec2 size;
uniform vec2 center;
void main(){
    vec2 iRes = vec2(1980, 1080);
    vec2 sz = size / iRes;
    float pct = 1.0 - smoothstep(edge0, edge1, length((gl_FragCoord.xy - center)/iRes/sz)  );
    frag_color =  vec4(1.0, 1.0, 1.0, pct*.75) * tint  * fs_in.color;
}