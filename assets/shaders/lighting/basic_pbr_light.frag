#include pbr.frag

uniform vec4 tint = vec4(1.0, 1.0, 1.0, 1.0);


void main()
{
    vec3 albedo = pow(texture(material.albedoMap, TexCoords).rgb, vec3(2.2));
    
    vec3 r_ao_m = pow(texture(material.r_ao_m_Map, TexCoords).rgb, vec3(2.2));
    float roughness = r_ao_m.r;
    float ao = r_ao_m.g;
    float metallic = r_ao_m.b;

    vec3 Lo = calcLight(albedo, roughness, metallic);

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    vec3 emissive = vec3(0);
    if(material.enableEmissive){
        emissive =  pow(texture(material.emissive, TexCoords).rgb, vec3(2.2));
    }
    color = pow(color+emissive, vec3(1.0/2.2));

    FragColor = tint * vec4(color, 1.0) ;
   
} 