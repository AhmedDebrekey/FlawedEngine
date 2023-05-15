#version 450 core

layout(location = 0) out vec3 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_normal1;
uniform samplerCube skybox;
uniform sampler2D shadowMap;

out vec4 FragColor;

in vec3 ourColor;
in vec3 Normal;  
in vec2 TexCoords;
in vec3 FragPos;

in vec3 Position;
in mat3 TBN;

in vec4 FragPosLightSpace;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float reflectivity;
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
uniform vec3 viewPos;

layout(binding = 1) uniform DirectionalLight {
     vec4 Direction;
     vec4 Ambient;
     vec4 Diffuse;
     vec4 Specular;
} DirLight;

float alpha = 0.1;
float gamma = 2.2;
bool IsNoTexture = true;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(DirLight.Direction.xyz - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

void CalcLighting(vec3 Direction, vec3 normal, vec3 viewDir, inout float diff, inout float spec)
{
    diff = max(dot(normal, Direction), 0.0);
    vec3 reflectDir = reflect(-Direction, normal);
    spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
}

void CalcADS(inout vec3 ambient, inout vec3 diffuse, inout vec3 specular, float diff, float spec, vec4 LAmbient, vec4 LDiffuse, vec4 LSpecular)
{
    if(IsNoTexture)
    {
        ambient  = LAmbient.xyz  * vec3(material.ambient);
        diffuse  = LDiffuse.xyz  * diff * pow(vec3(material.diffuse), vec3(gamma));
        specular = LSpecular.xyz * spec * vec3(material.specular);
    }
    else
    { 
        if (texture(texture_diffuse1, TexCoords).a < alpha)
            discard;

        ambient  = LAmbient.xyz  * vec3(texture(texture_diffuse1, TexCoords));
        diffuse  = LDiffuse.xyz  * diff *  pow(texture(texture_diffuse1, TexCoords).rgb, vec3(gamma));        
        specular = LSpecular.xyz * spec * vec3(texture(texture_specular1, TexCoords));
    }
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = 0;
    float spec = 0;

    CalcLighting(lightDir, normal, viewDir, diff, spec);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance);    

    vec3 ambient  = vec3(0);
    vec3 diffuse  = vec3(0);
    vec3 specular = vec3(0);

    CalcADS(ambient, diffuse, specular, diff, spec, vec4(light.ambient, 1.0), vec4(light.diffuse, 1.0), vec4(light.specular, 1.0));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
} 

vec3 CalcDirLight(vec3 normal, vec3 viewDir)
{ 
    vec3 lightDir = normalize(DirLight.Direction.xyz);
    float diff = 0;
    float spec = 0;

    CalcLighting(lightDir, normal, viewDir, diff, spec);

    vec3 ambient  = vec3(0);
    vec3 diffuse  = vec3(0);
    vec3 specular = vec3(0);

    CalcADS(ambient, diffuse, specular, diff, spec, DirLight.Ambient, DirLight.Diffuse, DirLight.Specular);

    float shadow = ShadowCalculation(FragPosLightSpace, normal);                      
    vec3 color = vec3(1.0);

    if(!IsNoTexture)
    { 
        color = texture(texture_diffuse1, TexCoords).rgb;
    }

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
}


void main()
{
    vec3 DiffuseTex = vec3(texture(texture_diffuse1, TexCoords));
    IsNoTexture = (length(DiffuseTex) == 0.0); 

    vec3 norm = texture(texture_normal1, TexCoords).rgb;
    norm = norm * 2.0 - 1.0;   
    norm = normalize(TBN * norm);

    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0);

    result = CalcDirLight(norm, viewDir);

    for(int i = 0; i < LightSize; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    
    vec3 I = normalize(Position - viewPos);
    vec3 R = reflect(I, normalize(Normal));

    FragColor = vec4(mix(texture(skybox, R).rgb, (result), material.reflectivity), 1.0);
}