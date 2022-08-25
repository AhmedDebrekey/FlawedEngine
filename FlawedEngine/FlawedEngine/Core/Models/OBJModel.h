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
		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions) override;
		virtual void Update() override;
		virtual void setDynamic(bool isDynamic) override;
		virtual void SetPhysics(eBasicObject Object, void* PhysicsWorld) override;

		bool LoadModel(const char* FilePath);
		void Populate();
		void SetRigidBody(eBasicObject Object);
		bool isPhysicsSet = false;
	private:
		btDiscreteDynamicsWorld* mPhysicsDynamicWorld;
		btCollisionShape* mCollisionShape;
		cModelRenderer Renderer;
		objl::Loader mLoader;
	};
}
