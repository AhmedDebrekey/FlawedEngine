#include "Triangle.h"
namespace FlawedEngine
{
	cTriangle::cTriangle()
	{
		mVertexBuffer.push_back({ glm::vec3(-0.5f, -0.5f, 0.0f) });
		mVertexBuffer.push_back({ glm::vec3(0.5f, -0.5f, 0.0f)  });
		mVertexBuffer.push_back({ glm::vec3(0.0f,  0.5f, 0.0f)  });
		Renderer.Init(mVertexBuffer, mTextureCoords);
	}
	
	cTriangle::~cTriangle()
	{
		//Delete if used other variables than mVertexBuffer and Texture and the indexbuffer
	}

	void cTriangle::Render(Transform& Trans) 
	{
		//Rendering of a triangle
		glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 10.0f, 0.0f));
		Trans.Model = Model;
		Renderer.Draw(Trans);
	}

	void cTriangle::Update()
	{
		//TODO: Should have an Input System here
	}
}