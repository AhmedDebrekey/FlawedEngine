#pragma once

#include <Bullet/btBulletDynamicsCommon.h>

namespace FlawedEngine {

	class cPhysics
	{
	public:
		cPhysics();
		~cPhysics();

		void* Init();

		void Update();

		btAlignedObjectArray<btCollisionShape*>* GetCollisionShapesArray() { return &collisionShapes; }

	private:
		btDefaultCollisionConfiguration*		collisionConfiguration	= nullptr;
		btCollisionDispatcher*					dispatcher				= nullptr;
		btBroadphaseInterface*					overlappingPairCache	= nullptr;
		btSequentialImpulseConstraintSolver*	solver					= nullptr;
		btDiscreteDynamicsWorld*				dynamicsWorld			= nullptr;
		btAlignedObjectArray<btCollisionShape*> collisionShapes;
	};

}

