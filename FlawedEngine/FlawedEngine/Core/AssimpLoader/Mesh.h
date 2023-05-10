#pragma once
#include "../Core.h"
#include "../Shader.h"
#include <unordered_map>

namespace FlawedEngine
{

	class cMesh
	{
    public:
        // mesh data
        std::vector<sVertex> mVertices;
        std::vector<unsigned int> mIndices;
        std::vector<sTexture> mTextures;

        cMesh(std::vector<sVertex> vertices, std::vector<unsigned int> indices, std::vector<sTexture> textures);
        void Draw(sTransform& Trans, sMaterial& Mat, std::unordered_map<std::string, sLight>& Lights, uint32_t* SkyBox, cShader& Shader, std::vector<glm::mat4> FinalBoneMatricies);
        void ShadowDraw(sTransform& Trans, cShader& Shader, glm::mat4& LightSpaceMatrix, uint32_t DepthMap);
        void DeleteTextures();
    private:
        //  render data
        unsigned int VAO, VBO, EBO, AnimationUBO, DirectionalLightUBO;
        // they are vec4 not vec3 because ubo restirictions
        std::vector<glm::vec4> DirectionalLights
        {
            glm::vec4(GetDirectionalLightPos(), 1.0f),    // Direction
            glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),        // Ambient
            glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),        // Diffuse
            glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)         // Specular
        };

        void setupMesh();
    private:
        glm::mat4 mLightSpaceMatrix;
        uint32_t mDepthMap;
	};
}