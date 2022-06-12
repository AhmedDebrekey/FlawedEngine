#include "Triangle.h"

namespace FlawedEngine
{
	cTriangle::cTriangle()
	{
		mVertexBuffer.push_back({-0.5f, -0.5f, 0.0f});
		mVertexBuffer.push_back({ 0.5f, -0.5f, 0.0f });
		mVertexBuffer.push_back({ 0.0f,  0.5f, 0.0f });
		Renderer.Init(mVertexBuffer, mTextureCoords);
	}
	
	cTriangle::~cTriangle()
	{
		//Delete if used other variables than mVertexBuffer and Texture and the indexbuffer
	}

	void cTriangle::Render(Transform Trans) 
	{
		//Rendering of a triangle
		Renderer.Draw(Trans);
	}

	//tempForDebugPurposes
	void cTriangle::GetInfo()
	{
		auto pos = Renderer.mVertecies.at(0).Postion;
		std::cout << "Renderer Vertecies: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
		std::cout << "Renderer VAO: " << Renderer.VAO << std::endl;
	}
}