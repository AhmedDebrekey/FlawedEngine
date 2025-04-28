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

	void cMesh::Draw(sTransform& Trans, sMaterial& Mat, std::unordered_map<std::string, sLight>& Lights, uint32_t* SkyBox, cShader& GeometryShader, cShader& LightShader, std::vector<glm::mat4> FinalBoneMatricies, sGBufferObjects* GeometryObject)
	{        
        GeometryShader.Bind();
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

            GeometryShader.SetInt((name + number), i);
            mGfxAPI->ActiveTexture(i);
            mGfxAPI->BindTexture(mTextures[i].ID);
            //std::cout << "Name: " << (name + number) << " ActiveTexture0 + " << i  << " Texture ID: " << mTextures[i].ID << std::endl << std::endl;
        }
        

        GeometryShader.SetMat4f("Projection", Trans.Projection);
        GeometryShader.SetMat4f("View", Trans.View);
        GeometryShader.SetMat4f("Model", Trans.Model);

        GeometryShader.SetBool("hasTexture", true);

        if (!FinalBoneMatricies.empty())
        {
            mGfxAPI->BindBuffer(eBufferType::Uniform, AnimationUBO);
            mGfxAPI->BindBufferSubData(eBufferType::Uniform, 0, sizeof(glm::mat4) * MAX_BONES, &FinalBoneMatricies[0]);
            GeometryShader.SetBool("UBOSET", true);
        }
        else
        {
            GeometryShader.SetBool("UBOSET", false);
        }

        mGfxAPI->BindBufferBase(eBufferType::Uniform, 0, AnimationUBO);

        mGfxAPI->SetDepthFunc(eGLFuncType::Less);
        mGfxAPI->SetDepthTest(true);
        glDisable(GL_BLEND);

        mGfxAPI->DrawElements(mIndices.size());

        mGfxAPI->ActiveTexture(mTextures.size() + 1); //depth
        mGfxAPI->BindTexture(0);


        //mGfxAPI->BindTexture(0, eTextureType::CubeMap);
        for (unsigned int i = 0; i < mTextures.size(); i++)
        {
            mGfxAPI->ActiveTexture(i);
            mGfxAPI->BindTexture(0);
        }

        mGfxAPI->ActiveTexture(0);
        mGfxAPI->BindBufferBase(eBufferType::Uniform, 0, 0); // unbind the UBO after drawing the object
        mGfxAPI->BindBufferBase(eBufferType::Uniform, 1, 0); // unbind the UBO after drawing the object
        mGfxAPI->BindVertexArray(0);
        GeometryShader.Unbind();
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
            mGfxAPI->DeleteTexture(Texture.ID);
        }
    }

	void cMesh::setupMesh()
	{
        VAO = mGfxAPI->CreateVertexArray();
        mGfxAPI->BindVertexArray(VAO);

        VBO = mGfxAPI->CreateBuffer(eBufferType::Vertex, &mVertices[0], mVertices.size() * sizeof(sVertex), eDrawType::Static);
        EBO = mGfxAPI->CreateBuffer(eBufferType::Index, &mIndices[0], mIndices.size() * sizeof(unsigned int), eDrawType::Static);

        AnimationUBO = mGfxAPI->CreateBuffer(eBufferType::Uniform, nullptr, (sizeof(glm::mat4) * MAX_BONES), eDrawType::Dynmaic);
        mGfxAPI->BindBufferBase(eBufferType::Uniform, 0, AnimationUBO);

        DirectionalLightUBO = mGfxAPI->CreateBuffer(eBufferType::Uniform, nullptr, DirectionalLights.size() * sizeof(glm::vec4), eDrawType::Dynmaic);
        mGfxAPI->BindBufferBase(eBufferType::Uniform, 1, DirectionalLightUBO);

        // vertex Positions
        mGfxAPI->VertexAttribProps(0, 3, eVertexType::Float, false, sizeof(sVertex), (void*)0);
        // vertex normals
        mGfxAPI->VertexAttribProps(1, 3, eVertexType::Float, false, sizeof(sVertex), (void*)offsetof(sVertex, Normal));

        // vertex texture coords
        mGfxAPI->VertexAttribProps(2, 2, eVertexType::Float, false, sizeof(sVertex), (void*)offsetof(sVertex, TexCoords));
        // vertex tangent
        mGfxAPI->VertexAttribProps(3, 3, eVertexType::Float, false, sizeof(sVertex), (void*)offsetof(sVertex, Tangent));
        // vertex bitangent
        mGfxAPI->VertexAttribProps(4, 3, eVertexType::Float, false, sizeof(sVertex), (void*)offsetof(sVertex, BiTangent));
        // ids
        mGfxAPI->VertexAttribProps(5, 4, eVertexType::Int, false, sizeof(sVertex), (void*)offsetof(sVertex, mBoneIDs));

        // weights
        mGfxAPI->VertexAttribProps(6, 4, eVertexType::Float, false, sizeof(sVertex), (void*)offsetof(sVertex, mWeights));


        mGfxAPI->BindVertexArray(0);
    }
}
