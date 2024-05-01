#include ../lighting/pbr.frag


#define color1 vec3(0.970, 0.940, 0.877)
#define color2 vec3(0.129, 0.163, 0.195)
#define color3 vec3(0.645, 0.283, 0.072)
#define color4 vec3(1)

  

void main()
{
    vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    // ==== masking
    vec3 colorMask =  pow(texture(colorMaskTexture, TexCoords).rgb, vec3(2.2));
    vec3 colorOut = mix( mix( mix(
                        albedo * color4, 
                        albedo * color1, 
        colorMask.r
    ), albedo * color2, colorMask.g 
    ), albedo * color3, colorMask.b);
    albedo = colorOut;
    // ==== masking


    vec3 r_ao_m = pow(texture(r_ao_m_Map, TexCoords).rgb, vec3(2.2));
    float roughness = r_ao_m.r;
    float ao = r_ao_m.g;
    float metallic = r_ao_m.b;

    vec3 Lo = calcLight(albedo, roughness, metallic);

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
   
} 