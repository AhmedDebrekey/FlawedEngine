#pragma once
#include "../Entity.h"
#include "../Models/ModelRenderer.h"
#include "OBJ_Loader.h"
#include <unordered_map>
#include "../Core.h"

namespace FlawedEngine
{
	class cOBJModel : public cEntity
	{
	public:
		cOBJModel(const char* FilePath, void* PhysicsWorld);
		~cOBJModel();
		virtual void Render(Transform& Trans, std::unordered_map<std::string, sLight>& LightPositions) override;
		virtual void Update() override;
		virtual void setDynamic(bool isDynamic) override;
		virtual void SetPhysics() override;

		bool LoadModel(const char* FilePath);
		void Populate();

		bool isPhysicsSet = false;
	private:
		btDiscreteDynamicsWorld* dynamicsWorld;
		btCollisionShape* groundShape;
		cModelRenderer Renderer;
		objl::Loader mLoader;
	};
}
