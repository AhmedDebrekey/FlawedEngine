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
        void DeleteTextures();
    private:
        //  render data
        unsigned int VAO, VBO, EBO, UBO;

        void setupMesh();
	};
}

