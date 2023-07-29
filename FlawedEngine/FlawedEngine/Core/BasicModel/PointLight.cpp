#include "PointLight.h"

namespace FlawedEngine
{
	cPointLight::cPointLight()
	{
		//https://pastebin.com/DXKEmvap
		mVertexBuffer.push_back({ glm::vec3(-1, -1, -1), glm::vec3(0, 0, 1), glm::vec2(0, 0) });
		mVertexBuffer.push_back({ glm::vec3(1, -1, -1), glm::vec3(1, 0, 0), glm::vec2(1, 0) });
		mVertexBuffer.push_back({ glm::vec3(1, 1, -1), glm::vec3(0, 0, -1), glm::vec2(1, 1) });
		mVertexBuffer.push_back({ glm::vec3(-1, 1, -1), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });
		mVertexBuffer.push_back({ glm::vec3(-1, -1, 1), glm::vec3(0, 1, 0), glm::vec2(0, 0) });
		mVertexBuffer.push_back({ glm::vec3(1, -1, 1), glm::vec3(0, -1, 0), glm::vec2(1, 0) });
		mVertexBuffer.push_back({ glm::vec3(1, 1, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0) });
		mVertexBuffer.push_back({ glm::vec3(-1, 1, 1), glm::vec3(1, 0, 0), glm::vec2(1, 1) });

		mIndices = {
			0, 1, 3, 3, 1, 2,
			1, 5, 2, 2, 5, 6,
			5, 4, 6, 6, 4, 7,
			4, 0, 7, 7, 0, 3,
			3, 2, 7, 7, 2, 6,
			4, 5, 0, 0, 5, 1
		};

		Renderer.Init(mVertexBuffer, mTextureCoords, mIndices);
	}

	cPointLight::~cPointLight()
	{
	}

	void cPointLight::Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox)
	{
		mColor = mMaterial.Color;
		Trans.Model = mModel;
		Renderer.Draw(Trans, mMaterial, LightPositions, SkyBox);
	}

	void cPointLight::ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap)
	{
	}

	void cPointLight::Update()
	{
	}
	void cPointLight::setDynamic(bool isDynamic)
	{
	}
	void cPointLight::SetPhysics(eBasicObject Object, void* PhysicsWorld)
	{
	}
	void cPointLight::UnSetPhysics()
	{
	}
	void cPointLight::SetupScripting(const char*, std::function<bool(int)>&)
	{
	}

	void cPointLight::SendEntity(cEntity* Entity)
	{
	}

	void cPointLight::SendInputToScripting(std::function<bool(int)>)
	{
	}

	void cPointLight::SetAABB(glm::vec3& Scale)
	{
	}

	void cPointLight::AddAnimation(const char*)
	{
	}
	
}