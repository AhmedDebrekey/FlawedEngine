#pragma once

#include "../Entity.h"
#include "../Models/ModelRenderer.h"
#include <unordered_map>

namespace FlawedEngine
{
	class cCube : public cEntity
	{
	public:
		cCube();
		~cCube();
		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox) override;
		virtual void Update() override;
		virtual void setDynamic(bool isDynamic) override;
		virtual void SetPhysics(eBasicObject Object, void* PhysicsWorld) override;
		virtual void SetAABB(glm::vec3& Scale) override;
		virtual void AddAnimation(const char*) override;

	private:
		cModelRenderer Renderer;
	};
}
