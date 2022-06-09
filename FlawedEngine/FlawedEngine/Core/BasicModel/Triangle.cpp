#include "Triangle.h"

namespace FlawedEngine
{
	cTriangle::cTriangle()
	{
		mVertexBuffer.push_back({-0.5f, -0.5f, 0.0f});
		mVertexBuffer.push_back({ 0.5f, -0.5f, 0.0f });
		mVertexBuffer.push_back({ 0.0f,  0.5f, 0.0f });
	}
	
	cTriangle::~cTriangle()
	{
		//Delete if used other variables than mVertexBuffer and Texture and the indexbuffer
	}

	void cTriangle::Render() 
	{
		//Rendering of a triangle
		Renderer.Draw();
	}
}