#pragma once
#include "../Entity.h"
#include "../Models/ModelRenderer.h"
#include "OBJ_Loader.h"
#include <unordered_map>
#include "../Core.h"

#include <functional>

#include <ImGui/imgui.h>

namespace FlawedEngine
{
	class cOBJModel : public cEntity
	{
	public:								//Have to copy it
		cOBJModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes);
		~cOBJModel();
		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox, sGBufferObjects* GeometryObject) override;
		virtual void ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap) override;
		virtual void Update(float deltaTime) override;
		virtual void AddAnimation(const char*) override;
		virtual void ChangeAnimation(const char*) override;
		virtual void SetAABB(glm::vec3& Scale) override;
		virtual bool isModelInFrustum() override;

		bool LoadModel(const char* FilePath);
		void Populate();

	private:
		cModelRenderer Renderer;
		objl::Loader mLoader;
	private:
		cShader ShadowShader;
	};
}
