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
        Shader.Create("Core/Models/Shaders/Vertex.glsl", "Core/Models/Shaders/Fragment.glsl", "{Flawed} Model Renderer");
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

        glGenBuffers(1, &DirectionalLightUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, DirectionalLightUBO);
        glBufferData(GL_UNIFORM_BUFFER, DirectionalLights.size() * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, DirectionalLightUBO);

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

    void cModelRenderer::ShadowDraw(sTransform& Trans, cShader& Shader, glm::mat4& LightSpaceMatrix, uint32_t DepthMap)
    {
        Shader.Bind();
        mLightSpaceMatrix = LightSpaceMatrix;
        mDepthMap = DepthMap;
        glBindVertexArray(VAO);

        Shader.SetMat4f("lightSpaceMatrix", LightSpaceMatrix);
        Shader.SetMat4f("model", Trans.Model);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        Shader.Unbind();
    }

	void cModelRenderer::Draw(sTransform& Trans, sMaterial& Mat, std::unordered_map<std::string, sLight>& Lights, uint32_t* SkyBox)
	{
    #ifdef OPENGL
        Shader.Bind();

        Shader.SetMat4f("Projection", Trans.Projection);
        Shader.SetMat4f("View", Trans.View);
        Shader.SetMat4f("Model", Trans.Model);

        Shader.SetBool("hasTexture", false);

        Shader.SetVec3("material.ambient", Mat.Color);
        Shader.SetVec3("material.diffuse", Mat.Diffuse);
        Shader.SetVec3("material.specular", Mat.Specular);
        Shader.SetFloat("material.shininess", Mat.Shininess);
        Shader.SetFloat("material.reflectivity", Mat.Reflectivity);


        glBindBuffer(GL_UNIFORM_BUFFER, DirectionalLightUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, DirectionalLights.size() * sizeof(glm::vec4), &DirectionalLights[0]);


        glBindBufferBase(GL_UNIFORM_BUFFER, 1, DirectionalLightUBO);
		glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, *SkyBox);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mDepthMap);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glBindVertexArray(0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, 0); // unbind the UBO after drawing the object

        Shader.Unbind();
    #endif 
	}
}