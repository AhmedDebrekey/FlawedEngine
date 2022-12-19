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
	public:								//Have to copy it
		cOBJModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes);
		~cOBJModel();
		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions) override;
		virtual void Update() override;
		virtual void setDynamic(bool isDynamic) override;
		virtual void SetPhysics(eBasicObject Object, void* PhysicsWorld) override;
		virtual void UnSetPhysics() override;

		bool LoadModel(const char* FilePath);
		void Populate();
		void SetRigidBody(eBasicObject Object);
		void SetCollisionShape(eBasicObject Object);
		bool isPhysicsSet = false;
		std::string mName;
	private:
		btDiscreteDynamicsWorld* mPhysicsDynamicWorld;
		btCollisionShape* mCollisionShape;
		cModelRenderer Renderer;
		objl::Loader mLoader;
		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray;
	};
}
