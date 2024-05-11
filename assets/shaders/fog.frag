#version 330

in vec3 position;
// The texture holding the scene pixels
uniform sampler2D tex;

in vec2 tex_coord;
out vec4 frag_color;


uniform vec4 fogColor;
// needed to calculate distance between camera and frag 
uniform vec3 cameraPosition;
in vec3 fragPosition;
// uniform vec3 fogColor;

#define FogMax 20.0
#define FogMin 10.0

float getFogFactor(float d) {

    if (d>=FogMax) return 1.0;
    if (d<=FogMin) return 0.0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
} 

void main(){
    float alpha = getFogFactor(distance(fragPosition, cameraPosition));
    
    frag_color= mix(texture(tex, tex_coord), fogColor, alpha);
}