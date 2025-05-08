#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

uniform bool hasTexture;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float reflectivity;
}; 
uniform Material material;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

bool isTexture = true;

void main()
{    
    gPosition = FragPos;

    if(hasTexture)
    { 
        vec3 norm = texture(texture_normal1, TexCoords).rgb;
        norm = norm * 2.0 - 1.0;   
        norm = -normalize(TBN * norm);
        gNormal = norm;

        gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
        gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;

    }
    else
    {
        gAlbedoSpec.rgb = vec3(material.ambient); // or diffuse idk
        gAlbedoSpec.a = material.specular.r;
        gNormal = normalize(Normal);
    }
}