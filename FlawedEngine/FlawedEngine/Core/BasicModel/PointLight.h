#pragma once

#include "../Entity.h"
#include "../Models/ModelRenderer.h"

#include <unordered_map>

namespace FlawedEngine
{
	class cPointLight : public cEntity
	{
	public:
		cPointLight();
		~cPointLight();
		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox) override;
		virtual void ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap) override;
		virtual void Update() override;
		virtual void setDynamic(bool isDynamic) override;
		virtual void SetPhysics(eBasicObject Object, void* PhysicsWorld) override;
		virtual void UnSetPhysics() override;
		virtual void SetupScripting(const char*, std::function<bool(int)>&) override;
		virtual void SendEntity(cEntity* Entity) override;
		virtual void SendInputToScripting(std::function<bool(int)>) override;
		virtual void SetAABB(glm::vec3& Scale) override;
		virtual void AddAnimation(const char*) override;
		glm::vec3 mPosition;

	private:
		cModelRenderer Renderer;
		glm::vec3 mColor;
	};
}

