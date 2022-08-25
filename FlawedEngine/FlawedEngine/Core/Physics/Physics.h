#pragma once

#include <Bullet/btBulletDynamicsCommon.h>
#include "../Core.h"

#include <memory>
namespace FlawedEngine
{
	class cPhysics
	{
	public:
		cPhysics();
		~cPhysics();

		void Setup();
		void Update();

		std::shared_ptr<btRigidBody> AddBox(eBasicObject Object, sPhysicsProps& Props);
		std::shared_ptr<btRigidBody> AddSphere(eBasicObject Object, sPhysicsProps& Props);
	private:
		btDefaultCollisionConfiguration*		mCollisionConfiguration	= nullptr;
		btCollisionDispatcher*					mDispatcher				= nullptr;
		btBroadphaseInterface*					mOverlappingPairCache	= nullptr;
		btSequentialImpulseConstraintSolver*	mSolver					= nullptr;
		btDiscreteDynamicsWorld*				mWorld					= nullptr; //I'm cool hehe 8)

		btAlignedObjectArray<btCollisionShape*> collisionShapes;

		bool isPlaying = false;

		std::shared_ptr<btCollisionShape> SetCollisionShape(eBasicObject Object);
	};
}

