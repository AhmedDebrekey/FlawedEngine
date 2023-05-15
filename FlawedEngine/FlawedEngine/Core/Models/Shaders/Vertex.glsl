#version 450 core
layout (location = 0) in vec3 aPos;   
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 aBoneIds; 
layout (location = 6) in vec4 aWeights;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform bool UBOSET;
uniform mat4 lightSpaceMatrix;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout(std140) uniform BonesBlock {
    mat4 finalBonesMatrices[MAX_BONES];
};

out vec3 Normal;
out vec3 FragPos;
out vec3 Position;
out vec2 TexCoords;
out mat3 TBN;
out vec4 FragPosLightSpace;


void CalculatePosition(vec4 Position, vec3 Normals, bool Animation = false)
{
    gl_Position = Projection * View * Model * Position;
    Normal = normalize(mat3(transpose(inverse(Model))) * Normals);

    if(Animation)
    {
        vec4 worldPosition = Model * Position;
        FragPos = worldPosition.xyz;
    }
    else
        FragPos = vec3(Model * Position);
}

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(Model))); 
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(cross(N, T));
    TBN = mat3(T, B, N);

    if(UBOSET)
    {
        vec4 totalPosition  = vec4(0.0f);
        vec3 totalNormal    = vec3(0.0f);
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(aBoneIds[i] == -1) 
                continue;
            if(aBoneIds[i] >= MAX_BONES) 
            {
                totalPosition = vec4(aPos,1.0f);
                break;
            }
            vec4 localPosition = finalBonesMatrices[aBoneIds[i]] * vec4(aPos,1.0f);
            totalPosition += localPosition * aWeights[i];
             mat3 localRotation = mat3(finalBonesMatrices[aBoneIds[i]]);
            totalNormal += aWeights[i] * (localRotation * aNormal);
       }
           CalculatePosition(totalPosition, totalNormal, true);
    }
    else
        CalculatePosition(vec4(aPos,1.0f), aNormal, false);
    
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    Position = vec3(Model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
}     
