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
		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox, sGBufferObjects* GeometryObject) override;
		virtual void ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap) override;
		virtual void Update() override;
		virtual void SetAABB(glm::vec3& Scale) override;
		virtual void AddAnimation(const char*) override;

	private:
		cModelRenderer Renderer;
	};
}
