#version 330 core

out vec4 frag_color;

// In this shader, we want to draw a checkboard where the size of each tile is (size x size).
// The color of the top-left most tile should be "colors[0]" and the 2 tiles adjacent to it
// should have the color "colors[1]".

//TODO: (Req 1) Finish this shader.

uniform int size = 32;
uniform vec3 colors[2];

void main(){
    //  gl_FragCoord represents window relative coordinate, when divided by size, it will get the posistion of the box
    // that is being drawn
    // The color can be determined by the summation of the row and the col. If even then the first color, otherwise the second.
    vec2 position = floor(gl_FragCoord.xy / size);
    uint color = uint (mod(position.x + mod(position.y,2.0), 2.0));
    frag_color = vec4(colors[color], 1.0);
}