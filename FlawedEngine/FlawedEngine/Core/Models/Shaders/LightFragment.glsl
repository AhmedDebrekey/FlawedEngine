#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

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

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for(int i = 0; i < LightSize; ++i)
    {

        vec3 ambient = lighting * pointLights[i].ambient;
        // diffuse
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * pointLights[i].diffuse;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = pointLights[i].specular * spec * Specular;
        // attenuation
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (1.0 + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance);

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular + ambient;        
    }
    FragColor = vec4(lighting, 1.0);
}