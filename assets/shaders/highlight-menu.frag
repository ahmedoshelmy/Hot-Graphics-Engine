#version 330 core

in Varyings {
    vec4 color;
    vec3 position;
} fs_in;

out vec4 frag_color;

uniform vec4 tint;
uniform mat4 transform;

uniform vec2 highlightCenter; // Center of the highlight
uniform float highlightRadius; // Radius of the highlight
uniform float highlightIntensity; // Intensity of the highlight
uniform float alpha; // Intensity of the highlight
uniform vec2 size;
uniform vec2 center;
void main(){
    // vec2 iRes = vec2(1980, 1080);
    // vec2 sz = size / iRes;
    // float pct = 1.0 - smoothstep(.2, .7, length((gl_FragCoord.xy - center)/iRes/sz)  );
    // frag_color =  vec4(1.0, 1.0, 1.0, 0.0) * fs_in.color;
    frag_color = tint*fs_in.color;
}