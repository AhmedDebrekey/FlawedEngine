#pragma once

#include "Entity.h"
#include "PerspectiveCamera.h"
#include <unordered_map>
#include <Bullet/btBulletDynamicsCommon.h>

namespace FlawedEngine
{
	class cScene
	{
	public:

		cScene(void* Window, void* Physics);
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
		void* PhysicsWorld = nullptr;
		cpCamera Camera;		
	};
}