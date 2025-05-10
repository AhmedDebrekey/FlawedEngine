#include "Triangle.h"
namespace FlawedEngine
{
	cTriangle::cTriangle()
	{
		mVertexBuffer.push_back({ glm::vec3(-0.5f, -0.5f, 0.0f) });
		mVertexBuffer.push_back({ glm::vec3(0.5f, -0.5f, 0.0f)  });
		mVertexBuffer.push_back({ glm::vec3(0.0f,  0.5f, 0.0f)  });

		mIndices = { 0,1,2 };

		Renderer.Init(mVertexBuffer, mTextureCoords, mIndices);
	}
	
	cTriangle::~cTriangle()
	{
		//Delete if used other variables than mVertexBuffer and Texture and the indexbuffer
	}

	void cTriangle::Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox, sGBufferObjects* GeometryObject)
	{
		//Rendering of a triangle
		Trans.Model = mModel;
		Renderer.Draw(Trans, mMaterial, LightPositions, SkyBox);
	}

	void cTriangle::ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap)
	{
	}
	void cTriangle::Update(float deltaTime)
	{
		//TODO: Should have an Input System here
	}
	void cTriangle::AddAnimation(const char*)
	{
	}
	void cTriangle::ChangeAnimation(const char*)
	{
	}
	void cTriangle::SetAABB(glm::vec3& Scale)
	{
	}
	bool cTriangle::isModelInFrustum()
	{
		return true;
	}
	void cTriangle::RemoveScripts()
	{
	}
}