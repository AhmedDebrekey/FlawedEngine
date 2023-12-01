#include "Mesh.h"
#include <iostream>

namespace FlawedEngine
{
#define MAX_BONES 100

	cMesh::cMesh(std::vector<sVertex> vertices, std::vector<unsigned int> indices, std::vector<sTexture> textures, cGraphicsAPI* Graphics_API)
	{
		mVertices = vertices;
		mIndices  = indices;
		mTextures = textures;
        mGfxAPI = Graphics_API;
		setupMesh();
	}

	void cMesh::Draw(sTransform& Trans, sMaterial& Mat, std::unordered_map<std::string, sLight>& Lights, uint32_t* SkyBox, cShader& Shader, std::vector<glm::mat4> FinalBoneMatricies)
	{
        
        Shader.Bind();
        mGfxAPI->BindVertexArray(VAO);
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        for (unsigned int i = 0; i < mTextures.size(); i++)
        {
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string& name = mTextures[i].Type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);

            Shader.SetInt((name + number), i);
            mGfxAPI->ActiveTexture(i);
            mGfxAPI->BindTexture(mTextures[i].ID);
            //std::cout << "Name: " << (name + number) << " ActiveTexture0 + " << i  << " Texture ID: " << mTextures[i].ID << std::endl << std::endl;
        }
        
        //ModelRenderer
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

        //create a struct, populate that struct, then use that struct to populate the uniform buffer. Does not have to be a struct obv.

        mGfxAPI->BindBuffer(eBufferType::Uniform, DirectionalLightUBO);
        mGfxAPI->BindBufferSubData(eBufferType::Uniform, 0, DirectionalLights.size() * sizeof(glm::vec4), &DirectionalLights[0]);

        Shader.SetMat4f("Projection", Trans.Projection);
        Shader.SetMat4f("View", Trans.View);
        Shader.SetMat4f("Model", Trans.Model);
        Shader.SetVec3("viewPos", Trans.Position);
        
        Shader.SetMat4f("lightSpaceMatrix", mLightSpaceMatrix);

        Shader.SetVec3("material.ambient", Mat.Color);
        Shader.SetVec3("material.diffuse", Mat.Diffuse);
        Shader.SetVec3("material.specular", Mat.Specular);
        Shader.SetFloat("material.shininess", Mat.Shininess);
        Shader.SetFloat("material.reflectivity", Mat.Reflectivity);
       
        if (!FinalBoneMatricies.empty())
        {
            mGfxAPI->BindBuffer(eBufferType::Uniform, AnimationUBO);
            mGfxAPI->BindBufferSubData(eBufferType::Uniform, 0, sizeof(glm::mat4) * MAX_BONES, &FinalBoneMatricies[0]);
            Shader.SetBool("UBOSET", true);
        }
        else
        {
            Shader.SetBool("UBOSET", false);
        }

        mGfxAPI->BindBufferBase(eBufferType::Uniform, 0, AnimationUBO);
        mGfxAPI->BindBufferBase(eBufferType::Uniform, 1, DirectionalLightUBO);

        mGfxAPI->BindBuffer(eBufferType::Index, EBO);
        mGfxAPI->ActiveTexture(mTextures.size());

        mGfxAPI->BindTexture(*SkyBox, eTextureType::CubeMap);
        Shader.SetInt("skybox", mTextures.size());

        mGfxAPI->ActiveTexture(mTextures.size() + 1); //depth
        mGfxAPI->BindTexture(mDepthMap);
        Shader.SetInt("shadowMap", mTextures.size() + 1);

        mGfxAPI->DrawElements(mIndices.size());

        mGfxAPI->ActiveTexture(mTextures.size() + 1); //depth
        mGfxAPI->BindTexture(0);

        mGfxAPI->BindTexture(0, eTextureType::CubeMap);
        for (unsigned int i = 0; i < mTextures.size(); i++)
        {
            mGfxAPI->ActiveTexture(i);
            mGfxAPI->BindTexture(0);
        }

        mGfxAPI->ActiveTexture(0);
        mGfxAPI->BindBufferBase(eBufferType::Uniform, 0, 0); // unbind the UBO after drawing the object
        mGfxAPI->BindBufferBase(eBufferType::Uniform, 1, 0); // unbind the UBO after drawing the object
        mGfxAPI->BindVertexArray(0);
        Shader.Unbind();
    }

    void cMesh::ShadowDraw(sTransform& Trans, cShader& Shader, glm::mat4& LightSpaceMatrix, uint32_t DepthMap)
    {
        Shader.Bind();
        mLightSpaceMatrix = LightSpaceMatrix;
        mDepthMap = DepthMap;
        mGfxAPI->BindVertexArray(VAO);

        Shader.SetMat4f("lightSpaceMatrix", LightSpaceMatrix);
        Shader.SetMat4f("model", Trans.Model);

        mGfxAPI->BindBuffer(eBufferType::Index, EBO);
        mGfxAPI->DrawElements(mIndices.size());
        mGfxAPI->BindVertexArray(0);

        Shader.Unbind();
    }

    void cMesh::DeleteTextures()
    {
        for (auto Texture : mTextures)
        {
            glDeleteTextures(1, &Texture.ID);
        }
    }

	void cMesh::setupMesh()
	{
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(sVertex), &mVertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &AnimationUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, AnimationUBO);
        glBufferData(GL_UNIFORM_BUFFER, (sizeof(glm::mat4) * MAX_BONES), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, AnimationUBO);

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
	}
}
