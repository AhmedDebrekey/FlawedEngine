#include "ModelRenderer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace FlawedEngine
{
    void cModelRenderer::Init(std::vector<sVertex>& Vertecies, std::vector<sTexture>& Textures, std::vector<uint32_t>& Indices)
    {
        mVertecies = Vertecies;
        mTextures = Textures;
        mIndices = Indices;

        Setup();
        Shader.Create("Core/Models/Shaders/Vertex.glsl", "Core/Models/Shaders/Fragment.glsl");
        Shader.Bind();
        Shader.Unbind();
    }

	void cModelRenderer::Setup()
	{
    #ifdef OPENGL
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mVertecies.size() * sizeof(sVertex), &mVertecies[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, BiTangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(sVertex), (void*)offsetof(sVertex, mBoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, mWeights));
        glBindVertexArray(0);
    #endif //OPENGL
	}

	void cModelRenderer::Draw(sTransform& Trans, sMaterial& Mat, std::unordered_map<std::string, sLight>& Lights, uint32_t* SkyBox)
	{
    #ifdef OPENGL
        Shader.Bind();
        int Iteration = 0;

        Shader.SetInt("LightSize", Lights.size());
        for (auto& Light : Lights)
        {
            auto CurrLight = Light.second;

            char buffer[64];

            sprintf_s(buffer, "pointLights[%i].position", Iteration);
            Shader.SetVec3(buffer, CurrLight.position);

            sprintf_s(buffer, "pointLights[%i].ambient", Iteration);
            Shader.SetVec3(buffer, CurrLight.ambient);

            sprintf_s(buffer, "pointLights[%i].diffuse", Iteration);
            Shader.SetVec3(buffer, CurrLight.diffuse);

            sprintf_s(buffer, "pointLights[%i].specular", Iteration);
            Shader.SetVec3(buffer, CurrLight.specular);

            sprintf_s(buffer, "pointLights[%i].constant", Iteration);
            Shader.SetFloat(buffer, CurrLight.constant);

            sprintf_s(buffer, "pointLights[%i].linear", Iteration);
            Shader.SetFloat(buffer, CurrLight.linear);
            
            sprintf_s(buffer, "pointLights[%i].quadratic", Iteration);
            Shader.SetFloat(buffer, CurrLight.quadratic);
            Iteration++;
        }

        Shader.SetVec3("dirLight.direction", glm::vec3(30.0f, -10.0f, 30.0f));
        Shader.SetVec3("dirLight.ambient", glm::vec3(0.9f));
        Shader.SetVec3("dirLight.diffuse", glm::vec3(0.9f));
        Shader.SetVec3("dirLight.specular", glm::vec3(0.9f));

        Shader.SetMat4f("Projection", Trans.Projection);
        Shader.SetMat4f("View", Trans.View);
        Shader.SetMat4f("Model", Trans.Model);
        Shader.SetVec3("viewPos", Trans.Position);
        Shader.SetVec3("material.ambient", Mat.Color);
        Shader.SetVec3("material.diffuse", Mat.Diffuse);
        Shader.SetVec3("material.specular", Mat.Specular);
        Shader.SetFloat("material.shininess", Mat.Shininess);

		glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *SkyBox);
		glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        Shader.Unbind();
    #endif 
	}
}