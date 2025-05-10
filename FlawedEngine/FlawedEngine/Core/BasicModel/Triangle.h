#pragma once

#include "../Entity.h"
#include "../Models/ModelRenderer.h"
#include "../Shader.h"

#include <unordered_map>

namespace FlawedEngine
{
	class cTriangle : public cEntity
	{
	public:
		cTriangle();
		~cTriangle();
		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox, sGBufferObjects* GeometryObject) override;
		virtual void ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap) override;
		virtual void Update(float deltaTime) override;
		virtual void AddAnimation(const char*) override;
		virtual void ChangeAnimation(const char*) override;
		virtual void SetAABB(glm::vec3& Scale) override;
		virtual bool isModelInFrustum() override;
		virtual void RemoveScripts() override;

	private:
		cModelRenderer Renderer;
	};
}