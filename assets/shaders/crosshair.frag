#version 330 core


#define SMOOTH(r,R) (1.0-smoothstep(R-0.001,R+0.001, r))
#define RANGE(a,b,x) ( step(a,x)*(1.0-step(b,x)) )
#define RANGE_SMOOTH(a,b,x) ( smoothstep(a-0.001,a+0.001,x)*(1.0-smoothstep(b-0.001,b+0.001,x)) )

#define PI 3.14159265359
#define TWO_PI 6.28318530718

#define blue1 vec3(0.74,0.95,1.00)
#define blue2 vec3(0.87,0.98,1.00)
#define blue3 vec3(0.35,0.76,0.83)
#define blue4 vec3(0.953,0.969,0.89)
#define red   vec3(1.00,0.38,0.227)

#define circle(st, r) SMOOTH(length(st), r)
#define circle_line(st, r, w) RANGE_SMOOTH(length(st) - w/2.0, length(st) + w/2.0, r)
#define circle_line_grad(st, r, w, g) RANGE_SMOOTH_GRAD(length(st) - w/2.0, length(st) + w/2.0, r, g)
out vec4 frag_color;

in Varyings {
    vec4 color;
} fs_in;


uniform vec4 tint;

void main() {
    vec2 st = gl_FragCoord.xy / vec2(1920.0, 1080.0);
    vec2 center = vec2(1920.0, 1080.0) / 2.0;
    // vec4 color = vec4(0., 0., 0., 0.);
    // // oultine
    // vec3 color_outline = blue3;

    // // lines
    // // circles

    // frag_color = color;

    frag_color =   vec4(blue3, 0.8 * circle_line(gl_FragCoord.xy - center, 10, 2));

}