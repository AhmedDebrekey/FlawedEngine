#pragma once

#include "../Core.h"
#include "../Entity.h"
#include "OBJModel.h"

namespace FlawedEngine
{
	class cObjectManager
	{
	public:
		cObjectManager();
		~cObjectManager();
		
		void Init(void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes);

		void RenderObjects(sTransform& tCamera);

		void AddObject(eBasicObject ObjectType, const char* Name);
		void RemoveObject(const char* Name);

		void ModifyObject(const char* Name, sModel& Model, sMaterial Material, bool setPhysics, sPhysicsProps PhysicsProps);
		
	private:
		void AddLight(const char* Name, sLight& Props);
		std::shared_ptr<cEntity> GetObjectByName(const char* Name);

	private:
		std::unordered_map<std::string, std::shared_ptr<cEntity>> SceneObjects;
		std::unordered_map<std::string, sLight> PointLights;

		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray = nullptr;

		sTransform tCamera;

		void* mPhysicsWorld = nullptr;
	};
}

