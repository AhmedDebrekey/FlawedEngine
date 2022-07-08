#include "ModelRenderer.h"

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

	void cModelRenderer::Draw(Transform& Trans, sMaterial& Mat, std::unordered_map<std::string, glm::vec3>& LightPositions)
	{
    #ifdef OPENGL
        Shader.Bind();
        for (auto& Light : LightPositions)
        {
            Shader.SetVec3("lightPos", Light.second);
        }
        Shader.SetMat4f("Projection", Trans.Projection);
        Shader.SetMat4f("View", Trans.View);
        Shader.SetMat4f("Model", Trans.Model);
        Shader.SetVec3("Color", Mat.Color);
		glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        Shader.Unbind();
    #endif 
	}
}