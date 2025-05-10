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
		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox, sGBufferObjects* GeometryObject) override;
		virtual void ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap) override;
		virtual void Update(float deltaTime) override;
		virtual void SetAABB(glm::vec3& Scale) override;
		virtual void AddAnimation(const char*) override;
		virtual void ChangeAnimation(const char*) override;
		virtual bool isModelInFrustum() override;
		virtual void RemoveScripts() override;

		glm::vec3 mPosition;

	private:
		cModelRenderer Renderer;
		glm::vec3 mColor;
	};
}

