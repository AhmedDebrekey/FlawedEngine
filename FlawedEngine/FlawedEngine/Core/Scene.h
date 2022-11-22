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

		void Print() { std::cout << "Print" << std::endl; }

		cObjectManager ObjectMan;
	private:
		void* mWindow;
		void* PhysicsWorld = nullptr;
		cpCamera Camera;
		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray = nullptr;
	};
}