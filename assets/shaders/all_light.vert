#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    FragPos = vec3(u_Model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(u_Model))) * normal;  
    TexCoords = tex_coord;
    
    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
}