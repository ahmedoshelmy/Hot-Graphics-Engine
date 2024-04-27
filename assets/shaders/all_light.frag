#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 
uniform Material material;

// ===== direction light
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;


// ===== point light
struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 8  
uniform PointLight pointLights[NR_POINT_LIGHTS];


struct SpotLight {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};
uniform SpotLight spotLight;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 cameraPos;



vec3 calcDirectionLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);  
    // diffuse shadding
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient   * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse   * diff * texture(material.diffuse, TexCoords).rgb;  
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
        
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shadding
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    

    vec3 ambient = light.ambient   * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse   * diff * texture(material.diffuse, TexCoords).rgb;  
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    return  (ambient + diffuse + specular);
}


vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - FragPos);
    
    // diffuse shadding
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shadding
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    

    vec3 ambient = light.ambient   * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse   * diff * texture(material.diffuse, TexCoords).rgb;  
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;


    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branch
    diffuse   *= attenuation ;
    specular *= attenuation;   
        
    return  (ambient + diffuse + specular);
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 result = vec3(0.0);

    result += calcDirectionLight(dirLight, norm, viewDir);
    // for(int i = 0;i < NR_POINT_LIGHTS;i++) {
    //     result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
    // }
    result += calcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
   
} 