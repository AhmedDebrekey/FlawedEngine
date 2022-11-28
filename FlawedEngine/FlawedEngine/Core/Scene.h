#pragma once

#include <unordered_map>
#include <vector>
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

		cObjectManager* GetObjectManager() { return &ObjectMan; }
		uint32_t loadCubemap(std::vector<std::string> faces);
	private:
		cObjectManager ObjectMan;
		void* mWindow;
		void* PhysicsWorld = nullptr;
		cpCamera Camera;
		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray = nullptr;
		cShader SkyboxShader;
		unsigned int skyboxVAO, skyboxVBO;
		uint32_t CubeMapTexture;
	};
}