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

	void cTriangle::Render(Transform& Trans, std::unordered_map<std::string, sLight>& LightPositions)
	{
		//Rendering of a triangle
		Trans.Model = mModel;
		Renderer.Draw(Trans, mMaterial, LightPositions);
	}

	void cTriangle::Update()
	{
		//TODO: Should have an Input System here
	}
	void cTriangle::setDynamic(bool isDynamic)
	{
	}
	void cTriangle::SetPhysics(eBasicObject Object)
	{
	}
}