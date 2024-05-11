#version 330 core


#define PI 3.14159265359
#define dotMaxZero(A, B) max(dot(A, B), 0.0)

#define MAX_LIGHTS 20

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

out vec4 FragColor;
in vec3 FragPos;
in mat3 TBN;
in vec2 TexCoords;
in vec3 Normal;

uniform vec3 cameraPos;
uniform int num_lights;
// material parameters
struct Material {
    sampler2D  albedoMap;
    sampler2D  colorMaskTexture;
    sampler2D  normalMap;
    sampler2D  r_ao_m_Map; // rough (R) AO (G) Metallic (B)
    sampler2D  emissive;
    bool  enableEmissive;
    bool  enableColorMasking;
    bool enableNormalTexture;
    vec3 color1;
    vec3 color2;
    vec3 color3;
    vec3 color4;
    float IOR;
};
uniform Material material;


// lights
struct Light {
    int type;
    vec3 color;
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
};

uniform Light lights[MAX_LIGHTS];

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(material.normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
/* Cook-Torrance BRDF
    F_r = kd * F_lambert + ks F_cook_torrance
    F_lambert = diffuse BRDF = c / PI                       ; (c) => albedo
    F_cook_torrance = specular BRDF = DFG / (4(N.H)^2)      ; (DFG) => D, F, G
*/

/*  Trowbridge-Reitz GGX (normal distrubted function)
    (D) statistically approximate the relative surface area of microfacets exactly aligned to the (halfway vector)
    parameters:
        N: normal vector
        H: halfway vector
        a: roughness parameter
    reference: https://learnopengl.com/PBR/Theory
*/
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

/*  Smith's Schlick-GGX for (geometry function)
    (G) statistically approximates the relative surface area where its micro surface-details overshadow each other, 
    causing light rays to be occluded.
    paramerters:
        N: normal vector
        V: view vector
        L: light vector
        k:      roughness parameter remapped based on what lighting we are using (direct vs image based) lighting
                k_direct = ((a + 1)**2) / 8
                k_ibl    = a**2/2
    equation:
        G(n, v, l, k) = G_schlick(N.V, k) * G_schlick(N.L, K)
        G_schlick(N.V, k)  => take account the view direction (geometry obstruction) 
        G_schlick(N.L, k)  => take account the light direction (geometry shadowing) 
    reference: https://learnopengl.com/PBR/Theory
*/


float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


/*  Fresnel-Schlick equation (F)
    - describes the ratio of light that gets reflected over the light that gets refracted
    - which varies over the angle we're looking at a surface
    - tells us the percentage of light that gets reflected.
    - from ratio of reflection and the energy conservation principle => obtain refracted portion
    - the more we looking at surface's grazing angles =>  halfway-view angle reaching 90 degrees => stronger fresnel affect => stronger reflections
    parameters:
        cosTheta: dot product of N and H (normal and halfway/view)
        F0: base reflectivity of the material (IOR)
    equation:
        F(h, v, F0) = F0 + (1 - F0) * (1 - (h.v))^5
        F0  = base reflectivity of the material
            = indices of refraction / IOR
            can recompute it to work on both metallic/dielectric
            eg: vec3 F0 = vec3(0.04); //0.04 hold for most dielectrics
                     F0 = mix(F0, surfaceColor.rgb, metalness);
            metalness in range[0,1] where 1 pure metallic & the values in between may descibing dust etc..
            library: https://refractiveindex.info/
    reference: https://learnopengl.com/PBR/Theory


*/
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  


vec3 getLightPosition(Light light) {
    if(light.type == DIRECTIONAL_LIGHT) {
        return vec3(normalize(-light.position));
    } 
    return vec3(normalize(light.position - FragPos));
}

vec3 getLightRadiance(Light light) {
    if(light.type == DIRECTIONAL_LIGHT) {
        return light.color;
    } 
    vec3 lightDirection = normalize(light.position - FragPos);
    float distance    = length(lightDirection);
    float attenuation = 1.0 / (distance * distance * light.attenuationQuadratic + distance * light.attenuationLinear + light.attenuationConstant );

    if(light.type == POINT_LIGHT) {
        return light.color * attenuation; 
    }
    float theta = dotMaxZero(lightDirection, normalize(-light.direction));
    float t = clamp((theta - light.outerCutOff) / (light.cutOff - light.outerCutOff), 0.0, 1.0);
    
    return light.color * attenuation * (t*t*(3 - 2*t));

}


vec3 calcLight(vec3 albedo, float roughness, float metallic) {
    vec3 N =    vec3(0.0);
    if(material.enableNormalTexture) {
        // N = normalize(TBN * (texture(material.normalMap, TexCoords).rgb * 2.0 - 1.0));   
        N = getNormalFromMap();
    } else {
        N = normalize(Normal); 
    }
    vec3 V = normalize(cameraPos - FragPos);

    vec3 Lo = vec3(0.0);
    vec3 F0 = vec3(material.IOR); 
         F0 = mix(F0, albedo, metallic);

    for(int i = 0;i < num_lights;i++) {
        vec3 L = getLightPosition(lights[i]);
        vec3 H = normalize(V + L);
    
        vec3 radiance = getLightRadiance(lights[i]);
    

        // (F) fresnel
        vec3 F  = fresnelSchlick(dotMaxZero(H, V), F0);
        // (G) gemoertry
        float G   = GeometrySmith(N, V, L, roughness);       
        // (D) distribution function
        float D = DistributionGGX(N, H, roughness);    

        // ====== cook-torrence equation ========
        vec3 specular = (D * G * F) / (4.0 * dotMaxZero(N, V) + 0.001);   

        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;

        Lo += (kD * albedo / PI + specular) * radiance * dotMaxZero(N, L);
    }
    return Lo;
}

