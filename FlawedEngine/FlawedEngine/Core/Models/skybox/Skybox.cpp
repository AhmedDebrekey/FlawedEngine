#include "Skybox.h"
#include "../../Core.h"


namespace FlawedEngine
{
	cSkybox::cSkybox()
	{
	}
	cSkybox::~cSkybox()
	{
	}
	void cSkybox::Setup(void* GfxAPI)
	{
		mGfxAPI = (cGraphicsAPI*)GfxAPI;
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		std::vector<std::string> Faces
		{
			"Core\\Models\\skybox\\skybox\\Sky\\px.png",
			"Core\\Models\\skybox\\skybox\\Sky\\nx.png",
			"Core\\Models\\skybox\\skybox\\Sky\\py.png",
			"Core\\Models\\skybox\\skybox\\Sky\\ny.png",
			"Core\\Models\\skybox\\skybox\\Sky\\pz.png",
			"Core\\Models\\skybox\\skybox\\Sky\\nz.png"
		};
		mCubeMapTexture = mGfxAPI->CreateCubeMapTexture(Faces);
		mSkyBoxVAO = mGfxAPI->CreateVertexArray();
		mGfxAPI->BindVertexArray(mSkyBoxVAO);

		mSkyBoxVBO = mGfxAPI->CreateBuffer(eBufferType::Vertex, &skyboxVertices, sizeof(skyboxVertices), eDrawType::Static);
		mGfxAPI->VertexAttribProps(0, 3, eVertexType::Float, false, 3 * sizeof(float), (void*)0);

		mSkyboxShader.Create("Core\\Models\\Shaders\\SkyboxVertex.glsl", "Core\\Models\\Shaders\\SkyboxFragment.glsl");
		mSkyboxShader.Bind();
		mSkyboxShader.SetInt("skybox", 0);
		mSkyboxShader.Unbind();
		mGfxAPI->BindVertexArray(0);
	}

	void cSkybox::RenderSkyBox(const sTransform& camera)
	{
		if (!mShouldRenderSkyBox)
			return;

		mGfxAPI->SetDepthFunc(eGLFuncType::LessEqual);
		mSkyboxShader.Bind();
		glm::mat4 view = glm::mat4(glm::mat3(camera.View));
		mSkyboxShader.SetMat4f("view", view);
		mSkyboxShader.SetMat4f("projection", camera.Projection);
		mGfxAPI->BindVertexArray(mSkyBoxVAO);
		mGfxAPI->ActiveTexture(0);
		mGfxAPI->BindTexture(mCubeMapTexture, eTextureType::CubeMap);
		mGfxAPI->DrawArrays(36);
		mGfxAPI->BindTexture(0, eTextureType::CubeMap);
		mGfxAPI->BindVertexArray(0);
		mGfxAPI->SetDepthFunc(eGLFuncType::Less);

	}

	void cSkybox::ToggleSkyBox()
	{
		mShouldRenderSkyBox = !mShouldRenderSkyBox;
	}

	uint32_t cSkybox::GetSkyTexture()
	{
		return mCubeMapTexture;
	}
}