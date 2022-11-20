#pragma once

#include "Entity.h"
#include "PerspectiveCamera.h"
#include <unordered_map>
#include <Bullet/btBulletDynamicsCommon.h>
#include "Physics/Physics.h"

namespace FlawedEngine
{
	class cScene
	{
	public:

		cScene(void* Window);
		~cScene();
		void Setup();
		void Render();
		void LoadModel(const char* FilePath, const char* Name, void* PhysicsWorld);
		void LoadModel(eBasicObject Object, const char* Name, void* PhysicsWorld);
		void AddLight(const char* Name, sLight& Props);

		std::shared_ptr<cEntity> GetObjectByName(const char* Name);

	private:
		std::unordered_map<std::string, std::shared_ptr<cEntity>> WorldEntities;
		std::unordered_map<std::string, sLight> PointLights;
		void* mWindow;
		cpCamera Camera;		

		std::shared_ptr<cPhysics> myPhysics = std::make_shared<cPhysics>(); //probably a bad idea . . .

	private: //Bullet Physics
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;

		btAlignedObjectArray<btCollisionShape*> collisionShapes;
	};
}