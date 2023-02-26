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

	void cTriangle::Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox)
	{
		//Rendering of a triangle
		Trans.Model = mModel;
		Renderer.Draw(Trans, mMaterial, LightPositions, SkyBox);
	}

	void cTriangle::Update()
	{
		//TODO: Should have an Input System here
	}
	void cTriangle::setDynamic(bool isDynamic)
	{
	}
	void cTriangle::SetPhysics(eBasicObject Object, void* PhysicsWorld)
	{
	}
	void cTriangle::UnSetPhysics()
	{
	}
	void cTriangle::SetupScripting(const char*)
	{
	}
	void cTriangle::SendEntity(cEntity* Entity)
	{
	}
	void cTriangle::SendInputToScripting(std::function<bool(int)>)
	{
	}
	void cTriangle::AddAnimation(const char*)
	{
	}
	void cTriangle::SetAABB(glm::vec3& Scale)
	{
	}
}