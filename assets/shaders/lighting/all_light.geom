#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;
out mat3 TBN;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform vec3 cameraForward;

in DATA {
    vec3 Normal;
    vec2 TexCoords;
    vec3 FragPos;
} data_in[];


void main() {

    // ============== calculate Tangent, Bitangent
    vec3 edge0 = vec3(0.0);
    vec3 edge1 = vec3(0.0);
    vec2 deltaUV0 = vec2(0.0);
    vec2 deltaUV1 = vec2(0.0);


    edge0    = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    edge1    = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    deltaUV0 = data_in[2].TexCoords - data_in[0].TexCoords;
    deltaUV1 = data_in[1].TexCoords - data_in[0].TexCoords;

    vec3 N = normalize(vec3(u_Model * vec4(cross(edge1, edge0), 0.0f))); // recalculate normal vector to make sure it normal on edges

    if(dot(N, cameraForward) == -1.0) {
        edge0    = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        edge1    = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

        deltaUV0 = data_in[1].TexCoords - data_in[0].TexCoords;
        deltaUV1 = data_in[2].TexCoords - data_in[0].TexCoords;
    }

    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    vec3 T = normalize(vec3(u_Model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(u_Model * vec4(bitangent, 0.0f)));
         N = normalize(vec3(u_Model * vec4(cross(edge1, edge0), 0.0f))); // recalculate normal vector to make sure it normal on edges
    TBN = mat3(T, B, N);


    // =========== default ==============
    gl_Position = u_Projection * u_View *  gl_in[0].gl_Position;
    Normal = data_in[0].Normal;
    TexCoords = data_in[0].TexCoords;
    FragPos = data_in[0].FragPos;
    EmitVertex(); 

    gl_Position = u_Projection * u_View *  gl_in[1].gl_Position;
    Normal = data_in[1].Normal;
    TexCoords = data_in[1].TexCoords;
    FragPos = data_in[1].FragPos;
    EmitVertex(); 

    gl_Position = u_Projection * u_View *  gl_in[2].gl_Position;
    Normal = data_in[2].Normal;
    TexCoords = data_in[2].TexCoords;
    FragPos = data_in[2].FragPos;
    EmitVertex(); 

    EndPrimitive();
}