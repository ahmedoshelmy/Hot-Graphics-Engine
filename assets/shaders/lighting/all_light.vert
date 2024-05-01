#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

 
uniform mat4 u_Model;


out DATA {
    vec3 Normal;
    vec2 TexCoords;
    vec3 FragPos;
} data_out;


void main()
{
    data_out.FragPos = vec3(u_Model * vec4(position, 1.0));
    data_out.Normal = mat3(transpose(inverse(u_Model))) * normal;  
    data_out.TexCoords = tex_coord;
    gl_Position = vec4(data_out.FragPos, 1.0);
}