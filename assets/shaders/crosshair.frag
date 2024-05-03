#version 330 core


#define SMOOTH(r,R) (1.0-smoothstep(R-0.001,R+0.001, r))
#define RANGE(a,b,x) ( step(a,x)*(1.0-step(b,x)) )
#define RANGE_SMOOTH(a,b,x) ( smoothstep(a-0.001,a+0.001,x)*(1.0-smoothstep(b-0.001,b+0.001,x)) )

#define PI 3.14159265359
#define TWO_PI 6.28318530718


#define circle(st, r) SMOOTH(length(st), r)
#define circle_line(st, r, w) RANGE_SMOOTH(length(st) - w/2.0, length(st) + w/2.0, r)
#define circle_line_grad(st, r, w, g) RANGE_SMOOTH_GRAD(length(st) - w/2.0, length(st) + w/2.0, r, g)
#define line(st, w) RANGE_SMOOTH(st.x-w/2., st.x+w/2., st.y)

out vec4 frag_color;

uniform vec4 tint;

void main() {
    vec2 st = gl_FragCoord.xy / vec2(1920.0, 1080.0);
    vec2 center = vec2(1920.0, 1080.0) / 2.0;
    
    float circle_alpha = circle_line(gl_FragCoord.xy - center, 10, 5);
    float cross_alpha = line(vec2(gl_FragCoord.xy - center), 10) + line(vec2(vec2(1920.0 - gl_FragCoord.x, gl_FragCoord.y) - center), 10);
    // float cross_alpha = line(st, 5);
    frag_color =   vec4(tint.rgb, tint.a * (circle_alpha - cross_alpha));

}