#version 330 core

layout(location = 0) out vec3 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_normal1;

out vec4 FragColor;  
in vec3 ourColor;
in vec3 Normal;  
in vec2 TexCoords;
in vec3 FragPos;

in vec3 Position;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

#define NR_POINT_LIGHTS 128
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int LightSize;


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // obtain normal from normal map in range [0,1]
    normal = texture(texture_normal1, TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results

    vec3 texDiffuseCol = vec3(texture(texture_diffuse1, TexCoords));
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
    if(length(texDiffuseCol) == 0.0)
    {
        ambient = light.ambient * vec3(material.ambient);
        diffuse = light.diffuse * diff * vec3(material.diffuse);
        specular = light.specular * spec * vec3(material.specular);
    }
    else
    { 
        ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
        diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));        
        specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords)) * 0.1;
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);

} 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

uniform samplerCube skybox;

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    // obtain normal from normal map in range [0,1]
    normal = texture(texture_normal1, TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  

    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 texDiffuseCol = vec3(texture(texture_diffuse1, TexCoords));
    // combine results
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
    if(length(texDiffuseCol) == 0.0)
    {
        ambient = light.ambient * vec3(material.ambient);
        diffuse = light.diffuse * diff * vec3(material.diffuse);
        specular = light.specular * spec * vec3(material.specular);
    }
    else
    { 
        ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
        diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));        
        specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords)) * 0.1;
    }

    return (ambient + diffuse + specular);
}

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0);
    result = CalcDirLight(dirLight, norm, viewDir);

    for(int i = 0; i < LightSize; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    
    vec3 I = normalize(Position - viewPos);
    vec3 R = reflect(I, normalize(Normal));

    FragColor = vec4(mix(texture(skybox, R).rgb, result, 0.9), 1.0);
    //FragColor = vec4(result, 1.0);
}