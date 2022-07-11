#version 330 core

out vec4 FragColor;  
in vec3 ourColor;
in vec3 Normal;  
in vec3 FragPos;

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
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(material.ambient);
    vec3 diffuse  = light.diffuse  * diff * vec3(material.diffuse);
    vec3 specular = light.specular * spec * vec3(material.specular);

    ambient  *= attenuation;
    diffuse  *= attenuation;
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

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(material.ambient);
    vec3 diffuse  = light.diffuse  * diff * vec3(material.diffuse);
    vec3 specular = light.specular * spec * vec3(material.specular) * 0.1;
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
    
    FragColor = vec4(result, 1.0);
}


/*
    Hello Future Debreky,
        This message is for you, because I know that you will PROBABLY forget everything that I did today.
        So let me simply explain to you.

        First Note that if we do not have the same number of pointlights in the scene as the "NR_POINT_LIGHTS", everything will be black

        Secondly, in the modelrenderer.cpp, a bunch of stuff have changed, now if you have a lot of point lights, it will be automatically sent
        over to the GPU

        IDK IF I AM MAKING SENSE, HOPEFULLY YOU WILL UNDERSTAND. AFTER ALL YOU ARE ME.

        My code does not support TEXTURES. not yet.

        It will

        In scene.cpp when creating a light, you have to have a sLight or light properties properly filled in order for everything to work.
*/