#include "Physics.h"
#include "../Entity.h"
#include <iostream>

namespace FlawedEngine
{
	cPhysics::cPhysics()
	{
	}

	cPhysics::~cPhysics()
	{
		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		for (int j = 0; j < collisionShapes.size(); j++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;
			delete shape;
		}

		delete dynamicsWorld;
		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfiguration;
		collisionShapes.clear();
	}

	void* cPhysics::Init()
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0.0, -9.81, 0));
		return dynamicsWorld;
	}

	void cPhysics::Update(float dt)
	{
		dynamicsWorld->stepSimulation(dt, 10);

		int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
			const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());

			cEntity* entityA = (cEntity*)obA->getCollisionShape()->getUserPointer();
			cEntity* entityB = (cEntity*)obB->getCollisionShape()->getUserPointer();
			entityA->CallOnCollision(entityB);
			entityB->CallOnCollision(entityA);
		}
	}
}