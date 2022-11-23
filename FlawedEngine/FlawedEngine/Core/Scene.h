#pragma once

#include <unordered_map>
#include <Bullet/btBulletDynamicsCommon.h>

#include "Entity.h"
#include "PerspectiveCamera.h"
#include "Models/ObjectManager.h"


namespace FlawedEngine
{
	class cScene
	{
	public:
		cScene(void* Window, void* Physics, btAlignedObjectArray<btCollisionShape*>* CollisionShapes);
		~cScene();
		void Setup();
		void Render();

		std::unordered_map<std::string, std::shared_ptr<cEntity>>* GetObjectsPointer() { return mSceneObjects; }

		cObjectManager* GetObjectManager() { return &ObjectMan; }

	private:
		cObjectManager ObjectMan;
		void* mWindow;
		void* PhysicsWorld = nullptr;
		cpCamera Camera;
		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray = nullptr;
		std::unordered_map<std::string, std::shared_ptr<cEntity>>* mSceneObjects;
	};
}