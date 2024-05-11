#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;
out vec4 frag_color;
uniform float amount;
uniform float iFrameTime;

void main(){
    float randomIntensity = fract(sin((gl_FragCoord.x + gl_FragCoord.y * iFrameTime) * 10000 * 3.14 / 180));

    frag_color = texture(tex, tex_coord);

    frag_color += vec4(amount  * randomIntensity);
}